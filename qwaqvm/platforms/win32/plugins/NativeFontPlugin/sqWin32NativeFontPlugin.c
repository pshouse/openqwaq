/**
 * Project OpenQwaq
 *
 * Copyright (c) 2005-2011, Teleplace, Inc., All Rights Reserved
 *
 * Redistributions in source code form must reproduce the above
 * copyright and this condition.
 *
 * The contents of this file are subject to the GNU General Public
 * License, Version 2 (the "License"); you may not use this file
 * except in compliance with the License. A copy of the License is
 * available at http://www.opensource.org/licenses/gpl-2.0.php.
 *
 */

#include <windows.h>
#include "sqVirtualMachine.h"
#include "sqConfig.h"
#include "sqPlatformSpecific.h"

extern struct VirtualMachine *interpreterProxy;

/* Ref to Squeak window for getting DC etc. */
static HWND *theSTWindow;

/* Number of elements in font cache to grow by */
#define FONT_CACHE_SIZE 50

#define FontFlagBold      1
#define FontFlagItalic    2
#define FontFlagUnderline 4
#define FontFlagStrikeout 8

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
typedef struct sqFont {
	HFONT handle;
	TEXTMETRIC textMetric;
	ABC abcWidths[256];
	int nKernPairs;
	KERNINGPAIR *kernPairs;
} sqFont;
/* font cache */
static sqFont *fontCache = NULL;
static int maxFonts = 0;

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/* font creation */

int ioCreateFont(int fontNameIndex, int fontNameLength, int pixelSize, int flags, int quality)
{
	LOGFONT logFont;
	HFONT hFont;
	int fontIndex, i, nKernPairs;
	HDC hDC = NULL;
	static int logPixelsY = 0;
	#define NumQualityOptions 4
	static int qualityOpts[NumQualityOptions] = {
	        DEFAULT_QUALITY,
	        PROOF_QUALITY,
		ANTIALIASED_QUALITY,
		5,  /* CLEARTYPE_QUALITY */
	};

	if(fontNameLength >= LF_FACESIZE) return -1;
	if(quality >= NumQualityOptions) return -1;
	if(!logPixelsY) {
	        hDC = GetDC(*theSTWindow);
	        logPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
	        ReleaseDC(*theSTWindow, hDC);
		hDC = NULL;
	}
	logFont.lfHeight = -MulDiv(pixelSize, logPixelsY, 72);
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = (flags & FontFlagBold) ? FW_BOLD : FW_REGULAR;
	logFont.lfItalic = (flags & FontFlagItalic) != 0;
	logFont.lfUnderline = (flags & FontFlagUnderline) != 0;
	logFont.lfStrikeOut = (flags & FontFlagStrikeout) != 0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfOutPrecision = OUT_TT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = qualityOpts[quality];
	logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	for(i=0;i < fontNameLength; i++) {
		logFont.lfFaceName[i] = ((char*)fontNameIndex)[i];
	}
	logFont.lfFaceName[fontNameLength] = 0;
	hFont = CreateFontIndirect(&logFont);
	if(!hFont) {
		MessageBox(*theSTWindow, "Failed to create font", "Font error", MB_OK);
		return -1;
	}
	/* Find free slot in fontCache */
	for(fontIndex = 0; fontIndex < maxFonts; fontIndex++)
		if(fontCache[fontIndex].handle == NULL) break;
	if(fontIndex == maxFonts) {
		int i;
		maxFonts = maxFonts + FONT_CACHE_SIZE; /* grow linearly */
		fontCache = (sqFont*) realloc(fontCache, maxFonts * sizeof(sqFont));
		for(i = fontIndex; i < maxFonts; i++) fontCache[i].handle = NULL;
	}
	fontCache[fontIndex].handle = hFont;
	/* Query for the width of each character */
	{
		HANDLE oldFont;
		hDC = GetDC(*theSTWindow);
		oldFont = SelectObject(hDC, hFont);
		if(!GetCharABCWidths(hDC, 0, 255, fontCache[fontIndex].abcWidths)) {
			/* This must have been a bitmap font */
			int widthArray[256];
			ABC *abc = fontCache[fontIndex].abcWidths;
			int i;
			GetCharWidth(hDC, 0, 255, widthArray);
			for(i=0; i < 256; i++) {
				abc[i].abcA = 0;
				abc[i].abcB = widthArray[i];
				abc[i].abcC = 0;
			}
		}
		GetTextMetrics(hDC, &(fontCache[fontIndex].textMetric));
		nKernPairs = GetKerningPairs(hDC, 0, NULL);
		fontCache[fontIndex].nKernPairs = nKernPairs;
		fontCache[fontIndex].kernPairs = (KERNINGPAIR*) calloc(nKernPairs, sizeof(KERNINGPAIR));
		GetKerningPairs(hDC, nKernPairs, fontCache[fontIndex].kernPairs);
		SelectObject(hDC, oldFont);
		ReleaseDC(*theSTWindow, hDC);
	}
	return fontIndex;
}

int ioDestroyFont(int fontIndex) {
	if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
	if(fontCache[fontIndex].handle == NULL) return 0;
	DeleteObject(fontCache[fontIndex].handle);
	free(fontCache[fontIndex].kernPairs);
	fontCache[fontIndex].kernPairs = NULL;
	fontCache[fontIndex].handle = 0;
	return 1;
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/* Font information */
int ioFontEncoding(int fontIndex) {
	/* Font encoding is always ANSI - we don't allow anything else */
	return 1;
}

int ioFontAscent(int fontIndex) {
	TEXTMETRIC *tm;
	if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
	if(fontCache[fontIndex].handle == NULL) return 0;
	tm = &(fontCache[fontIndex].textMetric);
	return tm->tmAscent /* + tm->tmInternalLeading */;
}

int ioFontDescent(int fontIndex) {
	TEXTMETRIC *tm;
	if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
	if(fontCache[fontIndex].handle == NULL) return 0;
	tm = &(fontCache[fontIndex].textMetric);
	return tm->tmDescent /* + tm->tmExternalLeading */;
}

int ioFontEmbeddingFlags(int fontIndex) {
  HDC hDC;
  HANDLE hFont;
  DWORD size;
  OUTLINETEXTMETRIC otm;
  if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
  if(fontCache[fontIndex].handle == NULL) return 0;
  hDC = GetDC(*theSTWindow);
  hFont = SelectObject(hDC, fontCache[fontIndex].handle);
  otm.otmSize = sizeof(otm);
  if(GetOutlineTextMetrics(hDC, sizeof(otm), &otm) == 0) {
    /* no embedding if function fails */
    otm.otmfsType = 1;
  }
  SelectObject(hDC, hFont);
  ReleaseDC(*theSTWindow, hDC);
  return otm.otmfsType;
}

int ioGetFontDataSize(int fontIndex) {
  HDC hDC;
  HANDLE hFont;
  DWORD size;
  if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
  if(fontCache[fontIndex].handle == NULL) return 0;
  hDC = GetDC(*theSTWindow);
  hFont = SelectObject(hDC, fontCache[fontIndex].handle);
  size = GetFontData(hDC, 0, 0, NULL, -1);
  SelectObject(hDC, hFont);
  ReleaseDC(*theSTWindow, hDC);
  return size;
}

int ioGetFontData(int fontIndex, char *fontDataBuffer, int bufferSize) {
  HDC hDC;
  HANDLE hFont;
  DWORD size;
  if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
  if(fontCache[fontIndex].handle == NULL) return 0;
  hDC = GetDC(*theSTWindow);
  hFont = SelectObject(hDC, fontCache[fontIndex].handle);
  size = GetFontData(hDC, 0, 0, fontDataBuffer, bufferSize);
  SelectObject(hDC, hFont);
  ReleaseDC(*theSTWindow, hDC);
  return size;
}


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/* character measuring */
int ioFontWidthOfChar(int fontIndex, int characterIndex)
{	ABC abcWidth;
	int width;

	if(fontIndex < 0 || fontIndex >= maxFonts) return -1;
	if(fontCache[fontIndex].handle == NULL) return -1;
	if(characterIndex < 0 || characterIndex > 255) return -1;
	return fontCache[fontIndex].abcWidths[characterIndex].abcB;
}

int ioFontFullWidthOfChar(int fontIndex, int characterIndex, int fullWidth[3])
{
	ABC abcWidth;
	if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
	if(fontCache[fontIndex].handle == NULL) return 0;
	if(characterIndex < 0 || characterIndex > 255) return 0;

	abcWidth = fontCache[fontIndex].abcWidths[characterIndex];
	fullWidth[0] = abcWidth.abcA;
	fullWidth[1] = abcWidth.abcB;
	fullWidth[2] = abcWidth.abcC;
	return 1;
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/* Kerning support */
int ioFontNumKernPairs(int fontIndex)
{
	if(fontIndex < 0 || fontIndex >= maxFonts) return -1;
	if(fontCache[fontIndex].handle == NULL) return -1;
	return fontCache[fontIndex].nKernPairs;
}

int ioFontGetKernPair(int fontIndex, int kernIndex, int kernPair[3])
{
	KERNINGPAIR kp;
	if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
	if(fontCache[fontIndex].handle == NULL) return 0;
	if(kernIndex < 1 || kernIndex > fontCache[fontIndex].nKernPairs) return 0;

	kp = fontCache[fontIndex].kernPairs[kernIndex-1];
	kernPair[0] = kp.wFirst;
	kernPair[1] = kp.wSecond;
	kernPair[2] = kp.iKernAmount;
	return 1;
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/* Glyph retrieval */
int ioFontGlyphOfChar(int fontIndex, int characterIndex, 
					  int formBitsIndex, int formWidth, int formHeight, int formDepth)
{
	HDC hDC;
	HDC mDC;
	HBITMAP hBM;
	HANDLE oldBM, oldFont;
	unsigned int *dibBits;
	static BITMAPINFO *bmi = NULL;

	char string[1];
	int xPos;

	if((formDepth != 1) && (formDepth != 32)) return 0;  /* unsupported */
	if(fontIndex < 0 || fontIndex >= maxFonts) return 0;
	if(fontCache[fontIndex].handle == NULL) return 0;
	if(characterIndex < 0 || characterIndex > 255) return 0;
	
	if(!bmi) {
		bmi = (BITMAPINFO*) calloc(1, sizeof(BITMAPINFO) + 2 * sizeof(RGBQUAD));
		bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	}
	bmi->bmiHeader.biWidth = formWidth;
	bmi->bmiHeader.biHeight = -formHeight;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = formDepth;
	bmi->bmiHeader.biCompression = BI_RGB;
	bmi->bmiColors[0].rgbRed = 255;
	bmi->bmiColors[0].rgbGreen = 255;
	bmi->bmiColors[0].rgbBlue = 255;
	bmi->bmiColors[1].rgbRed = 0;
	bmi->bmiColors[1].rgbGreen = 0;
	bmi->bmiColors[1].rgbBlue = 0;

	string[0] = characterIndex;

	/* Draw the character glyph */
	hDC = GetDC(*theSTWindow);
	mDC = CreateCompatibleDC(hDC);
	hBM = CreateDIBSection(mDC, bmi, DIB_RGB_COLORS, (void*)&dibBits, NULL, 0);
	oldBM = SelectObject(mDC, hBM);
	oldFont = SelectObject(mDC, fontCache[fontIndex].handle);
	PatBlt(mDC, 0, 0, formWidth, formHeight, BLACKNESS);
	SetTextAlign(mDC, TA_TOP | TA_LEFT | TA_NOUPDATECP);
	SetTextColor(mDC, RGB(0, 0, 0));
	/* Adjust for underhang, but don't trim insets. */
	/* Trimming insets will clip underlines and strikeouts. */
	xPos = -(fontCache[fontIndex].abcWidths[characterIndex].abcA);
	if(xPos < 0) xPos = 0;
	TextOut(mDC, xPos, 0, string, 1);

	/* BitBlt(hDC, (characterIndex & 15) * formWidth, (characterIndex >> 4) * formHeight, formWidth, formHeight, mDC, 0, 0, SRCCOPY); */

	/* And swap those bits */
	{
		unsigned int *bits = (unsigned int*) formBitsIndex;
		int numWords = ((formWidth * formDepth) + 31) / 32;
		int i;

#		define BYTE_SWAP(w) ((w << 24) | ((w & 0xFF00) << 8) | ((w >> 8) & 0xFF00) | (w >> 24))
		for(i=0; i < formHeight*numWords; i++) {
			if(formDepth == 1) bits[i] = BYTE_SWAP(dibBits[i]);
			if(formDepth == 32) bits[i] = dibBits[i];
		}
#		undef BYTE_SWAP
	}
	/* done it. */
	SelectObject(mDC, oldBM);
	SelectObject(mDC, oldFont);
	DeleteDC(mDC);
	DeleteObject(hBM);
	ReleaseDC(*theSTWindow, hDC);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/* Font name cache */

static LOGFONT *fontNameCache = NULL;
static int numFontNames = 0;
static int maxFontNames = 0;

int CALLBACK enumFontsCallback(ENUMLOGFONTEX *logFont, NEWTEXTMETRICEX *textMetric, int fontType, LPARAM param)
{
	int i;
	if(numFontNames == maxFontNames) {
		/* Resize font cache */
		maxFontNames = maxFontNames + FONT_CACHE_SIZE; /* grow linearly */
		fontNameCache = (LOGFONT*) realloc(fontNameCache, maxFontNames * sizeof(LOGFONT));
	}
	/* We check for unique names here since fonts will be listed in all available char sets */
	for(i = 0; i < numFontNames; i++) {
		if(strcmp(fontNameCache[i].lfFaceName, logFont->elfLogFont.lfFaceName) == 0) {
			/* we had this guy already */
			return 1; /* but continue enumerating */
		}
	}
	fontNameCache[numFontNames++] = logFont->elfLogFont;
	return 1; /* continue enumeration */
}

char *ioListFont(int fontIndex) {
	static char taggedName[256];
	/* Enumerate fonts on index 0 or first use */
	if(fontIndex == 0 || !fontNameCache) {
		LOGFONT logFont;
		HDC hDC;
		numFontNames = 0; /* start over with enumeration */
		hDC = GetDC(*theSTWindow);
		if(!hDC) return NULL;
		ZeroMemory(&logFont, sizeof(logFont));
		logFont.lfCharSet = DEFAULT_CHARSET;
		logFont.lfFaceName;
		EnumFontFamiliesEx(hDC, &logFont, enumFontsCallback, 0, 0);
		ReleaseDC(*theSTWindow, hDC);
	}
	if(!fontNameCache) return NULL;
	if(fontIndex >= numFontNames) return NULL;
	return fontNameCache[fontIndex].lfFaceName;
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/* initialize/shutdown */

int ioFontInit(void) {
	/* lookup the necessary things from interpreter */
	theSTWindow = (HWND*) interpreterProxy->ioLoadFunctionFrom("stWindow","");
	if(!theSTWindow) return 0;
	numFontNames = maxFontNames = 0;
	fontNameCache = NULL;
	maxFonts = 0;
	fontCache = NULL;
	return 1;
}

int ioFontShutdown(void) {
	int i;
	if(fontNameCache) free(fontNameCache);
	for(i=0; i < maxFonts; i++)
		if(fontCache[i].handle) DeleteObject(fontCache[i].handle);
	if(fontCache) free(fontCache);
	return 1;
}

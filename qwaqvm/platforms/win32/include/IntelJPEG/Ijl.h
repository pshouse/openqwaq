//  INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or 
//  nondisclosure agreement with Intel Corporation and may not be copied 
//  or disclosed except in accordance with the terms of that agreement. 
//
//     Copyright 1999 - 2000, Intel Corporation. All Rights Reserved. 
/* ijl.h
*/

#ifndef _IJL_H_
#define _IJL_H_

/*M*
////////////////////////////////////////////////////////////////////////////
//
// Name:		ijl.h
//
// Description:	This file contains:  definitions for data types, data
//				structures, error codes, and function prototypes used
//				in the Intel(R) JPEG Library (IJL).
//
// Version:		1.300
//
////////////////////////////////////////////////////////////////////////////
*M*/


#ifndef _IJL_API_
#define IJLAPI __declspec(dllimport)
#else
#define IJLAPI __declspec(dllexport)
#endif


#ifdef _MSC_VER
#pragma pack (8)
#endif


#define _X86_
#include <stdio.h>
#include <windef.h>


////////////////////////////////////////////////////////////////////////////
// Macros/Constants
#define IJL_NONE	0
#define IJL_OTHER	255
#define JBUFSIZE	4096	// Size of file I/O buffer (4K).


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		IJLIOTYPE
//
// Purpose:		Possible types of data read/write/other operations to be
//				performed by the functions IJL_Read and IJL_Write.
//
//				See the Developer's Guide for details on appropriate usage.
//
// Fields:
//
//	IJL_JFILE_XXXXXXX	Indicates JPEG data in a stdio file.
//
//	IJL_JBUFF_XXXXXXX	Indicates JPEG data in an addressable buffer.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef enum 
{
	IJL_SETUP					= -1,


	// Read JPEG parameters (i.e., height, width, channels,
	// sampling, etc.) from a JPEG bit stream.
	IJL_JFILE_READPARAMS		=  0,
	IJL_JBUFF_READPARAMS		=  1,

	// Read a JPEG Interchange Format image.
	IJL_JFILE_READWHOLEIMAGE	=  2,
	IJL_JBUFF_READWHOLEIMAGE	=  3,

	// Read JPEG tables from a JPEG Abbreviated Format bit stream.
	IJL_JFILE_READHEADER		=  4,
	IJL_JBUFF_READHEADER		=  5,

	// Read image info from a JPEG Abbreviated Format bit stream.
	IJL_JFILE_READENTROPY		=  6,
	IJL_JBUFF_READENTROPY		=  7,

	// Write an entire JFIF bit stream.
	IJL_JFILE_WRITEWHOLEIMAGE	=  8,
	IJL_JBUFF_WRITEWHOLEIMAGE	=  9,

	// Write a JPEG Abbreviated Format bit stream.
	IJL_JFILE_WRITEHEADER		= 10,
	IJL_JBUFF_WRITEHEADER		= 11,

	// Write image info to a JPEG Abbreviated Format bit stream.
	IJL_JFILE_WRITEENTROPY		= 12,
	IJL_JBUFF_WRITEENTROPY		= 13,


	// Scaled Decoding Options:

	// Reads a JPEG image scaled to 1/2 size.
	IJL_JFILE_READONEHALF		= 14,
	IJL_JBUFF_READONEHALF		= 15,

	// Reads a JPEG image scaled to 1/4 size.
	IJL_JFILE_READONEQUARTER	= 16,
	IJL_JBUFF_READONEQUARTER	= 17,

	// Reads a JPEG image scaled to 1/8 size.
	IJL_JFILE_READONEEIGHTH		= 18,
	IJL_JBUFF_READONEEIGHTH		= 19,

	// Reads an embedded thumbnail from a JFIF bit stream.
	IJL_JFILE_READTHUMBNAIL		= 20,
	IJL_JBUFF_READTHUMBNAIL		= 21

} IJLIOTYPE;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		IJL_COLOR
//
// Purpose:		Possible color space formats.
//
//				Note these formats do *not* necessarily denote
//				the number of channels in the color space.
//				There exists separate "channel" fields in the
//				JPEG_CORE_PROPERTIES data structure specifically
//				for indicating the number of channels in the
//				JPEG and/or DIB color spaces.
//
//				See the Developer's Guide for details on appropriate usage.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef enum
{
	IJL_RGB			= 1,	// Red-Green-Blue color space.
	IJL_BGR			= 2,	// Reversed channel ordering from IJL_RGB.
	IJL_YCBCR		= 3,	// Luminance-Chrominance color space as defined
							// by CCIR Recommendation 601.
	IJL_G			= 4,	// Grayscale color space.
	IJL_RGBA_FPX	= 5,	// FlashPix RGB 4 channel color space that
							// has pre-multiplied opacity.
	IJL_YCBCRA_FPX	= 6,	// FlashPix YCbCr 4 channel color space that
							// has pre-multiplied opacity.

//	IJL_OTHER				// Some other color space not defined by the IJL.
							// (This means no color space conversion will
							//  be done by the IJL.)
	
} IJL_COLOR;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		IJL_JPGSUBSAMPLING
//
// Purpose:		Possible subsampling formats used in the JPEG.
//
//				See the Developer's Guide for details on appropriate usage.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef enum
{
	IJL_411		= 1,	// Valid on a JPEG w/ 3 channels.
	IJL_422		= 2,	// Valid on a JPEG w/ 3 channels.

	IJL_4114	= 3,	// Valid on a JPEG w/ 4 channels.
	IJL_4224	= 4		// Valid on a JPEG w/ 4 channels.

//	IJL_NONE	= Corresponds to "No Subsampling".
//				  Valid on a JPEG w/ any number of channels.

//	IJL_OTHER	= Valid entry, but only used internally to the IJL.

} IJL_JPGSUBSAMPLING;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		IJL_DIBSUBSAMPLING
//
// Purpose:		Possible subsampling formats used in the DIB.
//
//				See the Developer's Guide for details on appropriate usage.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef enum
{
//	IJL_NONE	= Corresponds to "No Subsampling".

} IJL_DIBSUBSAMPLING;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		HUFFMAN_TABLE
//
// Purpose:		Stores Huffman table information in a fast-to-use format.
//
// Context:		Used by Huffman encoder/decoder to access Huffman table
//				data.  Raw Huffman tables are formatted to fit this
//				structure prior to use.
//
// Fields:
//	huff_class	0 == DC Huffman or lossless table, 1 == AC table.
//	ident		Huffman table identifier, 0-3 valid (Extended Baseline).
//	huffelem	Huffman elements for codes <= 8 bits long;
//				contains both zero run-length and symbol length in bits.
//	huffval		Huffman values for codes 9-16 bits in length.
//	mincode		Smallest Huffman code of length n.
//	maxcode		Largest Huffman code of length n.
//	valptr		Starting index into huffval[] for symbols of length k.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct _HUFFMAN_TABLE
{
	int huff_class;
	int ident;
	unsigned int huffelem[256];
	unsigned short huffval[256];
	unsigned short mincode[17];
	unsigned short maxcode[18];
	unsigned short valptr[17];

} HUFFMAN_TABLE;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		JPEGHuffTable
//
// Purpose:		Stores pointers to JPEG-binary spec compliant
//				Huffman table information.
//
// Context:		Used by interface and table methods to specify encoder 
//				tables to generate and store JPEG images.
//
// Fields:
//	bits		Points to number of codes of length i (<=16 supported).
//	vals		Value associated with each Huffman code.
//	hclass		0 == DC table, 1 == AC table.
//	ident		Specifies the identifier for this table.
//				0-3 for extended JPEG compliance.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct
{
	unsigned char *bits;
	unsigned char *vals;
	unsigned char hclass;
	unsigned char ident;

} JPEGHuffTable;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		QUANT_TABLE
//
// Purpose:		Stores quantization table information in a
//				fast-to-use format.
//
// Context:		Used by quantizer/dequantizer to store formatted
//				quantization tables.
//
// Fields:
//	precision	0 => elements contains 8-bit elements,
//				1 => elements contains 16-bit elements.
//	ident		Table identifier (0-3).
//	elements	Pointer to 64 table elements + 16 extra elements to catch
//				input data errors that may cause malfunction of the
//				Huffman decoder.
//	elarray		Space for elements (see above) plus 8 bytes to align
//				to a quadword boundary.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct _QUANT_TABLE
{
	int precision;
	int ident;
	short *elements;
	short elarray[84];

} QUANT_TABLE;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		JPEGQuantTable
//
// Purpose:		Stores pointers to JPEG binary spec compliant
//				quantization table information.
//
// Context:		Used by interface and table methods to specify encoder 
//				tables to generate and store JPEG images.
//
// Fields:
//	quantizer	Zig-zag order elements specifying quantization factors.
//	ident		Specifies identifier for this table.
//				0-3 valid for Extended Baseline JPEG compliance.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct
{
	unsigned char *quantizer;
	unsigned char ident;

} JPEGQuantTable;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		FRAME_COMPONENT
//
// Purpose:		One frame-component structure is allocated per component
//				in a frame.
//
// Context:		Used by Huffman decoder to manage components.
//
// Fields:
//	ident		Component identifier.  The tables use this ident to
//				determine the correct table for each component.
//	hsampling	Horizontal subsampling factor for this component,
//				1-4 are legal.
//	vsampling	Vertical subsampling factor for this component,
//				1-4 are legal.
//	quant_sel	Quantization table selector.  The quantization table
//				used by this component is determined via this selector.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct _FRAME_COMPONENT
{
	int ident;
	int hsampling;
	int vsampling;
	int quant_sel;

} FRAME_COMPONENT;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		FRAME
//
// Purpose:		Stores frame-specific data.
//
// Context:		One Frame structure per image.
//
// Fields:	
//	precision		Sample precision in bits.
//	width			Width of the source image in pixels.
//	height			Height of the source image in pixels.
//	MCUheight		Height of a frame MCU.
//	MCUwidth		Width of a frame MCU.
//	max_hsampling	Max horiz sampling ratio of any component in the frame.
//	max_vsampling	Max vert sampling ratio of any component in the frame.
//  ncomps			Number of components/channels in the frame.
//  horMCU			Number of horizontal MCUs in the frame.
//  totalMCU		Total number of MCUs in the frame.
//  comps			Array of 'ncomps' component descriptors.
//	restart_interv	Indicates number of MCUs after which to restart the
//					entropy parameters.
//	SeenAllDCScans	Used when decoding Multiscan images to determine if
//					all channels of an image have been decoded.
//	SeenAllACScans	(See SeenAllDCScans)
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct _FRAME
{
	int precision;
	int width;
	int height;
	int MCUheight;
	int MCUwidth;
	int max_hsampling;
	int max_vsampling;
	int ncomps;
	int horMCU;
	long totalMCU;
	FRAME_COMPONENT *comps;
	int restart_interv;
	int SeenAllDCScans;
	int SeenAllACScans;

} FRAME;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		SCAN_COMPONENT
//
// Purpose:		One scan-component structure is allocated per component
//				of each scan in a frame.
//
// Context:		Used by Huffman decoder to manage components within scans.
//
// Fields:
//	comp		Component number, index to the comps member of FRAME.
//	hsampling	Horizontal sampling factor.
//	vsampling	Vertical sampling factor.
//	dc_table	DC Huffman table pointer for this scan.
//	ac_table	AC Huffman table pointer for this scan.
//	quant_table	Quantization table pointer for this scan.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct
{
	int comp;
	int hsampling;
	int vsampling;
	HUFFMAN_TABLE *dc_table;
	HUFFMAN_TABLE *ac_table;
	QUANT_TABLE *quant_table;

} SCAN_COMPONENT;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		SCAN
//
// Purpose:		One SCAN structure is allocated per scan in a frame.
//
// Context:		Used by Huffman decoder to manage scans.
//
// Fields:
//	ncomps			Number of image components in a scan, 1-4 legal.
//	gray_scale		If TRUE, decode only the Y channel.
//	start_spec		Start coefficient of spectral or predictor selector.
//	end_spec		End coefficient of spectral selector.
//	approx_high		High bit position in successive approximation
//					Progressive coding.
//	approx_low		Low bit position in successive approximation
//					Progressive coding.
//	restart_interv	Restart interval, 0 if disabled.
//	curxMCU			Next horizontal MCU index to be processed after
//					an interrupted SCAN.
//	curyMCU			Next vertical MCU index to be processed after
//					an interrupted SCAN.
//	dc_diff			Array of DC predictor values for DPCM modes.
//	comps			Array of ncomps SCAN_COMPONENT component identifiers.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct _SCAN
{
	int ncomps;
	int gray_scale;
	int start_spec;
	int end_spec;
	int approx_high;
	int approx_low;
	unsigned int restart_interv;
	DWORD curxMCU;
	DWORD curyMCU;
	int dc_diff[4];
	SCAN_COMPONENT *comps;

} SCAN;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		DCTTYPE
//
// Purpose:		Possible algorithms to be used to perform the discrete
//				cosine transform (DCT).
//
// Fields:
//	IJL_AAN		The AAN (Arai, Agui, and Nakajima) algorithm from
//				Trans. IEICE, vol. E 71(11), 1095-1097, Nov. 1988.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef enum
{
	IJL_AAN = 0

} DCTTYPE;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		PROCESSOR_TYPE
//
// Purpose:		Possible types of processors.
//				Note that the enums are defined in ascending order
//				depending upon their various IA32 instruction support.
//
// Fields:
//
// IJL_OTHER_PROC
//		Does not support the CPUID instruction and
//		assumes no Pentium(R) processor instructions.
//
// IJL_PENTIUM_PROC
//		Corresponds to an Intel(R) Pentium processor
//		(or a 100% compatible) that supports the
//		Pentium processor instructions.
//
// IJL_PENTIUM_PRO_PROC
//		Corresponds to an Intel Pentium Pro processor
//		(or a 100% compatible) that supports the
//		Pentium Pro processor instructions.
//
// IJL_PENTIUM_PROC_MMX_TECH
//		Corresponds to an Intel Pentium processor
//		with MMX(TM) technology (or a 100% compatible)
//		that supports the MMX instructions.
//
// IJL_PENTIUM_II_PROC
//		Corresponds to an Intel Pentium II processor
//		(or a 100% compatible) that supports both the
//		Pentium Pro processor instructions and the
//		MMX instructions.
//
//	Any additional processor types that support a superset
//	of both the Pentium Pro processor instructions and the
//	MMX instructions should be given an enum value greater
//	than IJL_PENTIUM_II_PROC.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef enum
{
	IJL_OTHER_PROC				= 0,
	IJL_PENTIUM_PROC			= 1,
	IJL_PENTIUM_PRO_PROC		= 2,
	IJL_PENTIUM_PROC_MMX_TECH	= 3,
	IJL_PENTIUM_II_PROC			= 4

} PROCESSOR_TYPE;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		ENTROPYSTRUCT
//
// Purpose:		Stores the decoder state information necessary to "jump"
//				to a particular MCU row in a compressed entropy stream.  
//		
// Context:		Used to persist the decoder state within Decode_Scan when
//				decoding using ROIs.
//
// Fields:
//		offset				Offset (in bytes) into the entropy stream
//							from the beginning.
//		dcval1				DC val at the beginning of the MCU row
//							for component 1.
//		dcval2				DC val at the beginning of the MCU row
//							for component 2.
//		dcval3				DC val at the beginning of the MCU row
//							for component 3.
//		dcval4				DC val at the beginning of the MCU row
//							for component 4.
//		bit_buffer_64		64-bit Huffman bit buffer.  Stores current
//							bit buffer at the start of a MCU row.
//							Also used as a 32-bit buffer on 32-bit
//							architectures.
//		bitbuf_bits_valid	Number of valid bits in the above bit buffer.
//		unread_marker		Have any markers been decoded but not
//							processed at the beginning of a MCU row?
//							This entry holds the unprocessed marker, or
//							0 if none.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct
{
	DWORD offset;
	int  dcval1;
	int  dcval2;
	int  dcval3;
	int  dcval4;
	unsigned __int64  bit_buffer_64;
	int bitbuf_bits_valid;
	unsigned char unread_marker;

} ENTROPYSTRUCT;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		STATE
//
// Purpose:		Stores the active state of the IJL.
//
// Context:		Used by all low-level routines to store pseudo-global or
//				state variables.
//
// Fields:
//		bit_buffer_64			64-bit bitbuffer utilized by Huffman
//								encoder/decoder algorithms utilizing routines
//								designed for MMX(TM) technology.
//		bit_buffer_32			32-bit bitbuffer for all other Huffman
//								encoder/decoder algorithms.
//		bitbuf_bits_valid		Number of bits in the above two fields that
//								are valid.
//
//		cur_entropy_ptr			Current position (absolute address) in
//								the entropy buffer.
//		start_entropy_ptr		Starting position (absolute address) of
//								the entropy buffer.
//		end_entropy_ptr			Ending position (absolute address) of
//								the entropy buffer.
//		entropy_bytes_processed Number of bytes actually processed
//								(passed over) in the entropy buffer.
//		entropy_buf_maxsize		Max size of the entropy buffer.
//		entropy_bytes_left		Number of bytes left in the entropy buffer.
//		Prog_EndOfBlock_Run		Progressive block run counter.
//
//		DIB_ptr					Temporary offset into the input/output DIB.
//
//		unread_marker			If a marker has been read but not processed,
//								stick it in this field.
//		processor_type			(0, 1, or 2) == current processor does not
//								support MMX(TM) instructions.
//								(3 or 4) == current processor does
//								support MMX(TM) instructions.
//		cur_scan_comp			On which component of the scan are we working?
//		file					Process file handle, or
//								0x00000000 if no file is defined.
//		JPGBuffer				Entropy buffer (~4K).
//		
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct _STATE
{
	// Bit buffer.
	unsigned __int64 bit_buffer_64;
	unsigned long bit_buffer_32;
	int bitbuf_bits_valid;

	// Entropy.
	unsigned char *cur_entropy_ptr;
	unsigned char *start_entropy_ptr;
	unsigned char *end_entropy_ptr;
	long entropy_bytes_processed;
	long entropy_buf_maxsize;
	int entropy_bytes_left;
	int Prog_EndOfBlock_Run;

	// Input or output DIB.
	unsigned char *DIB_ptr;

	// Control.
	unsigned char unread_marker;
	PROCESSOR_TYPE processor_type;
	int cur_scan_comp;
	FILE *file;
	BYTE JPGBuffer[JBUFSIZE];

} STATE;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		FAST_MCU_PROCESSING_TYPE
//
// Purpose:		Advanced Control Option.  Do NOT modify.
//				WARNING:  Used for internal reference only.
//
// Fields:
//
//   IJL_(sampling)_(JPEG color space)_(sampling)_(DIB color space)
//      Decode is read left to right w/ upsampling.
//      Encode is read right to left w/ subsampling.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef enum
{
	IJL_NO_CC_OR_US						= 0,

	IJL_111_YCBCR_111_RGB				= 1,
	IJL_111_YCBCR_111_BGR				= 2,

	IJL_411_YCBCR_111_RGB				= 3,
	IJL_411_YCBCR_111_BGR				= 4,

	IJL_422_YCBCR_111_RGB				= 5,
	IJL_422_YCBCR_111_BGR				= 6,

	IJL_111_YCBCR_1111_RGBA_FPX			= 7,
	IJL_411_YCBCR_1111_RGBA_FPX			= 8,
	IJL_422_YCBCR_1111_RGBA_FPX			= 9,

	IJL_1111_YCBCRA_FPX_1111_RGBA_FPX	= 10,
	IJL_4114_YCBCRA_FPX_1111_RGBA_FPX	= 11,
	IJL_4224_YCBCRA_FPX_1111_RGBA_FPX	= 12,

	IJL_111_RGB_1111_RGBA_FPX			= 13,

	IJL_1111_RGBA_FPX_1111_RGBA_FPX		= 14

} FAST_MCU_PROCESSING_TYPE;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		JPEG_PROPERTIES
//
// Purpose:		Stores low-level and control information.  It is used by
//				both the encoder and decoder.  An advanced external user
//				may access this structure to expand the interface
//				capability.
//
//				See the Developer's Guide for an expanded description
//				of this structure and its use.
//
// Context:		Used by all interface methods and most IJL routines.
//
// Fields:
//
//	iotype				IN:		Specifies type of data operation
//								(read/write/other) to be
//								performed by IJL_Read or IJL_Write.
//	roi					IN:		Rectangle-Of-Interest to read from, or
//								write to, in pixels.
//	dcttype				IN:		DCT alogrithm to be used.
//	fast_processing		OUT:	Supported fast pre/post-processing path.
//								This is set by the IJL.
//	interrupt			IN:		Signals an interrupt has been requested.
//
//	DIBBytes			IN:		Pointer to buffer of uncompressed data.
//	DIBWidth			IN:		Width of uncompressed data.
//	DIBHeight			IN:		Height of uncompressed data.
//	DIBPadBytes			IN:		Padding (in bytes) at end of each 
//								row in the uncompressed data.
//	DIBChannels			IN:		Number of components in the
//								uncompressed data.
//	DIBColor			IN:		Color space of uncompressed data.
//	DIBSubsampling		IN:		Required to be IJL_NONE.
//	DIBLineBytes		OUT:	Number of bytes in an output DIB line
//								including padding.
//
//	JPGFile				IN:		Pointer to file based JPEG.
//	JPGBytes			IN:		Pointer to buffer based JPEG.
//	JPGSizeBytes		IN:		Max buffer size. Used with JPGBytes.
//						OUT:	Number of compressed bytes written.
//	JPGWidth			IN:		Width of JPEG image.
//						OUT:	After reading (except READHEADER).
//	JPGHeight			IN:		Height of JPEG image.
//						OUT:	After reading (except READHEADER).
//	JPGChannels			IN:		Number of components in JPEG image.
//						OUT:	After reading (except READHEADER).
//	JPGColor			IN:		Color space of JPEG image.
//	JPGSubsampling		IN:		Subsampling of JPEG image.
//						OUT:	After reading (except READHEADER).
//	JPGThumbWidth		OUT:	JFIF embedded thumbnail width [0-255].
//	JPGThumbHeight		OUT:	JFIF embedded thumbnail height [0-255].
//
//	cconversion_reqd	OUT:	If color conversion done on decode, TRUE.
//	upsampling_reqd		OUT:	If upsampling done on decode, TRUE.
//	jquality			IN:		[0-100] where highest quality is 100.
//	jinterleaveType		IN/OUT:	0 => MCU interleaved file, and
//								1 => 1 scan per component.
//	numxMCUs			OUT:	Number of MCUs in the x direction.
//	numyMCUs			OUT:	Number of MCUs in the y direction.
//
//	nqtables			IN/OUT:	Number of quantization tables.
//	maxquantindex		IN/OUT:	Maximum index of quantization tables.
//	nhuffActables		IN/OUT:	Number of AC Huffman tables.
//	nhuffDctables		IN/OUT:	Number of DC Huffman tables.
//	maxhuffindex		IN/OUT: Maximum index of Huffman tables.
//	jFmtQuant			IN/OUT:	Formatted quantization table info.
//	jFmtAcHuffman		IN/OUT:	Formatted AC Huffman table info.
//	jFmtDcHuffman		IN/OUT:	Formatted DC Huffman table info.
//
//	jEncFmtQuant		IN/OUT:	Pointer to one of the above, or
//								to externally persisted table.
//	jEncFmtAcHuffman	IN/OUT:	Pointer to one of the above, or
//								to externally persisted table.
//	jEncFmtDcHuffman	IN/OUT:	Pointer to one of the above, or
//								to externally persisted table.
//
//	use_default_qtables	IN:		Set to default quantization tables.
//								Clear to supply your own.
//	use_default_htables	IN:		Set to default Huffman tables.
//								Clear to supply your own.
//	rawquanttables		IN:		Up to 4 sets of quantization tables.
//	rawhufftables		IN:		Alternating pairs (DC/AC) of up to 4
//								sets of raw Huffman tables.
//	HuffIdentifierAC	IN:		Indicates what channel the user-
//								supplied Huffman AC tables apply to.
//	HuffIdentifierDC	IN:		Indicates what channel the user-
//								supplied Huffman DC tables apply to.
//
//	jframe				OUT:	Structure with frame-specific info.
//	needframe			OUT:	TRUE when a frame has been detected.
//
//	jscan				Persistence for current scan pointer when
//						interrupted.
//
//	state				OUT:	Contains info on the state of the IJL.
//	SawAdobeMarker		OUT:	Decoder saw an APP14 marker somewhere.
//	AdobeXform			OUT:	If SawAdobeMarker TRUE, this indicates
//								the JPEG color space given by that marker.
//
//	rowoffsets			Persistence for the decoder MCU row origins
//						when decoding by ROI.  Offsets (in bytes
//						from the beginning of the entropy data)
//						to the start of each of the decoded rows.
//						Fill the offsets with -1 if they have not
//						been initalized and NULL could be the
//						offset to the first row.
//
//	MCUBuf				OUT:	Quadword aligned internal buffer.
//								Big enough for the largest MCU
//								(10 blocks) with extra room for
//								additional operations.
//	tMCUBuf				OUT:	Version of above, without alignment.
//
//	processor_type		OUT:	Determines type of processor found
//								during initialization.
//
//	ignoreDCTs			IN:		Assert to bypass DCTs when processing
//								data.  Required for conformance
//								testing.
//
//	progressive_found	OUT:	1 when progressive image detected.
//	coef_buffer			IN:		Pointer to a larger buffer containing
//								frequency coefficients when they
//								cannot be decoded dynamically
//								(i.e., as in progressive decoding).
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef struct
{
	// Compression/Decompression control.
	IJLIOTYPE iotype;							// default = IJL_SETUP
	RECT roi;									// default = 0
	DCTTYPE dcttype;							// default = IJL_AAN
	FAST_MCU_PROCESSING_TYPE fast_processing;	// default = IJL_NO_CC_OR_US
	DWORD interrupt;							// default = FALSE

	// DIB specific I/O data specifiers.
	BYTE *DIBBytes;								// default = NULL
	DWORD DIBWidth;								// default = 0
	int DIBHeight;								// default = 0
	DWORD DIBPadBytes;							// default = 0
	DWORD DIBChannels;							// default = 3
	IJL_COLOR DIBColor;							// default = IJL_BGR
	IJL_DIBSUBSAMPLING DIBSubsampling;			// default = IJL_NONE
	int DIBLineBytes;							// default = 0

	// JPEG specific I/O data specifiers.
	char *JPGFile;								// default = NULL
	BYTE *JPGBytes;								// default = NULL
	DWORD JPGSizeBytes;							// default = 0
	DWORD JPGWidth;								// default = 0
	DWORD JPGHeight;							// default = 0
	DWORD JPGChannels;							// default = 3
	IJL_COLOR JPGColor;							// default = IJL_YCBCR
	IJL_JPGSUBSAMPLING JPGSubsampling;			// default = IJL_411
	DWORD JPGThumbWidth;						// default = 0
	DWORD JPGThumbHeight;						// default = 0

	// JPEG conversion properties.
	DWORD cconversion_reqd;						// default = TRUE
	DWORD upsampling_reqd;						// default = TRUE
	DWORD jquality;								// default = 75
	DWORD jinterleaveType;						// default = 0
	DWORD numxMCUs;								// default = 0
	DWORD numyMCUs;								// default = 0

	// Tables.
	DWORD nqtables;
	DWORD maxquantindex;
	DWORD nhuffActables;
	DWORD nhuffDctables;
	DWORD maxhuffindex;
	QUANT_TABLE jFmtQuant[4];
	HUFFMAN_TABLE jFmtAcHuffman[4];
	HUFFMAN_TABLE jFmtDcHuffman[4];

	short *jEncFmtQuant[4];
	HUFFMAN_TABLE *jEncFmtAcHuffman[4];
	HUFFMAN_TABLE *jEncFmtDcHuffman[4];

	// Allow user-defined tables.
	DWORD use_default_qtables;
	DWORD use_default_htables;
	JPEGQuantTable rawquanttables[4];
	JPEGHuffTable rawhufftables[8];
	BYTE HuffIdentifierAC[4];
	BYTE HuffIdentifierDC[4];

	// Frame specific members.
	FRAME jframe;
	int needframe;

	// SCAN persistent members.
	SCAN *jscan;

	// State members.
	STATE state;
	DWORD SawAdobeMarker;
	DWORD AdobeXform;

	// ROI decoder members.
	ENTROPYSTRUCT *rowoffsets;

	// Intermediate buffers.
	unsigned char *MCUBuf;
	unsigned char tMCUBuf[720*2];

	// Processor detected.
	PROCESSOR_TYPE processor_type;

	// Test specific members.
	DWORD ignoreDCTs;

	// Progressive mode members.
	int progressive_found;
	short *coef_buffer;

} JPEG_PROPERTIES;


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		JPEG_CORE_PROPERTIES
//
// Purpose:		This is the primary data structure between the IJL and
//				the external user.  It stores JPEG state information
//				and controls the IJL.  It is user-modifiable.
//
//				See the Developer's Guide for details on appropriate usage.
//
// Context:		Used by all low-level IJL routines to store
//				pseudo-global information.
//
// Fields:
//
//	UseJPEGPROPERTIES	Set this flag != 0 if you wish to override
//						the JPEG_CORE_PROPERTIES "IN" parameters with
//						the JPEG_PROPERTIES parameters.
//
//	DIBBytes			IN:		Pointer to buffer of uncompressed data.
//	DIBWidth			IN:		Width of uncompressed data.
//	DIBHeight			IN:		Height of uncompressed data.
//	DIBPadBytes			IN:		Padding (in bytes) at end of each 
//								row in the uncompressed data.
//	DIBChannels			IN:		Number of components in the
//								uncompressed data.
//	DIBColor			IN:		Color space of uncompressed data.
//	DIBSubsampling		IN:		Required to be IJL_NONE.
//
//	JPGFile				IN:		Pointer to file based JPEG.
//	JPGBytes			IN:		Pointer to buffer based JPEG.
//	JPGSizeBytes		IN:		Max buffer size. Used with JPGBytes.
//						OUT:	Number of compressed bytes written.
//	JPGWidth			IN:		Width of JPEG image.
//						OUT:	After reading (except READHEADER).
//	JPGHeight			IN:		Height of JPEG image.
//						OUT:	After reading (except READHEADER).
//	JPGChannels			IN:		Number of components in JPEG image.
//						OUT:	After reading (except READHEADER).
//	JPGColor			IN:		Color space of JPEG image.
//	JPGSubsampling		IN:		Subsampling of JPEG image.
//						OUT:	After reading (except READHEADER).
//	JPGThumbWidth		OUT:	JFIF embedded thumbnail width [0-255].
//	JPGThumbHeight		OUT:	JFIF embedded thumbnail height [0-255].
//
//	cconversion_reqd	OUT:	If color conversion done on decode, TRUE.
//	upsampling_reqd		OUT:	If upsampling done on decode, TRUE.
//	jquality			IN:		[0-100] where highest quality is 100.
//
//	jprops				"Low-Level" IJL data structure.
//
////////////////////////////////////////////////////////////////////////////
*D*/
struct JPEG_CORE_PROPERTIES
{
	DWORD UseJPEGPROPERTIES;				// default = 0

	// DIB specific I/O data specifiers.
	BYTE *DIBBytes;							// default = NULL
	DWORD DIBWidth;							// default = 0
	int DIBHeight;							// default = 0
	DWORD DIBPadBytes;						// default = 0
	DWORD DIBChannels;						// default = 3
	IJL_COLOR DIBColor;						// default = IJL_BGR
	IJL_DIBSUBSAMPLING DIBSubsampling;		// default = IJL_NONE

	// JPEG specific I/O data specifiers.
	char *JPGFile;							// default = NULL
	BYTE *JPGBytes;							// default = NULL
	DWORD JPGSizeBytes;						// default = 0
	DWORD JPGWidth;							// default = 0
	DWORD JPGHeight;						// default = 0
	DWORD JPGChannels;						// default = 3
	IJL_COLOR JPGColor;						// default = IJL_YCBCR
	IJL_JPGSUBSAMPLING JPGSubsampling;		// default = IJL_411
	DWORD JPGThumbWidth;					// default = 0
	DWORD JPGThumbHeight;					// default = 0

	// JPEG conversion properties.
	DWORD cconversion_reqd;					// default = TRUE
	DWORD upsampling_reqd;					// default = TRUE
	DWORD jquality;							// default = 75

	// Low-level properties.
	JPEG_PROPERTIES jprops;

};


/*D*
////////////////////////////////////////////////////////////////////////////
// Name:		IJLERR
//
// Purpose:		Listing of possible "error" codes returned by the IJL.
//
//				See the Developer's Guide for details on appropriate usage.
//
// Context:		Used for error checking.
//
////////////////////////////////////////////////////////////////////////////
*D*/
typedef enum
{
	// The following "error" values indicate an "OK" condition.
	IJL_OK								=   0,
	IJL_INTERRUPT_OK					=   1,
	IJL_ROI_OK							=   2,

	// The following "error" values indicate an error has occurred.
	IJL_EXCEPTION_DETECTED				=  -1,
	IJL_INVALID_ENCODER					=  -2,
	IJL_UNSUPPORTED_SUBSAMPLING			=  -3,
	IJL_UNSUPPORTED_BYTES_PER_PIXEL		=  -4,
	IJL_MEMORY_ERROR					=  -5,
	IJL_BAD_HUFFMAN_TABLE				=  -6,
	IJL_BAD_QUANT_TABLE					=  -7,
	IJL_INVALID_JPEG_PROPERTIES			=  -8,
	IJL_ERR_FILECLOSE					=  -9,
	IJL_INVALID_FILENAME				= -10,
	IJL_ERROR_EOF						= -11,
	IJL_PROG_NOT_SUPPORTED				= -12,
	IJL_ERR_NOT_JPEG					= -13,
	IJL_ERR_COMP						= -14,
	IJL_ERR_SOF							= -15,
	IJL_ERR_DNL							= -16,
	IJL_ERR_NO_HUF						= -17,
	IJL_ERR_NO_QUAN						= -18,
	IJL_ERR_NO_FRAME					= -19,
	IJL_ERR_MULT_FRAME					= -20,
	IJL_ERR_DATA						= -21,
	IJL_ERR_NO_IMAGE					= -22,
	IJL_FILE_ERROR						= -23,
	IJL_INTERNAL_ERROR					= -24,
	IJL_BAD_RST_MARKER					= -27,
	IJL_THUMBNAIL_DIB_TOO_SMALL			= -28,
	IJL_THUMBNAIL_DIB_WRONG_COLOR		= -29,
	IJL_RESERVED						= -99

} IJLERR;


////////////////////////////////////////////////////////////////////////////
// Function Prototypes (API Calls)
////////////////////////////////////////////////////////////////////////////


/*F*
////////////////////////////////////////////////////////////////////////////
// Name:		IJL_Init
//
// Purpose:		Used to initalize the IJL.
//
//				See the Developer's Guide for details on appropriate usage.
//
// Context:		Always call this before anything else.
//				Also, only call this with a new jcprops structure, or
//				after calling IJL_Free.  Otherwise, dynamically
//				allocated memory may be leaked.
//
// Returns:		Any IJLERR value.  IJL_OK indicates success.
//
// Parameters:
//	jcprops		Pointer to an externally allocated
//				JPEG_CORE_PROPERTIES structure.
//
////////////////////////////////////////////////////////////////////////////
*F*/
extern "C" IJLAPI
IJLERR IJL_Init(JPEG_CORE_PROPERTIES *jcprops);


/*F*
////////////////////////////////////////////////////////////////////////////
// Name:		IJL_Free
//
// Purpose:		Used to properly close down the IJL.
//
//				See the Developer's Guide for details on appropriate usage.
//
// Context:		Always call this when done using the IJL to perform
//				clean-up of dynamically allocated memory.
//				Note, IJL_Init will have to be called to use the
//				IJL again.
//
// Returns:		Any IJLERR value.  IJL_OK indicates success.
//
// Parameters:
//	jcprops		Pointer to an externally allocated
//				JPEG_CORE_PROPERTIES structure.
//
////////////////////////////////////////////////////////////////////////////
*F*/
extern "C" IJLAPI
IJLERR IJL_Free(JPEG_CORE_PROPERTIES *jcprops);


/*F*
////////////////////////////////////////////////////////////////////////////
// Name:		IJL_Read
//
// Purpose:		Used to read JPEG data (entropy, or header, or both) into
//				a user-supplied buffer (to hold the image data) and/or
//				into the JPEG_CORE_PROPERTIES structure (to hold the
//				header info).
//
// Context:		See the Developer's Guide for a detailed description
//				on the use of this function.  The jcprops main data
//				members are checked for consistency.
//
// Returns:		Any IJLERR value.  IJL_OK indicates success.
//
// Parameters:
//	jcprops		Pointer to an externally allocated
//				JPEG_CORE_PROPERTIES structure.
//	iotype		Specifies what type of read operation to perform.
//
////////////////////////////////////////////////////////////////////////////
*F*/
extern "C" IJLAPI
IJLERR IJL_Read(JPEG_CORE_PROPERTIES *jcprops, IJLIOTYPE iotype);


/*F*
////////////////////////////////////////////////////////////////////////////
// Name:		IJL_Write
//
// Purpose:		Used to write JPEG data (entropy, or header, or both) into
//				a user-supplied buffer (to hold the image data) and/or
//				into the JPEG_CORE_PROPERTIES structure (to hold the
//				header info).
//
// Context:		See the Developer's Guide for a detailed description
//				on the use of this function.  The jcprops main data
//				members are checked for consistency.
//
// Returns:		Any IJLERR value.  IJL_OK indicates success.
//
// Parameters:
//	jcprops		Pointer to an externally allocated
//				JPEG_CORE_PROPERTIES structure.
//	iotype		Specifies what type of write operation to perform.
//
////////////////////////////////////////////////////////////////////////////
*F*/
extern "C" IJLAPI
IJLERR IJL_Write(JPEG_CORE_PROPERTIES *jcprops, IJLIOTYPE iotype);


/*F*
////////////////////////////////////////////////////////////////////////////
// Name:		IJL_Version
//
// Purpose:		Used to properly identify the version number of the IJL.
//
//				See the Developer's Guide for details on appropriate usage.
//
// Context:		Call to get the IJL version number.
//
// Returns:		Returns a floating point number which indicates the
//				IJL version number (i.e., X.XXXX).  The units and
//				tenths places represent the IJL major release
//				(i.e., 1.2XX), and the hundredths and thousandths
//				places represent the IJL minor release (i.e., X.X02).
//
// Parameters:	none
//
////////////////////////////////////////////////////////////////////////////
*F*/
extern "C" IJLAPI
double IJL_Version( );


#endif  // _IJL_H_

# Generated from 'Menus.h'

def FOUR_CHAR_CODE(x): return x
noMark = 0
kMenuDrawMsg = 0
kMenuSizeMsg = 2
kMenuPopUpMsg = 3
kMenuCalcItemMsg = 5
kMenuThemeSavvyMsg = 7
mDrawMsg = 0
mSizeMsg = 2
mPopUpMsg = 3
mCalcItemMsg = 5
mChooseMsg = 1
mDrawItemMsg = 4
kMenuChooseMsg = 1
kMenuDrawItemMsg = 4
kThemeSavvyMenuResponse = 0x7473
kMenuInitMsg = 8
kMenuDisposeMsg = 9
kMenuFindItemMsg = 10
kMenuHiliteItemMsg = 11
kMenuDrawItemsMsg = 12
textMenuProc = 0
hMenuCmd = 27
hierMenu = -1
kInsertHierarchicalMenu = -1
mctAllItems = -98
mctLastIDIndic = -99
kMenuStdMenuProc = 63
kMenuStdMenuBarProc = 63
kMenuNoModifiers = 0
kMenuShiftModifier = (1 << 0)
kMenuOptionModifier = (1 << 1)
kMenuControlModifier = (1 << 2)
kMenuNoCommandModifier = (1 << 3)
kMenuNoIcon = 0
kMenuIconType = 1
kMenuShrinkIconType = 2
kMenuSmallIconType = 3
kMenuColorIconType = 4
kMenuIconSuiteType = 5
kMenuIconRefType = 6
kMenuCGImageRefType = 7
kMenuSystemIconSelectorType = 8
kMenuIconResourceType = 9
kMenuNullGlyph = 0x00
kMenuTabRightGlyph = 0x02
kMenuTabLeftGlyph = 0x03
kMenuEnterGlyph = 0x04
kMenuShiftGlyph = 0x05
kMenuControlGlyph = 0x06
kMenuOptionGlyph = 0x07
kMenuSpaceGlyph = 0x09
kMenuDeleteRightGlyph = 0x0A
kMenuReturnGlyph = 0x0B
kMenuReturnR2LGlyph = 0x0C
kMenuNonmarkingReturnGlyph = 0x0D
kMenuPencilGlyph = 0x0F
kMenuDownwardArrowDashedGlyph = 0x10
kMenuCommandGlyph = 0x11
kMenuCheckmarkGlyph = 0x12
kMenuDiamondGlyph = 0x13
kMenuAppleLogoFilledGlyph = 0x14
kMenuParagraphKoreanGlyph = 0x15
kMenuDeleteLeftGlyph = 0x17
kMenuLeftArrowDashedGlyph = 0x18
kMenuUpArrowDashedGlyph = 0x19
kMenuRightArrowDashedGlyph = 0x1A
kMenuEscapeGlyph = 0x1B
kMenuClearGlyph = 0x1C
kMenuLeftDoubleQuotesJapaneseGlyph = 0x1D
kMenuRightDoubleQuotesJapaneseGlyph = 0x1E
kMenuTrademarkJapaneseGlyph = 0x1F
kMenuBlankGlyph = 0x61
kMenuPageUpGlyph = 0x62
kMenuCapsLockGlyph = 0x63
kMenuLeftArrowGlyph = 0x64
kMenuRightArrowGlyph = 0x65
kMenuNorthwestArrowGlyph = 0x66
kMenuHelpGlyph = 0x67
kMenuUpArrowGlyph = 0x68
kMenuSoutheastArrowGlyph = 0x69
kMenuDownArrowGlyph = 0x6A
kMenuPageDownGlyph = 0x6B
kMenuAppleLogoOutlineGlyph = 0x6C
kMenuContextualMenuGlyph = 0x6D
kMenuPowerGlyph = 0x6E
kMenuF1Glyph = 0x6F
kMenuF2Glyph = 0x70
kMenuF3Glyph = 0x71
kMenuF4Glyph = 0x72
kMenuF5Glyph = 0x73
kMenuF6Glyph = 0x74
kMenuF7Glyph = 0x75
kMenuF8Glyph = 0x76
kMenuF9Glyph = 0x77
kMenuF10Glyph = 0x78
kMenuF11Glyph = 0x79
kMenuF12Glyph = 0x7A
kMenuF13Glyph = 0x87
kMenuF14Glyph = 0x88
kMenuF15Glyph = 0x89
kMenuControlISOGlyph = 0x8A
kMenuAttrExcludesMarkColumn = (1 << 0)
kMenuAttrAutoDisable = (1 << 2)
kMenuAttrUsePencilGlyph = (1 << 3)
kMenuAttrHidden = (1 << 4)
kMenuItemAttrDisabled = (1 << 0)
kMenuItemAttrIconDisabled = (1 << 1)
kMenuItemAttrSubmenuParentChoosable = (1 << 2)
kMenuItemAttrDynamic = (1 << 3)
kMenuItemAttrNotPreviousAlternate = (1 << 4)
kMenuItemAttrHidden = (1 << 5)
kMenuItemAttrSeparator = (1 << 6)
kMenuItemAttrSectionHeader = (1 << 7)
kMenuItemAttrIgnoreMeta = (1 << 8)
kMenuItemAttrAutoRepeat = (1 << 9)
kMenuItemAttrUseVirtualKey = (1 << 10)
kMenuItemAttrCustomDraw = (1 << 11)
kMenuItemAttrIncludeInCmdKeyMatching = (1 << 12)
kMenuTrackingModeMouse = 1
kMenuTrackingModeKeyboard = 2
kMenuEventIncludeDisabledItems = 0x0001
kMenuEventQueryOnly = 0x0002
kMenuEventDontCheckSubmenus = 0x0004
kMenuItemDataText = (1 << 0)
kMenuItemDataMark = (1 << 1)
kMenuItemDataCmdKey = (1 << 2)
kMenuItemDataCmdKeyGlyph = (1 << 3)
kMenuItemDataCmdKeyModifiers = (1 << 4)
kMenuItemDataStyle = (1 << 5)
kMenuItemDataEnabled = (1 << 6)
kMenuItemDataIconEnabled = (1 << 7)
kMenuItemDataIconID = (1 << 8)
kMenuItemDataIconHandle = (1 << 9)
kMenuItemDataCommandID = (1 << 10)
kMenuItemDataTextEncoding = (1 << 11)
kMenuItemDataSubmenuID = (1 << 12)
kMenuItemDataSubmenuHandle = (1 << 13)
kMenuItemDataFontID = (1 << 14)
kMenuItemDataRefcon = (1 << 15)
kMenuItemDataAttributes = (1 << 16)
kMenuItemDataCFString = (1 << 17)
kMenuItemDataProperties = (1 << 18)
kMenuItemDataIndent = (1 << 19)
kMenuItemDataCmdVirtualKey = (1 << 20)
kMenuItemDataAllDataVersionOne = 0x000FFFFF
kMenuItemDataAllDataVersionTwo = kMenuItemDataAllDataVersionOne | kMenuItemDataCmdVirtualKey
kMenuDefProcPtr = 0
kMenuPropertyPersistent = 0x00000001
kHierarchicalFontMenuOption = 0x00000001
gestaltContextualMenuAttr = FOUR_CHAR_CODE('cmnu')
gestaltContextualMenuUnusedBit = 0
gestaltContextualMenuTrapAvailable = 1
gestaltContextualMenuHasAttributeAndModifierKeys = 2
gestaltContextualMenuHasUnicodeSupport = 3
kCMHelpItemNoHelp = 0
kCMHelpItemAppleGuide = 1
kCMHelpItemOtherHelp = 2
kCMHelpItemRemoveHelp = 3
kCMNothingSelected = 0
kCMMenuItemSelected = 1
kCMShowHelpSelected = 3
keyContextualMenuName = FOUR_CHAR_CODE('pnam')
keyContextualMenuCommandID = FOUR_CHAR_CODE('cmcd')
keyContextualMenuSubmenu = FOUR_CHAR_CODE('cmsb')
keyContextualMenuAttributes = FOUR_CHAR_CODE('cmat')
keyContextualMenuModifiers = FOUR_CHAR_CODE('cmmd')

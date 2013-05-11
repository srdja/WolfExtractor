/*

	Copyright (C) 2007-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/**
 * \file wolf.c
 * \brief Wolfenstein 3-D data file decoder.
 * \author Michael Liebscher 
 * \date 2007-2013
 */

#include <stdio.h>

#include "wolf.h"

#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../filesys/file.h"
#include "../../memory/memory.h"
#include "../../loaders/tga.h"
#include "../../image/image.h"
#include "../../string/wtstring.h"
#include "../core/wolfcore.h"
#include "../../pak/pak.h"
#include "../wolfcore_decoder.h"


extern W8 wolf_gamepal[];

extern W32 _filterScale;
extern W32 _filterScale_Sprites;
extern wtBoolean _doRedux;
extern wtBoolean _outputInDirectory;


PRIVATE W32 CeilingColourWL6[] =
{
	0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0xbfbf,
	0x4e4e,0x4e4e,0x4e4e,0x1d1d,0x8d8d,0x4e4e,0x1d1d,0x2d2d,0x1d1d,0x8d8d,
	0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x1d1d,0x2d2d,0xdddd,0x1d1d,0x1d1d,0x9898,

	0x1d1d,0x9d9d,0x2d2d,0xdddd,0xdddd,0x9d9d,0x2d2d,0x4d4d,0x1d1d,0xdddd,
	0x7d7d,0x1d1d,0x2d2d,0x2d2d,0xdddd,0xd7d7,0x1d1d,0x1d1d,0x1d1d,0x2d2d,
	0x1d1d,0x1d1d,0x1d1d,0x1d1d,0xdddd,0xdddd,0x7d7d,0xdddd,0xdddd,0xdddd
};

PRIVATE char *WL6_songs[] =
{
//
// Episode One
//
	"GETTHEM",
	"SEARCHN",
	"POW",
	"SUSPENSE",
	"GETTHEM",
	"SEARCHN",
	"POW",
	"SUSPENSE",

	"WARMARCH",	    // Boss level
	"CORNER",		// Secret level

//
// Episode Two
//
	"NAZI_OMI",
	"PREGNANT",
	"GOINGAFT",
	"HEADACHE",
	"NAZI_OMI",
	"PREGNANT",
	"HEADACHE",
	"GOINGAFT",

	"WARMARCH",	// Boss level
	"DUNGEON",	// Secret level

//
// Episode Three
//
	"INTROCW3",
	"NAZI_RAP",
	"TWELFTH",
	"ZEROHOUR",
	"INTROCW3",
	"NAZI_RAP",
	"TWELFTH",
	"ZEROHOUR",

	"ULTIMATE",	    // Boss level
	"PACMAN",		// Secret level

//
// Episode Four
//
	"GETTHEM",
	"SEARCHN",
	"POW",
	"SUSPENSE",
	"GETTHEM",
	"SEARCHN",
	"POW",
	"SUSPENSE",

	"WARMARCH",	    // Boss level
	"CORNER",		// Secret level

//
// Episode Five
//
	"NAZI_OMI",
	"PREGNANT",
	"GOINGAFT",
	"HEADACHE",
	"NAZI_OMI",
	"PREGNANT",
	"HEADACHE",
	"GOINGAFT",

	"WARMARCH",	// Boss level
	"DUNGEON",	// Secret level

//
// Episode Six
//
	"INTROCW3",
	"NAZI_RAP",
	"TWELFTH",
	"ZEROHOUR",
	"INTROCW3",
	"NAZI_RAP",
	"TWELFTH",
	"ZEROHOUR",

	"ULTIMATE",	    // Boss level
	"FUNKYOU"		// Secret level
};

PRIVATE char *songTitles[] = 
{
    "CORNER",
	"DUNGEON",
	"WARMARCH",
	"GETTHEM",
	"HEADACHE",
	"HITLWLTZ",
	"INTROCW3",
	"NAZI_NOR",
	"NAZI_OMI",
	"P.O.W",
	"SALUTE",
	"SEARCHN",
	"SUSPENSE",
	"VICTORS",
	"WONDERIN",
	"FUNKYOU",
	"ENDLEVEL",
	"GOINGAFT",
	"PREGNANT",
	"ULTIMATE",
	"NAZI_RAP",
	"ZEROHOUR",
	"TWELFTH",
	"ROSTER",
	"URAHERO",
	"VICMARCH",
	"PACMAN"
};


PRIVATE parTimes_t parTimesWL6[] =
{
	 //
	 // Episode One Par Times
	 //
	 { 1.5,	"01:30" },
	 { 2,	"02:00" },
	 { 2,	"02:00" },
	 { 3.5,	"03:30" },
	 { 3,	"03:00" },
	 { 3,	"03:00" },
	 { 2.5,	"02:30" },
	 { 2.5,	"02:30" },
	 { 0,	"??:??" },	// Boss level
	 { 0,	"??:??" },	// Secret level

	 //
	 // Episode Two Par Times
	 //
	 { 1.5,	"01:30" },
	 { 3.5,	"03:30" },
	 { 3,	"03:00" },
	 { 2,	"02:00" },
	 { 4,	"04:00" },
	 { 6,	"06:00" },
	 { 1,	"01:00" },
	 { 3,	"03:00" },
	 { 0,	"??:??" },
	 { 0,	"??:??" },

	 //
	 // Episode Three Par Times
	 //
	 { 1.5,	"01:30" },
	 { 1.5,	"01:30" },
	 { 2.5,	"02:30" },
	 { 2.5,	"02:30" },
	 { 3.5,	"03:30" },
	 { 2.5,	"02:30" },
	 { 2,	"02:00" },
	 { 6,	"06:00" },
	 { 0,	"??:??" },
	 { 0,	"??:??" },

	 //
	 // Episode Four Par Times
	 //
	 { 2,	"02:00" },
	 { 2,	"02:00" },
	 { 1.5,	"01:30" },
	 { 1,	"01:00" },
	 { 4.5,	"04:30" },
	 { 3.5,	"03:30" },
	 { 2,	"02:00" },
	 { 4.5,	"04:30" },
	 { 0,	"??:??" },
	 { 0,	"??:??" },

	 //
	 // Episode Five Par Times
	 //
	 { 2.5,	"02:30" },
	 { 1.5,	"01:30" },
	 { 2.5,	"02:30" },
	 { 2.5,	"02:30" },
	 { 4,	"04:00" },
	 { 3,	"03:00" },
	 { 4.5,	"04:30" },
	 { 3.5,	"03:30" },
	 { 0,	"??:??" },
	 { 0,	"??:??" },

	 //
	 // Episode Six Par Times
	 //
	 { 6.5,	"06:30" },
	 { 4,	"04:00" },
	 { 4.5,	"04:30" },
	 { 6,	"06:00" },
	 { 5,	"05:00" },
	 { 5.5,	"05:30" },
	 { 5.5,	"05:30" },
	 { 8.5,	"08:30" },
	 { 0,	"??:??" },
	 { 0,	"??:??" }
};


/**
 * \brief Returns lump name string.
 * \param[in] id Chunk value to get string name for.
 * \return NULL on error, otherwise string name.
 */
PUBLIC char *WL1_10_GetReduxGFXFileName( const W32 id )
{
    switch( id )
    {
		case  3: return "H_BJPIC";
        case  4: return "H_CASTLEPIC";
		case  5: return "H_KEYBOARDPIC";
		case  6: return "H_CONTROLPIC";
		case  7: return "H_HEALPIC";
		case  8: return "H_BGPIC";
		case  9: return "H_GUNPIC";
		case 10: return "H_KEYPIC";
		case 11: return "H_BLAZEPIC";
		case 12: return "H_WEAPON1234PIC";
		case 13: return "H_WOLFLOGOPIC";
		case 14: return "H_VISAPIC";
		case 15: return "H_MCPIC";
		case 16: return "H_IDLOGOPIC";
		case 17: return "H_TOPWINDOWPI";
		case 18: return "H_LEFTWINDOWPIC";
		case 19: return "H_RIGHTWINDOWPIC";
		case 20: return "H_BOTTOMINFOPIC";

		case 21: return "C_OPTIONSPIC";
		case 22: return "C_CURSOR0PIC";
		case 23: return "C_CURSOR1PIC";
		case 24: return "C_NOTSELECTEDPIC";
		case 25: return "C_SELECTEDPIC";
		case 26: return "C_FXTITLEPIC";
		case 27: return "C_DIGITITLEPIC";
		case 28: return "C_MUSICTITLEPIC";
		case 29: return "C_MOUSELBACKPIC";
		case 30: return "C_SKILL1PIC";
		case 31: return "C_SKILL2PIC";
		case 32: return "C_SKILL3PIC";
		case 33: return "C_SKILL4PIC";
		case 34: return "C_LOADSAVEDISKPIC";
		case 35: return "C_DISKLOADING0PIC";
		case 36: return "C_DISKLOADING1PIC";
		case 37: return "C_CONTROLPIC";
		case 38: return "C_CUSTOMIZEPIC";
		case 39: return "C_LOADGAMEPIC";
		case 40: return "C_SAVEGAMEPIC";
		case 41: return "C_EPISODE1PIC";
		case 42: return "C_EPISODE2PIC";
		case 43: return "C_EPISODE3PIC";
		case 44: return "C_EPISODE4PIC";
		case 45: return "C_EPISODE5PIC";
		case 46: return "C_EPISODE6PIC";
		case 47: return "C_CODEPIC";		
		case 48: return "L_GUY0PIC";		
        
        case 49: 
        case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 61: 
		case 62: 
		case 63: 
		case 64: 
		case 65: 
		case 66: 
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86:
		case 87: return "L_FONTPIC";
             
		case 88: return "L_GUY1PIC";
		case 89: return "L_BJWINSPIC";
		case 90: return "STATUSBARPIC";
		case 91: return "TITLEPIC";
		case 92: return "PC13PIC";
		case 93: return "CREDITSPIC";
		case 94: return "HIGHSCORESPIC";
		case 95: return "KNIFEPIC";
		case 96: return "GUNPIC";
		case 97: return "MACHINEGUNPIC";
		case 98: return "GATLINGGUNPIC";
		case 99: return "99";
		case 100: return "GOLDKEYPIC";
		case 101: return "SILVERKEYPIC";
		case 102: return "102";
		
        case 103: 
		case 104: 
		case 105: 
		case 106: 
		case 107: 
		case 108: 
		case 109: 
		case 110:
		case 111:
		case 112: return "N_NUMPIC";
		        
		case 113: return "FACE1APIC";
		case 114: return "FACE1BPIC";
		case 115: return "FACE1CPIC";
		case 116: return "FACE2APIC";
		case 117: return "FACE2BPIC";
		case 118: return "FACE2CPIC";
		case 119: return "FACE3APIC";
		case 120: return "FACE3BPIC";
		case 121: return "FACE3CPIC";
		case 122: return "FACE4APIC";
		case 123: return "FACE4BPIC";
		case 124: return "FACE4CPIC";
		case 125: return "FACE5APIC";
		case 126: return "FACE5BPIC";
		case 127: return "FACE5CPIC";
		case 128: return "FACE6APIC";
		case 129: return "FACE6BPIC";
		case 130: return "FACE6CPIC";
		case 131: return "FACE7APIC";
		case 132: return "FACE7BPIC";
		case 133: return "FACE7CPIC";
		case 134: return "FACE8APIC";
		case 135: return "GOTGATLINGPIC";
		case 136: return "MUTANTBJPIC";
		case 137: return "PAUSEDPIC";
		case 138: return "GETPSYCHEDPIC";		

		default: return NULL;

    } /* End switch chunk */

}

/**
 * \brief Returns lump name string.
 * \param[in] id Chunk value to get string name for.
 * \return NULL on error, otherwise string name.
 */
PUBLIC char *WL1_11_GetReduxGFXFileName( const W32 id )
{
    switch( id )
    {
		case  3: return "H_BJPIC";
		case  4: return "H_CASTLEPIC";
		case  5: return "H_KEYBOARDPIC";
		case  6: return "H_CONTROLPIC";
		case  7: return "H_HEALPIC";
		case  8: return "H_BGPIC";
		case  9: return "H_GUNPIC";
		case 10: return "H_KEYPIC";
		case 11: return "H_BLAZEPIC";
		case 12: return "H_WEAPON1234PIC";
		case 13: return "H_WOLFLOGOPIC";
		case 14: return "H_VISAPIC";
		case 15: return "H_MCPIC";
		case 16: return "H_IDLOGOPIC";
		case 17: return "H_TOPWINDOWPI";
		case 18: return "H_LEFTWINDOWPIC";
		case 19: return "H_RIGHTWINDOWPIC";
		case 20: return "H_BOTTOMINFOPIC";

		case 21: return "C_OPTIONSPIC";
		case 22: return "C_CURSOR0PIC";
		case 23: return "C_CURSOR1PIC";
		case 24: return "C_NOTSELECTEDPIC";
		case 25: return "C_SELECTEDPIC";
		case 26: return "C_FXTITLEPIC";
		case 27: return "C_DIGITITLEPIC";
		case 28: return "C_MUSICTITLEPIC";
		case 29: return "C_MOUSELBACKPIC";
		case 30: return "C_SKILL1PIC";
		case 31: return "C_SKILL2PIC";
		case 32: return "C_SKILL3PIC";
		case 33: return "C_SKILL4PIC";
		case 34: return "C_LOADSAVEDISKPIC";
		case 35: return "C_DISKLOADING0PIC";
		case 36: return "C_DISKLOADING1PIC";
		case 37: return "C_CONTROLPIC";
		case 38: return "C_CUSTOMIZEPIC";
		case 39: return "C_LOADGAMEPIC";
		case 40: return "C_SAVEGAMEPIC";
		case 41: return "C_EPISODE1PIC";
		case 42: return "C_EPISODE2PIC";
		case 43: return "C_EPISODE3PIC";
		case 44: return "C_EPISODE4PIC";
		case 45: return "C_EPISODE5PIC";
		case 46: return "C_EPISODE6PIC";
		case 47: return "C_CODEPIC";
		case 48: return "C_TIMECODEPIC";
		case 49: return "C_LEVELPIC";		
		case 50: return "C_NAMEPIC";
		case 51: return "C_SCOREPIC";
		case 52: return "L_GUY0PIC";
        
		case 53: 
		case 54: 
		case 55: 
		case 56:
		case 57:
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
		case 63:
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
 		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86:
		case 87:              
		case 88:
		case 89:
		case 90:
		case 91:
		case 92: return "L_FONTPIC";

		case 93: return "L_GUY1PIC";
		case 94: return "L_BJWINSPIC";
		case 95: return "STATUSBARPIC";
		case 96: return "TITLEPIC";
		case 97: return "PC13PIC";
		case 98: return "CREDITSPIC";
		case 99: return "HIGHSCORESPIC";
		case 100: return "KNIFEPIC";
		case 101: return "GUNPIC";
		case 102: return "MACHINEGUNPIC";
		case 103: return "GATLINGGUNPIC";
		case 104: return "99";
		case 105: return "GOLDKEYPIC";
		case 106: return "SILVERKEYPIC";
		case 107: return "102";
		
		case 108: 
		case 109: 
		case 110: 
		case 111: 
		case 112: 
		case 113: 
		case 114: 
		case 115:
		case 116:
		case 117: return "N_NUMPIC";
		        
		case 118: return "FACE1APIC";
		case 119: return "FACE1BPIC";
		case 120: return "FACE1CPIC";
		case 121: return "FACE2APIC";
		case 122: return "FACE2BPIC";
		case 123: return "FACE2CPIC";
		case 124: return "FACE3APIC";
		case 125: return "FACE3BPIC";
		case 126: return "FACE3CPIC";
		case 127: return "FACE4APIC";
		case 128: return "FACE4BPIC";
		case 129: return "FACE4CPIC";
		case 130: return "FACE5APIC";
		case 131: return "FACE5BPIC";
		case 132: return "FACE5CPIC";
		case 133: return "FACE6APIC";
		case 134: return "FACE6BPIC";
		case 135: return "FACE6CPIC";
		case 136: return "FACE7APIC";
		case 137: return "FACE7BPIC";
		case 138: return "FACE7CPIC";
		case 139: return "FACE8APIC";
		case 140: return "GOTGATLINGPIC";
		case 141: return "MUTANTBJPIC";
		case 142: return "PAUSEDPIC";
		case 143: return "GETPSYCHEDPIC";		

		default: return NULL;

    } /* End switch chunk */

}


/**
 * \brief Returns lump name string.
 * \param[in] id Chunk value to get string name for.
 * \return NULL on error, otherwise string name.
 */
PUBLIC char *WL1_14_GetReduxGFXFileName( W32 id )
{
    switch( id )
    {
		case WL1_H_BJPIC: return "H_BJPIC";
		case WL1_H_CASTLEPIC: return "H_CASTLEPIC";
		case WL1_H_KEYBOARDPIC: return "H_KEYBOARDPIC";
		case WL1_H_CONTROLPIC: return "H_CONTROLPIC";
		case WL1_H_HEALPIC: return "H_HEALPIC";
		case WL1_H_BGPIC: return "H_BGPIC";
		case WL1_H_GUNPIC: return "H_GUNPIC";
		case WL1_H_KEYPIC: return "H_KEYPIC";
		case WL1_H_BLAZEPIC: return "H_BLAZEPIC";
		case WL1_H_WEAPON1234PIC: return "H_WEAPON1234PIC";
		case WL1_H_WOLFLOGOPIC: return "H_WOLFLOGOPIC";
		case WL1_H_VISAPIC: return "H_VISAPIC";
		case WL1_H_MCPIC: return "H_MCPIC";
		case WL1_H_IDLOGOPIC: return "H_IDLOGOPIC";
		case WL1_H_TOPWINDOWPIC: return "H_TOPWINDOWPI";
		case WL1_H_LEFTWINDOWPIC: return "H_LEFTWINDOWPIC";
		case WL1_H_RIGHTWINDOWPIC: return "H_RIGHTWINDOWPIC";
		case WL1_H_BOTTOMINFOPIC: return "H_BOTTOMINFOPIC";
		case WL1_H_GAMEPIC: return "H_GAMEPIC";

		case WL1_C_OPTIONSPIC: return "C_OPTIONSPIC";
		case WL1_C_CURSOR1PIC: return "C_CURSOR0PIC";
		case WL1_C_CURSOR2PIC: return "C_CURSOR1PIC";
		case WL1_C_NOTSELECTEDPIC: return "C_NOTSELECTEDPIC";
		case WL1_C_SELECTEDPIC: return "C_SELECTEDPIC";
		case WL1_C_FXTITLEPIC: return "C_FXTITLEPIC";
		case WL1_C_DIGITITLEPIC: return "C_DIGITITLEPIC";
		case WL1_C_MUSICTITLEPIC: return "C_MUSICTITLEPIC";
		case WL1_C_MOUSELBACKPIC: return "C_MOUSELBACKPIC";
		case WL1_C_BABYMODEPIC: return "C_SKILL1PIC";
		case WL1_C_EASYPIC: return "C_SKILL2PIC";
		case WL1_C_NORMALPIC: return "C_SKILL3PIC";
		case WL1_C_HARDPIC: return "C_SKILL4PIC";
		case WL1_C_LOADSAVEDISKPIC: return "C_LOADSAVEDISKPIC";
		case WL1_C_DISKLOADING1PIC: return "C_DISKLOADING0PIC";
		case WL1_C_DISKLOADING2PIC: return "C_DISKLOADING1PIC";
		case WL1_C_CONTROLPIC: return "C_CONTROLPIC";
		case WL1_C_CUSTOMIZEPIC: return "C_CUSTOMIZEPIC";
		case WL1_C_LOADGAMEPIC: return "C_LOADGAMEPIC";
		case WL1_C_SAVEGAMEPIC: return "C_SAVEGAMEPIC";
		case WL1_C_EPISODE1PIC: return "C_EPISODE1PIC";
		case WL1_C_EPISODE2PIC: return "C_EPISODE2PIC";
		case WL1_C_EPISODE3PIC: return "C_EPISODE3PIC";
		case WL1_C_EPISODE4PIC: return "C_EPISODE4PIC";
		case WL1_C_EPISODE5PIC: return "C_EPISODE5PIC";
		case WL1_C_EPISODE6PIC: return "C_EPISODE6PIC";
		case WL1_C_CODEPIC: return "C_CODEPIC";		
		case WL1_C_TIMECODEPIC: return "C_TIMECODEPIC";
		case WL1_C_LEVELPIC: return "C_LEVELPIC";
		case WL1_C_NAMEPIC: return "C_NAMEPIC";
		case WL1_C_SCOREPIC: return "C_SCOREPIC";
		case WL1_C_JOY1PIC: return "C_JOY1PIC";
		case WL1_C_JOY2PIC: return "C_JOY2PIC";

		case WL1_L_GUYPIC: return "L_GUY0PIC";
		case WL1_L_COLONPIC: 
		case WL1_L_NUM0PIC: 
		case WL1_L_NUM1PIC: 
		case WL1_L_NUM2PIC: 
		case WL1_L_NUM3PIC: 
		case WL1_L_NUM4PIC: 
		case WL1_L_NUM5PIC: 
		case WL1_L_NUM6PIC: 
		case WL1_L_NUM7PIC: 
		case WL1_L_NUM8PIC: 
		case WL1_L_NUM9PIC: 
		case WL1_L_PERCENTPIC: 
		case WL1_L_APIC:
		case WL1_L_BPIC:
		case WL1_L_CPIC:
		case WL1_L_DPIC:
		case WL1_L_EPIC:
		case WL1_L_FPIC:
		case WL1_L_GPIC:
		case WL1_L_HPIC:
		case WL1_L_IPIC:
		case WL1_L_JPIC:
		case WL1_L_KPIC:
		case WL1_L_LPIC:
		case WL1_L_MPIC:
		case WL1_L_NPIC:
		case WL1_L_OPIC:
		case WL1_L_PPIC:
		case WL1_L_QPIC:
		case WL1_L_RPIC:
		case WL1_L_SPIC:
		case WL1_L_TPIC:
		case WL1_L_UPIC:
		case WL1_L_VPIC:
		case WL1_L_WPIC:
		case WL1_L_XPIC:
		case WL1_L_YPIC:
		case WL1_L_ZPIC: 
		case WL1_L_EXPOINTPIC: 
		case WL1_L_APOSTROPHEPIC: return "L_FONTPIC";
		case WL1_L_GUY2PIC: return "L_GUY1PIC";
		case WL1_L_BJWINSPIC: return "L_BJWINSPIC";
		case WL1_STATUSBARPIC: return "STATUSBARPIC";
		case WL1_TITLEPIC: return "TITLEPIC";
		case WL1_PG13PIC: return "PC13PIC";
		case WL1_CREDITSPIC: return "CREDITSPIC";
		case WL1_HIGHSCORESPIC: return "HIGHSCORESPIC";

		case WL1_KNIFEPIC: return "KNIFEPIC";
		case WL1_GUNPIC: return "GUNPIC";
		case WL1_MACHINEGUNPIC: return "MACHINEGUNPIC";
		case WL1_GATLINGGUNPIC: return "GATLINGGUNPIC";
		case WL1_NOKEYPIC: return "NOKEYPIC";
		case WL1_GOLDKEYPIC: return "GOLDKEYPIC";
		case WL1_SILVERKEYPIC: return "SILVERKEYPIC";
		case WL1_N_BLANKPIC: return "N_BLANKPIC";
		case WL1_N_0PIC:
		case WL1_N_1PIC:
		case WL1_N_2PIC:
		case WL1_N_3PIC:
		case WL1_N_4PIC:
		case WL1_N_5PIC:
		case WL1_N_6PIC:
		case WL1_N_7PIC:
		case WL1_N_8PIC:
		case WL1_N_9PIC: return "N_NUMPIC";
		case WL1_FACE1APIC: return "FACE1APIC";
		case WL1_FACE1BPIC: return "FACE1BPIC";
		case WL1_FACE1CPIC: return "FACE1CPIC";
		case WL1_FACE2APIC: return "FACE2APIC";
		case WL1_FACE2BPIC: return "FACE2BPIC";
		case WL1_FACE2CPIC: return "FACE2CPIC";
		case WL1_FACE3APIC: return "FACE3APIC";
		case WL1_FACE3BPIC: return "FACE3BPIC";
		case WL1_FACE3CPIC: return "FACE3CPIC";
		case WL1_FACE4APIC: return "FACE4APIC";
		case WL1_FACE4BPIC: return "FACE4BPIC";
		case WL1_FACE4CPIC: return "FACE4CPIC";
		case WL1_FACE5APIC: return "FACE5APIC";
		case WL1_FACE5BPIC: return "FACE5BPIC";
		case WL1_FACE5CPIC: return "FACE5CPIC";
		case WL1_FACE6APIC: return "FACE6APIC";
		case WL1_FACE6BPIC: return "FACE6BPIC";
		case WL1_FACE6CPIC: return "FACE6CPIC";
		case WL1_FACE7APIC: return "FACE7APIC";
		case WL1_FACE7BPIC: return "FACE7BPIC";
		case WL1_FACE7CPIC: return "FACE7CPIC";
		case WL1_FACE8APIC: return "FACE8APIC";
		case WL1_GOTGATLINGPIC: return "GOTGATLINGPIC";
		case WL1_MUTANTBJPIC: return "MUTANTBJPIC";
		case WL1_PAUSEDPIC: return "PAUSEDPIC";
		case WL1_GETPSYCHEDPIC: return "GETPSYCHEDPIC";		

		default: return NULL;

    } /* End switch chunk */

}


/**
 * \brief Returns lump name string.
 * \param[in] id Chunk value to get string name for.
 * \return NULL on error, otherwise string name.
 */
PUBLIC char *WL6_GetReduxGFXFileName( W32 id )
{
    switch( id )
    {
		case WL6_H_BJPIC: return "H_BJPIC";
		case WL6_H_CASTLEPIC: return "H_CASTLEPIC";
		case WL6_H_BLAZEPIC: return "H_BLAZEPIC";
		case WL6_H_TOPWINDOWPIC: return "H_TOPWINDOWPIC";
		case WL6_H_LEFTWINDOWPIC: return "H_LEFTWINDOWPIC";
		case WL6_H_RIGHTWINDOWPIC: return "H_RIGHTWINDOWPIC";
		case WL6_H_BOTTOMINFOPIC: return "H_BOTTOMINFOPIC";
		case WL6_C_OPTIONSPIC: return "C_OPTIONSPIC";
		case WL6_C_CURSOR1PIC: return "C_CURSOR0PIC";
		case WL6_C_CURSOR2PIC: return "C_CURSOR1PIC";
		case WL6_C_NOTSELECTEDPIC: return "C_NOTSELECTEDPIC";
		case WL6_C_SELECTEDPIC: return "C_SELECTEDPIC";
		case WL6_C_FXTITLEPIC: return "C_FXTITLEPIC";
		case WL6_C_DIGITITLEPIC: return "C_DIGITITLEPIC";
		case WL6_C_MUSICTITLEPIC: return "C_MUSICTITLEPIC";
		case WL6_C_MOUSELBACKPIC: return "C_MOUSELBACKPIC";
		case WL6_C_BABYMODEPIC: return "C_SKILL1PIC";
		case WL6_C_EASYPIC: return "C_SKILL2PIC";
		case WL6_C_NORMALPIC: return "C_SKILL3PIC";
		case WL6_C_HARDPIC: return "C_SKILL4PIC";
		case WL6_C_LOADSAVEDISKPIC: return "C_LOADSAVEDISKPIC";
		case WL6_C_DISKLOADING1PIC: return "C_DISKLOADING0PIC";
		case WL6_C_DISKLOADING2PIC: return "C_DISKLOADING1PIC";
		case WL6_C_CONTROLPIC: return "C_CONTROLPIC";
		case WL6_C_CUSTOMIZEPIC: return "C_CUSTOMIZEPIC";
		case WL6_C_LOADGAMEPIC: return "C_LOADGAMEPIC";
		case WL6_C_SAVEGAMEPIC: return "C_SAVEGAMEPIC";
		case WL6_C_EPISODE1PIC: return "C_EPISODE1PIC";
		case WL6_C_EPISODE2PIC: return "C_EPISODE2PIC";
		case WL6_C_EPISODE3PIC: return "C_EPISODE3PIC";
		case WL6_C_EPISODE4PIC: return "C_EPISODE4PIC";
		case WL6_C_EPISODE5PIC: return "C_EPISODE5PIC";
		case WL6_C_EPISODE6PIC: return "C_EPISODE6PIC";
		case WL6_C_CODEPIC: return "C_CODEPIC";
		case WL6_C_TIMECODEPIC: return "C_TIMECODEPIC";
		case WL6_C_LEVELPIC: return "C_LEVELPIC";
		case WL6_C_NAMEPIC: return "C_NAMEPIC";
		case WL6_C_SCOREPIC: return "C_SCOREPIC";
		case WL6_C_JOY1PIC: return "C_JOY1PIC";
		case WL6_C_JOY2PIC: return "C_JOY2PIC";
		
		case WL6_L_GUYPIC: return "L_GUY0PIC";
		case WL6_L_COLONPIC: 
		case WL6_L_NUM0PIC: 
		case WL6_L_NUM1PIC: 
		case WL6_L_NUM2PIC: 
		case WL6_L_NUM3PIC: 
		case WL6_L_NUM4PIC: 
		case WL6_L_NUM5PIC: 
		case WL6_L_NUM6PIC: 
		case WL6_L_NUM7PIC: 
		case WL6_L_NUM8PIC: 
		case WL6_L_NUM9PIC: 
		case WL6_L_PERCENTPIC: 
		case WL6_L_APIC:
		case WL6_L_BPIC:
		case WL6_L_CPIC:
		case WL6_L_DPIC:
		case WL6_L_EPIC:
		case WL6_L_FPIC:
		case WL6_L_GPIC:
		case WL6_L_HPIC:
		case WL6_L_IPIC:
		case WL6_L_JPIC:
		case WL6_L_KPIC:
		case WL6_L_LPIC:
		case WL6_L_MPIC:
		case WL6_L_NPIC:
		case WL6_L_OPIC:
		case WL6_L_PPIC:
		case WL6_L_QPIC:
		case WL6_L_RPIC:
		case WL6_L_SPIC:
		case WL6_L_TPIC:
		case WL6_L_UPIC:
		case WL6_L_VPIC:
		case WL6_L_WPIC:
		case WL6_L_XPIC:
		case WL6_L_YPIC:
		case WL6_L_ZPIC: 
		case WL6_L_EXPOINTPIC: 
		case WL6_L_APOSTROPHEPIC: return "L_FONTPIC";
		case WL6_L_GUY2PIC: return "L_GUY1PIC";
		case WL6_L_BJWINSPIC: return "L_BJWINSPIC";
		case WL6_STATUSBARPIC: return "STATUSBARPIC";
		case WL6_TITLEPIC: return "TITLEPIC";
		case WL6_PG13PIC: return "PC13PIC";
		case WL6_CREDITSPIC: return "CREDITSPIC";
		case WL6_HIGHSCORESPIC: return "HIGHSCORESPIC";
		case WL6_KNIFEPIC: return "KNIFEPIC";
		case WL6_GUNPIC: return "GUNPIC";
		case WL6_MACHINEGUNPIC: return "MACHINEGUNPIC";
		case WL6_GATLINGGUNPIC: return "GATLINGGUNPIC";
		case WL6_NOKEYPIC: return "NOKEYPIC";
		case WL6_GOLDKEYPIC: return "GOLDKEYPIC";
		case WL6_SILVERKEYPIC: return "SILVERKEYPIC";
		case WL6_N_BLANKPIC: return "N_BLANKPIC";
		case WL6_N_0PIC:
		case WL6_N_1PIC:
		case WL6_N_2PIC:
		case WL6_N_3PIC:
		case WL6_N_4PIC:
		case WL6_N_5PIC:
		case WL6_N_6PIC:
		case WL6_N_7PIC:
		case WL6_N_8PIC:
		case WL6_N_9PIC: return "N_NUMPIC";
		case WL6_FACE1APIC: return "FACE1APIC";
		case WL6_FACE1BPIC: return "FACE1BPIC";
		case WL6_FACE1CPIC: return "FACE1CPIC";
		case WL6_FACE2APIC: return "FACE2APIC";
		case WL6_FACE2BPIC: return "FACE2BPIC";
		case WL6_FACE2CPIC: return "FACE2CPIC";
		case WL6_FACE3APIC: return "FACE3APIC";
		case WL6_FACE3BPIC: return "FACE3BPIC";
		case WL6_FACE3CPIC: return "FACE3CPIC";
		case WL6_FACE4APIC: return "FACE4APIC";
		case WL6_FACE4BPIC: return "FACE4BPIC";
		case WL6_FACE4CPIC: return "FACE4CPIC";
		case WL6_FACE5APIC: return "FACE5APIC";
		case WL6_FACE5BPIC: return "FACE5BPIC";
		case WL6_FACE5CPIC: return "FACE5CPIC";
		case WL6_FACE6APIC: return "FACE6APIC";
		case WL6_FACE6BPIC: return "FACE6BPIC";
		case WL6_FACE6CPIC: return "FACE6CPIC";
		case WL6_FACE7APIC: return "FACE7APIC";
		case WL6_FACE7BPIC: return "FACE7BPIC";
		case WL6_FACE7CPIC: return "FACE7CPIC";
		case WL6_FACE8APIC: return "FACE8APIC";
		case WL6_GOTGATLINGPIC: return "GOTGATLINGPIC";
		case WL6_MUTANTBJPIC: return "MUTANTBJPIC";
		case WL6_PAUSEDPIC: return "PAUSEDPIC";
		case WL6_GETPSYCHEDPIC: return "GETPSYCHEDPIC";
		
		default: return NULL;

    } /* End switch chunk */
}


PUBLIC char *WL1_GetReduxGFXFileName( const W32 picId )
{

    switch( wolf_version )
    {
        case WL1_V10:
            return WL1_10_GetReduxGFXFileName( picId );

        case WL1_V11:
            return WL1_11_GetReduxGFXFileName( picId );

        case WL1_V14:
            return WL1_14_GetReduxGFXFileName( picId );
    }

    //Should never get here
    return "Please define version in WL1_GetReduxGFXFileName";

}

/**
 * \brief Converts Wolf3D graphic data.
 * \param[in] dict Name of dictionary file to load.
 * \param[in] head Name of header file to load. 
 * \param[in] graph Name of graphics file to load. 
 * \param[in] start Offset to start of sprite data
 * \param[in] end Offset to end of sprite data
 * \param[in] picNum Pointer to picNum_t structure to 
 * \param[in] GetReduxGFXFileName Call back function to get file names for graphics data.
 * \return On success true, otherwise false.
 */
PRIVATE wtBoolean wolf3d_gfx( char *dict, char *head, char *graph, W32 start, W32 end, picNum_t *picNum,
							 char *(*GetReduxGFXFileName)( W32 ) )
{
	wtBoolean bRetVal;
	W32 width, height;
	void *data;
	W32 i;
	char tempFileName[ 1024 ];


    printf( "\nDecoding GFX..." );
	
		
	bRetVal = GFXFile_Setup( dict, head, graph );
    if( ! bRetVal )
	{		
        printf( "Failed\n" );
        return false;
    }

	for( i = 1; i < start; ++i )
	{			
		GFXFile_decodeFont( i, 256, 128, DIR_PICS );
	}

    // Create directory for help scripts   "gscripts/wolfhelp%.3d.txt"
	GFXFile_decodeScript( picNum->PN_HelpScript, picNum->PN_HelpScript+1, DIR_GSCRIPTS );
	GFXFile_decodeScript( picNum->PN_ScriptStart, picNum->PN_ScriptEnd, DIR_GSCRIPTS );


	for( i = start ; i < end ; ++i )
	{
		GFXFile_cacheChunk( i );
        data = GFXFile_decodeChunk_RGB32( i, &width, &height, wolf_gamepal );
		if( NULL == data )
		{
            continue;
        }

        if( _doRedux )
        {
        	W32 id;
        	void *updata;
        	
        	id = 0;
            updata = wolfcore_ReduxGFX( i, data, &width, &height, &id, wolf_gamepal, picNum );
            if( updata == NULL )
            {
            	MM_FREE( data );
            	
            	continue;
            }
            
            wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%s.tga", DIR_PICS, PATH_SEP, GetReduxGFXFileName( i ) );
			
            RGB32toRGB24( updata, updata, width * height * 4 );
			TGA_write( tempFileName, 24, width, height, updata, 0, 1 );
		
			 // updata and data could point to the same memory block.
            if( updata == data)
            {
	            MM_FREE( data );
            }
            else
            {
                MM_FREE( data );
                MM_FREE( updata );
            }
			
			i += id;                        
        }
		else
		{		
            wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, i );
			
            RGB32toRGB24( data, data, width * height * 4 ); 
			TGA_write( tempFileName, 24, width, height, data, 0, 1 );
		
			MM_FREE( data );
		}
				
	}

	GFXFile_Shutdown();

    printf( "Done\n" );
	
	return bRetVal;
}

/**
 * \brief Decodes Wolfenstein 3-D full version. 
 * \return Nothing.
 */
PUBLIC void wolffull_decoder( void )
{
	picNum_t picNum;
	W32 retCheck = 0;
		
	printf( "Wolfenstein 3-D Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }

    picNum.PN_StatusBar = WL6_STATUSBARPIC;
    picNum.PN_NoKey = WL6_NOKEYPIC;
    picNum.PN_Blank = WL6_N_BLANKPIC;
    picNum.PN_Title1 = 999;
    picNum.PN_Title2 = 999;
    picNum.PN_TitlePalette = 999;
    picNum.PN_0 = WL6_N_0PIC;
    picNum.PN_1 = WL6_N_1PIC;
    picNum.PN_9 = WL6_N_9PIC;
    picNum.PN_Colon = WL6_L_COLONPIC;
    picNum.PN_Num0 = WL6_L_NUM0PIC;
    picNum.PN_Num9 = WL6_L_NUM9PIC;
    picNum.PN_Percent = WL6_L_PERCENTPIC;
    picNum.PN_A = WL6_L_APIC;
    picNum.PN_Z = WL6_L_ZPIC;
    picNum.PN_Expoint = WL6_L_EXPOINTPIC;
    picNum.PN_Apostrophe = WL6_L_APOSTROPHEPIC;
    picNum.PN_IDGuys1 = 999;
    picNum.PN_IDGuys2 = 999;
    picNum.PN_EndScreen1 = 999;
    picNum.PN_EndScreen9 = 999;
	picNum.PN_HelpScript = 138;
	picNum.PN_ScriptStart = 143;
    picNum.PN_ScriptEnd = 149;
    picNum.PN_bottomInfoPic = WL6_H_BOTTOMINFOPIC;
		

	retCheck += wolf3d_gfx( "VGADICT.WL6", "VGAHEAD.WL6", "VGAGRAPH.WL6", 3, 135, &picNum, WL6_GetReduxGFXFileName );
		

	retCheck += PageFile_ReduxDecodePageData( "VSWAP.WL6", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, wolf_gamepal );


    if( _doRedux )
    {
    	retCheck += MapFile_ReduxDecodeMapData( "MAPHEAD.WL6", "GAMEMAPS.WL6", DIR_MAPS, wolf_gamepal, CeilingColourWL6, WL6_songs, parTimesWL6, "%s/w%.2d.map" );  
    }

	if( AudioFile_Setup( "AUDIOHED.WL6", "AUDIOT.WL6" ) )
	{
		retCheck += AudioFile_ReduxDecodeSound( 87, 174, DIR_SOUNDFX );

		retCheck += AudioFile_ReduxDecodeMusic( 261, 261+27, DIR_MUSIC, songTitles );
	}
	AudioFile_Shutdown();
	

    if( retCheck == 5  && ! _outputInDirectory )
    {
    	PAK_builder( "wolf.pak", 0, true );
    }


}

/**
 * \brief Decodes Wolfenstein 3-D Shareware versions 1.0 to 1.4
 * \return Nothing.
 */
void wolfshare_decoder( void )
{
	picNum_t picNum;
	W32 retCheck = 0;
    W32 pic_end = 147;
    char pakName[64];

    // Following values are for share V14
    W32 soundChunkStart = 87;
    W32 soundChunkEnd = 174;
    W32 musicChunkStart = 261;
    W32 musicChunkEnd = 261+27;

    W32 __WL1_L_COLONPIC = WL1_L_COLONPIC;
    W32 __WL1_L_NUM0PIC = WL1_L_NUM0PIC;
    W32 __WL1_L_NUM9PIC = WL1_L_NUM9PIC;
    W32 __WL1_L_PERCENTPIC = WL1_L_PERCENTPIC;
    W32 __WL1_L_APIC = WL1_L_APIC;
    W32 __WL1_L_ZPIC = WL1_L_ZPIC;
    W32 __WL1_L_EXPOINTPIC = WL1_L_EXPOINTPIC;
    W32 __WL1_L_APOSTROPHEPIC = WL1_L_APOSTROPHEPIC;
    W32 __WL1_STATUSBARPIC = WL1_STATUSBARPIC;
    W32 __WL1_NOKEYPIC = WL1_NOKEYPIC;
    W32 __WL1_N_BLANKPIC = WL1_N_BLANKPIC;
    W32 __WL1_N_0PIC = WL1_N_0PIC;
    W32 __WL1_N_1PIC = WL1_N_1PIC;
    W32 __WL1_N_9PIC = WL1_N_9PIC;
    W32 __WL1_H_BOTTOMINFOPIC = WL1_H_BOTTOMINFOPIC;

        
	printf( "Wolfenstein 3-D Shareware" );
    if( wolf_version == WL1_V10 )
    {
        printf( " V1.0" );


        wt_snprintf( pakName, sizeof(pakName), "%s", "wolf_shareV10.pak" );
        
        pic_end = 139;

        soundChunkStart = 87;
        soundChunkEnd = 146;
        musicChunkStart = 219;
        musicChunkEnd = 219+27;

         __WL1_L_COLONPIC = 49;
         __WL1_L_NUM0PIC = 50;
         __WL1_L_NUM9PIC = 59;
         __WL1_L_PERCENTPIC = 60;
         __WL1_L_APIC = 61;
         __WL1_L_ZPIC = 86;
         __WL1_L_EXPOINTPIC = 87;
         __WL1_L_APOSTROPHEPIC = 87; // none

         __WL1_STATUSBARPIC = 90;
		 __WL1_NOKEYPIC = 99;
         __WL1_N_BLANKPIC = 102;

         __WL1_N_0PIC = 103;
         __WL1_N_1PIC = 104;
         __WL1_N_9PIC = 112;

         __WL1_H_BOTTOMINFOPIC = 20;
    }
    if( wolf_version == WL1_V11 )
    {
        printf( " V1.1" );

        wt_snprintf( pakName, sizeof(pakName), "%s", "wolf_shareV11.pak" );

        pic_end = 141;

         __WL1_L_COLONPIC = 53;
         __WL1_L_NUM0PIC = 54;
         __WL1_L_NUM9PIC = 63;
         __WL1_L_PERCENTPIC = 64;
         __WL1_L_APIC = 65;
         __WL1_L_ZPIC = 90;
         __WL1_L_EXPOINTPIC = 91;
         __WL1_L_APOSTROPHEPIC = 92; 

         __WL1_STATUSBARPIC = 95;
		 __WL1_NOKEYPIC = 104;
         __WL1_N_BLANKPIC = 107;

         __WL1_N_0PIC = 108;
         __WL1_N_1PIC = 109;
         __WL1_N_9PIC = 117;

         __WL1_H_BOTTOMINFOPIC = 20;
    }
    if( wolf_version == WL1_V14 )
    {
        printf( " V1.4" );

        wt_snprintf( pakName, sizeof(pakName), "%s", "wolf_shareV14.pak" );
    }
    printf( " Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }

	picNum.PN_StatusBar = __WL1_STATUSBARPIC;
    picNum.PN_NoKey = __WL1_NOKEYPIC;
    picNum.PN_Blank = __WL1_N_BLANKPIC;
    picNum.PN_Title1 = 999;
    picNum.PN_Title2 = 999;
    picNum.PN_TitlePalette = 999;
    picNum.PN_0 = __WL1_N_0PIC;
    picNum.PN_1 = __WL1_N_1PIC;
    picNum.PN_9 = __WL1_N_9PIC;
    picNum.PN_Colon = __WL1_L_COLONPIC;
    picNum.PN_Num0 = __WL1_L_NUM0PIC;
    picNum.PN_Num9 = __WL1_L_NUM9PIC;
    picNum.PN_Percent = __WL1_L_PERCENTPIC;
    picNum.PN_A = __WL1_L_APIC;
    picNum.PN_Z = __WL1_L_ZPIC;
    picNum.PN_Expoint = __WL1_L_EXPOINTPIC;
    picNum.PN_Apostrophe = __WL1_L_APOSTROPHEPIC;
    picNum.PN_IDGuys1 = 999;
    picNum.PN_IDGuys2 = 999;
    picNum.PN_EndScreen1 = 999;
    picNum.PN_EndScreen9 = 999;
	picNum.PN_HelpScript = 0;
	picNum.PN_ScriptStart = 0;
    picNum.PN_ScriptEnd = 0;
    picNum.PN_bottomInfoPic = __WL1_H_BOTTOMINFOPIC;
    
    
    
	retCheck += wolf3d_gfx( "VGADICT.WL1", "VGAHEAD.WL1", "VGAGRAPH.WL1", 3, pic_end, &picNum, WL1_GetReduxGFXFileName );	

	retCheck += PageFile_ReduxDecodePageData( "VSWAP.WL1", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, wolf_gamepal );
    
    if( _doRedux )
    {
    	retCheck += MapFile_ReduxDecodeMapData( "MAPHEAD.WL1",  wolf_version == WL1_V10 ? "MAPTEMP.WL1" : "GAMEMAPS.WL1", DIR_MAPS, wolf_gamepal, CeilingColourWL6, WL6_songs, parTimesWL6, "%s/w%.2d.map" );  
    }

	if( AudioFile_Setup( "AUDIOHED.WL1", "AUDIOT.WL1" ) )
	{
		retCheck += AudioFile_ReduxDecodeSound( soundChunkStart, soundChunkEnd, DIR_SOUNDFX );
        
		retCheck += AudioFile_ReduxDecodeMusic( musicChunkStart, musicChunkEnd, DIR_MUSIC, songTitles );
	}
	AudioFile_Shutdown();
	

    if( retCheck == 5 && ! _outputInDirectory )
    {
    	PAK_builder( pakName, 0, true );
    }

}


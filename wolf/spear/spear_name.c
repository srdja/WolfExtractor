/*

	Copyright (C) 2006 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file spear_name.c
 * \brief Redux Spear of Destiny string names.
 * \author Michael Liebscher 
 * \date 2006
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#include "spear_def.h"

#include "../../common/common_utils.h"


/**
 * \brief Returns the graphic chunk name as a string value.
 * \param[in] id Chunk value to get string value of.
 * \return NULL on error, otherwise string name.
 */
PUBLIC char *SDM_GetReduxGFXFileName( W32 id )
{
    switch( id )
    {
        case SDM_C_BACKDROPPIC:		return "C_BACKDROPPIC";
		case SDM_C_MOUSELBACKPIC:	return "SC_MOUSELBACKPIC";
		case SDM_C_CURSOR1PIC:		return "SC_CURSOR0PIC";
		case SDM_C_CURSOR2PIC:		return "SC_CURSOR1PIC";
		case SDM_C_NOTSELECTEDPIC:	return "SC_NOTSELECTEDPIC";
		case SDM_C_SELECTEDPIC:		return "SC_SELECTEDPIC";

		case SDM_C_CUSTOMIZEPIC:	return "SC_CUSTOMIZEPIC";
		case SDM_C_JOY1PIC:			return "C_JOY1PIC";
		case SDM_C_JOY2PIC:			return "C_JOY2PIC";
		case SDM_C_MOUSEPIC:		return "C_MOUSEPIC";
		case SDM_C_JOYSTICKPIC:		return "C_JOYSTICKPIC";
		case SDM_C_KEYBOARDPIC:		return "C_KEYBOARDPIC";
		case SDM_C_CONTROLPIC:		return "SC_CONTROLPIC";

		case SDM_C_OPTIONSPIC:		return "SC_OPTIONSPIC";

		case SDM_C_FXTITLEPIC:		return "SC_FXTITLEPIC";
		case SDM_C_DIGITITLEPIC:	return "SC_DIGITITLEPIC";
		case SDM_C_MUSICTITLEPIC:	return "SC_MUSICTITLEPIC";

		case SDM_C_HOWTOUGHPIC:		return "C_HOWTOUGHPIC";
		case SDM_C_BABYMODEPIC:		return "SC_SKILL1PIC";
		case SDM_C_EASYPIC:			return "SC_SKILL2PIC";
		case SDM_C_NORMALPIC:		return "SC_SKILL3PIC";
		case SDM_C_HARDPIC:			return "SC_SKILL4PIC";

		case SDM_C_DISKLOADING1PIC: return "C_DISKLOADING0PIC";
		case SDM_C_DISKLOADING2PIC: return "C_DISKLOADING1PIC";
		case SDM_C_LOADGAMEPIC:		return "SC_LOADGAMEPIC";
		case SDM_C_SAVEGAMEPIC:		return "SC_SAVEGAMEPIC";

		case SDM_HIGHSCORESPIC:		return "SHIGHSCORESPIC";
		case SDM_C_WONSPEARPIC:		return "C_WONSPEARPIC";

		case SDM_L_GUYPIC:			return "L_GUY0PIC";
		case SDM_L_COLONPIC:
		case SDM_L_NUM0PIC: 
		case SDM_L_NUM1PIC: 
		case SDM_L_NUM2PIC: 
		case SDM_L_NUM3PIC: 
		case SDM_L_NUM4PIC: 
		case SDM_L_NUM5PIC: 
		case SDM_L_NUM6PIC: 
		case SDM_L_NUM7PIC: 
		case SDM_L_NUM8PIC: 
		case SDM_L_NUM9PIC:
		case SDM_L_PERCENTPIC:
		case SDM_L_APIC:
		case SDM_L_BPIC:
		case SDM_L_CPIC:
		case SDM_L_DPIC:
		case SDM_L_EPIC:
		case SDM_L_FPIC:
		case SDM_L_GPIC:
		case SDM_L_HPIC:
		case SDM_L_IPIC:
		case SDM_L_JPIC:
		case SDM_L_KPIC:
		case SDM_L_LPIC:
		case SDM_L_MPIC:
		case SDM_L_NPIC:
		case SDM_L_OPIC:
		case SDM_L_PPIC:
		case SDM_L_QPIC:
		case SDM_L_RPIC:
		case SDM_L_SPIC:
		case SDM_L_TPIC:
		case SDM_L_UPIC:
		case SDM_L_VPIC:
		case SDM_L_WPIC:
		case SDM_L_XPIC:
		case SDM_L_YPIC:
		case SDM_L_ZPIC: 
		case SDM_L_EXPOINTPIC:
		case SDM_L_APOSTROPHEPIC:		return "L_FONTPIC";
		case SDM_L_GUY2PIC:				return "L_GUY1PIC";
		case SDM_L_BJWINSPIC:			return "L_BJWINSPIC";

		case SDM_TITLE1PIC: 
		case SDM_TITLE2PIC:				return "STITLEPIC";		
		case SDM_STATUSBARPIC:			return "STATUSBARPIC";
		case SDM_PG13PIC:				return "PC13PIC";
		case SDM_CREDITSPIC:			return "SCREDITSPIC";

		case SDM_KNIFEPIC:				return "KNIFEPIC";
		case SDM_GUNPIC:				return "GUNPIC";
		case SDM_MACHINEGUNPIC:			return "MACHINEGUNPIC";
		case SDM_GATLINGGUNPIC:			return "GATLINGGUNPIC";
		case SDM_NOKEYPIC:				return "NOKEYPIC";
		case SDM_GOLDKEYPIC:			return "GOLDKEYPIC";
		case SDM_SILVERKEYPIC:			return "SILVERKEYPIC";
		case SDM_N_BLANKPIC:			return "N_BLANKPIC";
		case SDM_N_0PIC:
		case SDM_N_1PIC:
		case SDM_N_2PIC:
		case SDM_N_3PIC:
		case SDM_N_4PIC:
		case SDM_N_5PIC:
		case SDM_N_6PIC:
		case SDM_N_7PIC:
		case SDM_N_8PIC:
		case SDM_N_9PIC:			return "N_NUMPIC";
		case SDM_FACE1APIC:			return "FACE1APIC";
		case SDM_FACE1BPIC:			return "FACE1BPIC";
		case SDM_FACE1CPIC:			return "FACE1CPIC";
		case SDM_FACE2APIC:			return "FACE2APIC";
		case SDM_FACE2BPIC:			return "FACE2BPIC";
		case SDM_FACE2CPIC:			return "FACE2CPIC";
		case SDM_FACE3APIC:			return "FACE3APIC";
		case SDM_FACE3BPIC:			return "FACE3BPIC";
		case SDM_FACE3CPIC:			return "FACE3CPIC";
		case SDM_FACE4APIC:			return "FACE4APIC";
		case SDM_FACE4BPIC:			return "FACE4BPIC";
		case SDM_FACE4CPIC:			return "FACE4CPIC";
		case SDM_FACE5APIC:			return "FACE5APIC";
		case SDM_FACE5BPIC:			return "FACE5BPIC";
		case SDM_FACE5CPIC:			return "FACE5CPIC";
		case SDM_FACE6APIC:			return "FACE6APIC";
		case SDM_FACE6BPIC:			return "FACE6BPIC";
		case SDM_FACE6CPIC:			return "FACE6CPIC";
		case SDM_FACE7APIC:			return "FACE7APIC";
		case SDM_FACE7BPIC:			return "FACE7BPIC";
		case SDM_FACE7CPIC:			return "FACE7CPIC";
		case SDM_FACE8APIC:			return "FACE8APIC";
		case SDM_GOTGATLINGPIC:		return "GOTGATLINGPIC";
		case SDM_GODMODEFACE1PIC:	return "GODMODEFACE0PIC";
		case SDM_GODMODEFACE2PIC:	return "GODMODEFACE1PIC";
		case SDM_GODMODEFACE3PIC:	return "GODMODEFACE2PIC";
		case SDM_BJWAITING1PIC:		return "BJWAITING0PIC";
		case SDM_BJWAITING2PIC:		return "BJWAITING1PIC";
		case SDM_BJOUCHPIC:			return "BJOUCHPIC";
		case SDM_PAUSEDPIC:			return "PAUSEDPIC";
		case SDM_GETPSYCHEDPIC:		return "GETPSYCHEDPIC";
		
        default: return NULL;
    
	} // End switch id

	return NULL;
}


/**
 * \brief Returns the graphic chunk name as a string value.
 * \param[in] id Chunk value to get string value of.
 * \return NULL on error, otherwise string name.
 */
PUBLIC char *SOD_GetReduxGFXFileName( W32 id )
{
    switch( id )
    {
        case SOD_C_BACKDROPPIC:			return "C_BACKDROPPIC";
		case SOD_C_MOUSELBACKPIC:		return "SC_MOUSELBACKPIC";
		case SOD_C_CURSOR1PIC:			return "SC_CURSOR0PIC";
		case SOD_C_CURSOR2PIC:			return "SC_CURSOR1PIC";
		case SOD_C_NOTSELECTEDPIC:		return "SC_NOTSELECTEDPIC";
		case SOD_C_SELECTEDPIC:			return "SC_SELECTEDPIC";
		case SOD_C_CUSTOMIZEPIC:		return "SC_CUSTOMIZEPIC";
		case SOD_C_JOY1PIC:				return "C_JOY1PIC";
		case SOD_C_JOY2PIC:				return "C_JOY2PIC";
		case SOD_C_MOUSEPIC:			return "C_MOUSEPIC";
		case SOD_C_JOYSTICKPIC:			return "C_JOYSTICKPIC";
		case SOD_C_KEYBOARDPIC:			return "C_KEYBOARDPIC";
		case SOD_C_CONTROLPIC:			return "SC_CONTROLPIC";
		case SOD_C_OPTIONSPIC:			return "SC_OPTIONSPIC";
		case SOD_C_FXTITLEPIC:			return "SC_FXTITLEPIC";
		case SOD_C_DIGITITLEPIC:		return "SC_DIGITITLEPIC";
		case SOD_C_MUSICTITLEPIC:		return "SC_MUSICTITLEPIC";
		case SOD_C_HOWTOUGHPIC:			return "C_HOWTOUGHPIC";
		case SOD_C_BABYMODEPIC:			return "SC_SKILL1PIC";
		case SOD_C_EASYPIC:				return "SC_SKILL2PIC";
		case SOD_C_NORMALPIC:			return "SC_SKILL3PIC";
		case SOD_C_HARDPIC:				return "SC_SKILL4PIC";
		case SOD_C_DISKLOADING1PIC:		return "C_DISKLOADING0PIC";
		case SOD_C_DISKLOADING2PIC:		return "C_DISKLOADING1PIC";
		case SOD_C_LOADGAMEPIC:			return "SC_LOADGAMEPIC";
		case SOD_C_SAVEGAMEPIC:			return "SC_SAVEGAMEPIC";
		case SOD_HIGHSCORESPIC:			return "SHIGHSCORESPIC";
		case SOD_C_WONSPEARPIC:			return "C_WONSPEARPIC";
		case SOD_BJCOLLAPSE1PIC:		return "BJCOLLAPSE1PIC";
		case SOD_BJCOLLAPSE2PIC:		return "BJCOLLAPSE2PIC";
		case SOD_BJCOLLAPSE3PIC:		return "BJCOLLAPSE3PIC";
		case SOD_BJCOLLAPSE4PIC:		return "BJCOLLAPSE4PIC";
		case SOD_ENDPICPIC:				return "ENDPICPIC";
		case SOD_L_GUYPIC:				return "L_GUY0PIC";
		case SOD_L_COLONPIC:
		case SOD_L_NUM0PIC: 
		case SOD_L_NUM1PIC: 
		case SOD_L_NUM2PIC: 
		case SOD_L_NUM3PIC: 
		case SOD_L_NUM4PIC: 
		case SOD_L_NUM5PIC: 
		case SOD_L_NUM6PIC: 
		case SOD_L_NUM7PIC: 
		case SOD_L_NUM8PIC: 
		case SOD_L_NUM9PIC:
		case SOD_L_PERCENTPIC:
		case SOD_L_APIC:
		case SOD_L_BPIC:
		case SOD_L_CPIC:
		case SOD_L_DPIC:
		case SOD_L_EPIC:
		case SOD_L_FPIC:
		case SOD_L_GPIC:
		case SOD_L_HPIC:
		case SOD_L_IPIC:
		case SOD_L_JPIC:
		case SOD_L_KPIC:
		case SOD_L_LPIC:
		case SOD_L_MPIC:
		case SOD_L_NPIC:
		case SOD_L_OPIC:
		case SOD_L_PPIC:
		case SOD_L_QPIC:
		case SOD_L_RPIC:
		case SOD_L_SPIC:
		case SOD_L_TPIC:
		case SOD_L_UPIC:
		case SOD_L_VPIC:
		case SOD_L_WPIC:
		case SOD_L_XPIC:
		case SOD_L_YPIC:
		case SOD_L_ZPIC: 
		case SOD_L_EXPOINTPIC:
		case SOD_L_APOSTROPHEPIC:		return "L_FONTPIC";
		case SOD_L_GUY2PIC:				return "L_GUY1PIC";
		case SOD_L_BJWINSPIC:			return "L_BJWINSPIC";
		case SOD_TITLE1PIC: 
		case SOD_TITLE2PIC:				return "STITLEPIC";
		case SOD_ENDSCREEN1PIC:	    	return "ENDSCREEN1PIC";
		case SOD_ENDSCREEN2PIC: 		return "ENDSCREEN2PIC";
		case SOD_ENDSCREEN3PIC:			return "ENDSCREEN3PIC";
		case SOD_ENDSCREEN4PIC:			return "ENDSCREEN4PIC";
		case SOD_ENDSCREEN5PIC:			return "ENDSCREEN5PIC";
		case SOD_ENDSCREEN6PIC:			return "ENDSCREEN6PIC";
		case SOD_ENDSCREEN7PIC:			return "ENDSCREEN7PIC";
		case SOD_ENDSCREEN8PIC:			return "ENDSCREEN8PIC";
		case SOD_ENDSCREEN9PIC:			return "ENDSCREEN9PIC";
		case SOD_STATUSBARPIC:			return "STATUSBARPIC";
		case SOD_PG13PIC:				return "PC13PIC";
		case SOD_CREDITSPIC:			return "SCREDITSPIC";
		case SOD_IDGUYS1PIC: 
		case SOD_IDGUYS2PIC:			return "IDGUYSPIC";
		case SOD_COPYPROTTOPPIC:		return "COPYPROTTOPPIC";
		case SOD_COPYPROTBOXPIC:		return "COPYPROTBOXPIC";
		case SOD_BOSSPIC1PIC:			return "BOSSPIC1PIC";
		case SOD_BOSSPIC2PIC:			return "BOSSPIC2PIC";
		case SOD_BOSSPIC3PIC:			return "BOSSPIC3PIC";
		case SOD_BOSSPIC4PIC:			return "BOSSPIC4PIC";
		case SOD_KNIFEPIC:				return "KNIFEPIC";
		case SOD_GUNPIC:				return "GUNPIC";
		case SOD_MACHINEGUNPIC:			return "MACHINEGUNPIC";
		case SOD_GATLINGGUNPIC:			return "GATLINGGUNPIC";
		case SOD_NOKEYPIC:				return "NOKEYPIC";
		case SOD_GOLDKEYPIC:			return "GOLDKEYPIC";
		case SOD_SILVERKEYPIC:			return "SILVERKEYPIC";
		case SOD_N_BLANKPIC:			return "N_BLANKPIC";
		case SOD_N_0PIC:
		case SOD_N_1PIC:
		case SOD_N_2PIC:
		case SOD_N_3PIC:
		case SOD_N_4PIC:
		case SOD_N_5PIC:
		case SOD_N_6PIC:
		case SOD_N_7PIC:
		case SOD_N_8PIC:
		case SOD_N_9PIC:				return "N_NUMPIC";
		case SOD_FACE1APIC:				return "FACE1APIC";
		case SOD_FACE1BPIC:				return "FACE1BPIC";
		case SOD_FACE1CPIC:				return "FACE1CPIC";
		case SOD_FACE2APIC:				return "FACE2APIC";
		case SOD_FACE2BPIC:				return "FACE2BPIC";
		case SOD_FACE2CPIC:				return "FACE2CPIC";
		case SOD_FACE3APIC:				return "FACE3APIC";
		case SOD_FACE3BPIC:				return "FACE3BPIC";
		case SOD_FACE3CPIC:				return "FACE3CPIC";
		case SOD_FACE4APIC:				return "FACE4APIC";
		case SOD_FACE4BPIC:				return "FACE4BPIC";
		case SOD_FACE4CPIC:				return "FACE4CPIC";
		case SOD_FACE5APIC:				return "FACE5APIC";
		case SOD_FACE5BPIC:				return "FACE5BPIC";
		case SOD_FACE5CPIC:				return "FACE5CPIC";
		case SOD_FACE6APIC:				return "FACE6APIC";
		case SOD_FACE6BPIC:				return "FACE6BPIC";
		case SOD_FACE6CPIC:				return "FACE6CPIC";
		case SOD_FACE7APIC:				return "FACE7APIC";
		case SOD_FACE7BPIC:				return "FACE7BPIC";
		case SOD_FACE7CPIC:				return "FACE7CPIC";
		case SOD_FACE8APIC:				return "FACE8APIC";
		case SOD_GOTGATLINGPIC:			return "GOTGATLINGPIC";
		case SOD_GODMODEFACE1PIC:		return "GODMODEFACE0PIC";
		case SOD_GODMODEFACE2PIC:		return "GODMODEFACE1PIC";
		case SOD_GODMODEFACE3PIC:		return "GODMODEFACE2PIC";
		case SOD_BJWAITING1PIC:			return "BJWAITING0PIC";
		case SOD_BJWAITING2PIC:			return "BJWAITING1PIC";
		case SOD_BJOUCHPIC:				return "BJOUCHPIC";
		case SOD_PAUSEDPIC:				return "PAUSEDPIC";
		case SOD_GETPSYCHEDPIC:			return "GETPSYCHEDPIC";
		
        default: return NULL;
    
	} // End switch id

	return NULL;

}

/**
 * \brief Returns the music chunk name as a string value.
 * \param[in] id Chunk value to get string value of.
 * \return NULL on error, otherwise string value.
 */
PUBLIC char *SOD_GetReduxMusicFileName( W32 id )
{
    switch( id )
    {
		case SOD_XFUNKIE_MUS:	return "XFUNKIE";
		case SOD_DUNGEON_MUS:	return "DUNGEON";
		case SOD_XDEATH_MUS:	return "XDEATH";
		case SOD_GETTHEM_MUS:	return "GETTHEM";
		case SOD_XTIPTOE_MUS:	return "XTIPTOE";
		case SOD_GOINGAFT_MUS:	return "GOINGAFT";
		case SOD_URAHERO_MUS:	return "URAHERO";
		case SOD_XTHEEND_MUS:	return "XTHEEND";
		case SOD_NAZI_OMI_MUS:	return "NAZI_OMI";
		case SOD_POW_MUS:		return "POW";
		case SOD_TWELFTH_MUS:	return "TWELFTH";
		case SOD_SEARCHN_MUS:	return "SEARCHN";
		case SOD_SUSPENSE_MUS:	return "SUSPENSE";
		case SOD_ZEROHOUR_MUS:	return "ZEROHOUR";
		case SOD_WONDERIN_MUS:	return "WONDERIN";
		case SOD_ULTIMATE_MUS:	return "ULTIMATE";
		case SOD_ENDLEVEL_MUS:	return "ENDLEVEL";
		case SOD_XEVIL_MUS:		return "XEVIL";
		case SOD_XJAZNAZI_MUS:	return "XJAZNAZI";
		case SOD_COPYPRO_MUS:	return "COPYPRO";
		case SOD_XAWARD_MUS:	return "XAWARD";
		case SOD_XPUTIT_MUS:	return "XPUTIT";
		case SOD_XGETYOU_MUS:	return "XGETYOU";
		case SOD_XTOWER2_MUS:	return "XTOWER2";

        default: return NULL;
    
	} // End switch id

	return NULL;

}


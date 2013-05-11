/*

	Copyright (C) 2007 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file spear.h
 * \brief Spear of Destiny data mappings.
 * \author Michael Liebscher 
 * \date 2007
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#ifndef __SPEAR_DEF_H__
#define __SPEAR_DEF_H__


#include "../../common/platform.h"


/* Spear of Destiny Demo defines */
typedef enum 
{
		// Lump Start
		SDM_C_BACKDROPPIC = 3,
		SDM_C_MOUSELBACKPIC,                     // 4
		SDM_C_CURSOR1PIC,                        // 5
		SDM_C_CURSOR2PIC,                        // 6
		SDM_C_NOTSELECTEDPIC,                    // 7
		SDM_C_SELECTEDPIC,                       // 8
		// Lump Start
		SDM_C_CUSTOMIZEPIC,                      // 9
		SDM_C_JOY1PIC,                           // 10
		SDM_C_JOY2PIC,                           // 11
		SDM_C_MOUSEPIC,                          // 12
		SDM_C_JOYSTICKPIC,                       // 13
		SDM_C_KEYBOARDPIC,                       // 14
		SDM_C_CONTROLPIC,                        // 15
		// Lump Start
		SDM_C_OPTIONSPIC,                        // 16
		// Lump Start
		SDM_C_FXTITLEPIC,                        // 17
		SDM_C_DIGITITLEPIC,                      // 18
		SDM_C_MUSICTITLEPIC,                     // 19
		// Lump Start
		SDM_C_HOWTOUGHPIC,                       // 20
		SDM_C_BABYMODEPIC,                       // 21
		SDM_C_EASYPIC,                           // 22
		SDM_C_NORMALPIC,                         // 23
		SDM_C_HARDPIC,                           // 24
		// Lump Start
		SDM_C_DISKLOADING1PIC,                   // 25
		SDM_C_DISKLOADING2PIC,                   // 26
		SDM_C_LOADGAMEPIC,                       // 27
		SDM_C_SAVEGAMEPIC,                       // 28
		// Lump Start
		SDM_HIGHSCORESPIC,                       // 29
		SDM_C_WONSPEARPIC,                       // 30
		// Lump Start
		SDM_L_GUYPIC,                            // 31
		SDM_L_COLONPIC,                          // 32
		SDM_L_NUM0PIC,                           // 33
		SDM_L_NUM1PIC,                           // 34
		SDM_L_NUM2PIC,                           // 35
		SDM_L_NUM3PIC,                           // 36
		SDM_L_NUM4PIC,                           // 37
		SDM_L_NUM5PIC,                           // 38
		SDM_L_NUM6PIC,                           // 39
		SDM_L_NUM7PIC,                           // 40
		SDM_L_NUM8PIC,                           // 41
		SDM_L_NUM9PIC,                           // 42
		SDM_L_PERCENTPIC,                        // 43
		SDM_L_APIC,                              // 44
		SDM_L_BPIC,                              // 45
		SDM_L_CPIC,                              // 46
		SDM_L_DPIC,                              // 47
		SDM_L_EPIC,                              // 48
		SDM_L_FPIC,                              // 49
		SDM_L_GPIC,                              // 50
		SDM_L_HPIC,                              // 51
		SDM_L_IPIC,                              // 52
		SDM_L_JPIC,                              // 53
		SDM_L_KPIC,                              // 54
		SDM_L_LPIC,                              // 55
		SDM_L_MPIC,                              // 56
		SDM_L_NPIC,                              // 57
		SDM_L_OPIC,                              // 58
		SDM_L_PPIC,                              // 59
		SDM_L_QPIC,                              // 60
		SDM_L_RPIC,                              // 61
		SDM_L_SPIC,                              // 62
		SDM_L_TPIC,                              // 63
		SDM_L_UPIC,                              // 64
		SDM_L_VPIC,                              // 65
		SDM_L_WPIC,                              // 66
		SDM_L_XPIC,                              // 67
		SDM_L_YPIC,                              // 68
		SDM_L_ZPIC,                              // 69
		SDM_L_EXPOINTPIC,                        // 70
		SDM_L_APOSTROPHEPIC,                     // 71
		SDM_L_GUY2PIC,                           // 72
		SDM_L_BJWINSPIC,                         // 73
		// Lump Start
		SDM_TITLE1PIC,                           // 74
		SDM_TITLE2PIC,                           // 75
		SDM_STATUSBARPIC,                        // 76
		SDM_PG13PIC,                             // 77
		SDM_CREDITSPIC,                          // 78
		// Lump Start
		SDM_KNIFEPIC,                            // 79
		SDM_GUNPIC,                              // 80
		SDM_MACHINEGUNPIC,                       // 81
		SDM_GATLINGGUNPIC,                       // 82
		SDM_NOKEYPIC,                            // 83
		SDM_GOLDKEYPIC,                          // 84
		SDM_SILVERKEYPIC,                        // 85
		SDM_N_BLANKPIC,                          // 86
		SDM_N_0PIC,                              // 87
		SDM_N_1PIC,                              // 88
		SDM_N_2PIC,                              // 89
		SDM_N_3PIC,                              // 90
		SDM_N_4PIC,                              // 91
		SDM_N_5PIC,                              // 92
		SDM_N_6PIC,                              // 93
		SDM_N_7PIC,                              // 94
		SDM_N_8PIC,                              // 95
		SDM_N_9PIC,                              // 96
		SDM_FACE1APIC,                           // 97
		SDM_FACE1BPIC,                           // 98
		SDM_FACE1CPIC,                           // 99
		SDM_FACE2APIC,                           // 100
		SDM_FACE2BPIC,                           // 101
		SDM_FACE2CPIC,                           // 102
		SDM_FACE3APIC,                           // 103
		SDM_FACE3BPIC,                           // 104
		SDM_FACE3CPIC,                           // 105
		SDM_FACE4APIC,                           // 106
		SDM_FACE4BPIC,                           // 107
		SDM_FACE4CPIC,                           // 108
		SDM_FACE5APIC,                           // 109
		SDM_FACE5BPIC,                           // 110
		SDM_FACE5CPIC,                           // 111
		SDM_FACE6APIC,                           // 112
		SDM_FACE6BPIC,                           // 113
		SDM_FACE6CPIC,                           // 114
		SDM_FACE7APIC,                           // 115
		SDM_FACE7BPIC,                           // 116
		SDM_FACE7CPIC,                           // 117
		SDM_FACE8APIC,                           // 118
		SDM_GOTGATLINGPIC,                       // 119
		SDM_GODMODEFACE1PIC,                     // 120
		SDM_GODMODEFACE2PIC,                     // 121
		SDM_GODMODEFACE3PIC,                     // 122
		SDM_BJWAITING1PIC,                       // 123
		SDM_BJWAITING2PIC,                       // 124
		SDM_BJOUCHPIC,                           // 125
		SDM_PAUSEDPIC,                           // 126
		SDM_GETPSYCHEDPIC,                       // 127



		SDM_ORDERSCREEN = 129,
		SDM_ERRORSCREEN,                         // 130
		SDM_TITLEPALETTE,                        // 131
		SDM_T_DEMO0,                             // 132
		
		SDM_ENUMEND

} sdm_graphicnums;



/* Spear of Destiny defines */
typedef enum 
{
		// Lump Start
		SOD_C_BACKDROPPIC = 3,
		SOD_C_MOUSELBACKPIC,                     // 4
		SOD_C_CURSOR1PIC,                        // 5
		SOD_C_CURSOR2PIC,                        // 6
		SOD_C_NOTSELECTEDPIC,                    // 7
		SOD_C_SELECTEDPIC,                       // 8
		// Lump Start
		SOD_C_CUSTOMIZEPIC,                      // 9
		SOD_C_JOY1PIC,                           // 10
		SOD_C_JOY2PIC,                           // 11
		SOD_C_MOUSEPIC,                          // 12
		SOD_C_JOYSTICKPIC,                       // 13
		SOD_C_KEYBOARDPIC,                       // 14
		SOD_C_CONTROLPIC,                        // 15
		// Lump Start
		SOD_C_OPTIONSPIC,                        // 16
		// Lump Start
		SOD_C_FXTITLEPIC,                        // 17
		SOD_C_DIGITITLEPIC,                      // 18
		SOD_C_MUSICTITLEPIC,                     // 19
		// Lump Start
		SOD_C_HOWTOUGHPIC,                       // 20
		SOD_C_BABYMODEPIC,                       // 21
		SOD_C_EASYPIC,                           // 22
		SOD_C_NORMALPIC,                         // 23
		SOD_C_HARDPIC,                           // 24
		// Lump Start
		SOD_C_DISKLOADING1PIC,                   // 25
		SOD_C_DISKLOADING2PIC,                   // 26
		SOD_C_LOADGAMEPIC,                       // 27
		SOD_C_SAVEGAMEPIC,                       // 28
		// Lump Start
		SOD_HIGHSCORESPIC,                       // 29
		SOD_C_WONSPEARPIC,                       // 30
		// Lump Start
		SOD_BJCOLLAPSE1PIC,                      // 31
		SOD_BJCOLLAPSE2PIC,                      // 32
		SOD_BJCOLLAPSE3PIC,                      // 33
		SOD_BJCOLLAPSE4PIC,                      // 34
		SOD_ENDPICPIC,                           // 35
		// Lump Start
		SOD_L_GUYPIC,                            // 36
		SOD_L_COLONPIC,                          // 37
		SOD_L_NUM0PIC,                           // 38
		SOD_L_NUM1PIC,                           // 39
		SOD_L_NUM2PIC,                           // 40
		SOD_L_NUM3PIC,                           // 41
		SOD_L_NUM4PIC,                           // 42
		SOD_L_NUM5PIC,                           // 43
		SOD_L_NUM6PIC,                           // 44
		SOD_L_NUM7PIC,                           // 45
		SOD_L_NUM8PIC,                           // 46
		SOD_L_NUM9PIC,                           // 47
		SOD_L_PERCENTPIC,                        // 48
		SOD_L_APIC,                              // 49
		SOD_L_BPIC,                              // 50
		SOD_L_CPIC,                              // 51
		SOD_L_DPIC,                              // 52
		SOD_L_EPIC,                              // 53
		SOD_L_FPIC,                              // 54
		SOD_L_GPIC,                              // 55
		SOD_L_HPIC,                              // 56
		SOD_L_IPIC,                              // 57
		SOD_L_JPIC,                              // 58
		SOD_L_KPIC,                              // 59
		SOD_L_LPIC,                              // 60
		SOD_L_MPIC,                              // 61
		SOD_L_NPIC,                              // 62
		SOD_L_OPIC,                              // 63
		SOD_L_PPIC,                              // 64
		SOD_L_QPIC,                              // 65
		SOD_L_RPIC,                              // 66
		SOD_L_SPIC,                              // 67
		SOD_L_TPIC,                              // 68
		SOD_L_UPIC,                              // 69
		SOD_L_VPIC,                              // 70
		SOD_L_WPIC,                              // 71
		SOD_L_XPIC,                              // 72
		SOD_L_YPIC,                              // 73
		SOD_L_ZPIC,                              // 74
		SOD_L_EXPOINTPIC,                        // 75
		SOD_L_APOSTROPHEPIC,                     // 76
		SOD_L_GUY2PIC,                           // 77
		SOD_L_BJWINSPIC,                         // 78
		// Lump Start
		SOD_TITLE1PIC,                           // 79
		SOD_TITLE2PIC,                           // 80
		// Lump Start
		SOD_ENDSCREEN1PIC,                       // 81
		SOD_ENDSCREEN2PIC,                       // 82
		SOD_ENDSCREEN3PIC,                       // 83
		SOD_ENDSCREEN4PIC,                       // 84
		SOD_ENDSCREEN5PIC,                       // 85
		SOD_ENDSCREEN6PIC,                       // 86
		SOD_ENDSCREEN7PIC,                       // 87
		SOD_ENDSCREEN8PIC,                       // 88
		SOD_ENDSCREEN9PIC,                       // 89
		SOD_STATUSBARPIC,                        // 90
		SOD_PG13PIC,                             // 91
		SOD_CREDITSPIC,                          // 92
		// Lump Start
		SOD_IDGUYS1PIC,                          // 93
		SOD_IDGUYS2PIC,                          // 94
		// Lump Start
		SOD_COPYPROTTOPPIC,                      // 95
		SOD_COPYPROTBOXPIC,                      // 96
		SOD_BOSSPIC1PIC,                         // 97
		SOD_BOSSPIC2PIC,                         // 98
		SOD_BOSSPIC3PIC,                         // 99
		SOD_BOSSPIC4PIC,                         // 100
		// Lump Start
		SOD_KNIFEPIC,                            // 101
		SOD_GUNPIC,                              // 102
		SOD_MACHINEGUNPIC,                       // 103
		SOD_GATLINGGUNPIC,                       // 104
		SOD_NOKEYPIC,                            // 105
		SOD_GOLDKEYPIC,                          // 106
		SOD_SILVERKEYPIC,                        // 107
		SOD_N_BLANKPIC,                          // 108
		SOD_N_0PIC,                              // 109
		SOD_N_1PIC,                              // 110
		SOD_N_2PIC,                              // 111
		SOD_N_3PIC,                              // 112
		SOD_N_4PIC,                              // 113
		SOD_N_5PIC,                              // 114
		SOD_N_6PIC,                              // 115
		SOD_N_7PIC,                              // 116
		SOD_N_8PIC,                              // 117
		SOD_N_9PIC,                              // 118
		SOD_FACE1APIC,                           // 119
		SOD_FACE1BPIC,                           // 120
		SOD_FACE1CPIC,                           // 121
		SOD_FACE2APIC,                           // 122
		SOD_FACE2BPIC,                           // 123
		SOD_FACE2CPIC,                           // 124
		SOD_FACE3APIC,                           // 125
		SOD_FACE3BPIC,                           // 126
		SOD_FACE3CPIC,                           // 127
		SOD_FACE4APIC,                           // 128
		SOD_FACE4BPIC,                           // 129
		SOD_FACE4CPIC,                           // 130
		SOD_FACE5APIC,                           // 131
		SOD_FACE5BPIC,                           // 132
		SOD_FACE5CPIC,                           // 133
		SOD_FACE6APIC,                           // 134
		SOD_FACE6BPIC,                           // 135
		SOD_FACE6CPIC,                           // 136
		SOD_FACE7APIC,                           // 137
		SOD_FACE7BPIC,                           // 138
		SOD_FACE7CPIC,                           // 139
		SOD_FACE8APIC,                           // 140
		SOD_GOTGATLINGPIC,                       // 141
		SOD_GODMODEFACE1PIC,                     // 142
		SOD_GODMODEFACE2PIC,                     // 143
		SOD_GODMODEFACE3PIC,                     // 144
		SOD_BJWAITING1PIC,                       // 145
		SOD_BJWAITING2PIC,                       // 146
		SOD_BJOUCHPIC,                           // 147
		SOD_PAUSEDPIC,                           // 148
		SOD_GETPSYCHEDPIC,                       // 149

		SOD_ORDERSCREEN = 151,
		SOD_ERRORSCREEN,                         // 152
		SOD_TITLEPALETTE,                        // 153
		SOD_END1PALETTE,                         // 154
		SOD_END2PALETTE,                         // 155
		SOD_END3PALETTE,                         // 156
		SOD_END4PALETTE,                         // 157
		SOD_END5PALETTE,                         // 158
		SOD_END6PALETTE,                         // 159
		SOD_END7PALETTE,                         // 160
		SOD_END8PALETTE,                         // 161
		SOD_END9PALETTE,                         // 162
		SOD_IDGUYSPALETTE,                       // 163
		SOD_T_DEMO0,                             // 164
		SOD_T_DEMO1,                             // 165
		SOD_T_DEMO2,                             // 166
		SOD_T_DEMO3,                             // 167
		SOD_T_ENDART1,                           // 168
		SOD_ENUMEND

} sod_graphicnums;




typedef enum 
{
		SOD_XFUNKIE_MUS,             // 0
		SOD_DUNGEON_MUS,             // 1
		SOD_XDEATH_MUS,              // 2
		SOD_GETTHEM_MUS,             // 3
		SOD_XTIPTOE_MUS,             // 4
		SOD_GOINGAFT_MUS,            // 5
		SOD_URAHERO_MUS,             // 6
		SOD_XTHEEND_MUS,             // 7
		SOD_NAZI_OMI_MUS,            // 8
		SOD_POW_MUS,                 // 9
		SOD_TWELFTH_MUS,             // 10
		SOD_SEARCHN_MUS,             // 11
		SOD_SUSPENSE_MUS,            // 12
		SOD_ZEROHOUR_MUS,            // 13
		SOD_WONDERIN_MUS,            // 14
		SOD_ULTIMATE_MUS,            // 15
		SOD_ENDLEVEL_MUS,            // 16
		SOD_XEVIL_MUS,               // 17
		SOD_XJAZNAZI_MUS,            // 18
		SOD_COPYPRO_MUS,             // 19
		SOD_XAWARD_MUS,              // 20
		SOD_XPUTIT_MUS,              // 21
		SOD_XGETYOU_MUS,             // 22
		SOD_XTOWER2_MUS,             // 23
		SOD_LASTMUSIC

} sod_musicnames;



char *SDM_GetReduxGFXFileName( W32 id );
char *SOD_GetReduxGFXFileName( W32 id );

char *SOD_GetReduxMusicFileName( W32 id );



#endif /* __SPEAR_DEF_H__ */

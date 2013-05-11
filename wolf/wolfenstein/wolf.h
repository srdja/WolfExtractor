/*

	Copyright (C) 2006-2007 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolf.h
 * \brief Wolfenstein 3-D defines.
 * \author Michael Liebscher 
 * \date 2006-2007
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#ifndef __WOLF_H__
#define __WOLF_H__

#include "../../common/platform.h"

typedef enum {  DEFAULT_VERSION,
                WL1_V10,
                WL1_V11,
                WL1_V14,
                APOGEE_WL6_V11,
                APOGEE_WL6_V12,
                APOGEE_WL6_V14_A,
                APOGEE_WL6_V14_B,
                APOGEE_WL6_V14G,
                GTINTERACTIVE_WL6_V14A,
                GTINTERACTIVE_WL6_V14B,
                ACTIVISION_WL6_V14,
                ID_SOFTWARE_WL6_V14,
                SPEAR_OF_DESTINY,
                SPEAR_OF_DESTINY_DEMO,                
                SPEAR_OF_DESTINY_SD1,
                SPEAR_OF_DESTINY_SD2,
                SPEAR_OF_DESTINY_SD3

} WOLFID_t;

extern W32 wolf_version;



typedef enum 
{
		// Lump Start
		WL1_H_BJPIC = 3,
		WL1_H_CASTLEPIC,                 // 4
		WL1_H_KEYBOARDPIC,               // 5
		WL1_H_CONTROLPIC,                // 6
		WL1_H_HEALPIC,                   // 7
		WL1_H_BGPIC,                     // 8
		WL1_H_GUNPIC,                    // 9
		WL1_H_KEYPIC,                    // 10
		WL1_H_BLAZEPIC,                  // 11
		WL1_H_WEAPON1234PIC,             // 12
		WL1_H_WOLFLOGOPIC,               // 13
		WL1_H_VISAPIC,					 // 14
		WL1_H_MCPIC,		             // 15
		WL1_H_IDLOGOPIC,                 // 16
		WL1_H_TOPWINDOWPIC,              // 17
		WL1_H_LEFTWINDOWPIC,             // 18
		WL1_H_RIGHTWINDOWPIC,            // 19		
		WL1_H_BOTTOMINFOPIC,			 // 20
		WL1_H_GAMEPIC,					 // 21
		// Lump Start
		WL1_C_OPTIONSPIC,                // 22
		WL1_C_CURSOR1PIC,                // 23
		WL1_C_CURSOR2PIC,                // 24
		WL1_C_NOTSELECTEDPIC,            // 25
		WL1_C_SELECTEDPIC,               // 26
		WL1_C_FXTITLEPIC,                // 27
		WL1_C_DIGITITLEPIC,              // 28
		WL1_C_MUSICTITLEPIC,             // 29
		WL1_C_MOUSELBACKPIC,             // 30
		WL1_C_BABYMODEPIC,               // 31
		WL1_C_EASYPIC,                   // 32
		WL1_C_NORMALPIC,                 // 33
		WL1_C_HARDPIC,                   // 34
		WL1_C_LOADSAVEDISKPIC,           // 35
		WL1_C_DISKLOADING1PIC,           // 36
		WL1_C_DISKLOADING2PIC,           // 37
		WL1_C_CONTROLPIC,                // 38
		WL1_C_CUSTOMIZEPIC,              // 39
		WL1_C_LOADGAMEPIC,               // 40
		WL1_C_SAVEGAMEPIC,               // 41
		WL1_C_EPISODE1PIC,               // 42
		WL1_C_EPISODE2PIC,               // 43
		WL1_C_EPISODE3PIC,               // 44
		WL1_C_EPISODE4PIC,               // 45
		WL1_C_EPISODE5PIC,               // 46
		WL1_C_EPISODE6PIC,               // 47
		WL1_C_CODEPIC,                   // 48
		WL1_C_TIMECODEPIC,				 // 49
		WL1_C_LEVELPIC,					 // 50
		WL1_C_NAMEPIC,					 // 51
		WL1_C_SCOREPIC,					 // 52 
		WL1_C_JOY1PIC,					 // 53
		WL1_C_JOY2PIC,					 // 54
		// Lump Start
		WL1_L_GUYPIC,                    // 55
		WL1_L_COLONPIC,                  // 56
		WL1_L_NUM0PIC,                   // 57
		WL1_L_NUM1PIC,                   // 58
		WL1_L_NUM2PIC,                   // 59
		WL1_L_NUM3PIC,                   // 60
		WL1_L_NUM4PIC,                   // 61
		WL1_L_NUM5PIC,                   // 62
		WL1_L_NUM6PIC,                   // 63
		WL1_L_NUM7PIC,                   // 64
		WL1_L_NUM8PIC,                   // 65
		WL1_L_NUM9PIC,                   // 66
		WL1_L_PERCENTPIC,                // 67
		WL1_L_APIC,                      // 68
		WL1_L_BPIC,                      // 69
		WL1_L_CPIC,                      // 70
		WL1_L_DPIC,                      // 71
		WL1_L_EPIC,                      // 72
		WL1_L_FPIC,                      // 73
		WL1_L_GPIC,                      // 74
		WL1_L_HPIC,                      // 75
		WL1_L_IPIC,                      // 76
		WL1_L_JPIC,                      // 77
		WL1_L_KPIC,                      // 78
		WL1_L_LPIC,                      // 79
		WL1_L_MPIC,                      // 80
		WL1_L_NPIC,                      // 81
		WL1_L_OPIC,                      // 82
		WL1_L_PPIC,                      // 83
		WL1_L_QPIC,                      // 84
		WL1_L_RPIC,                      // 85
		WL1_L_SPIC,                      // 86
		WL1_L_TPIC,                      // 87
		WL1_L_UPIC,                      // 88
		WL1_L_VPIC,                      // 89
		WL1_L_WPIC,                      // 90
		WL1_L_XPIC,                      // 91
		WL1_L_YPIC,                      // 92
		WL1_L_ZPIC,                      // 93
		WL1_L_EXPOINTPIC,				 // 94
		WL1_L_APOSTROPHEPIC,			 // 95
		WL1_L_GUY2PIC,                   // 96
		WL1_L_BJWINSPIC,                 // 97
		WL1_STATUSBARPIC,                // 98
		WL1_TITLEPIC,                    // 99
		WL1_PG13PIC,                     // 100
		WL1_CREDITSPIC,                  // 101
		WL1_HIGHSCORESPIC,               // 102
		// Lump Start
		WL1_KNIFEPIC,                    // 103
		WL1_GUNPIC,                      // 104
		WL1_MACHINEGUNPIC,               // 105
		WL1_GATLINGGUNPIC,               // 106
		WL1_NOKEYPIC,                    // 107
		WL1_GOLDKEYPIC,                  // 108
		WL1_SILVERKEYPIC,                // 109
		WL1_N_BLANKPIC,                  // 110
		WL1_N_0PIC,                      // 111
		WL1_N_1PIC,                      // 112
		WL1_N_2PIC,                      // 113
		WL1_N_3PIC,                      // 114
		WL1_N_4PIC,                      // 115
		WL1_N_5PIC,                      // 116
		WL1_N_6PIC,                      // 117
		WL1_N_7PIC,                      // 118
		WL1_N_8PIC,                      // 119
		WL1_N_9PIC,                      // 120
		WL1_FACE1APIC,                   // 121
		WL1_FACE1BPIC,                   // 122
		WL1_FACE1CPIC,                   // 123
		WL1_FACE2APIC,                   // 124
		WL1_FACE2BPIC,                   // 125
		WL1_FACE2CPIC,                   // 126
		WL1_FACE3APIC,                   // 127
		WL1_FACE3BPIC,                   // 128
		WL1_FACE3CPIC,                   // 129
		WL1_FACE4APIC,                   // 130
		WL1_FACE4BPIC,                   // 131
		WL1_FACE4CPIC,                   // 132
		WL1_FACE5APIC,                   // 133
		WL1_FACE5BPIC,                   // 134
		WL1_FACE5CPIC,                   // 135
		WL1_FACE6APIC,                   // 136
		WL1_FACE6BPIC,                   // 137
		WL1_FACE6CPIC,                   // 138
		WL1_FACE7APIC,                   // 139
		WL1_FACE7BPIC,                   // 140
		WL1_FACE7CPIC,                   // 141
		WL1_FACE8APIC,                   // 142
		WL1_GOTGATLINGPIC,               // 143
		WL1_MUTANTBJPIC,				 // 144
		WL1_PAUSEDPIC,                   // 145
		WL1_GETPSYCHEDPIC,               // 146

		WL1_ENUMEND

} wl1_graphicnums;


typedef enum 
{
		// Lump Start
		WL6_H_BJPIC = 3,                          
		WL6_H_CASTLEPIC,                         // 4
		WL6_H_BLAZEPIC,                          // 5
		WL6_H_TOPWINDOWPIC,                      // 6
		WL6_H_LEFTWINDOWPIC,                     // 7
		WL6_H_RIGHTWINDOWPIC,                    // 8
		WL6_H_BOTTOMINFOPIC,                     // 9
		WL6_C_OPTIONSPIC,                        // 10
		WL6_C_CURSOR1PIC,                        // 11
		WL6_C_CURSOR2PIC,                        // 12
		WL6_C_NOTSELECTEDPIC,                    // 13
		WL6_C_SELECTEDPIC,                       // 14
		WL6_C_FXTITLEPIC,                        // 15
		WL6_C_DIGITITLEPIC,                      // 16
		WL6_C_MUSICTITLEPIC,                     // 17
		WL6_C_MOUSELBACKPIC,                     // 18
		WL6_C_BABYMODEPIC,                       // 19
		WL6_C_EASYPIC,                           // 20
		WL6_C_NORMALPIC,                         // 21
		WL6_C_HARDPIC,                           // 22
		WL6_C_LOADSAVEDISKPIC,                   // 23
		WL6_C_DISKLOADING1PIC,                   // 24
		WL6_C_DISKLOADING2PIC,                   // 25
		WL6_C_CONTROLPIC,                        // 26
		WL6_C_CUSTOMIZEPIC,                      // 27
		WL6_C_LOADGAMEPIC,                       // 28
		WL6_C_SAVEGAMEPIC,                       // 29
		WL6_C_EPISODE1PIC,                       // 30
		WL6_C_EPISODE2PIC,                       // 31
		WL6_C_EPISODE3PIC,                       // 32
		WL6_C_EPISODE4PIC,                       // 33
		WL6_C_EPISODE5PIC,                       // 34
		WL6_C_EPISODE6PIC,                       // 35
		WL6_C_CODEPIC,                           // 36
		WL6_C_TIMECODEPIC,                       // 37
		WL6_C_LEVELPIC,                          // 38
		WL6_C_NAMEPIC,                           // 39
		WL6_C_SCOREPIC,                          // 40
		WL6_C_JOY1PIC,                           // 41
		WL6_C_JOY2PIC,                           // 42
		// Lump Start
		WL6_L_GUYPIC,                            // 43
		WL6_L_COLONPIC,                          // 44
		WL6_L_NUM0PIC,                           // 45
		WL6_L_NUM1PIC,                           // 46
		WL6_L_NUM2PIC,                           // 47
		WL6_L_NUM3PIC,                           // 48
		WL6_L_NUM4PIC,                           // 49
		WL6_L_NUM5PIC,                           // 50
		WL6_L_NUM6PIC,                           // 51
		WL6_L_NUM7PIC,                           // 52
		WL6_L_NUM8PIC,                           // 53
		WL6_L_NUM9PIC,                           // 54
		WL6_L_PERCENTPIC,                        // 55
		WL6_L_APIC,                              // 56
		WL6_L_BPIC,                              // 57
		WL6_L_CPIC,                              // 58
		WL6_L_DPIC,                              // 59
		WL6_L_EPIC,                              // 60
		WL6_L_FPIC,                              // 61
		WL6_L_GPIC,                              // 62
		WL6_L_HPIC,                              // 63
		WL6_L_IPIC,                              // 64
		WL6_L_JPIC,                              // 65
		WL6_L_KPIC,                              // 66
		WL6_L_LPIC,                              // 67
		WL6_L_MPIC,                              // 68
		WL6_L_NPIC,                              // 69
		WL6_L_OPIC,                              // 70
		WL6_L_PPIC,                              // 71
		WL6_L_QPIC,                              // 72
		WL6_L_RPIC,                              // 73
		WL6_L_SPIC,                              // 74
		WL6_L_TPIC,                              // 75
		WL6_L_UPIC,                              // 76
		WL6_L_VPIC,                              // 77
		WL6_L_WPIC,                              // 78
		WL6_L_XPIC,                              // 79
		WL6_L_YPIC,                              // 80
		WL6_L_ZPIC,                              // 81
		WL6_L_EXPOINTPIC,                        // 82
		WL6_L_APOSTROPHEPIC,                     // 83
		WL6_L_GUY2PIC,                           // 84
		WL6_L_BJWINSPIC,                         // 85
		WL6_STATUSBARPIC,                        // 86
		WL6_TITLEPIC,                            // 87
		WL6_PG13PIC,                             // 88
		WL6_CREDITSPIC,                          // 89
		WL6_HIGHSCORESPIC,                       // 90
		// Lump Start
		WL6_KNIFEPIC,                            // 91
		WL6_GUNPIC,                              // 92
		WL6_MACHINEGUNPIC,                       // 93
		WL6_GATLINGGUNPIC,                       // 94
		WL6_NOKEYPIC,                            // 95
		WL6_GOLDKEYPIC,                          // 96
		WL6_SILVERKEYPIC,                        // 97
		WL6_N_BLANKPIC,                          // 98
		WL6_N_0PIC,                              // 99
		WL6_N_1PIC,                              // 100
		WL6_N_2PIC,                              // 101
		WL6_N_3PIC,                              // 102
		WL6_N_4PIC,                              // 103
		WL6_N_5PIC,                              // 104
		WL6_N_6PIC,                              // 105
		WL6_N_7PIC,                              // 106
		WL6_N_8PIC,                              // 107
		WL6_N_9PIC,                              // 108
		WL6_FACE1APIC,                           // 109
		WL6_FACE1BPIC,                           // 110
		WL6_FACE1CPIC,                           // 111
		WL6_FACE2APIC,                           // 112
		WL6_FACE2BPIC,                           // 113
		WL6_FACE2CPIC,                           // 114
		WL6_FACE3APIC,                           // 115
		WL6_FACE3BPIC,                           // 116
		WL6_FACE3CPIC,                           // 117
		WL6_FACE4APIC,                           // 118
		WL6_FACE4BPIC,                           // 119
		WL6_FACE4CPIC,                           // 120
		WL6_FACE5APIC,                           // 121
		WL6_FACE5BPIC,                           // 122
		WL6_FACE5CPIC,                           // 123
		WL6_FACE6APIC,                           // 124
		WL6_FACE6BPIC,                           // 125
		WL6_FACE6CPIC,                           // 126
		WL6_FACE7APIC,                           // 127
		WL6_FACE7BPIC,                           // 128
		WL6_FACE7CPIC,                           // 129
		WL6_FACE8APIC,                           // 130
		WL6_GOTGATLINGPIC,                       // 131
		WL6_MUTANTBJPIC,                         // 132
		WL6_PAUSEDPIC,                           // 133
		WL6_GETPSYCHEDPIC,                       // 134

		WL6_ORDERSCREEN=136,
		WL6_ERRORSCREEN,                         // 137
		WL6_T_HELPART,                           // 138
		WL6_T_DEMO0,                             // 139
		WL6_T_DEMO1,                             // 140
		WL6_T_DEMO2,                             // 141
		WL6_T_DEMO3,                             // 142
		WL6_T_ENDART1,                           // 143
		WL6_T_ENDART2,                           // 144
		WL6_T_ENDART3,                           // 145
		WL6_T_ENDART4,                           // 146
		WL6_T_ENDART5,                           // 147
		WL6_T_ENDART6,                           // 148
		
		WL6_ENUMEND

} wl6_graphicnums;


#endif /* __WOLF_H__ */


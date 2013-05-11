/*

	Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file adlib.h
 * \brief Interface to adlib hardware.
 * \author Michael Liebscher 
 * \date 2004
 * \note This code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#ifndef __ADLIB_H__
#define __ADLIB_H__


#include "../../common/platform.h"





typedef	struct
{
    W8   mChar, cChar,
         mScale, cScale,
         mAttack, cAttack,
         mSus, cSus,
         mWave, cWave,
         nConn,

    /* These are only for Muse - these bytes are really unused */
         voice,
         mode,
         unused[ 3 ];

} Instrument;

typedef	struct
{
    char length[ 4 ];
	char priority[ 2 ];

} SoundCommon;

typedef	struct
{
    SoundCommon	common;
    Instrument	inst;
    W8   block;
    W8   data[ 1 ];

} AdLibSound;


wtBoolean ADLIB_Init( W32 freq );
void ADLIB_Shutdown();

void *ADLIB_DecodeSound( AdLibSound *sound, W32 *length );

W32 ADLIB_getLength( void *musbuffer );
void ADLIB_LoadMusic( void *musbuffer );
W32 ADLIB_UpdateMusic( W32 size, void *buffer );


#endif /* __ADLIB_H__ */


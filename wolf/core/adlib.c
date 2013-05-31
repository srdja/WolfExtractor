/*

	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file adlib.c
 * \brief Interface to adlib hardware.
 * \author Michael Liebscher
 * \date 2004-2005
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#include <stdlib.h>
#include <stdio.h>


#include "adlib.h"
#include "fmopl.h"

#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../memory/memory.h"
#include "../../console/console.h"


#define OPL_INTERNAL_FREQ   3600000 // The OPL operates at 3.6MHz
#define OPL_NUM_CHIPS       1       // Number of OPL chips
#define ADLIB_FREQ          22050   // Frequency in Hz


// 	Registers for the AdLib card
#define	alFMStatus	0x388	// Read
#define	alFMAddr	0x388	// Write
#define	alFMData	0x389	// Write

//	Register addresses
// Operator stuff
#define	alChar		0x20
#define	alScale		0x40
#define	alAttack	0x60
#define	alSus		0x80
#define	alWave		0xe0
// Channel stuff
#define	alFreqL		0xa0
#define	alFreqH		0xb0
#define	alFeedCon	0xc0
// Global stuff
#define	alEffects	0xbd


// This table maps channel numbers to carrier and modulator op cells
PRIVATE	W8	carriers[  9 ] = { 3, 4, 5, 11, 12,13, 19, 20, 21 };
PRIVATE	W8	modifiers[ 9 ] = { 0, 1, 2, 8, 9, 10, 16, 17, 18 };


PRIVATE FM_OPL *hAdLib = NULL;



/**
 * \brief Start adlib hardware.
 * \return 1 on success, otherwise 0.
 * \note Must call ADLIB_Shutdown() when done.
 */
PUBLIC wtBoolean ADLIB_Init( W32 freq )
{
    hAdLib = OPLCreate( OPL_TYPE_YM3812, OPL_INTERNAL_FREQ, freq );

    if( hAdLib == NULL )
    {
		fprintf( stderr, "[ADLIB_Init]: Could not create AdLib OPL Emulator\n" );

		return false;
    }

	OPLWrite( hAdLib, 0x01, 0x20 ); /* Set WSE=1 */
	OPLWrite( hAdLib, 0x08, 0x00 ); /* Set CSM=0 & SEL=0 */

    return true;
}

/**
 * \brief Shutdown adlib hardware.
 */
PUBLIC void ADLIB_Shutdown( void )
{
    OPLDestroy( hAdLib );
}

/**
 * \brief Set Adlib FX instruction.
 * \param[in] inst Valid pointer to Instrument structure.
 * \return Nothing.
 */
PRIVATE void ADLIB_SetFXInst( Instrument *inst )
{
    W8   c, m;

    m = modifiers[ 0 ];
	c = carriers[ 0 ];


	OPLWrite( hAdLib, m + alChar, inst->mChar );
	OPLWrite( hAdLib, m + alScale, inst->mScale );
	OPLWrite( hAdLib, m + alAttack, inst->mAttack );
	OPLWrite( hAdLib, m + alSus, inst->mSus );
	OPLWrite( hAdLib, m + alWave, inst->mWave );
	OPLWrite( hAdLib, c + alChar, inst->cChar );
	OPLWrite( hAdLib, c + alScale, inst->cScale );
	OPLWrite( hAdLib, c + alAttack, inst->cAttack );
	OPLWrite( hAdLib, c + alSus, inst->cSus );
	OPLWrite( hAdLib, c + alWave, inst->cWave );

    OPLWrite( hAdLib, alFeedCon, 0 );
}

/**
 * \brief Decode adlib sound.
 * \param[in] inst Valid pointer to Instrument structure.
 * \param[out] length Length of decoded sound data in bytes.
 * \return On success true, otherwise false.
 */
PUBLIC void *ADLIB_DecodeSound( AdLibSound *sound, W32 *length )
{
    Instrument  inst;
	W32 alLengthLeft;
	W32 alBlock;
	W8  *alSound, s;
	W16 *ptr;
	W32 len;
	void *buffer;

	*length = 0;

	inst = sound->inst;
	alBlock = ( (sound->block & 7) << 2 ) | 0x20;


	alLengthLeft = *((PW32)sound->common.length);
	alLengthLeft = LittleLong( alLengthLeft );


	alSound = sound->data;


	len = alLengthLeft * 157 * 2; // 157[.5] = 22050 / 140

	buffer = (void *) MM_MALLOC( len );
	ptr = (PW16) buffer;

    OPLWrite( hAdLib, alFreqL, 0 );
	OPLWrite( hAdLib, alFreqH, 0 );

	ADLIB_SetFXInst( &inst );

    while( alLengthLeft )
	{
		s = *alSound++;
		if( ! s )
		{
			OPLWrite( hAdLib, alFreqH+0, 0 );
        }
		else
		{
			OPLWrite( hAdLib, alFreqL+0, s );
			OPLWrite( hAdLib, alFreqH+0, alBlock );
		}
		if( ! (--alLengthLeft) )
		{
			OPLWrite( hAdLib, alFreqH+0, 0 );
		}
		YM3812UpdateOne( hAdLib, ptr, 157 );
		ptr += 157;
	}

	*length = len;

	return (void *)buffer;
}







PRIVATE W16 *sqHackPtr;
PRIVATE W32 sqHackLen;
PRIVATE W32 sqHackTime;
PRIVATE W32 alTimeCount;

#define ADLIB_MUSIC_SPEED	44100
#define ADLIB_MUSIC_BYPS	(ADLIB_MUSIC_SPEED*2) // bytes per second (16 bit)

typedef	struct
{
	W16	length;
	W16	values[ 1 ];

} musicGroup_t;

musicGroup_t *music;


/**
 * \brief Setup music decoder.
 * \param[in] musbuffer musicGroup_t data structure.
 * \return Nothing.
 */
PUBLIC void ADLIB_LoadMusic( void *musbuffer )
{
	music = (musicGroup_t *)musbuffer;

	sqHackPtr = music->values;
	sqHackLen = LittleShort( music->length );
	sqHackTime = 0;
	alTimeCount = 0;
}

/**
 * \brief Decode adlib music sound.
 * \param[in] size Number of bytes to write to buffer.
 * \param[in,out] buffer Hold decoded sound data.
 * \return 1 on success, otherwise 0.
 * \note Data written to buffer is 44100/16/mono
 */
PUBLIC W32 ADLIB_UpdateMusic( W32 size, void *buffer )
{
	W8 *al;		//[2] {a, v} (register, value)
	W16 *ptr;
	W32 n;
	W32 AdLibTicks;


	AdLibTicks = size;

	ptr = (PW16) buffer;

	for( n = 0 ; n < AdLibTicks ; ++n )
	{
		while( sqHackLen && (sqHackTime <= alTimeCount) )
		{
			al = (PW8)sqHackPtr++;
			sqHackTime = alTimeCount + LittleShort( *sqHackPtr );
			sqHackPtr++;
			OPLWrite( hAdLib, al[ 0 ], al[ 1 ] );
			sqHackLen -= 4;
		}
		alTimeCount++;


		/* Now we'll get AdLib Output */
		YM3812UpdateOne( hAdLib, ptr, 63 );
		ptr += 63;

		if( sqHackLen <= 0 )
		{
			return (INT_PTR)ptr - (INT_PTR)buffer;
		}
	}

	return (AdLibTicks * ADLIB_MUSIC_BYPS / 700);
}

/**
 * \brief Get music length in milliseconds.
 * \param[in] musbuffer  musicGroup_t data structure.
 * \param[in,out] buffer Hold decoded sound data.
 * \return On success length in milliseconds.
 */
PUBLIC W32 ADLIB_getLength( void *musbuffer )
{
	W16 *ptr;
	W16 length;
	W32 Time;
	W32 alTime;

	ptr = ((musicGroup_t*)musbuffer)->values;
	length = LittleShort( ((musicGroup_t*)musbuffer)->length );
	Time = alTime = 0;


	for( ; ; )
	{
		while( length && (Time <= alTime) )
		{
			ptr++;
			Time = alTime + LittleShort( *ptr );
			ptr++;
			length -= 4;
		}

		alTime++;

		if( length <= 0 )
		{
			break;
		}
	}

	return (alTime * 1000 / 700); // in milliseconds
}

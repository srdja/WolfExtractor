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
 * \file wolfcore.h
 * \brief Wolfenstein 3-D file decoder.
 * \author Michael Liebscher 
 * \date 2007
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */



#ifndef __WOLFCORE_H__
#define __WOLFCORE_H__




#include "../../common/platform.h"



///////////////////////////////////////
//
//	GFX Decoder
//
///////////////////////////////////////

wtBoolean GFXFile_Setup( char *dictfname, char *headfname, char *graphfname );
void GFXFile_Shutdown( void );

void GFXFile_cacheChunk( const W32 chunkId );
void *GFXFile_getChunk( const W32 chunkId );

void *GFXFile_decodeChunk_RGB24( W32 chunkId, W32 *width_out, W32 *height_out, W8 *palette );
void *GFXFile_decodeChunk_RGB32( W32 chunkId, W32 *width_out, W32 *height_out, W8 *palette );

void GFXFile_printPicTable( void );
void GFXFile_decodeFont( W32 fontId, W32 font_width, W32 font_height, const char *path );

wtBoolean GFXFile_decodeScript( W32 textId_start, W32 textId_end, const char *path );

wtBoolean GFXFile_decodeGFX( W32 start, W32 end, W8 *palette, const char *path );


///////////////////////////////////////
//
//	Page File Decoder
//
///////////////////////////////////////

extern const W32 SAMPLERATE;

wtBoolean PageFile_Setup( const char *pagefname, W32 *nBlocks, W32 *SpriteStart, W32 *SoundStart );
void PageFile_Shutdown( void );

void *PageFile_getPage( W32 pagenum, W32 *length );
void *PageFile_decodeWall( W8 *data, W8 *palette );
void *PageFile_decodeSprite( W8 *data, W8 *palette );


wtBoolean PageFile_ReduxDecodePageData( const char *vsfname, const char *wallPath, const char *spritePath, const char *soundPath, W8 *palette );


///////////////////////////////////////
//
//	Audio File Decoder
//
///////////////////////////////////////
wtBoolean AudioFile_Setup( const char *aheadfname, const char *audfname );
void AudioFile_Shutdown( void );

void *AudioFile_CacheAudioChunk( const W32 chunkId );

void AudioFile_dataByteSwap( void *data, W32 length );



wtBoolean AudioFile_ReduxDecodeSound( const W32 start, const W32 end, const char *path );
wtBoolean AudioFile_ReduxDecodeMusic( const W32 start, const W32 end, const char *path, char *songNames[] );


///////////////////////////////////////
//
//	Map Decoder
//
///////////////////////////////////////

typedef struct
{
	float time;
	char timestr[ 6 ];

} parTimes_t;

wtBoolean MapFile_ReduxDecodeMapData( const char *fmaphead, const char *fmap, const char *path,
                                             W8 *palette, const W32 *ceilingColour, char *musicFileName[], parTimes_t *parTimes, char *format );


///////////////////////////////////////
//
//	Redux Decoder
//
///////////////////////////////////////



typedef struct
{
	W32 PN_StatusBar;
	W32 PN_NoKey;
	W32 PN_Blank;
	W32 PN_Title1;
	W32 PN_Title2;
	W32 PN_TitlePalette;
	W32 PN_0;
	W32 PN_1;
	W32 PN_9;
	W32 PN_Colon;
	W32 PN_Num0;
	W32 PN_Num9;
	W32 PN_Percent;
	W32 PN_A;
	W32 PN_Z;
	W32 PN_Expoint;
	W32 PN_Apostrophe;
	W32 PN_IDGuys1;
	W32 PN_IDGuys2;
	W32 PN_EndScreen1;
	W32 PN_EndScreen9;
	W32 PN_HelpScript;
	W32 PN_ScriptStart;
    W32 PN_ScriptEnd;
    W32 PN_bottomInfoPic;

} picNum_t;


void *wolfcore_ReduxGFX( const W32 chunkId, void *data, W32 *width, W32 *height, W32 *ChunkChange, W8 *gamePalette, picNum_t *picNum );


#endif /* __WOLFCORE_H__ */


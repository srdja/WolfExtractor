/*

	Copyright (C) 2006-2009 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolfcore_audio.c
 * \brief Interface to Adlib sound/music decoder.
 * \author Michael Liebscher 
 * \date 2006-2009
 */



#include <stdio.h>

#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../filesys/file.h"

#include "../../console/console.h"


#include "../core/adlib.h"
#include "../../loaders/wav.h"
#include "../../vorbis/vorbisenc_inter.h"

#include "../wolfenstein/wolf.h"


PRIVATE FILE *audiohandle;
PRIVATE W32 *audiostarts;

extern wtBoolean _saveAudioAsWav;
extern wtBoolean _saveMusicAsWav;

/**
 * \brief Setup for audio decoding.
 * \param[in] aheadfname Audio header file name. 
 * \param[in] audfname Audio file name. 
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean AudioFile_Setup( const W8 *aheadfname, const W8 *audfname )
{
	W8 tempFileName[ 1024 ];
	FILE *handle;
	SW32 length;
	SW32 count;		/* Number of bytes read from file */

	if( ! aheadfname || ! *aheadfname ||
		! audfname || ! *audfname )
	{
		fprintf( stderr, "[AudioFile_Setup]: NULL file name!\n" );
		
		return false;	
	}

//
// Load audiohed.XXX (offsets and lengths for audio file)
//
	wt_strlcpy( tempFileName, aheadfname, sizeof( tempFileName ) );
		
    handle = fopen( wt_strupr( tempFileName ), "rb" );
	if( handle == NULL )
	{
		handle = fopen( wt_strlwr( tempFileName ), "rb" );
		
		if( handle == NULL )
		{
			fprintf( stderr, "Could not open file (%s) for read!\n", tempFileName );
			
			return false;
		}
    }

	length = FS_FileLength( handle );
	if( length < 4 )
	{
		fclose( handle );

		fprintf( stderr, "[AudioFile_Setup]: Incorrect audio header size on file: %s\n", tempFileName );

		return false;
	}


	audiostarts = (PW32) MM_MALLOC( length );
	if( audiostarts == NULL )
	{
		fclose( handle );

		return false;
	}


	count = fread( audiostarts, 1, length, handle );
	if( count != length )
    {
        fclose( handle );

        fprintf( stderr, "[AudioFile_Setup]: Read error on file: (%s)", tempFileName  );

		return false;
	}


	fclose( handle );

//
// Open the Audio data file
//
	wt_strlcpy( tempFileName, audfname, sizeof( tempFileName ) );
	
	audiohandle = fopen( wt_strupr( tempFileName ), "rb" );
	if( audiohandle == NULL )
	{
		audiohandle = fopen( wt_strlwr( tempFileName ), "rb" );
		if( audiohandle == NULL )
		{
			fprintf( stderr, "[AudioFile_Setup]: Could not open file (%s) for read!\n", tempFileName ); 
			
			return false;
		}
    }
    
    return true;
}

/**
 * \brief Shutdown audio decoder.
 * \return Nothing.
 */
PUBLIC void AudioFile_Shutdown( void )
{    
    if( audiohandle )
    {
        fclose( audiohandle );
        audiohandle = NULL;
    }
    
    if( audiostarts )
    {
        MM_FREE( audiostarts );
    }    
}

/**
 * \brief Cache audio data.
 * \param[in] chunkId Id of chunk to cache. 
 * \return On success pointer to raw data, otherwise NULL.
 */
PUBLIC void *AudioFile_CacheAudioChunk( const W32 chunkId )
{
	W32	pos;
	W32 chunk_size;
	W32 count;			/* Number of bytes read from file */	
	SW8 *buffer;

//
// Load the chunk into a buffer
//
	pos = LittleLong( audiostarts[ chunkId ] );
	chunk_size = (LittleLong( audiostarts[ chunkId+1 ] )) - pos;
	if( chunk_size < 1 )
	{
		fprintf( stderr, "[AudioFile_CacheAudioChunk]: Chunk size not valid\n" );
		
		return NULL;
	}
	

	if( fseek( audiohandle, pos, SEEK_SET ) != 0 )
	{
		fprintf( stderr, "[AudioFile_CacheAudioChunk]: Could not seek in file!\n" );
		
		return NULL;
	}

	buffer = (PSW8) MM_MALLOC( chunk_size );
	if( buffer == NULL )
	{
		return NULL;
	}

	count = fread( buffer, 1, chunk_size, audiohandle );
	if( count != chunk_size )
	{
		fprintf( stderr, "[AudioFile_CacheAudioChunk]: Read error!\n" );

		MM_FREE( buffer );
		
		return NULL;	
	}

	return (void *)buffer;
}





#ifdef BIG_ENDIAN_SYSTEM

/**
 * \brief Endian byte swapping for wav file.
 * \param[in,out] data Data to byte swap. 
 * \param[in] length Length of data in bytes.
 * \return Nothing.
 * \note Call this function on Big Endian systems.
 */
PUBLIC void AudioFile_dataByteSwap( void *data, SW32 length )
{
	W8 *ptr;
	W8 temp;

	ptr = (PW8) data;
	
	do
	{
		temp = ptr[ 0 ];
		ptr[ 0 ] = ptr[ 1 ]; 
		ptr[ 1 ] = temp;

		length -= 2;
		ptr += 2;

	} while( length );

}

#endif

/**
 * \brief Remap sound index number based on game version
 * \param[in] index Sound index. 
 * \return Remapped sound index.
 */
PRIVATE int GetSoundMappedIndex( int index )
{
    int mappedIndex = index;

    if( wolf_version == ACTIVISION_WL6_V14 )
    {

    }

    return mappedIndex;
}

/**
 * \brief Decode sound fx.
 * \param[in] start Start of sound fx chunks. 
 * \param[in] end End of sound fx chunks.
 * \param[in] path Directory path to save file to.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean AudioFile_ReduxDecodeSound( const W32 start, const W32 end, const W8 *path )
{
	SW8 *buffChunk;
	void *buffWav;
	W32 i;
	W32 length;
	W8 filename[ 1024 ];

	printf( "Decoding Sound FX..." );

	if( ! ADLIB_Init( 22050 ) )
	{			
		return false;
	}


	for( i = start ; i < end ; ++i )
	{
		buffChunk = (PSW8) AudioFile_CacheAudioChunk( i );
		if( buffChunk == NULL )
		{
			continue;
		}


		buffWav = ADLIB_DecodeSound( (AdLibSound *)buffChunk, &length );
		if( buffWav == NULL )
		{
			MM_FREE( buffChunk );

			continue;
		}

	
#ifdef BIG_ENDIAN_SYSTEM
		
		AudioFile_dataByteSwap( buffWav, length );

#endif

        if( _saveAudioAsWav )
        {
		    wt_snprintf( filename, sizeof( filename ), "%s%c%.3d.wav", path, PATH_SEP, GetSoundMappedIndex( i - start ) );
		    wav_write( filename, buffWav, length, 1, 22050, 2 );
        }
        else
        {
            wt_snprintf( filename, sizeof( filename ), "%s%c%.3d.ogg", path, PATH_SEP, GetSoundMappedIndex( i - start ) );
            vorbis_encode( filename, buffWav, length, 1, 16, 22050, 0, 0, 0 );
        }
		
		MM_FREE( buffWav );
		MM_FREE( buffChunk );
	}

	

	ADLIB_Shutdown();

    printf( "Done\n" );

	return true;
}


/**
 * \brief Decode music chunks
 * \param[in] start Start of music chunks. 
 * \param[in] end End of music chunks.
 * \param[in] songNames Song titles.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean AudioFile_ReduxDecodeMusic( const W32 start, const W32 end, const W8 *path, W8 *songNames[] )
{
	SW8 *buffChunk;
	void *buffWav;
	W32 i;
	W32 length;
	W8 filename[ 1024 ];
	W32 uncompr_length;


	printf( "Decoding Music (This could take a while)..." );

	if( ! ADLIB_Init( 44100 ) )
	{			
		return false;
	}



	for( i = start ; i < end ; ++i )
	{
		buffChunk = (PSW8) AudioFile_CacheAudioChunk( i );
		if( buffChunk == NULL )
		{
			continue;
		}


		uncompr_length = ADLIB_getLength( buffChunk );
		if( uncompr_length <= 1 )
		{
			MM_FREE( buffChunk );

			continue;	
		}


		ADLIB_LoadMusic( buffChunk );

		
		buffWav = MM_MALLOC( uncompr_length * 64 * 2 );
		if( buffWav == NULL )
		{
			MM_FREE( buffChunk );

			continue;
		}

		length = ADLIB_UpdateMusic( uncompr_length, buffWav );


		
	
#ifdef BIG_ENDIAN_SYSTEM
		
		AudioFile_dataByteSwap( buffWav, length );

#endif
		
		
        // Save audio buffer
        if( _saveMusicAsWav )
        {

            if( songNames )
		    {
			    wt_snprintf( filename, sizeof( filename ), "%s%c%s.wav", path, PATH_SEP, songNames[ i - start ] );
		    }
		    else
		    {
			    wt_snprintf( filename, sizeof( filename ), "%s%c%d.wav", path, PATH_SEP, i - start );
		    }

            wav_write( filename, buffWav, length, 1, 44100, 2 );
        }
        else
        {

            if( songNames )
		    {
			    wt_snprintf( filename, sizeof( filename ), "%s%c%s.ogg", path, PATH_SEP, songNames[ i - start ] );
		    }
		    else
		    {
			    wt_snprintf( filename, sizeof( filename ), "%s%c%d.ogg", path, PATH_SEP, i - start );
		    }

		    vorbis_encode( filename, buffWav, length, 1, 16, 44100, 0, 0, 0 );
        }
		

		
		MM_FREE( buffWav );
		MM_FREE( buffChunk );
	}


	ADLIB_Shutdown();
	
    printf( "Done\n" );

	return true;
}

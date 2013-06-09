/*

	Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file pak.c
 * \brief Creates PAK files for Wolfenstein 3-D Redux.
 * \author Michael Liebscher
 * \date 2004-2013
 */

#include <string.h>
#include <zlib.h>

#include "../memory/memory.h"
#include "../common/platform.h"
#include "../common/common_utils.h"
#include "../string/wtstring.h"
#include "../common/linklist.h"
#include "../filesys/file.h"
#include "../zip/zip.h"

#include "../wolf/wolfcore_decoder.h"


#define SCRIPTNAME		"DEFAULT.CFG"

#define SCRIPT_DIR		"script"


PRIVATE char defaultscript[] =
"\n \
 unbindall\n\n \
 bind 0 \"impulse 0\"\n \
 bind 1 \"impulse 1\"\n \
 bind 2 \"impulse 2\"\n \
 bind 3 \"impulse 3\"\n \
 bind 4 \"impulse 4\"\n \
 bind 5 \"impulse 5\"\n \
 bind 6 \"impulse 6\"\n \
 bind 7 \"impulse 7\"\n \
 bind 8 \"impulse 8\"\n\n \
 bind    CTRL		+attack\n \
 bind    ALT		+strafe\n\n \
 bind    a          +moveleft\n \
 bind    d          +moveright\n \
 bind    w	        +forward\n \
 bind    s	        +back\n \
 bind    LEFTARROW  +left\n \
 bind    RIGHTARROW +right\n \
 bind	 /			weapnext\n \
 bind    SHIFT 		+speed\n \
 bind    SPACE      +use\n\n \
 bind    PAUSE      \"pause\"\n \
 bind    ESCAPE     \"togglemenu\"\n \
 bind    ~          \"toggleconsole\"\n \
 bind    `          \"toggleconsole\"\n\n \
 bind    TAB        \"toggleautomap\"\n\n \
 bind	 F4         \"menu_keys\"\n \
 bind	 F10        \"menu_quit\"\n \
 bind	 F12        \"screenshot\"\n\n \
 bind    MOUSE1     +attack\n \
 bind    MOUSE2     +strafe\n\n\n \
 set r_fullscreen	1\n \
 set win_noalttab	0\n \
 set sensitivity    300\n \
 set cl_run		    0\n \
 set hand		    0\n \
 set m_pitch		0.022\n \
 set m_yaw		    0.022\n \
 set m_forward		1\n \
 set m_side		    0.8\n \
 set lookspring		1\n \
 set lookstrafe		0\n\n \
 set name Player\n\n \
 alias   DemoLoop      \"intro pc13\"\n\n \
 set g_version      \n\n";

extern W32 _gameVersion;


PRIVATE linkList_t *zipChain = NULL;
PRIVATE linkList_t *zipChainLast = NULL;	/* pointer to last element in zipChain */


/**
 * \brief Writes the Local file chunk for a Zip file.
 * \param[in] filename Pointer to a NUL-terminated string that specifies the path of the file to zip.
 * \param[in,out] fout File stream to add compressed file to.
 * \return On success pointer to zipHead_t structure, otherwise NULL..
 */
PRIVATE zipHead_t *Pak_WriteLocalFileChunk( const char *filename, FILE *fout )
{
	W8 *data;
	zipHead_t *zentry;
	struct filestats fs;
	int err;
	W8 *compr;
	W32 retval;
	z_stream c_stream; /* compression stream */


	zentry = (zipHead_t *) MM_MALLOC( sizeof( *zentry ) );

	memset( zentry, 0, sizeof( *zentry ) );

	zentry->versionmadeby = VMB_VFAT;
	zentry->versionneeded = 20;
	zentry->disknumstart = 0;
	zentry->compression_method = CM_DEFLATED;

	zentry->deletefile = 1;

	zentry->uncompressed_size = FS_FileLoad( filename, (void *) &data );

	if( zentry->uncompressed_size == -1 || data == NULL )
	{
		fprintf( stderr, "[Pak_WriteLocalFileChunk]: Could not open file (%s)\n", filename );
		MM_FREE( zentry );
		MM_FREE( data );

		return NULL;
	}


	FS_GetFileAttributes( filename, &fs );

	zentry->timedate = UnixTimeToDosTime( (time_t *)&fs.lastwritetime );


//
//	Compression
//
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit( &c_stream, Z_DEFAULT_COMPRESSION );
	if( err != Z_OK )
	{
		MM_FREE( data );
		MM_FREE( zentry );

		return NULL;
	}


	zentry->compressed_size = (zentry->uncompressed_size / 10) + 12 + zentry->uncompressed_size;


	compr = (PW8) MM_MALLOC( zentry->compressed_size );





	c_stream.next_out = compr;
	c_stream.avail_out = (uInt)zentry->compressed_size;

	c_stream.next_in = data;
	c_stream.avail_in = (uInt)zentry->uncompressed_size;


	err = deflate( &c_stream, Z_FINISH );
	if( err != Z_STREAM_END )
	{
		MM_FREE( compr );
		MM_FREE( data );
		MM_FREE( zentry );

		return NULL;
	}


	err = deflateEnd( &c_stream );
    if( err != Z_OK )
	{
		MM_FREE( compr );
		MM_FREE( data );
		MM_FREE( zentry );

		return NULL;
	}


	//	When using the deflate method, ZLib adds a 2 byte head
	//	and a 4 byte tail. The head must be removed for zip
	//	compatability and the tail is not necessary.
	zentry->compressed_size = c_stream.total_out - 6;

//
//	End of compression
//


	zentry->crc32 = crc32( 0, data, zentry->uncompressed_size );

	wt_strlcpy( zentry->filename, filename, sizeof( zentry->filename ) );
	zentry->filename_length = strlen( zentry->filename );

	zentry->offset = ftell( fout );

//
// Write header to file
//
	if( ! zip_WriteLocalChunk( zentry, fout ) )
	{
		fprintf( stderr, "[Pak_WriteLocalFileChunk]: Error writing local header to zip file\n" );

		MM_FREE( compr );
		MM_FREE( data );
		MM_FREE( zentry );

		return NULL;
	}

//
// Write data to file
//
	retval = fwrite( compr+2, 1, zentry->compressed_size, fout );
	if( retval != zentry->compressed_size )
	{
		fprintf( stderr, "Error writing data after local header to zip file\n" );

		MM_FREE( compr );
		MM_FREE( data );
		MM_FREE( zentry );

		return NULL;
	}

	MM_FREE( compr );
	MM_FREE( data );

	return zentry;
}

/**
 * \brief Write central headers for Zip file.
 * \param[in] zipList Chain of zlist structures.
 * \param[in,out] fout File stream to write central header to.
 * \return On success true, otherwise false.
 */
PRIVATE wtBoolean Pak_WriteCentralChunk( linkList_t *zipList, FILE *fout )
{
	W32 central_offset;
	W32 central_size;
	W32 num = 0;
	zipHead_t *tempZipHead;

	if( zipList == NULL )
	{
		fprintf( stderr, "[Pak_WriteCentralChunk]: NULL zipList\n" );

		return false;
	}

	central_offset = ftell( fout );

	tempZipHead = zipList->element;
	do
	{
		++num;
		if( ! zip_WriteCentralChunk( tempZipHead, fout ) )
		{
			fprintf( stderr, "Error writing central header to zip file\n" );

			return false;
		}

	} while( (tempZipHead = linkList_GetNextElement( zipList )) );

	central_size = ftell( fout ) - central_offset;

	if( ! zip_WriteEndChunk( num, central_size, central_offset, 0, NULL, fout ) )
	{
		fprintf( stderr, "Error writing end header to zip file\n" );

		return false;
	}

	return true;
}

/**
 * \brief Add directory to zip file.
 * \param[in] path Directory path that will be added to zip file.
 * \param[in,out] f File stream to write compressed files to.
 * \return On success true, otherwise false.
 */
PRIVATE wtBoolean Pak_addDirectoryToZipFile( const char *path, FILE *f )
{
	char temp[ 256 ];
	char *ptr;
	zipHead_t *newZipNode = NULL;


	wt_strlcpy( temp, path, sizeof( temp ) );

	if( strstr( temp, "*" ) == NULL )
	{
		wt_strlcat( temp, "/*", sizeof( temp ) );
	}

	// Look for files
	ptr = FS_FindFirst( temp );

	do {
		wt_snprintf( temp, sizeof( temp ), "%s/%s", path, ptr );

		if ( temp[strlen(temp)-1] == '.' )
		{
			continue;
		}

		if( ! FS_CompareFileAttributes( temp, 0, FA_DIR ) )
		{
			continue;
		}

		newZipNode = Pak_WriteLocalFileChunk( temp, f );
		if( newZipNode == NULL )
		{
			continue;
		}

		// add new zipHead_t to chain
		zipChainLast = linkList_addList( zipChainLast, newZipNode );
	} while( (ptr = FS_FindNext()) != NULL );

	FS_FindClose();

	return true;
}

/**
 * \brief Add script file to zip file.
 * \param[in,out] fout File stream to write compressed files to.
 * \param[in] version  Version to write into script file.
 * \return On success pointer to zlist structure, otherwise NULL.
 */
PRIVATE zipHead_t *Pak_addScriptToZipFile( FILE *fout, W8 version )
{

	W8 *data;
	zipHead_t *zentry;
	struct filestats fs;
	SW32 err;
	W32 retval;
	W32 scriptSize;
	W8 *compr;
	z_stream c_stream; /* compression stream */



	scriptSize = sizeof( defaultscript ) / sizeof( defaultscript[ 0 ] );

	defaultscript[ scriptSize - 5 ] = version + 48;

	data = (PW8) &defaultscript;


	zentry = (zipHead_t *) MM_MALLOC( sizeof( *zentry ) );
	memset( zentry, 0, sizeof( *zentry ) );


	zentry->versionmadeby = VMB_VFAT;
	zentry->versionneeded = 20;
	zentry->disknumstart = 0;
	zentry->compression_method = CM_DEFLATED;

	zentry->deletefile = 0;

	zentry->uncompressed_size = scriptSize;

	//FS_GetFileAttributes( filename, &fs );

	zentry->timedate = UnixTimeToDosTime( (time_t *)&fs.lastwritetime );

//
//	Compression
//
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit( &c_stream, Z_DEFAULT_COMPRESSION );
	if( err != Z_OK )
	{
		MM_FREE( zentry );

		return NULL;
	}


	zentry->compressed_size = (zentry->uncompressed_size / 10) + 12 + zentry->uncompressed_size;


	compr = (PW8) MM_MALLOC( zentry->compressed_size );





	c_stream.next_out = compr;
	c_stream.avail_out = (uInt)zentry->compressed_size;

	c_stream.next_in = data;
	c_stream.avail_in = (uInt)zentry->uncompressed_size;


	err = deflate( &c_stream, Z_FINISH );
	if( err != Z_STREAM_END )
	{
		MM_FREE( compr );
		MM_FREE( zentry );

		return NULL;
	}


	err = deflateEnd( &c_stream );
	if( err != Z_OK )
	{
		MM_FREE( compr );
		MM_FREE( zentry );

		return NULL;
	}


	//	When using the deflate method, ZLib adds a 2 byte head
	//	and a 4 byte tail. The head must be removed for zip
	//	compatability and the tail is not necessary.
	zentry->compressed_size = c_stream.total_out - 6;

//
//	End of compression
//


	zentry->crc32 = crc32( 0, data, zentry->uncompressed_size );

	wt_strlcpy( zentry->filename, SCRIPTNAME, sizeof( zentry->filename ) );
	zentry->filename_length = strlen( zentry->filename );

	zentry->offset = ftell( fout );

//
// Write header to file
//
	if( ! zip_WriteLocalChunk( zentry, fout ) )
	{
		fprintf( stderr, "Error writing local header to zip file\n" );
		MM_FREE( compr );
		MM_FREE( zentry );

		return NULL;
	}

//
// Write data to file
//
	retval = fwrite( compr+2, 1, zentry->compressed_size, fout );
	if( retval != zentry->compressed_size )
	{
		fprintf( stderr, "Error writing data after local header to zip file\n" );
		MM_FREE( compr );
		MM_FREE( zentry );

		return NULL;
	}

	MM_FREE( compr );

	return zentry;
}


/**
 * \brief Delete zip file chain.
 * \param[in] in zlist structure chain to delete.
 * \param[in] deletefile  Delete the file?
 * \return Nothing.
 */
PRIVATE void Pak_DeleteZipFileList( linkList_t *in, wtBoolean deletefile )
{
	zipHead_t *tempZipHead;

	if( in == NULL )
	{
		fprintf( stderr, "[Pak_DeleteZipFileList]: NULL zip list passed in!\n" );

		return;
	}

	if( deletefile )
	{
		printf( "Removing cached files.\n" );
	}

	tempZipHead = in->element;
	do
	{
		if( tempZipHead )
		{
			// delete file
			if( deletefile && tempZipHead->deletefile )
			{
				if( ! FS_DeleteFile( tempZipHead->filename ) )
				{
					fprintf( stderr, "Unable to delete file (%s)\n", tempZipHead->filename );
				}
			}

			MM_FREE( tempZipHead );
		}

	} while( (tempZipHead = (zipHead_t *)linkList_GetNextElement( in )) );

}

/**
 * \brief Remove cache directories.
 */
PUBLIC void RemoveCacheDirectories( )
{
    FS_RemoveDirectory( DIR_MAPS );
    FS_RemoveDirectory( DIR_PICS );
    FS_RemoveDirectory( DIR_WALLS );
    FS_RemoveDirectory( DIR_MUSIC );
    FS_RemoveDirectory( DIR_SOUNDFX );
    FS_RemoveDirectory( DIR_DSOUND );
    FS_RemoveDirectory( DIR_SPRITES );

    FS_RemoveDirectory( DIR_SOD_SPRITES );
    FS_RemoveDirectory( DIR_SOD_DSOUND );
    FS_RemoveDirectory( DIR_SOD_SOUNDFX );
}

/**
 * \brief Builds a PAK file for Wolfenstein 3-D Redux.
 * \param[in] packname Name of PAK file to create.
 * \param[in] version Game version to write to default config file.
 * \param[in] deleteDirectories Delete cache directories after zip?
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean PAK_builder( const char *packname, W8 version, wtBoolean deleteDirectories )
{
	FILE *fout;
	zipHead_t *newZipHead;

	printf( "\n\nGenerating pak file (%s)\nThis could take a few minutes.\n", packname );


	fout = fopen( packname, "wb" );
	if( fout == NULL )
	{
		fprintf( stderr, "[PAK_builder]: Could not create file (%s)\n", packname );

		return false;
	}

	zipChainLast = zipChain = linkList_new();


	/* Script file should be first (first file that is needed by Redux) */
	newZipHead = Pak_addScriptToZipFile( fout, version );
	if( newZipHead == NULL )
	{
		fprintf( stderr, "[PAK_builder]: Unable to add (%s) to pak file\n", SCRIPTNAME );
	}
	else
	{
		zipChainLast = linkList_addList( zipChainLast, newZipHead );
	}




	Pak_addDirectoryToZipFile( DIR_MAPS, fout );
	Pak_addDirectoryToZipFile( DIR_PICS, fout );
	Pak_addDirectoryToZipFile( DIR_WALLS, fout );
	Pak_addDirectoryToZipFile( DIR_MUSIC, fout );


	if( version == 1 )
	{
		Pak_addDirectoryToZipFile( DIR_SOD_SPRITES, fout );
		Pak_addDirectoryToZipFile( DIR_SOD_DSOUND, fout );
		Pak_addDirectoryToZipFile( DIR_SOD_SOUNDFX, fout );
	}
	else
	{
		Pak_addDirectoryToZipFile( DIR_SPRITES, fout );
		Pak_addDirectoryToZipFile( DIR_DSOUND, fout );
		Pak_addDirectoryToZipFile( DIR_SOUNDFX, fout );
	}



	Pak_addDirectoryToZipFile( "script", fout );




	if( ! Pak_WriteCentralChunk( zipChain->next, fout ) )
	{

		Pak_DeleteZipFileList( zipChain->next, false );
		(void)linkList_delete( zipChain );
		zipChainLast = zipChain = NULL;

		// close and delete zip file.
		fclose( fout );

		FS_DeleteFile( packname );

		return false;
	}


	// close zip file.
	fclose( fout );



	// Remove directories
	if( deleteDirectories )
	{
		Pak_DeleteZipFileList( zipChain->next, deleteDirectories );

		RemoveCacheDirectories();
	}

	(void)linkList_delete( zipChain );
	zipChainLast = zipChain = NULL;

	return true;
}


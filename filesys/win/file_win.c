/*

	Copyright (C) 2004-2007 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file file_win.c
 * \brief Handles non-portable file services [Windows].
 * \author Michael Liebscher 
 * \date 2004-2007
 */

#include <windows.h>
#include <time.h>

#include "../file.h"
#include "../../string/wtstring.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../memory/memory.h"


PRIVATE char    findFile[ MAX_PATH ];
PRIVATE HANDLE  FindHandle;
 



/////////////////////////////////////////////////
//
//	File 
//
/////////////////////////////////////////////////


/**
 * \brief Converts file time to UNIX time format.
 * \param[in] ft Pointer to a FILETIME structure containing the file time to convert to UNIX date and time format.
 * \return On success nonzero, otherwise zero.
 */
PRIVATE W32 FS_FileTimeToUnixTime( FILETIME *ft )
{
	time_t days;
	SYSTEMTIME st;

	static const W16 ydays[] =
      { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };



	if( ! FileTimeToSystemTime( ft, &st ) )
	{
		return 0;
	}


	// Protection from overflow
	if( (ft->dwHighDateTime < 0x019DB1DEUL) ||
		((ft->dwHighDateTime == 0x019DB1DEUL) &&
		 (ft->dwLowDateTime < 0xD53E8000UL)) ) 
	{
		return 0;
	}

	if( (ft->dwHighDateTime > 0x0236485EUL) ||
		((ft->dwHighDateTime == 0x0236485EUL) &&
		(ft->dwLowDateTime > 0xD4A5E980UL)) ) 
	{		
		return 0;
	}


	#define leap(y) (((y) % 4 == 0 && (y)% 100 != 0) || (y) % 400 == 0)
	#define nleap(y) (((y) - 1969) / 4 - ((y) - 1901) / 100 + ((y) - 1601) / 400)

	// set 'days' to the number of days into the year
    days = st.wDay - 1 + ydays[ st.wMonth-1 ] + (st.wMonth > 2 && leap( st.wYear ));

    // now set 'days' to the number of days since 1 Jan 1970
    days += 365 * (time_t)(st.wYear - 1970) + (time_t)(nleap(st.wYear));


    return (time_t)(86400L * days + 3600L * (time_t)st.wHour + (time_t)(60 * st.wMinute + st.wSecond));
}

/**
 * \brief Deletes an existing file.
 * \param[in] filename Pointer to a NUL-terminated string that specifies the file to be deleted.
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC wtBoolean FS_DeleteFile( const char *filename )
{
	return (wtBoolean)DeleteFile( filename );	
}

/**
 * \brief Retrieves attributes for a specified file or directory. 
 * \param[in] filename Pointer to a NUL-terminated string that specifies a file or directory.  
 * \param[in] fs Pointer to a filestats structure that receives the attribute information. 
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean FS_GetFileAttributes( const char *filename, struct filestats *fs )
{
	WIN32_FILE_ATTRIBUTE_DATA fdata;

	fs->attributes = 0;
	
	if( ! GetFileAttributesEx( filename, GetFileExInfoStandard, &fdata ) )
	{
		return false;
	}


	if( fdata.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE )
	{
		fs->attributes |= FA_ARCH;
	}

	if( fdata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
	{
		fs->attributes |= FA_HIDDEN;
	}

	if( fdata.dwFileAttributes & FILE_ATTRIBUTE_READONLY )
	{
		fs->attributes |= FA_RDONLY;
	}

	if( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
	{
		fs->attributes |= FA_DIR;
	}

	if( fdata.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM )
	{
		fs->attributes |= FA_SYSTEM;
	}


	// Convert FILETIME to UNIX time.
	fs->creationtime = FS_FileTimeToUnixTime( &fdata.ftCreationTime );
	fs->lastaccesstime = FS_FileTimeToUnixTime( &fdata.ftLastAccessTime );
	fs->lastwritetime = FS_FileTimeToUnixTime( &fdata.ftLastWriteTime );

	return true;
}

/**
 * \brief Compare directory and file attributes. 
 * \param[in] path Specifies the path to compare file attributes. 
 * \param[in] musthave File or directory must have these attributes.
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success true, otherwise false.
 */

PUBLIC wtBoolean FS_CompareFileAttributes( const char *path, W32 musthave, W32 canthave )
{

	DWORD found;

	found = GetFileAttributes( path );

	if( INVALID_FILE_ATTRIBUTES == found )
	{
		return false;
	}

	if( ( found & FILE_ATTRIBUTE_READONLY ) && ( canthave & FA_RDONLY ) )
	{
		return false;
	}

	if( ( found & FILE_ATTRIBUTE_HIDDEN ) && ( canthave & FA_HIDDEN ) )
	{
		return false;
	}

	if( ( found & FILE_ATTRIBUTE_SYSTEM ) && ( canthave & FA_SYSTEM ) )
	{
		return false;
	}

	if( ( found & FILE_ATTRIBUTE_DIRECTORY ) && ( canthave & FA_DIR ) )
	{
		return false;
	}

	if( ( found & FILE_ATTRIBUTE_ARCHIVE ) && ( canthave & FA_ARCH ) )
	{
		return false;
	}





	if( ( musthave & FA_RDONLY ) && !( found & FILE_ATTRIBUTE_READONLY ) )
	{
		return false;
	}

	if( ( musthave & FA_HIDDEN ) && !( found & FILE_ATTRIBUTE_HIDDEN ) )
	{
		return false;
	}

	if( ( musthave & FA_SYSTEM ) && !( found & FILE_ATTRIBUTE_SYSTEM ) )
	{
		return false;
	}

	if( ( musthave & FA_DIR ) && !( found & FILE_ATTRIBUTE_DIRECTORY ) )
	{
		return false;
	}

	if( ( musthave & FA_ARCH ) && !( found & FILE_ATTRIBUTE_ARCHIVE ) )
	{
		return false;
	}

	return true;

}


/////////////////////////////////////////////////
//
//	Directory 
//
/////////////////////////////////////////////////


/**
 * \brief Creates a new directory.
 * \param[in] dirname Pointer to a NUL-terminated string that specifies the path of the directory to be created. 
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean FS_CreateDirectory( const char *dirname )
{
	BOOL ret_val = CreateDirectory( dirname, NULL );

	if( ret_val == 0 && GetLastError() == ERROR_ALREADY_EXISTS )
	{
		return 1;
	}

    return (wtBoolean)ret_val;
}

/**
 * \brief Deletes an existing empty directory. 
 * \param[in] dirname Pointer to a NUL-terminated string that specifies the directory to be deleted. 
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC wtBoolean FS_RemoveDirectory( const char *pathname )
{
	return (wtBoolean)RemoveDirectory( pathname );	
}

/**
 * \brief Gets the current working directory 
 * \return If the function succeeds, a pointer to a NUL-terminated string. Otherwise NULL.
 * \note Caller is responsible for freeing allocated memory.
 */
PUBLIC char *FS_GetCurrentDirectory( void )
{
	DWORD buffLen;
	char *dirName;

	buffLen = GetCurrentDirectory( 0, NULL );

	if( buffLen == 0 )
	{
		return NULL;
	}

	dirName = (char *)MM_MALLOC( buffLen + 1 );

	if( GetCurrentDirectory( buffLen + 1, dirName ) == 0 )
	{
		MM_FREE( dirName );

		return NULL;
	}

	return dirName;
}

/**
 * \brief Changes the current directory 
 * \param[in] path Pointer to a NUL-terminated string that specifies the path to the new directory.  
 * \return On success nonzero, otherwise zero.
 */
PUBLIC wtBoolean FS_ChangeCurrentDirectory( const char *path )
{
	return (wtBoolean)SetCurrentDirectory( path );
}


/////////////////////////////////////////////////
//
//	Find
//
/////////////////////////////////////////////////


/**
 * \brief Searches a directory for a file or subdirectory with a name that matches a specific name.   
 * \param[in] path Pointer to a NUL-terminated string that specifies a valid directory or path and file name that can contain wildcard characters. 
 * \return On success string of file name or directory, otherwise NULL.
 */
PUBLIC char *FS_FindFirst( const char *path )
{
    WIN32_FIND_DATA FindFileData;

	if( FindHandle )
	{
		fprintf( stderr, "[FS_FindFirst]: FS_FindFirst without close\n" );

		return NULL;
	}
  

	FindHandle = FindFirstFile( path, &FindFileData );

	if( FindHandle == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

    wt_strlcpy( findFile, FindFileData.cFileName, sizeof( findFile ) );

	return findFile;
}

/**
 * \brief Continues a file search from a previous call to the FS_FindFirst function. 
 * \return On success string of file name or directory, otherwise NULL.
 */
PUBLIC char *FS_FindNext( void )
{
	WIN32_FIND_DATA FindFileData;

	if( FindHandle == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	if( FindNextFile( FindHandle, &FindFileData ) == 0 )
	{
		return NULL;
	}
		   

    wt_strlcpy( findFile, FindFileData.cFileName, sizeof( findFile ) );
	
	return findFile;
}

/**
 * \brief Closes the search handle.
 */
PUBLIC void FS_FindClose( void )
{
	if( FindHandle != INVALID_HANDLE_VALUE )
	{
		FindClose( FindHandle );
    }
    
	FindHandle = 0;
}



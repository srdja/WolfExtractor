/*
	
	Copyright (C) 2005-2009 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file file_unix.c
 * \brief Handles non-portable file services [UNIX].
 * \author Michael Liebscher 
 * \date 2004-2009
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include <glob.h>


#include "../../memory/memory.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../string/wtstring.h"
#include "../file.h"




PRIVATE	wtBoolean  bFindOn = false;

PRIVATE glob_t glob_results;
PRIVATE W32 findCount = 0;




/////////////////////////////////////////////////
//
//	File
//
/////////////////////////////////////////////////


/**
 * \brief Deletes an existing file.
 * \param[in] filename Pointer to a NUL-terminated string that specifies the file to be deleted.
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC wtBoolean FS_DeleteFile( const char *filename )
{
	return ( ! unlink( filename ) );	
}



/**
 * \brief Retrieves attributes for a specified file or directory. 
 * \param[in] filename Pointer to a NUL-terminated string that specifies a file or directory.  
 * \param[in] fs Pointer to a filestats structure that receives the attribute information. 
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean FS_GetFileAttributes( const char *filename, struct filestats *fs )
{
	struct stat st;

	fs->attributes = 0;
	
	if( stat( filename, &st ) == -1 )
	{
		return false;
	}

	if( st.st_mode & S_IFDIR )
	{
		fs->attributes |= FA_DIR;
	}

	fs->creationtime = st.st_ctime;
	fs->lastaccesstime = st.st_atime;
	fs->lastwritetime = st.st_ctime;

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
	struct stat st;

	if( stat( path, &st ) == -1 )
	{
		return false; 
	}

	if( ( st.st_mode & S_IFDIR ) && ( canthave & FA_DIR ) )
	{
		return false;
	}

	if( ( musthave & FA_DIR ) && !( st.st_mode & S_IFDIR ) )
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
	int ret_val = mkdir( dirname, S_IRUSR | S_IWUSR | S_IXUSR );

	if( ret_val == -1 && errno == EEXIST )
	{
		return 1;
	}
	
	return  (wtBoolean)(! ret_val);
}

/**
 * \brief Deletes an existing empty directory. 
 * \param[in] dirname Pointer to a NUL-terminated string that specifies the directory to be deleted. 
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC wtBoolean FS_RemoveDirectory( const char *pathname )
{
	return ( ! rmdir( pathname ) );	
}

/**
 * \brief Gets the current working directory 
 * \return If the function succeeds, a pointer to a NUL-terminated string. Otherwise NULL.
 * \note Caller is responsible for freeing allocated memory.
 */
PUBLIC char *FS_GetCurrentDirectory( void )
{
	char *path;

	path = MM_MALLOC( MAXPATHLEN );
	if( path == NULL )
	{
		return NULL;
	}

	return getcwd( path, MAXPATHLEN );
}

/**
 * \brief Changes the current directory 
 * \param[in] path Pointer to a NUL-terminated string that specifies the path to the new directory.  
 * \return On success nonzero, otherwise zero.
 */
PUBLIC wtBoolean FS_ChangeCurrentDirectory( const char *path )
{
	return ! chdir( path );
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
 * \note Caller is responsible for freeing allocated memory block by calling MM_FREE.
 */
PUBLIC char *FS_FindFirst( const char *path )
{                                                                               
	if( bFindOn )
	{
		fprintf( stderr, "FS_FindFirst without close\n" );

		return NULL;
	}
               
	                                                     
	if( glob( path, GLOB_NOSORT, NULL, &glob_results ) != 0 )
	{
		return NULL;
	}

	if( glob_results.gl_pathc == 0 )
	{
		return NULL;
	}

	
	bFindOn = true;
	findCount = 1;

	return glob_results.gl_pathv[ 0 ];

}

/**
 * \brief Continues a file search from a previous call to the FS_FindFirst function. 
 * \return On success string of file name or directory, otherwise NULL.
 */
PUBLIC char *FS_FindNext( void )
{
	W32 temp;

	if( ! bFindOn )
	{
		return NULL;
	}

	if( findCount > glob_results.gl_pathc )
	{
		return NULL;
	}

	temp = findCount;
	findCount++;

	return glob_results.gl_pathv[ temp ];
}

/**
 * \brief Closes the search handle.
 */
PUBLIC void FS_FindClose( void )
{
	if( bFindOn )
	{
		globfree( &glob_results );
	}

	bFindOn = false;	
}



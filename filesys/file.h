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
 * \file file.h
 * \brief Access methods to file system services.
 * \author Michael Liebscher 
 * \date 2006-2007 
 */

#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>
#include <time.h>

#include "../common/platform.h"
#include "../common/num_type.h"




extern W32 UnixTimeToDosTime( time_t *t );


///////////////////////////////////////
//
// File Interface
//
///////////////////////////////////////
SW32 FS_FileLength( FILE *filestream );
SW32 FS_FileRead( void *ptr, size_t size, size_t nmemb, FILE *stream );
SW32 FS_FileOpen( const char *filename, FILE **file );
SW32 FS_FileLoad( const char *path, void **buffer );
wtBoolean FS_DeleteFile( const char *filename );



///////////////////////////////////////
//
// Directory Interface
//
///////////////////////////////////////
wtBoolean FS_CreateDirectory( const char *dirname );
wtBoolean FS_RemoveDirectory( const char *pathname );
wtBoolean FS_ChangeCurrentDirectory( const char *path );
char *FS_GetCurrentDirectory( void );




// directory/file attributes
#define FA_ARCH		BIT( 0 )
#define FA_HIDDEN	BIT( 1 )
#define FA_RDONLY	BIT( 2 )
#define FA_DIR		BIT( 3 )
#define FA_SYSTEM	BIT( 4 )

extern wtBoolean FS_CompareFileAttributes( const char *path, W32 musthave, W32 canthave );

struct filestats
{
	W32 attributes;			/* The following attributes are defined as FA_ARCH to FA_SYSTEM*/
	W32 creationtime;		/* Time when file data was created */		
	W32 lastaccesstime;		/* Time when file data last accessed */
	W32 lastwritetime;		/* Time when file data last modified */
};

wtBoolean FS_GetFileAttributes( const char *filename, struct filestats *fs );

char *FS_FindFirst( const char *path );
char *FS_FindNext( void );
void FS_FindClose( void );



//////////////////////////////////////////////
//
// Path and File string name manipulation
//
//////////////////////////////////////////////
char *FS_getFileName( char *path );
void FS_RemoveExtension( const char *in, char *out );
char *FS_getFileExtension( const char *in );
wtBoolean FS_getFileBase( const char *in, char *out, W32 size_out );
wtBoolean FS_getPath( const char *in, char *out, W32 size_out );


#endif /* __FILE_H__ */

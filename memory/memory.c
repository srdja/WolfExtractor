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
 * \file memory.c
 * \brief Memory allocation module.
 * \author Michael Liebscher 
 * \date 2004 
 */

#include <stdlib.h>
#include <stdio.h>

#include "memory.h"



#ifdef PARANOID

	#define PARANOID 1

#endif /* PARANOID */


/**
 * \brief Allocates memory blocks.
 * \param[in] size Bytes to allocate. 
 * \return Void pointer to the allocated space on success, or NULL if there is insufficient memory available.
 */
PUBLIC void *Memory_malloc( size_t size )
{
    void *ptr;
    ptr = malloc( size );
    
    if( ptr != NULL )
    {

#if PARANOID
		
		printf( "Memory malloc: %p size:%ld\n", ptr, (long) size );
		
#endif

        return ptr;
    }
    
 
    printf( "[memory.c] Could not allocate %ld bytes\n", (long) size );
    
    return NULL;
}


/**
 * \brief Allocates an array in memory with elements initialized to 0.
 * \param[in] num Number of elements.
 * \param[in] size Bytes to allocate.
 * \return Void pointer to the allocated space on success, or NULL if there is insufficient memory available.
 */
PUBLIC void *Memory_calloc( size_t num, size_t size )
{
    void *ptr;
    ptr = calloc( num, size );

    if( ptr != NULL )
    {

#if PARANOID
		
		printf( "Memory calloc: %p size:%ld num:%ld\n", ptr, (long) size, (long) num );
		
#endif

        return ptr;
    }


    printf( "[memory.c] Could not allocate %ld objects of size %ld\n", (long) num, (long) size);

    return NULL;
}


/**
 * \brief Reallocate memory blocks.
 * \param[in] memblock Pointer to previously allocated memory block.
 * \param[in] size Bytes to allocate.
 * \return A void pointer to the reallocated (and possibly moved) memory block. The return value is NULL if the size is zero and the buffer argument is not NULL, or if there is not enough  available memory to expand the block to the given size.
 */
PUBLIC void *Memory_realloc( void *memblock, size_t size )
{
    void *ptr;
    
    ptr = realloc( memblock, size );
    
    if( ptr != NULL )
    {

#if PARANOID
		
		printf( "Memory realloc: %p size:%ld\n", ptr, (long) size );
		
#endif

        return ptr;
    }
        
    printf( "[memory.c] Could not reallocate %ld bytes\n", (long) size );
    
    return NULL;
}


/**
 * \brief Deallocates or frees a memory block.
 * \param[in] memblock Previously allocated memory block to be freed.
 * \return Nothing.
 */
PUBLIC void Memory_free( void *memblock )
{
    if( memblock ) 
    {

#if PARANOID
		
		printf( "Memory free: %p\n", memblock );
		
#endif
	    
	    free( memblock );
    }
}

/**
 * \brief Print message out of memory.
 * \param[in] name Name of function this occurred in.
 * \param[in] file File name that this occurred in.
 * \param[in] line Line number this occurred on.
 * \return Nothing.
 */
PUBLIC void Memory_outofmem( const char *name, const char *file, W32 line )
{

    printf( "%s:%ld failed allocation for \"%s\"\n", file, (long) line, name );

}

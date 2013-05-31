/*

	Copyright (C) 2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file linklist.c
 * \brief Generic Link List.
 * \author Michael Liebscher 
 * \date 2005 
 */

#include "linklist.h"

#include "../memory/memory.h"


/**
 * \brief Create a new link list.
 * \return Pointer to new link list structure. 
 */
PUBLIC linkList_t *linkList_new( void )
{
    linkList_t *list;

	list = (linkList_t *) MM_MALLOC( sizeof( linkList_t ) );
    if( list == NULL )
	{
        MM_OUTOFMEM( "linkList_new" );
	}

    list->element = NULL;
    list->next = NULL;

    return list;
}

/**
 * \brief Delete link list.
 * \param[in] list Pointer to linkList_t structure.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean linkList_delete( linkList_t *list )
{
    if( list )
	{
		linkList_t *current;
		while( (current = list) )
		{
			list = list->next;
			MM_FREE( current );
		}

		return true;
    }

    return false;
}

/**
 * \brief Add element to link list.
 * \param[in] list Pointer to linkList_t structure.
 * \param[in] newElement Element to add to link list.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean linkList_add( linkList_t *list, void *newElement )
{
    if( list )
	{
		linkList_t *newNode;
		
		newNode = (linkList_t *) MM_MALLOC( sizeof( linkList_t ) );
		if( newNode == NULL )
		{
			MM_OUTOFMEM( "linkList_add" );
		}

		newNode->element = newElement;
		newNode->next = list->next;
		list->next = newNode;
		
		return true;
    } 
	
    return false;
}

/**
 * \brief Add element to the end of the link list.
 * \param[in] list Pointer to linkList_t structure.
 * \param[in] newElement Element to add to link list.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean linkList_append( linkList_t *list, void *newElement )
{
    if( list )
	{
		while( list->next )
		{
			list = list->next;
		}

		return linkList_add( list, newElement );
    }

    return false;
}

/**
 * \brief Add element to link list.
 * \param[in] list Pointer to linkList_t structure.
 * \param[in] newElement Element to add to link list.
 * \return On success pointer to newly added node, otherwise NULL.
 */
PUBLIC linkList_t *linkList_addList( linkList_t *list, void *newElement )
{
    if( list )
	{
		linkList_t *newNode;

		newNode = (linkList_t *) MM_MALLOC( sizeof( linkList_t ) );
		if( newNode == NULL )
		{
			MM_OUTOFMEM( "linkList_addList" );
		}

		newNode->element = newElement;
		newNode->next = list->next;
		list->next = newNode;

		return newNode;
    } 

    return (linkList_t *) NULL;
}

/**
 * \brief Add element to end of link list.
 * \param[in] list Pointer to linkList_t structure.
 * \param[in] newElement Element to add to link list.
 * \return On success pointer to newly added node, otherwise NULL.
 */
PUBLIC linkList_t *linkList_appendList( linkList_t *list, void *newElement )
{
	if( list )
	{
		while( list->next ) 
		{
			list = list->next;
		}

		return linkList_addList( list, newElement );
	}

	return (linkList_t *) NULL;
}

/**
 * \brief Remove node based on element.
 * \param[in] list Pointer to linkList_t structure.
 * \param[in] element Pointer to element of node to remove.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean linkList_removeNode( linkList_t *current, void *element )
{
    if( current )
	{
		linkList_t *previous;
		while( current->next ) 
		{
			previous = current;
		    current = current->next;
		    if( current->element == element )
			{
				previous->next = current->next;
				MM_FREE( current );

				return true;	
			}
		}
    }

    return false;	
}

/**
 * \brief Remove node.
 * \param[in] current Node to remove.
 * \param[in] previous Previous Node in chain.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean linkList_quickRemoveNode( linkList_t *current, linkList_t *previous )
{
    if( current && previous )
	{
		previous->next = current->next;
		MM_FREE( current );

		return true;	
    }

    return false;
}

/**
 * \brief Delete link list and elements.
 * \param[in] list Pointer to linkList_t structure.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean linkList_deleteFull( linkList_t *list )
{
    if( list )
	{
		linkList_t *current;
		while( (current = list) )
		{
			list = list->next;

			if( current->element )
			{
				MM_FREE( current->element );
			}
			MM_FREE( current );
		}

		return true;
    }

    return false;
}

/**
 * \brief Counts the numer of elements in link list.
 * \param[in] list Pointer to linkList_t structure.
 * \return Returns the numer of elements in the link list.
 */
PUBLIC W32 linkList_count( linkList_t *list )
{
    W32 count = 0;
    
	if( list )
	{
		while( (list = list->next) )
		{
			++count;
		}
	}

    return count;
}


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
 * \file linklist.h
 * \brief Generic Link List.
 * \author Michael Liebscher 
 * \date 2005 
 */

#ifndef __LINKLIST_H__
#define __LINKLIST_H__

#include "platform.h"
#include "common_utils.h"


typedef struct linkList_s 
{
	void *element;
	struct linkList_s *next;

} linkList_t;



linkList_t *linkList_new( void );
wtBoolean linkList_delete( linkList_t *list );
wtBoolean linkList_deleteFull( linkList_t *list );

wtBoolean linkList_add( linkList_t *list, void *newElement );

wtBoolean linkList_append( linkList_t *list, void *newElement );

linkList_t *linkList_addList( linkList_t *list, void *newElement );
linkList_t *linkList_appendList( linkList_t *list, void *newElement );

wtBoolean linkList_removeNode( linkList_t *current, void *element );
wtBoolean linkList_quickRemoveNode( linkList_t *current, linkList_t *previous );


W32 linkList_count( linkList_t *list );


#define	linkList_GetNextElement( x ) ( (x) && ((x) = (x)->next) ? (x)->element : NULL )


#endif /* __LINKLIST_H__ */

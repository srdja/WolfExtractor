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
 * \file mac.h
 * \brief This module is used to extract data from the Macintosh version of Wolfenstein 3-D.
 * \author Michael Liebscher 
 * \date 2004-2013 
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#ifndef __MAC_H__
#define __MAC_H__

#include "../../common/platform.h"

/* Directory Paths */
#define DIRPATHSPRITES	"mac_sprites"
#define DIRPATHWALLS	"mac_walls"
#define DIRPATHPICS		"mac_pics"
#define DIRPATHMIDI		"mac_midi"




void Macintosh_Decoder( void );

W8 *obverseWall( const W8 *src, W16 width, W16 height );


#endif /* __MAC_H__ */


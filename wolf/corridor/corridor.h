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
 * \file corridor.h
 * \brief Corridor 7 data file decoder.
 * \author Michael Liebscher 
 * \date 2007
 */

#ifndef __CORRIDOR_H__
#define __CORRIDOR_H__


#include "../../common/platform.h"

wtBoolean full_corridor_decode_gfx( void );
wtBoolean corridor_decode_gfx( void );


wtBoolean corridor_decode_GFXpm( const char *filename );


#endif /* __CORRIDOR_H__ */
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
 * \file blakestone.h
 * \brief Blake Stone Aliens of Gold/Planet Strike data file decoder.
 * \author Michael Liebscher 
 * \date 2007
 */

#ifndef __BLAKESTONE_DECODE_H__
#define __BLAKESTONE_DECODE_H__


#include "../../common/platform.h"


wtBoolean blake_decode_gfx( char *vgadict, char *vgahead, char *vgagraph );

wtBoolean blake_decode_GFXpm( const char *vswapfname );

wtBoolean blake_decode_Audio( const char *hfname, const char *afname );

#endif /* __BLAKESTONE_DECODE_H__ */


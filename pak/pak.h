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
 * \file pak.h
 * \brief Creates PAK files for Wolfenstein 3-D Redux.
 * \author Michael Liebscher 
 * \date 2004-2007 
 */



#ifndef __PAK_H__
#define __PAK_H__

#include "../common/platform.h"

wtBoolean PAK_builder( const char *packname, W8 version, wtBoolean deleteDirectories );


#endif /* __PAK_H__ */


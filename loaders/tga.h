/*

	Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
	Copyright (C) 1995 Spencer Kimball and Peter Mattis

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
 * \file tga.h
 * \brief Handles Targa file saving.
 * \author Michael Liebscher 
 * \author Spencer Kimball and Peter Mattis
 * \date 1995, 2004
 * \note Portion of this code was derived from The GIMP -- an image manipulation program, and was originally written by Spencer Kimball and Peter Mattis.
 * \note This module is implimented by tga.c
 */

#ifndef __TGA_H__
#define __TGA_H__

#include "../common/platform.h"


W8 TGA_write( const char *filename, W16 depth, W32 width, W32 height, 
            void *Data, W8 upsideDown, W8 rle );


#endif /* __TGA_H__ */


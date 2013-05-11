/*

	Copyright (C) 2006-2009 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file image.h
 * \brief Image manipulation methods.
 * \author Michael Liebscher 
 * \date 2006-2009 
 */

#ifndef __IMAGE_H__
#define __IMAGE_H__



#include "../common/platform.h"


void RGB32toRGB24( const W8 *src, W8 *dest, W32 size );
void RGB24toBGR565( const void *src, void *dest, W32 size );


void RGB24_adjustBrightness( void *data, W32 size );
void RGB32_adjustBrightness( void *data, W32 size );


void MergePics( const W8 *src, W8 *dest, W32 width, W32 height, W32 bpp, W16 totalwidth,
						W32 x_offset, W32 y_offset );

void MergeImages( W8 *src, W32 src_bpp, W32 src_totalwidth, W32 src_region_width, W32 src_region_height, W32 src_x_offset, W32 src_y_offset,
						  W8 *dest, W16 dest_bpp, W32 dest_totalwidth, W32 dest_region_width, W32 dest_region_height,  W32 dest_x_offset, W32 dest_y_offset );

void ReduxAlphaChannel_hq2x( W8 *data, W32 width, W32 height );



#endif /* __IMAGE_H__ */

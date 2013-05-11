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

/*
 *	image.c: Image manipulation.
 *	
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 */

#include "../common/platform.h"
#include "../common/common_utils.h"




/**
 * \brief Convert image from 32-bits to 24-bits.
 * \param[in] src Source buffer to convert from
 * \param[in,out] dest Destination buffer to convert to. 
 * \param[in] size Size of src buffer.
 * \return Nothing.
 * \note src and dest can point to the same memory block.
 */
PUBLIC void RGB32toRGB24( const W8 *src, W8 *dest, W32 size )
{
	W32 i;
	W32 npixels = size >> 2;
	
	for( i = 0 ; i < npixels ; ++i )
	{
		dest[ i * 3 + 0 ] = src[ i * 4 + 0 ];
		dest[ i * 3 + 1 ] = src[ i * 4 + 1 ];
		dest[ i * 3 + 2 ] = src[ i * 4 + 2 ];
	}
}

/**
 * \brief Convert image from 24-bits to 16-bits 565.
 * \param[in] src Source buffer to convert from.
 * \param[in,out] dest Destination buffer to convert to. 
 * \param[in] size Size of src buffer in bytes.
 * \return Nothing.
 * \note src and dest can point to the same memory block.
 */
PUBLIC void RGB24toBGR565( const void *src, void *dest, W32 size )
{
	W32 i;
	W32 npixels;
	W8 *pIn;
	W16 *pOut;
	W16 r,g,b;


	npixels = size / 3;

	
	pIn = (PW8) src;
	pOut = (PW16) dest;
	
	for( i = 0 ; i < npixels ; ++i )
	{		
		r = (W16) ((*(pIn++)) >> 3);
        g = (W16) ((*(pIn++)) >> 2);
        b = (W16) ((*(pIn++)) >> 3);

		*(pOut++) = (W16) ((b << 11) | (g << 5) | r);        
	}
}

/**
 * \brief Adjust the brightness of an RGB24 image.
 * \param[in,out] data RGB24 data to adjust
 * \param[in] size Size of data buffer in bytes.
 * \return Nothing. 
 */
PUBLIC void RGB24_adjustBrightness( void *data, W32 size )
{	
	W8 *ptr;
	W32 i;

	ptr = (PW8) data;

	for( i = 0 ; i < size ; ++i, ptr++ )
	{
		*ptr = (W8) ((*ptr << 2) & 0xFF);
	}
}

/**
 * \brief Adjust the brightness of an RGB32 image.
 * \param[in,out] data RGB32 data to adjust
 * \param[in] size Size of data buffer in bytes.
 * \return Nothing. 
 */
PUBLIC void RGB32_adjustBrightness( void *data, W32 size )
{	
	W8 *ptr;
	W32 i;

	ptr = (PW8) data;

	for( i = 0 ; i < size ; i += 4 )
	{
        ptr[ i + 0 ] = ((ptr[ i + 0 ] << 2) & 0xFF);
        ptr[ i + 1 ] = ((ptr[ i + 1 ] << 2) & 0xFF);  
        ptr[ i + 2 ] = ((ptr[ i + 2 ] << 2) & 0xFF);           
	}
}

/**
 * \brief Merge image inside another image.
 * \param[in] src Source image to copy from.
 * \param[in,out] dest Destination image to copy into. 
 * \param[in] width Width of source image. 
 * \param[in] height Height of source image.
 * \param[in] bpp Bytes per pixel of images.
 * \param[in] totalwidth Total width (in pixels) of destination image.
 * \param[in] x_offset X offset (in pixels) in destination image.
 * \param[in] y_offset Y offset (in pixels) in destination image.
 * \return Nothing. 
 */
PUBLIC void MergePics( const W8 *src, W8 *dest, W32 width, W32 height, W32 bpp, W32 totalwidth,
						W32 x_offset, W32 y_offset )
{
	W32	x, y;
	W8 *dptr;
	
	
	dptr = dest + (y_offset * totalwidth * bpp) + (x_offset * bpp);
	for( y = 0 ; y < height ; ++y )
	{
		for( x = 0 ; x < width ; ++x )
		{	
			dptr[ 0 ] = src[ 0 ];
			dptr[ 1 ] = src[ 1 ];
			dptr[ 2 ] = src[ 2 ];
			
			src += bpp;
			dptr += bpp;
		}

		dptr += (totalwidth * bpp) - width * bpp;
	}
}

/**
 * \brief Merge image region inside another image.
 * \param[in] src Source image to copy from.
 * \param[in] src_bpp Source image bytes per pixel. 
 * \param[in] src_totalwidth Width (in pixels) of source image. 
 * \param[in] src_region_width Width (in pixels) of region to copy from.
 * \param[in] src_region_height Height (in pixels) of region to copy from.
 * \param[in] src_x_offset X offset (in pixels) of region inside source image.
 * \param[in] src_y_offset Y offset (in pixels) of region inside source image. 
 * \param[in,out] dest Destination image to copy into.
 * \param[in] dest_bpp Destination image bytes per pixel. 
 * \param[in] dest_totalwidth Width (in pixels) of destination image. 
 * \param[in] dest_region_width Reserved for Future Use.
 * \param[in] dest_region_height Reserved for Future Use.
 * \param[in] dest_x_offset X offset (in pixels) of region inside destination image.
 * \param[in] dest_y_offset Y offset (in pixels) of region inside destination image.
 * \return Nothing. 
 */
PUBLIC void MergeImages( W8 *src, W32 src_bpp, W32 src_totalwidth, W32 src_region_width, W32 src_region_height, W32 src_x_offset, W32 src_y_offset,
						  W8 *dest, W32 dest_bpp, W32 dest_totalwidth, W32 dest_region_width, W32 dest_region_height,  W32 dest_x_offset, W32 dest_y_offset )
{

	W32	x, y, i;
	W8 *dptr;
	W8 *sptr;
	

	dptr = dest + (dest_y_offset * dest_totalwidth * dest_bpp) + (dest_x_offset * dest_bpp);
	sptr = src + (src_y_offset * src_totalwidth * src_bpp) + (src_x_offset * src_bpp);
	for( y = 0 ; y < src_region_height ; ++y )
	{
		for( x = 0 ; x < src_region_width ; ++x )
		{		
			for( i = 0 ; i < src_bpp ; ++i )
			{
                dptr[ i ] = sptr[ i ]; 
			}						
			
			sptr += src_bpp;
			dptr += dest_bpp;
		}

		dptr += (dest_totalwidth * dest_bpp) - dest_region_width * dest_bpp;
		sptr += (src_totalwidth * src_bpp) - src_region_width * src_bpp;
	}

}

/**
 * \brief Remove fringe from scaled hq2x images.
 * \param[in,out] data Image data.
 * \param[in] width Width (in pixels) of image data. 
 * \param[in] height Height (in pixels) of image image.  
 * \return Nothing. 
 */
PUBLIC void ReduxAlphaChannel_hq2x( W8 *data, W32 width, W32 height )
{

	W8 *ptr;
	W32 i;

	for( i = 0 ; i < (width * height) ; ++i )
	{
		ptr = data + i * 4;	


        if( ptr[ 0 ] >= 150 &&
            ptr[ 1 ] <= 60 &&
            ptr[ 2 ] >= 150 
          )
		{

			ptr[ 3 ] = 0x00;
		}

        // Start

        else if( ptr[ 0 ] >= 124 &&
            ptr[ 1 ] == 0 &&
            ptr[ 2 ] >= 124 
          )
		{
           
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 60 &&
            ptr[ 2 ] >= 170 
          )
		{
           
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 160 &&
            ptr[ 1 ] <= 100 &&
            ptr[ 2 ] >= 220 
          )
		{            
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 40 &&
            ptr[ 2 ] >= 150 
          )
		{
            
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 140 &&
            ptr[ 1 ] <= 90 &&
            ptr[ 2 ] >= 200 
          )
		{
            
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 70 &&
            ptr[ 2 ] >= 190 
          )
		{
          
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 70 &&
            ptr[ 2 ] >= 160 
          )
		{
           
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 200 &&
            ptr[ 1 ] <= 120 &&
            ptr[ 2 ] >= 230 
          )
		{
            
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 150 &&
            ptr[ 1 ] <= 110 &&
            ptr[ 2 ] >= 150 
          )
		{
           
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 120 &&
            ptr[ 1 ] <= 70 &&
            ptr[ 2 ] >= 120 
          )
		{
            
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 100 &&
            ptr[ 1 ] <= 55 &&
            ptr[ 2 ] >= 100 
          )
		{
           
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 180 &&
            ptr[ 1 ] <= 130 &&
            ptr[ 2 ] >= 180 
          )
		{
           
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 200 &&
            ptr[ 1 ] <= 150 &&
            ptr[ 2 ] >= 200 
          )
		{
            
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 190 &&
            ptr[ 1 ] <= 140 &&
            ptr[ 2 ] >= 190 
          )
		{
            
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 170 &&
            ptr[ 1 ] <= 120 &&
            ptr[ 2 ] >= 170 
          )
		{
            
			ptr[ 3 ] = 0x00;
		}
        else if( ptr[ 0 ] >= 130 &&
            ptr[ 1 ] <= 80 &&
            ptr[ 2 ] >= 130 
          )
		{
            
			ptr[ 3 ] = 0x00;
		}

        // End 

		else
		{
			ptr[ 3 ] = 0xFF;
		}
	}

}


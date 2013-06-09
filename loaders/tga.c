/*

	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * \file tga.c
 * \brief Handle Targa file format.
 * \author Michael Liebscher
 * \date 2004-2005
 * \note Portion of this code was derived from The GIMP -- an image manipulation program, and was originally written by Spencer Kimball and Peter Mattis.
 */

#include <string.h>
#include <stdio.h>

#include "../common/platform.h"
#include "../memory/memory.h"
#include "../common/common_utils.h"


/**
 * \brief Run length encode scanline.
 * \param[in] fp Pointer to valid FILE structure.
 * \param[in] buffer Scanline data.
 * \param[in] width Image scanline width.
 * \param[in] bytes Bytes per pixel.
 * \return Nothing.
 */
PRIVATE void rle_write( FILE   *fp,
						W8	*buffer,
						W32	width,
						W32	bytes )
{
	SW32    repeat = 0;
	SW32    direct = 0;
	W8	 *from   = buffer;
	W32    x;

	for( x = 1 ; x < width ; ++x )
	{
		if( memcmp( buffer, buffer + bytes, bytes ) )
		{
			/* next pixel is different */
			if( repeat )
			{
				putc( 128 + repeat, fp );
				fwrite( from, bytes, 1, fp );
				from = buffer + bytes; /* point to first different pixel */
				repeat = 0;
				direct = 0;
			}
			else
			{
				direct += 1;
			}
		}
		else
		{
			/* next pixel is the same */
			if( direct )
			{
				putc( direct - 1, fp );
				fwrite( from, bytes, direct, fp );
				from = buffer; /* point to first identical pixel */
				direct = 0;
				repeat = 1;
			}
			else
			{
				repeat += 1;
			}
		}

		if( repeat == 128 )
		{
			putc( 255, fp );
			fwrite( from, bytes, 1, fp );
			from = buffer + bytes;
			direct = 0;
			repeat = 0;
		}
		else if( direct == 128 )
		{
			putc( 127, fp );
			fwrite( from, bytes, direct, fp );
			from = buffer + bytes;
			direct = 0;
			repeat = 0;
		}

		buffer += bytes;
    }

	if( repeat > 0 )
	{
		putc( 128 + repeat, fp );
		fwrite( from, bytes, 1, fp );
	}
	else
	{
		putc( direct, fp );
		fwrite( from, bytes, direct + 1, fp );
	}
}


/**
 * \brief Write targa image file.
 * \param[in] filename Name of TGA file to save as.
 * \param[in] depth Bytes per pixel. (16, 24 or 32).
 * \param[in] width Width of image in pixels.
 * \param[in] height Height of image in pixels.
 * \param[in] Data Raw image data.
 * \param[in] upsideDown Is the data upside down? 1 yes, 0 no.
 * \param[in] rle Run Length encode? 1 yes, 0 no.
 * \return 0 on error, otherwise 1.
 */
PUBLIC W8 TGA_write( const char *filename, W16 bpp, W32 width, W32 height,
            void *Data, W8 upsideDown, W8 rle )
{
    W16	i, x, y, BytesPerPixel;
	W8	*scanline;
	W8 header[ 18 ];
	FILE *filestream;
	W8 *ptr = (PW8) Data;
	W8 temp;

	BytesPerPixel = bpp >> 3;

	filestream = fopen( filename, "wb" );
    if( filestream == NULL )
	{
		fprintf( stderr, "Could not open file (%s) for write!\n", filename );

		return 0;
	}

	memset( header, 0, 18 );
    header[ 2 ] = rle ? 10 : 2;

    header[ 12 ] = (W8)(width & 255);	/* width low */
    header[ 13 ] = (W8)(width >> 8);	/* width high */

    header[ 14 ] = (W8)(height & 255);	/* height low */
    header[ 15 ] = (W8)(height >> 8);	/* height high */

    header[ 16 ] = (W8)(bpp & 255);	/* pixel size */

    if( upsideDown )
	{
		header[ 17 ] |= 1 << 5; // Image Descriptor
    }


	fwrite( header, sizeof( W8 ), sizeof( header ), filestream  );



	scanline = (PW8) MM_MALLOC( width * BytesPerPixel );
    if( scanline == NULL )
	{
		fclose( filestream );

		return 0;
	}

	for( y = 0 ; y < height ; ++y )
	{
		W32 k = 0;

		for( i = 0 ; i < (width * BytesPerPixel) ; ++i )
		{
			scanline[ k++ ] = ptr[ (height - y - 1) * width * BytesPerPixel + i ];
		}



		if( bpp == 24 || bpp == 32 )
		{
			// swap rgb to bgr
			for( x = 0 ; x < (width * BytesPerPixel) ; x += BytesPerPixel )
			{
				temp = scanline[ x ];
				scanline[ x ] = scanline[ x + 2 ];
				scanline[ x + 2 ] = temp;
			}
		}


		if( rle )
		{
			rle_write( filestream, scanline, width, BytesPerPixel );
		}
		else
		{
			fwrite( scanline, sizeof( W8 ), width * BytesPerPixel, filestream );
		}
	}

	MM_FREE( scanline );

	fclose( filestream );

	return 1;
}

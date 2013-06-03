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

/**
 * \file wolfcore_gfx.c
 * \brief Wolfenstein 3-D GFX file decoder.
 * \author Michael Liebscher 
 * \date 2006-2007
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc. 
 */

#include <stdio.h>
#include <string.h>

#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../filesys/file.h"
#include "../../loaders/tga.h"


typedef struct
{
  // 0-255 is a character, > is a pointer to a node
  W16 bit0;
  W16 bit1;
  	
} huffnode;


typedef struct
{
	W16 width;
    W16 height;
	
} pictable_t;



PRIVATE pictable_t  *pictable;
PRIVATE huffnode	grhuffman[ 255 ];


PRIVATE FILE *file_handle_gfx;
PRIVATE SW32 *grstarts;	/* Array of offsets in vgagraph, -1 for sparse */
PRIVATE W32 numImages;

PRIVATE W32 start_pics	= 3; /* picture start offset. default is 3 */


#define NUM_CHUNKS	256
PRIVATE void *graphic_segments[ NUM_CHUNKS ];



/**
 * \brief Calculate graphic file position.
 * \param[in] chunk Chunk number to calculate file offset.
 * \return File offset value or -1 for sparse tile.
 * \note grstarts must be allocated and initialized before call. Uses global variable grstarts.
 */
PRIVATE SW32 getGFXFilePosition( W32 chunk )
{
	SW32 value;
	W32 offset;
	W8 *ptr;


	offset = chunk * 3;


	ptr = (PW8)grstarts + offset;
	value = ptr[ 0 ] | ptr[ 1 ] << 8 | ptr[ 2 ] << 16 | ptr[ 3 ] << 24;


	value &= 0x00ffffffl;

	if( value == 0xffffffl )
	{
		value = -1;
    }

	return value;
}

/**
 * \brief Calculate length of compressed graphic chunk. 
 * \param[in] chunk Chunk number to calculate file offset.
 * \return The length of the compressed graphic chunk.
 * \note Gets the length of an explicit length chunk (not tiles). The file pointer is positioned so the compressed data can be read in next.
 */
PRIVATE SW32 getGFXChunkLength( W32 chunk )
{   
	fseek( file_handle_gfx, getGFXFilePosition( chunk ) + sizeof( W32 ), SEEK_SET );
	
    return ( getGFXFilePosition( chunk + 1 ) - getGFXFilePosition( chunk ) - 4 );
}


/**
 * \brief Expand Huffman compressed data.
 * \param[in] source Pointer to compressed data.
 * \param[out] destination Pointer to hold decompressed data.
 * \param[in] length Length of expanded data in bytes.
 * \param[in] hufftable Huffman dictionary data.
 * \return Nothing.
 * \note Uses classic Huffman node tree (not optimized).
 */
PRIVATE void HuffExpand( const W8 *source, 
				W8 *destination, 
				W32 length, 
				huffnode *hufftable )
{	
	W32 bx;	/* node pointer */
	W32 dx;
	
	W32 ch;	/* high order of CX */
	W32 cl;	/* low order of CX */

	const W8 *si;	/* Source Index */
	W8 *di;			/* Destination Index */

    bx = 254;   /* head node is always node 254 */
	si = source;
	di = destination;	 
	

    ch = *si;   /* Load first byte */
    si++;
    cl = 1;

        
    do 
    {
        if( (ch & cl) & 0xff )  /* bit set? */
		{
            /* take bit1 path from node */
			dx = LittleShort( hufftable[ bx ].bit1 );  
		}
        else
		{
			/* take bit0 path from node */
            dx = LittleShort( hufftable[ bx ].bit0 );  
		}

		
        cl <<= 1;   /* advance to next bit position */
        
        if( cl & 0x100 )
        {
            ch = *si;   /* Load next byte */
            si++;
            cl = 1;		/* back to first bit */
        }        
        
		if( (cl & 0x100) == 0 )
        {
            if( dx < 256 )
            {
                *di = (W8)(dx & 0xff);    /* Write a decompressed byte out */
                di++;
                bx = 254;   /* back to the head node for next bit */

				length--;
            }
            else
            {
                bx = dx - 256;  /* next node = (huffnode *)code */
            }
        }        
        
    } while( length );
	  
}


/**
 * \brief Expand compressed graphic chunk.
 * \param[in] chunk Chunk number to expand.
 * \param[in] source Pointer to compressed data.
 * \return On success the size of the expaned chunk in bytes, otherwise -1.
 */
PRIVATE SW32 expandGFXChunk( W32 chunk, const W8 *source )
{
	W32 expanded;


	expanded = (source[ 0 ] | source[ 1 ] << 8) | (source[ 2 ] | source[ 3 ] << 8);
	source += 4; /* Skip over length */


//
// Allocate final space and decompress it.
// Sprites need to have shifts made and various other junk.
//
	graphic_segments[ chunk ] = MM_MALLOC( expanded );
	if( graphic_segments[ chunk ] == NULL )
	{
		return -1;
	}
	
	HuffExpand( source, graphic_segments[ chunk ], expanded, grhuffman );

    return expanded;
}


/**
 * \brief Load graphic chunk into memory.
 * \param[in] chunkId Chunk number to cache.
 * \return On success the length of the chunk in bytes, otherwise -1.
 */
PUBLIC SW32 GFXFile_cacheChunk( const W32 chunkId )
{
	SW32	file_offset;
	W32		compressed_size; /* size of compressed chunk in bytes */
	void	*buffer;
	W32		next_chunk;
	SW32	chunkSize;

	
	if( graphic_segments[ chunkId ] )
	{
		return -1;	/* Already in memory */
	}

	
//
// Load the chunk into a buffer
//
	file_offset = getGFXFilePosition( chunkId );
	if( file_offset < 0 )  // $FFFFFFFF start is a sparse tile
	{
		return -1;
	}

	next_chunk = chunkId + 1;
	while( getGFXFilePosition( next_chunk ) == -1 )   // skip past any sparse tiles
	{
		next_chunk++;
	}

	compressed_size = getGFXFilePosition( next_chunk ) - file_offset;

	fseek( file_handle_gfx, file_offset, SEEK_SET );

	
	buffer = MM_MALLOC( compressed_size );
	if( buffer == NULL )
	{
		return -1;
	}


	fread( buffer, 1, compressed_size, file_handle_gfx );
	
	chunkSize = expandGFXChunk( chunkId, buffer );


	MM_FREE( buffer );

    return chunkSize;
}

/**
 * \brief Setup graphic files for decoding.
 * \param[in] dictfname Huffman dictionary file name.
 * \param[in] headfname Graphic header file name.
 * \param[in] graphfname Graphic data file name.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean GFXFile_Setup( const char *dictfname, const char *headfname, const char *graphfname )
{
	FILE *handle;
	SW32 chunk_compressed_length; /* chunk compressed length */
	void *compressed_segment;
	char tempFileName[ 1024 ];
	SW32 filesize;

//
// Load in huffman dictionary
//
	wt_strlcpy( tempFileName, dictfname, sizeof( tempFileName ) );

	if( ( handle = fopen( wt_strupr( tempFileName ), "rb" ) ) ==  NULL )
	{
		if( ( handle = fopen( wt_strlwr( tempFileName ), "rb" ) ) ==  NULL )
		{
			fprintf( stderr, "Could not open file (%s) for read!\n", tempFileName );
			
			return false;
		}
	}


	fread( grhuffman, 1, sizeof( grhuffman ), handle );
	
	fclose( handle );




//
// Load in the data offsets
//	
	wt_strlcpy( tempFileName, headfname, sizeof( tempFileName ) );

	if( (handle = fopen( wt_strupr( tempFileName ), "rb" )) ==  NULL )
	{
		if( (handle = fopen( wt_strlwr( tempFileName ), "rb" )) ==  NULL )
		{
			fprintf( stderr, "Could not open file (%s) for read!\n", tempFileName );
			
			return false;
		}
	}


	filesize = FS_FileLength( handle );

	numImages = filesize / 3;

	grstarts = (PSW32) MM_MALLOC( filesize );
	if( grstarts == NULL )
	{
		if( handle ) {
			fclose( handle );
		}

		return false;
	}
  
	fread( grstarts, 1, filesize, handle );
	
	fclose( handle );


//
// Open the graphics file.
//

	wt_strlcpy( tempFileName, graphfname, sizeof( tempFileName ) );
	
	if( ( file_handle_gfx = fopen( wt_strupr( tempFileName ), "rb" ) ) ==  NULL )
	{
		if( ( file_handle_gfx = fopen( wt_strlwr( tempFileName ), "rb" ) ) ==  NULL )
		{
			fprintf( stderr, "Could not open file (%s) for read!\n", tempFileName );
			
			return false;
		}
	}

	


//
// Load the pic and sprite headers into the arrays.
//
	pictable = (pictable_t *) MM_MALLOC( numImages * sizeof( pictable_t ) );
	if( pictable == NULL )
	{
		return false;
	}


	chunk_compressed_length = getGFXChunkLength( 0 );  // pictable data is located at 0
	
	compressed_segment = MM_MALLOC( chunk_compressed_length );
	if( compressed_segment == NULL )
	{
		return false;
	}

	fread( compressed_segment, 1, chunk_compressed_length, file_handle_gfx );

	
	HuffExpand( compressed_segment, (PW8)pictable,  numImages * sizeof( pictable_t ), grhuffman );

	MM_FREE( compressed_segment ); 
 


    return true;            
}

/**
 * \brief Shutdown graphic files.
 */
PUBLIC void GFXFile_Shutdown( void )
{
    W32 i;

    if( grstarts )
    {
        MM_FREE( grstarts );
    }
  
    if( pictable )
    {
        MM_FREE( pictable );
    }
  
    if( file_handle_gfx )
	{
        fclose( file_handle_gfx );
	}
    
    for( i = 0; i < numImages; ++i )
    {
        if( graphic_segments[ i ] )
        {
            MM_FREE( graphic_segments[ i ] );
        }
    }
	
}

/**
 * \brief Prints out graphic header information [Used for debugging].
 */
PUBLIC void GFXFile_printPicTable( void )
{
	W32 i;

	for( i = 0 ; i < numImages ; ++i )
	{
		printf( "%d w:%d h:%d\n", i, LittleShort( pictable[ i ].width ), LittleShort( pictable[ i ].height ) ); 
	}

}

/**
 * \brief Get raw data.
 * \param[in] chunkId Graphic chunk number.
 * \return Pointer to raw data on success, otherwise NULL.
 */
PUBLIC void *GFXFile_getChunk( const W32 chunkId )
{
	if( graphic_segments[ chunkId ] )
	{
		return (void *)graphic_segments[ chunkId ];
	}

	return NULL;
}

/**
 * \brief Decode graphic chunk into RGB24 image data.
 * \param[in] chunkId Graphic chunk to decode.
 * \param[out] width_out Width of image in pixels.
 * \param[out] height_out Height of image in pixels.
 * \param[in] palette Pointer to image palette (Must have 768 entries).
 * \return Pointer to RGB24 image data on success, otherwise NULL. 
 * \note Caller is responsible for freeing allocated data by calling MM_FREE()
 */
PUBLIC void *GFXFile_decodeChunk_RGB24( W32 chunkId, W32 *width_out, W32 *height_out, W8 *palette )
{
	W32 i;
	W32	picnum;
	W32 width, height;
	W32 linewidth, plane, sx, sy;
	W32 size;
	W32 temp;
	W8 *pic;
	W8 *ptr;
	W8 *buffer;


	if( chunkId < start_pics )
	{
		return NULL;
	}

	picnum = chunkId - start_pics;

	pic = graphic_segments[ chunkId ];
	if( NULL == pic )
	{
		return NULL;
	}

	width  = LittleShort( pictable[ picnum ].width );
	height = LittleShort( pictable[ picnum ].height );

	buffer = (PW8) MM_MALLOC( width * height * 3 );
	if( NULL == buffer )
	{
		return NULL;
	}

	
	linewidth = width / 4;	
	size = width * height;
    for( i = 0 ; i < size ; ++i )
    {
        plane = i / (size / 4);
        sx = (i % (linewidth)) * 4 + plane;
        sy = (i / linewidth) % height;        
	    
        ptr = buffer + (sx * 3 + sy * width * 3); 
		      
		temp = pic[ i ] * 3;  
				
		ptr[ 0 ] = palette[ temp + 0 ];	/* R */
        ptr[ 1 ] = palette[ temp + 1 ];	/* G */
        ptr[ 2 ] = palette[ temp + 2 ];	/* B */  
    }

	*width_out = width;
	*height_out = height;

	return (void *)buffer;
}

/**
 * \brief Decode graphic chunk into RGB32 image data.
 * \param[in] chunkId Graphic chunk to decode.
 * \param[out] width_out Width of image in pixels.
 * \param[out] height_out Height of image in pixels.
 * \param[in] palette Pointer to image palette (Must have 768 entries).
 * \return Pointer to RGB32 image data on success, otherwise NULL. 
 * \note Caller is responsible for freeing allocated data by calling MM_FREE()
 */
PUBLIC void *GFXFile_decodeChunk_RGB32( W32 chunkId, W32 *width_out, W32 *height_out, W8 *palette )
{
	W32 i;
	W32	picnum;
	W32 width, height;
	W32 linewidth, plane, sx, sy;
	W32 size;
	W32 temp;
	W8 *pic;
	W8 *ptr;
	W8 *buffer;


	if( chunkId < start_pics )
	{
		return NULL;
	}

	picnum = chunkId - start_pics;

	pic = graphic_segments[ chunkId ];
	if( NULL == pic )
	{
		return NULL;
	}

	width  = LittleShort( pictable[ picnum ].width );
	height = LittleShort( pictable[ picnum ].height );

	buffer = (PW8) MM_MALLOC( width * height * 4 );
	if( NULL == buffer )
	{
		return NULL;
	}

	
	linewidth = width / 4;	
	size = width * height;
    for( i = 0 ; i < size ; ++i )
    {
        plane = i / (size / 4);
        sx = (i % (linewidth)) * 4 + plane;
        sy = (i / linewidth) % height;        
	    
        ptr = buffer + (sx * 4 + sy * width * 4); 
		      
		temp = pic[ i ] * 3;  
				
		ptr[ 0 ] = palette[ temp + 0 ];	/* R */
        ptr[ 1 ] = palette[ temp + 1 ];	/* G */
        ptr[ 2 ] = palette[ temp + 2 ];	/* B */  

        ptr[ 3 ] = 0xFF;	/* A */  
    }

	*width_out = width;
	*height_out = height;

	return (void *)buffer;
}


/**
 * \brief Set startpic value.
 * \param[in] startpic startpic value. 
 * \return Nothing.
 */
PUBLIC void GFXFile_setStartPicValue( W32 startpic )
{
	start_pics = startpic;
}

/**
 * \brief Get startpic value.
 * \return startpic value.. 
 */
PUBLIC W32 GFXFile_getStartPicValue( void )
{
	return start_pics;
}



typedef struct
{
	W16 height;
	W16 location[ 256 ];
	W8	width[ 256 ];

} fontstruct;

/**
 * \brief Extract and save font as TGA file.
 * \param[in] fontId Font chunk to save.
 * \param[in] font_width Width of font slate in pixels.
 * \param[in] font_height Height of font slate in pixels.
 * \param[in] path Path to save font files to.
 * \return Nothing.
 */
PUBLIC void GFXFile_decodeFont( W32 fontId, W32 font_width, W32 font_height, const char *path )
{
	fontstruct	*sfont;
	W16 i;
	W16 x, y;
	W16 px, py;
	W8 *buffer;	
	W8 *source;
	W8 *ptr;	
	char tempFileName[ 1024 ];
	SW32 chunkSize;

	chunkSize = GFXFile_cacheChunk( fontId );
    if( chunkSize < 0 )
    {
        return;
    }

	sfont = (fontstruct *)GFXFile_getChunk( fontId );	
	

	buffer = (PW8) MM_MALLOC( font_width * font_height * 4 );
	if( buffer == NULL )
	{
		return;
	}

	ptr = buffer;
	for( x = 0; x < font_width; ++x )
	{
		for( y = 0; y < font_height; ++y, ptr += 4 )
		{
			ptr[ 0 ] = ptr[ 1 ] = ptr[ 2 ] = 0xFF;
			ptr[ 3 ] = 0x00;
		}
	}

	px = py = 0;
	for( i = 0; i < 256; ++i )
	{		
		if( ! sfont->width[ i ] )
		{
			continue;
		}
		 
		if( px + sfont->width[ i ] > font_width - 1 )
		{
			py += sfont->height;
			px = 0;
		}
			
		source = ((PW8) sfont) + sfont->location[ i ];

		ptr = buffer + (py * font_width + px) * 4;
		for( y = 0; y < sfont->height; ++y, ptr += font_width * 4 )
		{
			for( x = 0; x < sfont->width[ i ]; ++x )
			{
				if( *source++ )	
				{
					ptr[ x * 4 + 3 ] = 0xFF;
				}
			}
		}
		
		px += 16;

	} // end for i = 0; i < 256; ++i


    wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%cfont%d.tga", path, PATH_SEP, fontId );
	
	TGA_write( tempFileName, 32, font_width, font_height, buffer, 0, 1 );	
	
	MM_FREE( buffer );
}


/**
 * \brief Extract and save game script to file.
 * \param[in] textId_start Text start identifier.
 * \param[in] textId_end Text end identifier.
 * \param[in] path Path to save script into.
 * \return Nothing.
 */
PUBLIC wtBoolean GFXFile_decodeScript( W32 textId_start, W32 textId_end, const char *path )
{
	W8 *text;
    W8 fileName[ 256 ];
	SW32 length;
	W32 i;
	FILE *fHandle;

	if( textId_start == 0 || textId_end == 0 || textId_end <= textId_start )
	{
		return false;
	}
		
	

	for( i = textId_start ; i < textId_end ; ++i )
	{
		length = GFXFile_cacheChunk( i );
        if( length < 0 )
        {
            continue;
        }

		text = (PW8) GFXFile_getChunk( i );	
        if( text == NULL )
        {
            continue;
        }


        wt_snprintf( fileName, sizeof( fileName ), "%s%c%.3d.txt", path, PATH_SEP, i );


        fHandle = fopen( fileName, "wb" );
	    if( ! fHandle )
	    {
		    continue;
	    }
        
		fwrite( (const void *)text, 1, length, fHandle );     

        fclose( fHandle );
	}

	

	return true;
}

/**
 * \brief Decode gfx data into TGA files.
 * \param[in] start start offset.
 * \param[in] end end offset.
 * \param[in] palette Pointer to palette array.
 * \param[in] path Directory path to save files to.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean GFXFile_decodeGFX( W32 start, W32 end, W8 *palette, const char *path )
{
	W32 i;
	void *data;
	W32 width, height;
	char tempFileName[ 1024 ];
    SW32 chunkSize;


	printf( "Decoding GFX data..." );


	GFXFile_setStartPicValue( start );

	
	for( i = start ; i <= end ; ++i )
	{
		chunkSize = GFXFile_cacheChunk( i );
        if( chunkSize < 0 )
        {
            continue;
        }
		
		data = GFXFile_decodeChunk_RGB24( i, &width, &height, palette );		
		if( data == NULL )
		{
			continue;
		}

        wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.tga", path, PATH_SEP, i );

		TGA_write( tempFileName, 24, width, height, data, 0, 1 );

		MM_FREE( data );
	}

    printf( "Done\n" );

	return true;
}


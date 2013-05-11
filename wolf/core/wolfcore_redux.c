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
 * \file wolfcore_redux.c
 * \brief Redux the Wolfenstein 3-D graphics data.
 * \author Michael Liebscher 
 * \date 2006-2009
 */


#include <string.h>

#include "../spear/spear_def.h"

#include "../core/wolfcore.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../filesys/file.h"
#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../loaders/tga.h"

#include "../../image/image.h"
#include "../../image/hq2x.h"
#include "../../image/scalebit.h"

#include "../wolfcore_decoder.h"
#include "../wolfenstein/wolf.h"

extern W32 _filterScale;


/**
 * \brief Scale and/or reassemble image chunks.
 * \param[in] chunkid Chunk id of data.
 * \param[in] data Image data in the form RGB32.
 * \param[out] width Width of image in pixels.
 * \param[out] height Height of image in pixels.
 * \param[out] chunkChange Next iteration use this chunk id.
 * \param[out] gamePalette Palette to Redux image data with.
 * \param[out] picNum Image details.
 * \return Image data on success, otherwise NULL.
 */
PUBLIC void *wolfcore_ReduxGFX( const W32 chunkId, void *data, W32 *width, W32 *height,
                                 W32 *chunkChange, W8 *gamePalette, picNum_t *picNum )
{
    void *scaledImgBuf;		    /* buffer to hold scaled image data */
    W32 width_out, height_out;	/* current Width and Height of image */
    
    W32 tempW, tempH;		/* temp Width and Height of image */
    void *tempData;
    W8 *tempPalette;		/* alternate Palette for image */
    W8 *buffer;
	W8 *ptr;
    
    W32 bytesPerPixel = 4;

	ptr = (PW8) data;
	buffer = NULL;

    width_out = *width;
    height_out = *height;

	                    
    if( chunkId == picNum->PN_StatusBar )
    {
        GFXFile_cacheChunk( picNum->PN_NoKey );
        tempData = GFXFile_decodeChunk_RGB32( picNum->PN_NoKey, &tempW, &tempH, gamePalette );
        
        
        MergePics( tempData, data, tempW, tempH, bytesPerPixel, 320, 240, 4 );
		MergePics( tempData, data, tempW, tempH, bytesPerPixel, 320, 240, 4+tempH );
        
        MM_FREE( tempData );        
    }
    else if( chunkId == picNum->PN_NoKey || chunkId == picNum->PN_Blank )
    {
    	return NULL;
    }
    else if( chunkId >= picNum->PN_EndScreen1 && chunkId <= picNum->PN_EndScreen9 ) /* SOD */
    {

        GFXFile_cacheChunk( SOD_END1PALETTE + (chunkId - picNum->PN_EndScreen1) );
    	tempPalette = (PW8)GFXFile_getChunk( SOD_END1PALETTE + (chunkId - picNum->PN_EndScreen1) );
    	
    	tempData = GFXFile_decodeChunk_RGB32( chunkId, &tempW, &tempH, tempPalette );
        RGB32_adjustBrightness( tempData, tempW * tempH * bytesPerPixel );           	
    	
		MM_MEMCPY( data, tempData, width_out * height_out * bytesPerPixel );

		MM_FREE( tempData );

    }
    else if( chunkId == picNum->PN_bottomInfoPic )
    {
        char fileName[ 1024 ];

        W32 width = 91;
        W32 height = 16;

        W8 *normalBuffer = (PW8) MM_MALLOC( 91 * 16 * 4 );


        // ////////////////////////////////////////////////////////////////////
        // PLAQUE_PAGE

        MergeImages( data, 4, 304, 91, 16, 24, 4,
				     normalBuffer, 4, 91, 91, 16, 0, 0 );
		
        if( _filterScale > 0 )
        {

            W8 *scaledImgBuf = (PW8) MM_MALLOC( (width*2) * (height*2) * 4 );

            if( 2 == _filterScale ) // hq2x
            {
                RGB32toRGB24( normalBuffer, normalBuffer, width * height * 4 );
                RGB24toBGR565( normalBuffer, normalBuffer, width * height * 3 ); 
              
                hq2x_32( (PW8)normalBuffer, (PW8)scaledImgBuf, width, height, width * 2 * bytesPerPixel  );

            }
            else if( 1 == _filterScale ) // Scale2x
            {
                scale( 2, (void *)scaledImgBuf, (width*2) * bytesPerPixel, normalBuffer, width * bytesPerPixel, bytesPerPixel, width, height );			                   
            }

            width *= 2;
            height *= 2;

            MM_FREE( normalBuffer );

            normalBuffer = scaledImgBuf;

        }
		
		RGB32toRGB24( normalBuffer, normalBuffer, width * height * 4 );

		wt_snprintf( fileName, sizeof( fileName ), "%s%c%s.tga", DIR_PICS, PATH_SEP, "PLAQUE_PAGE" );		
        TGA_write( fileName, 24, width, height, normalBuffer, 0, 1 );
		
        // ////////////////////////////////////////////////////////////////////
        // PLAQUE_BLANK

        width = 91;
        height = 16;

        MergeImages( (PW8)data, 4, 304, 91, 16, 192, 4,
					 normalBuffer, 4, 91, 91, 16, 0, 0 );

        wt_snprintf( fileName, sizeof( fileName ), "%s%c%s.tga", DIR_PICS, PATH_SEP, "PLAQUE_BLANK" );
		TGA_write( fileName, 32, width, height, normalBuffer, 0, 1 );

		if( _filterScale > 0 )
        {

            W8 *scaledImgBuf = (PW8) MM_MALLOC( (width*2) * (height*2) * 4 );

            if( 2 == _filterScale ) // hq2x
            {
                RGB32toRGB24( normalBuffer, normalBuffer, width * height * 4 );
                RGB24toBGR565( normalBuffer, normalBuffer, width * height * 3 ); 
              
                hq2x_32( (PW8)normalBuffer, (PW8)scaledImgBuf, width, height, width * 2 * bytesPerPixel  );

            }
            else if( 1 == _filterScale ) // Scale2x
            {
                scale( 2, (void *)scaledImgBuf, (width*2) * bytesPerPixel, normalBuffer, width * bytesPerPixel, bytesPerPixel, width, height );			                    
            }

            width *= 2;
            height *= 2;


            MM_FREE( normalBuffer );

            normalBuffer = scaledImgBuf;

        }
        
        RGB32toRGB24( normalBuffer, normalBuffer, width * height * 4 );

		wt_snprintf( fileName, sizeof( fileName ), "%s%c%s.tga", DIR_PICS, PATH_SEP, "PLAQUE_BLANK" );
		TGA_write( fileName, 24, width, height, normalBuffer, 0, 1 );

        MM_FREE( normalBuffer );

        return NULL;
    }
    else if( chunkId == picNum->PN_Title1 ) /* SOD */
    {
		W8 *tempBuf;

		GFXFile_cacheChunk( picNum->PN_TitlePalette );
		tempPalette = GFXFile_getChunk( picNum->PN_TitlePalette );

		tempBuf = GFXFile_decodeChunk_RGB32( picNum->PN_Title1, &tempW, &tempH, tempPalette );

		RGB32_adjustBrightness( tempBuf, tempW * tempH * bytesPerPixel );

		GFXFile_cacheChunk( picNum->PN_Title2 );
		tempData = GFXFile_decodeChunk_RGB32( picNum->PN_Title2, &tempW, &tempH, tempPalette );
      
        RGB32_adjustBrightness( tempData, tempW * tempH * bytesPerPixel );
        
        buffer = (PW8) MM_MALLOC( tempW * (tempH + height_out) * bytesPerPixel );
        
        MM_MEMCPY( buffer, tempBuf, width_out * height_out * bytesPerPixel );
        MM_MEMCPY( buffer + (width_out * height_out * bytesPerPixel), tempData, tempW * tempH * bytesPerPixel );

        
        height_out += tempH;
       
        ptr = buffer;
        
        *chunkChange = (picNum->PN_Title2 - picNum->PN_Title1);

		MM_FREE( tempBuf );
		MM_FREE( tempData );
    }
    else if( chunkId == picNum->PN_IDGuys1 ) /* SOD */
    {
		W8 *tempBuf;

		GFXFile_cacheChunk( SOD_IDGUYSPALETTE );
        tempPalette = GFXFile_getChunk( SOD_IDGUYSPALETTE );

		tempBuf = GFXFile_decodeChunk_RGB32( picNum->PN_IDGuys1, &tempW, &tempH, tempPalette );

		RGB32_adjustBrightness( tempBuf, tempW * tempH * bytesPerPixel );
    	
        GFXFile_cacheChunk( SOD_IDGUYS2PIC );
        tempData = GFXFile_decodeChunk_RGB32( SOD_IDGUYS2PIC, &tempW, &tempH, tempPalette );
        
        RGB32_adjustBrightness( tempData, tempW * tempH * bytesPerPixel );
        
        buffer = (PW8) MM_MALLOC( tempW * (tempH + height_out) * bytesPerPixel );
        
        MM_MEMCPY( buffer, tempBuf, width_out * height_out * bytesPerPixel );
        MM_MEMCPY( buffer + (width_out * height_out * bytesPerPixel), tempData, tempW * tempH * bytesPerPixel );

        
        height_out += tempH;
       
        ptr = buffer;
        
        *chunkChange = (SOD_IDGUYS2PIC - SOD_IDGUYS1PIC);

		MM_FREE( tempBuf );
		MM_FREE( tempData );
	}
	else if( chunkId == picNum->PN_0 )
	{
		W32 i;
    	W32 offset;
    	
    	offset = 0;
    	
    	/* create a clean slate */
		buffer = (PW8)MM_MALLOC( 90 * height_out * bytesPerPixel );
		
		/* colour fill the background rgb to 0,0,160 */
        for( i = 0 ; i < (90 * height_out * bytesPerPixel) ; i += bytesPerPixel )
        {
            buffer[ i + 0 ] = 0;    /* R */
            buffer[ i + 1 ] = 0;    /* G */
            buffer[ i + 2 ] = 160;  /* B */

            buffer[ i + 3 ] = 0x00;  /* A */
        }
		
		/* copy '0' to slate */
		MergePics( data, buffer, width_out, height_out, bytesPerPixel, 90, offset, 0 );
		
		/* copy '1' through '9' to slate */
        offset = width_out + 1;
        for( i = picNum->PN_1 ; i <= picNum->PN_9 ; ++i )
        {
            GFXFile_cacheChunk( i );
            tempData = GFXFile_decodeChunk_RGB32( i, &tempW, &tempH, gamePalette );
            
            MergePics( tempData, buffer, tempW, tempH, bytesPerPixel, 90, offset, 0 );
            
            offset += tempW + 1;
            
            MM_FREE( tempData );
        }
        
        width_out = 90;
        
        ptr = buffer;   
        
        *chunkChange = (picNum->PN_9 - picNum->PN_0);     
	}
	else if( chunkId == picNum->PN_Colon )
    {
    	W32 i;
    	W32 offset;
    	W32 y_offset;
    	
    	/* Create our clean slate */
        buffer = (PW8) MM_MALLOC( 256 * 64 * bytesPerPixel );
      
        /* colour fill the background RGB to 0,64,64 */
        for( i = 0 ; i < (256 * 64 * bytesPerPixel) ; i += 4 )
        {
            buffer[ i + 0 ] = 0;
            buffer[ i + 1 ] = 64;
            buffer[ i + 2 ] = 64;
            
            buffer[ i + 3 ] = 0xFF;

        }
      
        /* copy colon to slate */
        MergePics( data, buffer, width_out, height_out, bytesPerPixel, 256, 160, 16 ); 
        
        /* copy numbers to slate */
        offset = 0;
        for( i = picNum->PN_Num0 ; i <= picNum->PN_Num9 ; ++i )
        {
            GFXFile_cacheChunk( i );
            tempData = GFXFile_decodeChunk_RGB32( i, &tempW, &tempH, gamePalette );
            
            MergePics( tempData, buffer, tempW, tempH, bytesPerPixel, 256, offset, 16 );
            
            offset += tempW;
            
            MM_FREE( tempData );
        }
        
        /* copy percent sign to slate */        
        GFXFile_cacheChunk( picNum->PN_Percent );
        tempData = GFXFile_decodeChunk_RGB32( picNum->PN_Percent, &tempW, &tempH, gamePalette );
        
        MergePics( tempData, buffer, tempW, tempH, bytesPerPixel, 256, 80, 0 );
        
		MM_FREE( tempData );
        
        /* copy letters to slate */ 
        offset = 16;
        y_offset = 32;
        for( i = picNum->PN_A ; i <= picNum->PN_Z ; ++i )
        {
            GFXFile_cacheChunk( i );
            tempData = GFXFile_decodeChunk_RGB32( i, &tempW, &tempH, gamePalette );
            
            MergePics( tempData, buffer, tempW, tempH, bytesPerPixel, 256, offset, y_offset );
            
            offset += tempW;
            
            if( offset >= 256 )
			{
				offset = 0;
				y_offset += 16;
			}
            
            MM_FREE( tempData );
        }
        
        /* copy exclamation point to slate */        
        GFXFile_cacheChunk( picNum->PN_Expoint );
        tempData = GFXFile_decodeChunk_RGB32( picNum->PN_Expoint, &tempW, &tempH, gamePalette );
        
        MergePics( tempData, buffer, tempW, tempH, bytesPerPixel, 256, 16, 0 );
        
        MM_FREE( tempData );
        
        /* copy apostrophe to slate */        
        if( wolf_version >= APOGEE_WL6_V11 )
        {
            GFXFile_cacheChunk( picNum->PN_Apostrophe );
            tempData = GFXFile_decodeChunk_RGB32( picNum->PN_Apostrophe, &tempW, &tempH, gamePalette );
        
            MergePics( tempData, buffer, tempW, tempH, bytesPerPixel, 256, 112, 0 );
        
            MM_FREE( tempData );            
        }
                
        
        width_out = 256;
        height_out = 64;
        
        ptr = buffer;
        
        
        *chunkChange = (picNum->PN_Apostrophe - picNum->PN_Colon);
    }
      
    
    if( _filterScale > 0  )
    {

        scaledImgBuf = MM_MALLOC( (width_out * 2) * (height_out * 2) * bytesPerPixel );
        if( scaledImgBuf == NULL )
        {
    	    return NULL;
        }


        // ////////////////////////////////////////////////////////////////////////
        // Scale Image

        if( 2 == _filterScale ) // hq2x
        {
            RGB32toRGB24( ptr, ptr, width_out * height_out * 4 );
            RGB24toBGR565( ptr, ptr, width_out * height_out * 3 ); 
          
            hq2x_32( (PW8)ptr, (PW8)scaledImgBuf, width_out, height_out, width_out * 2 * bytesPerPixel  );

        }
        else if( 1 == _filterScale ) // Scale2x
        {
            scale( 2, (void *)scaledImgBuf, (width_out*2) * bytesPerPixel, ptr, width_out * bytesPerPixel, bytesPerPixel, width_out, height_out );			                
        }

        width_out *= 2;
        height_out *= 2;

        if( buffer )
        {
	        MM_FREE( buffer );
        }
        
        *width = width_out;
        *height = height_out;
    		
        return scaledImgBuf;

    }


    
    if( buffer )
    {
        *width = width_out;
        *height = height_out;
		
        return ptr;
    }
        
    
    
    *width = width_out;
    *height = height_out;
		
    return data;
}


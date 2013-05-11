/*

	Copyright (C) 2006-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolfcore_decoder.h
 * \brief Wolfenstein 3-D front end to data file decoder.
 * \author Michael Liebscher 
 * \date 2006-2013 
 */

#ifndef __WOLFCORE_DECODER_H__
#define __WOLFCORE_DECODER_H__


#include "../common/num_type.h"


// Output directories
#define DIR_PICS        "pics"
#define DIR_WALLS       "walls"
#define DIR_SPRITES     "sprites"
#define DIR_DSOUND      "sfx"
#define DIR_SOUNDFX     "lsfx"
#define DIR_MUSIC       "music"
#define DIR_MAPS        "maps"
#define DIR_GSCRIPTS    "gscripts"

#define DIR_SOD_SPRITES     "sodsprites"
#define DIR_SOD_DSOUND      "sodsfx"
#define DIR_SOD_SOUNDFX     "sodlsfx"

typedef enum _WDExtFlags 
{
    FND_WOLF_FULL			= BIT(  0 ),		/* Wolfenstein 3-D v1.4 */        
    FND_WOLF_SHARE		    = BIT(  1 ),		/* Wolfenstein 3-D Shareware */    
	FND_MACWOLF				= BIT(  3 ),		/* Wolfenstein 3-D for Macintosh */
	FND_3D0WOLF				= BIT(  4 ),		/* Wolfenstein 3-D for 3D0 */
	FND_JAGUARWOLF			= BIT(  5 ),		/* Wolfenstein 3-D for Jaguar */
	FND_SPEAR_FULL			= BIT(  6 ),		/* Spear of Destiny */
    FND_SPEAR_DEMO			= BIT(  7 ),		/* Spear of Destiny Demo */    
    FND_BLAKE_STONE_A_FULL	= BIT(  8 ),		/* Blake Stone: Aliens of Gold */
    FND_BLAKE_STONE_A_SHARE	= BIT(  9 ),		/* Blake Stone: Aliens of Gold Shareware */
    FND_BLAKE_STONE_PS		= BIT( 10 ),		/* Blake Stone: Planet Strike */    
    FND_CORRIDOR7_FULL		= BIT( 11 ),		/* Corridor 7 */
    FND_CORRIDOR7_SHARE		= BIT( 12 ),		/* Corridor 7 Shareware */
	FND_SUPER3D_NOAHS_ARK	= BIT( 13 ),		/* Super 3D Noah's Ark */
	FND_OP_BODYCOUNT_FULL	= BIT( 14 ),		/* Operation Body Count */
	FND_OP_BODYCOUNT_SHARE	= BIT( 15 ),		/* Operation Body Count Shareware */

    FND_ALL					= (W32) 0xFFFFFFFF	/* Must be last */

} WDExtFlags;


 



void wolfDataDecipher( void );

wtBoolean buildCacheDirectories( void );





#endif /* __WOLFCORE_DECODER_H__ */



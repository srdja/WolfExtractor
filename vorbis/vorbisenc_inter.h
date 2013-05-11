/*

	Copyright (C) 2005-2009 Michael Liebscher
	Copyright 2000-2002, Michael Smith <msmith@xiph.org>
	(c) Kenneth Arnold <kcarnold@yahoo.com>
	(c) Monty <monty@xiph.org>

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
 * \file vorbisenc_inter.h
 * \brief Simple Ogg Vorbis interface.
 * \author Michael Liebscher
 * \date 2005-2009
 * \note Portions from oggenc, Copyright 2000-2002, Michael Smith 
 * Vorbize, (c) Kenneth Arnold and libvorbis examples, (c) Monty 
 */

#ifndef __VORBISENC_INTER_H__
#define __VORBISENC_INTER_H__

int vorbis_encode( const char *filename, void *data, W32 size, W32 in_channels, W32 in_samplesize,
			   W32 rate, W32 quality, W32 max_bitrate, W32 min_bitrate  );

#endif /* __VORBISENC_INTER_H__ */


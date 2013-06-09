/*

	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file zip.h
 * \brief Zip file interface.
 * \author Michael Liebscher
 * \date 2004-2005
 */

#ifndef __ZIP_H__
#define __ZIP_H__

#include "../common/platform.h"

/* compression method */
#define CM_NO_COMPRESSION		0
#define CM_SHRUNK				1
#define CM_REDUCED_F1			2
#define CM_REDUCED_F2			3
#define CM_REDUCED_F3			4
#define CM_REDUCED_F4			5
#define CM_IMPLODED				6
#define CM_RESERVED_7			7
#define CM_DEFLATED				8
#define CM_DEFLATE64			9
#define CM_IMPLODING			10
#define CM_RESERVED_11			11
#define CM_BZIP2				12


/* version made by */
#define VMB_MSDOS_FAT			0
#define VMB_AMIGA				1
#define VMB_OPENVMS				2
#define VMB_UNIX				3
#define VMB_VM_CMS				4
#define VMB_ATARI_ST			5
#define VMB_OS_2_HPFS			6
#define VMB_MACINTOSH			7
#define VMB_Z_SYSTEM			8
#define VMB_CP_M				9
#define VMB_WINDOWS_NTFS		10
#define VMB_MVS_OS_390_Z_OS		11
#define VMB_VSE					12
#define VMB_ACORN_RISC			13
#define VMB_VFAT				14
#define VMB_ALTERNATE_MVS		15
#define VMB_BEOS				16
#define VMB_TANDEM				17
#define VMB_OS_400				18
#define VMB_OS_X_DARWIN			19




typedef struct zipHead_s
{
	W16 versionmadeby;
	W16 versionneeded;
	W16 flag;
	W16 compression_method;
	W32 timedate;
	W32 crc32;
	W32 compressed_size;
	SW32 uncompressed_size;
	W16 filename_length;
	W16 extrafield_length;
	W16 centralextra_length;
	W16 comment_length;
	W16 disknumstart;
	W16 internalattribute;
	W16 localflag;
	W32 externalattribute;
	W32 offset;
	wtBoolean deletefile;
	char filename[ 256 ];
	char extrafield[ 256 ];
	char centralextra[ 256 ];
	char comment[ 256 ];

} zipHead_t;


wtBoolean zip_WriteLocalChunk( zipHead_t *z, FILE *f );
wtBoolean zip_WriteCentralChunk( zipHead_t *z, FILE *f );
wtBoolean zip_WriteEndChunk( W16 num, W32 size, W32 offset, W16 len, char *comment, FILE *f );



#endif /* __ZIP_H__ */


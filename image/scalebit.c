/*
 * This file is part of the Scale2x project.
 *
 * Copyright (C) 2003, 2004 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * This file contains an example implementation of the Scale effect
 * applyed to a generic bitmap.
 *
 * You can find an high level description of the effect at :
 *
 * http://scale2x.sourceforge.net/
 *
 * Alternatively at the previous license terms, you are allowed to use this
 * code in your program with these conditions:
 * - the program is not used in commercial activities.
 * - the whole source code of the program is released with the binary.
 * - derivative works of the program are allowed.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "scale2x.h"

#include "../common/platform.h"

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <assert.h>
#include <stdlib.h>

#define SSDST(bits, num) (scale2x_uint##bits *)dst##num
#define SSSRC(bits, num) (const scale2x_uint##bits *)src##num

/**
 * Apply the Scale2x effect on a group of rows. Used internally.
 */
static INLINECALL void stage_scale2x(void* dst0, void* dst1, const void* src0, const void* src1, const void* src2, unsigned pixel, unsigned pixel_per_row)
{
	switch (pixel) {
#if defined(__GNUC__) && defined(__i386__)
		case 1 : scale2x_8_mmx(SSDST(8,0), SSDST(8,1), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), pixel_per_row); break;
		case 2 : scale2x_16_mmx(SSDST(16,0), SSDST(16,1), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), pixel_per_row); break;
		case 4 : scale2x_32_mmx(SSDST(32,0), SSDST(32,1), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), pixel_per_row); break;
#else
		case 1 : scale2x_8_def(SSDST(8,0), SSDST(8,1), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), pixel_per_row); break;
		case 2 : scale2x_16_def(SSDST(16,0), SSDST(16,1), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), pixel_per_row); break;
		case 4 : scale2x_32_def(SSDST(32,0), SSDST(32,1), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), pixel_per_row); break;
#endif
	}
}

/**
 * Apply the Scale2x3 effect on a group of rows. Used internally.
 */
static INLINECALL void stage_scale2x3(void* dst0, void* dst1, void* dst2, const void* src0, const void* src1, const void* src2, unsigned pixel, unsigned pixel_per_row)
{
	switch (pixel) {
#if defined(__GNUC__) && defined(__i386__)
		case 1 : scale2x3_8_mmx(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), pixel_per_row); break;
		case 2 : scale2x3_16_mmx(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), pixel_per_row); break;
		case 4 : scale2x3_32_mmx(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), pixel_per_row); break;
#else
		case 1 : scale2x3_8_def(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), pixel_per_row); break;
		case 2 : scale2x3_16_def(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), pixel_per_row); break;
		case 4 : scale2x3_32_def(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), pixel_per_row); break;
#endif
	}
}

/**
 * Apply the Scale2x4 effect on a group of rows. Used internally.
 */
static INLINECALL void stage_scale2x4(void* dst0, void* dst1, void* dst2, void* dst3, const void* src0, const void* src1, const void* src2, unsigned pixel, unsigned pixel_per_row)
{
	switch (pixel) {
#if defined(__GNUC__) && defined(__i386__)
		case 1 : scale2x4_8_mmx(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSDST(8,3), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), pixel_per_row); break;
		case 2 : scale2x4_16_mmx(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSDST(16,3), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), pixel_per_row); break;
		case 4 : scale2x4_32_mmx(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSDST(32,3), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), pixel_per_row); break;
#else
		case 1 : scale2x4_8_def(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSDST(8,3), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), pixel_per_row); break;
		case 2 : scale2x4_16_def(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSDST(16,3), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), pixel_per_row); break;
		case 4 : scale2x4_32_def(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSDST(32,3), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), pixel_per_row); break;
#endif
	}
}

/**
 * Apply the Scale4x effect on a group of rows. Used internally.
 */
static INLINECALL void stage_scale4x(void* dst0, void* dst1, void* dst2, void* dst3, const void* src0, const void* src1, const void* src2, const void* src3, unsigned pixel, unsigned pixel_per_row)
{
	stage_scale2x(dst0, dst1, src0, src1, src2, pixel, 2 * pixel_per_row);
	stage_scale2x(dst2, dst3, src1, src2, src3, pixel, 2 * pixel_per_row);
}

#define SCDST(i) (dst+(i)*dst_slice)
#define SCSRC(i) (src+(i)*src_slice)
#define SCMID(i) (mid[(i)])

/**
 * Apply the Scale2x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 2x2 times the size of the source bitmap.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
static void scale2x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	unsigned char* dst = (unsigned char*)void_dst;
	const unsigned char* src = (const unsigned char*)void_src;
	unsigned count;

	assert(height >= 2);

	count = height;

	stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(0), SCSRC(1), pixel, width);

	dst = SCDST(2);

	count -= 2;
	while (count) {
		stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(1), SCSRC(2), pixel, width);

		dst = SCDST(2);
		src = SCSRC(1);

		--count;
	}

	stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(1), SCSRC(1), pixel, width);

#if defined(__GNUC__) && defined(__i386__)
	scale2x_mmx_emms();
#endif
}

/**
 * Apply the Scale2x3 effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 2x3 times the size of the source bitmap.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
static void scale2x3(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	unsigned char* dst = (unsigned char*)void_dst;
	const unsigned char* src = (const unsigned char*)void_src;
	unsigned count;

	assert(height >= 2);

	count = height;

	stage_scale2x3(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(0), SCSRC(1), pixel, width);

	dst = SCDST(3);

	count -= 2;
	while (count) {
		stage_scale2x3(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(1), SCSRC(2), pixel, width);

		dst = SCDST(3);
		src = SCSRC(1);

		--count;
	}

	stage_scale2x3(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(1), SCSRC(1), pixel, width);

#if defined(__GNUC__) && defined(__i386__)
	scale2x_mmx_emms();
#endif
}

/**
 * Apply the Scale2x4 effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 2x4 times the size of the source bitmap.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
static void scale2x4(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	unsigned char* dst = (unsigned char*)void_dst;
	const unsigned char* src = (const unsigned char*)void_src;
	unsigned count;

	assert(height >= 2);

	count = height;

	stage_scale2x4(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCSRC(0), SCSRC(0), SCSRC(1), pixel, width);

	dst = SCDST(4);

	count -= 2;
	while (count) {
		stage_scale2x4(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCSRC(0), SCSRC(1), SCSRC(2), pixel, width);

		dst = SCDST(4);
		src = SCSRC(1);

		--count;
	}

	stage_scale2x4(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCSRC(0), SCSRC(1), SCSRC(1), pixel, width);

#if defined(__GNUC__) && defined(__i386__)
	scale2x_mmx_emms();
#endif
}


/**
 * Check if the scale implementation is applicable at the given arguments.
 * \param scale Scale factor. 2, 203 (fox 2x3), 204 (for 2x4), 3 or 4.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 * \return
 *   - -1 on precondition violated.
 *   - 0 on success.
 */
int scale_precondition(unsigned scale, unsigned pixel, unsigned width, unsigned height)
{
	if (pixel != 1 && pixel != 2 && pixel != 4)
		return -1;

	switch (scale) {
	case 202 :
	case 203 :
	case 204 :
	case 2 :
	case 303 :
	case 3 :
		if (height < 2)
			return -1;
		break;
	case 404 :
	case 4 :
		if (height < 4)
			return -1;
		break;
	default:
		return -1;
	}

	if (width < 2)
		return -1;

	return 0;
}

/**
 * Apply the Scale effect on a bitmap.
 * This function is simply a common interface for ::scale2x(), ::scale3x() and ::scale4x().
 * \param scale Scale factor. 2, 203 (fox 2x3), 204 (for 2x4), 3 or 4.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
void scale(unsigned scale, void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	switch (scale) {
	case 202 :
	case 2 :
		scale2x(void_dst, dst_slice, void_src, src_slice, pixel, width, height);
		break;
	case 203 :
		scale2x3(void_dst, dst_slice, void_src, src_slice, pixel, width, height);
		break;
	case 204 :
		scale2x4(void_dst, dst_slice, void_src, src_slice, pixel, width, height);
		break;	
	}
}


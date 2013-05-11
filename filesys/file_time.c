/*

	Copyright (C) 2006 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file file_time.c
 * \brief Portable file time conversion.
 * \author Michael Liebscher 
 * \date 2006 
 */

#include <time.h>


#include "../common/common_utils.h"
#include "../common/platform.h"



// DOS time start date is January 1, 1980
#define DOSTIME_STARTDATE         0x00210000L


/**
 * \brief Convert the date y/n/d and time h:m:s to a four byte DOS date and time (date in high two bytes, time in low two bytes)
 * \param[in] y year.
 * \param[in] c month.
 * \param[in] d day.
 * \param[in] h hour.
 * \param[in] m minute.
 * \param[in] s second.
 * \return DOS time.
 */
#define DOSTIME( y, c, d, h, m, s ) ( ((y) < 1980) ? DOSTIME_STARTDATE : (((W32)(y) - 1980) << 25) | ((W32)(c) << 21) | ((W32)(d) << 16) |  ((W32)(h) << 11) | ((W32)(m) << 5) | ((W32)(s) >> 1) )


/**
 * \brief Converts Unix time_t into DOS format.
 * \param[in] t unix time to convert.
 * \return Unix time_t in DOS format, rounded up to the next two second boundary.
 */
PUBLIC W32 UnixTimeToDosTime( time_t *t )
{
  time_t t_even;
  struct tm *s;         

  // Round up to even seconds.
  t_even = (time_t)(((W32)(*t) + 1) & (~1));
                                
  s = localtime( &t_even );
  if( s == (struct tm *)NULL ) 
  {
      // time conversion error; use current time instead         
      t_even = (time_t)(((W32)time(NULL) + 1) & (~1));
      s = localtime( &t_even );
  }

  return DOSTIME( s->tm_year + 1900, s->tm_mon + 1, s->tm_mday,
                 s->tm_hour, s->tm_min, s->tm_sec );
}



/*

	Copyright (C) 2005-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file main.c
 * \brief Interface to Classic Wolfenstein data decoder.
 * \author Michael Liebscher
 * \date 2005-2013
 */


/*
	wolfextractor(3) 		Wolfextractor Manual			wolfextractor(3)


		NAME
			wolfextractor -- Decodes Wolfenstein 3-D data files

		SYNOPSIS
			wolfextractor

		DESCRIPTION
			The wolfextractor utility is used to decipher Wolfenstein 3-D data
			files.

			The options are as follows:

			-h		Prints out a help message.

            -s X	Set graphic scale filter [ 0 = No scaling, 1 = Use Scale2x, 2 = Use hq2x ]

            -n      Do not Redux image data.

            -d      Do not zip directories.

            -w      Save audio data as WAV.

		SEE ALSO

*/


#include <stdlib.h>
#include <stdio.h>

#include "common/common_utils.h"
#include "string/wtstring.h"
#include "wolf/wolfcore_decoder.h"
#include "console/console.h"
#include "filesys/file.h"
#include "image/hq2x.h"





#if OS_WINDOWS

    #include "getopt/getopt.h"

#else

    #include <ctype.h>
    #include <getopt.h>

#endif




wtBoolean _force = false;
W32 _filterScale = 2;
W32 _filterScale_Sprites = 1;
wtBoolean _doRedux = true;
wtBoolean _outputInDirectory = false;
wtBoolean _saveAudioAsWav = true;
wtBoolean _saveMusicAsWav = false;
W32 _gameVersion = 0;


extern const char *APPLICATION_STRING;
extern const char *VERSION_STRING;


/**
 * \brief Parse the command line.
 * \param[in] argc Number of arguments.
 * \param[in] argv Arguments.
 * \return On success true, otherwise false.
 */
PRIVATE wtBoolean ParseCommandLine( int argc, char *const *argv )
{

	SW32 retValue;

    while( (retValue = getopt( argc, argv, "fndws:" )) != -1 )
	{
		switch( retValue )
		{
            case 'F':
			case 'f':
				_force = true;
				break;

            case 'N':
            case 'n':
				_doRedux = false;
				break;

            case 'D':
            case 'd':
				_outputInDirectory = true;
				break;

            case 'W':
            case 'w':
				_saveAudioAsWav = true;
                _saveMusicAsWav = true;
				break;

            case 'S':
            case 's':
                if( 0 == wt_stricmp( "0", optarg ) ) // original
                {
                    _filterScale = 0;
                    _filterScale_Sprites = 0;
                }
                else if( 0 == wt_stricmp( "1", optarg ) )  // Scale2x
                {
                   _filterScale = 1;
                   _filterScale_Sprites = 1;
                }
                else if( 0 == wt_stricmp( "2", optarg ) )  // hq2x
                {
                   _filterScale = 2;
                   _filterScale_Sprites = 2;
                }
                else
                {
                    fprintf (stderr, "Option -%c requires a valid argument [0 -Original, 1 -Scale2x, 2 - hq2x].\n", optopt );
                    return false;
                }
                break;

			case '?':
                if (optopt == 's')
                {
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                }

				if( isprint( optopt ) )
				{
					fprintf( stderr, "Unknown option `-%c'.\n", optopt );
				}
				else
				{
					fprintf( stderr, "Unknown option character `\\x%x'.\n", optopt );
				}
				return false;
            default:
                {
                    fprintf( stderr, "Unknown option '%c'.\n", (char) retValue);
                    break;
                }
		}
	}

	return true;

}

/**
 * \brief Display application version information.
 */
PRIVATE void displayVersionMsg( void )
{
	CPrintMsg( TC_GREEN, "\n%s %s %s\n", APPLICATION_STRING, PLATFORM_NAME, CPUSTRING );
	CPrintMsg( TC_GREEN, "\nVersion %s built on %s at %s\n\n\n\n", VERSION_STRING, __DATE__, __TIME__ );
}

/**
 * \brief Interface to Wolfenstein data decoder
 * \param[in] argc Size of argv.
 * \param[in] argv Holds command line arguments
 * \return 0 on success, non-zero otherwise.
 * \note This is the application entry point.
 * 1. Search for Wolfenstein data files.
 * 2. Decode data accordingly.
 */
PUBLIC int main( int argc, char *argv[] )
{

	ParseCommandLine( argc, argv );

	InitLUTs();

	/* Setup our console window */
	ConsoleWindow_Init();

	displayVersionMsg();


	wolfDataDecipher();


	/* Wait until a key is pressed before shutting down. */
	CWaitForConsoleKeyInput();


	/* Shut down our console window */
	ConsoleWindow_Shutdown();

	return 0;

}

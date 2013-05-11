
project (WolfExtractor) 


# Required cmake version
cmake_minimum_required ( VERSION 2.8 )

set( EXE_NAME "WolfExtractor" )


set( env_SOURCE 
	${CMAKE_SOURCE_DIR}/zlib/adler32.c
	${CMAKE_SOURCE_DIR}/wolf/core/adlib.c
	${CMAKE_SOURCE_DIR}/wolf/blakestone/blakestone.c
	${CMAKE_SOURCE_DIR}/wolf/blakestone/blakestone_pal.c
	${CMAKE_SOURCE_DIR}/zlib/compress.c	
	${CMAKE_SOURCE_DIR}/wolf/corridor/corridor.c
	${CMAKE_SOURCE_DIR}/wolf/corridor/corridor_pal.c
	${CMAKE_SOURCE_DIR}/zlib/crc32.c
	${CMAKE_SOURCE_DIR}/zlib/deflate.c
	${CMAKE_SOURCE_DIR}/filesys/file.c
	${CMAKE_SOURCE_DIR}/filesys/file_string.c
	${CMAKE_SOURCE_DIR}/filesys/file_time.c	
	${CMAKE_SOURCE_DIR}/wolf/core/fmopl.c
	${CMAKE_SOURCE_DIR}/getopt/getopt.c
	${CMAKE_SOURCE_DIR}/image/hq2x.c
	${CMAKE_SOURCE_DIR}/image/image.c
	${CMAKE_SOURCE_DIR}/common/linklist.c
	${CMAKE_SOURCE_DIR}/wolf/mac/mac.c
	${CMAKE_SOURCE_DIR}/main.c
	${CMAKE_SOURCE_DIR}/memory/memory.c
	${CMAKE_SOURCE_DIR}/wolf/jaguar/jaguar.c
	${CMAKE_SOURCE_DIR}/wolf/noah/noah.c
	${CMAKE_SOURCE_DIR}/wolf/noah/noah_gamepal.c
	${CMAKE_SOURCE_DIR}/wolf/bodycount/obc.c
	${CMAKE_SOURCE_DIR}/wolf/bodycount/obc_gamepal.c
	${CMAKE_SOURCE_DIR}/wolf/bodycount/obc_menupal.c
	${CMAKE_SOURCE_DIR}/pak/pak.c
	${CMAKE_SOURCE_DIR}/common/platform.c
	${CMAKE_SOURCE_DIR}/image/scale2x.c
	${CMAKE_SOURCE_DIR}/image/scalebit.c
	${CMAKE_SOURCE_DIR}/wolf/spear/spear.c
	${CMAKE_SOURCE_DIR}/wolf/spear/spear_name.c
	${CMAKE_SOURCE_DIR}/wolf/spear/spear_pal.c
	${CMAKE_SOURCE_DIR}/loaders/tga.c
	${CMAKE_SOURCE_DIR}/zlib/trees.c
	${CMAKE_SOURCE_DIR}/version.c
	${CMAKE_SOURCE_DIR}/vorbis/vorbisenc_inter.c
	${CMAKE_SOURCE_DIR}/loaders/wav.c
	${CMAKE_SOURCE_DIR}/wolf/wolfenstein/wolf.c
	${CMAKE_SOURCE_DIR}/wolf/wolfenstein/wolf_pal.c
	${CMAKE_SOURCE_DIR}/wolf/core/wolfcore_audio.c
	${CMAKE_SOURCE_DIR}/wolf/wolfcore_decoder.c
	${CMAKE_SOURCE_DIR}/wolf/core/wolfcore_gfx.c
	${CMAKE_SOURCE_DIR}/wolf/core/wolfcore_map.c
	${CMAKE_SOURCE_DIR}/wolf/core/wolfcore_pm.c
	${CMAKE_SOURCE_DIR}/wolf/core/wolfcore_redux.c
	${CMAKE_SOURCE_DIR}/string/wtstring.c
	${CMAKE_SOURCE_DIR}/string/wtstringnumeric.c
	${CMAKE_SOURCE_DIR}/zip/zipfile.c
	${CMAKE_SOURCE_DIR}/zlib/zutil.c
)
	
set( env_HEADER 	
			
	${CMAKE_SOURCE_DIR}/wolf/core/adlib.h
	${CMAKE_SOURCE_DIR}/wolf/blakestone/blakestone.h
	${CMAKE_SOURCE_DIR}/common/common_utils.h
	${CMAKE_SOURCE_DIR}/console/console.h
	${CMAKE_SOURCE_DIR}/wolf/corridor/corridor.h
	${CMAKE_SOURCE_DIR}/zlib/crc32.h
	${CMAKE_SOURCE_DIR}/zlib/deflate.h
	${CMAKE_SOURCE_DIR}/filesys/file.h
	${CMAKE_SOURCE_DIR}/wolf/core/fmopl.h
	${CMAKE_SOURCE_DIR}/getopt/getopt.h
	${CMAKE_SOURCE_DIR}/getopt/getopt_int.h
	${CMAKE_SOURCE_DIR}/image/hq2x.h
	${CMAKE_SOURCE_DIR}/image/image.h
	${CMAKE_SOURCE_DIR}/common/linklist.h
	${CMAKE_SOURCE_DIR}/wolf/mac/mac.h
	${CMAKE_SOURCE_DIR}/memory/memory.h
	${CMAKE_SOURCE_DIR}/wolf/jaguar/jaguar.h
	${CMAKE_SOURCE_DIR}/wolf/noah/noah.h
	${CMAKE_SOURCE_DIR}/common/num_type.h
	${CMAKE_SOURCE_DIR}/wolf/bodycount/obc.h
	${CMAKE_SOURCE_DIR}/pak/pak.h
	${CMAKE_SOURCE_DIR}/common/platform.h
	${CMAKE_SOURCE_DIR}/image/scale2x.h
	${CMAKE_SOURCE_DIR}/image/scalebit.h
	${CMAKE_SOURCE_DIR}/wolf/spear/spear_def.h
	${CMAKE_SOURCE_DIR}/loaders/tga.h
	${CMAKE_SOURCE_DIR}/zlib/trees.h
	${CMAKE_SOURCE_DIR}/vorbis/vorbisenc_inter.h
	${CMAKE_SOURCE_DIR}/loaders/wav.h
	${CMAKE_SOURCE_DIR}/wolf/wolfenstein/wolf.h
	${CMAKE_SOURCE_DIR}/wolf/core/wolfcore.h
	${CMAKE_SOURCE_DIR}/wolf/wolfcore_decoder.h
	${CMAKE_SOURCE_DIR}/string/wtstring.h
	${CMAKE_SOURCE_DIR}/zip/zip.h
	${CMAKE_SOURCE_DIR}/zlib/zutil.h
)


if ( WIN32 )

	set( platform_SOURCE 
	
		${CMAKE_SOURCE_DIR}/console/win32/console_win.c
		${CMAKE_SOURCE_DIR}/filesys/win/file_win.c
	
	)
	
elseif(UNIX)

	set( platform_SOURCE 
	
		${CMAKE_SOURCE_DIR}/console/unix/console_unix.c
		${CMAKE_SOURCE_DIR}/filesys/unix/file_unix.c
	
	)
	set( COMPILER_FLAGS -g -pg -Wall -Wextra -pedantic )

else ()

	message (FATAL_ERROR "Unknown Platform")

endif ()

# merge platforms
set( SOURCE ${env_SOURCE} ${platform_SOURCE} )
set( HEADER ${env_HEADER} )


add_executable( ${EXE_NAME} ${SOURCE} ${HEADER} )

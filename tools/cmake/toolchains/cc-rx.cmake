# Note, Cmake doesn't support cc-rx compiler.
# Even it finds the executable, it doesn't know its identificatioin.
# TODO: create a patch to Cmake

SET(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/cc-rx.cmake)

include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

# Find Renesas CC-RX.
afr_find_compiler(AFR_COMPILER_CC ccrx)
afr_find_compiler(AFR_COMPILER_ASM asrx)
set(AFR_COMPILER_CXX "${AFR_COMPILER_CC}" CACHE INTERNAL "")

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${AFR_COMPILER_CC} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${AFR_COMPILER_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${AFR_COMPILER_ASM} CACHE FILEPATH "ASM compiler")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_ASM_COMPILER_FORCED TRUE)

set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS s;src;S;asm)

# Specify compiler option.
SET(CMAKE_INCLUDE_FLAG_C "-include=")
SET(CMAKE_INCLUDE_FLAG_ASM "-include=")

SET (CMAKE_C_COMPILE_OBJECT "${CMAKE_C_COMPILER} <FLAGS> <INCLUDES> -output=obj=<OBJECT> <SOURCE>")
SET (CMAKE_CXX_COMPILE_OBJECT "${CMAKE_CXX_COMPILER} <FLAGS> <INCLUDES> -output=obj=<OBJECT> <SOURCE>")
set(CMAKE_ASM_COMPILE_OBJECT "${CMAKE_ASM_COMPILER} <FLAGS> <INCLUDES> -output=<OBJECT> <SOURCE>")

# Add target system root to cmake find path.
get_filename_component(AFR_COMPILER_DIR "${AFR_COMPILER_CC}" DIRECTORY)
get_filename_component(CMAKE_FIND_ROOT_PATH "${AFR_COMPILER_DIR}" DIRECTORY)

# Don't look for executable in target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

# Specify the archiver
SET(CMAKE_AR rlink)

# Specify archiver option
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 0)
set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> -form=library=u -output=<TARGET> -noprelink -nomessage -list -show=all -nooptimize -nologo <OBJECTS>")
#set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> -form=library=u -noprelink -nomessage -list -show=all -nooptimize -nologo <OBJECTS>")

# Specify the linker
SET(CMAKE_C_LINK_EXECUTABLE rlink)
#SET(CMAKE_CXX_LINK_EXECUTABLE rlink)

# Specify the make
SET(CMAKE_MAKE_PROGRAM make)

# Specify the Converter
SET(CMAKE_CONVERTER_EXECUTABLE rlink)

#specify X converter
SET(CMAKE_COMMAND "renesas_cc_converter")
### END CMAKE_TOOLCHAIN_FILE
# rar library

add_compile_options(-DRARDLL)
add_compile_options(-DUNRAR)
add_compile_options(-DSILENT)
add_compile_options(-D_CRT_SECURE_NO_DEPRECATE)
add_compile_options(-D_CRT_NONSTDC_NO_DEPRECATE)

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

set(LIBUNRAR unrar)

set(LIBUNRARSOURCES
    ${LIBUNRAR}/archive.cpp
    ${LIBUNRAR}/arcread.cpp
    ${LIBUNRAR}/blake2s.cpp
    ${LIBUNRAR}/cmddata.cpp
    ${LIBUNRAR}/consio.cpp
    ${LIBUNRAR}/crc.cpp
    ${LIBUNRAR}/crypt.cpp
    ${LIBUNRAR}/dll.cpp
    ${LIBUNRAR}/encname.cpp
    ${LIBUNRAR}/errhnd.cpp
    ${LIBUNRAR}/extinfo.cpp
    ${LIBUNRAR}/extract.cpp
    ${LIBUNRAR}/filcreat.cpp
    ${LIBUNRAR}/file.cpp
    ${LIBUNRAR}/filefn.cpp
    ${LIBUNRAR}/filestr.cpp
    ${LIBUNRAR}/find.cpp
    ${LIBUNRAR}/getbits.cpp
    ${LIBUNRAR}/global.cpp
    ${LIBUNRAR}/hash.cpp
    ${LIBUNRAR}/headers.cpp
    ${LIBUNRAR}/isnt.cpp
    ${LIBUNRAR}/match.cpp
    ${LIBUNRAR}/options.cpp
    ${LIBUNRAR}/pathfn.cpp
    ${LIBUNRAR}/qopen.cpp
    ${LIBUNRAR}/rar.cpp
    ${LIBUNRAR}/rarpch.cpp
    ${LIBUNRAR}/rarvm.cpp
    ${LIBUNRAR}/rawread.cpp
    ${LIBUNRAR}/rdwrfn.cpp
    ${LIBUNRAR}/rijndael.cpp
    ${LIBUNRAR}/rs.cpp
    ${LIBUNRAR}/rs16.cpp
    ${LIBUNRAR}/scantree.cpp
    ${LIBUNRAR}/secpassword.cpp
    ${LIBUNRAR}/sha1.cpp
    ${LIBUNRAR}/sha256.cpp
    ${LIBUNRAR}/smallfn.cpp
    ${LIBUNRAR}/strfn.cpp
    ${LIBUNRAR}/strlist.cpp
    ${LIBUNRAR}/system.cpp
    ${LIBUNRAR}/threadpool.cpp
    ${LIBUNRAR}/timefn.cpp
    ${LIBUNRAR}/ui.cpp
    ${LIBUNRAR}/unicode.cpp
    ${LIBUNRAR}/unpack.cpp
    ${LIBUNRAR}/volume.cpp
    ${LIBUNRAR}/dll.rc
    ${LIBUNRAR}/dll.def
)

set(rc_flags "/r /c1252")
set(CMAKE_RC_FLAGS ${rc_flags})
ADD_MSVC_PRECOMPILED_HEADER("rar.hpp" "rarpch.cpp" ${LIBUNRARSOURCES})

add_library(unrar SHARED 
    ${LIBUNRARSOURCES}
)


if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DBZ_DEBUG) # enable extra assertions
endif()

set(LIBBZ2_SOURCES
    bzip2/blocksort.c
    bzip2/huffman.c
    bzip2/crctable.c
    bzip2/randtable.c
    bzip2/compress.c
    bzip2/decompress.c
    bzip2/bzlib.c)

add_library(libbz2 ${LIBBZ2_SOURCES})
set_target_properties(libbz2 PROPERTIES ARCHIVE_OUTPUT_NAME bz2) # reqiured for FindBzip2 to work
if(BUILD_SHARED_LIBS)
    target_compile_definitions(libbz2 PRIVATE -DBZ_BUILD_DLL)
endif()

install(TARGETS libbz2
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

if(NOT BZIP2_SKIP_TOOLS)
    add_executable(bzip2 bzip2/bzip2.c ${LIBBZ2_SOURCES})
    add_executable(bzip2recover bzip2/bzip2recover.c ${LIBBZ2_SOURCES})
    install(TARGETS bzip2 bzip2recover DESTINATION tools)
endif()

if(NOT BZIP2_SKIP_HEADERS)
    install(FILES bzip2/bzlib.h DESTINATION include)
endif()

add_subdirectory(rhash)
add_subdirectory(keccak)
# if (MSVC)
#    set_target_properties(unrar PROPERTIES COMPILE_FLAGS "/Yurar.hpp")
#    set_source_files_properties(dll.cpp PROPERTIES COMPILE_FLAGS "/Ycrar.hpp")
# endif(MSVC)
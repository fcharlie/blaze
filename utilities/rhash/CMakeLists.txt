project(librhash C)

# Disable warnings to avoid changing 3rd party code.
if(CMAKE_C_COMPILER_ID MATCHES
    "^(GNU|Clang|AppleClang|XL|VisualAge|SunPro|MIPSpro|HP|Intel)$")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
elseif(CMAKE_C_COMPILER_ID STREQUAL "PathScale")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -woffall")
endif()

set(librhash_sources
  librhash/aich.c
  librhash/algorithms.c
  librhash/algorithms.h
  librhash/byte_order.c
  librhash/byte_order.h
  librhash/crc32.c
  librhash/crc32.h
  librhash/ed2k.c
  librhash/gost.c
  librhash/has160.c
  librhash/hex.c
  librhash/hex.h
  librhash/md4.c
  librhash/md5.c
  librhash/md5.h
  librhash/rhash.c
  librhash/rhash.h
  librhash/rhash_torrent.c
  librhash/ripemd-160.c
  librhash/sha1.c
  librhash/sha1.h
  librhash/sha256.c
  librhash/sha256.h
  librhash/sha3.c
  librhash/sha3.h
  librhash/sha512.c
  librhash/sha512.h
  librhash/torrent.c
  librhash/ustd.h
  librhash/util.h
  )

add_library(librhash STATIC ${librhash_sources})


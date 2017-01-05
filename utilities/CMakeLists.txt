# rar library

add_compile_options(-DRARDL)
add_compile_options(-DUNRAR)
add_compile_options(-DSILENT)

set(LIBRAR rar)
    # <ClCompile Include="archive.cpp" />
    # <ClCompile Include="arcread.cpp" />
    # <ClCompile Include="blake2s.cpp" />
    # <ClCompile Include="cmddata.cpp" />
    # <ClCompile Include="consio.cpp" />
    # <ClCompile Include="crc.cpp" />
    # <ClCompile Include="crypt.cpp" />
    # <ClCompile Include="dll.cpp" />
    # <ClCompile Include="encname.cpp" />
    # <ClCompile Include="errhnd.cpp" />
    # <ClCompile Include="extinfo.cpp" />
    # <ClCompile Include="extract.cpp" />
    # <ClCompile Include="filcreat.cpp" />
    # <ClCompile Include="file.cpp" />
    # <ClCompile Include="filefn.cpp" />
    # <ClCompile Include="filestr.cpp" />
    # <ClCompile Include="find.cpp" />
    # <ClCompile Include="getbits.cpp" />
    # <ClCompile Include="global.cpp">
    #  <ClCompile Include="hash.cpp" />
    # <ClCompile Include="headers.cpp" />
    # <ClCompile Include="isnt.cpp" />
    # <ClCompile Include="match.cpp" />
    # <ClCompile Include="options.cpp" />
    # <ClCompile Include="pathfn.cpp" />
    # <ClCompile Include="qopen.cpp" />
    # <ClCompile Include="rar.cpp" />
    # <ClCompile Include="rarpch.cpp">
    # <ClCompile Include="rarvm.cpp" />
    # <ClCompile Include="rawread.cpp" />
    # <ClCompile Include="rdwrfn.cpp" />
    # <ClCompile Include="rijndael.cpp" />
    # <ClCompile Include="rs.cpp" />
    # <ClCompile Include="rs16.cpp" />
    # <ClCompile Include="scantree.cpp" />
    # <ClCompile Include="secpassword.cpp" />
    # <ClCompile Include="sha1.cpp" />
    # <ClCompile Include="sha256.cpp" />
    # <ClCompile Include="smallfn.cpp" />
    # <ClCompile Include="strfn.cpp" />
    # <ClCompile Include="strlist.cpp" />
    # <ClCompile Include="system.cpp" />
    # <ClCompile Include="threadpool.cpp" />
    # <ClCompile Include="timefn.cpp" />
    # <ClCompile Include="ui.cpp" />
    # <ClCompile Include="unicode.cpp" />
    # <ClCompile Include="unpack.cpp" />
    # <ClCompile Include="volume.cpp" />
    # <ResourceCompile Include="dll.rc" />
add_library(unrar SHARED 
    ${LIBRAR}/archive.cpp
    ${LIBRAR}/arcread.cpp
    ${LIBRAR}
)
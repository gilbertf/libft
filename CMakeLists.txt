cmake_minimum_required (VERSION 2.6)
project (libft)

# Require dot, treat the other components as optional
find_package(Doxygen
             REQUIRED dot)

set(DOXYGEN_DOTFILE_DIRS .)
set(DOXYGEN_EXTRACT_PRIVATE yes)
set(DOXYGEN_HIDE_UNDOC_MEMBERS yes)

doxygen_add_docs(
    doxygen
    ${PROJECT_SOURCE_DIR}
)


set(CMAKE_BUILD_TYPE Debug)

add_executable(genPlots ftcom.cpp genPlots.cpp)
add_executable(testCT ct.cpp ftcom.cpp testCT.cpp)
add_executable(testFTCom ct.cpp ftcom.cpp testFTCom.cpp)
add_executable(decode ftcom.cpp decode.cpp)

add_executable(testFTComC ftcom.cpp testFTComC.c)

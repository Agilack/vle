cmake_minimum_required(VERSION 2.6)
project(VLE CXX C)
enable_testing()

#
# Options
#

option(WITH_GVLE "use qt to build gvle [default: on]" ON)
option(WITH_TEST "use the libboost test unit library [default: on]" ON)
option(WITH_DOXYGEN "build the documentation with doxygen [default: off]" OFF)
option(WITH_MVLE "build mvle [default: off]" OFF)
option(WITH_CVLE "build cvle [default: off]" OFF)

#
# Usefull variables
#

set(VLE_MAJOR 1)
set(VLE_MINOR 3)
set(VLE_PATCH 0)
set(VLE_EXTRA "dev")
set(VLE_VERSION "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}")
set(VLE_VERSION_SHORT "${VLE_MAJOR}.${VLE_MINOR}")
set(VLE_NAME "vle")
set(VLE_LOCALE_NAME "${VLE_NAME}-${VLE_MAJOR}-${VLE_MINOR}")

if ("${VLE_EXTRA}" STREQUAL "")
  set(VLE_COMPLETE "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}")
else ("${VLE_EXTRA}" STREQUAL "")
  set(VLE_COMPLETE "${VLE_MAJOR}.${VLE_MINOR}.${VLE_PATCH}-${VLE_EXTRA}")
endif ("${VLE_EXTRA}" STREQUAL "")

set(VLE_NAME_COMPLETE "${VLE_NAME}-${VLE_COMPLETE}")

#
# Installation variables
#

set(VLE_LIBRARY_DIRS "lib/vle-${VLE_VERSION_SHORT}")
set(VLE_INCLUDE_DIRS "include/vle-${VLE_VERSION_SHORT}/vle")
set(VLE_INCLUDE_DIR "include/vle-${VLE_VERSION_SHORT}")
set(VLE_MANPAGE_PATH "share/man/man1")
set(VLE_SHARE_DIRS "share/vle-${VLE_VERSION_SHORT}")


#
# CMake modules
#
include(CheckIncludeFileCXX)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CMakeDetermineCCompiler)

#
# append debug mode when compiler is GNU.
#

if (CMAKE_BUILD_TYPE STREQUAL "")
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif ()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

  option(WITH_COVERAGE "use coverage build [default: off]" OFF)
  if (WITH_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
  endif ()

  if ("${CMAKE_BUILD_TYPE}" EQUAL "Debug" OR
      "${CMAKE_BUILD_TYPE}" EQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
  endif ()
endif ()

# FIXME use old-style link directories for now
if (COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 OLD)
  cmake_policy(SET CMP0020 NEW)#for qt link
endif (COMMAND CMAKE_POLICY)

set(QMAKE_QUERY_LIBPATH qmake -query QT_INSTALL_LIBS)
execute_process(COMMAND ${QMAKE_QUERY_LIBPATH} OUTPUT_VARIABLE
  CMAKE_PREFIX_PATH)
set(QMAKE_QUERY_HOSTPATH qmake -query QT_HOST_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${QMAKE_QUERY_HOSTPATH} OUTPUT_VARIABLE
  QT_INSTALL_PATH)

if (WITH_GVLE)
  find_package(Qt5 COMPONENTS Widgets Xml Gui Help PrintSupport)
  if (NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 is required")
  endif (NOT Qt5_FOUND)
  if (NOT Qt5Widgets_FOUND)
    message(FATAL_ERROR "Qt5Widgets is required")
  endif (NOT Qt5Widgets_FOUND)
  if (NOT Qt5Xml_FOUND)
    message(FATAL_ERROR "Qt5Xml is required")
  endif (NOT Qt5Xml_FOUND)
  if (NOT Qt5Gui_FOUND)
    message(FATAL_ERROR "Qt5Gui is required")
  endif (NOT Qt5Gui_FOUND)
  if (NOT Qt5Help_FOUND)
    message(FATAL_ERROR "Qt5Help is required")
  endif (NOT Qt5Help_FOUND)
  if (NOT Qt5PrintSupport_FOUND)
    message(FATAL_ERROR "Qt5PrintSupport is required")
  endif (NOT Qt5PrintSupport_FOUND)
  set(VLE_HAVE_QT 1 CACHE INTERNAL "" FORCE)
  set(VLE_HAVE_GVLE 1 CACHE INTERNAL "" FORCE)
endif ()


if (WIN32)
  SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/share/cmake ${CMAKE_MODULE_PATH})
  set(VLEDEPS_DEBUG 1)
  find_package(VLEDEPS REQUIRED)
else ()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(VLEDEPS libarchive libxml-2.0)
  if (NOT VLEDEPS_FOUND)
    message(FATAL_ERROR "Failed to find minimal dependencies: libarchive "
      " and libxml-2.0")
  endif ()
endif ()

#
# CPack configuration
#
include(CMakeCPack.cmake)

# Propagates -std=C++11 CC option from pkg-config but VLEDEPS_CFLAGS_OTHER
# #seems to be a CMake'list. So we replace ';' with ' '.
string(REPLACE ";" " " VLEDEPS_CFLAGS_OTHER_STRING "${VLEDEPS_CFLAGS_OTHER}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VLEDEPS_CFLAGS_OTHER_STRING}")

#
# Test the libboost header and libboost-text library.
#

if (WIN32)
  set(Boost_THREADAPI "win32")
endif (WIN32)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)

set(Boost_USE_MULTITHREADED ON)
set(Boost_DETAILED_FAILURE_MSG ON)
set(Boost_DEBUG OFF)



if (VLE_BOOST_INCLUDE_PATH AND VLE_BOOST_LIBRARIES_PATH)
 set (BOOST_LIBRARYDIR ${VLE_BOOST_LIBRARIES_PATH})
 set (BOOST_INCLUDEDIR ${VLE_BOOST_INCLUDE_PATH})
endif ()

set(Boost_DEBUG 0)
if (WITH_MVLE OR WITH_CVLE)
  find_package(Boost COMPONENTS unit_test_framework thread filesystem system
    chrono date_time regex program_options serialization mpi)
else ()
  find_package(Boost COMPONENTS unit_test_framework thread filesystem system
    chrono date_time regex program_options)
endif()

if (NOT Boost_FILESYSTEM_FOUND)
  message(FATAL_ERROR "The boost filesystem library is required")
endif (NOT Boost_FILESYSTEM_FOUND)
if (NOT Boost_REGEX_FOUND)
  message(FATAL_ERROR "The boost regex library is required")
endif (NOT Boost_REGEX_FOUND)
if (NOT Boost_DATE_TIME_FOUND)
  message(FATAL_ERROR "The boost date_time library is required")
endif (NOT Boost_DATE_TIME_FOUND)
if (NOT Boost_THREAD_FOUND)
  message(FATAL_ERROR "The boost thread library is required")
endif (NOT Boost_THREAD_FOUND)
if (NOT Boost_PROGRAM_OPTIONS_FOUND)
  message(FATAL_ERROR "The boost program options library is required")
endif (NOT Boost_PROGRAM_OPTIONS_FOUND)

if (NOT Boost_SERIALIZATION_FOUND)
  message(STATUS "The boost serialization was not found, MPI disabled")
endif ()
if (NOT Boost_MPI_FOUND)
  message(STATUS "The boost mpi was not found, MPI disabled")
endif ()

if (Boost_MINOR_VERSION AND ${Boost_MINOR_VERSION} LESS "41")
  set(VLE_HAVE_BOOST_SPIRIT2 0 CACHE INTERNAL "" FORCE)
else (Boost_MINOR_VERSION AND ${Boost_MINOR_VERSION} LESS "41")
  set(VLE_HAVE_BOOST_SPIRIT2 1 CACHE INTERNAL "" FORCE)
endif (Boost_MINOR_VERSION AND ${Boost_MINOR_VERSION} LESS "41")

if (WITH_TEST)
  if (Boost_UNIT_TEST_FRAMEWORK_FOUND)
    set(VLE_HAVE_UNITTESTFRAMEWORK 1 CACHE INTERNAL "" FORCE)
  endif (Boost_UNIT_TEST_FRAMEWORK_FOUND)
endif (WITH_TEST)

#
# Check for an MPI implementation. building mvle and/or cvle
#


if (WITH_MVLE OR WITH_CVLE)
  find_package(MPI REQUIRED)
  message(STATUS "mpi include ${MPI_INCLUDE_PATH}")
  message(STATUS "mpi lib ${MPI_LIBRARY}")
  message(STATUS "mpi extra lib ${MPI_EXTRA_LIBRARY}")
  message(STATUS "mpi link ${MPI_LINK_FLAGS}")
  if (WITH_MVLE)
    set(VLE_HAVE_MVLE 1 CACHE INTERNAL "" FORCE)
  else ()
    set(VLE_HAVE_MVLE 0 CACHE INTERNAL "" FORCE)
  endif ()
  if (WITH_CVLE)
    if (Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)
      set(VLE_HAVE_CVLE 1 CACHE INTERNAL "" FORCE)
    else ()
      message(WARNING "boost mpi or boost serialization not found to build cvle")
      set(VLE_HAVE_CVLE 0 CACHE INTERNAL "" FORCE)
    endif ()
  else ()
    set(VLE_HAVE_CVLE 0 CACHE INTERNAL "" FORCE)
  endif ()
else ()
  set(VLE_HAVE_MVLE 0 CACHE INTERNAL "" FORCE)
  set(VLE_HAVE_CVLE 0 CACHE INTERNAL "" FORCE)
endif ()

#
# Generate the config.h
#
check_include_file_cxx(cxxabi.h VLE_HAVE_GCC_ABI_DEMANGLE)
check_include_file_cxx(execinfo.h VLE_HAVE_EXECINFO)
check_include_file_cxx(signal.h VLE_HAVE_SIGNAL)
include_directories(${VLE_BINARY_DIR})

#
# generate the vle.pc pkg-config file.
# Build some variable to build a correct Win32 pkg config file with:
# prefix=/target to detect the DIRNAME
# cflag=-Iboost_1_34_1 for the directory
#

set(VLE_PKGCONFIG_CFLAGS_OTHER "${VLEDEPS_CFLAGS_OTHER_STRING}")
if (UNIX)
  set(VLE_PKGCONFIG_PREFIXPATH "${CMAKE_INSTALL_PREFIX}")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
  set(VLE_PKGCONFIG_LIBDIR "lib")
else ()
  set(VLE_PKGCONFIG_PREFIXPATH "/target")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "\${includedir}")
  set(VLE_PKGCONFIG_LIBDIR "bin")
endif ()

#
# Install the README
#
if (UNIX)
  install(FILES "README.md" DESTINATION "${VLE_SHARE_DIRS}")
  install(FILES "COPYING" DESTINATION "${VLE_SHARE_DIRS}")
else ()
  install(FILES "README.md" DESTINATION "${VLE_SHARE_DIRS}" RENAME "Readme.txt")
  install(FILES "COPYING" DESTINATION "${VLE_SHARE_DIRS}" RENAME "CopyRight.txt")
endif ()

#
# Add sources for a target
# add_sources(<target> <source1> [<source2> ...])
#
function(add_sources target)
  get_property(prop_defined GLOBAL PROPERTY ${target}_SRCS DEFINED)
  if(NOT prop_defined)
    define_property(GLOBAL PROPERTY ${target}_SRCS
      BRIEF_DOCS "Sources for the ${target} target"
      FULL_DOCS "List of source files for the ${target} target")
  endif()

  set(SRCS)
  foreach(src ${ARGN})
    if(NOT IS_ABSOLUTE "${src}")
      get_filename_component(src "${src}" ABSOLUTE)
    endif()
    list(APPEND SRCS "${src}")
  endforeach()
  set_property(GLOBAL APPEND PROPERTY "${target}_SRCS" "${SRCS}")
endfunction()

#
# Browse the share subdirectory
#
add_subdirectory(share)

#
# Gettext utilities
#
find_package(Gettext)
if (GETTEXT_FOUND)
  set(VLE_HAVE_NLS 1 CACHE INTERNAL "" FORCE)
  add_subdirectory(i18n)
else (GETTEXT_FOUND)
  set(VLE_HAVE_NLS 0 CACHE INTERNAL "" FORCE)
endif (GETTEXT_FOUND)

#
# Generating documentation with doxygen
#

if (WITH_DOXYGEN)
  find_package(Doxygen)
  if (DOXYGEN)
    add_subdirectory(doxygen)
  else (DOXYGEN)
    message(STATUS "WARNING: Doxygen not found - No reference manual create")
  endif (DOXYGEN)
endif (WITH_DOXYGEN)

#
# Browse the src subdirectory
#
ADD_SUBDIRECTORY(src)


#
# Status
#
message(STATUS "- - - -")
message(STATUS "${VLE_NAME_COMPLETE} configured successfully")
message(STATUS "Using ${CMAKE_INSTALL_PREFIX} for installation")
message(STATUS "Build type ${CMAKE_BUILD_TYPE}")
message(STATUS "Build with GCC ABI Demangle...: ${VLE_HAVE_GCC_ABI_DEMANGLE}")
message(STATUS "Build with execinfo.h.........: ${VLE_HAVE_EXECINFO}")
message(STATUS "Build unit test...............: ${VLE_HAVE_UNITTESTFRAMEWORK}")
message(STATUS "Build with gvle...............: ${VLE_HAVE_GVLE}")
message(STATUS "Build with mvle...............: ${VLE_HAVE_MVLE}")
message(STATUS "Build with cvle...............: ${VLE_HAVE_CVLE}")

# vim:tw=0:ts=8:tw=0:sw=2:sts=2

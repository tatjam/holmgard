# Install script for directory: /home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/lua.h"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/lauxlib.h"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/lualib.h"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/lua.hpp"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/luajit.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblua.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblua.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblua.so"
         RPATH "$ORIGIN/../lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/liblua.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblua.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblua.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblua.so"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "$ORIGIN/../lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblua.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/lua" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/lua")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/lua"
         RPATH "$ORIGIN/../lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/lua")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/lua" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/lua")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/lua"
         OLD_RPATH "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake:"
         NEW_RPATH "$ORIGIN/../lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/lua")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/lua/5.1/jit" TYPE FILE FILES
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/bc.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/v.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dump.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_x86.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_x64.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_arm.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_ppc.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_mips.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_mipsel.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/bcsave.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/vmdef.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/p.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/zone.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_arm64.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_arm64be.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_mips64.lua"
    "/home/tatjam/data/code/osp/new-ospgl/dep/LuaJIT-cmake/src/jit/dis_mips64el.lua"
    )
endif()


#
# $Id $
#
# Author(s):  Min Yang Jung, Anton Deguet
# Created on: 2009
#
# (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# Locate Ice home

# This module defines the following variables:
# ICE_FOUND : YES if Ice is found, NO otherwise
# ICE_HOME  : path where to find include, lib, bin, etc.
# ICE_INCLUDE_DIR
# ICE_LIBRARY_DIR
# ICE_SLICE_DIR
# ICE_SLICE2CPP
#

#
# Ice for C++
#

# Assumption: we look for Ice/Ice.h and assume that the rest is there.
# i.e. slice2cpp, libIce.so, etc.
# to be more robust we can look for all of those things individually.

# start with 'not found'
set (ICE_FOUND "NO" CACHE BOOL "Do we have Ice?" FORCE)

find_path (ICE_ICE_H_INCLUDE_DIR
           NAMES Ice/Ice.h
           PATHS
             # installation selected by user
             ${ICE_HOME}/include
             $ENV{ICE_HOME}/include
             # debian package installs Ice here
             /usr/include
             # MacPort
             /opt/local/include
             # Test standard installation points: generic symlinks first, then standard dirs, newer first
             /opt/Ice/include
             /opt/Ice-4/include
             /opt/Ice-4.0/include
             /opt/Ice-3/include
             /opt/Ice-3.5/include
             /opt/Ice-3.4/include
             /opt/Ice-3.3/include
             # some people may manually choose to install Ice here
             /usr/local/include
             # Windows
             # 3.4.1
               "C:/Program Files/ZeroC/Ice-3.4.1/include"
               C:/Ice-3.4.1-VC90/include
               C:/Ice-3.4.1-VC80/include
               C:/Ice-3.4.1/include
             # 3.4.0
               "C:/Program Files/ZeroC/Ice-3.4.0/include"
               C:/Ice-3.4.0-VC90/include
               C:/Ice-3.4.0-VC80/include
               C:/Ice-3.4.0/include
             # 3.3.1
               C:/Ice-3.3.1-VC90/include
               C:/Ice-3.3.1-VC80/include
               C:/Ice-3.3.1/include
             # 3.3.0
               C:/Ice-3.3.0-VC90/include
               C:/Ice-3.3.0-VC80/include
               C:/Ice-3.3.0/include
            )

# NOTE: if ICE_HOME_INCLUDE_ICE is set to *-NOTFOUND it will evaluate to FALSE
if (ICE_ICE_H_INCLUDE_DIR)

  get_filename_component (ICE_HOME_STRING ${ICE_ICE_H_INCLUDE_DIR} PATH)
  set (ICE_HOME ${ICE_HOME_STRING} CACHE PATH "Ice home directory")

  message (STATUS "Setting ICE_HOME to ${ICE_HOME}")

  # include and lib dirs are easy
  if (WIN32)  
    set (ICE_INCLUDE_DIR
        ${ICE_ICE_H_INCLUDE_DIR}
        ${ICE_HOME}/slice
        ${ICE_HOME}/Ice/slice
        ${ICE_HOME}/Ice-3.4.1/slice
        ${ICE_HOME}/Ice-3.4.0/slice
        ${ICE_HOME}/Ice-3.3.1/slice
        ${ICE_HOME}/Ice-3.3.0/slice )
  else (WIN32)
    set (ICE_INCLUDE_DIR
        ${ICE_ICE_H_INCLUDE_DIR}
        ${ICE_HOME}/share/slice
        ${ICE_HOME}/share/ice/slice
        ${ICE_HOME}/share/Ice/slice
        # For Ice installation via Ubuntu Synaptic package manager
        ${ICE_HOME}/share/Ice-3.4.1/slice
        ${ICE_HOME}/share/Ice-3.4.0/slice
        ${ICE_HOME}/share/Ice-3.3.1/slice
        ${ICE_HOME}/share/Ice-3.3.0/slice )
  endif (WIN32)
  set (ICE_LIBRARY_DIR ${ICE_HOME}/lib)

  # debian package splits off slice files into a different place
  if (ICE_HOME MATCHES /usr)
    set (ICE_SLICE_DIR /usr/share/slice)
  else (ICE_HOME MATCHES /usr)
    set (ICE_SLICE_DIR ${ICE_HOME}/slice)
  endif (ICE_HOME MATCHES /usr)

  # some libs only care about IceUtil, we tell them to find IceUtil in the same place as Ice.
  set (ICEUTIL_HOME ${ICE_HOME})
  message (STATUS "Setting ICEUTIL_HOME to ${ICEUTIL_HOME} and ICE_LIBRARY_DIR to ${ICE_LIBRARY_DIR}")

  # try to figure if the ice library is libIce or libZeroCIce on Mac OS with MacPort
  if (APPLE)
    find_library (ICE_LIBRARY_NAME_ZEROC_ICE NAMES ZeroCIce PATHS ${ICE_LIBRARY_DIR} NO_DEFAULT_PATH)
      if (ICE_LIBRARY_NAME_ZEROC_ICE)
        set (ICE_LIBRARY_NAME ZeroCIce)
      else (ICE_LIBRARY_NAME_ZEROC_ICE)
        set (ICE_LIBRARY_NAME Ice)
      endif (ICE_LIBRARY_NAME_ZEROC_ICE)
  else (APPLE)
    set (ICE_LIBRARY_NAME Ice)
  endif (APPLE)
  message( STATUS "Ice library name is ${ICE_LIBRARY_NAME}")

  # find slice2cpp
  find_program (ICE_SLICE2CPP
                NAME slice2cpp
                PATHS "${ICE_HOME}/bin"
                NO_DEFAULT_PATH)

  # set ICE_FOUND if enough is found
  if (ICE_HOME AND ICE_LIBRARY_NAME AND ICE_SLICE2CPP)
    set (ICE_FOUND "YES" CACHE BOOL "Do we have Ice?" FORCE)
  endif (ICE_HOME AND ICE_LIBRARY_NAME AND ICE_SLICE2CPP)

  # quiet things down a bit
  if (ICE_FOUND)
    mark_as_advanced (ICE_FOUND ICE_HOME
                      ICE_INCLUDE_DIR ICE_ICE_H_INCLUDE_DIR ICE_SLICE_DIR
                      ICE_LIBRARY_NAME ICE_LIBRARY_NAME_ZEROC_ICE ICE_SLICE2CPP)
  endif (ICE_FOUND)

endif (ICE_ICE_H_INCLUDE_DIR)
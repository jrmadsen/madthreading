//  VERSION_STRING 0_1
//  madthreading version.hh configuration header file  ---------------------//

//  (C) Copyright Jonathan Madsen 2015.

#ifndef madthreading_VERSION_HH_
#define madthreading_VERSION_HH_

//
//  Caution, this is the only madthreading header that is guarenteed
//  to change with every release, including this header
//  will cause a recompile every time a new madthreading version is
//  released.
//
//  madthreading_VERSION % 100 is the patch level
//  madthreading_VERSION / 100 % 1000 is the minor version
//  madthreading_VERSION / 100000 is the major version

#define madthreading_VERSION 000100
#define madthreading_MAJOR_VERSION 0
#define madthreading_MINOR_VERSION 1
#define madthreading_PATCH_VERSION 0

//
//  madthreading_LIB_VERSION must be defined to be the same as
//  madthreading_VERSION but as a *string* in the form "x_y[_z]" where x is
//  the major version number, y is the minor version number, and z is the patch
//  level if not 0.

#define madthreading_LIB_VERSION "0_1"

#endif

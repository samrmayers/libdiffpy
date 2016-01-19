/*****************************************************************************
*
* libdiffpy         Complex Modeling Initiative
*                   (c) 2014 Brookhaven Science Associates,
*                   Brookhaven National Laboratory.
*                   All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE.txt for license information.
*
******************************************************************************
*
* Macro definitions for
*   DIFFPY_VERSION,
*   DIFFPY_VERSION_MAJOR,
*   DIFFPY_VERSION_MINOR,
*   DIFFPY_VERSION_MICRO,
*   DIFFPY_VERSION_STR,
*   DIFFPY_VERSION_DATE
*   DIFFPY_GIT_SHA
*
* Declaration of libdiffpy_version_info struct for version data resolved
* at runtime.
*
*****************************************************************************/

#ifndef LIBDIFFPY_VERSION_HPP_INCLUDED
#define LIBDIFFPY_VERSION_HPP_INCLUDED

#define DIFFPY_VERSION_MAJOR ${DIFFPY_VERSION_MAJOR}
#define DIFFPY_VERSION_MINOR ${DIFFPY_VERSION_MINOR}
#define DIFFPY_VERSION_MICRO ${DIFFPY_VERSION_MICRO}

// round(DIFFPY_VERSION) / 1000000 is the major version
// round(DIFFPY_VERSION) / 1000 % 1000 is the minor version
// round(DIFFPY_VERSION) % 1000 is the micro version
// round(DIFFPY_VERSION * 10000) % 1000
//      is number of git commits since the last tag
//
// alpha and beta releases have smaller DIFFPY_VERSION than
// a completed release.  Numerical comparison of DIFFPY_VERSION
// values from two pre-releases may be inaccurate.

#define DIFFPY_VERSION ${DIFFPY_VERSION}

// DIFFPY_VERSION_STR is a string form of DIFFPY_VERSION

#define DIFFPY_VERSION_STR "${DIFFPY_VERSION_STR}"

// DIFFPY_VERSION_DATE is the commit date of DIFFPY_VERSION

#define DIFFPY_VERSION_DATE "${DIFFPY_VERSION_DATE}"

// DIFFPY_GIT_SHA is a full git commit hash for the current version

#define DIFFPY_GIT_SHA "${DIFFPY_GIT_SHA}"

// libdiffpy_version_info will hold runtime version data, which may be
// different from client compile-time values.

struct libdiffpy_version_info {

    static const double version;
    static const char* version_str;
    static const int major;
    static const int minor;
    static const int micro;
    static const char* date;
    static const char* git_sha;

};

#endif  // LIBDIFFPY_VERSION_HPP_INCLUDED

// vim:ft=cpp:

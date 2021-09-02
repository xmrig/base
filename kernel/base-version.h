/* XMRig
 * Copyright (c) 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2021 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XMRIG_BASE_VERSION_H
#define XMRIG_BASE_VERSION_H


#define BASE_VER_MAJOR  0
#define BASE_VER_MINOR  1
#define BASE_VER_PATCH  0

#ifndef APP_DOMAIN
#   define APP_DOMAIN    "xmrig.com"
#endif

#ifndef APP_COPYRIGHT
#   define APP_COPYRIGHT "Copyright (C) 2016-2021 xmrig.com"
#endif

#ifdef _MSC_VER
#   if (_MSC_VER >= 1920)
#       define MSVC_VERSION 2019
#   elif (_MSC_VER >= 1910 && _MSC_VER < 1920)
#       define MSVC_VERSION 2017
#   elif _MSC_VER == 1900
#       define MSVC_VERSION 2015
#   elif _MSC_VER == 1800
#       define MSVC_VERSION 2013
#   elif _MSC_VER == 1700
#       define MSVC_VERSION 2012
#   elif _MSC_VER == 1600
#       define MSVC_VERSION 2010
#   else
#       define MSVC_VERSION 0
#   endif
#endif

#define XMRIG_STRINGIFY(x) #x
#define XMRIG_TOSTRING(x) XMRIG_STRINGIFY(x)

#ifdef GIT_COMMIT_HASH
#   define XMRIG_GIT_COMMIT_HASH XMRIG_TOSTRING(GIT_COMMIT_HASH)
#else
#   define XMRIG_GIT_COMMIT_HASH "0000000"
#endif

#ifdef GIT_BRANCH
#   define XMRIG_GIT_BRANCH XMRIG_TOSTRING(GIT_BRANCH)
#   define APP_VERSION XMRIG_TOSTRING(APP_VER_MAJOR.APP_VER_MINOR.APP_VER_PATCH) "-" XMRIG_GIT_BRANCH
#else
#   define APP_VERSION XMRIG_TOSTRING(APP_VER_MAJOR.APP_VER_MINOR.APP_VER_PATCH)
#endif

#define BASE_VERSION XMRIG_TOSTRING(BASE_VER_MAJOR.BASE_VER_MINOR.BASE_VER_PATCH)


#endif // XMRIG_BASE_VERSION_H
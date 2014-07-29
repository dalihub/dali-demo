# Copyright (c) 2014 Samsung Electronics Co., Ltd.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


m4_define([dali_version],[0.1.0])
AC_INIT([dali], [dali_version])
AM_INIT_AUTOMAKE([-Wall foreign])

AC_PROG_CXX
AC_PROG_LIBTOOL

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

DALI_VERSION=dali_version
AC_SUBST(DALI_VERSION)

PKG_CHECK_MODULES(DALI, dali)
PKG_CHECK_MODULES(DALI_TOOLKIT, dali-toolkit)
PKG_CHECK_MODULES(AUL, aul,
                  [AC_DEFINE([HAS_AUL], [1])],
                  [1])

DALIDEMO_CFLAGS=-DPLATFORM_SLP

AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug],
                              [Turns on debugging])],
              [enable_debug=$enableval],
              [enable_debug=no])

if test "x$enable_debug" = "xyes"; then
  DALIDEMO_CFLAGS="$DALIDEMO_CFLAGS -DDEBUG_ENABLED"
fi

if test x$DALI_APP_DIR != x; then
  appdatadir=$DALI_APP_DIR
  exedir=${appdatadir}/bin/
else
  appdatadir=${prefix}/share/com.samsung.dali-demo/
  exedir=${prefix}/bin/
fi

AC_SUBST(datadir)
AC_SUBST(appdatadir)
AC_SUBST(exedir)
AC_SUBST(DALIDEMO_CFLAGS)

dnl **************************************************************************
dnl ** Set Debian install Prefix                                            **
dnl **************************************************************************
debian_prefix=${prefix#*/}
AC_SUBST(debian_prefix)

AC_CONFIG_FILES([
  Makefile
  demo/Makefile
  examples/Makefile
])


AC_OUTPUT

echo "
Configuration
-------------
  Prefix:                           $prefix
  Debug Build:                      $enable_debug
  Application Data Dir:             $appdatadir
  Application Exe Dir:              $exedir
  DALIDEMO_CFLAGS:                  ${DALIDEMO_CFLAGS}
"

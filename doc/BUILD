Building RPS
============

pdinda@cs.northwestern.edu


Needed Software
---------------

The minimum required tools for RPS on a Unix or Unix-like system are
are a C++ compiler and GNU Make.  On windows, Visual Studio 6.0 is the
bare minimum, although it is easier to use cygwin.  

For additional functionality, other software is needed:

  - Fortran 77 compiler :  ARFIMA models (FracDiff)
  - CMU Remos : flow bandwidth measurement
  - PVM 3.3.3 or better : parallel offline time series evaluation codes
  - Java : simple java gui 
  - Perl 5 : assorted scripts
  - tcsh and sh : assorted scripts

Some functionality is currently unavailable on the Native Windows
build:

  - Communication using Unix domain sockets does not work
  - Predserver is not built, so stream-oriented prediction is not supported
  - Parts of various packages are not built


Build Procedure
---------------

To build on a Unix or Unix-like (cygwin) system:

0. export RPS_DIR=your rps directory, RPS_OS=your os, RPS_ARCH=your arch
1. edit build/setup_rps_env as appropriate
2. source setup_rps_env
3. cp build/Makefile.conf.most_appropriate Makefile.conf
4. edit Makefile.conf as needed (See doc/CONFIGURATION for more information)
   a. you must set RPS_DIR appropriately
   b. The first block of "HAVE_FOO" options determine which packages are built.
      If you try to build a package for which you don't have the software, the
      build process will fail.
   c. Each package then has a block for its options.  In many cases, it is 
      possible to build a package with reduced functionality even if you don't have
      the software.  If you indicate you have the software but you really don't,
      the build process will fail.
5. make rebuild_all


To build on native windows using Visual C++  (Not all components are built)

0. set RPS_DIR=your rps directory, RPS_OS=WIN32, RPS_ARCH=I386
1. make sure MSDEV.EXE is on your path and INCLUDE and LIB are set reasonably.
2. build\build_windows.bat

The build process will drop executables into $RPS_DIR/bin/ARCH/OS,
libraries into $RPS_DIR/lib/ARCH/OS, and include files into
$RPS_DIR/include, where ARCH and OS are the obvious things.

Here are the platforms on which this version of RPS has been
successfully built and tested along with comments on each.


Installation Support for Unix Systems
-------------------------------------

The result of the build process is that all binaries, includes, and
libraries land in the appropriate places in $RPS_DIR, for example,
binaries land in $RPS_DIR/bin/$RPS_ARCH/$RPS_OS.  You can also do a
trimmed down installation of the software in the following way, after
you have built everything:

make RPS_INSTALL_DIR=/where/you/want/RPS install

This will create a traditional directory tree:

$RPS_INSTALL_DIR
  bin
  doc
  lib
  include

In addition to binaries, the appropriate scripts will be copied 
into the bin subdirectory


Red Hat Linux 6.2 on Intel
--------------------------

targets in .../I386/LINUX

[pdinda@skysaw RPS-development]$ uname -a
Linux skysaw 2.2.14-5.0smp #1 SMP Tue Mar 7 21:01:40 EST 2000 i686 unknown
[pdinda@skysaw RPS-development]$ make -v
GNU Make version 3.78.1, by Richard Stallman and Roland McGrath.
Built for i386-redhat-linux-gnu
...
[pdinda@skysaw RPS-development]$ g++ -v
Reading specs from /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.3/specs
gcc version 2.95.3 20010315 (release)
...
[pdinda@skysaw RPS-development]$ g77 -v
g77 version 2.95.3 20010315 (release) (from FSF-g77 version 0.5.25 20010315 (relea
se))
...
[pdinda@skysaw RPS-development]$ java -version
java version "1.3.0"
Java(TM) 2 Runtime Environment, Standard Edition (build 1.3.0)
Java HotSpot(TM) Client VM (build 1.3.0, mixed mode)

RPS has also been built on out-of-the-box Red Hat 7.0.


Cygwin 1.1.8 on Windows 2000 on Intel
-------------------------------------

targets in .../I386/CYGWIN

bash$ uname -a
CYGWIN_NT-5.0 ITCHY 1.1.8(0.34/3/2) 2001-01-31 10:08 i686 unknown
bash$ make -v
GNU Make version 3.79.1, by Richard Stallman and Roland McGrath.
Built for i686-pc-cygwin
...
bash$ g++ -v
Reading specs from /usr/lib/gcc-lib/i686-pc-cygwin/2.95.3-1/specs
gcc driver version 2.95.3-1 (cygwin special) executing gcc version gcc-2.95.3-1
bash$ g77 -v
g77 version 2.95.3-1 (cygwin special) (from FSF-g77 version 0.5.25 20010315 (rel
ease))
...

Windows 2000 Native with Visual Studio 6.0 on Intel
---------------------------------------------------

c:\documents and settings\pdinda\my documents\Codes\RPS-development>cl
Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 12.00.8168 for 80x86
Copyright (C) Microsoft Corp 1984-1998. All rights reserved.

Solaris 2.8 on Sparc
--------------------

bash-2.03$ uname -a
SunOS sunlab-22 5.8 Generic sun4u sparc SUNW,Ultra-5_10
bash-2.03$ make -v
GNU Make version 3.79.1, by Richard Stallman and Roland McGrath.
Built for sparc-sun-solaris2.8
...
bash-2.03$ g++ -v
Reading specs from /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/2.95.3/specs
gcc version 2.95.3 20010315 (release)
bash-2.03$ g77 -v
g77 version 2.95.3 20010315 (release) (from FSF-g77 version 0.5.25 20010315 (relea
se))
bash-2.03$ java -version
java version "1.2.1"
Solaris VM (build Solaris_JDK_1.2.1_04c, native threads, sunwjit)

Digital Unix 4.0D on Alpha
--------------------------

(pyramid.cmcl) [/usr5/pdinda/RPS-development] uname -a
OSF1 pyramid.cmcl.cs.cmu.edu V4.0 878 alpha
(pyramid.cmcl) [/usr5/pdinda/RPS-development] make -v
GNU Make version 3.75, by Richard Stallman and Roland McGrath.
...
(pyramid.cmcl) [/usr5/pdinda/RPS-development] g++ -v
Reading specs from /usr/local/libexec/gcc-2.95.2/lib/gcc-lib/alpha-dec-osf4.0d/2.95.2/specs
gcc version 2.95.2 19991024 (release)
(pyramid.cmcl) [/usr5/pdinda/RPS-development] g77 -v
g77 version 2.95.2 19991024 (release) (from FSF-g77 version 0.5.25 19991024 (relea
se))
...
(pyramid.cmcl) [/usr5/pdinda/RPS-development] java -version
java version "1.2.2-6"
Classic VM (build J2SDK.v.1.2.2:04/26/2000-09:58, native threads,jit_122)

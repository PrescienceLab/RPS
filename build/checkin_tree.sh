#! /bin/bash

cvs add $1
cd $1
cvs add src include bin obj lib
cd bin
cvs add ALPHA I386 SUN
cd ALPHA
cvs add DUX
cd ..
cd I386
cvs add LINUX FREEBSD WIN32 CYGWIN
cd ..
cd SUN
cvs add SOLARIS
cd ..
cd ..
cd lib
cvs add ALPHA I386 SUN
cd ALPHA
cvs add DUX
cd ..
cd I386
cvs add LINUX FREEBSD WIN32 CYGWIN
cd ..
cd SUN
cvs add SOLARIS
cd ..
cd ..
cd obj
cvs add ALPHA I386 SUN
cd ALPHA
cvs add DUX
cd ..
cd I386
cvs add LINUX FREEBSD WIN32 CYGWIN
cd ..
cd SUN
cvs add SOLARIS

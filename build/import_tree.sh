#! /bin/bash

cvs add $1
cd $1
cvs add src include bin obj lib
cd bin
cvs add ALPHA I386 SUN
cd ALPHA; cvs add DUX 
cd ../I386; cvs add FREEBSD LINUX WIN32
cd ../SUN; cvs add SOLARIS
cd ..
cd lib
cvs add ALPHA I386 SUN
cd ALPHA; cvs add DUX
cd ../I386; cvs add FREEBSD LINUX WIN32
cd ../SUN; cvs add SOLARIS
cd ..
cd obj
cvs add ALPHA I386 SUN
cd ALPHA; cvs add DUX
cd ../I386; cvs add FREEBSD LINUX WIN32
cd ../SUN; cvs add SOLARIS
cd ..
cd ..

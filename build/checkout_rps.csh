#!/bin/csh

setenv CVSROOT /home/RPS/CVS_RPS
cvs co RPS
cd RPS
mkdir bin
mkdir bin/ALPHA
mkdir bin/ALPHA/DUX
mkdir bin/I386
mkdir bin/I386/LINUX
mkdir bin/I386/FREEBSD
mkdir bin/SUN
mkdir bin/SUN/SOLARIS
mkdir lib
mkdir lib/ALPHA
mkdir lib/ALPHA/DUX
mkdir lib/I386
mkdir lib/I386/LINUX
mkdir lib/I386/FREEBSD
mkdir lib/SUN
mkdir lib/SUN/SOLARIS
mkdir include
cd include
foreach i (`find .. -name "*.h" -print`)
  ln -s $i .
end
cd ..
cp build/Makefile .
cp build/work_linux_debug.conf Makefile.conf
echo "Edit Makefile.conf and then go gmake rebuild_all"

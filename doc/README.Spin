RPS's Spin Package
==================

rps-help@cs.northwestern.edu

The Spin package provides a server and client for running arbitrary
sized busy-loop tasks remotely.  

INCLUDE FILES
-------------

Spin.h
 - general include

LIBRARIES
---------

libspin.a


BINARIES
--------

spinserver
 - The spin server (runs tasks)

spinclient
 - The spin client (submits tasks)

calibrate
 - calibrate the apply loop and produce a calibration file
 - spin server will do this itself too

test_apply
 - test the apply loop

testspin
 - test the spin core code 

#! /bin/sh

CVSROOT=/killdeer/usr3/pdinda/CVS/REPOSITORY
export CVSROOT


cvs co RPS/Scripts

cd RPS

cvs co GetLoadAvg
cvs co GetFlowBW
cvs co TS
cvs co LoadMon




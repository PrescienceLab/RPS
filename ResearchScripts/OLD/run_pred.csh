#!/bin/csh

(hostloadpred server:tcp:5000 target:stdio:stdout 1000000 300 60 30 300 0.01 5 AR 16 | predbuffer 10 source:stdio:stdin server:tcp:5001) >& /dev/null &


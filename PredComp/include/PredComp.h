#ifndef _predcomp_env
#define _predcomp_env

#include "Sensors.h"
#include "TimeSeries.h"
#include "Mirror.h"
#include "RPSInterface.h"

#if defined(WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#endif



#endif

#ifndef _predcomp_env
#define _predcomp_env

#if defined(WIN32) && !defined(__CYGWIN__)
#pragma warning( disable: 4786)  //stop msvc from spewing crap about long template instaniations
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
extern "C" {
#include <windows.h>
}
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#endif

#include "Sensors.h"
#include "TimeSeries.h"
#include "Mirror.h"
#include "RPSInterface.h"
#include "banner.h"




#endif

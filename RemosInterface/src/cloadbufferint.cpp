#include <math.h>
#include <signal.h>

#include <iostream.h>

#include "socks.h"

/*
extern "C" {
#include <netinet/in.h>
#include <arpa/inet.h>
}
*/

#include "Buffer.h"
#include "EndPoint.h"
#include "Reference.h"
#include "LoadMeasurement.h"
#include "PredictionRequestResponse.h"

#if defined(__FreeBSD__) 
#include "glarp.h"
#endif

#define LOADPREDICTION 0
#define LOADMONITOR    1
#define PREDRECONFIG   2

#define LOADPREDPORT_EV "LOADPREDPORT"
#define LOADMONITORPORT_EV "LOADMONITORPORT"
#define PREDRECONFIGPORT_EV "PREDRECONFIGPORT"

#define ARGBUFSIZE 256

#ifndef HUGE
#define HUGE 9999999999e99
#endif

static int get_endpoint(int kind, 
			const char* ipaddr_str,
			char *argbuf) {



  char argstart[] = "source:tcp:";
  char default_lpport[] = "5111";
  char default_lmport[] = "5112";
  char default_prport[] = "5113";
  char *lmport;
  char *lpport;
  char *prport;
  char *theport;

  
  if (kind == LOADPREDICTION) {
    if ((lpport = getenv(LOADPREDPORT_EV)) == NULL) {
      theport = default_lpport;
    } else {
      theport = lpport;
    }
  } else if (kind == LOADMONITOR) {
    if ((lmport = getenv(LOADMONITORPORT_EV)) == NULL) {
      theport = default_lmport;
    } else {
      theport = lmport;
    }
  } else if (kind == PREDRECONFIG) {
    if ((prport = getenv(PREDRECONFIGPORT_EV)) == NULL) {
      theport = default_prport;
    } else {
      theport = prport;
    }
  }
  

  // set up req_str here.
  int argstart_len = strlen(argstart);
  int argend_len   = strlen(theport) + 1; // +1 for the ':'
  int ipaddr_str_len;

  ipaddr_str_len = strlen(ipaddr_str);
  
  if (argstart_len + argend_len + ipaddr_str_len <= ARGBUFSIZE) {
    sprintf(argbuf, "%s%s:%s", 
	    argstart, ipaddr_str, theport);
  } else {
    return -1;
  }

  return 0;
}

int get_load_prediction(EndPoint ep, double* timeframe, double* loadavg,
			const char *ipaddr_str) 
{
  Reference<BufferDataRequest,BufferDataReply<PredictionResponse> > ref;

  BufferDataRequest req;
  BufferDataReply<PredictionResponse> repl;

  // *timeframe should be positive.


  //cerr << " About to connect now\n";

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"Can't Connect to \"%s\"\n","ipaddr_str");
    return -3;
  }

  //cerr << " Connected\n";

  // how many should I ask for?   Well.  A buffer of predictions holds some
  // number of predictions. Each prediction has an array of predictions made 
  // at its timestamp for various points in the (then) future.
  // So...  I should ask for only 1 prediction (the most recent one), and
  // then make sure that it has an entry in its array for a time within 
  // "epsilon" of the requested time.  If the array entries don't go far 
  // enough into the future, request reconfiguration to a number of timesteps
  // that DOES go that far.
  req.num=1;

  //cerr << " Begincall req.num=" << req.num << "\n";

  if (ref.Call(req,repl)) { 
      cerr << " foo\n";
    fprintf(stderr,"Sorry, call to \"%s\" failed\n","prediction server");
    return -4;
  }

  //cerr << " End call\n";


  if (repl.num < 1) {
    cerr << " foo\n";
    fprintf(stderr,
	    "Sorry, call to \"%s\" returned too little data\n","prediction server");
    return -5;
  }

  TimeStamp tsNow;
  double requestedTSval = (double)(tsNow) + *timeframe;
  double periodInSeconds = (double)(repl.data[0].period_usec) / 1000000.0;
  double lastPredictionTS = 
    ((double)repl.data[0].predtimestamp + 
     (periodInSeconds * (double)(repl.data[0].numsteps)));

  if ((lastPredictionTS - requestedTSval) < 1.0) {
    // reconfigure and try again!
    int newnumsteps = (int) ((*timeframe + 1.0) * periodInSeconds);

#define DEFAULT_SERLEN 300
#define DEFAULT_PERIOD 1000000
    double temp[DEFAULT_SERLEN];
    unsigned serlen = DEFAULT_SERLEN;
    char argbuf[ARGBUFSIZE];
    int ep_success;

    EndPoint rc_ep;
    ep_success = get_endpoint(PREDRECONFIG, ipaddr_str, argbuf);
    if (ep_success != 0) {
      fprintf(stderr, "could not get endpoint for \"%s\"\n", argbuf);
      return -11;
    }

    PredictionReconfigurationRequest req(repl.data[0].modelinfo,
					 tsNow,
					 serlen,temp,
					 DEFAULT_PERIOD,
					 newnumsteps);
    PredictionReconfigurationResponse resp;

    Reference<PredictionReconfigurationRequest,PredictionReconfigurationResponse> rcref;

    if (rcref.ConnectTo(rc_ep)) {
      fprintf(stderr,"Can't connect to server\n");
      exit(-1);
    }
    
    if (rcref.Call(req,resp)) { 
      exit(-1);
    }
    
    fprintf(stdout,"Call succeeded.  Response follows...\n");
    resp.Print();
    
    
    // recur to do the work!
    return get_load_prediction(ep, timeframe, loadavg, ipaddr_str);
    
  }

  // all is well.  We will find a prediction that meets the request
  // somewhere within the array.
  double bestDiff = HUGE;
  double thisDiff;
  double thisTSval = (double)(repl.data[0].predtimestamp);
  int bestIdx = -1;
  int i;
  for (i=0;i<repl.data[0].numsteps;i++) {
    // find the entry that most closely matches the requested timeframe
    // and return it.
    thisDiff = requestedTSval - thisTSval;
    if (fabs(thisDiff) <= bestDiff) {
      bestDiff = thisDiff;
      bestIdx = i;
      if ((bestDiff == 0.0) || (thisTSval > requestedTSval)) {
	// We've either found exactly the entry we're looking for, or
	// gone past the requested timeframe.  In either case, there's
	// no point in searching farther.
	break;
      }
    }
    thisTSval += periodInSeconds;
  }
  //cerr << " Wrote stuff back\n";

  if (bestIdx >= 0) {
    // Found a match of some quality or other.
    *timeframe = *timeframe + bestDiff;
    *loadavg   = repl.data[0].preds[bestIdx];
    return 0;
  } else {
    *loadavg = -1.0;
    return -5;
  }

  
}


int get_load_measurement(EndPoint ep, double* timeframe, double* loadavg) {
  Reference<BufferDataRequest,BufferDataReply<LoadMeasurement> > ref;
  

  BufferDataRequest req;
  BufferDataReply<LoadMeasurement> repl;

  //cerr << " About to connect now\n";

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"Can't Connect to \"%s\"\n","ipaddr_str");
    return -3;
  }

  //cerr << " Connected\n";

  // how many should I ask for?  want to be sure I come as close as 
  // possible to the requested timeframe.  Currently ask for 1000 values.
  // Note that this is an arbitrary choice!
  req.num=1000;

  //cerr << " Begincall req.num=" << req.num << "\n";

  if (ref.Call(req,repl)) { 
      cerr << " foo\n";
    fprintf(stderr,"Sorry, call to \"%s\" failed\n","ipaddr_str");
    return -4;
  }

  //cerr << " End call\n";

  TimeStamp tsNow;
  double requestedTSval = (double)(tsNow) + *timeframe;
  //  double requestedTSval = 
  //(((double)(tsNow.tv_sec) + (double)tsNow.tv_usec/1e6) + *timeframe);
  double bestDiff = HUGE;
  double thisDiff;
  int bestIdx = -1;
  int i;
  for (i=0;i<repl.num;i++) {
    // find the entry that most closely matches the requested timeframe
    // and return it.
    double thisTSval = (double)(repl.data[i].timestamp);
    thisDiff = requestedTSval - thisTSval;
    if (fabs(thisDiff) <= bestDiff) {
      bestDiff = thisDiff;
      bestIdx = i;
      if (bestDiff == 0.0) {
	// found exactly the entry we're looking for.
	// no point in searching farther.
	break;
      }
    }
  }
  //cerr << " Wrote stuff back\n";

  if (bestIdx >= 0) {
    // Found a match of some quality or other.
    *timeframe = *timeframe + bestDiff;
    *loadavg   = repl.data[bestIdx].avgs[0];
    return 0;
  } else {
    *loadavg = -1.0;
    return -5;
  }

}

  // ipaddr_str is a string that holds the IpAddress of the machine we're
  //           interested in.
  // timeframe is an in/out variable.  Pass in
  //           a delta in seconds that represents the timeframe for which 
  //           you want data.  Negative numbers are seconds-in-the-past; 
  //           Positive numbers request predictions for seconds-in-the-future;
  //           0.0 gives now.
  //           On (successful) return, timeframe will hold the delta from "now"
  //           to the timestamp of the reported data. Note that we don't have 
  //           synchronized clocks(!!!!), so a request for 0.0 may come back
  //           with a number that looks slightly negative or slightly 
  //           positive! 
  // loadavg   is an OUT variable.  On (successful) return it will hold the
  //           load-average value (or prediction) for the given timeframe.
  // RETURN_VAL return value is 0 for a successful call.  other return values 
  //           indicate errors.  The values of timeframe and loadavg are 
  //           UNDEFINED when the return value is != 0.
extern "C" int getloadfrombuffer(const char * ipaddr_str, // IP addr of target
				 double *timeframe, // in/out req timeframe
				 double *loadavg) { // out loadaverage value

  char argbuf[ARGBUFSIZE];
  
  EndPoint ep;
  int ep_success;
  int get_success;

  // cerr << " in getloadfrombuffer\n";

  if (*timeframe > 0.0) {
    // Want a prediction.  Start by setting up the port number.
    ep_success = get_endpoint(LOADPREDICTION, ipaddr_str, argbuf);
  } else {
    ep_success = get_endpoint(LOADMONITOR, ipaddr_str, argbuf);
  }
  if (ep_success != 0) {
    fprintf(stderr, "could not get endpoint for \"%s\"\n", argbuf);
    return -1;
  }
  
  // cerr << " connect to " << argbuf << " \n";

  if (ep.Parse(argbuf) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"Failed to parse \"%s\"\n",argbuf);
    return -2;
  }

  // cerr << " Parsed!\n";

  if (*timeframe > 0.0) {
    get_success = get_load_prediction(ep, timeframe, loadavg, ipaddr_str);
  } else {
    get_success = get_load_measurement(ep, timeframe, loadavg);
  }
  
  return get_success;
}

#include "Spin.h"
#include "EstimateExecTime.h"
#include "random.h"
#include "TimeStamp.h"

void usage()
{
  fprintf(stderr,"usage: test_pred_tau num minint:maxint minsecs:macsecs taumin:taumax [tag prednetspec spinnetspec]+\n");
}



#ifdef ALPHA
extern "C" int usleep(int);
#endif



int main(int argc, char *argv[]) 
{
  int num;
  int numhosts;
  int firstserverarg;
  double minint,maxint;
  double minsecs, maxsecs;
  double taumin, taumax;
  int i,j;
  EndPoint      *sseps;
  SpinServerRef *ssrefs;
  EndPoint      *pbeps;
  PredBufferRef *pbrefs;
  SpinRequest spinreq;
  SpinReply   spinrepl;
  ExecTimeEstimationRequest ereq;
  ExecTimeEstimationReply   erepl;
  char **tags;
  

  if (argc<8 || ((argc-8)%3)) { 
    usage();
    exit(-1);
  }

  numhosts = (argc-4)/3;
  firstserverarg = 5;

  tags = new char * [numhosts];

  num=atoi(argv[1]);
  

  if ((sscanf(argv[2],"%lf:%lf",&minint,&maxint))!=2) { 
    fprintf(stderr,"Can't parse %s\n",argv[2]);
    exit(-1);
  }

  if ((sscanf(argv[3],"%lf:%lf",&minsecs,&maxsecs))!=2) { 
    fprintf(stderr,"Can't parse %s\n",argv[3]);
    exit(-1);
  }

  if ((sscanf(argv[4],"%lf:%lf",&taumin,&taumax))!=2) { 
    fprintf(stderr,"Can't parse %s\n",argv[4]);
    exit(-1);
  }

  sseps = new EndPoint [numhosts];
  ssrefs = new SpinServerRef [numhosts];
  pbeps = new EndPoint [numhosts];
  pbrefs = new PredBufferRef [numhosts];

  for (i=0,j=firstserverarg;i<numhosts;i++,j+=3) { 
    tags[i] = argv[j];
    if (pbeps[i].Parse(argv[j+1])) {
      fprintf(stderr,"Can't parse %s\n",argv[j]);
      goto fail;
    }
    if (pbrefs[i].ConnectTo(pbeps[i])) { 
      fprintf(stderr,"Can't connect to %s\n",argv[j]);
      goto fail;
    }
    if (sseps[i].Parse(argv[j+2])) {
      fprintf(stderr,"Can't parse %s\n",argv[j+1]);
      goto fail;
    }
    if (ssrefs[i].ConnectTo(sseps[i])) { 
      fprintf(stderr,"Can't connect to %s\n",argv[j+1]);
      goto fail;
    }
  }

  double intsec;
  double computesec;
  int host;

  InitRandom();

  //  char hostbuf[1024];
  // char *hostname,*temp;
  double tau;

  fprintf(stdout,"%%timestamp hostname tnom tlb texp tub tact(walltime) usrtime systime tau meanload discountedmeanload varload\n");

  for (i=0;i<num;i++) { 
    intsec=UniformRandom(minint,maxint);
    computesec=UniformRandom(minsecs,maxsecs);
    host = UnsignedRandom()%numhosts;
    tau = UniformRandom(taumin,taumax);
    spinreq.secs=computesec;
    //    spinreq.Print(stderr);
    usleep((int)(intsec*1e6));
    ereq.confidence=0.95;
    ereq.cputime=computesec;
    TimeStamp now(0);
    if (EstimateExecTime(pbrefs[host],ereq,erepl,tau)) { 
      fprintf(stderr,"EstimateExecTime Failed - aborting testcase\n");
      continue;
    }
    ssrefs[host].Call(spinreq,spinrepl);
    //    spinrepl.Print(stderr);
    //    strcpy(hostbuf,argv[firstserverarg+host+1]);
    // hostname=strstr(strstr(hostbuf,":")+1,":") + 1;
    //temp=strstr(hostname,":");
    //*temp=0;
    fprintf(stdout,"%f\t%s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
	    (double)now,tags[host],spinreq.secs,
	    erepl.cilower,erepl.expectedtime,erepl.ciupper,
	    spinrepl.wallsecs, spinrepl.usrsecs,spinrepl.syssecs,tau,
	    erepl.meanload,erepl.discountedmeanload,erepl.varload);
    fflush(stdout);
	    
  }
   
  delete [] pbrefs;
  delete [] pbeps;
  delete [] ssrefs;
  delete [] sseps;

  return 0;

 fail:
  delete [] pbrefs;
  delete [] pbeps;
  delete [] ssrefs;
  delete [] sseps;

  return 0;
}
  
  

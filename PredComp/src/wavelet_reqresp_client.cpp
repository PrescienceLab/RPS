#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "Trace.h"
#include "PredComp.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Client for one-off wavelet transforms\n\n"
	  "usage: %s client wavelettype forward|reverse details|approx  inputfile\n\n"
	  "client                    = client endpoint for server\n"
	  "wavelettype               = wavelet type to use\n"
          "forward|reverse           = direction of transform\n"
          "details|approx|transform  = details, approximations, transforms\n"
	  "inputfile                 = input datafile (one or two column)\n"
          "                            for forward, preorder traversal\n"
          "                            of wavelet tree in one column for reverse\n"
	  "\n%s",n,b);
  delete [] b;
}


WaveletType GetWaveletType(const char *x) 
{
   if (!strcasecmp(x,"DAUB2")) {
     return DAUB2;
   } else if (!strcasecmp(x,"DAUB4")) { 
     return DAUB4;
   } else if (!strcasecmp(x,"DAUB6")) { 
     return DAUB6;
   } else if (!strcasecmp(x,"DAUB8")) { 
     return DAUB8;
   } else if (!strcasecmp(x,"DAUB10")) { 
     return DAUB10;
   } else if (!strcasecmp(x,"DAUB12")) { 
     return DAUB12;
   } else if (!strcasecmp(x,"DAUB14")) { 
     return DAUB14;
   } else if (!strcasecmp(x,"DAUB16")) { 
     return DAUB16;
   } else if (!strcasecmp(x,"DAUB18")) { 
     return DAUB18;
   } else if (!strcasecmp(x,"DAUB20")) { 
     return DAUB20;
   } else {
     fprintf(stderr,"wavelet_reqresp_client: Unknown wavelet type\n");
     exit(-1);
   }
}

int main(int argc, char *argv[])
{
   char *infile;
   WaveletRepresentationType wtype;
   bool forward;

   WaveletTransformBlockRequest req;
   WaveletTransformBlockResponse resp;


   if (argc!=6) { 
     usage(argv[0]);
     exit(-1);
   }
   
   EndPoint ep;
   
   if (ep.Parse(argv[1])) { 
     fprintf(stderr,"wavelet_reqresp_client: Can't parse \"%s\"\n",argv[1]);
     exit(-1);
   }
   if (ep.atype != EndPoint::EP_SOURCE || ep.ctype !=EndPoint::COMM_TCP) {
     fprintf(stderr,"wavelet_reqresp_client: Invalid netspec\n");
     exit(-1);
   }
   
   
   InitRandom();
   
   if (toupper(argv[4][0])=='D') {
     wtype=WAVELET_DOMAIN_DETAIL;
   } else if (toupper(argv[4][0])=='A') {
     wtype=WAVELET_DOMAIN_APPROX;
   } else if (toupper(argv[4][0])=='T') {
     wtype=WAVELET_DOMAIN_TRANSFORM;
   } else {
     usage(argv[0]);
     exit(-1);
   }
   
   if (toupper(argv[3][0])=='F') { 
     forward=true;
   } else if (toupper(argv[3][0])=='R') {
     forward=false;
   } else {
     usage(argv[0]);
     exit(-1);
   }

   infile=argv[5];
   double *temp;
   int numsamples=LoadGenericAsciiTraceFile(infile,&temp);
   
   if (forward) {
     req.ttype.direction=WAVELET_FORWARD;
     req.ttype.rinfoin=WaveletRepresentationInfo(TIME_DOMAIN, DAUB2, 0, 1);
     req.ttype.rinfoout=WaveletRepresentationInfo(wtype,DAUB2,0,1);
     req.ttype.bin=INORDER;
     req.ttype.bout=PREORDER;
     req.ttype.rinfoout.wtype = GetWaveletType(argv[2]);
     req.ttype.rinfoout.levels=(int)LOG2(((double)numsamples));
     if (((int)pow(2.0,(double)(req.ttype.rinfoout.levels)))!=numsamples) { 
       numsamples=(int)pow(2.0,(double)(req.ttype.rinfoout.levels));
       fprintf(stderr,"Warning: number of samples is not a power of two.\n");
       fprintf(stderr,"         Only the first %d samples will be used.\n",numsamples);
     }
     req.block.Resize(numsamples);
     memcpy(req.block.series,temp,sizeof(double)*numsamples);
   } else {
     req.ttype.direction=WAVELET_REVERSE;
     req.ttype.rinfoin=WaveletRepresentationInfo(wtype,DAUB2,0,1);
     req.ttype.rinfoout=WaveletRepresentationInfo(TIME_DOMAIN, DAUB2, 0, 1);
     req.ttype.bin=PREORDER;
     req.ttype.bout=INORDER;
     req.ttype.rinfoin.wtype = GetWaveletType(argv[2]);
     req.ttype.rinfoin.levels=(int)LOG2(((double)numsamples));
     if (((int)pow(2.0,(double)(req.ttype.rinfoin.levels)))!=numsamples) { 
       numsamples=(int)pow(2.0,(double)(req.ttype.rinfoin.levels));
       fprintf(stderr,"Warning: number of samples is not a power of two.\n");
       fprintf(stderr,"         Only the first %d samples will be used.\n",numsamples);
     }
     req.block.Resize(numsamples);
     memcpy(req.block.series,temp,sizeof(double)*numsamples);
   }
	      
   delete [] temp;

   Reference<WaveletTransformBlockRequest,WaveletTransformBlockResponse> ref;


   if (ref.ConnectTo(ep)) {
     fprintf(stderr,"wavelet_reqresp_client: Can't Connect to %s\n",argv[1]);
     exit(-1);
   }

   req.tag=req.block.tag=UnsignedRandom();
   req.timein=req.block.timestamp=TimeStamp(0);
   req.block.rinfo=req.ttype.rinfoin;
   req.block.btype=req.ttype.bin;

   cerr << req << endl;

   if (ref.Call(req,resp)) {
     fprintf(stderr,"wavelet_reqresp_client: Call failed\n");
   } else {
     cerr << resp << endl;
   }

   for (int i=0;i<resp.block.serlen;i++) {
     cout << i<<"\t"<<resp.block.series[i]<<endl;
   }

   ref.Disconnect();
   return 0;
}

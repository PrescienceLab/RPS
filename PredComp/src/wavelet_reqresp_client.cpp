#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "PredComp.h"

void usage() 
{
   fprintf(stderr,
      "wavelet_reqresp_client netspec wavelettype inputfile\n");
}


int main(int argc, char *argv[])
{
   char *infile;
   FILE *inp;
   double junk;

   WaveletTransformBlockRequest req;
   WaveletTransformBlockResponse resp;


   if (argc!=4) {
      usage();
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
   
   req.ttype.direction=FORWARD;
   req.ttype.rinfoin=WaveletRepresentationInfo(TIME_DOMAIN, DAUB2, 0, 1);
   req.ttype.bin=INORDER;
   req.ttype.rinfoout=WaveletRepresentationInfo(WAVELET_DOMAIN,DAUB2,0,1);
   req.ttype.bout=POSTORDER;
   
   if (!strcasecmp(argv[2],"DAUB2")) {
     req.ttype.rinfoout.wtype=DAUB2;
   } else if (!strcasecmp(argv[2],"DAUB4")) { 
     req.ttype.rinfoout.wtype=DAUB4;
   } else if (!strcasecmp(argv[2],"DAUB6")) { 
     req.ttype.rinfoout.wtype=DAUB6;
   } else if (!strcasecmp(argv[2],"DAUB8")) { 
     req.ttype.rinfoout.wtype=DAUB8;
   } else if (!strcasecmp(argv[2],"DAUB10")) { 
     req.ttype.rinfoout.wtype=DAUB10;
   } else if (!strcasecmp(argv[2],"DAUB12")) { 
     req.ttype.rinfoout.wtype=DAUB12;
   } else if (!strcasecmp(argv[2],"DAUB14")) { 
     req.ttype.rinfoout.wtype=DAUB14;
   } else if (!strcasecmp(argv[2],"DAUB16")) { 
     req.ttype.rinfoout.wtype=DAUB16;
   } else if (!strcasecmp(argv[2],"DAUB18")) { 
     req.ttype.rinfoout.wtype=DAUB18;
   } else if (!strcasecmp(argv[2],"DAUB20")) { 
     req.ttype.rinfoout.wtype=DAUB20;
   } else {
     fprintf(stderr,"wavelet_reqresp_client: Unknown wavelet type\n");
     exit(-1);
   }

   infile=argv[3];

   inp = fopen(infile,"r");
   if (inp==0) {
     fprintf(stderr,"wavelet_reqresp_client: : %s not found.\n",infile);
     exit(-1);
   }
   
   int numsamples=0;
   while ((fscanf(inp,"%lf %lf\n",&junk,&junk)>=1)) {
    ++numsamples;
   }
   rewind(inp);

   req.ttype.rinfoout.levels=(int)LOG2(numsamples);
   
   if (((int)pow(2,req.ttype.rinfoout.levels))!=numsamples) { 
     numsamples=(int)pow(2,req.ttype.rinfoout.levels);
     fprintf(stderr,"Warning: number of samples is not a power of two.\n");
     fprintf(stderr,"         Only the first %d samples will be used.\n",numsamples);
   }

   req.block.Resize(numsamples);

   for (int i=0;i<numsamples;i++) { 
     if (fscanf(inp,"%lf %lf\n",&junk,&(req.block.series[i]))==1) { 
       req.block.series[i]=junk;
     }
   }

   fclose(inp);


   Reference<WaveletTransformBlockRequest,WaveletTransformBlockResponse> ref;


   if (ref.ConnectTo(ep)) {
     fprintf(stderr,"wavelet_reqresp_client: Can't Connect to %s\n",argv[1]);
     exit(-1);
   }

   req.tag=req.block.tag=UnsignedRandom();
   req.timein=req.block.timestamp=TimeStamp(0);
   req.block.rinfo=req.ttype.rinfoin;
   req.block.btype=req.ttype.bin;

   cout << req << endl;

   if (ref.Call(req,resp)) {
     fprintf(stderr,"wavelet_reqresp_client: Call failed\n");
   } else {
     cout << resp << endl;
   }

   ref.Disconnect();
   return 0;
}

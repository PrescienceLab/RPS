#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <math.h>

#include "Wavelets.h"
#include "PredComp.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Server for one-off wavelet transforms\n\n"
	  "usage: %s server+\n\n"
	  "server          = one or more server endpoints\n"
	  "\n%s",n,b);
  delete [] b;
}



typedef WaveletInputSample<double> WISD;
typedef WaveletOutputSample<double> WOSD;

//
// input is in time domain, a sequence
// output is the approximations and details
// breadth-first traversal of wavelet details/approxs
// DC|top-level|top-level-1-left-to-right|...
int WaveletForwardDiscreteTransform(const WaveletType               wtype,
				    const WaveletRepresentationType rtype,
				    const double  *input,
				    const int      num,
				    double        *output)
{
  // assume that the input block is time domain and make a sampleblock from it
  SampleBlock<WISD> inblock;
  for (int i=0;i<num;i++) {
    insample.PushSampleBack(WISD(input[i],i));
  }

  DiscreteWaveletOutputSampleBlock<WOSD> outblock

  ForwardDiscreteWaveletTransform<double,WOSD,WISD> trans(wtype);

  switch (rtype) {
  case WAVELET_DOMAIN_DETAIL:
    trans.DiscreteWaveletTransformDetails(outblock,inblock);
    break;
  case WAVELET_DOMAIN_APPROX:
    trans.DiscreteWaveletTransformApproximations(outblock,inblock);
    break;
  case WAVELET_DOMAIN_TRANSFORM:
    trans.DiscreteWaveletTransform(outblock,inblock);
    break;
  default:
    assert(0);
  }

  unsigned n = outblock.GetBlockSize();

  for (unsigned i=0;i<num;i++) {
    output[i]=outblock[i].GetSampleValue();
  }
  return 0;
}



//
// output is in time domain, a sequence
// input is 
// breadth-first traversal of wavelet details/approxs
// DC|top-level|top-level-1-left-to-right|...
// Either of approximations or details can be empty
int WaveletReverseDiscreteTransform(const WaveletType               wtype,
				    const WaveletRepresentationType rtype,
				    const double *input,
				    const int     num,
				    double        *output)
{
  // assume that the input block is time domain and make a sampleblock from it
  DiscreteWaveletOutputSampleBlock<WOSD> inblock;

  int numlevels = LOG2(num) + (rtype==WAVELET_DOMAIN_TRANSFORM ? 1 : 0);


  int curlevel=numlevels-1;
  
  for (int i=0,nextpow2=1;i<num;i++) {
    inblock.PushSampleBack(WOSD(input[i],curlevel));
    if ((i+1)==nextpow2) {
      curlevel--;
      nextpow2*=2;
    }
  }
  
  ReverseDiscreteWaveletTransform<double,WISD,WOSD> trans(wtype);

  SampleBlock<WISD> outblock;

  switch (rtype) {
  case WAVELET_DOMAIN_DETAIL:
    trans.InverseDiscreteWaveletTransformDetails(outblock,inblock);
    break;
  case WAVELET_DOMAIN_APPROX:
    trans.InverseDiscreteWaveletTransformApproximations(outblock,inblock);
    break;
  case WAVELET_DOMAIN_TRANSFORM:
    trans.InverseDiscreteWaveletTransform(outblock,inblock);
    break;
  default:
    assert(0);
  }

  unsigned n = outblock.GetBlockSize();

  for (unsigned i=0;i<num;i++) {
    output[i]=outblock[i].GetSampleValue();
  }
  return 0;
}






// Generic right now
class WaveletTransformBlock {
public:
  static int Compute(const WaveletTransformBlockRequest &req, 
                           WaveletTransformBlockResponse &resp) {

    fprintf(stderr,"In Compute\n");

    if (req.ttype.direction==WAVELET_FORWARD) {
      if (!(req.rinfoin.rtype==TIME_DOMAIN && req.bin.btype==INORDER &&
	    (req.rinfoout.rtype==WAVELET_DOMAIN_APPROX||
	     req.rinfoout.rtype==WAVELET_DOMAIN_DETAIL||
	     req.rinfoout.rtype==WAVELET_DOMAIN_TRANSFORM) &&
	    req.bout==PREORDER)) {
	resp=req;
	return -1;
      }

      resp=req;

      resp.block.rinfo=req.rinfoout;
      resp.block.btype=req.bout;
      
      if (req.rinfoout.rtype!=WAVLET_DOMAIN_TRANSFORM) {
	resp.block.Resize(resp.block.serlen-1,false);
      }
      
      WaveletForwardDiscreteTransform(resp.rinfoout.wtype,
				      resp.rinfoout.rtype,
				      req.block.series,
				      req.block.serlen,
				      resp.block.series);

      resp.rinfoout.levels=LOG2(req.block.serlen) + 1 ;

      resp.block.timestamp=resp.timeout=TimeStamp();
      
      return 0;
    } else if  (req.ttype.direction==WAVELET_REVERSE) {
      if (!(req.rinfoout.rtype==TIME_DOMAIN && req.bout.btype==INORDER &&
	    (req.rinfoin.rtype==WAVELET_DOMAIN_APPROX||
	     req.rinfoin.rtype==WAVELET_DOMAIN_DETAIL||
	     req.rinfoin.rtype==WAVELET_DOMAIN_TRANSFORM) &&
	    req.bin==PREORDER)) {
	resp=req;
	return -1;
      }

      resp=req;

      resp.block.rinfo=req.rinfoout;
      resp.block.btype=req.bout;
      
      if (req.rinfoout.rtype!=WAVLET_DOMAIN_TRANSFORM) {
	resp.block.Resize(resp.block.serlen+1,false);
      }
      

      WaveletReverseDiscreteTransform(req.rinfoout.wtype,
				      req.rinfoout.rtype,
				      req.block.series,
				      req.block.serlen,
				      resp.block.series);

      resp.block.timestamp=resp.timeout=TimeStamp();
      
      return 0;
    }
  }
};

typedef  SerializeableRequestResponseMirror<WaveletTransformBlockRequest,
					    WaveletTransformBlock,
					    WaveletTransformBlockResponse> 
WaveletTransformBlockEngine;

class WaveletTransformBlockResponseSink : public LocalTarget {
public:
  void ProcessData(Buffer &buf) {
    WaveletTransformBlockResponse r;
    r.Unserialize(buf);
    r.Print();
  }
};



int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<2) {
    usage(argv[0]);
    exit(0);
  }


  WaveletTransformBlockEngine wave;
  WaveletTransformBlockResponseSink sink;

  wave.AddLocalTarget(&sink);

  for (i=1;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"wavelet_reqresp_server: Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (wave.AddEndPoint(*ep)) { 
      fprintf(stderr,"wavelet_reqresp_server: Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  fprintf(stderr,"wavelet_reqresp_server running\n");

  wave.Run();
  return 0;
}


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <math.h>

#include "Wavelets.h"
#include "PredComp.h"

#define LOG2(x) (log(x)/log(2.0))

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
				    const unsigned num,
				    double        *output)
{
  unsigned numlevels = (unsigned) LOG2(num) + (rtype==WAVELET_DOMAIN_TRANSFORM ? 1 : 0);

  // assume that the input block is time domain and make a sampleblock from it
  SampleBlock<WISD> inblock;
  for (unsigned i=0;i<num;i++) {
    inblock.PushSampleBack(WISD(input[i],i));
  }

  DiscreteWaveletOutputSampleBlock<WOSD> outblock;
  

  ForwardDiscreteWaveletTransform<double,WOSD,WISD> trans(wtype);

  switch (rtype) {
  case WAVELET_DOMAIN_DETAIL:
    trans.DiscreteWaveletDetailOperation(outblock,inblock);
    break;
  case WAVELET_DOMAIN_APPROX:
    trans.DiscreteWaveletApproxOperation(outblock,inblock);
    break;
  case WAVELET_DOMAIN_TRANSFORM:
    trans.DiscreteWaveletTransformOperation(outblock,inblock);
    break;
  default:
    assert(0);
  }

  unsigned n = outblock.GetBlockSize();

#if 0
  // FX
  for (unsigned i=0, level=numlevels-1;level>=0;level--) { 
    deque<WOSD> levelout;
    outblock.GetSamplesAtLevel(levelout,level);
    for (;i<pow;i++) {
      output[i]=levelout[i];
    }
  }
#endif

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
				    const unsigned num,
				    double        *output)
{
  // assume that the input block is time domain and make a sampleblock from it
  DiscreteWaveletOutputSampleBlock<WOSD> inblock;

  ReverseDiscreteWaveletTransform<double,WISD,WOSD> trans(wtype);

  SampleBlock<WISD> outblock;

  unsigned numlevels = (unsigned) LOG2(num) + (rtype==WAVELET_DOMAIN_TRANSFORM ? 1 : 0);

  unsigned curlevel;

#if 0 // FIX
  for (unsigned i=0, level=numlevels-1;level>=0;level--) { 
    deque<WOSD> levelout;
    for (;;i++) {
      levelout[i]=input[i];
    }
    outblock.GetSamplesAtLevel(levelout,level);
  }
#endif

  for (unsigned i=0,prevpow2=0,nextpow2=1;i<num;i++) {
    //
    // PROBABLY WRONG
    //
    inblock.PushSampleBack(WOSD(input[i],curlevel,i-prevpow2));
    if ((i+1)==nextpow2) {
      curlevel--;
      prevpow2=nextpow2;
      nextpow2*=2;
    }
  }
  

  switch (rtype) {
#if 0
  case WAVELET_DOMAIN_DETAIL:
    trans.InverseDiscreteWaveletDetailOperation(outblock,inblock);
    break;
  case WAVELET_DOMAIN_APPROX:
    trans.InverseDiscreteWaveletApproxOperation(outblock,inblock);
    break;
#endif
  case WAVELET_DOMAIN_TRANSFORM:
    trans.DiscreteWaveletTransformOperation(outblock,inblock);
    break;
  default:
    assert(0);
  }

  unsigned n = outblock.GetBlockSize();

  for (unsigned i=0;i<MIN(num,n);i++) {
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
      if (!(req.ttype.rinfoin.rtype==TIME_DOMAIN && req.ttype.bin==INORDER &&
	    (req.ttype.rinfoout.rtype==WAVELET_DOMAIN_APPROX||
	     req.ttype.rinfoout.rtype==WAVELET_DOMAIN_DETAIL||
	     req.ttype.rinfoout.rtype==WAVELET_DOMAIN_TRANSFORM) &&
	    req.ttype.bout==PREORDER)) {
	resp=req;
	return -1;
      }

      resp=req;

      resp.block.rinfo=req.ttype.rinfoout;
      resp.block.btype=req.ttype.bout;
      
      if (req.ttype.rinfoout.rtype!=WAVELET_DOMAIN_TRANSFORM) {
	resp.block.Resize(resp.block.serlen-1,false);
      }
      
      WaveletForwardDiscreteTransform(resp.ttype.rinfoout.wtype,
				      resp.ttype.rinfoout.rtype,
				      req.block.series,
				      req.block.serlen,
				      resp.block.series);

      resp.ttype.rinfoout.levels=(unsigned)LOG2(req.block.serlen) + 1 ;

      resp.block.timestamp=resp.timeout=TimeStamp();
      
      return 0;
    } else if  (req.ttype.direction==WAVELET_REVERSE) {
      if (!(req.ttype.rinfoout.rtype==TIME_DOMAIN && req.ttype.bout==INORDER &&
	    (req.ttype.rinfoin.rtype==WAVELET_DOMAIN_APPROX||
	     req.ttype.rinfoin.rtype==WAVELET_DOMAIN_DETAIL||
	     req.ttype.rinfoin.rtype==WAVELET_DOMAIN_TRANSFORM) &&
	    req.ttype.bin==PREORDER)) {
	resp=req;
	return -1;
      }

      resp=req;

      resp.block.rinfo=req.ttype.rinfoout;
      resp.block.btype=req.ttype.bout;
      
      if (req.ttype.rinfoout.rtype!=WAVELET_DOMAIN_TRANSFORM) {
	resp.block.Resize(resp.block.serlen+1,false);
      }
      

      WaveletReverseDiscreteTransform(req.ttype.rinfoout.wtype,
				      req.ttype.rinfoout.rtype,
				      req.block.series,
				      req.block.serlen,
				      resp.block.series);

      resp.block.timestamp=resp.timeout=TimeStamp();
      
      return 0;
    }
    return -1;
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


#include "linear.h"

int Filter::Apply(double *inseq, double *outseq, int num)
{
   if (num<0) { 
      return -1;
   }

   int i;

   for (i=0;i<num;i++) {
      outseq[i]=Evaluate(inseq,i);
   }

   return 0;
}

FIRFilter::FIRFilter() 
{
   coeffs=0;
}

FIRFilter::~FIRFilter()
{
   if (coeffs!=0) {
      delete [] coeffs;
   }
}


 
int FIRFilter::Initialize(int extent) 
{
   if (coeffs!=0) { 
      delete [] coeffs;
   } 
   this->extent=extent;
   coeffs=new double [extent];
   if (coeffs==0) {
      return -1;
   } else {
      return 0;
   }
}
      
#define MIN(x,y) ((x)<(y) ? (x) : (y))

// Note that the filter coeffs are stored in reverse
// order a_k, a_k-1, ... a_0 to facilitate convolution
// where both the filter coeffs and the sequence coeffs
// are indexed in the positive direction.
double FIRFilter::Evaluate(double *seq, int t)
{
   double val;
   int seq_i, filt_i;

   int num_pts = MIN(extent,t+1);

   val=0.0;
   for (seq_i=t-num_pts+1,filt_i=extent-num_pts;
        seq_i<=t;
        seq_i++, filt_i++) {
      val+=coeffs[filt_i]*seq[seq_i];
   }
   return val;
}

int FIRFilter::GetExtent()
{
   return extent;
}


void FIRFilter::SetCoefficient(int i, double value)
{
   if (i>=0 && i < extent) { 
      coeffs[extent-i-1]=value;
   }
}

double FIRFilter::GetCoefficient(int i)
{
   if (i>=0 && i < extent) { 
      return coeffs[extent-i-1];
   } else {
      return 0.0;
   }
}

void FIRFilter::Dump(FILE *out)
{
   fprintf(out,"FIR Filter with %d extent, coeffs:",extent);
   int i;
   for (i=0;i<extent;i++) {
      fprintf(out," %lf",GetCoefficient(i));
   }
   fprintf(out,"\n");
}


int MultipleOutputFilter::Apply(double *inseq, double *outmatrix, int num)
{
   if (num<0) { 
      return -1;
   }

   int i;
   int numfilt=GetNumOutputs();

   for (i=0;i<num;i++) {
      if (Evaluate(inseq,i,&(outmatrix[i*numfilt]))) {
         return -1;
      }
   }
   return 0;
}


MultiFIRFilter::MultiFIRFilter()
{
   numfilt=0;
   filters=0;
}

MultiFIRFilter::~MultiFIRFilter()
{
   if (filters!=0) {
      delete [] filters;
   }
}

int MultiFIRFilter::Initialize(int numfilt,int extent)
{
   int i;

   if (filters!=0) { 
      delete [] filters;
   }

   this->numfilt = numfilt;
   filters = new FIRFilter [ numfilt];
   
   if (filters==0) {
      return -1;
   }

   for (i=0;i<numfilt;i++) { 
      if (filters[i].Initialize(extent)) {
         delete [] filters;
         numfilt=0;
         return -1;
      }
   }

   return 0;

}

int MultiFIRFilter::Evaluate(double *inseq, int t, double *outvec)
{
   int i;

   for (i=0;i<numfilt;i++) {
      outvec[i]=filters[i].Evaluate(inseq,t);
   }
   return 0;
}

int MultiFIRFilter::GetNumOutputs()
{
   return numfilt;
}

int MultiFIRFilter::GetExtent()
{
   if (filters==0) {
      return 0;
   } else {
      return filters[0].GetExtent();
   }
}

int MultiFIRFilter::GetNumFilters()
{
   return numfilt;
}

void MultiFIRFilter::SetCoefficient(int filt, int num, double value)
{
   if (filt>=0 && filt<numfilt) {
      filters[filt].SetCoefficient(num,value);
   }
}

double MultiFIRFilter::GetCoefficient(int filt, int num)
{
   if (filt>=0 && filt<numfilt) {
      return filters[filt].GetCoefficient(num);
   } else {
      return 0.0;
   }
}

void MultiFIRFilter::Dump(FILE *out)
{
   fprintf(out,"Multiple Output FIR Filter with %d outputs:\n",numfilt);
   int i;
   for (i=0;i<numfilt;i++) {
      filters[i].Dump(out);
   }
}



#ifndef _Linear
#define _Linear

extern "C" {
#include <stdio.h>
}

class Filter {
public:
   virtual ~Filter() {}; 
   virtual double Evaluate(double *inseq, int t)=0;
   virtual int Apply(double *inseq, double *outseq, int num);
   virtual void Dump(FILE *out=stdout)=0;
};

// Causal, finite extent linear filter
class FIRFilter : public Filter {
private:
   int     extent;
   double *coeffs;
public:
   FIRFilter();
   virtual ~FIRFilter();
   int Initialize(int extent);
   virtual double Evaluate(double *inseq, int t);
   int            GetExtent();
   void           SetCoefficient(int i, double value);
   double         GetCoefficient(int i);
   void           Dump(FILE *out=stdout);
};


class MultipleOutputFilter {
public:
   virtual ~MultipleOutputFilter() {}
   virtual int GetNumOutputs()=0;
   virtual int Evaluate(double *inseq, int t, double *outvec)=0;
   virtual int Apply(double *inseq, double *outmatrix, int num);
   virtual void Dump(FILE *out=stdout)=0;
};


class MultiFIRFilter : public MultipleOutputFilter {
private:
   int       numfilt;
   FIRFilter *filters;
public:
   MultiFIRFilter();
   virtual ~MultiFIRFilter();
   int Initialize(int numfilt, int extent);
   virtual int Evaluate(double *inseq, int t, double *outvec);
   int    GetNumOutputs();
   int    GetExtent();
   int    GetNumFilters();
   void   SetCoefficient(int filt, int num, double value);
   double GetCoefficient(int filt, int num);
   void   Dump(FILE *out=stdout);
};

#endif

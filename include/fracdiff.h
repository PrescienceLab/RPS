#define FLMIN 
extern "C" void fracdf_(double *x, int *n, int *m, int *nar, 
			int *nma, double *dtol, double *drange,
			double *hood, double *d, double *ar, double *ma,
			double *w, int *lenw, int *inform, double *flmin,
			double *flmax, double *epmin, double *epmax);

inline 
int  FitFARIMA(double       *x,  // series
	       int           n,  // length of series
               int           m,  // terms in MLE approximation (100)
               int         nar,  // # AR params
               int         nma,  // # MA params
               double     dtol,  // desired length of uncertainty for d (-1)
               double     dmin,  // min d
               double     dmax,  // max d
               double      ind,  // initial estimate of d
               double *outdtol,  // output dtol
               double    *outd,  // output d
               double    *hood,  // ML log
               double      *ar,  // ar paramters
               double      *ma)  // ma parameters
{
  
                     

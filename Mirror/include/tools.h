#ifndef _tools
#define _tools


double Mean(double *sequence, int len);
double Variance(double *sequence,int len);
double StandardDeviation(double *sequence, int len);
double Min(double *seq, int len);
double Max(double *seq, int len);
double Median(double *sequence, int len);

// subtract the mean of the sequence from each element and return the 
// mean
double MeanifySequence(double *sequence, int len);
void   UnMeanifySequence(double *sequence, int len, double mean);

// Difference a sequence and return first element
// sequence has one fewer element
double DifferenceSequence(double *sequence, int len);
// Reverse a difference - sequence has one more element
void   UnDifferenceSequence(double *sequence, int len, double first);

// Compute the first numlags components of the autocovariance function
// and return it in acovf[0..maxlag-1] and also return the mean
double ComputeAutoCov(double *sequence, int len, double *acovf, int numlag);

// And autocorrelation
double ComputeAutoCor(double *sequence, int len, double *acf, int numlag);


// Returns the highest confidence level at which we can
// reject the IID hypothesis
double AcfTestForIID(double *seq, int len, int maxlag=100, int numiters=100);
// Returns the number of coeffs that are outside the confidence interval
int    AcfTestForIID(double *seq, int len, int maxlag=100, double conflevel=0.95);

// returns the highest confidence level at which we can reject
// the IID hypothesis
double TurningPointTestForIID(double *seq, int len, int *numtps=0);

// returns the highest confidence level at which we can reject that the sequence
// is IID by the nunber of sign changes
double SignTestForIID(double *seq, int len, int *nums=0);

// returns the highest confidence level at which we can reject that the sequence
// is IID by the portmanteau test
// Ljung/Box variant
double PortmanteauTestForIID(double *seq, int len, double *Q=0, int maxlag=100);

// Compare quantiles of seq1 to seq2, returning R^2 value of least squares
// line fitted, as well as m and b in seq1=m*seq2+b
double QuantileQuantile(double *seq1, double *seq2, int len, double *m=0, double *b=0);
// Compare quantiles of seq to quantiles of standard normal
double QuantilesOfNormal(double *seq, int len, double *m=0, double *b=0);


// Solve Sum(j=0,N-1; R[N+i-j-1]x[j]) = y[i] for i=0..N-1
// R[2*N-1],  x[N],  y[N]
int    SolveToeplitz(double *R, double *x, double *y, int N);

// LeastSquares fit a  y=mx+b line to (x,y) and return R^2 value
// as well as the fit parameters m and b
double LeastSquares(double *x, double *y, int len, double *m, double *b);


#endif

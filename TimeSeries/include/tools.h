#ifndef _tools
#define _tools


double Mean(const double *sequence, const int len);
double Variance(const double *sequence,const int len);
double StandardDeviation(const double *sequence, const int len);
double Min(const double *seq, const int len);
double Max(const double *seq, const int len);
double Median(const double *sequence, const int len);
double Sum(const double *sequence, const int len);
double SumOfSquares(const double *sequence, const int len);

// subtract the mean of the sequence from each element and return the 
// mean
double MeanifySequence(double *sequence, const int len);
void   UnMeanifySequence(double *sequence, const int len, const double mean);

// Difference a sequence and return first element
// sequence has one fewer element
double DifferenceSequence(double *sequence, const int len);
// Reverse a difference - sequence has one more element
void   UnDifferenceSequence(double *sequence, const int len, const double first);

// Compute the first numlags components of the autocovariance function
// and return it in acovf[0..maxlag-1] and also return the mean
double ComputeAutoCov(const double *sequence, const int len, double *acovf, const int numlag);

// And autocorrelation
double ComputeAutoCor(const double *sequence, const int len, double *acf, const int numlag);

// Compute the variance of each of the cumulative sums from 0..numlag-1
// And return the variance of the ultimate sum (sumvars[0])
// note that sumvars[0] is the sum of 1 item, sumvars[2] is the sum of 2, etc.
double AutoCov2VarianceOfSum(const double *autocov, const int num, double *sumvars);
double AutoCov2VarianceOfSum(const double *autocov, const int numautocov, 
			     double *sumvars, const int numsums);

double AutoCov2Covariances(const double *autocov, const int numautocov,
			   double *covars, const int numrows);

// Returns the highest confidence level at which we can
// reject the IID hypothesis
double AcfTestForIID(const double *seq, const int len, const int maxlag=100, const int numiters=100);
// Returns the number of coeffs that are outside the confidence interval
int    AcfTestForIID(const double *seq, const int len, const int maxlag=100, const double conflevel=0.95);

// returns the highest confidence level at which we can reject
// the IID hypothesis
double TurningPointTestForIID(const double *seq, const int len, int *numtps=0);

// returns the highest confidence level at which we can reject that the sequence
// is IID by the nunber of sign changes
double SignTestForIID(const double *seq, const int len, int *nums=0);

// returns the highest confidence level at which we can reject that the sequence
// is IID by the portmanteau test
// Ljung/Box variant
double PortmanteauTestForIID(const double *seq, const int len, double *Q=0, const int maxlag=100);

// Compare quantiles of seq1 to seq2, returning R^2 value of least squares
// line fitted, as well as m and b in seq1=m*seq2+b
double QuantileQuantile(const double *seq1, const double *seq2, const int len, double *m=0, double *b=0);
// Compare quantiles of seq to quantiles of standard normal
double QuantilesOfNormal(const double *seq, const int len, double *m=0, double *b=0);


// Solve Sum(j=0,N-1; R[N+i-j-1]x[j]) = y[i] for i=0..N-1
// R[2*N-1],  x[N],  y[N]
int    SolveToeplitz(const double *R, double *x, const double *y, const int N);

// LeastSquares fit a  y=mx+b line to (x,y) and return R^2 value
// as well as the fit parameters m and b
double LeastSquares(const double *x, const double *y, const int len, double *m, double *b);


#endif

#ifndef _IT
#define _IT


// Count unique values in sequence
// values and counts are also allocated and returned
int CountUnique(const double *seq, const int len, int *numuniq, 
                double **values, int **counts);
// Compute the average information (shannon entropy)
double AverageInformation(const double *seq, const int len);
// Compute the average mutual information between two sequences
double AverageMutualInformation(const double *seq1, const int len1, 
				const double *seq2, const int len2);
// Compute the average mutual information between the sequence and
// lagged versions of itself
int    AverageMutualInformationLagged(const double *seq, const int len,
				      double *mutinfo, 
				      const int minlag, const int steplag, const int maxlag);

// Compute the fraction of nearest neighbors that are false for
// global embedding dimensions
int    GlobalFalseNearestNeighbors(const double *seq, const int len, const int spacing, 
				   double *fractfalse, 
				   const int mindim, const int stepdim, const int maxdim,
				   const double threshold);

#endif

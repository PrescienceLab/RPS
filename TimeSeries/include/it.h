#ifndef _IT
#define _IT


// Count unique values in sequence
// values and counts are also allocated and returned
int CountUnique(double *seq, int len, int *numuniq, 
                double **values, int **counts);
// Compute the average information (shannon entropy)
double AverageInformation(double *seq, int len);
// Compute the average mutual information between two sequences
double AverageMutualInformation(double *seq1, int len1, 
				double *seq2, int len2);
// Compute the average mutual information between the sequence and
// lagged versions of itself
int    AverageMutualInformationLagged(double *seq, int len,
				      double *mutinfo, 
				      int minlag, int steplag, int maxlag);

// Compute the fraction of nearest neighbors that are false for
// global embedding dimensions
int    GlobalFalseNearestNeighbors(double *seq, int len, int spacing, 
				   double *fractfalse, 
				   int mindim, int stepdim, int maxdim,
				   double threshold);

#endif

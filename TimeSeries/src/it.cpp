#include <stdlib.h>
#include "util.h"
#include "tools.h"
#include <math.h>
#include <string.h>
#include "NewHashTable.h"
#include "it.h"


extern "C" int CompareDoubles(const void *pa, const void *pb)
{
  double &a=*((double*)pa);
  double &b=*((double*)pb);

  return (a<b ? -1 : (a>b ? 1 : 0));
}


int CountUnique(double *seq, int len, int *numuniq, 
                double **values, int **counts)
{
  double *work = new double [len];
  int i;
  int numunique;
  double cur;

  memcpy(work,seq,sizeof(double)*len);
  
  // Sort sequence
  qsort((void*)work,len,sizeof(double),&CompareDoubles);

  // Find number of unique values
  numunique=1;
  cur=work[0];
  for (i=1;i<len;i++) {
    if (work[i]!=cur) {
      ++numunique;
      cur=work[i];
    }
  }
  
  // count number of each unique variable
  int *num = new int [numunique];
  double *val = new double [numunique];
  for (i=0;i<numunique;i++) {
    num[i]=0;
  }

  int curunique=0;
  cur=work[0];
  val[0]=work[0];
  num[0]=1;
  for (i=1;i<len;i++) {
    if (work[i]!=cur) {
      ++curunique;
      cur=work[i];
      val[curunique]=work[i];
      num[curunique]=1;
    } else {
      num[curunique]++;
    }
  }

  CHK_DEL_MAT(work);
  
  *values=val;
  *counts=num;
  *numuniq=numunique;
  
  return 0;
}

double AverageInformation(double *seq, int len)
{
  double *val;
  int *num;
  int i;
  int numunique;

  if (CountUnique(seq,len,&numunique,&val,&num)) {
    return -1.0;
  }

  // Now compute the average information (Shannon Entropy)
  double H=0.0;

  for (i=0;i<numunique;i++) {
    H+=(((double)(num[i]))/((double)len))
       *LOG2(((double)(num[i]))/((double)len));
  }

  CHK_DEL_MAT(num);
  CHK_DEL_MAT(val);

  return -H;
}

class CompareDoubles {
public:
  static inline int Compare(double first, double second) {
    return (first < second ? -1 : (first > second ? +1 : 0 ));
  }		
};

class HashDouble {
public:
  static inline unsigned Hash(double a, unsigned size) {
    return (((unsigned *)(&a))[0] + ((unsigned *)(&a))[1]) % size;
  }
};

#ifdef __GNUC__
template class 
   ChainingHashTable<int,double,class CompareDoubles,class HashDouble>;
template class 
   Queue<ChainingHashTableItem<int,double> >;
#endif

double AverageMutualInformation(double *seq1, int len1, double *seq2, int len2)
{
#if 0
  double *vals1, *vals2;
  int *num1, *num2;
  int numunique1, numunique2;
  int i,j;

  // We need the unique symbols for each sequence and their
  // probabilities
  
  if (CountUnique(seq1,len1,&numunique1,&vals1,&num1)) {
    return -1.0;
  }

  if (CountUnique(seq2,len2,&numunique2,&vals2,&num2)) {
    CHK_DEL_MAT(vals1);	
    CHK_DEL_MAT(num1);
    return -1.0;
  }

  // Compute joint probabilities
  // This is rather slow - A serious implementation would use hashing
  int numsamples = MIN(len1,len2);
  int *jointcounts = new int [numunique1*numunique2];
  for (i=0;i<numunique1*numunique2;i++) {
    jointcounts[i]=0;
  }
  double *s1, *s2;
  
  for (i=0;i<numsamples;i++) {
    s1=(double*)
       bsearch(&(seq1[i]),vals1,numunique1,sizeof(double),CompareDoubles);
    s2=(double*)
       bsearch(&(seq2[i]),vals2,numunique2,sizeof(double),CompareDoubles);
    jointcounts[(s1-vals1)*numunique1+(s2-vals2)]++;
  }

  // Compute Mutual Informration

  double MI=0.0;

  for (i=0;i<numunique1;i++) {
    for (j=0;j<numunique2;j++) {
      MI+=((double)(jointcounts[i*numunique1+j])/((double)(numsamples)))
         *LOG2(
          ((double)(jointcounts[i*numunique1+j])/((double)(numsamples)))
                /
          ((((double)(num1[i]))/((double)len1))
                *  (((double)(num2[j]))/((double)len2))) ) ;
    }
  }

  CHK_DEL_MAT(jointcounts);
  CHK_DEL_MAT(vals1);
  CHK_DEL_MAT(vals2);
  CHK_DEL_MAT(num1);
  CHK_DEL_MAT(num2);

  return MI;

#else

  double *vals1, *vals2;
  int *num1, *num2;
  int numunique1, numunique2;
  int i,j;

  // We need the unique symbols for each sequence and their
  // probabilities
  
  if (CountUnique(seq1,len1,&numunique1,&vals1,&num1)) {
    return -1.0;
  }

  if (CountUnique(seq2,len2,&numunique2,&vals2,&num2)) {
    CHK_DEL_MAT(vals1);	
    CHK_DEL_MAT(num1);
    return -1.0;
  }
  // Now put the value->index mappings in hash tables
  ChainingHashTable<int,double,class CompareDoubles,class HashDouble>
    h1(numunique1), h2(numunique2);
  
  for (i=0;i<numunique1;i++) {
    h1.Add(i,vals1[i]);
  }

  for (i=0;i<numunique2;i++) {
    h2.Add(i,vals2[i]);
  }

  // Compute joint probabilities
  // This is rather slow - A serious implementation would use hashing
  int numsamples = MIN(len1,len2);
  int *jointcounts = new int [numunique1*numunique2];
  for (i=0;i<numunique1*numunique2;i++) {
    jointcounts[i]=0;
  }
  int s1, s2;
  
  for (i=0;i<numsamples;i++) {
    s1=h1.Lookup(seq1[i]);
    s2=h2.Lookup(seq2[i]);
    jointcounts[s1*numunique1+s2]++;
  }

  // Compute Mutual Informration

  double MI=0.0;

  for (i=0;i<numunique1;i++) {
    for (j=0;j<numunique2;j++) {
      MI+=((double)(jointcounts[i*numunique1+j])/((double)(numsamples)))
         *LOG2(
          ((double)(jointcounts[i*numunique1+j])/((double)(numsamples)))
                /
          ((((double)(num1[i]))/((double)len1))
                *  (((double)(num2[j]))/((double)len2))) ) ;
    }
  }

  CHK_DEL_MAT(jointcounts);
  CHK_DEL_MAT(vals1);
  CHK_DEL_MAT(vals2);
  CHK_DEL_MAT(num1);
  CHK_DEL_MAT(num2);

  return MI;
#endif
}  

//#define OLD

int    AverageMutualInformationLagged(double *seq, int len,
				      double *mutinfo, 
				      int minlag, int steplag, int maxlag)
{
#ifdef OLD
  int i;

  for (i=0;i<=maxlag;i++) {
    mutinfo[i] = AverageMutualInformation(seq,len-i,&(seq[i]),len-i);
  }

  return 0;
#else
#if 0
  double *vals;
  int *num;
  int numunique;
  int i,j,lag;

  // We need the unique symbols for each sequence and their
  // probabilities - only need to do this once
  
  CountUnique(seq,len,&numunique,&vals,&num);

  int *jointcounts = new int [numunique*numunique];

  for (lag=0;lag<=maxlag;lag++) {
    // Compute joint probabilities
    // This is rather slow - A serious implementation would use hashing
    int numsamples = len-lag;
    for (i=0;i<numunique*numunique;i++) {
      jointcounts[i]=0;
    }
    double *s1, *s2;
  
    for (i=0;i<numsamples;i++) {
      s1=(double*)
	bsearch(&(seq[i]),vals,numunique,sizeof(double),CompareDoubles);
      s2=(double*)
	bsearch(&(seq[i+lag]),vals,numunique,sizeof(double),CompareDoubles);
      jointcounts[(s1-vals)*numunique+(s2-vals)]++;
    }
    
    // Compute Mutual Informration	
    mutinfo[lag]=0.0;

    for (i=0;i<numunique;i++) {
      for (j=0;j<numunique;j++) {
        mutinfo[lag]+=
         ((double)(jointcounts[i*numunique+j])/((double)(numsamples)))
         *LOG2(
          ((double)(jointcounts[i*numunique+j])/((double)(numsamples)))
                /
          ((((double)(num[i]))/((double)len))
                *  (((double)(num[j]))/((double)len))) ) ;
      }
    }
  }

  CHK_DEL_MAT(jointcounts);
  CHK_DEL_MAT(vals);
  CHK_DEL_MAT(num);

  return 0;
#else
  double *vals;
  int *num;
  int numunique;
  int i,j,lag;
  int curidx;

  // We need the unique symbols for each sequence and their
  // probabilities - only need to do this once
  
  CountUnique(seq,len,&numunique,&vals,&num);

  int *jointcounts = new int [numunique*numunique];

  ChainingHashTable<int,double,class CompareDoubles, class HashDouble>
    h(numunique);

  for (i=0;i<numunique;i++) {
    h.Add(i,vals[i]);
  }
  
  for (lag=minlag;lag<=maxlag;lag+=steplag) {
    // Compute joint probabilities
    // This is rather slow - A serious implementation would use hashing
    int numsamples = len-lag;
    for (i=0;i<numunique*numunique;i++) {
      jointcounts[i]=0;
    }
    int s1,s2;
  
    for (i=0;i<numsamples;i++) {
      s1=h.Lookup(seq[i]);
      s2=h.Lookup(seq[i+lag]);
      jointcounts[s1*numunique+s2]++;
    }

    curidx = (lag-minlag)/steplag;
    // Compute Mutual Informration	
    mutinfo[curidx]=0.0;

    for (i=0;i<numunique;i++) {
      for (j=0;j<numunique;j++) {

        mutinfo[curidx]+= (jointcounts[i*numunique+j]==0) ? 0.0 :
         ((double)(jointcounts[i*numunique+j])/((double)(numsamples)))
         *LOG2(
          ((double)(jointcounts[i*numunique+j])/((double)(numsamples)))
                /
          ((((double)(num[i]))/((double)len))
                *  (((double)(num[j]))/((double)len))) ) ;
      }
    }
  }

  CHK_DEL_MAT(jointcounts);
  CHK_DEL_MAT(vals);
  CHK_DEL_MAT(num);

  return 0;
#endif
#endif
}



inline double ComputeDistance(int left, int right,
			      double *seq, int len, 
			      int spacing, int dim)
{
  int i;
  double dist;

  dist=0.0;
  for (i=0;i<dim;i++) {
    dist += SQUARE(seq[(left+i)*spacing]-seq[(right+i)*spacing]);
  }
  return dist;
}

// This is a really inefficent O(n^2) nearest neighbor search
// We compute the distance of each point
// [seq_i, seq_i+spacing, ... seq_i+(dim-1)spacing] 
//         for
//         i=0,spacing,...(spacing*floor(len/spacing))-spacing*(dim-1)
// to every other such point
int    FindNearestNeighbors(double *seq, int len, int spacing,
			    int dim, int *nearest, double *distances)
{
  int i,j;
  double thisdist;
  int numpts = (len/spacing) - dim + 1;

  for (i=0;i<numpts;i++) {
    distances[i]=1e99;
  }

  for (i=0;i<numpts;i++) {
    for (j=0;j<numpts;j++) {
      if (i!=j) {
	thisdist=ComputeDistance(i,j,seq,len,spacing,dim);
	if (thisdist<distances[i]) {
	  distances[i]=thisdist;
	  nearest[i]=j;
	}
      }
    }
  }
  return 0;
}   

int    ComputeDistances(double *seq, int len, int spacing,
			int dim, int *topts, double *distances)
{
  int i;
  int numpts = (len/spacing) - dim + 1;

  for (i=0;i<numpts;i++) {
    distances[i]=1e99;
  }

  for (i=0;i<numpts;i++) {
    distances[i]=ComputeDistance(i,topts[i],seq,len,spacing,dim);
  }
  return 0;
}  

  

int    GlobalFalseNearestNeighbors(double *seq, int len, int spacing, 
				   double *fractfalse, 
				   int mindim, int stepdim, int maxdim, 
				   double threshold)
{
  int i;
  int numfalse;
  int numneighbors;
  int dim;

  // Number of observations given the spacing between significant
  // observations T
  int numobs = len/spacing;

  double var = SQUARE(StandardDeviation(seq,len));

  // Indices and distances of nearest neighbors to each
  int *nearest = new int [numobs];
  double *dist = new double [numobs];
  double *newdist = new double [numobs];
  threshold*=threshold;

  for (dim=1;dim<=maxdim;dim++) {
    // Nearest neighbors in this dimension
    FindNearestNeighbors(seq,len,spacing,dim,nearest,dist);
    // Distances to those neighbors in the next dimension
    ComputeDistances(seq,len,spacing,dim+1,nearest,newdist);
    // Now see what fraction are false nearest neighbors
    numneighbors = (len/spacing) - ((dim+1) -1) ;
    numfalse=0;
    for (i=0;i<numneighbors;i++) {
      if (dist[i]==0.0) {
	if (newdist[i]>0.0) {
	  ++numfalse;
	}
      }else {
	if ((((newdist[i] - dist[i])/dist[i]) > threshold) 
            || 
            (((newdist[i] - dist[i])/var) > threshold) ) {
	  ++numfalse;
	}
      }
    }
    fractfalse[dim-1] = ((double)numfalse)/((double)numneighbors);
  }

  CHK_DEL_MAT(newdist);
  CHK_DEL_MAT(dist);
  CHK_DEL_MAT(nearest);

  return 0;
}

    

  
  
  

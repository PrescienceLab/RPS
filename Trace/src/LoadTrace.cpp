#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "LoadTrace.h"


#if !defined(IEEE_FLOAT_LSB) && !defined(IEEE_FLOAT_MSB)
#if defined (__alpha) || defined (__alpha__)
#define IEEE_FLOAT_LSB
#elif defined(sun) || defined(sparc)
#define IEEE_FLOAT_MSB
#elif defined(__i386__) || defined(i386) || defined(__i386)
#define IEEE_FLOAT_LSB
#else
#error Can not determine IEEE float byte order
#endif
#endif

#if !defined(IEEE_DOUBLE_LSB) && !defined(IEEE_DOUBLE_MSB)
#if defined (__alpha) || defined (__alpha__)
#define IEEE_DOUBLE_LSB
#elif defined(sun) || defined(sparc)
#define IEEE_DOUBLE_MSB
#elif defined(__i386__) || defined(i386) || defined(__i386)
#define IEEE_DOUBLE_LSB
#else
#error Can not determine IEEE double byte order
#endif
#endif

#if defined(IEEE_FLOAT_LSB)
#warning This is a IEEE_FLOAT_LSB machine
#endif
#if defined(IEEE_FLOAT_MSB)
#warning This is a IEEE_FLOAT_MSB machine
#endif
#if defined(IEEE_DOUBLE_LSB)
#warning This is a IEEE_DOUBLE_LSB machine
#endif
#if defined(IEEE_DOUBLE_MSB)
#warning This is a IEEE_DOUBLE_MSB machine
#endif

float htonf(const float x) 
{
#ifdef IEEE_FLOAT_LSB
// Swap bytes to make MSB  
  float x2=x;
  char *xt = (char *) &x2;
  float y;
  char *yt = (char *) &y;
  yt[0]=xt[3];
  yt[1]=xt[2];
  yt[2]=xt[1];
  yt[3]=xt[0];
  return y;
#else
// MSB first is the desired network byte order for floats
  return x;
#endif
}


float ntohf(const float x) 
{
  return htonf(x);
}


double htond(const double x) 
{
#ifdef IEEE_DOUBLE_LSB
// Swap bytes to make MSB 
  double x2=x;
  char *xt = (char *) &x2;
  double y;
  char *yt = (char *) &y;
  yt[0]=xt[7];
  yt[1]=xt[6];
  yt[2]=xt[5];
  yt[3]=xt[4];
  yt[4]=xt[3];
  yt[5]=xt[2];
  yt[6]=xt[1];
  yt[7]=xt[0];
  return y;
#else
// MSB first is the desired network byte order for doubles
  return x;
#endif
}


double ntohd(const double x) 
{
  return htond(x);
}


#define USE_PERVERT

void Pervert8(char *buf)
{
        char temp;
        unsigned i;

        for (i=0;i<4;i++) {
                temp=buf[i];
                buf[i]=buf[7-i];
                buf[7-i]=temp;
        }
}

int PervertBinaryTrace(double *timestamps, double *vals, int num)
{
  int i;
  for (i=0;i<num;i++) { 
    Pervert8((char*)&(timestamps[i]));
    Pervert8((char*) &(vals[i]));
  }
  return num;
}


int DenetworkifyBinaryTrace(double *timestamps, double *vals, int num)
{
#ifdef USE_PERVERT
#ifdef IEEE_DOUBLE_LSB
  return PervertBinaryTrace(timestamps,vals,num);
#else
  return num;
#endif
#else
  int i;
  for (i=0;i<num;i++) { 
    timestamps[i]=ntohd(timestamps[i]);
    vals[i]=ntohd(vals[i]);
  }
  return num;
#endif
}

int NetworkifyBinaryTrace(double *timestamps, double *vals, int num)
{
#ifdef USE_PERVERT
#ifdef IEEE_DOUBLE_LSB
  return PervertBinaryTrace(timestamps,vals,num);
#else
  return num;
#endif
#else
  int i;
  double temp;
  for (i=0;i<num;i++) { 
    timestamps[i]=htond(timestamps[i]);
    vals[i]=htond(vals[i]);
  }
  return num;
#endif
}


// returns number of entries
int LoadRawBinaryTraceFile(char *name, 
			   double **timestamps, 
			   double **vals)
{
  FILE *infile;
  struct stat st;
  unsigned numsamples;
  unsigned i;
  double *alldata;
  double *vs;
  double *ts;

  if (stat(name,&st)) { 
    fprintf(stderr,"Can't stat %s\n",name);
    return 0;
  }
  if ((st.st_size/16)*16 != st.st_size) { 
    fprintf(stderr,"Warning - %s does not contain an integral number of records\n",
	    name); 
  }

  numsamples = st.st_size/16;

  infile=fopen(name,"rb");

  alldata = new double [numsamples*2];
  ts = new double [numsamples];
  vs = new double [numsamples];

  if (fread(alldata,2*sizeof(double),numsamples,infile)!=numsamples) {
    fprintf(stderr,"fread failed\n");
    delete []  alldata;
    delete []  ts;
    delete []  vs;
    return -1;
  }

  for (i=0;i<numsamples;i++) { 
    ts[i] = alldata[0+i*2];
    vs[i]=alldata[1+i*2];
  }
  fclose(infile);
  delete [] alldata;
  *timestamps = ts;
  *vals = vs;
  return numsamples;
}


int StoreRawBinaryTraceFile(char *name, 
			    double *timestamps, 
			    double *vals,
			    int numsamples)
{
  FILE *outfile;
  int i;
  double *alldata;


  outfile=fopen(name,"wb");

  alldata = new double [numsamples*2];

  for (i=0;i<numsamples;i++) { 
    alldata[0+i*2] = timestamps[i];
    alldata[1+i*2] = vals[i];
  }

  if (fwrite(alldata,2*sizeof(double),numsamples,outfile)!=(unsigned)numsamples) {
    fprintf(stderr,"fwrite failed\n");
    delete [] alldata;
    return -1;
  }

  fclose(outfile);
  delete [] alldata;
  return numsamples;
}


int LoadNetworkBinaryTraceFile(char *name, 
			       double **timestamps, 
			       double **vals)
{
  int num = LoadRawBinaryTraceFile(name,timestamps,vals);
  return DenetworkifyBinaryTrace(*timestamps,*vals,num);
}


int LoadAsciiTraceFile(char *name,
		       double **timestamps, 
		       double **vals)
{
  double junk;
  int numrecs,i;
  FILE *inp;


  inp = fopen(name,"r");
  numrecs=0;
  while (fscanf(inp,"%lf %lf",&junk,&junk)==2) {
    numrecs++;
  }
  fclose(inp);

  *timestamps = new double [numrecs];
  *vals = new double [numrecs];

  inp = fopen(name,"r");
  for (i=0;i<numrecs;i++) { 
    fscanf(inp,"%lf %lf",&((*timestamps)[i]),&((*vals)[i]));
  }
  fclose(inp);

  return numrecs;
}

int StoreAsciiTraceFile(char *name,
			double *timestamps, 
			double *vals,
			int    numsamples)
{
  int i;
  FILE *out;
  
  out=fopen(name,"w");

  for (i=0;i<numsamples;i++) { 
    fprintf(out,"%f\t%f\n",timestamps[i],vals[i]);
  }
  return numsamples;
}

  
  


int StoreNetworkBinaryTraceFile(char *name, 
				double *timestamps, 
				double *vals,
				int numsamples)
{
  int num = NetworkifyBinaryTrace(timestamps,vals,numsamples);
  return StoreRawBinaryTraceFile(name,timestamps,vals,num);
}

int LoadAlphaBinaryTraceFile(char *name,
			     double **timestamps,
			     double **vals)
{
  int num = LoadRawBinaryTraceFile(name,timestamps,vals);
  /* Alpha is an IEEE_DOUBLE_LSB machine */
#ifdef IEEE_DOUBLE_MSB
  return PervertBinaryTrace(*timestamps,*vals,num);
#else
  return num;
#endif
}
  


int StoreAlphaBinaryTraceFile(char *name, 
			      double *timestamps, 
			      double *vals,
			      int numsamples)
{
#ifdef IEEE_DOUBLE_MSB
  int num = PervertBinaryTrace(timestamps,vals,numsamples);
#else
  int num = numsamples;
#endif
  return StoreRawBinaryTraceFile(name,timestamps,vals,num);
}




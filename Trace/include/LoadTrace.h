#ifndef _LoadTrace
#define _LoadTrace


int LoadNetworkBinaryTraceFile(char *name, 
			       double **timestamps, 
			       double **vals);

int StoreNetworkBinaryTraceFile(char *name, 
				double *timestamps, 
				double *vals,
				int numsamples);

int LoadAlphaBinaryTraceFile(char *name,
			     double **timestamps,
			     double **vals);

int StoreAlphaBinaryTraceFile(char *name, 
			      double *timestamps, 
			      double *vals,
			      int numsamples);

int LoadRawBinaryTraceFile(char *name, 
			   double **timestamps, 
			   double **vals);

int StoreRawBinaryTraceFile(char *name, 
			    double *timestamps, 
			    double *vals,
			    int numsamples);


int LoadAsciiTraceFile(char *name,
		       double **timestamps, 
		       double **vals);

int StoreAsciiTraceFile(char *name,
			double *timestamps, 
			double *vals,
			int    numsamples);

int LoadGenericAsciiTraceFile(char *name,
			      double **vals);

int StoreGenericAsciiTraceFile(char *name,
			       double *vals,
			       int numsamples);



int PervertBinaryTrace(double *timestamps, double *vals, int num);
int DenetworkifyBinaryTrace(double *timestamps, double *vals, int num);
int NetworkifyBinaryTrace(double *timestamps, double *vals, int num);
#endif

#include <string.h>
#include <stdio.h>
#include "TimeStamp.h"


#define NUMBYTES (16*1048576)


void init(char *buf, int n)
{
  int i;
  for (i=0;i<n;i++) { 
    buf[i]=i%256;
  }
}

void loop(char *to, const char *from, int n)
{
  int i;
  for (i=0;i<n;i++) {
    to[i]=from[i];
  }
}


    
int main()
{
  char *from = new char [NUMBYTES];
  char *to = new char [NUMBYTES];

  init(from,NUMBYTES);
  init(to,NUMBYTES);

  loop(to,from,NUMBYTES);

  TimeStamp startloop(0);
  loop(to,from,NUMBYTES);
  TimeStamp stoploop(0);

  memcpy(to,from,NUMBYTES);

  TimeStamp startmemcpy(0);
  memcpy(to,from,NUMBYTES);
  TimeStamp stopmemcpy(0);


  bcopy(from,to,NUMBYTES);

  TimeStamp startbcopy(0);
  bcopy(from,to,NUMBYTES);
  TimeStamp stopbcopy(0);

  fprintf(stdout,"loop:    %lf bytes/sec\n"
                 "memcpy:  %lf bytes/sec\n"
	         "bcpy:    %lf bytes/sec\n",
	  NUMBYTES/((double)stoploop-(double)startloop),
	  NUMBYTES/((double)stopmemcpy-(double)startmemcpy),
	  NUMBYTES/((double)stopbcopy-(double)startbcopy));
}

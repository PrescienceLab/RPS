#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int pow2(int i)
{
  int s=1;
  for (;i>0;i--) { 
    s*=2;
  }
  return s;
}

void usage()
{
  fprintf(stdout,"waveint NUMCOEFS NUMLEVELS DELAY_CONSTRAINT\n\n");
}

int main(int argc, char *argv[])
{
  int n, l, k;
  
  if (argc != 4) {
    usage();
    return 1;
  }

  n=atoi(argv[1]);
  l=atoi(argv[2]);
  k=atoi(argv[3]);

  int d[l];

  int bsyk=((pow2(l-1)-1)*(n-1))%pow2(l-1);
  int last=bsyk;

  fprintf(stdout,"bsyk=%d\n",bsyk);

  if (k>bsyk) {
    while (k>bsyk) {
      last = bsyk;
      bsyk+=pow2(l-1);
    }

    if ((bsyk-k) > (k-last)) {
      bsyk = last;
    }

  } else if (k<bsyk) {
    while (k<bsyk) {
      last = bsyk;
      bsyk-=pow2(l-1);
    }

    if ((k-bsyk) > (last-k)) {
      bsyk = last;
    }
    
  }
  
  fprintf(stdout,"bsyk nearest k=%d is %d\n", k,bsyk);

  k=bsyk;


  /* sanity check to see if k'=k will work */
  for (int i=0;i<l;i++) { 
    int t,b;
    t=k-(pow2(i)-1)*(n-1);
    b=pow2(i);
    if (abs(t)%abs(b)) {
      fprintf(stderr, "Won't work - d[%d] is noninteger (%f)\n",
	      i,((double)(t)/(double(b))));
      exit(-1);
    }
    d[i] = t/b;
  }

  for (int i=0;i<l;i++) { 
    fprintf(stdout,"d[%d]=%d ",i,d[i]);
    fprintf(stdout,"(2^%d-1)*(%d-1)+d[%d]*2^%d = %d (should =%d)\n",
	    i,n,i,i,(pow2(i)-1)*(n-1)+d[i]*pow2(i),k);
  }
  return 0;
}

    

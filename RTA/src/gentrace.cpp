#include <stdio.h>

main()
{
  int i;
  double trace[1000][2];

  for (i=0;i<1000;i++) {
    trace[i][0]=i;
    trace[i][1]=0.0;
  }

  for (i=60;i<120;i++) {
    trace[i][1]=0.5;
  }

  for (i=180;i<240;i++) {
    trace[i][1]=1.0;
  }

  for (i=300;i<360;i++) {
    trace[i][1]=1.5;
  }

  for (i=420;i<480;i++) {
    trace[i][1]=2.0;
  }

  for (i=540;i<600;i++) {
    trace[i][1]=.75;
  }

  for (i=660;i<720;i++) {
    trace[i][1]=((double)(i-660))/60.0;
  }
  
  fwrite(trace,sizeof(double)*2,1000,stdout);

}


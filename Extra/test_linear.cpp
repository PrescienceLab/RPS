#include "linear.h"
#include <stdio.h>

main()
{
   // Make an exp decay filter with 3 outputs
   // output 1:  1 .5 .25 .125
   // output 2:  1 -1 1 0 
   // output 3:  1 -.5 .25 -.125
   MultiFIRFilter mf;

   mf.Initialize(3,4);

   mf.SetCoefficient(0,0,1.0);
   mf.SetCoefficient(0,1,0.5);
   mf.SetCoefficient(0,2,0.25);
   mf.SetCoefficient(0,3,0.125);

   mf.SetCoefficient(1,0,1.0);
   mf.SetCoefficient(1,1,-1.0);
   mf.SetCoefficient(1,2,1.0);
   mf.SetCoefficient(1,3,0);

   mf.SetCoefficient(2,0,1.0);
   mf.SetCoefficient(2,1,-0.5);
   mf.SetCoefficient(2,2,0.25);
   mf.SetCoefficient(2,3,-0.125);

   // Set up an impulse input

   double *inseq = new double [100];
   double outvec[3*10];
   int i;
   inseq[0]=1.0;
   for (i=1;i<100;i++) {
      inseq[i]=0.0;
   }
   // Compute and display outputs
   mf.Apply(inseq,outvec,10);
   for (i=0;i<10;i++) {
      printf("%f\t%f\t%f\n",outvec[i*3+0],outvec[i*3+1],outvec[i*3+2]);
   }
}

#include <stdio.h>
#include <math.h>
#include "maths.h"

void main()
{
  int i;
  for (i=0;i<10;i++) {
    printf("%f\n",Gamma(i-0.4));
  }
  for (i=1;i<10;i++) {
    printf("%f\n",Gamma(i));
  }
  
   for (i=0;i<10;i++) {
     double a=Gamma(i-0.4);
     double b=Gamma(i+1);
     double c=Gamma(-0.4);
      printf("%f\n",a/(b*c));
   }
}

#ifndef _maths
#define _maths

#include <math.h>
#include <stdio.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#define M_PI       3.1415926535897932385E0  /*Hex  2^ 1 * 1.921FB54442D18 */
#endif


#include "rps_log.h"  
  

static inline double Factorial(int x)
{
  if (x<=1) {
    return 1.0;
  } else {
    int i;
    double val=1.0;
    for (i=2;i<x;i++) {
      val*=(double)i;
    }
    return val;
  }
}

// Return ln gamma(x) using the Lanczos method described in 
// Numerical Methods in Fortran Second Edition, pp 207
// xx>0 is required
static inline double LnGammaPos(double xx)
{
#define COEFFS           { 76.18009172947146e0, \
                          -86.50532032941677e0,\
                           24.01409824083091e0,\
                           -1.231739572450155e0,\
                            0.1208650973866179e-2,\
                           -0.5395239384953e-5}

   const double cof[6] = COEFFS;
   const double stp = 2.5066282746310005e0 ;
   int j;
   double ser, tmp, x, y;

   if (xx<=0) { 
     RPSLog(CONTEXT,50,"LnGammaPos() called with negative argument or zero.\n");
     return 0.0;
   }
   x=xx;
   y=x;
   tmp=x+5.5e0;
   tmp=(x+0.5e0)*log(tmp)-tmp;
   ser=1.000000000190015e0;
   for (j=0;j<6;j++) {
      y=y+1.0e0;
      ser=ser+cof[j]/y;
   }
   return tmp+log(stp*ser/x);
}

// Note that the gamma() in DEC's libm is FUCKING USELESS
// For example, it FAILS for x<=0 even for non-pole values!
// MS, of course, is even more fucking useless and doesn't even
// include a gamma() function
static inline double Gamma(double x)
{
  if (x>0.0) {
    return exp(LnGammaPos(x));
  } else {
    double xp = 1-x;
    return M_PI/(exp(LnGammaPos(xp))*sin(M_PI*xp));
  }
}

// These gamma functions are really meant for use by the chi
// squared stuff and may not be usable on their own.
static inline double GammaPdf(double alpha, double omega, double x)
{
  if (x<0.0) {
    return 0.0;
  } else {
    return (pow(x,alpha-1)*exp(-x/omega))/(Gamma(alpha)*pow(omega,alpha));
  }
}

static inline double GammaCdf(double alpha, double omega, double x)
{
  double val, sum;
  int i;

  val=1.0;
  sum=1.0;
  for (i=1;i<alpha;i++) {
    val*=(x/(omega*i));
    sum+=val;
  }
  return 1 - exp(-x/omega)*sum;
}

static inline double ChiSquaredPdf(int r, double x)
{
  return GammaPdf((double)r/2,2.0,x);
}

static inline double ChiSquaredCdf(int r, double x)
{
  return GammaCdf((double)r/2,2.0,x);
}

static inline double Erf(double x) 
{
#if defined(WIN32) && !defined(__CYGWIN__)
  return 0.0;
#else
#if 1  
  return erf(x);
#else 
  return 0.0;
#endif
#endif
}


static inline double inverse_error_func(double p);

static inline double ErfInv(double x)
{
  return inverse_error_func(x);
}  

static inline double ConfToSigmas(double conf)
{
  return sqrt(2.0)*ErfInv(conf);
}

static inline double SigmasToConf(double sigmas)
{
  return Erf(sigmas/sqrt(2.0));
}



// Stolen from the gnuplot 3.5 sources
static inline double 
inverse_error_func(double p) 
{
	/* 
           Source: This routine was derived (using f2c) from the 
                   FORTRAN subroutine MERFI found in 
                   ACM Algorithm 602 obtained from netlib.

                   MDNRIS code contains the 1978 Copyright 
                   by IMSL, INC. .  Since MERFI has been 
                   submitted to netlib, it may be used with 
                   the restriction that it may only be 
                   used for noncommercial purposes and that
                   IMSL be acknowledged as the copyright-holder
                   of the code.
        */



	/* Initialized data */
	static double a1 = -.5751703;
	static double a2 = -1.896513;
	static double a3 = -.05496261;
	static double b0 = -.113773;
	static double b1 = -3.293474;
	static double b2 = -2.374996;
	static double b3 = -1.187515;
	static double c0 = -.1146666;
	static double c1 = -.1314774;
	static double c2 = -.2368201;
	static double c3 = .05073975;
	static double d0 = -44.27977;
	static double d1 = 21.98546;
	static double d2 = -7.586103;
	static double e0 = -.05668422;
	static double e1 = .3937021;
	static double e2 = -.3166501;
	static double e3 = .06208963;
	static double f0 = -6.266786;
	static double f1 = 4.666263;
	static double f2 = -2.962883;
	static double g0 = 1.851159e-4;
	static double g1 = -.002028152;
	static double g2 = -.1498384;
	static double g3 = .01078639;
	static double h0 = .09952975;
	static double h1 = .5211733;
	static double h2 = -.06888301;

	/* Local variables */
	static double a, b, f, w, x, y, z, sigma, z2, sd, wi, sn;

	x = p;

	/* determine sign of x */
	if (x > 0)
		sigma = 1.0;
	else
		sigma = -1.0;

	/* Note: -1.0 < x < 1.0 */

	z = fabs(x);

	/* z between 0.0 and 0.85, approx. f by a 
	   rational function in z  */

	if (z <= 0.85) {
		z2 = z * z;
		f = z + z * (b0 + a1 * z2 / (b1 + z2 + a2 
		    / (b2 + z2 + a3 / (b3 + z2))));

	/* z greater than 0.85 */
	} else {
		a = 1.0 - z;
		b = z;

		/* reduced argument is in (0.85,1.0), 
		   obtain the transformed variable */

		w = sqrt(-(double)log(a + a * b));

		/* w greater than 4.0, approx. f by a 
		   rational function in 1.0 / w */

		if (w >= 4.0) {
			wi = 1.0 / w;
			sn = ((g3 * wi + g2) * wi + g1) * wi;
			sd = ((wi + h2) * wi + h1) * wi + h0;
			f = w + w * (g0 + sn / sd);

		/* w between 2.5 and 4.0, approx. 
		   f by a rational function in w */

		} else if (w < 4.0 && w > 2.5) {
			sn = ((e3 * w + e2) * w + e1) * w;
			sd = ((w + f2) * w + f1) * w + f0;
			f = w + w * (e0 + sn / sd);

		/* w between 1.13222 and 2.5, approx. f by 
		   a rational function in w */
		} else if (w <= 2.5 && w > 1.13222) {
			sn = ((c3 * w + c2) * w + c1) * w;
			sd = ((w + d2) * w + d1) * w + d0;
			f = w + w * (c0 + sn / sd);
		}
	}
	y = sigma * f;
	return(y);
}


#endif

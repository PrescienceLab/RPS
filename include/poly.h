#ifndef _poly
#define _poly

#include <stdio.h>
#include <math.h>

#define ADD_OPTIONS_NONE     0
#define ADD_OPTIONS_NO_FIXUP 1

class PolynomialRatio;

class Polynomial {
private:
  double *coeffs;
  int    coeffsize;
  int    numcoeffs;
  int    power;
  int    Resize(int newnumcoeffs);
  int    Fixup();
  int    AddInternal(Polynomial *right, int options=ADD_OPTIONS_NONE);
public:
  Polynomial();
  virtual ~Polynomial();


  int    Duplicate(Polynomial *right);
  Polynomial *Clone();
  int    Clear();

  int    Initialize(int pow, int numcoeffs, double *coeffs);
  int    GetNumCoeffs();
  int    SetCoeff(int i, double newcoeff);
  double GetCoeff(int i);
  int    SetPower(int pow);
  int    GetPower();
  double Evaluate(double x);

  int    Equals(Polynomial *right);

  int Add(Polynomial *right);
  int Subtract(Polynomial *right);
  int MultiplyBy(double c);
  int MultiplyBy(Polynomial *right);
  // by defau lt, division stops once the result has as many coeffs
  // as the dividend.  The user can specify how many division steps to
  // execute using truncation limit
  int DivideBy(Polynomial *right, int trunctationlimit=0);
  // In this variant, the remainder is also returned
  int DivideBy(Polynomial *right, PolynomialRatio **remainder, 
	       int truncationlimit=0);
  int RaiseTo(int power);

  int RaiseTo(double power, int truncationlimit=0);

  void Dump(FILE *out=stdout);
};



class PolynomialRatio {
private:
  Polynomial *numerator, *denominator;
public:
  PolynomialRatio();
  virtual ~PolynomialRatio();
  
  int Fixup();

  int SetNumerator(Polynomial *num);
  int SetDenominator(Polynomial *denom);
  Polynomial *GetNumerator();
  Polynomial *GetDenominator();

  double Evaluate(double x);
  Polynomial *Approximate(int truncationlimit=0);

  void Dump(FILE *out=stdout);

};

Polynomial * Add(Polynomial *left, Polynomial *right);
Polynomial * Subtract(Polynomial *left, Polynomial *right);
Polynomial * Multiply(Polynomial *left, Polynomial *right);
Polynomial * Divide(Polynomial *left, Polynomial *right, int truncationlimit=0);
Polynomial * Divide(Polynomial *left, Polynomial *right, 
		    PolynomialRatio **remainder, int truncationlimit=0);
Polynomial * Raise(Polynomial *left, double power);

// Generate the polynomial (1-B^-1)^d (for d in 0.0 0.5)
Polynomial *MakeDeltaFracD(double d, int truncationlimit);

#endif

#ifndef _poly
#define _poly

#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

#define ADD_OPTIONS_NONE     0
#define ADD_OPTIONS_NO_FIXUP 1

class PolynomialRatio;

class Polynomial {
private:
  double *coeffs;
  int    coeffsize;
  int    numcoeffs;
  int    power;
  int    Resize(const int newnumcoeffs);
  int    Fixup();
  int    AddInternal(const Polynomial *right, const int options=ADD_OPTIONS_NONE);
public:
  Polynomial();
  Polynomial(const Polynomial &rhs);
  virtual ~Polynomial();

  Polynomial & operator=(const Polynomial &rhs);
  
  int    Duplicate(const Polynomial *right) ;
  Polynomial *Clone() const;
  int    Clear();

  int    Initialize(const int pow, const int numcoeffs, const double *coeffs);
  int    GetNumCoeffs() const;
  int    SetCoeff(const int i, const double newcoeff);
  double GetCoeff(const int i) const;
  int    SetPower(const int pow);
  int    GetPower() const;
  double Evaluate(const double x) const;

  int    Equals(const Polynomial *right) const;

  int Add(const Polynomial *right);
  int Subtract(const Polynomial *right);
  int MultiplyBy(const double c);
  int MultiplyBy(const Polynomial *right);
  // by defau lt, division stops once the result has as many coeffs
  // as the dividend.  The user can specify how many division steps to
  // execute using truncation limit
  int DivideBy(const Polynomial *right, const int trunctationlimit=0);
  // In this variant, the remainder is also returned
  int DivideBy(const Polynomial *right, PolynomialRatio **remainder, 
	       const int truncationlimit=0);
  int RaiseTo(const int power);

  int RaiseTo(const double power, const int truncationlimit=0);

  void Dump(FILE *out=stdout) const; 

  ostream & operator<<(ostream &rhs) const;
};


inline ostream & operator<< (ostream &os, const Polynomial &p) {
  return p.operator<<(os);
}


class PolynomialRatio {
private:
  Polynomial *numerator, *denominator;
public:
  PolynomialRatio();
  PolynomialRatio(const PolynomialRatio &rhs);
  virtual ~PolynomialRatio();

  PolynomialRatio & operator=(const PolynomialRatio &rhs);
  
  int Fixup();

  int SetNumerator(Polynomial *num);
  int SetDenominator(Polynomial *denom);
  Polynomial *GetNumerator() const;
  Polynomial *GetDenominator() const;

  double Evaluate(const double x) const;
  Polynomial *Approximate(const int truncationlimit=0) const;

  void Dump(FILE *out=stdout) const;

  ostream & operator<<(ostream &rhs) const;

};

inline ostream & operator<< (ostream &os, const PolynomialRatio &p) {
  return p.operator<<(os);
}


Polynomial * Add(const Polynomial *left, const Polynomial *right);
Polynomial * Subtract(const Polynomial *left, const Polynomial *right);
Polynomial * Multiply(const Polynomial *left, const Polynomial *right);
Polynomial * Divide(const Polynomial *left, const Polynomial *right, const int truncationlimit=0);
Polynomial * Divide(const Polynomial *left, const Polynomial *right, 
		    PolynomialRatio **remainder, const int truncationlimit=0);
Polynomial * Raise(const Polynomial *left, const double power);

// Generate the polynomial (1-B^-1)^d (for d in 0.0 0.5)
Polynomial *MakeDeltaFracD(const double d, const int truncationlimit);

#endif

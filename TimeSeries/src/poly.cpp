#include "poly.h"
#include "maths.h"
#include <string.h>

#define CHK_DEL(x) if ((x)!=0) { delete (x); (x)=0; }
#define CHK_DEL_MAT(x) if ((x)!=0) { delete [] (x); (x)=0;}

#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))

int Polynomial::Resize(int newnumcoeffs)
{
   if (coeffsize<newnumcoeffs) {
      double *newcoeffs = new double [newnumcoeffs];
      if (newcoeffs==0) { 
         return -1;
      }
      memcpy(newcoeffs,coeffs,sizeof(double)*numcoeffs);
      int i;
      for (i=numcoeffs;i<newnumcoeffs;i++) {
         newcoeffs[i]=0;
      }
      CHK_DEL_MAT(coeffs);
      coeffs=newcoeffs;
      coeffsize=newnumcoeffs;
   } 

   numcoeffs=newnumcoeffs;

   return 0;
}

int Polynomial::Initialize(int pow, int numc, double *coeffs)
{
   if (Resize(numc)) {
      return -1;
   }

   power=pow;

   int i;
   for (i=0;i<numc;i++) {
      this->coeffs[i]=coeffs[i];
   }
   return 0;
}


#define IS_ZERO(x) (fabs((x)) > 1e-99)

int Polynomial::Fixup()
{
   int firstnonzero, lastnonzero;
   int i;

   for (firstnonzero=0;firstnonzero<numcoeffs;firstnonzero++) {
      if (IS_ZERO(coeffs[firstnonzero])) { 
         break;
      }
   }

   if (firstnonzero==numcoeffs) { // they are all zeros!
      power=0;
      Resize(0);
      return 0;
   }

   for (lastnonzero=numcoeffs-1;lastnonzero>=0;lastnonzero--) {
      if (IS_ZERO(coeffs[lastnonzero])) {  
         break;
      }
   }

   for (i=0;i<(lastnonzero-firstnonzero+1);i++) {
      coeffs[i]=coeffs[firstnonzero+i];
   }

   if (Resize(lastnonzero-firstnonzero+1)) { 
      return -1;
   }
   power-=firstnonzero;
   return 0;
}


Polynomial::Polynomial() 
{
   coeffs=0;
   coeffsize=0;
   numcoeffs=0;
   power=0;
}

Polynomial::~Polynomial()
{
   CHK_DEL_MAT(coeffs);
   coeffsize=0;
}

int Polynomial::Duplicate(Polynomial *right)
{
   if (Resize(right->numcoeffs)) { 
      return -1;
   }
   if (numcoeffs>0) { 
      memcpy(coeffs,right->coeffs,numcoeffs*sizeof(double));
   }
   power=right->power;
   return 0;
}

Polynomial * Polynomial::Clone()
{
   Polynomial *p = new Polynomial;

   if (p->Duplicate(this)) {
      delete p;
      return 0;
   }

   return p;
}

int Polynomial::Clear()
{
   power=0;
   return Resize(0);
}


int Polynomial::GetNumCoeffs()
{
   return numcoeffs;
}

int Polynomial::SetCoeff(int i, double newcoeff)
{
   if (i<0) {
      return -1;
   }
   if ((i+1) > numcoeffs) {
      if (Resize(i+1)) { 
         return -1;
      }
   }
   coeffs[i]=newcoeff;
   if (Fixup()) { 
      return -1;
   }
   return 0;
}

double Polynomial::GetCoeff(int i)
{
   if (i<0 || i>=numcoeffs) {
      return 0.0;
   }

   return coeffs[i];
}

int Polynomial::GetPower()
{
   return power;
}

int Polynomial::SetPower(int pow)
{
   power=pow;
   return 0;
}

double Polynomial::Evaluate(double x)
{
   int i;
   double result=0.0;

   for (i=0;i<numcoeffs;i--) {
      result = x*result + coeffs[i];
   }
   result*=pow(x,power);
   return result;
}

int Polynomial::AddInternal(Polynomial *right,int options)
{
   int i;

   // We require that lhs be of >= power than rhs
   // if that's not the case, flip the problem around and
   // copy back the answer
   if (power < right->power) { 
      Polynomial temp;

      if (temp.Duplicate(right)) {
         return -1;
      }
      if (temp.Add(this)) {
         return -1;
      }
      if (Duplicate(&temp)) {
         return -1;
      }
      return 0;
   } else {
      int startcoeff = power - right->power;
      int newsize = startcoeff + MAX(numcoeffs,startcoeff+right->numcoeffs);

      if (Resize(newsize)) {
         return -1;
      }

      for (i=startcoeff;i<MIN(newsize,startcoeff+right->numcoeffs);i++) {
         coeffs[i]+=right->coeffs[i-startcoeff];
      }

      if (!(options&ADD_OPTIONS_NO_FIXUP)) {
         if (Fixup()) {
            return -1;
         }
      }

      return 0;
   }
}

int Polynomial::Add(Polynomial *right)
{
   return AddInternal(right);
}

int Polynomial::Subtract(Polynomial *right)
{
   Polynomial temp;
   
   if (temp.Duplicate(right)) {
      return -1;
   }
   if (temp.MultiplyBy(-1.0)) {
      return -1;
   }
   if (temp.Add(this)) {
      return -1;
   }
   if (Duplicate(&temp)) {
      return -1;
   }
   return 0;
}

int Polynomial::MultiplyBy(double c)
{
   int i;

   for (i=0;i<numcoeffs;i++) {
      coeffs[i]*=c;
   }
   return 0;
}

// This is a simple convolution based implementation
int Polynomial::MultiplyBy(Polynomial *right)
{
// zero special case
   if (right->numcoeffs==0) { 
      power=0;
      if (Resize(0))  {
         return -1;
      } else {
         return 0;
      }
   }

   int numnewcoeffs = numcoeffs + right->numcoeffs - 1;
   double *outputcoeffs = new double[numcoeffs+right->numcoeffs-1];

   if (outputcoeffs==0) { 
      return -1;
   }

   int i,j;

   for (i=0;i<numnewcoeffs;i++) {
      outputcoeffs[i]=0.0;
   }

   for (i=0;i<numnewcoeffs;i++) {
      for (j=0;j<numnewcoeffs;j++) {
         outputcoeffs[i]+=GetCoeff(j)*right->GetCoeff(i-j);
      }
   }

   CHK_DEL_MAT(coeffs);
   coeffs=outputcoeffs;
   numcoeffs=coeffsize=numnewcoeffs;
   power+=right->power;
   if (Fixup()) {
      return -1;
   }
   return 0;
}

// Assume that denominator is larger than numerator
int Polynomial::DivideBy(Polynomial *right, 
                         int truncationlimit)
{
  PolynomialRatio *rem;

  DivideBy(right,&rem,truncationlimit);

  delete rem;

  return 0;
}


int Polynomial::Equals(Polynomial *right)
{
   if (numcoeffs != right->numcoeffs || power != right->power) {
      return -1;
   } else {
      int i;

      for (i=0;i<numcoeffs;i++) { 
         if (coeffs[i]!=right->coeffs[i]) {
            return -1;
         }
      } 

      return 0;
   }
}

int Polynomial::DivideBy(Polynomial *right, 
                         PolynomialRatio **remainder,
                         int truncationlimit)
{
  int i;
  int numsteps = truncationlimit==0 ? numcoeffs : truncationlimit;
  double *newcoeffs = new double [numsteps];
  Polynomial cur, mpyres;

  cur.Duplicate(this);
  cur.SetPower(0);
  
  for (i=0;i<numsteps;i++) {
     newcoeffs[i] = cur.coeffs[i] / right->coeffs[0];
     mpyres.Duplicate(right);
     mpyres.MultiplyBy(-newcoeffs[i]);
     mpyres.SetPower(-i);
     cur.AddInternal(&mpyres,ADD_OPTIONS_NO_FIXUP);
  }

  cur.SetPower(cur.GetPower()+right->power+(power-right->power));
  cur.Fixup();
  
  PolynomialRatio *rem = new PolynomialRatio;
  Polynomial *num = new Polynomial;
  Polynomial *denom = new Polynomial;

  num->Duplicate(&cur);
  denom->Duplicate(right);
  rem->SetNumerator(num);
  rem->SetDenominator(denom);
  *remainder=rem;

  CHK_DEL_MAT(coeffs);
  coeffs=newcoeffs;
  numcoeffs=numsteps;
  coeffsize=numsteps;
  
  power = power - right->power;

  return 0;

/*
  int i,j,k;
  int shift;
  int firststep;

  shift = numcoeffs-right->numcoeffs;

  // If
  if (shift<0) {
     Polynomial *num = new Polynomial;
     Polynomial *denom = new Polynomial;
     num->Duplicate(this);
     denom->Duplicate(right);
     PolynomialRatio *rat= new PolynomialRatio;
     rat->SetNumerator(num);
     rat->SetDenominator(denom);
     *remainder=rat;
     Resize(0);
     return 0;
  }

  double *newcoeffs = new double [numsteps];


  if (newcoeffs==0) {
    return -1;
  }

  // First, knock off leading zeros
  for (firststep=0,i=0;i<right->numcoeffs;i++) {
    if (right->coeffs[i] == 0) {
      newcoeffs[i]=0.0;
      ++firststep;
    } else {
      break;
    }
  }
  
  Polynomial cursub, mpyby;
  Polynomial zero;
  Polynomial *curresult = new Polynomial;
  
  curresult->Duplicate(this);

  for (i=0;i<numsteps;i++) {
    newcoeffs[i]=curresult->GetCoeff(i)/right->coeffs[firststep];
    mpyby.Duplicate(&zero);
    mpyby.SetCoeff(i,newcoeffs[i+firststep]);
    cursub.Duplicate(right);
    cursub.MultiplyBy(&mpyby);
    curresult->Subtract(&cursub);
  }

  CHK_DEL_MAT(coeffs);
  coeffs=newcoeffs;
  numcoeffs=numsteps;
  coeffsize=numsteps;

  Polynomial *denom = new Polynomial;
  PolynomialRatio *rat = new PolynomialRatio;

  denom->Duplicate(right);
  rat->SetNumerator(curresult);
  rat->SetDenominator(denom);

  *remainder=rat;
  Fixup();
  rat->Fixup();
  return 0;
*/
}

int Polynomial::RaiseTo(int power) 
{
   if (power==0) { 
      if (Resize(1)) { 
         return -1;
      } else {
	SetCoeff(0,1);
	return 0;
      }
   }

   if (power==1) {
     return 0;
   }

   if (power<0) {
      Polynomial *temp = new Polynomial;
      if (temp==0) { 
         return -1;
      }
      temp->SetCoeff(0,1);
      temp->DivideBy(this);
      this->Duplicate(temp);
      delete temp;
      power=-power;
   }

   int i;
   int numexp = (int) floor(log(power-1)/log(2));
   Polynomial *original = Clone();

   for (i=0;i<numexp;i++) {
      if (MultiplyBy(this)) {
         Duplicate(original);
         return -1;
      }
   }
   for (i=numexp;i<power-1;i++) {
      if (MultiplyBy(original)) {
         Duplicate(original);
         return -1;
      }
   }
   delete original;
   Fixup();
   return 0;
}

int Polynomial::RaiseTo(double power, int truncationlimit)
{
   return -1;
}


void Polynomial::Dump(FILE *out)
{
   int i;

   if (numcoeffs==0) {
      fprintf(out,"0\n");
   } else {
      for (i=0;i<numcoeffs;i++) { 
         fprintf(out," %s%lfx^%i",coeffs[i]>=0.0 ? "+" : "",coeffs[i],power-i);
      }
      fprintf(out,"\n");
   }
}


PolynomialRatio::PolynomialRatio()
{
  numerator=denominator=0;
}

PolynomialRatio::~PolynomialRatio()
{
  CHK_DEL(numerator);
  CHK_DEL(denominator);
}

int PolynomialRatio::Fixup()
{
   /*
   numerator->Fixup();
   denominator->Fixup();
   */
   return 0;
}

Polynomial *PolynomialRatio::GetNumerator()
{
  return numerator;
}


Polynomial *PolynomialRatio::GetDenominator()
{
  return denominator;
}


int PolynomialRatio::SetDenominator(Polynomial *denom)
{
  denominator=denom;
  return 0;
}

int PolynomialRatio::SetNumerator(Polynomial *num)
{
  numerator=num;
  return 0;
}


double PolynomialRatio::Evaluate(double x)
{
  return (numerator->Evaluate(x))/(denominator->Evaluate(x));
}

Polynomial *PolynomialRatio::Approximate(int truncationlimit)
{
  Polynomial *temp = new Polynomial;

  temp->Duplicate(numerator);

  temp->DivideBy(denominator,truncationlimit);

  return temp;

}

void PolynomialRatio::Dump(FILE *out)
{
  numerator->Dump(out);
  fprintf(out,"/\n");
  denominator->Dump(out);
}


Polynomial * Add(Polynomial *left, Polynomial *right)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->Add(right);
   return temp;
}

Polynomial * Subtract(Polynomial *left, Polynomial *right)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->Subtract(right);
   return temp;
}

Polynomial * Multiply(Polynomial *left, Polynomial *right)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->MultiplyBy(right);
   return temp;
}

Polynomial * Divide(Polynomial *left, Polynomial *right, int truncationlimit)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->DivideBy(right,truncationlimit);
   return temp;
}

Polynomial * Divide(Polynomial *left, Polynomial *right, 
		    PolynomialRatio **remainder, int truncationlimit)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->DivideBy(right,remainder,truncationlimit);
   return temp;
}

Polynomial * Raise(Polynomial *left, double power)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->RaiseTo(power);
   return temp;
}


Polynomial *MakeDeltaFracD(double d, int truncationlimit)
{
   Polynomial *diff = new Polynomial;

   diff->SetPower(0);

   int i;
   double gmdinv = 1.0/Gamma(-d);

   // Compute the coefficients for which i-d is negative or zero
   // Using the conventional formula since LnGamma(x) is complex for x<=0
   for (i=0;i<MIN(truncationlimit,(int)ceil(d));i++) {
     diff->SetCoeff(i,gmdinv*(Gamma(i-d)/Gamma(i+1)));
   }

   // Compute the remainder of the coefficeints using
   // logartihms to avoid having the multiplication explode
   for (i=MIN(truncationlimit,(int)ceil(d));i<truncationlimit;i++) {
      diff->SetCoeff(i,gmdinv*exp(LnGammaPos(i-d)-LnGammaPos(i+1)));
   }

   return diff;
}


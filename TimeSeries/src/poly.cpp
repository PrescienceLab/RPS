#include <new>
#include "poly.h"
#include "maths.h"
#include <string.h>

#define CHK_DEL(x) if ((x)!=0) { delete (x); (x)=0; }
#define CHK_DEL_MAT(x) if ((x)!=0) { delete [] (x); (x)=0;}

#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))

int Polynomial::Resize(const int newnumcoeffs)
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

int Polynomial::Initialize(const int pow, const int numc, const double *coeffs)
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

Polynomial::Polynomial(const Polynomial &rhs) 
{
  coeffs=0;
  coeffsize=0;
  numcoeffs=0;
  power=0;
  Resize(rhs.numcoeffs);
  power=rhs.power;
  memcpy(coeffs,rhs.coeffs,sizeof(coeffs[0])*rhs.numcoeffs);
}


Polynomial::~Polynomial()
{
   CHK_DEL_MAT(coeffs);
   coeffsize=0;
}

Polynomial & Polynomial::operator=(const Polynomial &rhs)
{
  this->~Polynomial();
  return *(new(this)Polynomial(rhs));
}


int Polynomial::Duplicate(const Polynomial *right)
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

Polynomial * Polynomial::Clone() const
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


int Polynomial::GetNumCoeffs() const
{
   return numcoeffs;
}

int Polynomial::SetCoeff(const int i, const double newcoeff)
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

double Polynomial::GetCoeff(const int i) const
{
   if (i<0 || i>=numcoeffs) {
      return 0.0;
   }

   return coeffs[i];
}

int Polynomial::GetPower() const
{
   return power;
}

int Polynomial::SetPower(const int pow)
{
   power=pow;
   return 0;
}

double Polynomial::Evaluate(const double x) const
{
   int i;
   double result=0.0;

   for (i=0;i<numcoeffs;i--) {
      result = x*result + coeffs[i];
   }
   result*=pow(x,power);
   return result;
}

int Polynomial::AddInternal(const Polynomial *right, const int options)
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

int Polynomial::Add(const Polynomial *right)
{
   return AddInternal(right);
}

int Polynomial::Subtract(const Polynomial *right)
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

int Polynomial::MultiplyBy(const double c)
{
   int i;

   for (i=0;i<numcoeffs;i++) {
      coeffs[i]*=c;
   }
   return 0;
}

// This is a simple convolution based implementation
int Polynomial::MultiplyBy(const Polynomial *right)
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
int Polynomial::DivideBy(const Polynomial *right, 
                         const int truncationlimit)
{
  PolynomialRatio *rem;

  DivideBy(right,&rem,truncationlimit);

  delete rem;

  return 0;
}


int Polynomial::Equals(const Polynomial *right) const
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

int Polynomial::DivideBy(const Polynomial *right, 
                         PolynomialRatio **remainder,
                         const int truncationlimit)
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

}

int Polynomial::RaiseTo(const int power) 
{
  int ppower=power;

   if (ppower==0) { 
      if (Resize(1)) { 
         return -1;
      } else {
	SetCoeff(0,1);
	return 0;
      }
   }

   if (ppower==1) {
     return 0;
   }

   if (ppower<0) {
      Polynomial *temp = new Polynomial;
      if (temp==0) { 
         return -1;
      }
      temp->SetCoeff(0,1);
      temp->DivideBy(this);
      this->Duplicate(temp);
      delete temp;
      ppower=-ppower;
   }

   int i;
   int numexp = (int) floor(log((double)(ppower-1))/log(2.0));
   Polynomial *original = Clone();

   for (i=0;i<numexp;i++) {
      if (MultiplyBy(this)) {
         Duplicate(original);
         return -1;
      }
   }
   for (i=numexp;i<ppower-1;i++) {
      if (MultiplyBy(original)) {
         Duplicate(original);
         return -1;
      }
   }
   delete original;
   Fixup();
   return 0;
}

int Polynomial::RaiseTo(const double power, const int truncationlimit)
{
   return -1;
}


void Polynomial::Dump(FILE *out) const
{
   int i;

   if (numcoeffs==0) {
      fprintf(out,"0\n");
   } else {
      for (i=0;i<numcoeffs;i++) { 
         fprintf(out," %s%f*x^%i",coeffs[i]>=0.0 ? "+" : "",coeffs[i],power-i);
      }
      fprintf(out,"\n");
   }
}

ostream & Polynomial::operator<<(ostream &os) const
{
  os <<"Polynomial(power="<<power<<", numcoeffs="<<numcoeffs<<"coeffs=(";
  
  for (int i=0;i<numcoeffs;i++) {
    if (i>0) {
      os <<", ";
    }
    os << coeffs[i];
  }
  os <<"))";
  return os;
}


PolynomialRatio::PolynomialRatio()
{
  numerator=denominator=0;
}

PolynomialRatio::PolynomialRatio(const PolynomialRatio &rhs)
{
  numerator=rhs.numerator->Clone();
  denominator=rhs.denominator->Clone();
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

Polynomial *PolynomialRatio::GetNumerator() const
{
  return numerator;
}


Polynomial *PolynomialRatio::GetDenominator() const
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


double PolynomialRatio::Evaluate(const double x) const
{
  return (numerator->Evaluate(x))/(denominator->Evaluate(x));
}

Polynomial *PolynomialRatio::Approximate(const int truncationlimit) const
{
  Polynomial *temp = new Polynomial;

  temp->Duplicate(numerator);

  temp->DivideBy(denominator,truncationlimit);

  return temp;

}

void PolynomialRatio::Dump(FILE *out) const
{
  numerator->Dump(out);
  fprintf(out,"/\n");
  denominator->Dump(out);
}

ostream &PolynomialRatio::operator<<(ostream &os) const
{
  os <<"PolynomialRatio(numerator=";
  os << *numerator;
  os <<", denominator=";
  os << *denominator;
  os <<")";
  return os;
}


Polynomial * Add(const Polynomial *left, const Polynomial *right)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->Add(right);
   return temp;
}

Polynomial * Subtract(const Polynomial *left, const Polynomial *right)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->Subtract(right);
   return temp;
}

Polynomial * Multiply(const Polynomial *left, const Polynomial *right)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->MultiplyBy(right);
   return temp;
}

Polynomial * Divide(const Polynomial *left, const Polynomial *right, const int truncationlimit)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->DivideBy(right,truncationlimit);
   return temp;
}

Polynomial * Divide(const Polynomial *left, const Polynomial *right, 
		    PolynomialRatio **remainder, const int truncationlimit)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->DivideBy(right,remainder,truncationlimit);
   return temp;
}

Polynomial * Raise(const Polynomial *left, const double power)
{
   Polynomial *temp=new Polynomial;
   temp->Duplicate(left);
   temp->RaiseTo(power);
   return temp;
}


Polynomial *MakeDeltaFracD(const double d, const int truncationlimit)
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


#ifndef _coefficients
#define _coefficients

#include <iostream>
#include <string>
#include <vector>

#include "waveletinfo.h"
#include "util.h"

class WaveletCoefficients {
private:
  WaveletType wt;
  string      waveletname;
  unsigned    numcoefs;
  double      *g_coefs, *h_coefs;

  void        init(WaveletType wt);

public:
  WaveletCoefficients();
  WaveletCoefficients(const WaveletCoefficients &rhs);
  WaveletCoefficients(WaveletType wt);
  virtual ~WaveletCoefficients();

  WaveletCoefficients & operator=(const WaveletCoefficients &rhs);

  void Initialize(WaveletType wt);
  void ChangeType(WaveletType wt);
  string GetWaveletName() const;
  unsigned GetNumCoefs() const;

  void GetTransformCoefsLPF(vector<double> & coefs) const; // g(-n) -> G(z^(-1))
  void GetTransformCoefsHPF(vector<double> & coefs) const; // h(-n) -> H(z^(-1))
  void GetInverseCoefsLPF(vector<double> & coefs) const;   // g(n)  -> G(z)
  void GetInverseCoefsHPF(vector<double> & coefs) const;   // h(n)  -> H(z)

  ostream & Print(ostream &os) const;
};

#endif

#include <cassert>
#include <cmath>

#include "coefficients.h"

// The following are tables of coefficients for different types of wavelets
//  g_n values are shown

// DAUBECHIES WAVELETS

// N=2
const double daub_g2[2] = {1.0/sqrt(2.0),
			   1.0/sqrt(2.0)};

// N=4
const double daub_g4[4] = {(1+sqrt(3.0))/(4*sqrt(2.0)),
			   (3+sqrt(3.0))/(4*sqrt(2.0)),
			   (3-sqrt(3.0))/(4*sqrt(2.0)),
			   (1-sqrt(3.0))/(4*sqrt(2.0))};

// N=6
const double K6 = sqrt(5+2*sqrt(10.0));
const double daub_g6[6] = {(1+sqrt(10.0)+K6)/(16*sqrt(2.0)),
			   (5+sqrt(10.0)+3*K6)/(16*sqrt(2.0)),
			   (5-sqrt(10.0)+K6)/(8*sqrt(2.0)),
			   (5-sqrt(10.0)-K6)/(8*sqrt(2.0)),
			   (5+sqrt(10.0)-3*K6)/(16*sqrt(2.0)),
			   (1+sqrt(10.0)-K6)/(16*sqrt(2.0))};

// N=8
const double daub_g8[8] = {+0.325803428051/sqrt(2.0),
			   +1.010945715092/sqrt(2.0),
			   +0.892200138246/sqrt(2.0),
			   -0.039575026236/sqrt(2.0),
			   -0.264507167369/sqrt(2.0),
			   +0.043616300475/sqrt(2.0),
			   +0.046503601071/sqrt(2.0),
			   -0.014986989330/sqrt(2.0)};

// N=10
const double daub_g10[10] = {+0.226418982583/sqrt(2.0),
			     +0.853943542705/sqrt(2.0),
			     +1.024326944260/sqrt(2.0),
			     +0.195766961347/sqrt(2.0),
			     -0.342656715382/sqrt(2.0),
			     -0.045601131884/sqrt(2.0),
			     +0.109702658642/sqrt(2.0),
			     -0.008826800109/sqrt(2.0),
			     -0.017791870102/sqrt(2.0),
			     +0.004717427938/sqrt(2.0)};

// N=12
const double daub_g12[12] = {+0.157742432003/sqrt(2.0),
			     +0.699503814075/sqrt(2.0),
			     +1.062263759882/sqrt(2.0),
			     +0.445831322930/sqrt(2.0),
			     -0.319986598891/sqrt(2.0),
			     -0.183518064060/sqrt(2.0),
			     +0.137888092974/sqrt(2.0),
			     +0.038923209708/sqrt(2.0),
			     -0.044663748331/sqrt(2.0),
			     +0.000783251152/sqrt(2.0),
			     +0.006756062363/sqrt(2.0),
			     -0.001523533805/sqrt(2.0)};

// N=14
const double daub_g14[14] = {+0.110099430746/sqrt(2.0),
			     +0.560791283626/sqrt(2.0),
			     +1.031148491636/sqrt(2.0),
			     +0.664372482211/sqrt(2.0),
			     -0.203513822463/sqrt(2.0),
			     -0.316835011281/sqrt(2.0),
			     +0.100846465010/sqrt(2.0),
			     +0.114003445160/sqrt(2.0),
			     -0.053782452590/sqrt(2.0),
			     -0.023439941565/sqrt(2.0),
			     +0.017749792379/sqrt(2.0),
			     +0.000607514996/sqrt(2.0),
			     -0.002547904718/sqrt(2.0),
			     +0.000500226853/sqrt(2.0)};

// N=16
const double daub_g16[16] = {+0.076955622108/sqrt(2.0),
			     +0.442467247152/sqrt(2.0),
			     +0.955486150427/sqrt(2.0),
			     +0.827816532422/sqrt(2.0),
			     -0.022385735333/sqrt(2.0),
			     -0.401658632782/sqrt(2.0),
			     +0.000668194093/sqrt(2.0),
			     +0.182076356847/sqrt(2.0),
			     -0.024563901046/sqrt(2.0),
			     -0.062350206651/sqrt(2.0),
			     +0.019772159296/sqrt(2.0),
			     +0.012368844819/sqrt(2.0),
			     -0.006887719256/sqrt(2.0),
			     -0.000554004548/sqrt(2.0),
			     +0.000955229711/sqrt(2.0),
			     -0.000166137261/sqrt(2.0)};


// N=18
const double daub_g18[18] = {+0.053850349589/sqrt(2.0),
			     +0.344834303815/sqrt(2.0),
			     +0.855349064359/sqrt(2.0),
			     +0.929545714366/sqrt(2.0),
			     +0.188369549506/sqrt(2.0),
			     -0.414751761802/sqrt(2.0),
			     -0.136953549025/sqrt(2.0),
			     +0.210068342279/sqrt(2.0),
			     +0.043452675461/sqrt(2.0),
			     -0.095647264120/sqrt(2.0),
			     +0.000354892813/sqrt(2.0),
			     +0.031624165853/sqrt(2.0),
			     -0.006679620227/sqrt(2.0),
			     -0.006054960574/sqrt(2.0),
			     +0.002612967280/sqrt(2.0),
			     +0.000325814672/sqrt(2.0),
			     -0.000356329759/sqrt(2.0),
			     +0.000055645514/sqrt(2.0)};

// N=20
const double daub_g20[20] = {+0.037717157593/sqrt(2.0),
			     +0.266122182794/sqrt(2.0),
			     +0.745575071487/sqrt(2.0),
			     +0.973628110734/sqrt(2.0),
			     +0.397637741770/sqrt(2.0),
			     -0.353336201794/sqrt(2.0),
			     -0.277109878720/sqrt(2.0),
			     +0.180127448534/sqrt(2.0),
			     +0.131602987102/sqrt(2.0),
			     -0.100966571196/sqrt(2.0),
			     -0.041659248088/sqrt(2.0),
			     +0.046969814097/sqrt(2.0),
			     +0.005100436968/sqrt(2.0),
			     -0.015179002335/sqrt(2.0),
			     +0.001973325365/sqrt(2.0),
			     +0.002817686590/sqrt(2.0),
			     -0.000969947840/sqrt(2.0),
			     -0.000164709006/sqrt(2.0),
			     +0.000132354366/sqrt(2.0),
			     -0.000018758416/sqrt(2.0)};

// Filter Bank hack for now

// Filter Bank Coefficients
// Nayebi-Barnwell-Smith, 8-tap, 1 sample delay filters
const double nbs_g_d1[8] = {+3.89565e-1/sqrt(2.0),
                            +6.32632e-1/sqrt(2.0),
                            +1.40179e-1/sqrt(2.0),
                            -2.12770e-1/sqrt(2.0),
                            +4.41944e-3/sqrt(2.0),
                            +8.06863e-2/sqrt(2.0),
                            -2.10077e-2/sqrt(2.0),
                            -9.78011e-3/sqrt(2.0)};

const double nbs_h_d1[8] = {+4.15186e-1*sqrt(2.0),
                            -6.09244e-1*sqrt(2.0),
                            +1.06619e-1*sqrt(2.0),
                            +1.65608e-1*sqrt(2.0),
                            -4.31897e-3*sqrt(2.0),
                            -3.19333e-2*sqrt(2.0),
                            +7.99625e-3*sqrt(2.0),
                            +3.76170e-3*sqrt(2.0)};

const double *waveletCoefTable[NUM_WAVELET_TYPES] = {daub_g2,
						     daub_g4,
						     daub_g6,
						     daub_g8,
						     daub_g10,
						     daub_g12,
						     daub_g14,
						     daub_g16,
						     daub_g18,
						     daub_g20,
                                                     nbs_g_d1,
                                                     nbs_h_d1};


const unsigned numWaveletCoefTable[NUM_WAVELET_TYPES] = {2,
							 4,
							 6,
							 8,
							 10,
							 12,
							 14,
							 16,
							 18,
							 20,
                                                         8,
                                                         8};

char *waveletNames[NUM_WAVELET_TYPES] = {"Daubechies 2 (Haar)",
					 "Daubechies 4",
					 "Daubechies 6",
					 "Daubechies 8",
					 "Daubechies 10",
					 "Daubechies 12",
					 "Daubechies 14",
					 "Daubechies 16",
					 "Daubechies 18",
					 "Daubechies 20",
                                         "NBS Filterbank G8, delay 1",
                                         "NBS Filterbank H8, delay 1"};

WaveletCoefficients::WaveletCoefficients(const WaveletType wt)
{
  DEBUG_PRINT("WaveletCoefficients::WaveletCoefficients(const WaveletType wt)");

  assert(wt<=NUM_WAVELET_TYPES);
  this->wt = wt;
  numcoefs = numWaveletCoefTable[wt];
  waveletname = waveletNames[wt];

  g_coefs = new double[numcoefs];
  h_coefs = new double[numcoefs];

  unsigned i;

  if (wt==FILTERBANK_NBS_G8_D1 || wt==FILTERBANK_NBS_H8_D1) {
    const double *coefptr_g=waveletCoefTable[FILTERBANK_NBS_G8_D1];
    const double *coefptr_h=waveletCoefTable[FILTERBANK_NBS_H8_D1];
    for (i=0; i<numcoefs; ++i) {
      g_coefs[i] = coefptr_g[i];
      h_coefs[i] = coefptr_h[i];
    }    
  } else {
    const double *coefptr = waveletCoefTable[wt];
    for (i=0; i<numcoefs; ++i) {
      g_coefs[i] = coefptr[i];
      h_coefs[i] = coefptr[numcoefs-1-i]*pow(-1.0,(double)(i+1));
    }
  }

  DEBUG_PRINT("  wt= " << wt << endl
	      << "  waveletname= " << waveletname << endl
	      << "  numcoefs= " << numcoefs << endl
	      << "  &g_coefs= " << g_coefs << endl
	      << "  &h_coefs= " << h_coefs << endl);
}

WaveletCoefficients::WaveletCoefficients(const WaveletCoefficients &rhs)
{
  DEBUG_PRINT("WaveletCoefficients::WaveletCoefficients"
	      <<"(const WaveletCoefficients &rhs)");

  wt = rhs.wt;
  waveletname = rhs.waveletname;
  numcoefs = rhs.numcoefs;

  g_coefs = new double[numcoefs];
  h_coefs = new double[numcoefs];

  for (unsigned i=0; i<numcoefs; ++i) {
    const double *coefptr = waveletCoefTable[wt];
    g_coefs[i] = coefptr[i];
    h_coefs[i] = coefptr[numcoefs-1-i]*pow(-1.0,(double)(i+1));
  }
}


WaveletCoefficients::~WaveletCoefficients()
{
  DEBUG_PRINT("WaveletCoefficients::~WaveletCoefficients()"<<endl
	      << "  &g_coefs= " << g_coefs << endl
	      << "  &h_coefs= " << h_coefs << endl);

  CHK_DEL(g_coefs);
  CHK_DEL(h_coefs);
}

WaveletCoefficients & WaveletCoefficients::operator=
(const WaveletCoefficients &rhs)
{
  DEBUG_PRINT("WaveletCoefficients::operator=(const WaveletCoefficients &rhs)");

  wt = rhs.wt;
  CHK_DEL(g_coefs);
  CHK_DEL(h_coefs);
  init(wt);
  return *this;
}

void WaveletCoefficients::Initialize(const WaveletType wt)
{
  assert(wt<=NUM_WAVELET_TYPES);
  this->wt = wt;
  CHK_DEL(g_coefs);
  CHK_DEL(h_coefs);
  init(wt);
}

void WaveletCoefficients::ChangeType(const WaveletType wt)
{
  assert(wt<=NUM_WAVELET_TYPES);
  this->wt = wt;
  CHK_DEL(g_coefs);
  CHK_DEL(h_coefs);
  init(wt);
}

string WaveletCoefficients::GetWaveletName() const
{
  return waveletname;
}

unsigned WaveletCoefficients::GetNumCoefs() const
{
  return numcoefs;
}

void WaveletCoefficients::GetTransformCoefsLPF(vector<double> &coefs) const
{
  assert(coefs.size()==0);
  unsigned i;
  if (wt==FILTERBANK_NBS_G8_D1 || wt==FILTERBANK_NBS_H8_D1) {
    for (i=0; i<numcoefs; i++) {
      coefs.push_back(g_coefs[i]);
    }
  } else {
    for (i=0; i<numcoefs; i++) {
      coefs.push_back(g_coefs[numcoefs-1-i]/sqrt(2.0));
    }
  }
}

void WaveletCoefficients::GetTransformCoefsHPF(vector<double> &coefs) const
{
  assert(coefs.size()==0);
  unsigned i;
  if (wt==FILTERBANK_NBS_G8_D1 || wt==FILTERBANK_NBS_H8_D1) {
    for (i=0; i<numcoefs; i++) {
      coefs.push_back(h_coefs[i]);
    }
  } else {
    for (i=0; i<numcoefs; i++) {
      coefs.push_back(h_coefs[numcoefs-1-i]/sqrt(2.0));
    }
  }
}


void WaveletCoefficients::GetInverseCoefsLPF(vector<double> &coefs) const
{
  assert(coefs.size()==0);
  unsigned i;
  if (wt==FILTERBANK_NBS_G8_D1 || wt==FILTERBANK_NBS_H8_D1) {    
    for (i=0; i<numcoefs; i++) {
      coefs.push_back(h_coefs[i]*pow(-1.0,i));
    }
  } else {
    for (i=0; i<numcoefs; i++) {
      coefs.push_back(g_coefs[i]*sqrt(2.0));
    }
  }
}

void WaveletCoefficients::GetInverseCoefsHPF(vector<double> &coefs) const
{
  assert(coefs.size()==0);
  unsigned i;
  if (wt==FILTERBANK_NBS_G8_D1 || wt==FILTERBANK_NBS_H8_D1) {
    for (i=0; i<numcoefs; i++) {
      coefs.push_back(-g_coefs[i]*pow(-1.0,i));
    }
  } else {
    for (i=0; i<numcoefs; i++) {
      coefs.push_back(h_coefs[i]*sqrt(2.0));
    }
  }
}

ostream & WaveletCoefficients::Print(ostream &os) const
{
  os << "Daubechies, " << numcoefs << " coefficients" << endl;
  os << "LPF Coefficients,\tHPF Coefficients" << endl;
  for (unsigned i=0; i<numcoefs; i++) {
    os << "  " << g_coefs[i] << ",\t\t" << h_coefs[i] << endl;
  }
  return os;
}

ostream & WaveletCoefficients::operator<<(ostream &os) const
{
  return Print(os);
}



// Private functions
void WaveletCoefficients::init(const WaveletType wt)
{
  numcoefs = numWaveletCoefTable[wt];
  waveletname = waveletNames[wt];

  g_coefs = new double[numcoefs];
  h_coefs = new double[numcoefs];

  for (unsigned i=0; i<numcoefs; ++i) {
    const double *coefptr = waveletCoefTable[wt];
    g_coefs[i] = coefptr[i];
    h_coefs[i] = coefptr[numcoefs-1-i]*pow(-1.0,(double)(i+1));
  }
}

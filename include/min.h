#ifndef _MIN
#define _MIN

#define F(i) ((i)-1)
#define F2(i,j,n) (((j)-1)*(n)+((i)-1))



template <class func>
class FuncIn1D {

};

template <class func>
class LinMin {
};

// double func::Value(double *param, int nump) is expected
template <class func> 
class PowellMinimizer {
public:
  virtual static double Minimize(double *p,
				 int n, 
				 double ftol,
				 int maxiters) {
    int iter;
    double fret, *xi;
    int i. ibig, j;
    double del, fp, fptt, t, *pt, *ptt, *xit;

    xi=new double [n*n];
    pt=new double [n];
    ptt=new double [n];
    xit=new double [n];
    
    // Initialize xit to the unit vectors
    for (i=1;i<=n;i++) {
      for (j=1;j<=n;j++) {
	xi[F2(i,j,n)]=0.0;
      }
    }
    for (i=1;i<=n;i++) {
      xi[F2(i,i,n)] = 1.0;
    }

    fret=func::Value(p,n);

    for (j=1;j<=n;j++) {
      pt[F(j)] = p[F(j)];
    }

    iter=0;
    while (1) {
      iter++;
      fp=fret;
      ibig=0;
      del=0.0;
      for (i=1;i<=n;i++) {
	for (j=1;j<=n;j++) {
	  xit[F(j)] = xi[F2(j,i,n)];
	}
	fptt=fret;
	linmin(p,xit,n,fret);  // minimize along direction
	if (fabs(fptt-fret) > del) {
	  del=fabs(fptt-fret);
	  ibig=i;
	}
      }
      if (2.0*fabs(fp-fret) <= ftol*(fabs(fp)+fabs(fret))) {
	return fret;
      }
      if (iter==maxiters) {
	return fret;
      }
      for (j=1;j<=n;j++) {
	ptt[F(j)] = 2.0*p[F(j)]-pt[F(j)];
	xit[F(j)] = p[F(j)] - pt[F(j)];
	pt[F(j)] = p[F(j)];
      }
      fptt = func::Value(ptt,n);
      if (fptt >= fp) {
	continue;
      }
      t=2.0*(fp-2.0*fret+fptt)*SQUARED(fp-fret-del)-del*SQUARED(fp-fptt);
      if (t>=0.0) {
	continue;
      }
      linmin(p,xit,n,fret);
      for (j=1;j<=n;j++) {
	xi[F2(j,ibig,n)] = xi[F2(j,n)];
	xi[F2(j,n)] = xit[F(j)];
      }
    }
  }
};



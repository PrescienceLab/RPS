#! /bin/sh
# This is a shell archive.  Remove anything before this line, then feed it
# into a shell via "sh file" or similar.  To overwrite existing files,
# type "sh file -c".
# The tool that generated this appeared in the comp.sources.unix newsgroup;
# send mail to comp-sources-unix@uunet.uu.net if you want that tool.
# Contents:  README Makefile fracdiff.DOC example.f fdcore.f fdsim.f
#   fdout.f fdhess.f fdmach.f fdmin.f fdlin.f fdgam.f fdran.f
# Wrapped by fraley@hose on Tue Apr  5 10:31:45 1994
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
echo If this archive is complete, you will see the following message:
echo '          "shar: End of archive."'
if test -f 'README' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'README'\"
else
  echo shar: Extracting \"'README'\" \(1679 characters\)
  sed "s/^X//" >'README' <<'END_OF_FILE'
Xfracdf  Maximum likelihood estimation of the parameters of a fractionally
X        differenced ARIMA (p,d,q) model. For long-memory dependence in
X        time series. (Haslett and Raftery, Applied Statistics 38, 1989, 1-50.)
X
XThe submission consists of the following files :
X
Xfracdiff.DOC    documentation from top-level subroutines fracdf and fdgen
Xfdcore.f        subroutines implementing the Haslett and Raftery algorithm
Xfdsim.f         simulates data for use with fracdiff
Xfdout.f         output-handling subroutines 
Xfdhess.f        subroutines for computing Hessian and variance information
Xfdmach.f        auxiliary subroutines for machine constants
Xfdmin.f         auxiliary subroutines from MINPACK
Xfdgam.f         auxiliary subroutines for the gamma function
Xfdlin.f         auxiliary subroutines from LINPACK
Xfdran.f         auxiliary subroutines from RANLIB
Xexample.f       sample main program using test problem generator
XMakefile        UNIX Makefile for running the example
X
XThe relevant documentation is contained in the file fracdiff.DOC, which is a 
Xcopy of the comments for the top-level subroutines fracdf (in fdcore.f),
Xfdsim and fdvar (in fdhess.f). Subroutine fdvar is included to allow users to
Xrecompute the variance estimate with a different finite-difference interval
Xfor derivatives with respect to the fractional-differencing parameter.
X
XStandard output unit number is assumed to be 6.
X
Xcopyright 1991 Department of Statistics, Univeristy of Washington
Xfunded by ONR contracts N-00014-88-K-0265 and N-00014-91-J-1074
X
XPermission granted for unlimited redistribution for non-commercial use.
X
XPlease report all anomalies to fraley@stat.washington.edu.
END_OF_FILE
  if test 1679 -ne `wc -c <'README'`; then
    echo shar: \"'README'\" unpacked with wrong size!
  fi
  # end of 'README'
fi
if test -f 'Makefile' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'Makefile'\"
else
  echo shar: Extracting \"'Makefile'\" \(157 characters\)
  sed "s/^X//" >'Makefile' <<'END_OF_FILE'
XFFLAGS  = -O
XOFILES  = example.o fdcore.o fdsim.o fdhess.o fdout.o\
X        fdmach.o fdmin.o fdlin.o fdgam.o fdran.o
X
Xa.out : $(OFILES) 
X       f77 $(OFILES) 
X
X
X
X
END_OF_FILE
  if test 157 -ne `wc -c <'Makefile'`; then
    echo shar: \"'Makefile'\" unpacked with wrong size!
  fi
  # end of 'Makefile'
fi
if test -f 'fracdiff.DOC' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fracdiff.DOC'\"
else
  echo shar: Extracting \"'fracdiff.DOC'\" \(6391 characters\)
  sed "s/^X//" >'fracdiff.DOC' <<'END_OF_FILE'
X
X*******************************************************************************
X*******************************************************************************
X
X      subroutine fracdf( x, n, M, nar, nma, dtol, drange, h, hood, d,
X     *                   ar, ma, cov, lcov, stderr, cor, lcor,
X     *                   hess, lhess, w, lenw, iprint)
X
X      implicit double precision (a-h,o-z)
X
X      integer            n, M, nar, nma, lcov, lcor, lenw
X      real               x(n)
X      double precision   drange(2), ar(nar), ma(nma), stderr(nar+nma+1)
X      double precision   cov(lcov,nar+nma+1), cor(lcor,nar+nma+1)
X      double precision   d, dtol, h, hood
X      double precision   w(lenw)
X      integer            iprint
X
Xc------------------------------------------------------------------------------
Xc
Xc       Calculates the maximum likelihood estimators of the parameters
Xc       of a fractionally-differenced ARIMA (p,d,q) model, together
Xc       with their estimated covariance and correlation matrices and
Xc       standard errors, as well as the value of the maximized likelihood.
Xc       The fast approximate algorithm of Haslett and Raftery (1989) is used.
Xc
Xc       The optimization is carried out in two levels : an outer univariate 
Xc       unimodal optimization with respect to d over the interval [0,.5] 
Xc       (uses Brent's fmin algorithm), and an inner nonlinear least-squares 
Xc       optimization with respect to the AR and MA parameters to minimize 
Xc       white noise variance (uses the MINPACK subroutine LMDER).
Xc
Xc REFERENCES : 
Xc
Xc    J. Haslett and A. E. Raftery,
Xc        Space-time modeling with  Long-Memory Dependence : 
Xc           Assessing Ireland's Wind Power Resource,
Xc             Applied Statistics 38 (1989) 1-50.
Xc
Xc    R. Brent, Algorithms for Minimization without Derivatives,
Xc        Prentice-Hall (1973).
Xc
Xc    J. J. More, B. S. Garbow, and K. E. Hillstrom,
Xc        User's Guide for MINPACK-1, Technical Report ANL-80-74,
Xc        Applied Mathematics Division, Argonne National Laboratory,
Xc       (August 1980).
Xc
Xc   Input :
Xc
Xc  x       real     time series for the ARIMA model
Xc  n       integer  length of the time series
Xc  M       integer  number of terms in the likelihood approximation
Xc                   suggested value 100 (see Haslett and Raftery 1989)
Xc  nar     integer  number of autoregressive parameters
Xc  nma     integer  number of moving average parameters
Xc  lcov    integer  row dimension of an array for the covariance matrix
Xc                   must be as declared in the callng program 
Xc                  (at least nar+nma+1)
Xc  lcor    integer  row dimension of an array for the correlation matrix
Xc                   must be as declared in the callng program 
Xc                  (at least nar+nma+1)
Xc  dtol    double   desired length of final interval of uncertainty for d
Xc                   suggested value : 4th root of machine precision
Xc                   if dtol < 0 it is automatically set to this value
Xc                   dtol will be altered if necessary by the program
Xc  drange  double   array of length 2 giving mimimum and maximum values
Xc                   for the fractional differencing parameter
Xc  h       double   finite-difference interval for estimating derivatives
Xc                   with respect to d. If h < 0 it is automatically reset.
Xc  d       double   initial guess for optimal fractional differencing parameter
Xc  ar      double   initial autoregressive parameter estimates
Xc  ma      double   initial moving average parameter estimates       
Xc  w       double   work array
Xc  lenw    integer  length of double precision workspace w, must be at least 
Xc max(nar+nma+2*(n+M),3*n+(n+6.5)*(nar+nma)+1,(3+2*(nar+nma+1))*(nar+nma+1)+1)
Xc  iprint  integer  <= 0 no printing 
Xc                   >  0 intermediate results printed
Xc
Xc  Output :
Xc 
Xc  dtol    double   value of dtol ultimately used by the algorithm
Xc  h       double   value of h ultimately used by the algorithm
Xc  d       double   optimal fractional differencing parameter
Xc  hood    double   logarithm of the maximum likelihood
Xc  ar      double   optimal autoregressive parameter estimates
Xc  ma      double   optimal moving average parameter estimates       
Xc  cov     double   covariance matrix of the parameter estimates (d,p,q)
Xc  stderr  double   standard errors of the parameter estimates (d,p,q)
Xc  cor     double   correlation matrix of the parameter estimates (d,p,q)
Xc
Xc------------------------------------------------------------------------------
X
X      subroutine fdsim( n, ip, iq, ar, ma, d, rmu, iseed, y, x)
Xc  generates a random time series for use with fracdf
Xc
Xc  Input :
Xc
Xc  n      integer  length of the time series
Xc  ip     integer  number of autoregressive parameters
Xc  ar     real    (ip) autoregressive parameters
Xc  ma     real    (iq) moving average parameters
Xc  d      real     fractional differencing parameters
Xc  rmu    real     time series mean
Xc  y      real    (n+iq) workspace
Xc  s      real    (n+iq) workspace
Xc
Xc  Output :
Xc
Xc  x      real    (n) the generated time series
X
Xc------------------------------------------------------------------------------
Xc
Xc        Simulates a series of length n from an ARIMA (p,d,q) model
Xc        with fractional d (0<d<0.5). 
Xc
Xc------------------------------------------------------------------------------
X
X      subroutine fdvar(  x, n, M, nar, nma, dtol, drange, h, hood, d, 
X     *                   ar, ma, cov, lcov, stderr, cor, lcor, 
X     *                   hess, lhess, w, lenw)
X
X      implicit double precision (a-h,o-z)
X
X      integer            n, M, nar, nma, lcov, lcor, lenw
Xc     real               x(n)
X      real               x(*)
Xc     double precision   drange(2), ar(nar), ma(nma), stderr(nar+nma+1) 
X      double precision   drange(2), ar(*), ma(*), stderr(*)
Xc     double precision   cov(lcov,nar+nma+1), cor(lcor,nar+nma+1)
X      double precision   cov(lcov,*), cor(lcor,*), hess(lhess,*)
X      double precision   d, dtol, h, hood
Xc     double precision   w(lenw)
X      double precision   w(*)
X
Xc------------------------------------------------------------------------------
Xc
Xc       Recomputes the covariance, correlation and standard error given
Xc       a finite-dfference interval h for derivatives with respect to d.
Xc
Xc------------------------------------------------------------------------------
END_OF_FILE
  if test 6391 -ne `wc -c <'fracdiff.DOC'`; then
    echo shar: \"'fracdiff.DOC'\" unpacked with wrong size!
  fi
  # end of 'fracdiff.DOC'
fi
if test -f 'example.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'example.f'\"
else
  echo shar: Extracting \"'example.f'\" \(1529 characters\)
  sed "s/^X//" >'example.f' <<'END_OF_FILE'
Xc note : the workspace limit is set to 30000 but may be modified
Xc standard output unit number is assumed to be 6
X
X      implicit double precision (a-h,o-z)
X
X      integer           lenw
X      data              lenw /30000/
X      double precision  w(30000)
X
X      real              x(10010)
X      
X      real              p(5), q(5)
X      double precision  ar(5), ma(5), drange(2)
X      double precision  cov(6,6), se(6), cor(6,6), hess(6,6)
X
X      integer           np, nq, n, M
X
X      double precision  d, hood, dtol
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley (3/91)
X
X
Xc-----------------------------------------------------------------------------
X
X      open( unit=6, file="fracdiff.out")
X
X      np = 1
X      nq = 1
X
X      n = 5000
X
X      p(1) =  .2
X      q(1) = -.4
X
X      call fdsim( n, np, nq, p, q, .3, 0., w, x)
X
X      M     = 100
X
X      dtol = -1.d0
X
X      do i = 1, np
X        ar(i) = 0.d0
X      end do
X
X      d     = 0.d0
X
X      do i = 1, nq
X        ma(i) = 0.d0
X      end do
X
X      drange(1) = 0.d0
X      drange(2) = .5d0
X
X      h = -1.d0
X
X      call fracdf( x, n, M, np, nq, dtol, drange, h, hood, d, ar, ma, 
X     *             cov, 6, se, cor, 6, hess, 6, w, lenw, 1)
X
Xc recompute covariance, etc, using a different finite-difference interval
X
X      h = .0001d0
X
X      call fdvar( x, n, M, np, nq, dtol, drange, h, hood, d, ar, ma, 
X     *             cov, 6, se, cor, 6, hess, 6, w, lenw)
X
X      stop
X 900  write(6,*) '+++ input error'
X      stop
X      end
X
X
X
X
END_OF_FILE
  if test 1529 -ne `wc -c <'example.f'`; then
    echo shar: \"'example.f'\" unpacked with wrong size!
  fi
  # end of 'example.f'
fi
if test -f 'fdcore.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdcore.f'\"
else
  echo shar: Extracting \"'fdcore.f'\" \(32133 characters\)
  sed "s/^X//" >'fdcore.f' <<'END_OF_FILE'
X      subroutine fracdf( x, n, M, nar, nma, dtol, drange, h, hood, d, 
X     *                   ar, ma, cov, lcov, stderr, cor, lcor, 
X     *                   hess, lhess, w, lenw, iprint)
X
X      implicit double precision (a-h,o-z)
X
X      integer            n, M, nar, nma, lcov, lcor, lenw
Xc     real               x(n)
X      real               x(*)
Xc     double precision   drange(2), ar(nar), ma(nma), stderr(nar+nma+1) 
X      double precision   drange(2), ar(*), ma(*), stderr(*)
Xc     double precision   cov(lcov,nar+nma+1), cor(lcor,nar+nma+1)
X      double precision   cov(lcov,*), cor(lcor,*), hess(lhess,*)
X      double precision   d, dtol, h, hood
Xc     double precision   w(lenw)
X      double precision   w(*)
X      integer            iprint
X
Xc------------------------------------------------------------------------------
Xc
Xc       Calculates the maximum likelihood estimators of the parameters
Xc       of a fractionally-differenced ARIMA (p,d,q) model, together
Xc       with their estimated covariance and correlation matrices and
Xc       standard errors, as well as the value of the maximized likelihood.
Xc       The fast approximate algorithm of Haslett and Raftery (1989) is used.
Xc
Xc       The optimization is carried out in two levels : an outer univariate 
Xc       unimodal optimization with respect to d over the interval [0,.5] 
Xc       (uses Brent's fmin algorithm), and an inner nonlinear least-squares 
Xc       optimization with respect to the AR and MA parameters to minimize 
Xc       white noise variance (uses the MINPACK subroutine LMDER).
Xc
Xc REFERENCES : 
Xc
Xc    J. Haslett and A. E. Raftery,
Xc        Space-time modeling with  Long-Memory Dependence : 
Xc           Assessing Ireland's Wind Power Resource,
Xc             Applied Statistics 38 (1989) 1-50.
Xc
Xc    R. Brent, Algorithms for Minimization without Derivatives,
Xc        Prentice-Hall (1973).
Xc
Xc    J. J. More, B. S. Garbow, and K. E. Hillstrom,
Xc        User's Guide for MINPACK-1, Technical Report ANL-80-74,
Xc        Applied Mathematics Division, Argonne National Laboratory,
Xc       (August 1980).
Xc
Xc   Input :
Xc
Xc  x       real     time series for the ARIMA model
Xc  n       integer  length of the time series
Xc  M       integer  number of terms in the likelihood approximation
Xc                   suggested value 100 (see Haslett and Raftery 1989)
Xc  nar     integer  number of autoregressive parameters
Xc  nma     integer  number of moving average parameters
Xc  lcov    integer  row dimension of an array for the covariance matrix
Xc                   must be as declared in the callng program 
Xc                  (at least nar+nma+1)
Xc  lcor    integer  row dimension of an array for the correlation matrix
Xc                   must be as declared in the callng program 
Xc                  (at least nar+nma+1)
Xc  dtol    double   desired length of final interval of uncertainty for 
Xc                   suggested value : 4th root of machine precision
Xc                   if dtol < 0 it is automatically set to this value
Xc                   dtol will be altered if necessary by the program
Xc  drange  double   array of length 2 giving mimimum and maximum values 
Xc                   for the fractional differencing parameter
Xc  h       double   finite-difference interval for estimating derivatives
Xc                   with respect to d. If h < 0 it is automatically reset.
Xc  d       double   initial guess for optimal fractional differencing parameter
Xc  ar      double   initial autoregressive parameter estimates
Xc  ma      double   initial moving average parameter estimates       
Xc  w       double   work array
Xc  lenw    integer  length of double precision workspace w, must be at least 
Xc max(nar+nma+2*(n+M),3*n+(n+6.5)*(nar+nma)+1,(3+2*(nar+nma+1))*(nar+nma+1)+1)
Xc  iprint  integer  <= 0 no printing 
Xc                   >  0 intermediate results printed
Xc
Xc  Output :
Xc 
Xc  dtol    double   value of dtol ultimately used by the algorithm
Xc  h       double   value of h ultimately used by the algorithm
Xc  d       double   optimal fractional differencing parameter
Xc  hood    double   logarithm of the maximum likelihood
Xc  ar      double   optimal autoregressive parameter estimates
Xc  ma      double   optimal moving average parameter estimates       
Xc  cov     double   covariance matrix of the parameter estimates (d,p,q)
Xc  stderr  double   standard errors of the parameter estimates (d,p,q)
Xc  cor     double   correlation matrix of the parameter estimates (d,p,q)
Xc  hess    double   hessian matrix of the parameter estimates (d,p,q)
Xc
Xc------------------------------------------------------------------------------
X
X      double precision   dlamch, dopt
X
X      double precision   BASE
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      save   /MAUXFD/
X
X      integer            nn, MM, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    nn, MM, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      integer            maxopt, maxfun, nopt, nfun, ngrd,
X     *                   ifun, igrd, info
X      common /CNTRFD/    maxopt, maxfun, nopt, nfun, ngrd,
X     *                   ifun, igrd, info
X      save   /CNTRFD/    
X
X      double precision   told, tolf, tolx, tolg, anorm, deltax, gnorm
X      common /TOLSFD/    told, tolf, tolx, tolg, anorm, deltax, gnorm
X      save   /TOLSFD/
X
X      integer            lenthw, lwfree
X      common /WORKFD/    lenthw, lwfree
X      save   /WORKFD/
X
X      integer            ly, lamk, lak, lvk, lphi, lpi
X      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
X      save   /WFILFD/
X
X      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf, 
X     *                   lwa1, lwa2, lwa3, lwa4
X      save   /WOPTFD/
X
X      integer            ILIMIT, JLIMIT
X      common /LIMSFD/    ILIMIT, JLIMIT
X      save   /LIMSFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
X      integer            IMINPK, JMINPK
X      common /MNPKFD/    IMINPK, JMINPK
X      save   /MNPKFD/
X
X      integer            KSVD, KCOV, KCOR
X      common /HESSFD/    KSVD, KCOV, KCOR
X      save   /HESSFD/
X
X      double precision   zero, one
X      parameter         (zero=0.d0, one=1.d0)
X
X      data               ncalls /0/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc-----------------------------------------------------------------------------
X
X      if (ncalls .eq. 0) then
X
Xc machine constants
X
X        FLTMIN = dlamch( 'S' )
XC       FLTMAX = dlamch( 'O' )  not guaranteed to work
X        FLTMAX = one/FLTMIN
X
X        BASE   = dlamch( 'B' )
X        EPSMIN = dlamch( 'E' )
X        EPSMAX = EPSMIN*BASE
X        EPSPT5 = sqrt(EPSMIN)
X        EPSP25 = sqrt(EPSPT5)
X        EPSPT3 = EPSMIN**(.3)
X        EPSP75 = EPSMIN**(.75)
X        BIGNUM = one / EPSMIN
X        ncalls = 1
X      endif
X
Xc set error and warning flags
X      ILIMIT = 0
X      JLIMIT = 0
X
X      IGAMMA = 0
X      JGAMMA = 0
X
X      IMINPK = 0
X      JMINPK = 0
X
X      KSVD   = 0
X      KCOV   = 0
X      KCOR   = 0
X
Xc useful quantities
X
X      if (M .le. 0) M = 100
X
X      nn    = n
X      MM    = M
X      np    = nar
X      nq    = nma
X
X      npq    = np + nq
X      npq1   = npq + 1
X      maxpq  = max(np,nq)
X      minpq  = min(np,nq)
X      maxpq1 = maxpq + 1
X
X      maxopt = 100
X      maxfun = 100
X
X      if (dtol .gt. .1d0)  dtol = .1d0
X
X      if (dtol .le. zero) then
X        told  =  EPSP25
X        tolf  =  EPSPT3
X        tolx  =  told
X        tolg  =  EPSPT3
X      else
X        told  =  max( dtol, EPSPT5)
X        tolf  =  max( dtol/1.d1, EPSP75)
X        tolx  =  told
X        tolg  =  max( dtol/1.d1, EPSP75)
X      end if
X
X      dtol   = told
X
X      nm     = n - maxpq
X 
Xc workspace allocation
X
X      lqp    = 1
X      ly     = lqp    +  npq
X      lamk   = ly
X      lak    = lamk   +  n
X      lphi   = lak    +  n
X      lvk    = lphi   +  M
X      lpi    = lphi
X      la     = ly     +  n
X      lajac  = la     +  n - minpq
X      ipvt   = lajac  +  max( (n-np)*np, (n-nq)*nq, (n-maxpq)*npq)
X      ldiag  = ipvt   +  npq/2 + 1
X      lqtf   = ldiag  +  npq
X      lwa1   = lqtf   +  npq
X      lwa2   = lwa1   +  npq
X      lwa3   = lwa2   +  npq
X      lwa4   = lwa3   +  npq
X      lfree  = lwa4   +  n - minpq
X
X      lwfree = max( (lvk+M), (lwa4+n-minpq), (12*31))
X      lenthw = lenw
X
X      if (lwfree  .gt. (lenw+1)) then
X        ILIMIT = lwfree - lenw
Xc       write( 6, *) 'insufficient storage : ',
Xc    *               'increase length of w by at least', incw
X        return
X      endif
X
Xc     call dcopy( npq, zero, 0, w(lqp), 1)
X
X      if (npq .ne. 0) then
X        call dcopy( np, ar, 1, w(lqp+nq), 1)
X        call dcopy( nq, ma, 1, w(lqp)   , 1)
X      end if
X
X      nopt = 0
X      nfun = 0
X      ngrd = 0
X
X      if (iprint .gt. 0) then
X        write(6,*)
X        write(6,900) 
X      end if
X
X      d = dopt( x, d, drange, hood, delta, w, iprint)
X
X      if (nopt .ge. maxopt) JLIMIT = 1
Xc       write( 6, *)
Xc       write( 6, *) 'WARNING : optimization limit reached'
Xc     end if
X
X      if (IGAMMA .ne. 0 .or. IMINPK .ne. 0) then
X        d    = FLTMAX
X        hood = FLTMAX
X        call dcopy( np, FLTMAX, 0, ar, 1)
X        call dcopy( nq, FLTMAX, 0, ma, 1)
X        call dcopy( npq1, FLTMAX, 0, stderr, 1)
X        do j = 1, npq1
X          call dcopy( npq1, FLTMAX, 0, cov( 1, j), 1)
X          call dcopy( npq1, FLTMAX, 0, cor( 1, j), 1)
X        end do
X        return
X      end if
X     
X      call dcopy( np, w(lqp+nq), 1, ar, 1)
X      call dcopy( nq, w(lqp   ), 1, ma, 1)
X
X      call fdhpq( x, hess, lhess, w)
X
X      if (npq .gt. 0) then
X        do i = 2, npq1
X         call dcopy( npq, hess(2,i), 1, cov(2,i), 1)
X        end do
X    
X        call fdcov( x, d, h, hess, cov, lcov, cor, lcor, stderr, w)
X
X      end if
X
X      call fdout ( d, ar, ma, dtol, drange, h, hood,
X     *             cov, lcov, stderr, cor, lcor)
X
X      return
X 900  format( 4h itr, 14h     d          ,   14h    est mean  , 
X     *                16h     white noise,  17h     log likelihd,
X     *                 4h  nf, 3h ng)
X      end
X
X*******************************************************************************
X*******************************************************************************
X
X      double precision function dopt( x, dinit, drange, hood, delta, w, 
X     *                                iprint)
X
X      implicit double precision (a-h,o-z)
X
X      integer           iprint
X
Xc     real              x(n)
X      real              x(*) 
X
X      double precision  dinit, drange(2), hood, delta
X      double precision  w(*)
Xc
Xc optimization with repsect to d based on Brent's fmin algorithm
Xc
X      double precision  pqopt
X      double precision  dd, ee, hh, rr, ss, tt
X      double precision  uu, vv, ww, fu, fv, fw
X      double precision  eps, tol1, tol2, tol3
X
X      intrinsic         abs, sqrt
X
X      double precision  cc
X      data              cc /.38196601125011d0/
X      
X      double precision  zero, half, one, two, three
X      parameter        (zero=0.d0, half=.5d0, one=1.d0,
X     *                   two=2.d0, three=3.d0)
X
X      integer           maxopt, maxfun, nopt, nfun, ngrd, 
X     *                  ifun, igrd, info
X      common /CNTRFD/   maxopt, maxfun, nopt, nfun, ngrd, 
X     *                  ifun, igrd, info
X      save   /CNTRFD/
X        
X      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X        
X      double precision   aa, xx, bb, fa, fx, fb
X      save               aa, xx, bb, fa, fx, fb
X
X      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      save   /WOPTFD/
X
X      double precision   hatmu, wnv, cllf
X      common /FILTFD/    hatmu, wnv, cllf
X      save   /FILTFD/
X
X      double precision   dtol, ftol, xtol, gtol, anorm, deltax, gnorm
X      common /TOLSFD/    dtol, ftol, xtol, gtol, anorm, deltax, gnorm
X      save   /TOLSFD/
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
X      integer            IMINPK, JMINPK
X      common /MNPKFD/    IMINPK, JMINPK
X      save   /MNPKFD/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc------------------------------------------------------------------------------
Xc
Xc  cc is the squared inverse of the golden ratio (see data statement)
Xc
Xc     cc = half*(three-sqrt(5.0d0))
Xc
Xc  eps is approximately the square root of the relative machine
Xc  precision.
Xc
X      eps  =  EPSMAX
X      tol1 =  one + eps
X      eps  =  sqrt(eps)
Xc
X      aa   =  drange(1)
X      bb   =  drange(2)
X      if (dinit .gt. (aa + dtol) .and. dinit .lt. (bb - dtol)) then
X        vv = dinit
X      else
X        vv = aa + cc*(bb-aa)
X      end if
X      ww   =  vv
X      xx   =  vv
X      uu   =  xx
X      ee   =  zero
X
X      nopt = 1
X
X      fx   =  pqopt( x, xx, w)
X      
X      fv   =  fx
X      fw   =  fx
X 
X      tol  = max(dtol,zero)
X      tol3 = tol/three
Xc
Xc  main loop starts here
Xc
X   10 continue
X
X      if (iprint .gt. 0) write(6,900)  nopt, uu, hatmu, wnv, cllf,
X     *                                 ifun, igrd
X
X      if (IGAMMA .ne. 0 .or. IMINPK .ne. 0) then
X        d    = uu
X        hood = FLTMAX
X        return
X      end if
X
X      hh   =  half*(aa+bb)
X      tol1 =  eps*(one+abs(xx)) + tol3
X      tol2 =  two*tol1
Xc
Xc  check stopping criterion
Xc
X      delta = abs(xx-hh) + half*(bb-aa)
Xc     if (abs(xx-hh) .le. (tol2-half*(bb-aa))) go to 100
X      if (delta .le. tol2) go to 100
X
X      if (nopt .ge. maxopt) go to 100
X
Xc     if (delpq .le. EPSMAX*(one+pqnorm)) go to 100
X
X      rr   =  zero
X      ss   =  zero
X      tt   =  zero
X
X      if (abs(ee) .gt. tol1) then
Xc
Xc  fit parabola
Xc
X        rr   = (xx-ww)*(fx-fv)
X        ss   = (xx-vv)*(fx-fw)
X        tt   = (xx-vv)*ss-(xx-ww)*rr
X        ss   =  two*(ss-rr)
X        if (ss .le. zero) then
X          ss = -ss
X        else             
X          tt = -tt
X        end if
X        rr   =  ee
X        ee   =  dd
X      end if
X     
X      if ((abs(tt) .ge. abs(half*ss*rr)) .or. 
X     *   (tt .le. ss*(aa-xx)) .or. (tt .ge. ss*(bb-xx))) then
Xc
Xc  a golden-section step
Xc
X        if (xx .ge. hh) then
X          ee = aa - xx
X        else           
X          ee = bb - xx
X        end if
X        dd   =  cc*ee
X
X      else 
Xc
Xc  a parabolic-interpolation step
Xc
X        dd   =  tt / ss
X        uu   =  xx + dd
Xc
Xc  f must not be evaluated too close to aa or bb
Xc
X        if (((uu-aa) .lt. tol2) .or. ((bb-uu) .lt. tol2)) then
X          dd  =  tol1
X          if (xx .ge. hh) dd = -dd
X        end if
X      end if
Xc
Xc  f must not be evaluated too close to xx
Xc
X      if (abs(dd) .ge. tol1)  then
X        uu = xx + dd
X      else
X        if (dd .le. zero) then
X          uu = xx - tol1
X        else             
X          uu = xx + tol1
X        end if
X      end if 
X
X      nopt = nopt + 1
X
X      fu   =  pqopt( x, uu, w)
Xc
Xc  update  aa, bb, vv, ww, and xx
Xc
X      if (fx .ge. fu) then
X        if (uu .ge. xx) then
X          aa = xx
X          fa = fx
X        else           
X          bb = xx
X          fb = fx
X        end if
X        vv   =  ww
X        fv   =  fw
X        ww   =  xx
X        fw   =  fx
X        xx   =  uu
X        fx   =  fu
X      else
X        if (uu .ge. xx) then
X          bb = uu
X          fb = fu
X        else
X          aa = uu
X          fa = fu
X        end if
X        if ((fu .gt. fw) .and. (ww .ne. xx)) then
X          if ((fu .le. fv) .or. (vv .eq. xx) .or. (vv .eq. ww)) then
X             vv   =  uu
X             fv   =  fu
X          end if
X        else
X          vv   =  ww
X          fv   =  fw
X          ww   =  uu
X          fw   =  fu
X        end if
X      end if
X      
X      go to 10
Xc
Xc  end of main loop
Xc
X  100 dopt =  xx
X      hood = -fx
X      cllf = hood
X      
X      return
X 900  format( i4, 2(1pe14.6), 1pe16.7, 1pe17.8, 1x, 2(i3))
X      end
X
X***************************************************************************
X***************************************************************************
X
X      double precision function pqopt( x, d, w)
X
X      implicit double precision (a-h,o-z)
X
Xc     real              x(n)
X      real              x(*)
X
X      double precision  d
X      double precision  w(*)
X
X      double precision bic, slogvk
X      double precision t, u
X
X      intrinsic        log
X
X      double precision  ddot
X      external          ajqp
X
X      double precision zero, one
X      parameter       (zero=0.d0, one=1.d0)
X
X      double precision  hatmu, wnv, hood
X      common /FILTFD/   hatmu, wnv, hood
X      save   /FILTFD/   
X
X      double precision dtol, ftol, xtol, gtol, anorm, deltax, gnorm
X      common /TOLSFD/  dtol, ftol, xtol, gtol, anorm, deltax, gnorm
X      save   /TOLSFD/  
X
X      integer          n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/  n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/ 
X
X      integer          maxopt, maxfun, nopt, nfun, ngrd, 
X     *                 ifun, igrd, info
X      common /CNTRFD/  maxopt, maxfun, nopt, nfun, ngrd, 
X     *                 ifun, igrd, info
X      save   /CNTRFD/
X
X      integer           ly, lamk, lak, lvk, lphi, lpi
X      common /WFILFD/   ly, lamk, lak, lvk, lphi, lpi
X      save   /WFILFD/
X     
X      integer           lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                  lwa1, lwa2, lwa3, lwa4
X      common /WOPTFD/   lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                  lwa1, lwa2, lwa3, lwa4
X      save   /WOPTFD/
X
X      integer           modelm
X      double precision  factlm
X      data              modelm/1/, factlm /100.d0/
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc----------------------------------------------------------------------------
X
X        call fdfilt( x, d, w(ly), slogvk, 
X     *               w(lamk), w(lak), w(lvk), w(lphi), w(lpi))
X
X        if (IGAMMA .ne. 0) then
X          pqopt  =  FLTMAX
X          wnv    =  FLTMAX
X          hood   = -FLTMAX
X          return
X        end if 
X
X        t = dble(n)
X
X       if (npq .eq. 0) then
X          wnv   = ddot( n, w(ly), 1, w(ly), 1) / t
X          ifun  =  0
X          igrd  =  0
X          info  = -1
X          goto 100
X        endif
Xc
Xc optimize as an unconstrained optimization problem
Xc
X         if (modelm .eq. 2) call dcopy( npq,  one, 0, w(ldiag), 1)
X
X         if (nopt .lt. 0) then
X           if (np .ne. 0) then
X             call LMDER1( ajp, n-np, np, w(lqp+nq),w(la),w(lajac), n-np,
X     *                    ftol, xtol, gtol, maxfun, w(ldiag), modelm,
X     *                    factlm, info, ifun, igrd, w(ipvt), w(lqtf),
X     *                    w(lwa1), w(lwa2), w(lwa3), w(lwa4), w(ly))
X           end if
X           if (nq .ne. 0) then
X             call LMDER1( ajq, n-nq, nq, w(lqp),w(la),w(lajac), n-nq,
X     *                    ftol, xtol, gtol, maxfun, w(ldiag), modelm,
X     *                    factlm, info, ifun, igrd, w(ipvt), w(lqtf),
X     *                    w(lwa1), w(lwa2), w(lwa3), w(lwa4), w(ly))
X           end if
X         end if
X
X         call LMDER1( ajqp, nm, npq, w(lqp), w(la), w(lajac), nm,
X     *                ftol, xtol, gtol, maxfun, w(ldiag), modelm,
X     *                factlm, info, ifun, igrd, w(ipvt), w(lqtf),
X     *                w(lwa1), w(lwa2), w(lwa3), w(lwa4), w(ly))
X
X        if (info .eq. 0) then
Xc         write( 6, *) 'MINPACK : improper input parameters
X          IMINPK = 10
X          pqopt  =  FLTMAX
X          wnv    =  FLTMAX
X          hood   = -FLTMAX
X          return
X        end if
X
X        if (info .eq. 5) then
Xc         write( 6, *) 'MINPACK : function evaluation limit reached'
X          JMINPK = 5
X        end if
X
X        if (info .eq. 6 ) then
Xc         write( 6, *) 'MINPACK : ftol is too small'
X          JMINPK = 6
X        end if
X
X        if (info .eq. 7) then
Xc         write( 6, *) 'MINPACK : xtol is too small'
X          JMINPK = 7
X        end if
X
X        if (info .eq. 8) then
Xc         write( 6, *) 'MINPACK : gtol is too small'
X          JMINPK = 8
X        end if
X
Xc        call daxpy( npq, (-one), w(lpq), 1, w(lqp), 1
Xc        delpq  = sqrt(ddot( npq, w(lqp), 1, w(lqp), 1))         
Xc        pqnorm = sqrt(ddot( npq, w(lpq), 1, w(lpq), 1))
X
X        wnv   =  (anorm*anorm) / dble(nm-1)
X 100    u     = (t*(2.8378d0+log(wnv))+slogvk)
X        pqopt =  u / 2.d0
X        bic   =  u + dble(np+nq+1)*log(t)
X        hood  = -pqopt
X
X      return
X      end
X
X***************************************************************************
X***************************************************************************
X
X      subroutine fdfilt( x, d, y, slogvk, amk, ak, vk, phi, pi)
X
X      implicit double precision (a-h,o-z)
X
Xc     real              x(n)
X      real              x(*)
X      double precision  d, slogvk
Xc     double precision  y(n), amk(n), ak(n)
X      double precision  y(*), amk(*), ak(*)
Xc     double precision  vk(M), phi(M), pi(M)
X      double precision  vk(*), phi(*), pi(*)
X
Xc**************************************************************************
Xc input  :
Xc          x       real    original time series
Xc          d       double  estimated value of d
Xc output :
Xc          y       double  flitered series
Xc          slogvk  double  the sum of the logarithms of the vk
Xc notes  :
Xc          y can use the same storage as either ak or amk
Xc          phi and pi can use the same storage
Xc          can be arranged so that phi, pi and vk share the same storage
Xc**************************************************************************
X
X      integer           j, k, km, mcap, mcap1
X      double precision  g0, r, s, t, u, v, z
X
X      double precision  zero, one, two
X      parameter        (zero=0.d0, one=1.d0, two=2.d0)
X
X      double precision  dgamma, dgamr
X
X      intrinsic         log, sqrt
X
X      double precision  hatmu, wnv, cllf
X      common /FILTFD/   hatmu, wnv, cllf
X      save   /FILTFD/
X
X      integer           n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/   n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/ 
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X        
Xc-----------------------------------------------------------------------
X
X       mcap  = min(M,n) 
X        mcap1 = mcap + 1
Xc
Xc calculate amk(k), vk(k), and ak(k) for k=1,n (see W522-4 for notation).
Xc
Xc
Xc  k = 1 
Xc
X       amk(1) = zero
X       ak(1)  = one
Xc
Xc  k = 2 ;  initialize phi(1)
Xc
X       z      = d/(one-d)
X       amk(2) = z*dble(x(1))
X       ak(2)  = one - z
X        phi(1) = z
X
X        t  = dgamr(one-d)
X        if (IGAMMA .ne. 0) return
X
X       g0 = dgamma(one-(two*d))*(t*t)
X        if (IGAMMA .ne. 0) return
X 
X       vk(1)  = g0
X       vk(2)  = g0*(one-(z*z))
Xc
Xc  k = 3, mcap
Xc
X       do k = 3, mcap
X          km = k - 1
X          t  = dble(km)
X          u  = t - d
Xc 
Xc  calculate phi() and vk() using the recursion formula on W498
Xc
X         do j = 1, km-1
X            s      = t-dble(j)
X           phi(j) = phi(j)*(t*(s-d)/(u*s))
X          end do
X
X          v       = d / u
X          phi(km) = v
X          vk(k)   = vk(km)*(one-(v*v))
Xc
Xc  form amk(k) and ak(k)
Xc
X         u = zero
X         v =  one
X         do j = 1, km
X            t  = phi(j)
X           u  = u + (t*dble(x(k-j)))
X           v  = v - t
X          end do
X          amk(k) = u
X          ak(k)  = v
X        end do
X       
X        if (mcap .eq. n) go to 200
Xc
Xc  k = mcap+1, n
Xc      
Xc calculate pi(j), j = 1,mcap
Xc
X       pi(1) = d
X       s     = d
X       do j = 2, mcap
X          u     = dble(j)
X          t     = pi(j-1)*((u-one-d)/u)
X         s     = s + t
X          pi(j) = t
X        end do
X
X        s =  one - s
X       r = zero
X        u = dble(mcap)
X        t = u*pi(mcap)
Xc
X       do  k = mcap1, n
X         km = k - mcap
X          z  = zero
X         do  j = 1, mcap
X            z = z + (pi(j)*dble(x(k-j)))
X          end do
X          if (r .eq. zero) then
X            amk(k) = z
X                   ak(k)  = s
X          else 
X            v      = (t*(one - (u/dble(k))**d))/d
X           amk(k) = z + ((v*r)/(dble(km)-one))
X            ak(k)  = s - v
X          end if
X          r = r + dble(x(km))
X        end do
X
X 200    continue
Xc
Xc  form muhat - see formula on W523.
Xc
X        r = zero
X       s = zero
X       do  k = 1, n
X           t = ak(k)
X          u = (dble(x(k))-amk(k))*t
X          v = t*t
X           if (k .le. mcap) then
X             z = vk(k)
X             u = u / z
X             v = v / z
X           end if
X           r = r + u
X           s = s + v
X        end do
X
X       hatmu = r / s
Xc
Xc  form filtered version
Xc
X        s = zero   
X       do k= 1, mcap
X         s = s + log(vk(k))
X        end do
X
X        slogvk = s
X        sumlog = s
X
X        s = zero
X       do k= 1, n
X          t    = (dble(x(k))-amk(k)-hatmu*ak(k))
X          if (k .le. mcap) t = t / sqrt(vk(k))
X         s    = s + t
X          y(k) = t
X        end do
X
X        if (npq .eq. 0) return
X
X        t = dble(n)
X
X        u = z / t
X       do k= 1, n
X          y(k) = y(k) - u
X        end do
X
X      return
X      end
X
X*****************************************************************************
X*****************************************************************************
X
X      subroutine ajqp( qp, a, ajac, lajac, iflag, y)
X
X      implicit double precision (a-h,o-z)
X
X      integer          lajac, iflag
X
Xc     double precision qp(npq), a(nm), ajac(nm,npq), y(n)
X      double precision qp(*), a(*), ajac(lajac,*), y(*)
X
X      integer          maxopt, maxfun, nopt, nfun, ngrd,
X     *                 ifun, igrd, info
X      common /CNTRFD/  maxopt, maxfun, nopt, nfun, ngrd,
X     *                 ifun, igrd, info
X      save   /CNTRFD/
X     
X      integer          n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/  n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      save   /MAUXFD/
X
X      double precision s, t
X
X      double precision zero
X      parameter       (zero=0.d0)
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc--------------------------------------------------------------------------
X
X        if (iflag .eq. 2) goto 200
X
X        if (iflag .ne. 1) return
Xc
Xc  objective calculation
Xc
X        do k = maxpq1, n
X          km = k - maxpq
X          t  = zero
X          if (np .ne. 0) then
X            do l = 1, np
X              t  = t - qp(nq+l)*y(k-l)
X            end do
X          end if
X          s = zero
X          if (nq .ne. 0) then
X            do l = 1, nq
X              if (km .le. l) goto 101
X              s  = s + qp(l)*a(km-l)
X            end do
X          end if
X 101      s = y(k) + (t + s)
X          if (abs(s) .le. BIGNUM) then
X            a(km) = s
X          else                    
X            a(km) = sign(one,s)*BIGNUM
X          end if
X        end do
X
X        nfun = nfun + 1
X
X        return
X
X 200    continue
Xc
Xc  jacobian calculation
Xc
X        do i = 1, npq
X          do k = maxpq1, n
X            km  =  k - maxpq
X            t   = zero
X            if (nq .ne. 0) then
X              do l = 1, nq
X                if (km .le. l) goto 201
X                t  = t +  qp(l)*ajac(km-l,i)
X              end do
X            end if
X 201        continue
X            if (i .le. nq) then
X              if (km .gt. i) then
X                s = a(km-i) + t
X              else
X                s =           t
X              end if
X            else
X              s = -y(k-(i-nq)) + t
X            end if
X            if (abs(s) .le. BIGNUM) then
X              ajac(km,i) = s
X            else                    
X              ajac(km,i) = sign(one,s)*BIGNUM
X            end if
X          end do
X        end do
X
X        ngrd = ngrd + 1
X
X      return
X      end
X
X*****************************************************************************
X*****************************************************************************
X
X      subroutine  ajp( p, a, ajac, lajac, iflag, y)
X
X      implicit double precision (a-h,o-z)
X
X      integer          lajac, iflag
Xc     double precision p(np), a(nm), ajac(nm,npq), y(n)
X      double precision p(*), a(*), ajac(lajac,*), y(*)
X     
X      integer          n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/  n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      double precision  t
X
X      double precision zero
X      parameter       (zero=0.d0)
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc--------------------------------------------------------------------------
X
X        if (iflag .eq. 2) goto 200
X
X        if (iflag .ne. 1) return
X
X        if (np .eq. 0) return
Xc
Xc  objective calculation
Xc
X        do k = np+1, n
X          t  = zero
X          do l = 1, np
X            t  = t - p(l)*y(k-l)
X          end do
X 101      a(k-np) = y(k) + t
X        end do
X
X        return
X
X 200    continue
Xc
Xc  jacobian calculation
Xc
X          do i = 1, np
X            do k = np+1, n
X              ajac(k-np,i) = -y(k-i) 
X            end do
X          end do
X
X      return
X      end
X
X*****************************************************************************
X*****************************************************************************
X
X      subroutine  ajq( qp, a, ajac, lajac, iflag, y)
X
X      implicit double precision (a-h,o-z)
X
X      integer          lajac, iflag
Xc     double precision qp(npq), a(nm), ajac(nm,npq), y(n)
X      double precision qp(*), a(*), ajac(lajac,*), y(*)
X     
X      integer          n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/  n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      integer          maxopt, maxfun, nopt, nfun, ngrd, 
X     *                 ifun, igrd, info
X      common /CNTRFD/  maxopt, maxfun, nopt, nfun, ngrd, 
X     *                 ifun, igrd, info
X      save   /CNTRFD/
X
X      double precision s, t
X
X      double precision zero
X      parameter       (zero=0.d0)
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc--------------------------------------------------------------------------
X
X        if (iflag .eq. 2) goto 200
X
X        if (iflag .ne. 1) return
X
X        if (nq. eq. 0) return
Xc
Xc  objective calculation
Xc
X        do k = maxpq1, n
X          km = k - maxpq
X          t  = zero
X          if (np .ne. 0) then
X            do l = 1, np
X              t  = t - qp(nq+l)*y(k-l)
X            end do
X          end if
X          s = zero
X          if (nq .ne. 0) then
X            do l = 1, nq
X              if (km .le. l) goto 101
X              s  = s + qp(l)*a(km-l)
X            end do
X          end if
X 101      a(km) = y(k) + (t + s)
X        end do
X
X        nfun = nfun + 1
X
X        return
X
X 200    continue
Xc
Xc  jacobian calculation
Xc
X        do i = 1, npq
X          do k = maxpq1, n
X            km  =  k - maxpq
X            t   = zero
X            if (nq .ne. 0) then
X              do l = 1, nq
X                if (km .le. l) goto 201
X                t  = t +  qp(l)*ajac(km-l,i)
X              end do
X            end if
X 201        continue
X            if (i .le. nq) then
X              if (km .gt. i) then
X                ajac(km,i) = a(km-i)    + t
X              else
X                ajac(km,i) =              t
X              end if
X            else
X              ajac(km,i) = -y(k-(i-nq)) + t
X            end if
X          end do
X        end do
X
X        ngrd = ngrd + 1
X
X      return
X      end
END_OF_FILE
  if test 32133 -ne `wc -c <'fdcore.f'`; then
    echo shar: \"'fdcore.f'\" unpacked with wrong size!
  fi
  # end of 'fdcore.f'
fi
if test -f 'fdsim.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdsim.f'\"
else
  echo shar: Extracting \"'fdsim.f'\" \(3737 characters\)
  sed "s/^X//" >'fdsim.f' <<'END_OF_FILE'
X      subroutine fdsim( n, ip, iq, ar, ma, d, rmu, y, s)
X
X      implicit double precision (a-h,o-z)
X
Xc  generates a random time series for use with fracdf
Xc
Xc  Input :
Xc
Xc  n      integer  length of the time series
Xc  ip     integer  number of autoregressive parameters
Xc  ar     real    (ip) autoregressive parameters
Xc  ma     real    (iq) moving average parameters
Xc  d      real     fractional differencing parameters
Xc  rmu    real     time series mean
Xc  y      real    (n+iq) 1st n : normalized random numbers
Xc  s      real    (n+iq) workspace
Xc
Xc  Output :
Xc
Xc  s      real   (n) the generated time series
X
Xc-----------------------------------------------------------------------------
Xc
Xc        Simulates a series of length n from an ARIMA (p,d,q) model
Xc        with fractional d (0 < d < 0.5). 
Xc
Xc-----------------------------------------------------------------------------
X
X      integer            n, ip, iq
Xc     real               ar(ip), ma(iq), rmu, d
X      real               ar(*), ma(*), rmu, d
X
X      real               g0, vk, amk, sum, dk1, dk1d, dj, temp
Xc     real               y(n+iq), s(n+iq)
X      real               y(*), s(*)
X
X      double precision   dgamr, dgamma, dlamch
X
X      real               gennor
X
X      external           dgamr, dgamma, dlamch, gennor
X
X      integer            k, j, i
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
X      real              zero, one, two
X      parameter        (zero = 0.0, one = 1.0, two = 2.0)
X
Xc--------------------------------------------------------------------------
X
X        IGAMMA = 0
X        JGAMMA = 0
X
X        FLTMIN  = dlamch( 'S' )
Xc       FLTMAX  = dlamch( 'O' )  not guaranteed to work
X        FLTMAX  = 1.0 / FLTMIN
X        BASE    = dlamch( 'B' )
X        EPSMIN  = dlamch( 'E' )
X        EPSMAX  = EPSMIN*BASE
Xc
Xc       Calculate g0
X
X        temp = real(dgamr(dble(one-d)))
X        if (IGAMMA .ne. 0) then
X          do i = 1, n
X            s(i) = zero
X          end do
X          return
X        end if
X
X        g0   = real(dgamma(dble(one-two*d)))*(temp*temp)
X        if (IGAMMA .ne. 0) then
X          do i = 1, n
X            s(i) = zero
X          end do
X          return
X        end if
Xc
Xc       Generate y(1)
Xc
X       y(1) = gennor(0.,1.)*sqrt(g0)
Xc
Xc       Generate y(2) and initialise vk,phi(j)
Xc
X       temp  = d / (one-d)
X       vk    = g0*(one-(temp*temp))
X
X       amk   = temp*y(1)
X        s(1)  = temp
X       y(2)  = amk + gennor(0.,1.)*sqrt(vk)
Xc
Xc       Generate y(3),...,y(n+iq)
Xc
X       do k = 3, n + iq
X          dk1  = real(k) - one
X          dk1d = dk1 - d
Xc
Xc       Update the phi(j) using the recursion formula on W498
Xc
X          do j = 1, k-2
X            dj   = dk1 - real(j) 
X            s(j) = s(j)*(dk1*(dj-d)/(dk1d*dj))
X          end do
X
X                 temp   = d / dk1d
X          s(k-1) = temp
Xc
Xc       Update vk
Xc
X         vk = vk * (one-(temp*temp))
Xc
Xc       Form amk
Xc
X         amk = zero
X         do j = 1, k-1
X           amk = amk + s(j)*y(k-j)
X          end do
Xc
Xc       Generate y(k)
Xc
X         y(k) = amk + gennor(0.,1.)*sqrt(vk)
X
X       end do
Xc
Xc       We now have an ARIMA (0,d,0) realisation of length n+iq in 
Xc       y(k),k=1,n+iq. We now run this through an inverse ARMA(p,q)
Xc       filter to get the final output in x(k),k=1,n.
Xc
X
X       do k = 1, n 
X
X         sum = zero
X
X          do i = 1, ip
X           if (k .le. i) go to 10
X           sum = sum + ar(i)*s(k-i)
X          end do
X
X10        continue
X
X          do j = 1, iq
X           sum = sum-ma(j)*y(k+iq-j)
X          end do
X
X         s(k) = sum + y(k+iq)
X
X        end do
X
X        if (rmu .ne. zero) then
X          do i = 1, n
X            s(i) = s(i) + rmu
X          end do
X        end if
X
X       return
X       end
END_OF_FILE
  if test 3737 -ne `wc -c <'fdsim.f'`; then
    echo shar: \"'fdsim.f'\" unpacked with wrong size!
  fi
  # end of 'fdsim.f'
fi
if test -f 'fdout.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdout.f'\"
else
  echo shar: Extracting \"'fdout.f'\" \(7475 characters\)
  sed "s/^X//" >'fdout.f' <<'END_OF_FILE'
X      subroutine fdout ( d, p, q, dtol, drange, h, hood,
X     *                   cov, lcov, stderr, cor, lcor)
X
X      integer            lcov, lcor
Xc     double precision   p(np), q(nq), stderr(npq1) 
X      double precision   p(*), q(*), stderr(*) 
Xc     double precision   cov(lcov,npq1), cor(lcor,npq1)
X      double precision   cov(lcov,*), cor(lcor,*)
X      double precision   d, dtol, drange(2), h, hood
X
X      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            lenw, lfree
X      common /WORKFD/    lenw, lfree
X      save   /WORKFD/
X
X      integer            ILIMIT, JLIMIT
X      common /LIMSFD/    ILIMIT, JLIMIT
X      save   /LIMSFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
X      integer            IMINPK, JMINPK
X      common /MNPKFD/    IMINPK, JMINPK
X      save   /MNPKFD/
X
X      integer            KSVD, KCOV, KCOR
X      common /HESSFD/    KSVD, KCOV, KCOR
X      save   /HESSFD/
X
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc-----------------------------------------------------------------------------
X
X      write( 6, *)
X      write( 6, *) 'n = ', n, '   M = ', M
X
X      if (ILIMIT .ne. 0) goto 300
X      if (IGAMMA .ne. 0 .or. IMINPK .ne. 0) goto 200
X
X      write( 6, *)
X      write( 6, *) 'log maximum likelihood : ', hood
X      write( 6, *)
X      write( 6, *) 'fractional differencing parameter : d  = ', d
X      write( 6, *)
X      write( 6, *) 'autoregressive parameters :'
X      if (np .gt. 0) then
X        do i = 1, np
X          write(6,*) i, p(i)
X        end do
X      end if
X      write( 6, *)
X      write( 6, *) 'moving average parameters :'
X      if (nq .gt. 0) then
X        do i = 1, nq
X          write(6,*) i, q(i)
X        end do
X      end if
X
X      if (KSVD .ne. 0)  goto 100
X
X      if (KCOR .eq. 0) then
X
X        write(6,*)
X        write(6,*) 'standard error (d,p,q) : '
X        do i = 1, npq1
X          write( 6, *) i, stderr(i)
X        end do
X
X        write(6,*)
X        write(6,*) 'covariance, correlation matrices (d,p,q) : '
X        do i = 1, npq1
X          do j = i, npq1
X            write( 6, *) i, j, cov(i,j), cor(i,j)
X          end do
X       end do
X
X      else
X
X        write(6,*)
X        write(6,*) 'covariance matrix (d,p,q) : '
X        do i = 1, npq1
X          do j = i, npq1
X            write( 6, *) i, j, cov(i,j)
X          end do
X       end do
X
X      end if
X
X 100  write( 6, *)
X      write( 6, *) 'interval of uncertainty for d : ', dtol
X      write( 6, *) '                  range for d : ', drange
X      write( 6, *) '   finite difference interval :', h
X      write( 6, *) '   relative machine precision : ', EPSMAX
X
X 200  write(6,*)
X      write(6,*) 'work space  required', lfree
X      write(6,*) 'work space allocated', lenw
X
X 300  call fdmsg
X
X      return
X 900  format( 4h itr, 14h     d          ,   14h    est mean  , 
X     *                16h     white noise,  17h     log likelihd,
X     *                 4h  nf, 3h ng)
X      end
X
X*******************************************************************************
X*******************************************************************************
X
X      subroutine fdmsg()
X
X      integer            ILIMIT, JLIMIT
X      common /LIMSFD/    ILIMIT, JLIMIT
X      save   /LIMSFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
X      integer            IMINPK, JMINPK
X      common /MNPKFD/    IMINPK, JMINPK
X      save   /MNPKFD/
X
X      integer            KSVD, KCOV, KCOR
X      common /HESSFD/    KSVD, KCOV, KCOR
X      save   /HESSFD/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc-----------------------------------------------------------------------------
X
X      if (ILIMIT .ne. 0) then
X        write(6,*)
X        write(6,*) '+++ insufficient storage'
X        write(6,*) '    increase work space by at least ', ILIMIT
X      end if
X
X      if (IGAMMA .ne. 0) then
X        write(6,*)
X        write(6,*) '+++ error condition in gamma function computation'
X        if (IGAMMA .eq. 11) write(6,*) 
X     *    'dgamma : x = 0'
X        if (IGAMMA .eq. 12) write(6,*) 
X     *    'dgamma : x too close to a negative integer'
X        if (IGAMMA .eq. 13) write(6,*) 
X     *    'dgamma : x so close to 0. that result would overflow'
X        if (IGAMMA .eq. 14) write(6,*) 
X     *    'dgamma : x so large that result would overflow'
X        if (IGAMMA .eq. 21) write(6,*) 
X     *    'd9gaml : unable to compute xmin'
X        if (IGAMMA .eq. 22) write(6,*) 
X     *    'd9gaml : unable to compute xmax'
X        if (IGAMMA .eq. 41) write(6,*) 
X     *    'dcsevl : number of terms < 0'
X        if (IGAMMA .eq. 42) write(6,*) 
X     *    'dcsevl : more than 1000 terms'
X        if (IGAMMA .eq. 43) write(6,*) 
X     *    'dcsevl : x outside (-1,+1)'
X        if (IGAMMA .eq. 51) write(6,*) 
X     *    'd9lgmc : x must be >= 10'
X        if (IGAMMA .eq. 61) write(6,*) 
X     *    'dlngam : abs(x) so large that result would overflow'
X        if (IGAMMA .eq. 62) write(6,*) 
X     *    'dlngam : x is a negative integer'
X        write(6,*)
X      end if
X
X      if (IMINPK .ne. 0) then
X        write(6,*)
X        write(6,*) '+++ error condition in optimization'
X        if (JMINPK .eq. 10) write(6,*) 
X     *    'MINPACK : improper input parameters'
X        write(6,*)
X      end if
X
X      if (JGAMMA .ne. 0) then
X        write(6,*)
X        write(6,*) '+++ WARNING in gamma function computation'
X        if (JGAMMA .eq. 11) write(6,*) 
X     *    'dgamma : precision lost since x too near a negative integer'
X        if (JGAMMA .eq. 12) write(6,*) 
X     *    'dgamma : x so small that result underflows'
X        if (JGAMMA .eq. 31) write(6,*) 
X     *    'initds : number of coefficients < 1'
X        if (JGAMMA .eq. 32) write(6,*) 
X     *    'initds : eta may be too small'
X        if (JGAMMA .eq. 51) write(6,*) 
X     *    'd9lgmc : x so big that result would overflow'
X        if (JGAMMA .eq. 61) write(6,*) 
X     *    'dlngam : precision lost since x too near a negative integer'
X        write(6,*)
X      end if
X
X      if (JMINPK .ne. 0) then
X        write(6,*)
X        write(6,*) '+++ WARNING in optimization'
X        if (JMINPK .eq. 5) write(6,*) 
X     *    'MINPACK : function evaluation limit reached'
X        if (JMINPK .eq. 6) write(6,*) 
X     *    'MINPACK : ftol is too small'
X        if (JMINPK .eq. 7) write(6,*) 
X     *    'MINPACK : xtol is too small'
X        if (JMINPK .eq. 8) write(6,*) 
X     *    'MINPACK : gtol is too small'
X        write(6,*)
X      end if
X
X      if (JLIMIT .ne. 0) then
X        write(6,*)
X        write(6,*) '+++ WARNING : optimization limit reached'
X        write(6,*)
X      end if
X
X      if (KSVD .ne. 0) then
X        write(6,*)
X        write(6,*) 
X     *    'dsvdc : cannot compute singular values of covariance matrix'
X        return
X      end if 
X
X      if (KCOV .ne. 0) then
X        write(6,*)
X        write(6,*) 
X     *      '+++ Hessian matrix is singular, pseudo-inverse returned'
X      end if 
X
X      if (KCOR .ne. 0) then
X        write(6,*)
X        write(6,*) 
X     *  '+++ covariance matrix is not computationally positive definite'
X      end if 
X
X      return
X      end
END_OF_FILE
  if test 7475 -ne `wc -c <'fdout.f'`; then
    echo shar: \"'fdout.f'\" unpacked with wrong size!
  fi
  # end of 'fdout.f'
fi
if test -f 'fdhess.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdhess.f'\"
else
  echo shar: Extracting \"'fdhess.f'\" \(22963 characters\)
  sed "s/^X//" >'fdhess.f' <<'END_OF_FILE'
X      subroutine fdvar(  x, n, M, nar, nma, dtol, drange, h, hood, d, 
X     *                   ar, ma, cov, lcov, stderr, cor, lcor, 
X     *                   hess, lhess, w, lenw)
X
X      implicit double precision (a-h,o-z)
X
X      integer            n, M, nar, nma, lcov, lcor, lenw
Xc     real               x(n)
X      real               x(*)
Xc     double precision   drange(2), ar(nar), ma(nma), stderr(nar+nma+1) 
X      double precision   drange(2), ar(*), ma(*), stderr(*)
Xc     double precision   cov(lcov,nar+nma+1), cor(lcor,nar+nma+1)
X      double precision   cov(lcov,*), cor(lcor,*), hess(lhess,*)
X      double precision   d, dtol, h, hood
Xc     double precision   w(lenw)
X      double precision   w(*)
X
Xc------------------------------------------------------------------------------
Xc
Xc       Recomputes the covariance, correlation and standard error given
Xc       a finite-dfference interval h for derivatives with respect to d.
Xc
Xc------------------------------------------------------------------------------
X
X      double precision   dlamch
X
X      double precision   BASE
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      save   /MAUXFD/
X
X      integer            nn, MM, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    nn, MM, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      integer            maxopt, maxfun, nopt, nfun, ngrd,
X     *                   ifun, igrd, info
X      common /CNTRFD/    maxopt, maxfun, nopt, nfun, ngrd,
X     *                   ifun, igrd, info
X      save   /CNTRFD/    
X
X      double precision   told, tolf, tolx, tolg, anorm, deltax, gnorm
X      common /TOLSFD/    told, tolf, tolx, tolg, anorm, deltax, gnorm
X      save   /TOLSFD/
X
X      integer            lenthw, lwfree
X      common /WORKFD/    lenthw, lwfree
X      save   /WORKFD/
X
X      integer            ly, lamk, lak, lvk, lphi, lpi
X      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
X      save   /WFILFD/
X
X      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf, 
X     *                   lwa1, lwa2, lwa3, lwa4
X      save   /WOPTFD/
X
X      integer            ILIMIT, JLIMIT
X      common /LIMSFD/    ILIMIT, JLIMIT
X      save   /LIMSFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
X      integer            IMINPK, JMINPK
X      common /MNPKFD/    IMINPK, JMINPK
X      save   /MNPKFD/
X
X      integer            KSVD, KCOV, KCOR
X      common /HESSFD/    KSVD, KCOV, KCOR
X      save   /HESSFD/
X
X      double precision   zero, one
X      parameter         (zero=0.d0, one=1.d0)
X
X      data               ncalls /0/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc-----------------------------------------------------------------------------
X
X      if (ncalls .eq. 0) then
X
Xc machine constants
X
X        FLTMIN  = dlamch( 'S' )
XC       FLTMAX  = dlamch( 'O' )  not guaranteed to work
X        FLTMAX  = one/FLTMIN
X        BASE   = dlamch( 'B' )
X        EPSMIN = dlamch( 'E' )
X        EPSMAX = EPSMIN*BASE
X        EPSPT5 = sqrt(EPSMIN)
X        EPSP25 = sqrt(EPSPT5)
X        EPSPT3 = EPSMIN**(.3)
X        EPSP75 = EPSMIN**(.75)
X        BIGNUM = one / EPSMIN
X        ncalls = 1
X      endif
X
Xc set error and warning flags
X
X      IGAMMA = 0
X      IMINPK = 0
X      ILIMIT = 0
X
X      JGAMMA = 0
X      JMINPK = 0
X      JLIMIT = 0
X
X      KSVD   = 0
X      KCOV   = 0
X      KCOR   = 0
X
Xc useful quantities
X
X      if (M .le. 0) M = 100
X
X      nn    = n
X      MM    = M
X      np    = nar
X      nq    = nma
X
X      npq    = np + nq
X      npq1   = npq + 1
X      maxpq  = max(np,nq)
X      minpq  = min(np,nq)
X      maxpq1 = maxpq + 1
X
X      told = dtol
X
X      if (dtol .le. zero) then
X        tolf  =  EPSPT3
X        tolx  =  told
X        tolg  =  EPSPT3
X      else
X        tolf  =  max( dtol/1.d1, EPSP75)
X        tolx  =  told
X        tolg  =  max( dtol/1.d1, EPSP75)
X      end if
X
X      nm     = n - maxpq
X 
Xc workspace allocation
X
X      lqp    = 1
X      ly     = lqp    +  npq
X      lamk   = ly
X      lak    = lamk   +  n
X      lphi   = lak    +  n
X      lvk    = lphi   +  M
X      lpi    = lphi
X      la     = ly     +  n
X      lajac  = la     +  n - minpq
X      ipvt   = lajac  +  max( (n-np)*np, (n-nq)*nq, (n-maxpq)*npq)
X      ldiag  = ipvt   +  npq/2 + 1
X      lqtf   = ldiag  +  npq
X      lwa1   = lqtf   +  npq
X      lwa2   = lwa1   +  npq
X      lwa3   = lwa2   +  npq
X      lwa4   = lwa3   +  npq
X      lfree  = lwa4   +  n - minpq
X
X      lwfree = max( (lvk+M), (lwa4+n-minpq), (12*31))
X      lenthw = lenw
X
X      if (lwfree  .gt. (lenw+1)) then
X        ILIMIT = lwfree - lenw
Xc       write( 6, *) 'insufficient storage : ',
Xc    *               'increase length of w by at least', incw
X        return
X      endif
X
X      if (npq .gt. 0) then
X        do i = 2, npq1
X         call dcopy( npq, hess(2,i), 1, cov(2,i), 1)
X        end do
X    
X        call fdcov( x, d, h, hess, cov, lcov, cor, lcor, stderr, w)
X
X        do i = 1, npq
X          hess(1,i+1) = hess(i+1,1)
X        end do
X      end if
X
X      call fdout ( d, ar, ma, dtol, drange, h, hood,
X     *             cov, lcov, stderr, cor, lcor)
X
X      return
X      end
X
X*******************************************************************************
X*******************************************************************************
X
X      subroutine fdhpq( x, H, lH, w)
X
X      implicit double precision (a-h,o-z)
X
X      integer            lH
Xc     real               x(n)
X      real               x(*)
Xc     double precision   H(lH, npq1)
X      double precision   H(lH, *)
X      double precision   w(*)
X
X      double precision   zero
X      parameter         (zero=0.d0)
X
X      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      integer            ly, lamk, lak, lvk, lphi, lpi
X      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
X      save   /WFILFD/
X
X      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf, 
X     *                   lwa1, lwa2, lwa3, lwa4
X      save   /WOPTFD/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc-----------------------------------------------------------------------------
X
X      call hesspq( w(lqp), w(la), w(lajac), nm, H, lH, 
X     *             w(lwa4), w(lwa1))
X
Xc     call dcopy( npq1, zero, 0, H(1,1), lH)
Xc     call dcopy( npq , zero, 0, H(2,1), 1)
X
X      return
X      end
X
X*******************************************************************************
X*******************************************************************************
X
X      subroutine fdcov( x, d, hh, hd, cov, lcov, cor, lcor, se, w)
X
X      implicit double precision (a-h,o-z)
X
X      integer            lcov
Xc     real               x(n)
X      real               x(*)
Xc     double precision   d, hh, hd(npq1), cov(lcov,npq1), 
Xc    *                   cor(lcor,npq1), se(npq1)
X      double precision   d, hh, hd(*), cov(lcov,*), cor(lcor,*), se(*)
X      double precision   w(*)
X
X      double precision   temp
X
X      double precision   zero, one, two
X      parameter         (zero=0.d0, one=1.d0, two=2.d0)
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      save   /MAUXFD/
X
X      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      integer            ly, lamk, lak, lvk, lphi, lpi
X      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
X      save   /WFILFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
X      integer            KSVD, KCOV, KCOR
X      common /HESSFD/    KSVD, KCOV, KCOR
X      save   /HESSFD/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
Xc-----------------------------------------------------------------------------
X
X      call hesdpq( x, d, hh, hd, w)
X
X      call dcopy( npq1, hd, 1, cov, lcov)
X
X      IGAMMA = 0
X      JGAMMA = 0
X
X      KSVD = 0
X      KCOV = 0
X      KCOR = 0
X
X      temp = one
X      do i = 1, npq1
X        do j = i+1, npq1
X          cov(j,i) = cov(i,j)
X        end do
X      end do
X
X      ls    = ly
X      lu    = ls    + npq1 + 1
X      lv    = lu    + npq1*npq1
X      le    = lv    + npq1*npq1
X      lwork = le    + npq1
X      lfree = lwork + npq1
X
X      call dsvdc( cov, lcov, npq1, npq1, w(ls), w(le), 
X     *            w(lu), npq1, w(lv), npq1, w(lwork), 11, info)
X
X      if (info .ne. 0) then
X        call dcopy( npq1, zero, 0, se, 1)
X        do j = 1, npq1
X           call dcopy( npq1, zero, 0, cov(1,j), 1)
X        end do
X        KSVD = 1
X        info = 3
X        return
X      end if
X
X      call invsvd( w(ls), w(lu), npq1, w(lv), npq1, cov, lcov)
X
X      do i = 1, npq1
X        do j = i+1, npq1 
X          cov(j,i) = cov(i,j)
X        end do
X      end do
X
X      temp = one
X      do j = 1, npq1
X        if (cov(j,j) .gt. zero) then
X          se(j) = sqrt(cov(j,j))
X        else
X          temp  = min(temp,cov(j,j))
X          se(j) = zero
X        end if
X      end do
X
X      if (temp .eq. one) then
X        do k = 1, npq1
X          call dcopy( k, cov( 1, k), 1, cor( 1, k), 1)
X        end do
X        do i = 1, npq1
X          call dscal( (npq1-i+1), (one/se(i)), cor(i,i), lcor)
X        end do
X        do j = 1, npq1
X          call dscal( j, (one/se(j)), cor(1,j),    1)
X        end do
X      else
X        KCOR = 1
X        do j = 1, npq1
X          call dcopy( npq1, zero, 0, cor(1,j), 1)
X        end do
X      end if
X
X      do i = 1, npq1
X        do j = i+1, npq1
X          cor(j,i) = cor(i,j)
X        end do
X      end do
X
X      return
X      end
X
X*******************************************************************************
X*******************************************************************************
X
X      subroutine invsvd ( s, u, lu, v, lv, cov, lcov)
X
X      implicit double precision (a-h,o-z)
X
X      integer            lu, lv, lcov
Xc     double precision   s(npq1), u(lu,npq1), v(lv,npq1), cov(lcov,npq1)
X      double precision   s(*), u(lu,*), v(lv,*), cov(lcov,*)
X
X      integer            krank
X      double precision   ss
X
X      double precision   zero, one
X      parameter         (zero=0.d0, one=1.d0)
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      save   /MAUXFD/
X
X      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      integer            KSVD, KCOV, KCOR
X      common /HESSFD/    KSVD, KCOV, KCOR
X      save   /HESSFD/
X
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc-----------------------------------------------------------------------------
X
X      krank = npq1
X     
X      do i = 1, npq1
X        ss = s(i)
X        do j = 1, npq1
X          if (ss .lt. one) then
X            if (abs(u(i,j)) .gt. ss*FLTMAX) then
X              krank = i - 1
X              KCOV  = 1
X              goto 100
X            end if
X          end if
X        end do
X      end do
X
X 100  continue
X
X      do k = 1, npq1
X        call dcopy( k, zero, 0, cov( 1, k), 1)
X      end do
X
X      if (krank .eq. 0) return
X
Xc      do k = 1, npq1
Xc        do i = 1, npq1
Xc          do j = i, npq1
Xc            H(i,j) =  H(i,j) + s(k)*u(i,k)*v(j,k)
Xc          end do
Xc        end do
Xc      end do
X      
Xc      do k = 1, npq1
Xc        ss = s(k)
Xc        do j = 1, npq1
Xc          call daxpy( j, ss*v(j,k), u(1,k), 1, H(1,j), 1)
Xc        end do
Xc      end do
X      
X      do k = 1, krank
X        ss = (-one/s(k))
X        do j = 1, npq1
X          call daxpy( j, (ss*u(j,k)), v(1,k), 1, cov(1,j), 1)
X        end do
X      end do
X
X      return
X      end
X
X*******************************************************************************
X*******************************************************************************
X
X      subroutine hesspq( qp, a, ajac, lajac, H, lH, aij, g)
X
X      implicit double precision (a-h,o-z)
X      integer           lajac, lH
Xc     double precision  qp(npq), a(nm), ajac(nm,npq)
X      double precision  qp(*), a(*), ajac(lajac,*)
Xc     double precision  H(lH,npq1), aij(nm), g(npq)
X      double precision  H(lH,*), aij(*), g(*)
X
Xc analytic Hessian with respect to p and q variables
X     
X      double precision   ddot
X
X      integer           n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/   n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      double precision   hatmu, wnv, cllf
X      common /FILTFD/    hatmu, wnv, cllf
X      save   /FILTFD/
X
X      double precision  fac, s, t, u
X
X      double precision  zero, one, two
X      parameter        (zero=0.d0, one=1.d0, two=2.d0)
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc-----------------------------------------------------------------------------
X
X      fac = one / (wnv * dble(nm-1))
X
X      if (nq .ne. 0 .and. np .ne. 0) then
X        do k = 1, npq
X          g(k) = ddot( nm, a, 1, ajac( 1, k), 1) 
X        end do
X        do i = 1, np
X          u = g(nq+i)
X          do j = 1, nq
X            u = g(j)*u
X            do k = maxpq1, n
X              km = k - maxpq
X              t  = zero
X              do l = 1, nq
X                if (km .le. l) goto 301
X                t  = t + qp(l)*aij(km-l)
X              end do
X 301          continue
X              if (km .gt. j) then
X                aij(km) = ajac(km-j,nq+i) + t
X              else
X                aij(km) =                   t
X              end if
X            end do
X            s = ddot( nm, ajac( 1, nq+i), 1, ajac( 1, j), 1) 
X            t = ddot( nm, a             , 1, aij        , 1) 
X            H(i+1,np+j+1) = -dble(n)*((s + t) - two*fac*u)*fac
X          end do
X        end do
X      end if 
X
X      if (nq .ne. 0) then
X        do i = 1, nq
X          u = g(i)
X          do j = i, nq
X            u = g(j)*u
X            do k = maxpq1, n
X              km = k - maxpq
X              t  = zero
X              do l = 1, nq
X                if (km .le. l) goto 302
X                t  = t + qp(l)*aij(km-l)
X              end do
X 302          continue
X              s  = zero
X              if (km .gt. i) s = s + ajac(km-i,j) 
X              if (km .gt. j) s = s + ajac(km-j,i)
X              aij(km) = s + t
X            end do
X            s = ddot( nm, ajac( 1, i), 1, ajac( 1, j), 1) 
X            t = ddot( nm, a          , 1, aij        , 1) 
X            H(np+i+1,np+j+1) = -dble(n)*((s + t) - two*fac*u)*fac    
X          end do
X        end do
X      end if 
X
X      if (np .ne. 0) then
X        do i = 1, np
X          u = g(nq+i)
X          do j = i, np
X            u = g(nq+j)*u
Xc            do k = maxpq1, n
Xc              km  =  k - maxpq
Xc              t  = zero
Xc              if (nq .ne. 0) then
Xc               do l = 1, nq
Xc                  if (km .le. l) goto 303
Xc                  t  = t + qp(l)*aij(km-l)
Xc               end do
Xc              end if
Xc 303          continue
Xc              aij(km) = t
Xc            end do
X            s = ddot( nm, ajac( 1, nq+i), 1, ajac( 1, nq+j), 1) 
Xc            t = ddot( nm, a             , 1, aij           , 1) 
Xc            H(i+1,j+1) = -dble(n)*((s + t) - two*fac*u)*fac
X            H(i+1,j+1) = -dble(n)*(s - two*fac*u)*fac
X          end do
X        end do
X      end if 
X
X      return
X      end
X
X*******************************************************************************
X*******************************************************************************
X
X      subroutine hesdpq( x, d, hh, hd, w)
X
X      implicit double precision (a-h,o-z)
X
Xc     real               x(n)
X      real               x(*)
X
Xc     double precision   d, hh, hd(npq1), w(*)
X      double precision   d, hh, hd(*), w(*)
X
X      double precision   slogvk
X
X      intrinsic          log
X      double precision   ddot
X
X      double precision   hatmu, wnv, cllf
X      common /FILTFD/    hatmu, wnv, cllf
X      save   /FILTFD/
X
X      double precision   zero, half, one, two
X      parameter         (zero=0.d0, half=.5d0, one=1.d0, two=2.d0)
X
X      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      integer            ly, lamk, lak, lvk, lphi, lpi
X      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
X      save   /WFILFD/
X
X      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      save   /WOPTFD/      
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      save   /MAUXFD/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc-----------------------------------------------------------------------------
X
X      if (hh .le. zero) hh = (one+abs(cllf))*EPSPT5
X
X      hh = min( hh, .1d0)
X
X      if ((d-hh) .gt. zero) then
X
X        call fdfilt( x, (d-hh), w(ly), slogvk,
X     *               w(lamk), w(lak), w(lvk), w(lphi), w(lpi))
X
X        if (npq .ne. 0) then
X          call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
X          call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))
X
X          call gradpq( w(lwa1), w(la), w(lajac), nm)
X
X          wnv = ddot( nm, w(la), 1, w(la), 1) 
X
X          call dscal( npq, (one/wnv), w(lwa1), 1)
X
X          wnv = wnv / dble(nm - 1)
X        else
X          wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
X        end if
X
X        fa  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two
X
X        if ((d+hh) .lt. half) then
X
X          call fdfilt( x, (d+hh), w(ly), slogvk,
X     *                 w(lamk), w(lak), w(lvk), w(lphi), w(lpi))
X
X          if (npq .ne. 0) then
X            call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
X            call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))
X
X            call gradpq( w(lwa2), w(la), w(lajac), nm)
X
X            wnv = ddot( nm, w(la), 1, w(la), 1) 
X
X            call dscal( npq, (one/wnv), w(lwa2), 1)
X
X            wnv = wnv / dble(nm - 1)
X          else 
X            wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
X          end if
X           
X          fb  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two
X
X          hd(1) = ((fa + fb) - two*cllf) / (hh*hh)
X
X        else
X
X          call fdfilt( x, (d-two*hh), w(ly), slogvk,
X     *                 w(lamk), w(lak), w(lvk), w(lphi), w(lpi))
X
X          if (npq .ne. 0) then
X            call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
X            call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))
X
X            call gradpq( w(lwa2), w(la), w(lajac), nm)
X
X            wnv = ddot( nm, w(la), 1, w(la), 1) 
X
X            call dscal( npq, (one/wnv), w(lwa2), 1)
X
X            wnv = wnv / dble(nm - 1)
X          else 
X            wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
X          end if
X
X          fb  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two
X 
X          hd(1) = ((cllf + fb) -two*fa) / (two*hh*hh)
X
X        endif
X            
X      else
X
X        call fdfilt( x, (d+hh), w(ly), slogvk,
X     *               w(lamk), w(lak), w(lvk), w(lphi), w(lpi))
X
X        if (npq .ne. 0) then
X          call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
X          call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))
X
X          call gradpq( w(lwa1), w(la), w(lajac), nm)
X
X          wnv = ddot( nm, w(la), 1, w(la), 1) 
X
X          call dscal( npq, (one/wnv), w(lwa1), 1)
X
X          wnv = wnv / dble(nm - 1)
X        else
X          wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
X        end if
X
X        fa  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two
X
X        call fdfilt( x, (d+two*hh), w(ly), slogvk,
X     *               w(lamk), w(lak), w(lvk), w(lphi), w(lpi))
X
X        if (npq .ne. 0) then
X          call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
X          call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))
X
X          call gradpq( w(lwa1), w(la), w(lajac), nm)
X
X          wnv = ddot( nm, w(la), 1, w(la), 1) 
X
X          call dscal( npq, (one/wnv), w(lwa1), 1)
X
X          wnv = wnv / dble(nm - 1)
X        else
X          wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
X
X        end if
X
X        fb  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two
X 
X        hd(1) = ((cllf + fb) - two*fa) / (two*hh*hh)
X
X      end if
X
X      if (npq .eq. 0) return
X
X      call daxpy( npq, (-one), w(lwa2), 1, w(lwa1), 1)
X      call dscal( npq, (dble(n)/(two*hh)), w(lwa1), 1)
X
X      call dcopy( npq, w(lwa1), 1, hd(2), 1)
X
X      return
X      end
X
X*******************************************************************************
X*******************************************************************************
X
X      subroutine gradpq( g, a, ajac, ljac)
X
X      implicit double precision (a-h,o-z)
X
X      integer            ljac
Xc     double precision   g(npq), a(nm), ajac(nm,npq)
X      double precision   g(*), a(*), ajac(ljac,*)
X
X      double precision   ddot
X
X      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
X      save   /DIMSFD/
X
X      integer            ly, lamk, lak, lvk, lphi, lpi
X      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
X      save   /WFILFD/
X
X      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf,
X     *                   lwa1, lwa2, lwa3, lwa4
X      save   /WOPTFD/
X
Xc copyright 1991 Department of Statistics, University of Washington
Xc written by Chris Fraley 
X
Xc------------------------------------------------------------------------------
X
X      if (np .ne. 0) then
X        do i = 1, np
X          g(i)    = ddot( nm, a, 1, ajac( 1, nq+i), 1) 
X        end do
X      end if
X
X      if ( nq .ne. 0) then
X        do j = 1, nq
X          g(np+j) = ddot( nm, a, 1, ajac( 1,    j), 1) 
X        end do
X      end if 
X
X      return
X      end
END_OF_FILE
  if test 22963 -ne `wc -c <'fdhess.f'`; then
    echo shar: \"'fdhess.f'\" unpacked with wrong size!
  fi
  # end of 'fdhess.f'
fi
if test -f 'fdmach.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdmach.f'\"
else
  echo shar: Extracting \"'fdmach.f'\" \(22293 characters\)
  sed "s/^X//" >'fdmach.f' <<'END_OF_FILE'
X      DOUBLE PRECISION FUNCTION DLAMCH( CMACH )
X*
X*  -- LAPACK auxiliary routine (preliminary version) --
X*     Univ. of Tennessee, Oak Ridge National Lab, Argonne National Lab,
X*     Courant Institute, NAG Ltd., and Rice University
X*     March 26, 1990
X*
X*     .. Scalar Arguments ..
X      CHARACTER          CMACH
X*     ..
X*
X*  Purpose
X*  =======
X*
X*  DLAMCH determines double precision machine parameters.
X*
X*-----------------------------------------------------------------------
X*
X*  The value returned by DLAMCH is determined by the parameter CMACH as
X*  follows:
X*
X*     CMACH = 'E' or 'e',   DLAMCH := eps
X*     CMACH = 'S' or 's ,   DLAMCH := sfmin
X*     CMACH = 'B' or 'b',   DLAMCH := base
X*     CMACH = 'N' or 'n',   DLAMCH := t
X*     CMACH = 'R' or 'r',   DLAMCH := rnd
X*     CMACH = 'M' or 'm',   DLAMCH := emin
X*     CMACH = 'U' or 'u',   DLAMCH := rmin
X*     CMACH = 'L' or 'l',   DLAMCH := emax
X*     CMACH = 'O' or 'o',   DLAMCH := rmax
X*
X*  where
X*
X*     eps   = relative machine precision
X*     sfmin = safe minimum, such that 1/sfmin does not overflow
X*     base  = base of the machine
X*     t     = number of (base) digits in the mantissa
X*     rnd   = 1.0 when rounding occurs in addition, 0.0 otherwise
X*     emin  = minimum exponent before (gradual) underflow
X*     rmin  = underflow threshold - base**(emin-1)
X*     emax  = largest exponent before overflow
X*     rmax  = overflow threshold  - (base**emax)*(1-eps)
X*
X*
X*     .. Parameters ..
X      DOUBLE PRECISION   ONE, ZERO
X      PARAMETER          ( ONE = 1.0D+0, ZERO = 0.0D+0 )
X*     ..
X*     .. Local Scalars ..
X      LOGICAL            FIRST, LRND
X      INTEGER            BETA, IMAX, IMIN, IT
X      DOUBLE PRECISION   BASE, EMAX, EMIN, EPS, RMACH, RMAX, RMIN, RND,
X     $                   SFMIN, SMALL, T
X*     ..
X*     .. Save statement ..
X      SAVE               FIRST, EPS, SFMIN, BASE, T, RND, EMIN, RMIN,
X     $                   EMAX, RMAX
X*     ..
X*     .. External Subroutines ..
X      EXTERNAL           DLAMC2
X*     ..
X*     .. Data statements ..
X      DATA               FIRST / .TRUE. /
X*     ..
X*     .. Executable Statements ..
X      IF( FIRST ) THEN
X         FIRST = .FALSE.
X         CALL DLAMC2( BETA, IT, LRND, EPS, IMIN, RMIN, IMAX, RMAX )
X         BASE = BETA
X         T = IT
X         IF( LRND ) THEN
X            RND = ONE
X            EPS = ( BASE**( 1-IT ) ) / 2
X         ELSE
X            RND = ZERO
X            EPS = BASE**( 1-IT )
X         END IF
X         EMIN = IMIN
X         EMAX = IMAX
X         SFMIN = RMIN
X         SMALL = ONE / RMAX
X         IF( SMALL.GE.SFMIN ) THEN
X*
X*           Use SMALL plus a bit, to avoid the possibility of rounding
X*           causing overflow when computing  1/sfmin.
X*
X            SFMIN = SMALL*( ONE+EPS )
X         END IF
X      END IF
X*
X      IF( ( CMACH.EQ.'E' ) .OR. ( CMACH.EQ.'e' ) ) THEN
X         RMACH = EPS
X      ELSE IF( ( CMACH.EQ.'S' ) .OR. ( CMACH.EQ.'s' ) ) THEN
X         RMACH = SFMIN
X      ELSE IF( ( CMACH.EQ.'B' ) .OR. ( CMACH.EQ.'b' ) ) THEN
X         RMACH = BASE
X      ELSE IF( ( CMACH.EQ.'N' ) .OR. ( CMACH.EQ.'n' ) ) THEN
X         RMACH = T
X      ELSE IF( ( CMACH.EQ.'R' ) .OR. ( CMACH.EQ.'r' ) ) THEN
X         RMACH = RND
X      ELSE IF( ( CMACH.EQ.'M' ) .OR. ( CMACH.EQ.'m' ) ) THEN
X         RMACH = EMIN
X      ELSE IF( ( CMACH.EQ.'U' ) .OR. ( CMACH.EQ.'u' ) ) THEN
X         RMACH = RMIN
X      ELSE IF( ( CMACH.EQ.'L' ) .OR. ( CMACH.EQ.'l' ) ) THEN
X         RMACH = EMAX
X      ELSE IF( ( CMACH.EQ.'O' ) .OR. ( CMACH.EQ.'o' ) ) THEN
X         RMACH = RMAX
X      END IF
X*
X      DLAMCH = RMACH
X      RETURN
X*
X*     End of DLAMCH
X*
X      END
X*
X************************************************************************
X*
X      SUBROUTINE DLAMC1( BETA, T, RND, IEEE1 )
X*
X*     DLAMC1 returns the machine parameters given by:
X*
X*        BETA - INTEGER.
X*               The base of the machine.
X*
X*        T    - INTEGER.
X*               The number of ( BETA ) digits in the mantissa.
X*
X*        RND  - LOGICAL.
X*               Whether  proper  rounding  ( RND = .TRUE. )  or chopping
X*               ( RND = .FALSE. )  occurs in addition. This may not be a
X*               reliable guide  to the  way in which the machine perfoms
X*               its arithmetic.
X*
X*       IEEE1 - LOGICAL.
X*               Whether  rounding appears to be done in the IEEE  'round
X*               to nearest' style.
X*
X*     The  routine  is  based  on  the  routine  ENVRON  by  Malcolm and
X*     incorporates suggestions by Gentleman and Marovich. See
X*
X*        Malcolm M. A. (1972) Algorithms to reveal properties of
X*           floating-point arithmetic. Comms. of the ACM, 15, 949-951.
X*
X*        Gentleman W. M. and Marovich S. B. (1974) More on algorithms
X*           that reveal properties of floating point arithmetic units.
X*           Comms. of the ACM, 17, 276-277.
X*
X*
X*     .. Scalar Arguments ..
X      LOGICAL            IEEE1, RND
X      INTEGER            BETA, T
X*     ..
X*     .. Local Scalars ..
X      LOGICAL            FIRST, LIEEE1, LRND
X      INTEGER            LBETA, LT
X      DOUBLE PRECISION   A, B, C, F, ONE, QTR, SAVEC, T1, T2
X*     ..
X*     .. External Functions ..
X      DOUBLE PRECISION   DLAMC3
X      EXTERNAL           DLAMC3
X*     ..
X*     .. Save statement ..
X      SAVE               FIRST, LIEEE1, LBETA, LRND, LT
X*     ..
X*     .. Data statements ..
X*
X      DATA               FIRST / .TRUE. /
X*     ..
X*     .. Executable Statements ..
X*
X      IF( FIRST ) THEN
X         FIRST = .FALSE.
X         ONE = 1
X*
X*        LBETA,  LIEEE1,  LT and  LRND  are the  local values  of  BETA,
X*        IEEE1, T and RND.
X*
X*        Throughout this routine  we use the function  DLAMC3  to ensure
X*        that relevant values are  stored and not held in registers,  or
X*        are not affected by optimizers.
X*
X*        Compute  a = 2.0**m  with the  smallest positive integer m such
X*        that
X*
X*           fl( a + 1.0 ) = a.
X*
X         A = 1
X         C = 1
X*
X*+       WHILE( C.EQ.ONE )LOOP
X   10    CONTINUE
X         IF( C.EQ.ONE ) THEN
X            A = 2*A
X            C = DLAMC3( A, ONE )
X            C = DLAMC3( C, -A )
X            GO TO 10
X         END IF
X*+       END WHILE
X*
X*        Now compute  b = 2.0**m  with the smallest positive integer m
X*        such that
X*
X*           fl( a + b ) .gt. a.
X*
X         B = 1
X         C = DLAMC3( A, B )
X*
X*+       WHILE( C.EQ.A )LOOP
X   20    CONTINUE
X         IF( C.EQ.A ) THEN
X            B = 2*B
X            C = DLAMC3( A, B )
X            GO TO 20
X         END IF
X*+       END WHILE
X*
X*        Now compute the base.  a and c  are neighbouring floating point
X*        numbers  in the  interval  ( beta**t, beta**( t + 1 ) )  and so
X*        their difference is beta. Adding 0.25 to c is to ensure that it
X*        is truncated to beta and not ( beta - 1 ).
X*
X         QTR = ONE / 4
X         SAVEC = C
X         C = DLAMC3( C, -A )
X         LBETA = C + QTR
X*
X*        Now determine whether rounding or chopping occurs,  by adding a
X*        bit  less  than  beta/2  and a  bit  more  than  beta/2  to  a.
X*
X         B = LBETA
X         F = DLAMC3( B/2, -B/100 )
X         C = DLAMC3( F, A )
X         IF( C.EQ.A ) THEN
X            LRND = .TRUE.
X         ELSE
X            LRND = .FALSE.
X         END IF
X         F = DLAMC3( B/2, B/100 )
X         C = DLAMC3( F, A )
X         IF( ( LRND ) .AND. ( C.EQ.A ) )
X     $      LRND = .FALSE.
X*
X*        Try and decide whether rounding is done in the  IEEE  'round to
X*        nearest' style. B/2 is half a unit in the last place of the two
X*        numbers A and SAVEC. Furthermore, A is even, i.e. has last  bit
X*        zero, and SAVEC is odd. Thus adding B/2 to A should not  change
X*        A, but adding B/2 to SAVEC should change SAVEC.
X*
X         T1 = DLAMC3( B/2, A )
X         T2 = DLAMC3( B/2, SAVEC )
X         LIEEE1 = ( T1.EQ.A ) .AND. ( T2.GT.SAVEC ) .AND. LRND
X*
X*        Now find  the  mantissa, t.  It should  be the  integer part of
X*        log to the base beta of a,  however it is safer to determine  t
X*        by powering.  So we find t as the smallest positive integer for
X*        which
X*
X*           fl( beta**t + 1.0 ) = 1.0.
X*
X         LT = 0
X         A = 1
X         C = 1
X*
X*+       WHILE( C.EQ.ONE )LOOP
X   30    CONTINUE
X         IF( C.EQ.ONE ) THEN
X            LT = LT + 1
X            A = A*LBETA
X            C = DLAMC3( A, ONE )
X            C = DLAMC3( C, -A )
X            GO TO 30
X         END IF
X*+       END WHILE
X*
X      END IF
X*
X      BETA = LBETA
X      T = LT
X      RND = LRND
X      IEEE1 = LIEEE1
X      RETURN
X*
X*     End of DLAMC1
X*
X      END
X*
X************************************************************************
X*
X      SUBROUTINE DLAMC2( BETA, T, RND, EPS, EMIN, RMIN, EMAX, RMAX )
X*
X*     DLAMC2 returns the machine parameters given by:
X*
X*        BETA - INTEGER.
X*               The base of the machine.
X*
X*        T    - INTEGER.
X*               The number of ( BETA ) digits in the mantissa.
X*
X*        RND  - LOGICAL.
X*               Whether  proper  rounding  ( RND = .TRUE. )  or chopping
X*               ( RND = .FALSE. )  occurs in addition. This may not be a
X*               reliable guide  to the  way in which the machine perfoms
X*               its arithmetic.
X*
X*        EPS  - DOUBLE PRECISION.
X*               The smallest positive number such that
X*
X*                  fl( 1.0 - EPS ) .LT. 1.0,
X*
X*               where fl denotes the computed value.
X*
X*        EMIN - INTEGER.
X*               The minimum exponent before (gradual) underflow occurs.
X*
X*        RMIN - DOUBLE PRECISION.
X*               The smallest normalized number for the machine given by
X*               BASE**( EMIN - 1 ),  where  BASE  is the floating point
X*               value of BETA.
X*
X*        EMAX - INTEGER.
X*               The maximum exponent before overflow occurs.
X*
X*        RMAX - DOUBLE PRECISION.
X*               The  largest  positive number for the  machine given by
X*               BASE**EMAX * ( 1 - EPS ),  where  BASE  is the floating
X*               point value of BETA.
X*
X*
X*     The  computation  of  EPS  is based  on  a  routine,  PARANOIA by
X*     W. Kahan of the University of California at Berkeley.
X*
X*
X*     .. Scalar Arguments ..
X      LOGICAL            RND
X      INTEGER            BETA, EMAX, EMIN, T
X      DOUBLE PRECISION   EPS, RMAX, RMIN
X*     ..
X*     .. Local Scalars ..
X      LOGICAL            FIRST, IEEE, IWARN, LIEEE1, LRND
X      INTEGER            GNMIN, GPMIN, I, LBETA, LEMAX, LEMIN, LT,
X     $                   NGNMIN, NGPMIN
X      DOUBLE PRECISION   A, B, C, HALF, LEPS, LRMAX, LRMIN, ONE, RBASE,
X     $                   SIXTH, SMALL, THIRD, TWO, ZERO
X*     ..
X*     .. External Functions ..
X      DOUBLE PRECISION   DLAMC3
X      EXTERNAL           DLAMC3
X*     ..
X*     .. External Subroutines ..
X      EXTERNAL           DLAMC1, DLAMC4, DLAMC5
X*     ..
X*     .. Intrinsic Functions ..
X      INTRINSIC          ABS, MAX, MIN
X*     ..
X*     .. Save statement ..
X      SAVE               FIRST, IWARN, LBETA, LEMAX, LEMIN, LEPS, LRMAX,
X     $                   LRMIN, LT
X*     ..
X*     .. Data statements ..
X      DATA               FIRST / .TRUE. / , IWARN / .FALSE. /
X*     ..
X*     .. Executable Statements ..
X*
X      IF( FIRST ) THEN
X         FIRST = .FALSE.
X         ZERO = 0
X         ONE = 1
X         TWO = 2
X*
X*        LBETA, LT, LRND, LEPS, LEMIN and LRMIN  are the local values of
X*        BETA, T, RND, EPS, EMIN and RMIN.
X*
X*        Throughout this routine  we use the function  DLAMC3  to ensure
X*        that relevant values are stored  and not held in registers,  or
X*        are not affected by optimizers.
X*
X*        DLAMC1 returns the parameters  LBETA, LT, LRND and LIEEE1.
X*
X         CALL DLAMC1( LBETA, LT, LRND, LIEEE1 )
X*
X*        Start to find EPS.
X*
X         B = LBETA
X         A = B**( -LT )
X         LEPS = A
X*
X*        Try some tricks to see whether or not this is the correct  EPS.
X*
X         B = TWO / 3
X         HALF = ONE / 2
X         SIXTH = DLAMC3( B, -HALF )
X         THIRD = DLAMC3( SIXTH, SIXTH )
X         B = DLAMC3( THIRD, -HALF )
X         B = DLAMC3( B, SIXTH )
X         B = ABS( B )
X         IF( B.LT.LEPS )
X     $      B = LEPS
X*
X         LEPS = 1
X*
X*+       WHILE( ( LEPS.GT.B ).AND.( B.GT.ZERO ) )LOOP
X   10    CONTINUE
X         IF( ( LEPS.GT.B ) .AND. ( B.GT.ZERO ) ) THEN
X            LEPS = B
X            C = DLAMC3( HALF*LEPS, ( TWO**5 )*( LEPS**2 ) )
X            C = DLAMC3( HALF, -C )
X            B = DLAMC3( HALF, C )
X            C = DLAMC3( HALF, -B )
X            B = DLAMC3( HALF, C )
X            GO TO 10
X         END IF
X*+       END WHILE
X*
X         IF( A.LT.LEPS )
X     $      LEPS = A
X*
X*        Computation of EPS complete.
X*
X*        Now find  EMIN.  Let A = + or - 1, and + or - (1 + BASE**(-3)).
X*        Keep dividing  A by BETA until (gradual) underflow occurs. This
X*        is detected when we cannot recover the previous A.
X*
X         RBASE = ONE / LBETA
X         SMALL = ONE
X         DO 20 I = 1, 3
X            SMALL = DLAMC3( SMALL*RBASE, ZERO )
X   20    CONTINUE
X         A = DLAMC3( ONE, SMALL )
X         CALL DLAMC4( NGPMIN, ONE, LBETA )
X         CALL DLAMC4( NGNMIN, -ONE, LBETA )
X         CALL DLAMC4( GPMIN, A, LBETA )
X         CALL DLAMC4( GNMIN, -A, LBETA )
X         IEEE = .FALSE.
X*
X         IF( ( NGPMIN.EQ.NGNMIN ) .AND. ( GPMIN.EQ.GNMIN ) ) THEN
X            IF( NGPMIN.EQ.GPMIN ) THEN
X               LEMIN = NGPMIN
X*            ( Non twos-complement machines, no gradual underflow;
X*              e.g.,  VAX )
X            ELSE IF( ( GPMIN-NGPMIN ).EQ.3 ) THEN
X               LEMIN = NGPMIN - 1 + LT
X               IEEE = .TRUE.
X*            ( Non twos-complement machines, with gradual underflow;
X*              e.g., IEEE standard followers )
X            ELSE
X               LEMIN = MIN( NGPMIN, GPMIN )
X*            ( A guess; no known machine )
X               IWARN = .TRUE.
X            END IF
X*
X         ELSE IF( ( NGPMIN.EQ.GPMIN ) .AND. ( NGNMIN.EQ.GNMIN ) ) THEN
X            IF( ABS( NGPMIN-NGNMIN ).EQ.1 ) THEN
X               LEMIN = MAX( NGPMIN, NGNMIN )
X*            ( Twos-complement machines, no gradual underflow;
X*              e.g., CYBER 205 )
X            ELSE
X               LEMIN = MIN( NGPMIN, NGNMIN )
X*            ( A guess; no known machine )
X               IWARN = .TRUE.
X            END IF
X*
X         ELSE IF( ( ABS( NGPMIN-NGNMIN ).EQ.1 ) .AND.
X     $            ( GPMIN.EQ.GNMIN ) ) THEN
X            IF( ( GPMIN-MIN( NGPMIN, NGNMIN ) ).EQ.3 ) THEN
X               LEMIN = MAX( NGPMIN, NGNMIN ) - 1 + LT
X*            ( Twos-complement machines with gradual underflow;
X*              no known machine )
X            ELSE
X               LEMIN = MIN( NGPMIN, NGNMIN )
X*            ( A guess; no known machine )
X               IWARN = .TRUE.
X            END IF
X*
X         ELSE
X            LEMIN = MIN( NGPMIN, NGNMIN, GPMIN, GNMIN )
X*         ( A guess; no known machine )
X            IWARN = .TRUE.
X         END IF
X***
X* Comment out this if block if EMIN is ok
X         IF( IWARN ) THEN
X            FIRST = .TRUE.
XC           WRITE( 6, FMT = 9999 )LEMIN
X         END IF
X***
X*
X*        Assume IEEE arithmetic if we found denormalised  numbers above,
X*        or if arithmetic seems to round in the  IEEE style,  determined
X*        in routine DLAMC1. A true IEEE machine should have both  things
X*        true; however, faulty machines may have one or the other.
X*
X         IEEE = IEEE .OR. LIEEE1
X*
X*        Compute  RMIN by successive division by  BETA. We could compute
X*        RMIN as BASE**( EMIN - 1 ),  but some machines underflow during
X*        this computation.
X*
X         LRMIN = 1
X         DO 30 I = 1, 1 - LEMIN
X            LRMIN = DLAMC3( LRMIN*RBASE, ZERO )
X   30    CONTINUE
X*
X*        Finally, call DLAMC5 to compute EMAX and RMAX.
X*
X         CALL DLAMC5( LBETA, LT, LEMIN, IEEE, LEMAX, LRMAX )
X      END IF
X*
X      BETA = LBETA
X      T = LT
X      RND = LRND
X      EPS = LEPS
X      EMIN = LEMIN
X      RMIN = LRMIN
X      EMAX = LEMAX
X      RMAX = LRMAX
X*
X      RETURN
X*
X 9999 FORMAT( //' WARNING. The value EMIN may be incorrect:-',
X     $      '  EMIN = ', I8, /
X     $      ' If, after inspection, the value EMIN looks',
X     $      ' acceptable please comment out ',
X     $      /' the IF block as marked within the code of routine',
X     $      ' DLAMC2,', /' otherwise supply EMIN explicitly.', / )
X*
X*     End of DLAMC2
X*
X      END
X*
X************************************************************************
X*
X      DOUBLE PRECISION FUNCTION DLAMC3( A, B )
X*     .. Scalar Arguments ..
X      DOUBLE PRECISION   A, B
X*     ..
X*     .. Executable Statements ..
X*
X*     DLAMC3  is intended to force  A and B  to be stored prior to doing
X*     the addition of  A and B.  For use in situations where  optimizers
X*     might hold one of these in a register.
X*
X*
X      DLAMC3 = A + B
X*
X      RETURN
X*
X*     End of DLAMC3
X*
X      END
X*
X************************************************************************
X*
X      SUBROUTINE DLAMC4( EMIN, START, BASE )
X*
X*     Service routine for DLAMC2.
X*
X*
X*     .. Scalar Arguments ..
X      INTEGER            BASE, EMIN
X      DOUBLE PRECISION   START
X*     ..
X*     .. Local Scalars ..
X      INTEGER            I
X      DOUBLE PRECISION   A, B1, B2, C1, C2, D1, D2, ONE, RBASE, ZERO
X*     ..
X*     .. External Functions ..
X      DOUBLE PRECISION   DLAMC3
X      EXTERNAL           DLAMC3
X*     ..
X*     .. Executable Statements ..
X*
X      A = START
X      ONE = 1
X      RBASE = ONE / BASE
X      ZERO = 0
X      EMIN = 1
X      B1 = DLAMC3( A*RBASE, ZERO )
X      C1 = A
X      C2 = A
X      D1 = A
X      D2 = A
X*+    WHILE( ( C1.EQ.A ).AND.( C2.EQ.A ).AND.
X*    $       ( D1.EQ.A ).AND.( D2.EQ.A )      )LOOP
X   10 CONTINUE
X      IF( ( C1.EQ.A ) .AND. ( C2.EQ.A ) .AND. ( D1.EQ.A ) .AND.
X     $    ( D2.EQ.A ) ) THEN
X         EMIN = EMIN - 1
X         A = B1
X         B1 = DLAMC3( A/BASE, ZERO )
X         C1 = DLAMC3( B1*BASE, ZERO )
X         D1 = ZERO
X         DO 20 I = 1, BASE
X            D1 = D1 + B1
X   20    CONTINUE
X         B2 = DLAMC3( A*RBASE, ZERO )
X         C2 = DLAMC3( B2/RBASE, ZERO )
X         D2 = ZERO
X         DO 30 I = 1, BASE
X            D2 = D2 + B2
X   30    CONTINUE
X         GO TO 10
X      END IF
X*+    END WHILE
X*
X      RETURN
X*
X*     End of DLAMC4
X*
X      END
X*
X************************************************************************
X*
X      SUBROUTINE DLAMC5( BETA, P, EMIN, IEEE, EMAX, RMAX )
X*
X*     Given BETA, the base of floating-point arithmetic, P, the
X*     number of base BETA digits in the mantissa of a floating-point
X*     value, EMIN, the minimum exponent, and IEEE, a logical
X*     flag saying whether or not the arithmetic system is thought
X*     to comply with the IEEE standard, this routine attempts to
X*     compute RMAX, the largest machine floating-point number,
X*     without overflow. The routine assumes that EMAX + abs(EMIN)
X*     sum approximately to a power of 2. It will fail on machines
X*     where this assumption does not hold, for example the Cyber 205
X*     (EMIN = -28625, EMAX = 28718). It will also fail if the value
X*     supplied for EMIN is too large (i.e. too close to zero),
X*     probably with overflow.
X*
X*
X*     .. Parameters ..
X      DOUBLE PRECISION   ZERO, ONE
X      PARAMETER          ( ZERO = 0.0D0, ONE = 1.0D0 )
X*     ..
X*     .. Scalar Arguments ..
X      LOGICAL            IEEE
X      INTEGER            BETA, EMAX, EMIN, P
X      DOUBLE PRECISION   RMAX
X*     ..
X*     .. Local Scalars ..
X      INTEGER            EXBITS, EXPSUM, I, LEXP, NBITS, TRY, UEXP
X      DOUBLE PRECISION   OLDY, RECBAS, Y, Z
X*     ..
X*     .. External Functions ..
X      DOUBLE PRECISION   DLAMC3
X      EXTERNAL           DLAMC3
X*     ..
X*     .. Intrinsic Functions ..
X      INTRINSIC          MOD
X*     ..
X*     .. Executable Statements ..
X*
X*     First compute LEXP and UEXP, two powers of 2 that bound
X*     abs(EMIN). We then assume that EMAX + abs(EMIN) will sum
X*     approximately to the bound that is closest to abs(EMIN).
X*     (EMAX is the exponent of the required number RMAX).
X*
X      LEXP = 1
X      EXBITS = 1
X   10 CONTINUE
X      TRY = LEXP*2
X      IF( TRY.LE.( -EMIN ) ) THEN
X         LEXP = TRY
X         EXBITS = EXBITS + 1
X         GO TO 10
X      END IF
X      IF( LEXP.EQ.-EMIN ) THEN
X         UEXP = LEXP
X      ELSE
X         UEXP = TRY
X         EXBITS = EXBITS + 1
X      END IF
X*
X*     Now -LEXP is less than or equal to EMIN, and -UEXP is greater
X*     than or equal to EMIN. EXBITS is the number of bits needed to
X*     store the exponent.
X*
X      IF( ( UEXP+EMIN ).GT.( -LEXP-EMIN ) ) THEN
X         EXPSUM = 2*LEXP
X      ELSE
X         EXPSUM = 2*UEXP
X      END IF
X*
X*     EXPSUM is the exponent range, approximately equal to
X*     EMAX - EMIN + 1 .
X*
X      EMAX = EXPSUM + EMIN - 1
X      NBITS = 1 + EXBITS + P
X*
X*     NBITS is the total number of bits needed to store a
X*     floating-point number.
X*
X      IF( ( MOD( NBITS, 2 ).EQ.1 ) .AND. ( BETA.EQ.2 ) ) THEN
X*
X*        Either there are an odd number of bits used to store a
X*        floating-point number, which is unlikely, or some bits are
X*        not used in the representation of numbers, which is possible,
X*        (e.g. Cray machines) or the mantissa has an implicit bit,
X*        (e.g. IEEE machines, Dec Vax machines), which is perhaps the
X*        most likely. We have to assume the last alternative.
X*        If this is true, then we need to reduce EMAX by one because
X*        there must be some way of representing zero in an implicit-bit
X*        system. On machines like Cray, we are reducing EMAX by one
X*        unnecessarily.
X*
X         EMAX = EMAX - 1
X      END IF
X*
X      IF( IEEE ) THEN
X*
X*        Assume we are on an IEEE machine which reserves one exponent
X*        for infinity and NaN.
X*
X         EMAX = EMAX - 1
X      END IF
X*
X*     Now create RMAX, the largest machine number, which should
X*     be equal to (1.0 - BETA**(-P)) * BETA**EMAX .
X*
X*     First compute 1.0 - BETA**(-P), being careful that the
X*     result is less than 1.0 .
X*
X      RECBAS = ONE / BETA
X      Z = ONE
X      Y = ZERO
X      DO 20 I = 1, P
X         Z = Z*RECBAS
X         IF( Y.LT.ONE )
X     $      OLDY = Y
X         Y = DLAMC3( Y, Z )
X   20 CONTINUE
X      IF( Y.GE.ONE )
X     $   Y = OLDY
X*
X*     Now multiply by BETA**EMAX to get RMAX.
X*
X      DO 30 I = 1, EMAX
X         Y = DLAMC3( Y*BETA, ZERO )
X   30 CONTINUE
X*
X      RMAX = Y
X      RETURN
X*
X*     End of DLAMC5
X*
X      END
END_OF_FILE
  if test 22293 -ne `wc -c <'fdmach.f'`; then
    echo shar: \"'fdmach.f'\" unpacked with wrong size!
  fi
  # end of 'fdmach.f'
fi
if test -f 'fdmin.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdmin.f'\"
else
  echo shar: Extracting \"'fdmin.f'\" \(39452 characters\)
  sed "s/^X//" >'fdmin.f' <<'END_OF_FILE'
X      subroutine lmder1(fcn,m,n,x,fvec,fjac,ldfjac,ftol,xtol,gtol,
X     *                  maxfev,diag,mode,factor,info,nfev,njev,
X     *                  ipvt,qtf,wa1,wa2,wa3,wa4,Y)
X      integer m,n,ldfjac,maxfev,mode,nprint,info,nfev,njev
X      integer ipvt(n)
X      double precision ftol,xtol,gtol,factor
X      double precision x(n),fvec(m),fjac(ldfjac,n),diag(n),qtf(n),
X     *                 wa1(n),wa2(n),wa3(n),wa4(m),Y(*)
X      external         fcn
Xc     **********
Xc
Xc     subroutine lmder
Xc
Xc     the purpose of lmder is to minimize the sum of the squares of
Xc     m nonlinear functions in n variables by a modification of
Xc     the levenberg-marquardt algorithm. the user must provide a
Xc     subroutine which calculates the functions and the jacobian.
Xc
Xc     the subroutine statement is
Xc       subroutine lmder(fcn,m,n,x,fvec,fjac,ldfjac,ftol,xtol,gtol,
Xc                        maxfev,diag,mode,factor,nprint,info,nfev,
Xc                        njev,ipvt,qtf,wa1,wa2,wa3,wa4)
Xc
Xc     where
Xc
Xc       fcn is the name of the user-supplied subroutine which
Xc         calculates the functions and the jacobian. fcn must
Xc         be declared in an external statement in the user
Xc         calling program, and should be written as follows.
Xc
Xc         subroutine fcn(m,n,x,fvec,fjac,ldfjac,iflag)
Xc         integer m,n,ldfjac,iflag
Xc         double precision x(n),fvec(m),fjac(ldfjac,n)
Xc         ----------
Xc         if iflag = 1 calculate the functions at x and
Xc         return this vector in fvec. do not alter fjac.
Xc         if iflag = 2 calculate the jacobian at x and
Xc         return this matrix in fjac. do not alter fvec.
Xc         ----------
Xc         return
Xc         end
Xc
Xc         the value of iflag should not be changed by fcn unless
Xc         the user wants to terminate execution of lmder.
Xc         in this case set iflag to a negative integer.
Xc
Xc       m is a positive integer input variable set to the number
Xc         of functions.
Xc
Xc       n is a positive integer input variable set to the number
Xc         of variables. n must not exceed m.
Xc
Xc       x is an array of length n. on input x must contain
Xc         an initial estimate of the solution vector. on output x
Xc         contains the final estimate of the solution vector.
Xc
Xc       fvec is an output array of length m which contains
Xc         the functions evaluated at the output x.
Xc
Xc       fjac is an output m by n array. the upper n by n submatrix
Xc         of fjac contains an upper triangular matrix r with
Xc         diagonal elements of nonincreasing magnitude such that
Xc
Xc                t     t           t
Xc               p *(jac *jac)*p = r *r,
Xc
Xc         where p is a permutation matrix and jac is the final
Xc         calculated jacobian. column j of p is column ipvt(j)
Xc         (see below) of the identity matrix. the lower trapezoidal
Xc         part of fjac contains information generated during
Xc         the computation of r.
Xc
Xc       ldfjac is a positive integer input variable not less than m
Xc         which specifies the leading dimension of the array fjac.
Xc
Xc       ftol is a nonnegative input variable. termination
Xc         occurs when both the actual and predicted relative
Xc         reductions in the sum of squares are at most ftol.
Xc         therefore, ftol measures the relative error desired
Xc         in the sum of squares.
Xc
Xc       xtol is a nonnegative input variable. termination
Xc         occurs when the relative error between two consecutive
Xc         iterates is at most xtol. therefore, xtol measures the
Xc         relative error desired in the approximate solution.
Xc
Xc       gtol is a nonnegative input variable. termination
Xc         occurs when the cosine of the angle between fvec and
Xc         any column of the jacobian is at most gtol in absolute
Xc         value. therefore, gtol measures the orthogonality
Xc         desired between the function vector and the columns
Xc         of the jacobian.
Xc
Xc       maxfev is a positive integer input variable. termination
Xc         occurs when the number of calls to fcn with iflag = 1
Xc         has reached maxfev.
Xc
Xc       diag is an array of length n. if mode = 1 (see
Xc         below), diag is internally set. if mode = 2, diag
Xc         must contain positive entries that serve as
Xc         multiplicative scale factors for the variables.
Xc
Xc       mode is an integer input variable. if mode = 1, the
Xc         variables will be scaled internally. if mode = 2,
Xc         the scaling is specified by the input diag. other
Xc         values of mode are equivalent to mode = 1.
Xc
Xc       factor is a positive input variable used in determining the
Xc         initial step bound. this bound is set to the product of
Xc         factor and the euclidean norm of diag*x if nonzero, or else
Xc         to factor itself. in most cases factor should lie in the
Xc         interval (.1,100.).100. is a generally recommended value.
Xc
Xc       nprint is an integer input variable that enables controlled
Xc         printing of iterates if it is positive. in this case,
Xc         fcn is called with iflag = 0 at the beginning of the first
Xc         iteration and every nprint iterations thereafter and
Xc         immediately prior to return, with x, fvec, and fjac
Xc         available for printing. fvec and fjac should not be
Xc         altered. if nprint is not positive, no special calls
Xc         of fcn with iflag = 0 are made.
Xc
Xc       info is an integer output variable. if the user has
Xc         terminated execution, info is set to the (negative)
Xc         value of iflag. see description of fcn. otherwise,
Xc         info is set as follows.
Xc
Xc         info = 0  improper input parameters.
Xc
Xc         info = 1  both actual and predicted relative reductions
Xc                   in the sum of squares are at most ftol.
Xc
Xc         info = 2  relative error between two consecutive iterates
Xc                   is at most xtol.
Xc
Xc         info = 3  conditions for info = 1 and info = 2 both hold.
Xc
Xc         info = 4  the cosine of the angle between fvec and any
Xc                   column of the jacobian is at most gtol in
Xc                   absolute value.
Xc
Xc         info = 5  number of calls to fcn with iflag = 1 has
Xc                   reached maxfev.
Xc
Xc         info = 6  ftol is too small. no further reduction in
Xc                   the sum of squares is possible.
Xc
Xc         info = 7  xtol is too small. no further improvement in
Xc                   the approximate solution x is possible.
Xc
Xc         info = 8  gtol is too small. fvec is orthogonal to the
Xc                   columns of the jacobian to machine precision.
Xc
Xc       nfev is an integer output variable set to the number of
Xc         calls to fcn with iflag = 1.
Xc
Xc       njev is an integer output variable set to the number of
Xc         calls to fcn with iflag = 2.
Xc
Xc       ipvt is an integer output array of length n. ipvt
Xc         defines a permutation matrix p such that jac*p = q*r,
Xc         where jac is the final calculated jacobian, q is
Xc         orthogonal (not stored), and r is upper triangular
Xc         with diagonal elements of nonincreasing magnitude.
Xc         column j of p is column ipvt(j) of the identity matrix.
Xc
Xc       qtf is an output array of length n which contains
Xc         the first n elements of the vector (q transpose)*fvec.
Xc
Xc       wa1, wa2, and wa3 are work arrays of length n.
Xc
Xc       wa4 is a work array of length m.
Xc
Xc     subprograms called
Xc
Xc       user-supplied ...... fcn
Xc
Xc       minpack-supplied ... dpmpar,enorm,lmpar,qrfac
Xc
Xc       fortran-supplied ... dabs,dmax1,dmin1,dsqrt,mod
Xc
Xc     argonne national laboratory. minpack project. march 1980.
Xc     burton s. garbow, kenneth e. hillstrom, jorge j. more
Xc
Xc     **********
X      integer i,iflag,iter,j,l
X      double precision actred,dirder,fnorm1,
X     *                 one,par,pnorm,prered,p1,p5,p25,p75,p0001,ratio,
X     *                 sum,temp,temp1,temp2,xnorm,zero
Xc     double precision dpmpar,enorm
X      double precision enorm
X      data one,p1,p5,p25,p75,p0001,zero
X     *     /1.0d0,1.0d-1,5.0d-1,2.5d-1,7.5d-1,1.0d-4,0.0d0/
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, epsmch
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, epsmch
X      save   /MACHFD/
X
X      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
X      save   /MAUXFD/
X
X      double precision   told, tolf, tolx, tolg, fnorm, delta, gnorm
X      common /TOLSFD/    told, tolf, tolx, tolg, fnorm, delta, gnorm
X      save   /TOLSFD/
Xc
Xc     epsmch is the machine precision.
Xc
Xc     epsmch = dpmpar(1)
Xc
X      nprint = 0
X      info = 0
X      iflag = 0
X      nfev = 0
X      njev = 0
X
Xc     check the input parameters for errors.
Xc
X      if (n .le. 0 .or. m .lt. n .or. ldfjac .lt. m
X     *    .or. ftol .lt. zero .or. xtol .lt. zero .or. gtol .lt. zero
X     *    .or. maxfev .le. 0 .or. factor .le. zero) go to 300
X      if (mode .ne. 2) go to 20
X      do 10 j = 1, n
X         if (diag(j) .le. zero) go to 300
X   10    continue
X   20 continue
Xc
Xc     evaluate the function at the starting point
Xc     and calculate its norm.
Xc
X      iflag = 1
X      call fcn(x,fvec,fjac,ldfjac,iflag,Y)
X      nfev = 1
X      if (iflag .lt. 0) go to 300
X      fnorm = min(enorm(m,fvec),BIGNUM)
Xc
Xc     initialize levenberg-marquardt parameter and iteration counter.
Xc
X      par = zero
X      iter = 1
Xc
Xc     beginning of the outer loop.
Xc
X   30 continue
X
Xc
Xc        calculate the jacobian matrix.
Xc
X         iflag = 2
X         call fcn(x,fvec,fjac,ldfjac,iflag,Y)
X            njev = njev + 1
X         if (iflag .lt. 0) go to 300
Xc
Xc        if requested, call fcn to enable printing of iterates.
Xc
X         if (nprint .le. 0) go to 40
X         iflag = 0
X         if (mod(iter-1,nprint) .eq. 0)
X     *      call fcn(x,fvec,fjac,ldfjac,iflag,Y)
X         if (iflag .lt. 0) go to 300
X   40    continue
Xc
Xc        compute the qr factorization of the jacobian.
Xc
X         call qrfac(m,n,fjac,ldfjac,.true.,ipvt,n,wa1,wa2,wa3)
Xc
Xc        on the first iteration and if mode is 1, scale according
Xc        to the norms of the columns of the initial jacobian.
Xc
X         if (iter .ne. 1) go to 80
X         if (mode .eq. 2) go to 60
X         do 50 j = 1, n
X            diag(j) = wa2(j)
X            if (wa2(j) .eq. zero) diag(j) = one
X   50       continue
X   60    continue
Xc
Xc        on the first iteration, calculate the norm of the scaled x
Xc        and initialize the step bound delta.
Xc
X         do 70 j = 1, n
X            wa3(j) = diag(j)*x(j)
X   70       continue
X         xnorm = enorm(n,wa3)
X         delta = factor*xnorm
X         if (delta .eq. zero) delta = factor
X   80    continue
Xc
Xc        form (q transpose)*fvec and store the first n components in
Xc        qtf.
Xc
X         do 90 i = 1, m
X            wa4(i) = fvec(i)
X   90       continue
X         do 130 j = 1, n
X            if (fjac(j,j) .eq. zero) go to 120
X            sum = zero
X            do 100 i = j, m
X               sum = sum + fjac(i,j)*wa4(i)
X  100          continue
X            temp = -sum/fjac(j,j)
X            do 110 i = j, m
X               wa4(i) = wa4(i) + fjac(i,j)*temp
X  110          continue
X  120       continue
X            fjac(j,j) = wa1(j)
X            qtf(j) = wa4(j)
X  130       continue
Xc
Xc        compute the norm of the scaled gradient.
Xc
X         gnorm = zero
X         if (fnorm .eq. zero) go to 170
X         do 160 j = 1, n
X            l = ipvt(j)
X            if (wa2(l) .eq. zero) go to 150
X            sum = zero
X            do 140 i = 1, j
X               sum = sum + fjac(i,j)*(qtf(i)/fnorm)
X  140          continue
X            gnorm = dmax1(gnorm,dabs(sum/wa2(l)))
X  150       continue
X  160       continue
X  170    continue
Xc
Xc        test for convergence of the gradient norm.
Xc
X         if (gnorm .le. gtol)  info = 4
X         if (info .ne. 0) go to 300
Xc
Xc        rescale if necessary.
Xc
X         if (mode .eq. 2) go to 190
X         do 180 j = 1, n
X            diag(j) = dmax1(diag(j),wa2(j))
X  180       continue
X  190    continue
Xc
Xc        beginning of the inner loop.
Xc
X  200    continue
X
X
Xc           determine the levenberg-marquardt parameter.
Xc
X            call lmpar(n,fjac,ldfjac,ipvt,diag,qtf,delta,par,wa1,wa2,
X     *                 wa3,wa4)
Xc
Xc           store the direction p and x + p. calculate the norm of p.
Xc
X            do 210 j = 1, n
X               wa1(j) = -wa1(j)
X               wa2(j) = x(j) + wa1(j)
X               wa3(j) = diag(j)*wa1(j)
X  210          continue
X            pnorm = enorm(n,wa3)
Xc
Xc           on the first iteration, adjust the initial step bound.
Xc
X            if (iter .eq. 1) delta = dmin1(delta,pnorm)
Xc
Xc           evaluate the function at x + p and calculate its norm.
Xc
X            iflag = 1
X            call fcn(wa2,wa4,fjac,ldfjac,iflag,Y)
X            nfev = nfev + 1
X            if (iflag .lt. 0) go to 300
X            fnorm1 = min(enorm(m,wa4),BIGNUM)
Xc
Xc           compute the scaled actual reduction.
Xc
X           actred = -one
X           if (p1*fnorm1 .lt. fnorm) actred = one - (fnorm1/fnorm)**2
XC          actred = (fnorm*fnorm - fnorm1*fnorm1) 
Xc
Xc           compute the scaled predicted reduction and
Xc           the scaled directional derivative.
Xc
X            do 230 j = 1, n
X               wa3(j) = zero
X               l = ipvt(j)
X               temp = wa1(l)
X               do 220 i = 1, j
X                  wa3(i) = wa3(i) + fjac(i,j)*temp
X  220             continue
X  230          continue
X            temp1 = enorm(n,wa3)/fnorm
X            temp2 = (dsqrt(par)*pnorm)/fnorm
X            prered = temp1**2 + temp2**2/p5
XC           temp1  = enorm(n,wa3)
XC           temp2  = (dsqrt(par)*pnorm)
XC           prered = (temp1**2 + 2.d0*temp2**2) 
X            dirder = -(temp1**2 + temp2**2)
Xc
Xc           compute the ratio of the actual to the predicted
Xc           reduction.
Xc
X            ratio = zero
X            if (prered .ne. zero) ratio = actred/prered
Xc
Xc           update the step bound.
Xc
X            if (ratio .gt. p25) go to 240
X               if (actred .ge. zero) temp = p5
X               if (actred .lt. zero)
X     *            temp = p5*dirder/(dirder + p5*actred)
X               if (p1*fnorm1 .ge. fnorm .or. temp .lt. p1) temp = p1
X               delta = temp*dmin1(delta,pnorm/p1)
X               par = par/temp
X               go to 260
X  240       continue
X               if (par .ne. zero .and. ratio .lt. p75) go to 250
X               delta = pnorm/p5
X               par = p5*par
X  250          continue
X  260       continue
Xc
Xc           test for successful iteration.
Xc
X            if (ratio .lt. p0001) go to 290
Xc
Xc           successful iteration. update x, fvec, and their norms.
Xc
Xc
X            do 270 j = 1, n
X               x(j) = wa2(j)
X               wa2(j) = diag(j)*x(j)
X  270          continue
X            do 280 i = 1, m
X               fvec(i) = wa4(i)
X  280          continue
X            xnorm = enorm(n,wa2)
X            fnorm = fnorm1
X            iter = iter + 1
X  290       continue
Xc
Xc           tests for convergence.
Xc
X            if (dabs(actred) .le. ftol .and. prered .le. ftol
X     *          .and. p5*ratio .le. one) info = 1
X            if (fnorm  .le. ftol) info = 1
X            if (delta  .le. xtol) info = 2
X            if (dabs(actred) .le. ftol .and. prered .le. ftol
X     *          .and. p5*ratio .le. one .and. info .eq. 2) info = 3
X            if (info .ne. 0) go to 300
Xc
Xc           tests for termination and stringent tolerances.
Xc
X            if (nfev .ge. maxfev) info = 5
X            if (dabs(actred) .le. epsmch .and. prered .le. epsmch
X     *          .and. p5*ratio .le. one) info = 6
X            if (delta .le. epsmch) info = 7
X            if (gnorm .le. epsmch) info = 8
X            if (info .ne. 0) go to 300
Xc
Xc           end of the inner loop. repeat if iteration unsuccessful.
Xc
X            if (ratio .lt. p0001) go to 200
Xc
Xc        end of the outer loop.
Xc
X         go to 30
X  300 continue
Xc
Xc     termination, either normal or user imposed.
Xc
X      if (iflag .lt. 0) info = iflag
X      iflag = 0
X      if (nprint .gt. 0) call fcn(x,fvec,fjac,ldfjac,iflag,Y)
X      return
Xc
Xc     last card of subroutine lmder.
Xc
X      end
X
X      double precision function enorm(n,x)
X      integer n
X      double precision x(n)
Xc     **********
Xc
Xc     function enorm
Xc
Xc     given an n-vector x, this function calculates the
Xc     euclidean norm of x.
Xc
Xc     the euclidean norm is computed by accumulating the sum of
Xc     squares in three different sums. the sums of squares for the
Xc     small and large components are scaled so that no overflows
Xc     occur. non-destructive underflows are permitted. underflows
Xc     and overflows do not occur in the computation of the unscaled
Xc     sum of squares for the intermediate components.
Xc     the definitions of small, intermediate and large components
Xc     depend on two constants, rdwarf and rgiant. the main
Xc     restrictions on these constants are that rdwarf**2 not
Xc     underflow and rgiant**2 not overflow. the constants
Xc     given here are suitable for every known computer.
Xc
Xc     the function statement is
Xc
Xc       double precision function enorm(n,x)
Xc
Xc     where
Xc
Xc       n is a positive integer input variable.
Xc
Xc       x is an input array of length n.
Xc
Xc     subprograms called
Xc
Xc       fortran-supplied ... dabs,dsqrt
Xc
Xc     argonne national laboratory. minpack project. march 1980.
Xc     burton s. garbow, kenneth e. hillstrom, jorge j. more
Xc
Xc     **********
X      integer i
X      double precision agiant,floatn,one,rdwarf,rgiant,s1,s2,s3,xabs,
X     *                 x1max,x3max,zero
X      data one,zero,rdwarf,rgiant /1.0d0,0.0d0,3.834d-20,1.304d19/
X      s1 = zero
X      s2 = zero
X      s3 = zero
X      x1max = zero
X      x3max = zero
X      floatn = n
X      agiant = rgiant/floatn
X      do 90 i = 1, n
X         xabs = dabs(x(i))
X         if (xabs .gt. rdwarf .and. xabs .lt. agiant) go to 70
X            if (xabs .le. rdwarf) go to 30
Xc
Xc              sum for large components.
Xc
X               if (xabs .le. x1max) go to 10
X                  s1 = one + s1*(x1max/xabs)**2
X                  x1max = xabs
X                  go to 20
X   10          continue
X                  s1 = s1 + (xabs/x1max)**2
X   20          continue
X               go to 60
X   30       continue
Xc
Xc              sum for small components.
Xc
X               if (xabs .le. x3max) go to 40
X                  s3 = one + s3*(x3max/xabs)**2
X                  x3max = xabs
X                  go to 50
X   40          continue
X                  if (xabs .ne. zero) s3 = s3 + (xabs/x3max)**2
X   50          continue
X   60       continue
X            go to 80
X   70    continue
Xc
Xc           sum for intermediate components.
Xc
X            s2 = s2 + xabs**2
X   80    continue
X   90    continue
Xc
Xc     calculation of norm.
Xc
X      if (s1 .eq. zero) go to 100
X         enorm = x1max*dsqrt(s1+(s2/x1max)/x1max)
X         go to 130
X  100 continue
X         if (s2 .eq. zero) go to 110
X            if (s2 .ge. x3max)
X     *         enorm = dsqrt(s2*(one+(x3max/s2)*(x3max*s3)))
X            if (s2 .lt. x3max)
X     *         enorm = dsqrt(x3max*((s2/x3max)+(x3max*s3)))
X            go to 120
X  110    continue
X            enorm = x3max*dsqrt(s3)
X  120    continue
X  130 continue
X      return
Xc
Xc     last card of function enorm.
Xc
X      end
X      subroutine qrfac(m,n,a,lda,pivot,ipvt,lipvt,rdiag,acnorm,wa)
X      integer m,n,lda,lipvt
X      integer ipvt(lipvt)
X      logical pivot
X      double precision a(lda,n),rdiag(n),acnorm(n),wa(n)
Xc     **********
Xc
Xc     subroutine qrfac
Xc
Xc     this subroutine uses householder transformations with column
Xc     pivoting (optional) to compute a qr factorization of the
Xc     m by n matrix a. that is, qrfac determines an orthogonal
Xc     matrix q, a permutation matrix p, and an upper trapezoidal
Xc     matrix r with diagonal elements of nonincreasing magnitude,
Xc     such that a*p = q*r. the householder transformation for
Xc     column k, k = 1,2,...,min(m,n), is of the form
Xc
Xc                           t
Xc           i - (1/u(k))*u*u
Xc
Xc     where u has zeros in the first k-1 positions. the form of
Xc     this transformation and the method of pivoting first
Xc     appeared in the corresponding linpack subroutine.
Xc
Xc     the subroutine statement is
Xc
Xc       subroutine qrfac(m,n,a,lda,pivot,ipvt,lipvt,rdiag,acnorm,wa)
Xc
Xc     where
Xc
Xc       m is a positive integer input variable set to the number
Xc         of rows of a.
Xc
Xc       n is a positive integer input variable set to the number
Xc         of columns of a.
Xc
Xc       a is an m by n array. on input a contains the matrix for
Xc         which the qr factorization is to be computed. on output
Xc         the strict upper trapezoidal part of a contains the strict
Xc         upper trapezoidal part of r, and the lower trapezoidal
Xc         part of a contains a factored form of q (the non-trivial
Xc         elements of the u vectors described above).
Xc
Xc       lda is a positive integer input variable not less than m
Xc         which specifies the leading dimension of the array a.
Xc
Xc       pivot is a logical input variable. if pivot is set true,
Xc         then column pivoting is enforced. if pivot is set false,
Xc         then no column pivoting is done.
Xc
Xc       ipvt is an integer output array of length lipvt. ipvt
Xc         defines the permutation matrix p such that a*p = q*r.
Xc         column j of p is column ipvt(j) of the identity matrix.
Xc         if pivot is false, ipvt is not referenced.
Xc
Xc       lipvt is a positive integer input variable. if pivot is false,
Xc         then lipvt may be as small as 1. if pivot is true, then
Xc         lipvt must be at least n.
Xc
Xc       rdiag is an output array of length n which contains the
Xc         diagonal elements of r.
Xc
Xc       acnorm is an output array of length n which contains the
Xc         norms of the corresponding columns of the input matrix a.
Xc         if this information is not needed, then acnorm can coincide
Xc         with rdiag.
Xc
Xc       wa is a work array of length n. if pivot is false, then wa
Xc         can coincide with rdiag.
Xc
Xc     subprograms called
Xc
Xc       minpack-supplied ... dpmpar,enorm
Xc
Xc       fortran-supplied ... dmax1,dsqrt,min0
Xc
Xc     argonne national laboratory. minpack project. march 1980.
Xc     burton s. garbow, kenneth e. hillstrom, jorge j. more
Xc
Xc     **********
X      integer i,j,jp1,k,kmax,minmn
X      double precision ajnorm,epsmch,one,p05,sum,temp,zero
Xc     double precision dpmpar,enorm
X      double precision enorm
X      data one,p05,zero /1.0d0,5.0d-2,0.0d0/
X      double precision   FLTMIN, FLTMAX, EPSMIN
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, epsmch
X      save   /MACHFD/
Xc
Xc     epsmch is the machine precision.
Xc
Xc     epsmch = dpmpar(1)
Xc
Xc     compute the initial column norms and initialize several arrays.
Xc
X      do 10 j = 1, n
X         acnorm(j) = enorm(m,a(1,j))
X         rdiag(j) = acnorm(j)
X         wa(j) = rdiag(j)
X         if (pivot) ipvt(j) = j
X   10    continue
Xc
Xc     reduce a to r with householder transformations.
Xc
X      minmn = min0(m,n)
X      do 110 j = 1, minmn
X         if (.not.pivot) go to 40
Xc
Xc        bring the column of largest norm into the pivot position.
Xc
X         kmax = j
X         do 20 k = j, n
X            if (rdiag(k) .gt. rdiag(kmax)) kmax = k
X   20       continue
X         if (kmax .eq. j) go to 40
X         do 30 i = 1, m
X            temp = a(i,j)
X            a(i,j) = a(i,kmax)
X            a(i,kmax) = temp
X   30       continue
X         rdiag(kmax) = rdiag(j)
X         wa(kmax) = wa(j)
X         k = ipvt(j)
X         ipvt(j) = ipvt(kmax)
X         ipvt(kmax) = k
X   40    continue
Xc
Xc        compute the householder transformation to reduce the
Xc        j-th column of a to a multiple of the j-th unit vector.
Xc
X         ajnorm = enorm(m-j+1,a(j,j))
X         if (ajnorm .eq. zero) go to 100
X         if (a(j,j) .lt. zero) ajnorm = -ajnorm
X         do 50 i = j, m
X            a(i,j) = a(i,j)/ajnorm
X   50       continue
X         a(j,j) = a(j,j) + one
Xc
Xc        apply the transformation to the remaining columns
Xc        and update the norms.
Xc
X         jp1 = j + 1
X         if (n .lt. jp1) go to 100
X         do 90 k = jp1, n
X            sum = zero
X            do 60 i = j, m
X               sum = sum + a(i,j)*a(i,k)
X   60          continue
X            temp = sum/a(j,j)
X            do 70 i = j, m
X               a(i,k) = a(i,k) - temp*a(i,j)
X   70          continue
X            if (.not.pivot .or. rdiag(k) .eq. zero) go to 80
X            temp = a(j,k)/rdiag(k)
X            rdiag(k) = rdiag(k)*dsqrt(dmax1(zero,one-temp**2))
X            if (p05*(rdiag(k)/wa(k))**2 .gt. epsmch) go to 80
X            rdiag(k) = enorm(m-j,a(jp1,k))
X            wa(k) = rdiag(k)
X   80       continue
X   90       continue
X  100    continue
X         rdiag(j) = -ajnorm
X  110    continue
X      return
Xc
Xc     last card of subroutine qrfac.
Xc
X      end
X      subroutine lmpar(n,r,ldr,ipvt,diag,qtb,delta,par,x,sdiag,wa1,
X     *                 wa2)
X      integer n,ldr
X      integer ipvt(n)
X      double precision delta,par
X      double precision r(ldr,n),diag(n),qtb(n),x(n),sdiag(n),wa1(n),
X     *                 wa2(n)
Xc     **********
Xc
Xc     subroutine lmpar
Xc
Xc     given an m by n matrix a, an n by n nonsingular diagonal
Xc     matrix d, an m-vector b, and a positive number delta,
Xc     the problem is to determine a value for the parameter
Xc     par such that if x solves the system
Xc
Xc           a*x = b ,     sqrt(par)*d*x = 0 ,
Xc
Xc     in the least squares sense, and dxnorm is the euclidean
Xc     norm of d*x, then either par is zero and
Xc
Xc           (dxnorm-delta) .le. 0.1*delta ,
Xc
Xc     or par is positive and
Xc
Xc           abs(dxnorm-delta) .le. 0.1*delta .
Xc
Xc     this subroutine completes the solution of the problem
Xc     if it is provided with the necessary information from the
Xc     qr factorization, with column pivoting, of a. that is, if
Xc     a*p = q*r, where p is a permutation matrix, q has orthogonal
Xc     columns, and r is an upper triangular matrix with diagonal
Xc     elements of nonincreasing magnitude, then lmpar expects
Xc     the full upper triangle of r, the permutation matrix p,
Xc     and the first n components of (q transpose)*b. on output
Xc     lmpar also provides an upper triangular matrix s such that
Xc
Xc            t   t                   t
Xc           p *(a *a + par*d*d)*p = s *s .
Xc
Xc     s is employed within lmpar and may be of separate interest.
Xc
Xc     only a few iterations are generally needed for convergence
Xc     of the algorithm. if, however, the limit of 10 iterations
Xc     is reached, then the output par will contain the best
Xc     value obtained so far.
Xc
Xc     the subroutine statement is
Xc
Xc       subroutine lmpar(n,r,ldr,ipvt,diag,qtb,delta,par,x,sdiag,
Xc                        wa1,wa2)
Xc
Xc     where
Xc
Xc       n is a positive integer input variable set to the order of r.
Xc
Xc       r is an n by n array. on input the full upper triangle
Xc         must contain the full upper triangle of the matrix r.
Xc         on output the full upper triangle is unaltered, and the
Xc         strict lower triangle contains the strict upper triangle
Xc         (transposed) of the upper triangular matrix s.
Xc
Xc       ldr is a positive integer input variable not less than n
Xc         which specifies the leading dimension of the array r.
Xc
Xc       ipvt is an integer input array of length n which defines the
Xc         permutation matrix p such that a*p = q*r. column j of p
Xc         is column ipvt(j) of the identity matrix.
Xc
Xc       diag is an input array of length n which must contain the
Xc         diagonal elements of the matrix d.
Xc
Xc       qtb is an input array of length n which must contain the first
Xc         n elements of the vector (q transpose)*b.
Xc
Xc       delta is a positive input variable which specifies an upper
Xc         bound on the euclidean norm of d*x.
Xc
Xc       par is a nonnegative variable. on input par contains an
Xc         initial estimate of the levenberg-marquardt parameter.
Xc         on output par contains the final estimate.
Xc
Xc       x is an output array of length n which contains the least
Xc         squares solution of the system a*x = b, sqrt(par)*d*x = 0,
Xc         for the output par.
Xc
Xc       sdiag is an output array of length n which contains the
Xc         diagonal elements of the upper triangular matrix s.
Xc
Xc       wa1 and wa2 are work arrays of length n.
Xc
Xc     subprograms called
Xc
Xc       minpack-supplied ... dpmpar,enorm,qrsolv
Xc
Xc       fortran-supplied ... dabs,dmax1,dmin1,dsqrt
Xc
Xc     argonne national laboratory. minpack project. march 1980.
Xc     burton s. garbow, kenneth e. hillstrom, jorge j. more
Xc
Xc     **********
X      integer i,iter,j,jm1,jp1,k,l,nsing
X      double precision dxnorm,dwarf,fp,gnorm,parc,parl,paru,p1,p001,
X     *                 sum,temp,zero
Xc     double precision dpmpar,enorm
X      double precision enorm
X      data p1,p001,zero /1.0d-1,1.0d-3,0.0d0/
X      double precision  FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/   FLTMIN, FLTMAX, EPSMIN, EPSMAX
Xc
Xc     dwarf is the smallest positive magnitude.
Xc
Xc     dwarf = dpmpar(2)
X      dwarf = FLTMIN
Xc
Xc     compute and store in x the gauss-newton direction. if the
Xc     jacobian is rank-deficient, obtain a least squares solution.
Xc
X      nsing = n
X      do 10 j = 1, n
X         wa1(j) = qtb(j)
X         if (r(j,j) .eq. zero .and. nsing .eq. n) nsing = j - 1
X         if (nsing .lt. n) wa1(j) = zero
X   10    continue
X      if (nsing .lt. 1) go to 50
X      do 40 k = 1, nsing
X         j = nsing - k + 1
X         wa1(j) = wa1(j)/r(j,j)
X         temp = wa1(j)
X         jm1 = j - 1
X         if (jm1 .lt. 1) go to 30
X         do 20 i = 1, jm1
X            wa1(i) = wa1(i) - r(i,j)*temp
X   20       continue
X   30    continue
X   40    continue
X   50 continue
X      do 60 j = 1, n
X         l = ipvt(j)
X         x(l) = wa1(j)
X   60    continue
Xc
Xc     initialize the iteration counter.
Xc     evaluate the function at the origin, and test
Xc     for acceptance of the gauss-newton direction.
Xc
X      iter = 0
X      do 70 j = 1, n
X         wa2(j) = diag(j)*x(j)
X   70    continue
X      dxnorm = enorm(n,wa2)
X      fp = dxnorm - delta
X      if (fp .le. p1*delta) go to 220
Xc
Xc     if the jacobian is not rank deficient, the newton
Xc     step provides a lower bound, parl, for the zero of
Xc     the function. otherwise set this bound to zero.
Xc
X      parl = zero
X      if (nsing .lt. n) go to 120
X      do 80 j = 1, n
X         l = ipvt(j)
X         wa1(j) = diag(l)*(wa2(l)/dxnorm)
X   80    continue
X      do 110 j = 1, n
X         sum = zero
X         jm1 = j - 1
X         if (jm1 .lt. 1) go to 100
X         do 90 i = 1, jm1
X            sum = sum + r(i,j)*wa1(i)
X   90       continue
X  100    continue
X         wa1(j) = (wa1(j) - sum)/r(j,j)
X  110    continue
X      temp = enorm(n,wa1)
X      parl = ((fp/delta)/temp)/temp
X  120 continue
Xc
Xc     calculate an upper bound, paru, for the zero of the function.
Xc
X      do 140 j = 1, n
X         sum = zero
X         do 130 i = 1, j
X            sum = sum + r(i,j)*qtb(i)
X  130       continue
X         l = ipvt(j)
X         wa1(j) = sum/diag(l)
X  140    continue
X      gnorm = enorm(n,wa1)
X      paru = gnorm/delta
X      if (paru .eq. zero) paru = dwarf/dmin1(delta,p1)
Xc
Xc     if the input par lies outside of the interval (parl,paru),
Xc     set par to the closer endpoint.
Xc
X      par = dmax1(par,parl)
X      par = dmin1(par,paru)
X      if (par .eq. zero) par = gnorm/dxnorm
Xc
Xc     beginning of an iteration.
Xc
X  150 continue
X         iter = iter + 1
Xc
Xc        evaluate the function at the current value of par.
Xc
X         if (par .eq. zero) par = dmax1(dwarf,p001*paru)
X         temp = dsqrt(par)
X         do 160 j = 1, n
X            wa1(j) = temp*diag(j)
X  160       continue
X         call qrsolv(n,r,ldr,ipvt,wa1,qtb,x,sdiag,wa2)
X         do 170 j = 1, n
X            wa2(j) = diag(j)*x(j)
X  170       continue
X         dxnorm = enorm(n,wa2)
X         temp = fp
X         fp = dxnorm - delta
Xc
Xc        if the function is small enough, accept the current value
Xc        of par. also test for the exceptional cases where parl
Xc        is zero or the number of iterations has reached 10.
Xc
X         if (dabs(fp) .le. p1*delta
X     *       .or. parl .eq. zero .and. fp .le. temp
X     *            .and. temp .lt. zero .or. iter .eq. 10) go to 220
Xc
Xc        compute the newton correction.
Xc
X         do 180 j = 1, n
X            l = ipvt(j)
X            wa1(j) = diag(l)*(wa2(l)/dxnorm)
X  180       continue
X         do 210 j = 1, n
X            wa1(j) = wa1(j)/sdiag(j)
X            temp = wa1(j)
X            jp1 = j + 1
X            if (n .lt. jp1) go to 200
X            do 190 i = jp1, n
X               wa1(i) = wa1(i) - r(i,j)*temp
X  190          continue
X  200       continue
X  210       continue
X         temp = enorm(n,wa1)
X         parc = ((fp/delta)/temp)/temp
Xc
Xc        depending on the sign of the function, update parl or paru.
Xc
X         if (fp .gt. zero) parl = dmax1(parl,par)
X         if (fp .lt. zero) paru = dmin1(paru,par)
Xc
Xc        compute an improved estimate for par.
Xc
X         par = dmax1(parl,par+parc)
Xc
Xc        end of an iteration.
Xc
X         go to 150
X  220 continue
Xc
Xc     termination.
Xc
X      if (iter .eq. 0) par = zero
X      return
Xc
Xc     last card of subroutine lmpar.
Xc
X      end
X      subroutine qrsolv(n,r,ldr,ipvt,diag,qtb,x,sdiag,wa)
X      integer n,ldr
X      integer ipvt(n)
X      double precision r(ldr,n),diag(n),qtb(n),x(n),sdiag(n),wa(n)
Xc     **********
Xc
Xc     subroutine qrsolv
Xc
Xc     given an m by n matrix a, an n by n diagonal matrix d,
Xc     and an m-vector b, the problem is to determine an x which
Xc     solves the system
Xc
Xc           a*x = b ,     d*x = 0 ,
Xc
Xc     in the least squares sense.
Xc
Xc     this subroutine completes the solution of the problem
Xc     if it is provided with the necessary information from the
Xc     qr factorization, with column pivoting, of a. that is, if
Xc     a*p = q*r, where p is a permutation matrix, q has orthogonal
Xc     columns, and r is an upper triangular matrix with diagonal
Xc     elements of nonincreasing magnitude, then qrsolv expects
Xc     the full upper triangle of r, the permutation matrix p,
Xc     and the first n components of (q transpose)*b. the system
Xc     a*x = b, d*x = 0, is then equivalent to
Xc
Xc                  t       t
Xc           r*z = q *b ,  p *d*p*z = 0 ,
Xc
Xc     where x = p*z. if this system does not have full rank,
Xc     then a least squares solution is obtained. on output qrsolv
Xc     also provides an upper triangular matrix s such that
Xc
Xc            t   t               t
Xc           p *(a *a + d*d)*p = s *s .
Xc
Xc     s is computed within qrsolv and may be of separate interest.
Xc
Xc     the subroutine statement is
Xc
Xc       subroutine qrsolv(n,r,ldr,ipvt,diag,qtb,x,sdiag,wa)
Xc
Xc     where
Xc
Xc       n is a positive integer input variable set to the order of r.
Xc
Xc       r is an n by n array. on input the full upper triangle
Xc         must contain the full upper triangle of the matrix r.
Xc         on output the full upper triangle is unaltered, and the
Xc         strict lower triangle contains the strict upper triangle
Xc         (transposed) of the upper triangular matrix s.
Xc
Xc       ldr is a positive integer input variable not less than n
Xc         which specifies the leading dimension of the array r.
Xc
Xc       ipvt is an integer input array of length n which defines the
Xc         permutation matrix p such that a*p = q*r. column j of p
Xc         is column ipvt(j) of the identity matrix.
Xc
Xc       diag is an input array of length n which must contain the
Xc         diagonal elements of the matrix d.
Xc
Xc       qtb is an input array of length n which must contain the first
Xc         n elements of the vector (q transpose)*b.
Xc
Xc       x is an output array of length n which contains the least
Xc         squares solution of the system a*x = b, d*x = 0.
Xc
Xc       sdiag is an output array of length n which contains the
Xc         diagonal elements of the upper triangular matrix s.
Xc
Xc       wa is a work array of length n.
Xc
Xc     subprograms called
Xc
Xc       fortran-supplied ... dabs,dsqrt
Xc
Xc     argonne national laboratory. minpack project. march 1980.
Xc     burton s. garbow, kenneth e. hillstrom, jorge j. more
Xc
Xc     **********
X      integer i,j,jp1,k,kp1,l,nsing
X      double precision cos,cotan,p5,p25,qtbpj,sin,sum,tan,temp,zero
X      data p5,p25,zero /5.0d-1,2.5d-1,0.0d0/
Xc
Xc     copy r and (q transpose)*b to preserve input and initialize s.
Xc     in particular, save the diagonal elements of r in x.
Xc
X      do 20 j = 1, n
X         do 10 i = j, n
X            r(i,j) = r(j,i)
X   10       continue
X         x(j) = r(j,j)
X         wa(j) = qtb(j)
X   20    continue
Xc
Xc     eliminate the diagonal matrix d using a givens rotation.
Xc
X      do 100 j = 1, n
Xc
Xc        prepare the row of d to be eliminated, locating the
Xc        diagonal element using p from the qr factorization.
Xc
X         l = ipvt(j)
X         if (diag(l) .eq. zero) go to 90
X         do 30 k = j, n
X            sdiag(k) = zero
X   30       continue
X         sdiag(j) = diag(l)
Xc
Xc        the transformations to eliminate the row of d
Xc        modify only a single element of (q transpose)*b
Xc        beyond the first n, which is initially zero.
Xc
X         qtbpj = zero
X         do 80 k = j, n
Xc
Xc           determine a givens rotation which eliminates the
Xc           appropriate element in the current row of d.
Xc
X            if (sdiag(k) .eq. zero) go to 70
X            if (dabs(r(k,k)) .ge. dabs(sdiag(k))) go to 40
X               cotan = r(k,k)/sdiag(k)
X               sin = p5/dsqrt(p25+p25*cotan**2)
X               cos = sin*cotan
X               go to 50
X   40       continue
X               tan = sdiag(k)/r(k,k)
X               cos = p5/dsqrt(p25+p25*tan**2)
X               sin = cos*tan
X   50       continue
Xc
Xc           compute the modified diagonal element of r and
Xc           the modified element of ((q transpose)*b,0).
Xc
X            r(k,k) = cos*r(k,k) + sin*sdiag(k)
X            temp = cos*wa(k) + sin*qtbpj
X            qtbpj = -sin*wa(k) + cos*qtbpj
X            wa(k) = temp
Xc
Xc           accumulate the tranformation in the row of s.
Xc
X            kp1 = k + 1
X            if (n .lt. kp1) go to 70
X            do 60 i = kp1, n
X               temp = cos*r(i,k) + sin*sdiag(i)
X               sdiag(i) = -sin*r(i,k) + cos*sdiag(i)
X               r(i,k) = temp
X   60          continue
X   70       continue
X   80       continue
X   90    continue
Xc
Xc        store the diagonal element of s and restore
Xc        the corresponding diagonal element of r.
Xc
X         sdiag(j) = r(j,j)
X         r(j,j) = x(j)
X  100    continue
Xc
Xc     solve the triangular system for z. if the system is
Xc     singular, then obtain a least squares solution.
Xc
X      nsing = n
X      do 110 j = 1, n
X         if (sdiag(j) .eq. zero .and. nsing .eq. n) nsing = j - 1
X         if (nsing .lt. n) wa(j) = zero
X  110    continue
X      if (nsing .lt. 1) go to 150
X      do 140 k = 1, nsing
X         j = nsing - k + 1
X         sum = zero
X         jp1 = j + 1
X         if (nsing .lt. jp1) go to 130
X         do 120 i = jp1, nsing
X            sum = sum + r(i,j)*wa(i)
X  120       continue
X  130    continue
X         wa(j) = (wa(j) - sum)/sdiag(j)
X  140    continue
X  150 continue
Xc
Xc     permute the components of z back to components of x.
Xc
X      do 160 j = 1, n
X         l = ipvt(j)
X         x(l) = wa(j)
X  160    continue
X      return
Xc
Xc     last card of subroutine qrsolv.
Xc
X      end
END_OF_FILE
  if test 39452 -ne `wc -c <'fdmin.f'`; then
    echo shar: \"'fdmin.f'\" unpacked with wrong size!
  fi
  # end of 'fdmin.f'
fi
if test -f 'fdlin.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdlin.f'\"
else
  echo shar: Extracting \"'fdlin.f'\" \(26964 characters\)
  sed "s/^X//" >'fdlin.f' <<'END_OF_FILE'
X      subroutine dsvdc(x,ldx,n,p,s,e,u,ldu,v,ldv,work,job,info)
X      integer ldx,n,p,ldu,ldv,job,info
X      double precision x(ldx,1),s(1),e(1),u(ldu,1),v(ldv,1),work(1)
Xc
Xc
Xc     dsvdc is a subroutine to reduce a double precision nxp matrix x
Xc     by orthogonal transformations u and v to diagonal form.  the
Xc     diagonal elements s(i) are the singular values of x.  the
Xc     columns of u are the corresponding left singular vectors,
Xc     and the columns of v the right singular vectors.
Xc
Xc     on entry
Xc
Xc         x         double precision(ldx,p), where ldx.ge.n.
Xc                   x contains the matrix whose singular value
Xc                   decomposition is to be computed.  x is
Xc                   destroyed by dsvdc.
Xc
Xc         ldx       integer.
Xc                   ldx is the leading dimension of the array x.
Xc
Xc         n         integer.
Xc                   n is the number of rows of the matrix x.
Xc
Xc         p         integer.
Xc                   p is the number of columns of the matrix x.
Xc
Xc         ldu       integer.
Xc                   ldu is the leading dimension of the array u.
Xc                   (see below).
Xc
Xc         ldv       integer.
Xc                   ldv is the leading dimension of the array v.
Xc                   (see below).
Xc
Xc         work      double precision(n).
Xc                   work is a scratch array.
Xc
Xc         job       integer.
Xc                   job controls the computation of the singular
Xc                   vectors.  it has the decimal expansion ab
Xc                   with the following meaning
Xc
Xc                        a.eq.0    do not compute the left singular
Xc                                  vectors.
Xc                        a.eq.1    return the n left singular vectors
Xc                                  in u.
Xc                        a.ge.2    return the first min(n,p) singular
Xc                                  vectors in u.
Xc                        b.eq.0    do not compute the right singular
Xc                                  vectors.
Xc                        b.eq.1    return the right singular vectors
Xc                                  in v.
Xc
Xc     on return
Xc
Xc         s         double precision(mm), where mm=min(n+1,p).
Xc                   the first min(n,p) entries of s contain the
Xc                   singular values of x arranged in descending
Xc                   order of magnitude.
Xc
Xc         e         double precision(p), 
Xc                   e ordinarily contains zeros.  however see the
Xc                   discussion of info for exceptions.
Xc
Xc         u         double precision(ldu,k), where ldu.ge.n.  if
Xc                                   joba.eq.1 then k.eq.n, if joba.ge.2
Xc                                   then k.eq.min(n,p).
Xc                   u contains the matrix of left singular vectors.
Xc                   u is not referenced if joba.eq.0.  if n.le.p
Xc                   or if joba.eq.2, then u may be identified with x
Xc                   in the subroutine call.
Xc
Xc         v         double precision(ldv,p), where ldv.ge.p.
Xc                   v contains the matrix of right singular vectors.
Xc                   v is not referenced if job.eq.0.  if p.le.n,
Xc                   then v may be identified with x in the
Xc                   subroutine call.
Xc
Xc         info      integer.
Xc                   the singular values (and their corresponding
Xc                   singular vectors) s(info+1),s(info+2),...,s(m)
Xc                   are correct (here m=min(n,p)).  thus if
Xc                   info.eq.0, all the singular values and their
Xc                   vectors are correct.  in any event, the matrix
Xc                   b = trans(u)*x*v is the bidiagonal matrix
Xc                   with the elements of s on its diagonal and the
Xc                   elements of e on its super-diagonal (trans(u)
Xc                   is the transpose of u).  thus the singular
Xc                   values of x and b are the same.
Xc
Xc     linpack. this version dated 08/14/78 .
Xc              correction made to shift 2/84.
Xc     g.w. stewart, university of maryland, argonne national lab.
Xc
Xc     dsvdc uses the following functions and subprograms.
Xc
Xc     external drot
Xc     blas daxpy,ddot,dscal,dswap,dnrm2,drotg
Xc     fortran dabs,dmax1,max0,min0,mod,dsqrt
Xc
Xc     internal variables
Xc
X      integer i,iter,j,jobu,k,kase,kk,l,ll,lls,lm1,lp1,ls,lu,m,maxit,
X     *        mm,mm1,mp1,nct,nctp1,ncu,nrt,nrtp1
X      double precision ddot,t
X      double precision b,c,cs,el,emm1,f,g,dnrm2,scale,shift,sl,sm,sn,
X     *                 smm1,t1,test,ztest
X      logical wantu,wantv
Xc
Xc
Xc     set the maximum number of iterations.
Xc
X      maxit = 30
Xc
Xc     determine what is to be computed.
Xc
X      wantu = .false.
X      wantv = .false.
X      jobu = mod(job,100)/10
X      ncu = n
X      if (jobu .gt. 1) ncu = min0(n,p)
X      if (jobu .ne. 0) wantu = .true.
X      if (mod(job,10) .ne. 0) wantv = .true.
Xc
Xc     reduce x to bidiagonal form, storing the diagonal elements
Xc     in s and the super-diagonal elements in e.
Xc
X      info = 0
X      nct = min0(n-1,p)
X      nrt = max0(0,min0(p-2,n))
X      lu = max0(nct,nrt)
X      if (lu .lt. 1) go to 170
X      do 160 l = 1, lu
X         lp1 = l + 1
X         if (l .gt. nct) go to 20
Xc
Xc           compute the transformation for the l-th column and
Xc           place the l-th diagonal in s(l).
Xc
X            s(l) = dnrm2(n-l+1,x(l,l),1)
X            if (s(l) .eq. 0.0d0) go to 10
X               if (x(l,l) .ne. 0.0d0) s(l) = dsign(s(l),x(l,l))
X               call dscal(n-l+1,1.0d0/s(l),x(l,l),1)
X               x(l,l) = 1.0d0 + x(l,l)
X   10       continue
X            s(l) = -s(l)
X   20    continue
X         if (p .lt. lp1) go to 50
X         do 40 j = lp1, p
X            if (l .gt. nct) go to 30
X            if (s(l) .eq. 0.0d0) go to 30
Xc
Xc              apply the transformation.
Xc
X               t = -ddot(n-l+1,x(l,l),1,x(l,j),1)/x(l,l)
X               call daxpy(n-l+1,t,x(l,l),1,x(l,j),1)
X   30       continue
Xc
Xc           place the l-th row of x into  e for the
Xc           subsequent calculation of the row transformation.
Xc
X            e(j) = x(l,j)
X   40    continue
X   50    continue
X         if (.not.wantu .or. l .gt. nct) go to 70
Xc
Xc           place the transformation in u for subsequent back
Xc           multiplication.
Xc
X            do 60 i = l, n
X               u(i,l) = x(i,l)
X   60       continue
X   70    continue
X         if (l .gt. nrt) go to 150
Xc
Xc           compute the l-th row transformation and place the
Xc           l-th super-diagonal in e(l).
Xc
X            e(l) = dnrm2(p-l,e(lp1),1)
X            if (e(l) .eq. 0.0d0) go to 80
X               if (e(lp1) .ne. 0.0d0) e(l) = dsign(e(l),e(lp1))
X               call dscal(p-l,1.0d0/e(l),e(lp1),1)
X               e(lp1) = 1.0d0 + e(lp1)
X   80       continue
X            e(l) = -e(l)
X            if (lp1 .gt. n .or. e(l) .eq. 0.0d0) go to 120
Xc
Xc              apply the transformation.
Xc
X               do 90 i = lp1, n
X                  work(i) = 0.0d0
X   90          continue
X               do 100 j = lp1, p
X                  call daxpy(n-l,e(j),x(lp1,j),1,work(lp1),1)
X  100          continue
X               do 110 j = lp1, p
X                  call daxpy(n-l,-e(j)/e(lp1),work(lp1),1,x(lp1,j),1)
X  110          continue
X  120       continue
X            if (.not.wantv) go to 140
Xc
Xc              place the transformation in v for subsequent
Xc              back multiplication.
Xc
X               do 130 i = lp1, p
X                  v(i,l) = e(i)
X  130          continue
X  140       continue
X  150    continue
X  160 continue
X  170 continue
Xc
Xc     set up the final bidiagonal matrix or order m.
Xc
X      m = min0(p,n+1)
X      nctp1 = nct + 1
X      nrtp1 = nrt + 1
X      if (nct .lt. p) s(nctp1) = x(nctp1,nctp1)
X      if (n .lt. m) s(m) = 0.0d0
X      if (nrtp1 .lt. m) e(nrtp1) = x(nrtp1,m)
X      e(m) = 0.0d0
Xc
Xc     if required, generate u.
Xc
X      if (.not.wantu) go to 300
X         if (ncu .lt. nctp1) go to 200
X         do 190 j = nctp1, ncu
X            do 180 i = 1, n
X               u(i,j) = 0.0d0
X  180       continue
X            u(j,j) = 1.0d0
X  190    continue
X  200    continue
X         if (nct .lt. 1) go to 290
X         do 280 ll = 1, nct
X            l = nct - ll + 1
X            if (s(l) .eq. 0.0d0) go to 250
X               lp1 = l + 1
X               if (ncu .lt. lp1) go to 220
X               do 210 j = lp1, ncu
X                  t = -ddot(n-l+1,u(l,l),1,u(l,j),1)/u(l,l)
X                  call daxpy(n-l+1,t,u(l,l),1,u(l,j),1)
X  210          continue
X  220          continue
X               call dscal(n-l+1,-1.0d0,u(l,l),1)
X               u(l,l) = 1.0d0 + u(l,l)
X               lm1 = l - 1
X               if (lm1 .lt. 1) go to 240
X               do 230 i = 1, lm1
X                  u(i,l) = 0.0d0
X  230          continue
X  240          continue
X            go to 270
X  250       continue
X               do 260 i = 1, n
X                  u(i,l) = 0.0d0
X  260          continue
X               u(l,l) = 1.0d0
X  270       continue
X  280    continue
X  290    continue
X  300 continue
Xc
Xc     if it is required, generate v.
Xc
X      if (.not.wantv) go to 350
X         do 340 ll = 1, p
X            l = p - ll + 1
X            lp1 = l + 1
X            if (l .gt. nrt) go to 320
X            if (e(l) .eq. 0.0d0) go to 320
X               do 310 j = lp1, p
X                  t = -ddot(p-l,v(lp1,l),1,v(lp1,j),1)/v(lp1,l)
X                  call daxpy(p-l,t,v(lp1,l),1,v(lp1,j),1)
X  310          continue
X  320       continue
X            do 330 i = 1, p
X               v(i,l) = 0.0d0
X  330       continue
X            v(l,l) = 1.0d0
X  340    continue
X  350 continue
Xc
Xc     main iteration loop for the singular values.
Xc
X      mm = m
X      iter = 0
X  360 continue
Xc
Xc        quit if all the singular values have been found.
Xc
Xc     ...exit
X         if (m .eq. 0) go to 620
Xc
Xc        if too many iterations have been performed, set
Xc        flag and return.
Xc
X         if (iter .lt. maxit) go to 370
X            info = m
Xc     ......exit
X            go to 620
X  370    continue
Xc
Xc        this section of the program inspects for
Xc        negligible elements in the s and e arrays.  on
Xc        completion the variables kase and l are set as follows.
Xc
Xc           kase = 1     if s(m) and e(l-1) are negligible and l.lt.m
Xc           kase = 2     if s(l) is negligible and l.lt.m
Xc           kase = 3     if e(l-1) is negligible, l.lt.m, and
Xc                        s(l), ..., s(m) are not negligible (qr step).
Xc           kase = 4     if e(m-1) is negligible (convergence).
Xc
X         do 390 ll = 1, m
X            l = m - ll
Xc        ...exit
X            if (l .eq. 0) go to 400
X            test = dabs(s(l)) + dabs(s(l+1))
X            ztest = test + dabs(e(l))
X            if (ztest .ne. test) go to 380
X               e(l) = 0.0d0
Xc        ......exit
X               go to 400
X  380       continue
X  390    continue
X  400    continue
X         if (l .ne. m - 1) go to 410
X            kase = 4
X         go to 480
X  410    continue
X            lp1 = l + 1
X            mp1 = m + 1
X            do 430 lls = lp1, mp1
X               ls = m - lls + lp1
Xc           ...exit
X               if (ls .eq. l) go to 440
X               test = 0.0d0
X               if (ls .ne. m) test = test + dabs(e(ls))
X               if (ls .ne. l + 1) test = test + dabs(e(ls-1))
X               ztest = test + dabs(s(ls))
X               if (ztest .ne. test) go to 420
X                  s(ls) = 0.0d0
Xc           ......exit
X                  go to 440
X  420          continue
X  430       continue
X  440       continue
X            if (ls .ne. l) go to 450
X               kase = 3
X            go to 470
X  450       continue
X            if (ls .ne. m) go to 460
X               kase = 1
X            go to 470
X  460       continue
X               kase = 2
X               l = ls
X  470       continue
X  480    continue
X         l = l + 1
Xc
Xc        perform the task indicated by kase.
Xc
X         go to (490,520,540,570), kase
Xc
Xc        deflate negligible s(m).
Xc
X  490    continue
X            mm1 = m - 1
X            f = e(m-1)
X            e(m-1) = 0.0d0
X            do 510 kk = l, mm1
X               k = mm1 - kk + l
X               t1 = s(k)
X               call drotg(t1,f,cs,sn)
X               s(k) = t1
X               if (k .eq. l) go to 500
X                  f = -sn*e(k-1)
X                  e(k-1) = cs*e(k-1)
X  500          continue
X               if (wantv) call drot(p,v(1,k),1,v(1,m),1,cs,sn)
X  510       continue
X         go to 610
Xc
Xc        split at negligible s(l).
Xc
X  520    continue
X            f = e(l-1)
X            e(l-1) = 0.0d0
X            do 530 k = l, m
X               t1 = s(k)
X               call drotg(t1,f,cs,sn)
X               s(k) = t1
X               f = -sn*e(k)
X               e(k) = cs*e(k)
X               if (wantu) call drot(n,u(1,k),1,u(1,l-1),1,cs,sn)
X  530       continue
X         go to 610
Xc
Xc        perform one qr step.
Xc
X  540    continue
Xc
Xc           calculate the shift.
Xc
X            scale = dmax1(dabs(s(m)),dabs(s(m-1)),dabs(e(m-1)),
X     *                    dabs(s(l)),dabs(e(l)))
X            sm = s(m)/scale
X            smm1 = s(m-1)/scale
X            emm1 = e(m-1)/scale
X            sl = s(l)/scale
X            el = e(l)/scale
X            b = ((smm1 + sm)*(smm1 - sm) + emm1**2)/2.0d0
X            c = (sm*emm1)**2
X            shift = 0.0d0
X            if (b .eq. 0.0d0 .and. c .eq. 0.0d0) go to 550
X               shift = dsqrt(b**2+c)
X               if (b .lt. 0.0d0) shift = -shift
X               shift = c/(b + shift)
X  550       continue
X            f = (sl + sm)*(sl - sm) + shift
X            g = sl*el
Xc
Xc           chase zeros.
Xc
X            mm1 = m - 1
X            do 560 k = l, mm1
X               call drotg(f,g,cs,sn)
X               if (k .ne. l) e(k-1) = f
X               f = cs*s(k) + sn*e(k)
X               e(k) = cs*e(k) - sn*s(k)
X               g = sn*s(k+1)
X               s(k+1) = cs*s(k+1)
X               if (wantv) call drot(p,v(1,k),1,v(1,k+1),1,cs,sn)
X               call drotg(f,g,cs,sn)
X               s(k) = f
X               f = cs*e(k) + sn*s(k+1)
X               s(k+1) = -sn*e(k) + cs*s(k+1)
X               g = sn*e(k+1)
X               e(k+1) = cs*e(k+1)
X               if (wantu .and. k .lt. n)
X     *            call drot(n,u(1,k),1,u(1,k+1),1,cs,sn)
X  560       continue
X            e(m-1) = f
X            iter = iter + 1
X         go to 610
Xc
Xc        convergence.
Xc
X  570    continue
Xc
Xc           make the singular value  positive.
Xc
X            if (s(l) .ge. 0.0d0) go to 580
X               s(l) = -s(l)
X               if (wantv) call dscal(p,-1.0d0,v(1,l),1)
X  580       continue
Xc
Xc           order the singular value.
Xc
X  590       if (l .eq. mm) go to 600
Xc           ...exit
X               if (s(l) .ge. s(l+1)) go to 600
X               t = s(l)
X               s(l) = s(l+1)
X               s(l+1) = t
X               if (wantv .and. l .lt. p)
X     *            call dswap(p,v(1,l),1,v(1,l+1),1)
X               if (wantu .and. l .lt. n)
X     *            call dswap(n,u(1,l),1,u(1,l+1),1)
X               l = l + 1
X            go to 590
X  600       continue
X            iter = 0
X            m = m - 1
X  610    continue
X      go to 360
X  620 continue
X      return
X      end
X      double precision function dnrm2 ( n, dx, incx)
X      integer          next
X      double precision   dx(1), cutlo, cuthi, hitest, sum, xmax,zero,one
X      data   zero, one /0.0d0, 1.0d0/
Xc
Xc     euclidean norm of the n-vector stored in dx() with storage
Xc     increment incx .
Xc     if    n .le. 0 return with result = 0.
Xc     if n .ge. 1 then incx must be .ge. 1
Xc
Xc           c.l.lawson, 1978 jan 08
Xc
Xc     four phase method     using two built-in constants that are
Xc     hopefully applicable to all machines.
Xc         cutlo = maximum of  dsqrt(u/eps)  over all known machines.
Xc         cuthi = minimum of  dsqrt(v)      over all known machines.
Xc     where
Xc         eps = smallest no. such that eps + 1. .gt. 1.
Xc         u   = smallest positive no.   (underflow limit)
Xc         v   = largest  no.            (overflow  limit)
Xc
Xc     brief outline of algorithm..
Xc
Xc     phase 1    scans zero components.
Xc     move to phase 2 when a component is nonzero and .le. cutlo
Xc     move to phase 3 when a component is .gt. cutlo
Xc     move to phase 4 when a component is .ge. cuthi/m
Xc     where m = n for x() real and m = 2*n for complex.
Xc
Xc     values for cutlo and cuthi..
Xc     from the environmental parameters listed in the imsl converter
Xc     document the limiting values are as follows..
Xc     cutlo, s.p.   u/eps = 2**(-102) for  honeywell.  close seconds are
Xc                   univac and dec at 2**(-103)
Xc                   thus cutlo = 2**(-51) = 4.44089e-16
Xc     cuthi, s.p.   v = 2**127 for univac, honeywell, and dec.
Xc                   thus cuthi = 2**(63.5) = 1.30438e19
Xc     cutlo, d.p.   u/eps = 2**(-67) for honeywell and dec.
Xc                   thus cutlo = 2**(-33.5) = 8.23181d-11
Xc     cuthi, d.p.   same as s.p.  cuthi = 1.30438d19
Xc     data cutlo, cuthi / 8.232d-11,  1.304d19 /
Xc     data cutlo, cuthi / 4.441e-16,  1.304e19 /
X      data cutlo, cuthi / 8.232d-11,  1.304d19 /
Xc
X      if(n .gt. 0) go to 10
X         dnrm2  = zero
X         go to 300
Xc
X   10 assign 30 to next
X      sum = zero
X      nn = n * incx
Xc                                                 begin main loop
X      i = 1
X   20    go to next,(30, 50, 70, 110)
X   30 if( dabs(dx(i)) .gt. cutlo) go to 85
X      assign 50 to next
X      xmax = zero
Xc
Xc                        phase 1.  sum is zero
Xc
X   50 if( dx(i) .eq. zero) go to 200
X      if( dabs(dx(i)) .gt. cutlo) go to 85
Xc
Xc                                prepare for phase 2.
X      assign 70 to next
X      go to 105
Xc
Xc                                prepare for phase 4.
Xc
X  100 i = j
X      assign 110 to next
X      sum = (sum / dx(i)) / dx(i)
X  105 xmax = dabs(dx(i))
X      go to 115
Xc
Xc                   phase 2.  sum is small.
Xc                             scale to avoid destructive underflow.
Xc
X   70 if( dabs(dx(i)) .gt. cutlo ) go to 75
Xc
Xc                     common code for phases 2 and 4.
Xc                     in phase 4 sum is large.  scale to avoid overflow.
Xc
X  110 if( dabs(dx(i)) .le. xmax ) go to 115
X         sum = one + sum * (xmax / dx(i))**2
X         xmax = dabs(dx(i))
X         go to 200
Xc
X  115 sum = sum + (dx(i)/xmax)**2
X      go to 200
Xc
Xc
Xc                  prepare for phase 3.
Xc
X   75 sum = (sum * xmax) * xmax
Xc
Xc
Xc     for real or d.p. set hitest = cuthi/n
Xc     for complex      set hitest = cuthi/(2*n)
Xc
X   85 hitest = cuthi/float( n )
Xc
Xc                   phase 3.  sum is mid-range.  no scaling.
Xc
X      do 95 j =i,nn,incx
X      if(dabs(dx(j)) .ge. hitest) go to 100
X   95    sum = sum + dx(j)**2
X      dnrm2 = dsqrt( sum )
X      go to 300
Xc
X  200 continue
X      i = i + incx
X      if ( i .le. nn ) go to 20
Xc
Xc              end of main loop.
Xc
Xc              compute square root and adjust for scaling.
Xc
X      dnrm2 = xmax * dsqrt(sum)
X  300 continue
X      return
X      end
X      subroutine  dscal(n,da,dx,incx)
Xc
Xc     scales a vector by a constant.
Xc     uses unrolled loops for increment equal to one.
Xc     jack dongarra, linpack, 3/11/78.
Xc
X      double precision da,dx(1)
X      integer i,incx,m,mp1,n,nincx
Xc
X      if(n.le.0)return
X      if(incx.eq.1)go to 20
Xc
Xc        code for increment not equal to 1
Xc
X      nincx = n*incx
X      do 10 i = 1,nincx,incx
X        dx(i) = da*dx(i)
X   10 continue
X      return
Xc
Xc        code for increment equal to 1
Xc
Xc
Xc        clean-up loop
Xc
X   20 m = mod(n,5)
X      if( m .eq. 0 ) go to 40
X      do 30 i = 1,m
X        dx(i) = da*dx(i)
X   30 continue
X      if( n .lt. 5 ) return
X   40 mp1 = m + 1
X      do 50 i = mp1,n,5
X        dx(i) = da*dx(i)
X        dx(i + 1) = da*dx(i + 1)
X        dx(i + 2) = da*dx(i + 2)
X        dx(i + 3) = da*dx(i + 3)
X        dx(i + 4) = da*dx(i + 4)
X   50 continue
X      return
X      end
X      double precision function ddot(n,dx,incx,dy,incy)
Xc
Xc     forms the dot product of two vectors.
Xc     uses unrolled loops for increments equal to one.
Xc     jack dongarra, linpack, 3/11/78.
Xc
X      double precision dx(1),dy(1),dtemp
X      integer i,incx,incy,ix,iy,m,mp1,n
Xc
X      ddot = 0.0d0
X      dtemp = 0.0d0
X      if(n.le.0)return
X      if(incx.eq.1.and.incy.eq.1)go to 20
Xc
Xc        code for unequal increments or equal increments
Xc          not equal to 1
Xc
X      ix = 1
X      iy = 1
X      if(incx.lt.0)ix = (-n+1)*incx + 1
X      if(incy.lt.0)iy = (-n+1)*incy + 1
X      do 10 i = 1,n
X        dtemp = dtemp + dx(ix)*dy(iy)
X        ix = ix + incx
X        iy = iy + incy
X   10 continue
X      ddot = dtemp
X      return
Xc
Xc        code for both increments equal to 1
Xc
Xc
Xc        clean-up loop
Xc
X   20 m = mod(n,5)
X      if( m .eq. 0 ) go to 40
X      do 30 i = 1,m
X        dtemp = dtemp + dx(i)*dy(i)
X   30 continue
X      if( n .lt. 5 ) go to 60
X   40 mp1 = m + 1
X      do 50 i = mp1,n,5
X        dtemp = dtemp + dx(i)*dy(i) + dx(i + 1)*dy(i + 1) +
X     *   dx(i + 2)*dy(i + 2) + dx(i + 3)*dy(i + 3) + dx(i + 4)*dy(i + 4)
X   50 continue
X   60 ddot = dtemp
X      return
X      end
X      subroutine daxpy(n,da,dx,incx,dy,incy)
Xc
Xc     constant times a vector plus a vector.
Xc     uses unrolled loops for increments equal to one.
Xc     jack dongarra, linpack, 3/11/78.
Xc
X      double precision dx(1),dy(1),da
X      integer i,incx,incy,ix,iy,m,mp1,n
Xc
X      if(n.le.0)return
X      if (da .eq. 0.0d0) return
X      if(incx.eq.1.and.incy.eq.1)go to 20
Xc
Xc        code for unequal increments or equal increments
Xc          not equal to 1
Xc
X      ix = 1
X      iy = 1
X      if(incx.lt.0)ix = (-n+1)*incx + 1
X      if(incy.lt.0)iy = (-n+1)*incy + 1
X      do 10 i = 1,n
X        dy(iy) = dy(iy) + da*dx(ix)
X        ix = ix + incx
X        iy = iy + incy
X   10 continue
X      return
Xc
Xc        code for both increments equal to 1
Xc
Xc
Xc        clean-up loop
Xc
X   20 m = mod(n,4)
X      if( m .eq. 0 ) go to 40
X      do 30 i = 1,m
X        dy(i) = dy(i) + da*dx(i)
X   30 continue
X      if( n .lt. 4 ) return
X   40 mp1 = m + 1
X      do 50 i = mp1,n,4
X        dy(i) = dy(i) + da*dx(i)
X        dy(i + 1) = dy(i + 1) + da*dx(i + 1)
X        dy(i + 2) = dy(i + 2) + da*dx(i + 2)
X        dy(i + 3) = dy(i + 3) + da*dx(i + 3)
X   50 continue
X      return
X      end
X      subroutine  dcopy(n,dx,incx,dy,incy)
Xc
Xc     copies a vector, x, to a vector, y.
Xc     uses unrolled loops for increments equal to one.
Xc     jack dongarra, linpack, 3/11/78.
Xc
X      double precision dx(1),dy(1)
X      integer i,incx,incy,ix,iy,m,mp1,n
Xc
X      if(n.le.0)return
X      if(incx.eq.1.and.incy.eq.1)go to 20
Xc
Xc        code for unequal increments or equal increments
Xc          not equal to 1
Xc
X      ix = 1
X      iy = 1
X      if(incx.lt.0)ix = (-n+1)*incx + 1
X      if(incy.lt.0)iy = (-n+1)*incy + 1
X      do 10 i = 1,n
X        dy(iy) = dx(ix)
X        ix = ix + incx
X        iy = iy + incy
X   10 continue
X      return
Xc
Xc        code for both increments equal to 1
Xc
Xc
Xc        clean-up loop
Xc
X   20 m = mod(n,7)
X      if( m .eq. 0 ) go to 40
X      do 30 i = 1,m
X        dy(i) = dx(i)
X   30 continue
X      if( n .lt. 7 ) return
X   40 mp1 = m + 1
X      do 50 i = mp1,n,7
X        dy(i) = dx(i)
X        dy(i + 1) = dx(i + 1)
X        dy(i + 2) = dx(i + 2)
X        dy(i + 3) = dx(i + 3)
X        dy(i + 4) = dx(i + 4)
X        dy(i + 5) = dx(i + 5)
X        dy(i + 6) = dx(i + 6)
X   50 continue
X      return
X      end
X      subroutine  dswap (n,dx,incx,dy,incy)
Xc
Xc     interchanges two vectors.
Xc     uses unrolled loops for increments equal one.
Xc     jack dongarra, linpack, 3/11/78.
Xc
X      double precision dx(1),dy(1),dtemp
X      integer i,incx,incy,ix,iy,m,mp1,n
Xc
X      if(n.le.0)return
X      if(incx.eq.1.and.incy.eq.1)go to 20
Xc
Xc       code for unequal increments or equal increments not equal
Xc         to 1
Xc
X      ix = 1
X      iy = 1
X      if(incx.lt.0)ix = (-n+1)*incx + 1
X      if(incy.lt.0)iy = (-n+1)*incy + 1
X      do 10 i = 1,n
X        dtemp = dx(ix)
X        dx(ix) = dy(iy)
X        dy(iy) = dtemp
X        ix = ix + incx
X        iy = iy + incy
X   10 continue
X      return
Xc
Xc       code for both increments equal to 1
Xc
Xc
Xc       clean-up loop
Xc
X   20 m = mod(n,3)
X      if( m .eq. 0 ) go to 40
X      do 30 i = 1,m
X        dtemp = dx(i)
X        dx(i) = dy(i)
X        dy(i) = dtemp
X   30 continue
X      if( n .lt. 3 ) return
X   40 mp1 = m + 1
X      do 50 i = mp1,n,3
X        dtemp = dx(i)
X        dx(i) = dy(i)
X        dy(i) = dtemp
X        dtemp = dx(i + 1)
X        dx(i + 1) = dy(i + 1)
X        dy(i + 1) = dtemp
X        dtemp = dx(i + 2)
X        dx(i + 2) = dy(i + 2)
X        dy(i + 2) = dtemp
X   50 continue
X      return
X      end
X      subroutine drotg(da,db,c,s)
Xc
Xc     construct givens plane rotation.
Xc     jack dongarra, linpack, 3/11/78.
Xc                    modified 9/27/86.
Xc
X      double precision da,db,c,s,roe,scale,r,z
Xc
X      roe = db
X      if( dabs(da) .gt. dabs(db) ) roe = da
X      scale = dabs(da) + dabs(db)
X      if( scale .ne. 0.0d0 ) go to 10
X         c = 1.0d0
X         s = 0.0d0
X         r = 0.0d0
X         go to 20
X   10 r = scale*dsqrt((da/scale)**2 + (db/scale)**2)
X      r = dsign(1.0d0,roe)*r
X      c = da/r
X      s = db/r
X   20 z = s
X      if( dabs(c) .gt. 0.0d0 .and. dabs(c) .le. s ) z = 1.0d0/c
X      da = r
X      db = z
X      return
X      end
X      subroutine  drot (n,dx,incx,dy,incy,c,s)
Xc
Xc     applies a plane rotation.
Xc     jack dongarra, linpack, 3/11/78.
Xc
X      double precision dx(1),dy(1),dtemp,c,s
X      integer i,incx,incy,ix,iy,n
Xc
X      if(n.le.0)return
X      if(incx.eq.1.and.incy.eq.1)go to 20
Xc
Xc       code for unequal increments or equal increments not equal
Xc         to 1
Xc
X      ix = 1
X      iy = 1
X      if(incx.lt.0)ix = (-n+1)*incx + 1
X      if(incy.lt.0)iy = (-n+1)*incy + 1
X      do 10 i = 1,n
X        dtemp = c*dx(ix) + s*dy(iy)
X        dy(iy) = c*dy(iy) - s*dx(ix)
X        dx(ix) = dtemp
X        ix = ix + incx
X        iy = iy + incy
X   10 continue
X      return
Xc
Xc       code for both increments equal to 1
Xc
X   20 do 30 i = 1,n
X        dtemp = c*dx(i) + s*dy(i)
X        dy(i) = c*dy(i) - s*dx(i)
X        dx(i) = dtemp
X   30 continue
X      return
X      end
X      double precision function dasum(n,dx,incx)
Xc
Xc     takes the sum of the absolute values.
Xc     jack dongarra, linpack, 3/11/78.
Xc
X      double precision dx(1),dtemp
X      integer i,incx,m,mp1,n,nincx
Xc
X      dasum = 0.0d0
X      dtemp = 0.0d0
X      if(n.le.0)return
X      if(incx.eq.1)go to 20
Xc
Xc        code for increment not equal to 1
Xc
X      nincx = n*incx
X      do 10 i = 1,nincx,incx
X        dtemp = dtemp + dabs(dx(i))
X   10 continue
X      dasum = dtemp
X      return
Xc
Xc        code for increment equal to 1
Xc
Xc
Xc        clean-up loop
Xc
X   20 m = mod(n,6)
X      if( m .eq. 0 ) go to 40
X      do 30 i = 1,m
X        dtemp = dtemp + dabs(dx(i))
X   30 continue
X      if( n .lt. 6 ) go to 60
X   40 mp1 = m + 1
X      do 50 i = mp1,n,6
X        dtemp = dtemp + dabs(dx(i)) + dabs(dx(i + 1)) + dabs(dx(i + 2))
X     *  + dabs(dx(i + 3)) + dabs(dx(i + 4)) + dabs(dx(i + 5))
X   50 continue
X   60 dasum = dtemp
X      return
X      end
END_OF_FILE
  if test 26964 -ne `wc -c <'fdlin.f'`; then
    echo shar: \"'fdlin.f'\" unpacked with wrong size!
  fi
  # end of 'fdlin.f'
fi
if test -f 'fdgam.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdgam.f'\"
else
  echo shar: Extracting \"'fdgam.f'\" \(21685 characters\)
  sed "s/^X//" >'fdgam.f' <<'END_OF_FILE'
X      double precision function dgamma (x)
Xc jan 1984 edition.  w. fullerton, c3, los alamos scientific lab.
XC     double precision x, gamcs(42), dxrel, pi, sinpiy, sq2pil, xmax,
XC    1  xmin, y, d9lgmc, dcsevl, d1mach, dexp, dint, dlog,
XC    2  dsin, dsqrt
X      double precision x, gamcs(42), dxrel, pi, sinpiy, sq2pil, xmax,
X     1  xmin, y, d9lgmc, dcsevl
X      double precision temp
XC     external d1mach, d9lgmc, dcsevl, dexp, dint, dlog, dsin, dsqrt,
XC    1  initds
X      external d9lgmc, dcsevl, initds
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
Xc
Xc series for gam        on the interval  0.          to  1.00000e+00
Xc                                        with weighted error   5.79e-32
Xc                                         log weighted error  31.24
Xc                               significant figures required  30.00
Xc                                    decimal places required  32.05
Xc
X      data gam cs(  1) / +.8571195590 9893314219 2006239994 2 d-2      /
X      data gam cs(  2) / +.4415381324 8410067571 9131577165 2 d-2      /
X      data gam cs(  3) / +.5685043681 5993633786 3266458878 9 d-1      /
X      data gam cs(  4) / -.4219835396 4185605010 1250018662 4 d-2      /
X      data gam cs(  5) / +.1326808181 2124602205 8400679635 2 d-2      /
X      data gam cs(  6) / -.1893024529 7988804325 2394702388 6 d-3      /
X      data gam cs(  7) / +.3606925327 4412452565 7808221722 5 d-4      /
X      data gam cs(  8) / -.6056761904 4608642184 8554829036 5 d-5      /
X      data gam cs(  9) / +.1055829546 3022833447 3182350909 3 d-5      /
X      data gam cs( 10) / -.1811967365 5423840482 9185589116 6 d-6      /
X      data gam cs( 11) / +.3117724964 7153222777 9025459316 9 d-7      /
X      data gam cs( 12) / -.5354219639 0196871408 7408102434 7 d-8      /
X      data gam cs( 13) / +.9193275519 8595889468 8778682594 0 d-9      /
X      data gam cs( 14) / -.1577941280 2883397617 6742327395 3 d-9      /
X      data gam cs( 15) / +.2707980622 9349545432 6654043308 9 d-10     /
X      data gam cs( 16) / -.4646818653 8257301440 8166105893 3 d-11     /
X      data gam cs( 17) / +.7973350192 0074196564 6076717535 9 d-12     /
X      data gam cs( 18) / -.1368078209 8309160257 9949917230 9 d-12     /
X      data gam cs( 19) / +.2347319486 5638006572 3347177168 8 d-13     /
X      data gam cs( 20) / -.4027432614 9490669327 6657053469 9 d-14     /
X      data gam cs( 21) / +.6910051747 3721009121 3833697525 7 d-15     /
X      data gam cs( 22) / -.1185584500 2219929070 5238712619 2 d-15     /
X      data gam cs( 23) / +.2034148542 4963739552 0102605193 2 d-16     /
X      data gam cs( 24) / -.3490054341 7174058492 7401294910 8 d-17     /
X      data gam cs( 25) / +.5987993856 4853055671 3505106602 6 d-18     /
X      data gam cs( 26) / -.1027378057 8722280744 9006977843 1 d-18     /
X      data gam cs( 27) / +.1762702816 0605298249 4275966074 8 d-19     /
X      data gam cs( 28) / -.3024320653 7353062609 5877211204 2 d-20     /
X      data gam cs( 29) / +.5188914660 2183978397 1783355050 6 d-21     /
X      data gam cs( 30) / -.8902770842 4565766924 4925160106 6 d-22     /
X      data gam cs( 31) / +.1527474068 4933426022 7459689130 6 d-22     /
X      data gam cs( 32) / -.2620731256 1873629002 5732833279 9 d-23     /
X      data gam cs( 33) / +.4496464047 8305386703 3104657066 6 d-24     /
X      data gam cs( 34) / -.7714712731 3368779117 0390152533 3 d-25     /
X      data gam cs( 35) / +.1323635453 1260440364 8657271466 6 d-25     /
X      data gam cs( 36) / -.2270999412 9429288167 0231381333 3 d-26     /
X      data gam cs( 37) / +.3896418998 0039914493 2081663999 9 d-27     /
X      data gam cs( 38) / -.6685198115 1259533277 9212799999 9 d-28     /
X      data gam cs( 39) / +.1146998663 1400243843 4761386666 6 d-28     /
X      data gam cs( 40) / -.1967938586 3451346772 9510399999 9 d-29     /
X      data gam cs( 41) / +.3376448816 5853380903 3489066666 6 d-30     /
X      data gam cs( 42) / -.5793070335 7821357846 2549333333 3 d-31     /
Xc
X      data pi / 3.1415926535 8979323846 2643383279 50 d0 /
Xc sq2pil is 0.5*alog(2*pi) = alog(sqrt(2*pi))
X      data sq2pil / 0.9189385332 0467274178 0329736405 62 d0 /
X      data ngam, xmin, xmax, xsml, dxrel / 0, 4*0.d0 /
Xc
X      if (ngam.ne.0) go to 10
XC     ngam = initds (gamcs, 42, 0.1*sngl(  d1mach) )
X      ngam = initds (gamcs, 42, 0.1*sngl(  EPSMIN ) )
Xc
X      call d9gaml (xmin, xmax)
X      if (IGAMMA .ne. 0) return
XC     xsml = dexp (dmax1 (dlog(d1mach(1)), -dlog(d1mach(2)))+0.01d0)
X      xsml =  exp ( max  ( log( FLTMIN  ), - log( FLTMAX  ))+0.01d0)
XC     dxrel = dsqrt (d1mach(4))
X      dxrel =  sqrt (  EPSMAX )
Xc
XC10   y = dabs(x)
X 10   y =  abs(x)
X      if (y.gt.10.d0) go to 50
Xc
Xc compute gamma(x) for -xbnd .le. x .le. xbnd.  reduce interval and find
Xc gamma(1+y) for 0.0 .le. y .lt. 1.0 first of all.
Xc
X      n = int(x)
X      if (x.lt.0.d0) n = n - 1
X      y = x - dble(float(n))
X      n = n - 1
XC     dgamma = 0.9375d0 + dcsevl (2.d0*y-1.d0, gamcs, ngam)
X      temp = dcsevl (2.d0*y-1.d0, gamcs, ngam) 
X      if (IGAMMA .ne. 0) return
X      dgamma = 0.9375d0 + temp
X      if (n.eq.0) return
Xc
X      if (n.gt.0) go to 30
Xc
Xc compute gamma(x) for x .lt. 1.0
Xc
X      n = -n
X
XC     if (x.eq.0.d0) call seteru (14hdgamma  x is 0, 14, 4, 2)
XC     if (x.lt.0.0d0 .and. x+dble(float(n-2)).eq.0.d0) call seteru (
XC    1  31hdgamma  x is a negative integer, 31, 4, 2)
XC     if (x.lt.(-0.5d0) .and. dabs((x-dint(x-0.5d0))/x).lt.dxrel) call
XC    1  seteru (68hdgamma  answer lt half precision because x too near n
XC    2egative integer, 68, 1, 1)
XC     if (y.lt.xsml) call seteru (
XC    1  54hdgamma  x is so close to 0.0 that the result overflows,
XC    2  54, 5, 2)
X
X      if (x.eq.0.d0) then
XC       write(6,*) 'dgamma : x is 0'
X        IGAMMA = 11
X        return
X      end if
X
X      if (x.lt.0.0d0 .and. x+dble(float(n-2)).eq.0.d0) then
XC       write( 6, *) 'dgamma : x is a negative integer'
X        IGAMMA = 12
X        return
X      end if
X
X      if (x.lt.(-0.5d0) .and. abs((x-dble(int(x-0.5d0)))/x).lt.dxrel)
XC    1  write(6,*) 'dgamma : answer lt half precision because
XC    2                       x too near a negative integer'
X     *  JGAMMA = 11
X
X      if (y.lt.xsml) then
Xc       write(6,*)  'dgamma :,
Xc    1               x is so close to 0.0 that the result overflows'
X        IGAMMA = 13
X        return
X      end if
Xc
X      do 20 i=1,n
X        dgamma = dgamma/(x+dble(float(i-1)) )
X 20   continue
X      return
Xc
Xc gamma(x) for x .ge. 2.0 and x .le. 10.0
Xc
X 30   do 40 i=1,n
X        dgamma = (y+dble(float(i))) * dgamma
X 40   continue
X      return
Xc
Xc gamma(x) for dabs(x) .gt. 10.0.  recall y = dabs(x).
Xc
XC50   if (x.gt.xmax) call seteru (32hdgamma  x so big gamma overflows,
XC    1  32, 3, 2)
X
X 50   if (x.gt.xmax) then
Xc       write(6,*) 'dgamma : x so big gamma overflows'
X        IGAMMA = 14
X        return
X      end if
Xc
X      dgamma = 0.d0
XC     if (x.lt.xmin) call seteru (35hdgamma  x so small gamma underflows
XC    1  , 35, 2, 0)
XC     if (x.lt.xmin) return
X
X      if (x.lt.xmin) then
Xc       write(6,*) 'dgamma : x so small gamma underflows'
X        JGAMMA = 12
X        return
X      end if
Xc
XC     dgamma = dexp ((y-0.5d0)*dlog(y) - y + sq2pil + d9lgmc(y) )
X      temp = d9lgmc(y)
X      if (IGAMMA .ne. 0) return
X      dgamma =  exp ((y-0.5d0)* log(y) - y + sq2pil + temp)
X      if (x.gt.0.d0) return
Xc
XC     if (dabs((x-dint(x-0.5d0))/x).lt.dxrel) call seteru (
XC    1  61hdgamma  answer lt half precision, x too near negative integer
XC    2  , 61, 1, 1)
X
X      if (abs((x-dble(int(x-0.5d0)))/x).lt.dxrel) JGAMMA = 11
Xc
XC     sinpiy = dsin (pi*y)
X      sinpiy =  sin (pi*y)
XC     if (sinpiy.eq.0.d0) call seteru (
XC    1  31hdgamma  x is a negative integer, 31, 4, 2)
X 
X      if (sinpiy.eq.0.d0) then
XC       write(6,*) 'dgamma : x is a negative integer'
X        IGAMMA = 12
X        return
X      end if
Xc
X      dgamma = -pi/(y*sinpiy*dgamma)
Xc
X      return
X      end
X      double precision function dgamr (x)
Xc july 1977 edition.  w. fullerton, c3, los alamos scientific lab.
Xc this routine, not dgamma(x), should be the fundamental one.
Xc
XC     double precision x, alngx, sgngx, dgamma, dint, dexp, d1mach
X      double precision x, alngx, sgngx, dgamma
X
XC     external dexp, dgamma, dint, d1mach
X      external dgamma
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
Xc
X      dgamr = 0.0d0
XC     if (x.le.0.0d0 .and. dint(x).eq.x) return
X      if (x.le.0.0d0 .and. dble(int(x)).eq.x) return
Xc
XC     call entsrc (irold, 1)
X      if (dabs(x).gt.10.0d0) go to 10
XC     dgamr = 1.0d0/dgamma(x)
XC     call erroff
XC     call entsrc (ir, irold)
X      temp = dgamma(x)
X      if (IGAMMA .ne. 0) then
XC       dgamr = d1mach(2)
X        dgamr = FLTMAX
X        return
X      end if
X      dgamr = 1.0d0/temp
X      return
Xc
X 10   call dlgams (x, alngx, sgngx)
X      if (IGAMMA .ne. 0) return
XC     call erroff
XC     call entsrc (ir, irold)
XC     dgamr = sgngx * dexp(-alngx)
X      dgamr = sgngx *  exp(-alngx)
X      return
Xc
X      end
X      subroutine dlgams (x, dlgam, sgngam)
Xc july 1977 edition.  w. fullerton, c3, los alamos scientific lab.
Xc
Xc evaluate log abs (gamma(x)) and return the sign of gamma(x) in sgngam.
Xc sgngam is either +1.0 or -1.0.
Xc
XC     double precision x, dlgam, sgngam, dint, dlngam
X      double precision x, dlgam, sgngam, dlngam
XC     external dint, dlngam
X      external dlngam
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
Xc
X      dlgam = dlngam(x)
X      if (IGAMMA .ne. 0) return
X      sgngam = 1.0d0
X      if (x.gt.0.d0) return
Xc
XC     int = dmod (-dint(x), 2.0d0) + 0.1d0
XC     if (int.eq.0) sgngam = -1.0d0
X      intx =  mod (-dble(int(x)), 2.0d0) + 0.1d0
X      if (intx.eq.0) sgngam = -1.0d0
Xc
X      return
X      end
X      function initds (dos, nos, eta)
Xc june 1977 edition.   w. fullerton, c3, los alamos scientific lab.
Xc
Xc initialize the double precision orthogonal series dos so that initds
Xc is the number of terms needed to insure the error is no larger than
Xc eta.  ordinarily eta will be chosen to be one-tenth machine precision.
Xc
Xc             input arguments --
Xc dos    dble prec array of nos coefficients in an orthogonal series.
Xc nos    number of coefficients in dos.
Xc eta    requested accuracy of series.
Xc
X      double precision dos(nos)
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
Xc
XC     if (nos.lt.1) call seteru (
XC    1  35hinitds  number of coefficients lt 1, 35, 2, 2)
X      if (nos.lt.1) JGAMMA = 31
Xc
X      err = 0.
X      do 10 ii=1,nos
X        i = nos + 1 - ii
X        err = err + abs(sngl(dos(i)))
X        if (err.gt.eta) go to 20
X 10   continue
Xc
XC20   if (i.eq.nos) call seteru (28hinitds  eta may be too small, 28,
XC    1  1, 2)
X 20   continue
XC     if (i.eq.nos) write(6,*) 'initds : eta may be too small'
X      if (i.eq.nos) JGAMMA = 32
X      initds = i
Xc
X      return
X      end
X      subroutine d9gaml (xmin, xmax)
Xc june 1977 edition.   w. fullerton, c3, los alamos scientific lab.
Xc
Xc calculate the minimum and maximum legal bounds for x in gamma(x).
Xc xmin and xmax are not the only bounds, but they are the only non-
Xc trivial ones to calculate.
Xc
Xc             output arguments --
Xc xmin   dble prec minimum legal value of x in gamma(x).  any smaller
Xc        value of x might result in underflow.
Xc xmax   dble prec maximum legal value of x in gamma(x).  any larger
Xc        value of x might cause overflow.
Xc
XC     double precision xmin, xmax, alnbig, alnsml, xln, xold, d1mach,
XC    1  dlog
X      double precision xmin, xmax, alnbig, alnsml, xln, xold
XC     external d1mach, dlog
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
Xc
XC     alnsml = dlog(d1mach(1))
X      alnsml =  log( FLTMIN  )
X      xmin = -alnsml
X      do 10 i=1,10
X        xold = xmin
XC       xln = dlog(xmin)
X        xln =  log(xmin)
X        xmin = xmin - xmin*((xmin+0.5d0)*xln - xmin - 0.2258d0 + alnsml)
X     1    / (xmin*xln+0.5d0)
XC       if (dabs(xmin-xold).lt.0.005d0) go to 20
X        if ( abs(xmin-xold).lt.0.005d0) go to 20
X 10   continue
XC     call seteru (27hd9gaml  unable to find xmin, 27, 1, 2)
XC     write(6,*) 'd9gaml : unable to find xmin'
X      IGAMMA = 21
X      return
X
Xc
X 20   xmin = -xmin + 0.01d0
Xc
XC     alnbig = dlog (d1mach(2))
X      alnbig =  log ( FLTMAX  )
X      xmax = alnbig
X      do 30 i=1,10
X        xold = xmax
XC       xln = dlog(xmax)
X        xln =  log(xmax)
X        xmax = xmax - xmax*((xmax-0.5d0)*xln - xmax + 0.9189d0 - alnbig)
X     1    / (xmax*xln-0.5d0)
XC       if (dabs(xmax-xold).lt.0.005d0) go to 40
X        if ( abs(xmax-xold).lt.0.005d0) go to 40
X 30   continue
XC     call seteru (27hd9gaml  unable to find xmax, 27, 2, 2)
XC     write(6,*) 'd9gaml : unable to find xmax'
X      IGAMMA = 22
X      return
Xc
X 40   xmax = xmax - 0.01d0
X      xmin = dmax1 (xmin, -xmax+1.d0)
Xc
X      return
X      end
X      double precision function d9lgmc (x)
Xc august 1977 edition.  w. fullerton, c3, los alamos scientific lab.
Xc
Xc compute the log gamma correction factor for x .ge. 10. so that
Xc dlog (dgamma(x)) = dlog(dsqrt(2*pi)) + (x-.5)*dlog(x) - x + d9lgmc(x)
Xc
XC     double precision x, algmcs(15), xbig, xmax, dcsevl, d1mach,
XC    1  dexp, dlog, dsqrt
X      double precision x, algmcs(15), xbig, xmax, dcsevl
X      double precision temp
XC     external d1mach, dcsevl, dexp, dlog, dsqrt, initds
X      external dcsevl, initds
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
Xc
Xc series for algm       on the interval  0.          to  1.00000e-02
Xc                                        with weighted error   1.28e-31
Xc                                         log weighted error  30.89
Xc                               significant figures required  29.81
Xc                                    decimal places required  31.48
Xc
X      data algmcs(  1) / +.1666389480 4518632472 0572965082 2 d+0      /
X      data algmcs(  2) / -.1384948176 0675638407 3298605913 5 d-4      /
X      data algmcs(  3) / +.9810825646 9247294261 5717154748 7 d-8      /
X      data algmcs(  4) / -.1809129475 5724941942 6330626671 9 d-10     /
X      data algmcs(  5) / +.6221098041 8926052271 2601554341 6 d-13     /
X      data algmcs(  6) / -.3399615005 4177219443 0333059966 6 d-15     /
X      data algmcs(  7) / +.2683181998 4826987489 5753884666 6 d-17     /
X      data algmcs(  8) / -.2868042435 3346432841 4462239999 9 d-19     /
X      data algmcs(  9) / +.3962837061 0464348036 7930666666 6 d-21     /
X      data algmcs( 10) / -.6831888753 9857668701 1199999999 9 d-23     /
X      data algmcs( 11) / +.1429227355 9424981475 7333333333 3 d-24     /
X      data algmcs( 12) / -.3547598158 1010705471 9999999999 9 d-26     /
X      data algmcs( 13) / +.1025680058 0104709120 0000000000 0 d-27     /
X      data algmcs( 14) / -.3401102254 3167487999 9999999999 9 d-29     /
X      data algmcs( 15) / +.1276642195 6300629333 3333333333 3 d-30     /
Xc
X      data nalgm, xbig, xmax / 0, 2*0.d0 /
Xc
X      if (nalgm.ne.0) go to 10
XC     nalgm = initds (algmcs, 15, sngl(d1mach(3)) )
X      nalgm = initds (algmcs, 15, sngl(  EPSMIN ) )
XC     xbig = 1.0d0/dsqrt(d1mach(3))
X      xbig = 1.0d0/ sqrt(  EPSMIN )
XC     xmax = dexp (dmin1(dlog(d1mach(2)/12.d0), -dlog(12.d0*d1mach(1))))
X      xmax =  exp ( min ( log(FLTMAX   /12.d0), - log(12.d0*FLTMIN   )))
Xc
XC10   if (x.lt.10.d0) call seteru (23hd9lgmc  x must be ge 10, 23, 1, 2)
Xc
X 10   if (x.lt.10.d0) then
Xc       write(6,*) 'd9lgmc : x must be ge 10'
X        IGAMMA = 51
XC       d9lgmc = d1mach(2)
X        d9lgmc = FLTMAX
X        return
X      end if
X
X      if (x.ge.xmax) go to 20
Xc
X      d9lgmc = 1.d0/(12.d0*x)
XC     if (x.lt.xbig) d9lgmc = dcsevl (2.0d0*(10.d0/x)**2-1.d0, algmcs,
XC    1  nalgm) / x
X
X      if (x.lt.xbig) then
X        temp   = dcsevl(2.0d0*(10.d0/x)**2-1.d0, algmcs, nalgm)
X        if (IGAMMA .ne. 0) then
XC         d9gmlc = d1mach(2)
X          d9gmlc = FLTMAX
X        else
X          d9lgmc = temp / x
X        end if
X      end if
X      return
Xc
X 20   d9lgmc = 0.d0
XC     call seteru (34hd9lgmc  x so big d9lgmc underflows, 34, 2, 0)
Xc     write(6,*) 'd9lgmc : x so big d9lgmc underflows'
X      JGAMMA = 51
X      return
Xc
X      end
X      double precision function dcsevl (x, a, n)
Xc
Xc evaluate the n-term chebyshev series a at x.  adapted from
Xc r. broucke, algorithm 446, c.a.c.m., 16, 254 (1973).
Xc
Xc             input arguments --
Xc x      dble prec value at which the series is to be evaluated.
Xc a      dble prec array of n terms of a chebyshev series.  in eval-
Xc        uating a, only half the first coef is summed.
Xc n      number of terms in array a.
Xc
X      double precision a(n), x, twox, b0, b1, b2
XC     double precision d1mach
XC     external         d1mach
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
X
Xc
XC     if (n.lt.1) call seteru (28hdcsevl  number of terms le 0, 28, 2,2)
XC     if (n.gt.1000) call seteru (31hdcsevl  number of terms gt 1000,
XC    1  31, 3, 2)
XC     if (x.lt.(-1.1d0) .or. x.gt.1.1d0) call seteru (
XC    1  25hdcsevl  x outside (-1,+1), 25, 1, 1)
Xc
X      if (n.lt.1) then
XC       write(6,*) 'dcsevl : number of terms le 0'
X        IGAMMA = 41
XC       dcsevl = d1mach(2)
X        dcsevl = FLTMAX
X        return
X      end if
X
X      if (n.gt.1000) then
XC       write(6,*) 'dcsevl : number of terms gt 1000'
X        IGAMMA = 42
XC       dcsevl = d1mach(2)
X        dcsevl = FLTMAX
X        return
X      end if
X
X      if (x.lt.(-1.1d0) .or. x.gt.1.1d0) then
XC       write(6,*) 'dcsevl : x outside (-1,+1)'
X        IGAMMA = 43
XC       dcsevl = d1mach(2)
X        dcsevl = FLTMAX
X        return
X      end if
Xc
X      twox = 2.0d0*x
X      b1 = 0.d0
X      b0 = 0.d0
X      do 10 i=1,n
X        b2 = b1
X        b1 = b0
X        ni = n - i + 1
X        b0 = twox*b1 - b2 + a(ni)
X 10   continue
Xc
X      dcsevl = 0.5d0 * (b0-b2)
Xc
X      return
X      end
X      double precision function dlngam (x)
Xc august 1980 edition.   w. fullerton, c3, los alamos scientific lab.
XC     double precision x, dxrel, pi, sinpiy, sqpi2l, sq2pil,
XC    1  y, xmax, dint, dgamma, d9lgmc, d1mach, dlog, dsin, dsqrt
X      double precision x, dxrel, pi, sinpiy, sqpi2l, sq2pil,
X     1  y, xmax, dgamma, d9lgmc
X      double precision   temp
XC     external d1mach, d9lgmc, dgamma, dint, dlog, dsin, dsqrt
X      external d9lgmc, dgamma
X
X      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
X      save   /MACHFD/
X
X      integer            IGAMMA, JGAMMA
X      common /GAMMFD/    IGAMMA, JGAMMA
X      save   /GAMMFD/
Xc
X      data sq2pil / 0.9189385332 0467274178 0329736405 62 d0 /
Xc sq2pil = alog (sqrt(2*pi)),  sqpi2l = alog(sqrt(pi/2))
X      data sqpi2l / +.2257913526 4472743236 3097614947 441 d+0    /
X      data pi / 3.1415926535 8979323846 2643383279 50 d0 /
Xc
X      data xmax, dxrel / 2*0.d0 /
Xc
X      if (xmax.ne.0.d0) go to 10
XC     xmax = d1mach(2)/dlog(d1mach(2))
X      xmax =  FLTMAX  / log( FLTMAX  )
XC     dxrel = dsqrt (d1mach(4))
X      dxrel =  sqrt ( FLTMAX  )
Xc
XC10   y = dabs (x)
X 10   y =  abs (x)
X      if (y.gt.10.d0) go to 20
Xc
Xc dlog (dabs (dgamma(x)) ) for dabs(x) .le. 10.0
Xc
XC     dlngam = dlog (dabs (dgamma(x)) )
X      temp   = dgamma(x)
X      if (IGAMMA .ne. 0) then
XC       dlngam = d1mach(2)
X        dlngam = FLTMAX
X        return
X      end if
X      dlngam = log (abs (temp) )
X      return
Xc
Xc dlog ( dabs (dgamma(x)) ) for dabs(x) .gt. 10.0
Xc
XC20   if (y.gt.xmax) call seteru (
XC    1  39hdlngam  dabs(x) so big dlngam overflows, 39, 2, 2)
X 
X 20   if (y.gt.xmax) then
Xc       write(6,*) 'dlngam : abs(x) so big dlngam overflows'
X        IGAMMA = 61
XC       dlngam = d1mach(2)
X        dlngam = FLTMAX
X        return
X      end if
Xc
XC     if (x.gt.0.d0) dlngam = sq2pil + (x-0.5d0)*dlog(x) - x + d9lgmc(y)
X       
X      temp = d9lgmc(y)
X      if (IGAMMA .ne. 0) then
XC       dlngam = d1mach(2)
X        dlngam = FLTMAX
X        return
X      end if
X
X      if (x.gt.0.d0) dlngam = sq2pil + (x-0.5d0)*log(x) - x + temp
X      if (x.gt.0.d0) return
Xc
XC     sinpiy = dabs (dsin(pi*y))
X      sinpiy =  abs ( sin(pi*y))
XC     if (sinpiy.eq.0.d0) call seteru (
XC    1  31hdlngam  x is a negative integer, 31, 3, 2)
X 
X      if (sinpiy.eq.0.d0) then
Xc       write(6,*) 'dlngam : x is a negative integer'
X        IGAMMA = 62
XC       dlngam = d1mach(2)
X        dlngam = FLTMAX
X        return
X      end if
Xc
XC     dlngam = sqpi2l + (x-0.5d0)*dlog(y) - x - dlog(sinpiy) - d9lgmc(y)
X
X      temp = d9lgmc(y)
X      if (IGAMMA .ne. 0) then
XC       dlngam = d1mach(2)
X        dlngam = FLTMAX
X        return
X      end if
X
X      dlngam = sqpi2l + (x-0.5d0)*log(y) - x - log(sinpiy) - temp
Xc
XC     if (dabs((x-dint(x-0.5d0))*dlngam/x).lt.dxrel) call seteru (
XC    1  68hdlngam  answer lt half precision because x too near negative
XC    2integer, 68, 1, 1)
X      if ( abs((x-dble(int(x-0.5d0)))*dlngam/x).lt.dxrel) JGAMMA = 61
X
X      return
Xc
X      end
END_OF_FILE
  if test 21685 -ne `wc -c <'fdgam.f'`; then
    echo shar: \"'fdgam.f'\" unpacked with wrong size!
  fi
  # end of 'fdgam.f'
fi
if test -f 'fdran.f' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'fdran.f'\"
else
  echo shar: Extracting \"'fdran.f'\" \(21583 characters\)
  sed "s/^X//" >'fdran.f' <<'END_OF_FILE'
X      REAL FUNCTION gennor(av,sd)
XC**********************************************************************
XC
XC     REAL FUNCTION GENNOR( AV, SD )
XC
XC         GENerate random deviate from a NORmal distribution
XC
XC
XC                              Function
XC
XC
XC     Generates a single random deviate from a normal distribution
XC     with mean, AV, and standard deviation, SD.
XC
XC
XC                              Arguments
XC
XC
XC     AV --> Mean of the normal distribution.
XC                              REAL AV
XC
XC     SD --> Standard deviation of the normal distribution.
XC                              REAL SD
XC
XC     GENNOR <-- Generated normal deviate.
XC                              REAL GENNOR
XC
XC
XC                              Method
XC
XC
XC     Renames SNORM from TOMS as slightly modified by BWB to use RANF
XC     instead of SUNIF.
XC
XC     For details see:
XC               Ahrens, J.H. and Dieter, U.
XC               Extensions of Forsythe's Method for Random
XC               Sampling from the Normal Distribution.
XC               Math. Comput., 27,124 (Oct. 1973), 927 - 937.
XC
XC
XC**********************************************************************
XC     .. Scalar Arguments ..
X      REAL av,sd
XC     ..
XC     .. External Functions ..
X      REAL snorm
X      EXTERNAL snorm
XC     ..
XC     .. Executable Statements ..
X      gennor = sd*snorm() + av
X      RETURN
X
X      END
X      SUBROUTINE getcgn(g)
X      INTEGER g
XC**********************************************************************
XC
XC      SUBROUTINE GETCGN(G)
XC                         Get GeNerator
XC
XC     Returns in G the number of the current random number generator
XC
XC
XC                              Arguments
XC
XC
XC     G <-- Number of the current random number generator (1..32)
XC                    INTEGER G
XC
XC**********************************************************************
XC
X      INTEGER curntg,numg
X      SAVE curntg
X      PARAMETER (numg=32)
X      DATA curntg/1/
XC
X      g = curntg
X      RETURN
X
X      ENTRY setcgn(g)
XC**********************************************************************
XC
XC     SUBROUTINE SETCGN( G )
XC                      Set GeNerator
XC
XC     Sets  the  current  generator to G.    All references to a generat
XC     are to the current generator.
XC
XC
XC                              Arguments
XC
XC
XC     G --> Number of the current random number generator (1..32)
XC                    INTEGER G
XC
XC**********************************************************************
XC
XC     Abort if generator number out of range
XC
X      IF (.NOT. (g.LT.0.OR.g.GT.numg)) GO TO 10
X      WRITE (*,*) ' Generator number out of range in SETCGN:',
X     +  ' Legal range is 1 to ',numg,' -- ABORT!'
X      STOP ' Generator number out of range in SETCGN'
X
X   10 curntg = g
X      RETURN
X
X      END
X      INTEGER FUNCTION ignlgi()
XC**********************************************************************
XC
XC     INTEGER FUNCTION IGNLGI()
XC               GeNerate LarGe Integer
XC
XC     Returns a random integer following a uniform distribution over
XC     (1, 2147483562) using the current generator.
XC
XC     This is a transcription from Pascal to Fortran of routine
XC     Random from the paper
XC
XC     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
XC     with Splitting Facilities." ACM Transactions on Mathematical
XC     Software, 17:98-111 (1991)
XC
XC**********************************************************************
XC     .. Parameters ..
X      INTEGER numg
X      PARAMETER (numg=32)
XC     ..
XC     .. Scalars in Common ..
X      INTEGER a1,a1vw,a1w,a2,a2vw,a2w,m1,m2
XC     ..
XC     .. Arrays in Common ..
X      INTEGER cg1(numg),cg2(numg),ig1(numg),ig2(numg),lg1(numg),
X     +        lg2(numg)
X      LOGICAL qanti(numg)
XC     ..
XC     .. Local Scalars ..
X      INTEGER curntg,k,s1,s2,z
X      LOGICAL qqssd
XC     ..
XC     .. External Functions ..
X      LOGICAL qrgnin
X      EXTERNAL qrgnin
XC     ..
XC     .. External Subroutines ..
X      EXTERNAL getcgn,inrgcm,rgnqsd,setall
XC     ..
XC     .. Common blocks ..
X      COMMON /globe/m1,m2,a1,a2,a1w,a2w,a1vw,a2vw,ig1,ig2,lg1,lg2,cg1,
X     +       cg2,qanti
XC     ..
XC     .. Save statement ..
X      SAVE /globe/
XC     ..
XC     .. Executable Statements ..
XC
XC     IF THE RANDOM NUMBER PACKAGE HAS NOT BEEN INITIALIZED YET, DO SO.
XC     IT CAN BE INITIALIZED IN ONE OF TWO WAYS : 1) THE FIRST CALL TO
XC     THIS ROUTINE  2) A CALL TO SETALL.
XC
X      IF (.NOT. (qrgnin())) CALL inrgcm()
X      CALL rgnqsd(qqssd)
X      IF (.NOT. (qqssd)) CALL setall(1234567890,123456789)
XC
XC     Get Current Generator
XC
X      CALL getcgn(curntg)
X      s1 = cg1(curntg)
X      s2 = cg2(curntg)
X      k = s1/53668
X      s1 = a1* (s1-k*53668) - k*12211
X      IF (s1.LT.0) s1 = s1 + m1
X      k = s2/52774
X      s2 = a2* (s2-k*52774) - k*3791
X      IF (s2.LT.0) s2 = s2 + m2
X      cg1(curntg) = s1
X      cg2(curntg) = s2
X      z = s1 - s2
X      IF (z.LT.1) z = z + m1 - 1
X      IF (qanti(curntg)) z = m1 - z
X      ignlgi = z
X      RETURN
X
X      END
X      SUBROUTINE initgn(isdtyp)
XC**********************************************************************
XC
XC     SUBROUTINE INITGN(ISDTYP)
XC          INIT-ialize current G-e-N-erator
XC
XC     Reinitializes the state of the current generator
XC
XC     This is a transcription from Pascal to Fortran of routine
XC     Init_Generator from the paper
XC
XC     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
XC     with Splitting Facilities." ACM Transactions on Mathematical
XC     Software, 17:98-111 (1991)
XC
XC
XC                              Arguments
XC
XC
XC     ISDTYP -> The state to which the generator is to be set
XC
XC          ISDTYP = -1  => sets the seeds to their initial value
XC          ISDTYP =  0  => sets the seeds to the first value of
XC                          the current block
XC          ISDTYP =  1  => sets the seeds to the first value of
XC                          the next block
XC
XC                                   INTEGER ISDTYP
XC
XC**********************************************************************
XC     .. Parameters ..
X      INTEGER numg
X      PARAMETER (numg=32)
XC     ..
XC     .. Scalar Arguments ..
X      INTEGER isdtyp
XC     ..
XC     .. Scalars in Common ..
X      INTEGER a1,a1vw,a1w,a2,a2vw,a2w,m1,m2
XC     ..
XC     .. Arrays in Common ..
X      INTEGER cg1(numg),cg2(numg),ig1(numg),ig2(numg),lg1(numg),
X     +        lg2(numg)
X      LOGICAL qanti(numg)
XC     ..
XC     .. Local Scalars ..
X      INTEGER g
XC     ..
XC     .. External Functions ..
X      LOGICAL qrgnin
X      INTEGER mltmod
X      EXTERNAL qrgnin,mltmod
XC     ..
XC     .. External Subroutines ..
X      EXTERNAL getcgn
XC     ..
XC     .. Common blocks ..
X      COMMON /globe/m1,m2,a1,a2,a1w,a2w,a1vw,a2vw,ig1,ig2,lg1,lg2,cg1,
X     +       cg2,qanti
XC     ..
XC     .. Save statement ..
X      SAVE /globe/
XC     ..
XC     .. Executable Statements ..
XC     Abort unless random number generator initialized
X      IF (qrgnin()) GO TO 10
X      WRITE (*,*) ' INITGN called before random number generator ',
X     +  ' initialized -- abort!'
X      STOP ' INITGN called before random number generator initialized'
X
X   10 CALL getcgn(g)
X      IF ((-1).NE. (isdtyp)) GO TO 20
X      lg1(g) = ig1(g)
X      lg2(g) = ig2(g)
X      GO TO 50
X
X   20 IF ((0).NE. (isdtyp)) GO TO 30
X      CONTINUE
X      GO TO 50
XC     do nothing
X   30 IF ((1).NE. (isdtyp)) GO TO 40
X      lg1(g) = mltmod(a1w,lg1(g),m1)
X      lg2(g) = mltmod(a2w,lg2(g),m2)
X      GO TO 50
X
X   40 STOP 'ISDTYP NOT IN RANGE'
X
X   50 cg1(g) = lg1(g)
X      cg2(g) = lg2(g)
X      RETURN
X
X      END
X      SUBROUTINE inrgcm()
XC**********************************************************************
XC
XC     SUBROUTINE INRGCM()
XC          INitialize Random number Generator CoMmon
XC
XC
XC                              Function
XC
XC
XC     Initializes common area  for random number  generator.  This saves
XC     the  nuisance  of  a  BLOCK DATA  routine  and the  difficulty  of
XC     assuring that the routine is loaded with the other routines.
XC
XC**********************************************************************
XC     .. Parameters ..
X      INTEGER numg
X      PARAMETER (numg=32)
XC     ..
XC     .. Scalars in Common ..
X      INTEGER a1,a1vw,a1w,a2,a2vw,a2w,m1,m2
XC     ..
XC     .. Arrays in Common ..
X      INTEGER cg1(numg),cg2(numg),ig1(numg),ig2(numg),lg1(numg),
X     +        lg2(numg)
X      LOGICAL qanti(numg)
XC     ..
XC     .. Local Scalars ..
X      INTEGER i
X      LOGICAL qdum
XC     ..
XC     .. External Functions ..
X      LOGICAL qrgnsn
X      EXTERNAL qrgnsn
XC     ..
XC     .. Common blocks ..
X      COMMON /globe/m1,m2,a1,a2,a1w,a2w,a1vw,a2vw,ig1,ig2,lg1,lg2,cg1,
X     +       cg2,qanti
XC     ..
XC     .. Save statement ..
X      SAVE /globe/
XC     ..
XC     .. Executable Statements ..
XC     V=20;                            W=30;
XC
XC     A1W = MOD(A1**(2**W),M1)         A2W = MOD(A2**(2**W),M2)
XC     A1VW = MOD(A1**(2**(V+W)),M1)    A2VW = MOD(A2**(2**(V+W)),M2)
XC
XC   If V or W is changed A1W, A2W, A1VW, and A2VW need to be recomputed.
XC    An efficient way to precompute a**(2*j) MOD m is to start with
XC    a and square it j times modulo m using the function MLTMOD.
XC
X      m1 = 2147483563
X      m2 = 2147483399
X      a1 = 40014
X      a2 = 40692
X      a1w = 1033780774
X      a2w = 1494757890
X      a1vw = 2082007225
X      a2vw = 784306273
X      DO 10,i = 1,numg
X          qanti(i) = .FALSE.
X   10 CONTINUE
XC
XC     Tell the world that common has been initialized
XC
X      qdum = qrgnsn(.TRUE.)
X      RETURN
X
X      END
X      INTEGER FUNCTION mltmod(a,s,m)
XC**********************************************************************
XC
XC     INTEGER FUNCTION MLTMOD(A,S,M)
XC
XC                    Returns (A*S) MOD M
XC
XC     This is a transcription from Pascal to Fortran of routine
XC     MULtMod_Decompos from the paper
XC
XC     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
XC     with Splitting Facilities." ACM Transactions on Mathematical
XC     Software, 17:98-111 (1991)
XC
XC
XC                              Arguments
XC
XC
XC     A, S, M  -->
XC                         INTEGER A,S,M
XC
XC**********************************************************************
XC     .. Parameters ..
X      INTEGER h
X      PARAMETER (h=32768)
XC     ..
XC     .. Scalar Arguments ..
X      INTEGER a,m,s
XC     ..
XC     .. Local Scalars ..
X      INTEGER a0,a1,k,p,q,qh,rh
XC     ..
XC     .. Executable Statements ..
XC
XC     H = 2**((b-2)/2) where b = 32 because we are using a 32 bit
XC      machine. On a different machine recompute H
XC
X      IF (.NOT. (a.LE.0.OR.a.GE.m.OR.s.LE.0.OR.s.GE.m)) GO TO 10
X      WRITE (*,*) ' A, M, S out of order in MLTMOD - ABORT!'
X      WRITE (*,*) ' A = ',a,' S = ',s,' M = ',m
X      WRITE (*,*) ' MLTMOD requires: 0 < A < M; 0 < S < M'
X      STOP ' A, M, S out of order in MLTMOD - ABORT!'
X
X   10 IF (.NOT. (a.LT.h)) GO TO 20
X      a0 = a
X      p = 0
X      GO TO 120
X
X   20 a1 = a/h
X      a0 = a - h*a1
X      qh = m/h
X      rh = m - h*qh
X      IF (.NOT. (a1.GE.h)) GO TO 50
X      a1 = a1 - h
X      k = s/qh
X      p = h* (s-k*qh) - k*rh
X   30 IF (.NOT. (p.LT.0)) GO TO 40
X      p = p + m
X      GO TO 30
X
X   40 GO TO 60
X
X   50 p = 0
XC
XC     P = (A2*S*H)MOD M
XC
X   60 IF (.NOT. (a1.NE.0)) GO TO 90
X      q = m/a1
X      k = s/q
X      p = p - k* (m-a1*q)
X      IF (p.GT.0) p = p - m
X      p = p + a1* (s-k*q)
X   70 IF (.NOT. (p.LT.0)) GO TO 80
X      p = p + m
X      GO TO 70
X
X   80 CONTINUE
X   90 k = p/qh
XC
XC     P = ((A2*H + A1)*S)MOD M
XC
X      p = h* (p-k*qh) - k*rh
X  100 IF (.NOT. (p.LT.0)) GO TO 110
X      p = p + m
X      GO TO 100
X
X  110 CONTINUE
X  120 IF (.NOT. (a0.NE.0)) GO TO 150
XC
XC     P = ((A2*H + A1)*H*S)MOD M
XC
X      q = m/a0
X      k = s/q
X      p = p - k* (m-a0*q)
X      IF (p.GT.0) p = p - m
X      p = p + a0* (s-k*q)
X  130 IF (.NOT. (p.LT.0)) GO TO 140
X      p = p + m
X      GO TO 130
X
X  140 CONTINUE
X  150 mltmod = p
XC
X      RETURN
X
X      END
X      LOGICAL FUNCTION qrgnin()
XC**********************************************************************
XC
XC     LOGICAL FUNCTION QRGNIN()
XC               Q Random GeNerators INitialized?
XC
XC     A trivial routine to determine whether or not the random
XC     number generator has been initialized.  Returns .TRUE. if
XC     it has, else .FALSE.
XC
XC**********************************************************************
XC     .. Scalar Arguments ..
X      LOGICAL qvalue
XC     ..
XC     .. Local Scalars ..
X      LOGICAL qinit
XC     ..
XC     .. Entry Points ..
X      LOGICAL qrgnsn
XC     ..
XC     .. Save statement ..
X      SAVE qinit
XC     ..
XC     .. Data statements ..
X      DATA qinit/.FALSE./
XC     ..
XC     .. Executable Statements ..
X      qrgnin = qinit
X      RETURN
X
X      ENTRY qrgnsn(qvalue)
XC**********************************************************************
XC
XC     LOGICAL FUNCTION QRGNSN( QVALUE )
XC               Q Random GeNerators Set whether iNitialized
XC
XC     Sets state of whether random number generator is initialized
XC     to QVALUE.
XC
XC     This routine is actually an entry in QRGNIN, hence it is a
XC     logical function.  It returns the (meaningless) value .TRUE.
XC
XC**********************************************************************
X      qinit = qvalue
X      qrgnsn = .TRUE.
X      RETURN
X
X      END
X      REAL FUNCTION ranf()
XC**********************************************************************
XC
XC     REAL FUNCTION RANF()
XC                RANDom number generator as a Function
XC
XC     Returns a random floating point number from a uniform distribution
XC     over 0 - 1 (endpoints of this interval are not returned) using the
XC     current generator
XC
XC     This is a transcription from Pascal to Fortran of routine
XC     Uniform_01 from the paper
XC
XC     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
XC     with Splitting Facilities." ACM Transactions on Mathematical
XC     Software, 17:98-111 (1991)
XC
XC**********************************************************************
XC     .. External Functions ..
X      INTEGER ignlgi
X      EXTERNAL ignlgi
XC     ..
XC     .. Executable Statements ..
XC
XC     4.656613057E-10 is 1/M1  M1 is set in a data statement in IGNLGI
XC      and is currently 2147483563. If M1 changes, change this also.
XC
X      ranf = ignlgi()*4.656613057E-10
X      RETURN
X
X      END
X      SUBROUTINE setall(iseed1,iseed2)
XC**********************************************************************
XC
XC      SUBROUTINE SETALL(ISEED1,ISEED2)
XC               SET ALL random number generators
XC
XC     Sets the initial seed of generator 1 to ISEED1 and ISEED2. The
XC     initial seeds of the other generators are set accordingly, and
XC     all generators states are set to these seeds.
XC
XC     This is a transcription from Pascal to Fortran of routine
XC     Set_Initial_Seed from the paper
XC
XC     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
XC     with Splitting Facilities." ACM Transactions on Mathematical
XC     Software, 17:98-111 (1991)
XC
XC
XC                              Arguments
XC
XC
XC     ISEED1 -> First of two integer seeds
XC                                   INTEGER ISEED1
XC
XC     ISEED2 -> Second of two integer seeds
XC                                   INTEGER ISEED1
XC
XC**********************************************************************
XC     .. Parameters ..
X      INTEGER numg
X      PARAMETER (numg=32)
XC     ..
XC     .. Scalar Arguments ..
X      INTEGER iseed1,iseed2
X      LOGICAL qssd
XC     ..
XC     .. Scalars in Common ..
X      INTEGER a1,a1vw,a1w,a2,a2vw,a2w,m1,m2
XC     ..
XC     .. Arrays in Common ..
X      INTEGER cg1(numg),cg2(numg),ig1(numg),ig2(numg),lg1(numg),
X     +        lg2(numg)
X      LOGICAL qanti(numg)
XC     ..
XC     .. Local Scalars ..
X      INTEGER g,ocgn
X      LOGICAL qqssd
XC     ..
XC     .. External Functions ..
X      INTEGER mltmod
X      LOGICAL qrgnin
X      EXTERNAL mltmod,qrgnin
XC     ..
XC     .. External Subroutines ..
X      EXTERNAL getcgn,initgn,inrgcm,setcgn
XC     ..
XC     .. Common blocks ..
X      COMMON /globe/m1,m2,a1,a2,a1w,a2w,a1vw,a2vw,ig1,ig2,lg1,lg2,cg1,
X     +       cg2,qanti
XC     ..
XC     .. Save statement ..
X      SAVE /globe/,qqssd
XC     ..
XC     .. Data statements ..
X      DATA qqssd/.FALSE./
XC     ..
XC     .. Executable Statements ..
XC
XC     TELL IGNLGI, THE ACTUAL NUMBER GENERATOR, THAT THIS ROUTINE
XC      HAS BEEN CALLED.
XC
X      qqssd = .TRUE.
X      CALL getcgn(ocgn)
XC
XC     Initialize Common Block if Necessary
XC
X      IF (.NOT. (qrgnin())) CALL inrgcm()
X      ig1(1) = iseed1
X      ig2(1) = iseed2
X      CALL initgn(-1)
X      DO 10,g = 2,numg
X          ig1(g) = mltmod(a1vw,ig1(g-1),m1)
X          ig2(g) = mltmod(a2vw,ig2(g-1),m2)
X          CALL setcgn(g)
X          CALL initgn(-1)
X   10 CONTINUE
X      CALL setcgn(ocgn)
X      RETURN
X
X      ENTRY rgnqsd(qssd)
XC**********************************************************************
XC
XC     SUBROUTINE RGNQSD
XC                    Random Number Generator Query SeeD set?
XC
XC     Returns (LOGICAL) QSSD as .TRUE. if SETALL has been invoked,
XC     otherwise returns .FALSE.
XC
XC**********************************************************************
X      qssd = qqssd
X      RETURN
X
X      END
X      REAL FUNCTION snorm()
XC**********************************************************************C
XC                                                                      C
XC                                                                      C
XC     (STANDARD-)  N O R M A L  DISTRIBUTION                           C
XC                                                                      C
XC                                                                      C
XC**********************************************************************C
XC**********************************************************************C
XC                                                                      C
XC     FOR DETAILS SEE:                                                 C
XC                                                                      C
XC               AHRENS, J.H. AND DIETER, U.                            C
XC               EXTENSIONS OF FORSYTHE'S METHOD FOR RANDOM             C
XC               SAMPLING FROM THE NORMAL DISTRIBUTION.                 C
XC               MATH. COMPUT., 27,124 (OCT. 1973), 927 - 937.          C
XC                                                                      C
XC     ALL STATEMENT NUMBERS CORRESPOND TO THE STEPS OF ALGORITHM 'FL'  C
XC     (M=5) IN THE ABOVE PAPER     (SLIGHTLY MODIFIED IMPLEMENTATION)  C
XC                                                                      C
XC     Modified by Barry W. Brown, Feb 3, 1988 to use RANF instead of   C
XC     SUNIF.  The argument IR thus goes away.                          C
XC                                                                      C
XC**********************************************************************C
XC
X      DIMENSION a(32),d(31),t(31),h(31)
XC
XC     THE DEFINITIONS OF THE CONSTANTS A(K), D(K), T(K) AND
XC     H(K) ARE ACCORDING TO THE ABOVEMENTIONED ARTICLE
XC
X      DATA a/0.0,.3917609E-1,.7841241E-1,.1177699,.1573107,.1970991,
X     +     .2372021,.2776904,.3186394,.3601299,.4022501,.4450965,
X     +     .4887764,.5334097,.5791322,.6260990,.6744898,.7245144,
X     +     .7764218,.8305109,.8871466,.9467818,1.009990,1.077516,
X     +     1.150349,1.229859,1.318011,1.417797,1.534121,1.675940,
X     +     1.862732,2.153875/
X      DATA d/5*0.0,.2636843,.2425085,.2255674,.2116342,.1999243,
X     +     .1899108,.1812252,.1736014,.1668419,.1607967,.1553497,
X     +     .1504094,.1459026,.1417700,.1379632,.1344418,.1311722,
X     +     .1281260,.1252791,.1226109,.1201036,.1177417,.1155119,
X     +     .1134023,.1114027,.1095039/
X      DATA t/.7673828E-3,.2306870E-2,.3860618E-2,.5438454E-2,
X     +     .7050699E-2,.8708396E-2,.1042357E-1,.1220953E-1,.1408125E-1,
X     +     .1605579E-1,.1815290E-1,.2039573E-1,.2281177E-1,.2543407E-1,
X     +     .2830296E-1,.3146822E-1,.3499233E-1,.3895483E-1,.4345878E-1,
X     +     .4864035E-1,.5468334E-1,.6184222E-1,.7047983E-1,.8113195E-1,
X     +     .9462444E-1,.1123001,.1364980,.1716886,.2276241,.3304980,
X     +     .5847031/
X      DATA h/.3920617E-1,.3932705E-1,.3950999E-1,.3975703E-1,
X     +     .4007093E-1,.4045533E-1,.4091481E-1,.4145507E-1,.4208311E-1,
X     +     .4280748E-1,.4363863E-1,.4458932E-1,.4567523E-1,.4691571E-1,
X     +     .4833487E-1,.4996298E-1,.5183859E-1,.5401138E-1,.5654656E-1,
X     +     .5953130E-1,.6308489E-1,.6737503E-1,.7264544E-1,.7926471E-1,
X     +     .8781922E-1,.9930398E-1,.1155599,.1404344,.1836142,.2790016,
X     +     .7010474/
XC
X   10 u = ranf()
X      s = 0.0
X      IF (u.GT.0.5) s = 1.0
X      u = u + u - s
X   20 u = 32.0*u
X      i = int(u)
X      IF (i.EQ.32) i = 31
X      IF (i.EQ.0) GO TO 100
XC
XC                                START CENTER
XC
X   30 ustar = u - float(i)
X      aa = a(i)
X   40 IF (ustar.LE.t(i)) GO TO 60
X      w = (ustar-t(i))*h(i)
XC
XC                                EXIT   (BOTH CASES)
XC
X   50 y = aa + w
X      snorm = y
X      IF (s.EQ.1.0) snorm = -y
X      RETURN
XC
XC                                CENTER CONTINUED
XC
X   60 u = ranf()
X      w = u* (a(i+1)-aa)
X      tt = (0.5*w+aa)*w
X      GO TO 80
X
X   70 tt = u
X      ustar = ranf()
X   80 IF (ustar.GT.tt) GO TO 50
X   90 u = ranf()
X      IF (ustar.GE.u) GO TO 70
X      ustar = ranf()
X      GO TO 40
XC
XC                                START TAIL
XC
X  100 i = 6
X      aa = a(32)
X      GO TO 120
X
X  110 aa = aa + d(i)
X      i = i + 1
X  120 u = u + u
X      IF (u.LT.1.0) GO TO 110
X  130 u = u - 1.0
X  140 w = u*d(i)
X      tt = (0.5*w+aa)*w
X      GO TO 160
X
X  150 tt = u
X  160 ustar = ranf()
X      IF (ustar.GT.tt) GO TO 50
X  170 u = ranf()
X      IF (ustar.GE.u) GO TO 150
X      u = ranf()
X      GO TO 140
X
X      END
END_OF_FILE
  if test 21583 -ne `wc -c <'fdran.f'`; then
    echo shar: \"'fdran.f'\" unpacked with wrong size!
  fi
  # end of 'fdran.f'
fi
echo shar: End of archive.
exit 0



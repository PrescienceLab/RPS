      subroutine fracdf( x, n, M, nar, nma, dtol, drange, h, hood, d, 
     *                   ar, ma, cov, lcov, stderr, cor, lcor, 
     *                   hess, lhess, w, lenw, iprint)

      implicit double precision (a-h,o-z)

      logical DEBUG
      parameter (DEBUG=.false.)

      integer            n, M, nar, nma, lcov, lcor, lenw
c     real               x(n)
c      real               x(*)
      real   x(*)
c     double precision   drange(2), ar(nar), ma(nma), stderr(nar+nma+1) 
      double precision   drange(2), ar(*), ma(*), stderr(*)
c     double precision   cov(lcov,nar+nma+1), cor(lcor,nar+nma+1)
      double precision   cov(lcov,*), cor(lcor,*), hess(lhess,*)
      double precision   d, dtol, h, hood
c     double precision   w(lenw)
      double precision   w(*)
      integer            iprint

c------------------------------------------------------------------------------
c
c       Calculates the maximum likelihood estimators of the parameters
c       of a fractionally-differenced ARIMA (p,d,q) model, together
c       with their estimated covariance and correlation matrices and
c       standard errors, as well as the value of the maximized likelihood.
c       The fast approximate algorithm of Haslett and Raftery (1989) is used.
c
c       The optimization is carried out in two levels : an outer univariate 
c       unimodal optimization with respect to d over the interval [0,.5] 
c       (uses Brent's fmin algorithm), and an inner nonlinear least-squares 
c       optimization with respect to the AR and MA parameters to minimize 
c       white noise variance (uses the MINPACK subroutine LMDER).
c
c REFERENCES : 
c
c    J. Haslett and A. E. Raftery,
c        Space-time modeling with  Long-Memory Dependence : 
c           Assessing Ireland's Wind Power Resource,
c             Applied Statistics 38 (1989) 1-50.
c
c    R. Brent, Algorithms for Minimization without Derivatives,
c        Prentice-Hall (1973).
c
c    J. J. More, B. S. Garbow, and K. E. Hillstrom,
c        User's Guide for MINPACK-1, Technical Report ANL-80-74,
c        Applied Mathematics Division, Argonne National Laboratory,
c       (August 1980).
c
c   Input :
c
c  x       real     time series for the ARIMA model
c  n       integer  length of the time series
c  M       integer  number of terms in the likelihood approximation
c                   suggested value 100 (see Haslett and Raftery 1989)
c  nar     integer  number of autoregressive parameters
c  nma     integer  number of moving average parameters
c  lcov    integer  row dimension of an array for the covariance matrix
c                   must be as declared in the callng program 
c                  (at least nar+nma+1)
c  lcor    integer  row dimension of an array for the correlation matrix
c                   must be as declared in the callng program 
c                  (at least nar+nma+1)
c  dtol    double   desired length of final interval of uncertainty for 
c                   suggested value : 4th root of machine precision
c                   if dtol < 0 it is automatically set to this value
c                   dtol will be altered if necessary by the program
c  drange  double   array of length 2 giving mimimum and maximum values 
c                   for the fractional differencing parameter
c  h       double   finite-difference interval for estimating derivatives
c                   with respect to d. If h < 0 it is automatically reset.
c  d       double   initial guess for optimal fractional differencing parameter
c  ar      double   initial autoregressive parameter estimates
c  ma      double   initial moving average parameter estimates       
c  w       double   work array
c  lenw    integer  length of double precision workspace w, must be at least 
c max(nar+nma+2*(n+M),3*n+(n+6.5)*(nar+nma)+1,(3+2*(nar+nma+1))*(nar+nma+1)+1)
c  iprint  integer  <= 0 no printing 
c                   >  0 intermediate results printed
c
c  Output :
c 
c  dtol    double   value of dtol ultimately used by the algorithm
c  h       double   value of h ultimately used by the algorithm
c  d       double   optimal fractional differencing parameter
c  hood    double   logarithm of the maximum likelihood
c  ar      double   optimal autoregressive parameter estimates
c  ma      double   optimal moving average parameter estimates       
c  cov     double   covariance matrix of the parameter estimates (d,p,q)
c  stderr  double   standard errors of the parameter estimates (d,p,q)
c  cor     double   correlation matrix of the parameter estimates (d,p,q)
c  hess    double   hessian matrix of the parameter estimates (d,p,q)
c
c------------------------------------------------------------------------------

      double precision   dlamch, dopt

      double precision   BASE
      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
      save   /MACHFD/

      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      save   /MAUXFD/

      integer            nn, MM, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/    nn, MM, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      integer            maxopt, maxfun, nopt, nfun, ngrd,
     *                   ifun, igrd, info
      common /CNTRFD/    maxopt, maxfun, nopt, nfun, ngrd,
     *                   ifun, igrd, info
      save   /CNTRFD/    

      double precision   told, tolf, tolx, tolg, anorm, deltax, gnorm
      common /TOLSFD/    told, tolf, tolx, tolg, anorm, deltax, gnorm
      save   /TOLSFD/

      integer            lenthw, lwfree
      common /WORKFD/    lenthw, lwfree
      save   /WORKFD/

      integer            ly, lamk, lak, lvk, lphi, lpi
      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
      save   /WFILFD/

      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
     *                   lwa1, lwa2, lwa3, lwa4
      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf, 
     *                   lwa1, lwa2, lwa3, lwa4
      save   /WOPTFD/

      integer            ILIMIT, JLIMIT
      common /LIMSFD/    ILIMIT, JLIMIT
      save   /LIMSFD/

      integer            IGAMMA, JGAMMA
      common /GAMMFD/    IGAMMA, JGAMMA
      save   /GAMMFD/

      integer            IMINPK, JMINPK
      common /MNPKFD/    IMINPK, JMINPK
      save   /MNPKFD/

      integer            KSVD, KCOV, KCOR
      common /HESSFD/    KSVD, KCOV, KCOR
      save   /HESSFD/

      double precision   zero, one
      parameter         (zero=0.d0, one=1.d0)

      data               ncalls /0/

C      if (DEBUG) then
C         print *,"On Entry to fracdf():"
C         print *,"x(1)=",x(1)
C         print *,"n=",n
C         print *,"M=",M
C         print *,"nar=",nar
C         print *,"nma=",nma
C         print *,"dtol=",dtol
C         print *,"drange=",drange
C         print *,"h=",h
C         print *,"hood=", hood
C         print *,"d=",d
C         print *,"ar=",ar
C         print *,"ma=",ma
C         print *,"cov=",cov
C         print *,"lcov=",lcov
C         print *,"stderr=",stderr
C         print *,"cor=",cor
C         print *,"lcor=",lcor
C         print *,"hess=",hess
C         print *,"lhess=",lhess
C         print *,"w(1)=",w(1)
C         print *,"lenw=",lenw
C         print *,"iprint=",iprint
C      endif

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c-----------------------------------------------------------------------------

      if (ncalls .eq. 0) then

c machine constants

        FLTMIN = dlamch( 'S' )
C       FLTMAX = dlamch( 'O' )  not guaranteed to work
        FLTMAX = one/FLTMIN

        BASE   = dlamch( 'B' )
        EPSMIN = dlamch( 'E' )
        EPSMAX = EPSMIN*BASE
        EPSPT5 = sqrt(EPSMIN)
        EPSP25 = sqrt(EPSPT5)
        EPSPT3 = EPSMIN**(.3)
        EPSP75 = EPSMIN**(.75)
        BIGNUM = one / EPSMIN
        ncalls = 1
      endif

c set error and warning flags
      ILIMIT = 0
      JLIMIT = 0

      IGAMMA = 0
      JGAMMA = 0

      IMINPK = 0
      JMINPK = 0

      KSVD   = 0
      KCOV   = 0
      KCOR   = 0

c useful quantities

      if (M .le. 0) M = 100

      nn    = n
      MM    = M
      np    = nar
      nq    = nma

      npq    = np + nq
      npq1   = npq + 1
      maxpq  = max(np,nq)
      minpq  = min(np,nq)
      maxpq1 = maxpq + 1

      maxopt = 100
      maxfun = 100

      if (dtol .gt. .1d0)  dtol = .1d0

      if (dtol .le. zero) then
        told  =  EPSP25
        tolf  =  EPSPT3
        tolx  =  told
        tolg  =  EPSPT3
      else
        told  =  max( dtol, EPSPT5)
        tolf  =  max( dtol/1.d1, EPSP75)
        tolx  =  told
        tolg  =  max( dtol/1.d1, EPSP75)
      end if

      dtol   = told

      nm     = n - maxpq
 
c workspace allocation

      lqp    = 1
      ly     = lqp    +  npq
      lamk   = ly
      lak    = lamk   +  n
      lphi   = lak    +  n
      lvk    = lphi   +  M
      lpi    = lphi
      la     = ly     +  n
      lajac  = la     +  n - minpq
      ipvt   = lajac  +  max( (n-np)*np, (n-nq)*nq, (n-maxpq)*npq)
      ldiag  = ipvt   +  npq/2 + 1
      lqtf   = ldiag  +  npq
      lwa1   = lqtf   +  npq
      lwa2   = lwa1   +  npq
      lwa3   = lwa2   +  npq
      lwa4   = lwa3   +  npq
      lfree  = lwa4   +  n - minpq

      lwfree = max( (lvk+M), (lwa4+n-minpq), (12*31))
      lenthw = lenw

      if (lwfree  .gt. (lenw+1)) then
        ILIMIT = lwfree - lenw
c       write( 6, *) 'insufficient storage : ',
c    *               'increase length of w by at least', incw
        return
      endif

c     call dcopy( npq, zero, 0, w(lqp), 1)

      if (npq .ne. 0) then
        call dcopy( np, ar, 1, w(lqp+nq), 1)
        call dcopy( nq, ma, 1, w(lqp)   , 1)
      end if

      nopt = 0
      nfun = 0
      ngrd = 0

      if (iprint .gt. 0) then
        write(6,*)
        write(6,900) 
      end if

      d = dopt( x, d, drange, hood, delta, w, iprint)

      if (nopt .ge. maxopt) JLIMIT = 1
c       write( 6, *)
c       write( 6, *) 'WARNING : optimization limit reached'
c     end if

      if (IGAMMA .ne. 0 .or. IMINPK .ne. 0) then
        d    = FLTMAX
        hood = FLTMAX
        call dcopy( np, FLTMAX, 0, ar, 1)
        call dcopy( nq, FLTMAX, 0, ma, 1)
        call dcopy( npq1, FLTMAX, 0, stderr, 1)
        do j = 1, npq1
          call dcopy( npq1, FLTMAX, 0, cov( 1, j), 1)
          call dcopy( npq1, FLTMAX, 0, cor( 1, j), 1)
        end do
        return
      end if
     
      call dcopy( np, w(lqp+nq), 1, ar, 1)
      call dcopy( nq, w(lqp   ), 1, ma, 1)

      call fdhpq( x, hess, lhess, w)

      if (npq .gt. 0) then
        do i = 2, npq1
         call dcopy( npq, hess(2,i), 1, cov(2,i), 1)
        end do
    
        call fdcov( x, d, h, hess, cov, lcov, cor, lcor, stderr, w)

      end if

      if (iprint.gt.0) then
         call fdout ( d, ar, ma, dtol, drange, h, hood,
     *        cov, lcov, stderr, cor, lcor)
      end if 

C      if (DEBUG) then
C         print *,"On Exit from fracdf():"
C         print *,"x(1)=",x
C         print *,"n=",n
C         print *,"M=",M
C         print *,"nar=",nar
C         print *,"nma=",nma
C         print *,"dtol=",dtol
C         print *,"drange(1)=",drange(1)
C         print *,"h="
C         print *,"hood=", hood
C         print *,"d=",d
C         print *,"ar(1)=",ar(1)
C         print *,"ma(1)=",ma(1)
C         print *,"cov(1,1)=",cov(1,1)
C         print *,"lcov=",lcov
C         print *,"stderr(1)=",stderr
C         print *,"cor(1,1)=",cor(1,1)
C         print *,"lcor=",lcor
C         print *,"hess(1,1)=",hess
C         print *,"lhess=",lhess
C         print *,"w(1)=",w(1)
C         print *,"lenw=",lenw
C         print *,"iprint=",iprint
C      endif


      return
 900  format( 4h itr, 14h     d          ,   14h    est mean  , 
     *                16h     white noise,  17h     log likelihd,
     *                 4h  nf, 3h ng)
      end

*******************************************************************************
*******************************************************************************

      double precision function dopt( x, dinit, drange, hood, delta, w, 
     *                                iprint)

      implicit double precision (a-h,o-z)

      integer           iprint

c     real              x(n)
      real              x(*) 

      double precision  dinit, drange(2), hood, delta
      double precision  w(*)
c
c optimization with repsect to d based on Brent's fmin algorithm
c
      double precision  pqopt
      double precision  dd, ee, hh, rr, ss, tt
      double precision  uu, vv, ww, fu, fv, fw
      double precision  eps, tol1, tol2, tol3

      intrinsic         abs, sqrt

      double precision  cc
      data              cc /.38196601125011d0/
      
      double precision  zero, half, one, two, three
      parameter        (zero=0.d0, half=.5d0, one=1.d0,
     *                   two=2.d0, three=3.d0)

      integer           maxopt, maxfun, nopt, nfun, ngrd, 
     *                  ifun, igrd, info
      common /CNTRFD/   maxopt, maxfun, nopt, nfun, ngrd, 
     *                  ifun, igrd, info
      save   /CNTRFD/
        
      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/
        
      double precision   aa, xx, bb, fa, fx, fb
      save               aa, xx, bb, fa, fx, fb

      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
     *                   lwa1, lwa2, lwa3, lwa4
      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf,
     *                   lwa1, lwa2, lwa3, lwa4
      save   /WOPTFD/

      double precision   hatmu, wnv, cllf
      common /FILTFD/    hatmu, wnv, cllf
      save   /FILTFD/

      double precision   dtol, ftol, xtol, gtol, anorm, deltax, gnorm
      common /TOLSFD/    dtol, ftol, xtol, gtol, anorm, deltax, gnorm
      save   /TOLSFD/

      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
      save   /MACHFD/

      integer            IGAMMA, JGAMMA
      common /GAMMFD/    IGAMMA, JGAMMA
      save   /GAMMFD/

      integer            IMINPK, JMINPK
      common /MNPKFD/    IMINPK, JMINPK
      save   /MNPKFD/

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c------------------------------------------------------------------------------
c
c  cc is the squared inverse of the golden ratio (see data statement)
c
c     cc = half*(three-sqrt(5.0d0))
c
c  eps is approximately the square root of the relative machine
c  precision.
c
      eps  =  EPSMAX
      tol1 =  one + eps
      eps  =  sqrt(eps)
c
      aa   =  drange(1)
      bb   =  drange(2)
      if (dinit .gt. (aa + dtol) .and. dinit .lt. (bb - dtol)) then
        vv = dinit
      else
        vv = aa + cc*(bb-aa)
      end if
      ww   =  vv
      xx   =  vv
      uu   =  xx
      ee   =  zero

      nopt = 1

      fx   =  pqopt( x, xx, w)
      
      fv   =  fx
      fw   =  fx
 
      tol  = max(dtol,zero)
      tol3 = tol/three
c
c  main loop starts here
c
   10 continue

      if (iprint .gt. 0) write(6,900)  nopt, uu, hatmu, wnv, cllf,
     *                                 ifun, igrd

      if (IGAMMA .ne. 0 .or. IMINPK .ne. 0) then
        d    = uu
        hood = FLTMAX
        return
      end if

      hh   =  half*(aa+bb)
      tol1 =  eps*(one+abs(xx)) + tol3
      tol2 =  two*tol1
c
c  check stopping criterion
c
      delta = abs(xx-hh) + half*(bb-aa)
c     if (abs(xx-hh) .le. (tol2-half*(bb-aa))) go to 100
      if (delta .le. tol2) go to 100

      if (nopt .ge. maxopt) go to 100

c     if (delpq .le. EPSMAX*(one+pqnorm)) go to 100

      rr   =  zero
      ss   =  zero
      tt   =  zero

      if (abs(ee) .gt. tol1) then
c
c  fit parabola
c
        rr   = (xx-ww)*(fx-fv)
        ss   = (xx-vv)*(fx-fw)
        tt   = (xx-vv)*ss-(xx-ww)*rr
        ss   =  two*(ss-rr)
        if (ss .le. zero) then
          ss = -ss
        else             
          tt = -tt
        end if
        rr   =  ee
        ee   =  dd
      end if
     
      if ((abs(tt) .ge. abs(half*ss*rr)) .or. 
     *   (tt .le. ss*(aa-xx)) .or. (tt .ge. ss*(bb-xx))) then
c
c  a golden-section step
c
        if (xx .ge. hh) then
          ee = aa - xx
        else           
          ee = bb - xx
        end if
        dd   =  cc*ee

      else 
c
c  a parabolic-interpolation step
c
        dd   =  tt / ss
        uu   =  xx + dd
c
c  f must not be evaluated too close to aa or bb
c
        if (((uu-aa) .lt. tol2) .or. ((bb-uu) .lt. tol2)) then
          dd  =  tol1
          if (xx .ge. hh) dd = -dd
        end if
      end if
c
c  f must not be evaluated too close to xx
c
      if (abs(dd) .ge. tol1)  then
        uu = xx + dd
      else
        if (dd .le. zero) then
          uu = xx - tol1
        else             
          uu = xx + tol1
        end if
      end if 

      nopt = nopt + 1

      fu   =  pqopt( x, uu, w)
c
c  update  aa, bb, vv, ww, and xx
c
      if (fx .ge. fu) then
        if (uu .ge. xx) then
          aa = xx
          fa = fx
        else           
          bb = xx
          fb = fx
        end if
        vv   =  ww
        fv   =  fw
        ww   =  xx
        fw   =  fx
        xx   =  uu
        fx   =  fu
      else
        if (uu .ge. xx) then
          bb = uu
          fb = fu
        else
          aa = uu
          fa = fu
        end if
        if ((fu .gt. fw) .and. (ww .ne. xx)) then
          if ((fu .le. fv) .or. (vv .eq. xx) .or. (vv .eq. ww)) then
             vv   =  uu
             fv   =  fu
          end if
        else
          vv   =  ww
          fv   =  fw
          ww   =  uu
          fw   =  fu
        end if
      end if
      
      go to 10
c
c  end of main loop
c
  100 dopt =  xx
      hood = -fx
      cllf = hood
      
      return
 900  format( i4, 2(1pe14.6), 1pe16.7, 1pe17.8, 1x, 2(i3))
      end

***************************************************************************
***************************************************************************

      double precision function pqopt( x, d, w)

      implicit double precision (a-h,o-z)

c     real              x(n)
      real              x(*)

      double precision  d
      double precision  w(*)

      double precision bic, slogvk
      double precision t, u

      intrinsic        log

      double precision  ddot
      external          ajqp

      double precision zero, one
      parameter       (zero=0.d0, one=1.d0)

      double precision  hatmu, wnv, hood
      common /FILTFD/   hatmu, wnv, hood
      save   /FILTFD/   

      double precision dtol, ftol, xtol, gtol, anorm, deltax, gnorm
      common /TOLSFD/  dtol, ftol, xtol, gtol, anorm, deltax, gnorm
      save   /TOLSFD/  

      integer          n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/  n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/ 

      integer          maxopt, maxfun, nopt, nfun, ngrd, 
     *                 ifun, igrd, info
      common /CNTRFD/  maxopt, maxfun, nopt, nfun, ngrd, 
     *                 ifun, igrd, info
      save   /CNTRFD/

      integer           ly, lamk, lak, lvk, lphi, lpi
      common /WFILFD/   ly, lamk, lak, lvk, lphi, lpi
      save   /WFILFD/
     
      integer           lqp, la, lajac, ipvt, ldiag, lqtf,
     *                  lwa1, lwa2, lwa3, lwa4
      common /WOPTFD/   lqp, la, lajac, ipvt, ldiag, lqtf,
     *                  lwa1, lwa2, lwa3, lwa4
      save   /WOPTFD/

      integer           modelm
      double precision  factlm
      data              modelm/1/, factlm /100.d0/

      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
      save   /MACHFD/

      integer            IGAMMA, JGAMMA
      common /GAMMFD/    IGAMMA, JGAMMA
      save   /GAMMFD/

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c----------------------------------------------------------------------------

        call fdfilt( x, d, w(ly), slogvk, 
     *               w(lamk), w(lak), w(lvk), w(lphi), w(lpi))

        if (IGAMMA .ne. 0) then
          pqopt  =  FLTMAX
          wnv    =  FLTMAX
          hood   = -FLTMAX
          return
        end if 

        t = dble(n)

       if (npq .eq. 0) then
          wnv   = ddot( n, w(ly), 1, w(ly), 1) / t
          ifun  =  0
          igrd  =  0
          info  = -1
          goto 100
        endif
c
c optimize as an unconstrained optimization problem
c
         if (modelm .eq. 2) call dcopy( npq,  one, 0, w(ldiag), 1)

         if (nopt .lt. 0) then
           if (np .ne. 0) then
             call LMDER1( ajp, n-np, np, w(lqp+nq),w(la),w(lajac), n-np,
     *                    ftol, xtol, gtol, maxfun, w(ldiag), modelm,
     *                    factlm, info, ifun, igrd, w(ipvt), w(lqtf),
     *                    w(lwa1), w(lwa2), w(lwa3), w(lwa4), w(ly))
           end if
           if (nq .ne. 0) then
             call LMDER1( ajq, n-nq, nq, w(lqp),w(la),w(lajac), n-nq,
     *                    ftol, xtol, gtol, maxfun, w(ldiag), modelm,
     *                    factlm, info, ifun, igrd, w(ipvt), w(lqtf),
     *                    w(lwa1), w(lwa2), w(lwa3), w(lwa4), w(ly))
           end if
         end if

         call LMDER1( ajqp, nm, npq, w(lqp), w(la), w(lajac), nm,
     *                ftol, xtol, gtol, maxfun, w(ldiag), modelm,
     *                factlm, info, ifun, igrd, w(ipvt), w(lqtf),
     *                w(lwa1), w(lwa2), w(lwa3), w(lwa4), w(ly))

        if (info .eq. 0) then
c         write( 6, *) 'MINPACK : improper input parameters
          IMINPK = 10
          pqopt  =  FLTMAX
          wnv    =  FLTMAX
          hood   = -FLTMAX
          return
        end if

        if (info .eq. 5) then
c         write( 6, *) 'MINPACK : function evaluation limit reached'
          JMINPK = 5
        end if

        if (info .eq. 6 ) then
c         write( 6, *) 'MINPACK : ftol is too small'
          JMINPK = 6
        end if

        if (info .eq. 7) then
c         write( 6, *) 'MINPACK : xtol is too small'
          JMINPK = 7
        end if

        if (info .eq. 8) then
c         write( 6, *) 'MINPACK : gtol is too small'
          JMINPK = 8
        end if

c        call daxpy( npq, (-one), w(lpq), 1, w(lqp), 1
c        delpq  = sqrt(ddot( npq, w(lqp), 1, w(lqp), 1))         
c        pqnorm = sqrt(ddot( npq, w(lpq), 1, w(lpq), 1))

        wnv   =  (anorm*anorm) / dble(nm-1)
 100    u     = (t*(2.8378d0+log(wnv))+slogvk)
        pqopt =  u / 2.d0
        bic   =  u + dble(np+nq+1)*log(t)
        hood  = -pqopt

      return
      end

***************************************************************************
***************************************************************************

      subroutine fdfilt( x, d, y, slogvk, amk, ak, vk, phi, pi)

      implicit double precision (a-h,o-z)

c     real              x(n)
      real              x(*)
      double precision  d, slogvk
c     double precision  y(n), amk(n), ak(n)
      double precision  y(*), amk(*), ak(*)
c     double precision  vk(M), phi(M), pi(M)
      double precision  vk(*), phi(*), pi(*)

c**************************************************************************
c input  :
c          x       real    original time series
c          d       double  estimated value of d
c output :
c          y       double  flitered series
c          slogvk  double  the sum of the logarithms of the vk
c notes  :
c          y can use the same storage as either ak or amk
c          phi and pi can use the same storage
c          can be arranged so that phi, pi and vk share the same storage
c**************************************************************************

      integer           j, k, km, mcap, mcap1
      double precision  g0, r, s, t, u, v, z

      double precision  zero, one, two
      parameter        (zero=0.d0, one=1.d0, two=2.d0)

      double precision  dgamma, dgamr

      intrinsic         log, sqrt

      double precision  hatmu, wnv, cllf
      common /FILTFD/   hatmu, wnv, cllf
      save   /FILTFD/

      integer           n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/   n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/ 

      integer            IGAMMA, JGAMMA
      common /GAMMFD/    IGAMMA, JGAMMA
      save   /GAMMFD/

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 
        
c-----------------------------------------------------------------------

       mcap  = min(M,n) 
        mcap1 = mcap + 1
c
c calculate amk(k), vk(k), and ak(k) for k=1,n (see W522-4 for notation).
c
c
c  k = 1 
c
       amk(1) = zero
       ak(1)  = one
c
c  k = 2 ;  initialize phi(1)
c
       z      = d/(one-d)
       amk(2) = z*dble(x(1))
       ak(2)  = one - z
        phi(1) = z

        t  = dgamr(one-d)
        if (IGAMMA .ne. 0) return

       g0 = dgamma(one-(two*d))*(t*t)
        if (IGAMMA .ne. 0) return
 
       vk(1)  = g0
       vk(2)  = g0*(one-(z*z))
c
c  k = 3, mcap
c
       do k = 3, mcap
          km = k - 1
          t  = dble(km)
          u  = t - d
c 
c  calculate phi() and vk() using the recursion formula on W498
c
         do j = 1, km-1
            s      = t-dble(j)
           phi(j) = phi(j)*(t*(s-d)/(u*s))
          end do

          v       = d / u
          phi(km) = v
          vk(k)   = vk(km)*(one-(v*v))
c
c  form amk(k) and ak(k)
c
         u = zero
         v =  one
         do j = 1, km
            t  = phi(j)
           u  = u + (t*dble(x(k-j)))
           v  = v - t
          end do
          amk(k) = u
          ak(k)  = v
        end do
       
        if (mcap .eq. n) go to 200
c
c  k = mcap+1, n
c      
c calculate pi(j), j = 1,mcap
c
       pi(1) = d
       s     = d
       do j = 2, mcap
          u     = dble(j)
          t     = pi(j-1)*((u-one-d)/u)
         s     = s + t
          pi(j) = t
        end do

        s =  one - s
       r = zero
        u = dble(mcap)
        t = u*pi(mcap)
c
       do  k = mcap1, n
         km = k - mcap
          z  = zero
         do  j = 1, mcap
            z = z + (pi(j)*dble(x(k-j)))
          end do
          if (r .eq. zero) then
            amk(k) = z
                   ak(k)  = s
          else 
            v      = (t*(one - (u/dble(k))**d))/d
           amk(k) = z + ((v*r)/(dble(km)-one))
            ak(k)  = s - v
          end if
          r = r + dble(x(km))
        end do

 200    continue
c
c  form muhat - see formula on W523.
c
        r = zero
       s = zero
       do  k = 1, n
           t = ak(k)
          u = (dble(x(k))-amk(k))*t
          v = t*t
           if (k .le. mcap) then
             z = vk(k)
             u = u / z
             v = v / z
           end if
           r = r + u
           s = s + v
        end do

       hatmu = r / s
c
c  form filtered version
c
        s = zero   
       do k= 1, mcap
         s = s + log(vk(k))
        end do

        slogvk = s
        sumlog = s

        s = zero
       do k= 1, n
          t    = (dble(x(k))-amk(k)-hatmu*ak(k))
          if (k .le. mcap) t = t / sqrt(vk(k))
         s    = s + t
          y(k) = t
        end do

        if (npq .eq. 0) return

        t = dble(n)

        u = z / t
       do k= 1, n
          y(k) = y(k) - u
        end do

      return
      end

*****************************************************************************
*****************************************************************************

      subroutine ajqp( qp, a, ajac, lajac, iflag, y)

      implicit double precision (a-h,o-z)

      integer          lajac, iflag

c     double precision qp(npq), a(nm), ajac(nm,npq), y(n)
      double precision qp(*), a(*), ajac(lajac,*), y(*)

      integer          maxopt, maxfun, nopt, nfun, ngrd,
     *                 ifun, igrd, info
      common /CNTRFD/  maxopt, maxfun, nopt, nfun, ngrd,
     *                 ifun, igrd, info
      save   /CNTRFD/
     
      integer          n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/  n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      save   /MAUXFD/

      double precision s, t

      double precision zero
      parameter       (zero=0.d0)

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c--------------------------------------------------------------------------

        if (iflag .eq. 2) goto 200

        if (iflag .ne. 1) return
c
c  objective calculation
c
        do k = maxpq1, n
          km = k - maxpq
          t  = zero
          if (np .ne. 0) then
            do l = 1, np
              t  = t - qp(nq+l)*y(k-l)
            end do
          end if
          s = zero
          if (nq .ne. 0) then
            do l = 1, nq
              if (km .le. l) goto 101
              s  = s + qp(l)*a(km-l)
            end do
          end if
 101      s = y(k) + (t + s)
          if (abs(s) .le. BIGNUM) then
            a(km) = s
          else                    
            a(km) = sign(one,s)*BIGNUM
          end if
        end do

        nfun = nfun + 1

        return

 200    continue
c
c  jacobian calculation
c
        do i = 1, npq
          do k = maxpq1, n
            km  =  k - maxpq
            t   = zero
            if (nq .ne. 0) then
              do l = 1, nq
                if (km .le. l) goto 201
                t  = t +  qp(l)*ajac(km-l,i)
              end do
            end if
 201        continue
            if (i .le. nq) then
              if (km .gt. i) then
                s = a(km-i) + t
              else
                s =           t
              end if
            else
              s = -y(k-(i-nq)) + t
            end if
            if (abs(s) .le. BIGNUM) then
              ajac(km,i) = s
            else                    
              ajac(km,i) = sign(one,s)*BIGNUM
            end if
          end do
        end do

        ngrd = ngrd + 1

      return
      end

*****************************************************************************
*****************************************************************************

      subroutine  ajp( p, a, ajac, lajac, iflag, y)

      implicit double precision (a-h,o-z)

      integer          lajac, iflag
c     double precision p(np), a(nm), ajac(nm,npq), y(n)
      double precision p(*), a(*), ajac(lajac,*), y(*)
     
      integer          n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/  n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      double precision  t

      double precision zero
      parameter       (zero=0.d0)

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c--------------------------------------------------------------------------

        if (iflag .eq. 2) goto 200

        if (iflag .ne. 1) return

        if (np .eq. 0) return
c
c  objective calculation
c
        do k = np+1, n
          t  = zero
          do l = 1, np
            t  = t - p(l)*y(k-l)
          end do
 101      a(k-np) = y(k) + t
        end do

        return

 200    continue
c
c  jacobian calculation
c
          do i = 1, np
            do k = np+1, n
              ajac(k-np,i) = -y(k-i) 
            end do
          end do

      return
      end

*****************************************************************************
*****************************************************************************

      subroutine  ajq( qp, a, ajac, lajac, iflag, y)

      implicit double precision (a-h,o-z)

      integer          lajac, iflag
c     double precision qp(npq), a(nm), ajac(nm,npq), y(n)
      double precision qp(*), a(*), ajac(lajac,*), y(*)
     
      integer          n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/  n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      integer          maxopt, maxfun, nopt, nfun, ngrd, 
     *                 ifun, igrd, info
      common /CNTRFD/  maxopt, maxfun, nopt, nfun, ngrd, 
     *                 ifun, igrd, info
      save   /CNTRFD/

      double precision s, t

      double precision zero
      parameter       (zero=0.d0)

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c--------------------------------------------------------------------------

        if (iflag .eq. 2) goto 200

        if (iflag .ne. 1) return

        if (nq. eq. 0) return
c
c  objective calculation
c
        do k = maxpq1, n
          km = k - maxpq
          t  = zero
          if (np .ne. 0) then
            do l = 1, np
              t  = t - qp(nq+l)*y(k-l)
            end do
          end if
          s = zero
          if (nq .ne. 0) then
            do l = 1, nq
              if (km .le. l) goto 101
              s  = s + qp(l)*a(km-l)
            end do
          end if
 101      a(km) = y(k) + (t + s)
        end do

        nfun = nfun + 1

        return

 200    continue
c
c  jacobian calculation
c
        do i = 1, npq
          do k = maxpq1, n
            km  =  k - maxpq
            t   = zero
            if (nq .ne. 0) then
              do l = 1, nq
                if (km .le. l) goto 201
                t  = t +  qp(l)*ajac(km-l,i)
              end do
            end if
 201        continue
            if (i .le. nq) then
              if (km .gt. i) then
                ajac(km,i) = a(km-i)    + t
              else
                ajac(km,i) =              t
              end if
            else
              ajac(km,i) = -y(k-(i-nq)) + t
            end if
          end do
        end do

        ngrd = ngrd + 1

      return
      end

      subroutine fdvar(  x, n, M, nar, nma, dtol, drange, h, hood, d, 
     *                   ar, ma, cov, lcov, stderr, cor, lcor, 
     *                   hess, lhess, w, lenw)

      implicit double precision (a-h,o-z)

      integer            n, M, nar, nma, lcov, lcor, lenw
c     real               x(n)
      real               x(*)
c     double precision   drange(2), ar(nar), ma(nma), stderr(nar+nma+1) 
      double precision   drange(2), ar(*), ma(*), stderr(*)
c     double precision   cov(lcov,nar+nma+1), cor(lcor,nar+nma+1)
      double precision   cov(lcov,*), cor(lcor,*), hess(lhess,*)
      double precision   d, dtol, h, hood
c     double precision   w(lenw)
      double precision   w(*)

c------------------------------------------------------------------------------
c
c       Recomputes the covariance, correlation and standard error given
c       a finite-dfference interval h for derivatives with respect to d.
c
c------------------------------------------------------------------------------

      double precision   dlamch

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

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c-----------------------------------------------------------------------------

      if (ncalls .eq. 0) then

c machine constants

        FLTMIN  = dlamch( 'S' )
C       FLTMAX  = dlamch( 'O' )  not guaranteed to work
        FLTMAX  = one/FLTMIN
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

      IGAMMA = 0
      IMINPK = 0
      ILIMIT = 0

      JGAMMA = 0
      JMINPK = 0
      JLIMIT = 0

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

      told = dtol

      if (dtol .le. zero) then
        tolf  =  EPSPT3
        tolx  =  told
        tolg  =  EPSPT3
      else
        tolf  =  max( dtol/1.d1, EPSP75)
        tolx  =  told
        tolg  =  max( dtol/1.d1, EPSP75)
      end if

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

      if (npq .gt. 0) then
        do i = 2, npq1
         call dcopy( npq, hess(2,i), 1, cov(2,i), 1)
        end do
    
        call fdcov( x, d, h, hess, cov, lcov, cor, lcor, stderr, w)

        do i = 1, npq
          hess(1,i+1) = hess(i+1,1)
        end do
      end if

      if (.false.) then
         call fdout ( d, ar, ma, dtol, drange, h, hood,
     *        cov, lcov, stderr, cor, lcor)
      end if

      return
      end

*******************************************************************************
*******************************************************************************

      subroutine fdhpq( x, H, lH, w)

      implicit double precision (a-h,o-z)

      integer            lH
c     real               x(n)
      real               x(*)
c     double precision   H(lH, npq1)
      double precision   H(lH, *)
      double precision   w(*)

      double precision   zero
      parameter         (zero=0.d0)

      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      integer            ly, lamk, lak, lvk, lphi, lpi
      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
      save   /WFILFD/

      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
     *                   lwa1, lwa2, lwa3, lwa4
      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf, 
     *                   lwa1, lwa2, lwa3, lwa4
      save   /WOPTFD/

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c-----------------------------------------------------------------------------

      call hesspq( w(lqp), w(la), w(lajac), nm, H, lH, 
     *             w(lwa4), w(lwa1))

c     call dcopy( npq1, zero, 0, H(1,1), lH)
c     call dcopy( npq , zero, 0, H(2,1), 1)

      return
      end

*******************************************************************************
*******************************************************************************

      subroutine fdcov( x, d, hh, hd, cov, lcov, cor, lcor, se, w)

      implicit double precision (a-h,o-z)

      integer            lcov
c     real               x(n)
      real               x(*)
c     double precision   d, hh, hd(npq1), cov(lcov,npq1), 
c    *                   cor(lcor,npq1), se(npq1)
      double precision   d, hh, hd(*), cov(lcov,*), cor(lcor,*), se(*)
      double precision   w(*)

      double precision   temp

      double precision   zero, one, two
      parameter         (zero=0.d0, one=1.d0, two=2.d0)

      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
      save   /MACHFD/

      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      save   /MAUXFD/

      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      integer            ly, lamk, lak, lvk, lphi, lpi
      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
      save   /WFILFD/

      integer            IGAMMA, JGAMMA
      common /GAMMFD/    IGAMMA, JGAMMA
      save   /GAMMFD/

      integer            KSVD, KCOV, KCOR
      common /HESSFD/    KSVD, KCOV, KCOR
      save   /HESSFD/

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 
c-----------------------------------------------------------------------------

      call hesdpq( x, d, hh, hd, w)

      call dcopy( npq1, hd, 1, cov, lcov)

      IGAMMA = 0
      JGAMMA = 0

      KSVD = 0
      KCOV = 0
      KCOR = 0

      temp = one
      do i = 1, npq1
        do j = i+1, npq1
          cov(j,i) = cov(i,j)
        end do
      end do

      ls    = ly
      lu    = ls    + npq1 + 1
      lv    = lu    + npq1*npq1
      le    = lv    + npq1*npq1
      lwork = le    + npq1
      lfree = lwork + npq1

      call dsvdc( cov, lcov, npq1, npq1, w(ls), w(le), 
     *            w(lu), npq1, w(lv), npq1, w(lwork), 11, info)

      if (info .ne. 0) then
        call dcopy( npq1, zero, 0, se, 1)
        do j = 1, npq1
           call dcopy( npq1, zero, 0, cov(1,j), 1)
        end do
        KSVD = 1
        info = 3
        return
      end if

      call invsvd( w(ls), w(lu), npq1, w(lv), npq1, cov, lcov)

      do i = 1, npq1
        do j = i+1, npq1 
          cov(j,i) = cov(i,j)
        end do
      end do

      temp = one
      do j = 1, npq1
        if (cov(j,j) .gt. zero) then
          se(j) = sqrt(cov(j,j))
        else
          temp  = min(temp,cov(j,j))
          se(j) = zero
        end if
      end do

      if (temp .eq. one) then
        do k = 1, npq1
          call dcopy( k, cov( 1, k), 1, cor( 1, k), 1)
        end do
        do i = 1, npq1
          call dscal( (npq1-i+1), (one/se(i)), cor(i,i), lcor)
        end do
        do j = 1, npq1
          call dscal( j, (one/se(j)), cor(1,j),    1)
        end do
      else
        KCOR = 1
        do j = 1, npq1
          call dcopy( npq1, zero, 0, cor(1,j), 1)
        end do
      end if

      do i = 1, npq1
        do j = i+1, npq1
          cor(j,i) = cor(i,j)
        end do
      end do

      return
      end

*******************************************************************************
*******************************************************************************

      subroutine invsvd ( s, u, lu, v, lv, cov, lcov)

      implicit double precision (a-h,o-z)

      integer            lu, lv, lcov
c     double precision   s(npq1), u(lu,npq1), v(lv,npq1), cov(lcov,npq1)
      double precision   s(*), u(lu,*), v(lv,*), cov(lcov,*)

      integer            krank
      double precision   ss

      double precision   zero, one
      parameter         (zero=0.d0, one=1.d0)

      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
      save   /MACHFD/

      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      save   /MAUXFD/

      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      integer            KSVD, KCOV, KCOR
      common /HESSFD/    KSVD, KCOV, KCOR
      save   /HESSFD/


c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c-----------------------------------------------------------------------------

      krank = npq1
     
      do i = 1, npq1
        ss = s(i)
        do j = 1, npq1
          if (ss .lt. one) then
            if (abs(u(i,j)) .gt. ss*FLTMAX) then
              krank = i - 1
              KCOV  = 1
              goto 100
            end if
          end if
        end do
      end do

 100  continue

      do k = 1, npq1
        call dcopy( k, zero, 0, cov( 1, k), 1)
      end do

      if (krank .eq. 0) return

c      do k = 1, npq1
c        do i = 1, npq1
c          do j = i, npq1
c            H(i,j) =  H(i,j) + s(k)*u(i,k)*v(j,k)
c          end do
c        end do
c      end do
      
c      do k = 1, npq1
c        ss = s(k)
c        do j = 1, npq1
c          call daxpy( j, ss*v(j,k), u(1,k), 1, H(1,j), 1)
c        end do
c      end do
      
      do k = 1, krank
        ss = (-one/s(k))
        do j = 1, npq1
          call daxpy( j, (ss*u(j,k)), v(1,k), 1, cov(1,j), 1)
        end do
      end do

      return
      end

*******************************************************************************
*******************************************************************************

      subroutine hesspq( qp, a, ajac, lajac, H, lH, aij, g)

      implicit double precision (a-h,o-z)
      integer           lajac, lH
c     double precision  qp(npq), a(nm), ajac(nm,npq)
      double precision  qp(*), a(*), ajac(lajac,*)
c     double precision  H(lH,npq1), aij(nm), g(npq)
      double precision  H(lH,*), aij(*), g(*)

c analytic Hessian with respect to p and q variables
     
      double precision   ddot

      integer           n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/   n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      double precision   hatmu, wnv, cllf
      common /FILTFD/    hatmu, wnv, cllf
      save   /FILTFD/

      double precision  fac, s, t, u

      double precision  zero, one, two
      parameter        (zero=0.d0, one=1.d0, two=2.d0)

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c-----------------------------------------------------------------------------

      fac = one / (wnv * dble(nm-1))

      if (nq .ne. 0 .and. np .ne. 0) then
        do k = 1, npq
          g(k) = ddot( nm, a, 1, ajac( 1, k), 1) 
        end do
        do i = 1, np
          u = g(nq+i)
          do j = 1, nq
            u = g(j)*u
            do k = maxpq1, n
              km = k - maxpq
              t  = zero
              do l = 1, nq
                if (km .le. l) goto 301
                t  = t + qp(l)*aij(km-l)
              end do
 301          continue
              if (km .gt. j) then
                aij(km) = ajac(km-j,nq+i) + t
              else
                aij(km) =                   t
              end if
            end do
            s = ddot( nm, ajac( 1, nq+i), 1, ajac( 1, j), 1) 
            t = ddot( nm, a             , 1, aij        , 1) 
            H(i+1,np+j+1) = -dble(n)*((s + t) - two*fac*u)*fac
          end do
        end do
      end if 

      if (nq .ne. 0) then
        do i = 1, nq
          u = g(i)
          do j = i, nq
            u = g(j)*u
            do k = maxpq1, n
              km = k - maxpq
              t  = zero
              do l = 1, nq
                if (km .le. l) goto 302
                t  = t + qp(l)*aij(km-l)
              end do
 302          continue
              s  = zero
              if (km .gt. i) s = s + ajac(km-i,j) 
              if (km .gt. j) s = s + ajac(km-j,i)
              aij(km) = s + t
            end do
            s = ddot( nm, ajac( 1, i), 1, ajac( 1, j), 1) 
            t = ddot( nm, a          , 1, aij        , 1) 
            H(np+i+1,np+j+1) = -dble(n)*((s + t) - two*fac*u)*fac    
          end do
        end do
      end if 

      if (np .ne. 0) then
        do i = 1, np
          u = g(nq+i)
          do j = i, np
            u = g(nq+j)*u
c            do k = maxpq1, n
c              km  =  k - maxpq
c              t  = zero
c              if (nq .ne. 0) then
c               do l = 1, nq
c                  if (km .le. l) goto 303
c                  t  = t + qp(l)*aij(km-l)
c               end do
c              end if
c 303          continue
c              aij(km) = t
c            end do
            s = ddot( nm, ajac( 1, nq+i), 1, ajac( 1, nq+j), 1) 
c            t = ddot( nm, a             , 1, aij           , 1) 
c            H(i+1,j+1) = -dble(n)*((s + t) - two*fac*u)*fac
            H(i+1,j+1) = -dble(n)*(s - two*fac*u)*fac
          end do
        end do
      end if 

      return
      end

*******************************************************************************
*******************************************************************************

      subroutine hesdpq( x, d, hh, hd, w)

      implicit double precision (a-h,o-z)

c     real               x(n)
      real               x(*)

c     double precision   d, hh, hd(npq1), w(*)
      double precision   d, hh, hd(*), w(*)

      double precision   slogvk

      intrinsic          log
      double precision   ddot

      double precision   hatmu, wnv, cllf
      common /FILTFD/    hatmu, wnv, cllf
      save   /FILTFD/

      double precision   zero, half, one, two
      parameter         (zero=0.d0, half=.5d0, one=1.d0, two=2.d0)

      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      integer            ly, lamk, lak, lvk, lphi, lpi
      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
      save   /WFILFD/

      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
     *                   lwa1, lwa2, lwa3, lwa4
      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf,
     *                   lwa1, lwa2, lwa3, lwa4
      save   /WOPTFD/      

      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
      save   /MACHFD/

      double precision   EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      common /MAUXFD/    EPSP25, EPSPT3, EPSPT5, EPSP75, BIGNUM
      save   /MAUXFD/

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c-----------------------------------------------------------------------------

      if (hh .le. zero) hh = (one+abs(cllf))*EPSPT5

      hh = min( hh, .1d0)

      if ((d-hh) .gt. zero) then

        call fdfilt( x, (d-hh), w(ly), slogvk,
     *               w(lamk), w(lak), w(lvk), w(lphi), w(lpi))

        if (npq .ne. 0) then
          call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
          call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))

          call gradpq( w(lwa1), w(la), w(lajac), nm)

          wnv = ddot( nm, w(la), 1, w(la), 1) 

          call dscal( npq, (one/wnv), w(lwa1), 1)

          wnv = wnv / dble(nm - 1)
        else
          wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
        end if

        fa  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two

        if ((d+hh) .lt. half) then

          call fdfilt( x, (d+hh), w(ly), slogvk,
     *                 w(lamk), w(lak), w(lvk), w(lphi), w(lpi))

          if (npq .ne. 0) then
            call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
            call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))

            call gradpq( w(lwa2), w(la), w(lajac), nm)

            wnv = ddot( nm, w(la), 1, w(la), 1) 

            call dscal( npq, (one/wnv), w(lwa2), 1)

            wnv = wnv / dble(nm - 1)
          else 
            wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
          end if
           
          fb  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two

          hd(1) = ((fa + fb) - two*cllf) / (hh*hh)

        else

          call fdfilt( x, (d-two*hh), w(ly), slogvk,
     *                 w(lamk), w(lak), w(lvk), w(lphi), w(lpi))

          if (npq .ne. 0) then
            call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
            call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))

            call gradpq( w(lwa2), w(la), w(lajac), nm)

            wnv = ddot( nm, w(la), 1, w(la), 1) 

            call dscal( npq, (one/wnv), w(lwa2), 1)

            wnv = wnv / dble(nm - 1)
          else 
            wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
          end if

          fb  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two
 
          hd(1) = ((cllf + fb) -two*fa) / (two*hh*hh)

        endif
            
      else

        call fdfilt( x, (d+hh), w(ly), slogvk,
     *               w(lamk), w(lak), w(lvk), w(lphi), w(lpi))

        if (npq .ne. 0) then
          call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
          call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))

          call gradpq( w(lwa1), w(la), w(lajac), nm)

          wnv = ddot( nm, w(la), 1, w(la), 1) 

          call dscal( npq, (one/wnv), w(lwa1), 1)

          wnv = wnv / dble(nm - 1)
        else
          wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)
        end if

        fa  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two

        call fdfilt( x, (d+two*hh), w(ly), slogvk,
     *               w(lamk), w(lak), w(lvk), w(lphi), w(lpi))

        if (npq .ne. 0) then
          call ajqp( w(lqp), w(la), w(lajac), nm, 1, w(ly))
          call ajqp( w(lqp), w(la), w(lajac), nm, 2, w(ly))

          call gradpq( w(lwa1), w(la), w(lajac), nm)

          wnv = ddot( nm, w(la), 1, w(la), 1) 

          call dscal( npq, (one/wnv), w(lwa1), 1)

          wnv = wnv / dble(nm - 1)
        else
          wnv = ddot( nm, w(ly), 1, w(ly), 1) / dble(nm-1)

        end if

        fb  = -(dble(n)*(2.8378d0+log(wnv))+slogvk) / two
 
        hd(1) = ((cllf + fb) - two*fa) / (two*hh*hh)

      end if

      if (npq .eq. 0) return

      call daxpy( npq, (-one), w(lwa2), 1, w(lwa1), 1)
      call dscal( npq, (dble(n)/(two*hh)), w(lwa1), 1)

      call dcopy( npq, w(lwa1), 1, hd(2), 1)

      return
      end

*******************************************************************************
*******************************************************************************

      subroutine gradpq( g, a, ajac, ljac)

      implicit double precision (a-h,o-z)

      integer            ljac
c     double precision   g(npq), a(nm), ajac(nm,npq)
      double precision   g(*), a(*), ajac(ljac,*)

      double precision   ddot

      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      integer            ly, lamk, lak, lvk, lphi, lpi
      common /WFILFD/    ly, lamk, lak, lvk, lphi, lpi
      save   /WFILFD/

      integer            lqp, la, lajac, ipvt, ldiag, lqtf,
     *                   lwa1, lwa2, lwa3, lwa4
      common /WOPTFD/    lqp, la, lajac, ipvt, ldiag, lqtf,
     *                   lwa1, lwa2, lwa3, lwa4
      save   /WOPTFD/

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c------------------------------------------------------------------------------

      if (np .ne. 0) then
        do i = 1, np
          g(i)    = ddot( nm, a, 1, ajac( 1, nq+i), 1) 
        end do
      end if

      if ( nq .ne. 0) then
        do j = 1, nq
          g(np+j) = ddot( nm, a, 1, ajac( 1,    j), 1) 
        end do
      end if 

      return
      end

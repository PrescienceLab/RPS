      subroutine fdout ( d, p, q, dtol, drange, h, hood,
     *                   cov, lcov, stderr, cor, lcor)

      integer            lcov, lcor
c     double precision   p(np), q(nq), stderr(npq1) 
      double precision   p(*), q(*), stderr(*) 
c     double precision   cov(lcov,npq1), cor(lcor,npq1)
      double precision   cov(lcov,*), cor(lcor,*)
      double precision   d, dtol, drange(2), h, hood

      integer            n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      common /DIMSFD/    n, M, np, nq, npq, npq1, maxpq, maxpq1, nm
      save   /DIMSFD/

      double precision   FLTMIN, FLTMAX, EPSMIN, EPSMAX
      common /MACHFD/    FLTMIN, FLTMAX, EPSMIN, EPSMAX
      save   /MACHFD/

      integer            lenw, lfree
      common /WORKFD/    lenw, lfree
      save   /WORKFD/

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


c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c-----------------------------------------------------------------------------

      write( 6, *)
      write( 6, *) 'n = ', n, '   M = ', M

      if (ILIMIT .ne. 0) goto 300
      if (IGAMMA .ne. 0 .or. IMINPK .ne. 0) goto 200

      write( 6, *)
      write( 6, *) 'log maximum likelihood : ', hood
      write( 6, *)
      write( 6, *) 'fractional differencing parameter : d  = ', d
      write( 6, *)
      write( 6, *) 'autoregressive parameters :'
      if (np .gt. 0) then
        do i = 1, np
          write(6,*) i, p(i)
        end do
      end if
      write( 6, *)
      write( 6, *) 'moving average parameters :'
      if (nq .gt. 0) then
        do i = 1, nq
          write(6,*) i, q(i)
        end do
      end if

      if (KSVD .ne. 0)  goto 100

      if (KCOR .eq. 0) then

        write(6,*)
        write(6,*) 'standard error (d,p,q) : '
        do i = 1, npq1
          write( 6, *) i, stderr(i)
        end do

        write(6,*)
        write(6,*) 'covariance, correlation matrices (d,p,q) : '
        do i = 1, npq1
          do j = i, npq1
            write( 6, *) i, j, cov(i,j), cor(i,j)
          end do
       end do

      else

        write(6,*)
        write(6,*) 'covariance matrix (d,p,q) : '
        do i = 1, npq1
          do j = i, npq1
            write( 6, *) i, j, cov(i,j)
          end do
       end do

      end if

 100  write( 6, *)
      write( 6, *) 'interval of uncertainty for d : ', dtol
      write( 6, *) '                  range for d : ', drange
      write( 6, *) '   finite difference interval :', h
      write( 6, *) '   relative machine precision : ', EPSMAX

 200  write(6,*)
      write(6,*) 'work space  required', lfree
      write(6,*) 'work space allocated', lenw

 300  call fdmsg

      return
 900  format( 4h itr, 14h     d          ,   14h    est mean  , 
     *                16h     white noise,  17h     log likelihd,
     *                 4h  nf, 3h ng)
      end

*******************************************************************************
*******************************************************************************

      subroutine fdmsg()

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

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley 

c-----------------------------------------------------------------------------

      if (ILIMIT .ne. 0) then
        write(6,*)
        write(6,*) '+++ insufficient storage'
        write(6,*) '    increase work space by at least ', ILIMIT
      end if

      if (IGAMMA .ne. 0) then
        write(6,*)
        write(6,*) '+++ error condition in gamma function computation'
        if (IGAMMA .eq. 11) write(6,*) 
     *    'dgamma : x = 0'
        if (IGAMMA .eq. 12) write(6,*) 
     *    'dgamma : x too close to a negative integer'
        if (IGAMMA .eq. 13) write(6,*) 
     *    'dgamma : x so close to 0. that result would overflow'
        if (IGAMMA .eq. 14) write(6,*) 
     *    'dgamma : x so large that result would overflow'
        if (IGAMMA .eq. 21) write(6,*) 
     *    'd9gaml : unable to compute xmin'
        if (IGAMMA .eq. 22) write(6,*) 
     *    'd9gaml : unable to compute xmax'
        if (IGAMMA .eq. 41) write(6,*) 
     *    'dcsevl : number of terms < 0'
        if (IGAMMA .eq. 42) write(6,*) 
     *    'dcsevl : more than 1000 terms'
        if (IGAMMA .eq. 43) write(6,*) 
     *    'dcsevl : x outside (-1,+1)'
        if (IGAMMA .eq. 51) write(6,*) 
     *    'd9lgmc : x must be >= 10'
        if (IGAMMA .eq. 61) write(6,*) 
     *    'dlngam : abs(x) so large that result would overflow'
        if (IGAMMA .eq. 62) write(6,*) 
     *    'dlngam : x is a negative integer'
        write(6,*)
      end if

      if (IMINPK .ne. 0) then
        write(6,*)
        write(6,*) '+++ error condition in optimization'
        if (JMINPK .eq. 10) write(6,*) 
     *    'MINPACK : improper input parameters'
        write(6,*)
      end if

      if (JGAMMA .ne. 0) then
        write(6,*)
        write(6,*) '+++ WARNING in gamma function computation'
        if (JGAMMA .eq. 11) write(6,*) 
     *    'dgamma : precision lost since x too near a negative integer'
        if (JGAMMA .eq. 12) write(6,*) 
     *    'dgamma : x so small that result underflows'
        if (JGAMMA .eq. 31) write(6,*) 
     *    'initds : number of coefficients < 1'
        if (JGAMMA .eq. 32) write(6,*) 
     *    'initds : eta may be too small'
        if (JGAMMA .eq. 51) write(6,*) 
     *    'd9lgmc : x so big that result would overflow'
        if (JGAMMA .eq. 61) write(6,*) 
     *    'dlngam : precision lost since x too near a negative integer'
        write(6,*)
      end if

      if (JMINPK .ne. 0) then
        write(6,*)
        write(6,*) '+++ WARNING in optimization'
        if (JMINPK .eq. 5) write(6,*) 
     *    'MINPACK : function evaluation limit reached'
        if (JMINPK .eq. 6) write(6,*) 
     *    'MINPACK : ftol is too small'
        if (JMINPK .eq. 7) write(6,*) 
     *    'MINPACK : xtol is too small'
        if (JMINPK .eq. 8) write(6,*) 
     *    'MINPACK : gtol is too small'
        write(6,*)
      end if

      if (JLIMIT .ne. 0) then
        write(6,*)
        write(6,*) '+++ WARNING : optimization limit reached'
        write(6,*)
      end if

      if (KSVD .ne. 0) then
        write(6,*)
        write(6,*) 
     *    'dsvdc : cannot compute singular values of covariance matrix'
        return
      end if 

      if (KCOV .ne. 0) then
        write(6,*)
        write(6,*) 
     *      '+++ Hessian matrix is singular, pseudo-inverse returned'
      end if 

      if (KCOR .ne. 0) then
        write(6,*)
        write(6,*) 
     *  '+++ covariance matrix is not computationally positive definite'
      end if 

      return
      end

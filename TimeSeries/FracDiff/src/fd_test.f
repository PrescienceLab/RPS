c note : the workspace limit is set to 30000 but may be modified
c standard output unit number is assumed to be 6

      implicit double precision (a-h,o-z)

      integer           lenw
      data              lenw /54011/
      double precision  w(54011)

      real              x(10800)
      
c      real              p(5), q(5)
      double precision  ar(1), ma(1), drange(2)
      double precision  cov(3,3), se(3), cor(3,3), hess(3,3)

      integer           np, nq, n, M

      double precision  d, hood, dtol

      double precision  junk, mean

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley (3/91)


c-----------------------------------------------------------------------------

c      open( unit=6, file="fracdiff.out")

      np = 1
      nq = 1

      n = 10800




c      call fdsim( n, np, nq, p, q, .3, 0., w, x)
      mean=0
      do i=1, n
         read (*,*) junk, x(i)
         mean=mean+x(i)
      end do
      mean=mean/n
c normalize to zero mean, just in case
c      do i=1,n
c         x(i) = x(i) - mean
c      end do
      print *, "First 10  values are"
      do i=1, 10
         print *, x(i)
      end do

      M     = 100

      dtol = -1.d0

      do i = 1, np
        ar(i) = 0.d0
      end do

      d     = 0.d0

      do i = 1, nq
        ma(i) = 0.d0
      end do

      drange(1) = 0.d0
      drange(2) = .5d0

      h = -1.d0

      print *,"Calling fracdf"
      call fracdf( x, n, M, np, nq, dtol, drange, h, hood, d, ar, ma, 
     *             cov, 3, se, cor, 3, hess, 3, w, lenw, 1)
      print *,"Finished fracdf"

      print *,"d=",d

c recompute covariance, etc, using a different finite-difference interval

      h = .0001d0

      print *,"Calling fdvar"

      call fdvar( x, n, M, np, nq, dtol, drange, h, hood, d, ar, ma, 
     *             cov, 3, se, cor, 3, hess, 3, w, lenw)

      print *,"Finished fdvar successfully"
      
      print *, "d=", d
      print *, "ar="
      write (*,*) (ar(i),i=1,np)
      print *, "ma="
      write (*,*) (ma(i),i=1,nq)


c      print *,"Calling fdsim"
c      call fdsim( n, np, nq, ar, ma, d, 0., w, x)
c      print *,"Finished fssim successfully"
c
c      print *,"outputing sequence"
c      open (unit=10, file="sequence.out")
c      do i=1,n
c         write (10,*) x(i)
c      end do
c      print *,"done"
c      close (10)


      stop
 900  write(6,*) '+++ input error'
      stop
      end





c note : the workspace limit is set to 30000 but may be modified
c standard output unit number is assumed to be 6

      implicit double precision (a-h,o-z)

      integer           lenw
      data              lenw /30000/
      double precision  w(30000)

      real              x(10010)
      
      real              p(5), q(5)
      double precision  ar(5), ma(5), drange(2)
      double precision  cov(6,6), se(6), cor(6,6), hess(6,6)

      integer           np, nq, n, M

      double precision  d, hood, dtol

c copyright 1991 Department of Statistics, University of Washington
c written by Chris Fraley (3/91)


c-----------------------------------------------------------------------------

      open( unit=6, file="fracdiff.out")

      np = 1
      nq = 1

      n = 5000

      p(1) =  .2
      q(1) = -.4

      call fdsim( n, np, nq, p, q, .3, 0., w, x)

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

      call fracdf( x, n, M, np, nq, dtol, drange, h, hood, d, ar, ma, 
     *             cov, 6, se, cor, 6, hess, 6, w, lenw, 1)

c recompute covariance, etc, using a different finite-difference interval

      h = .0001d0

      call fdvar( x, n, M, np, nq, dtol, drange, h, hood, d, ar, ma, 
     *             cov, 6, se, cor, 6, hess, 6, w, lenw)

      stop
 900  write(6,*) '+++ input error'
      stop
      end





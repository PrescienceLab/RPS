      REAL FUNCTION gennor(av,sd)
C**********************************************************************
C
C     REAL FUNCTION GENNOR( AV, SD )
C
C         GENerate random deviate from a NORmal distribution
C
C
C                              Function
C
C
C     Generates a single random deviate from a normal distribution
C     with mean, AV, and standard deviation, SD.
C
C
C                              Arguments
C
C
C     AV --> Mean of the normal distribution.
C                              REAL AV
C
C     SD --> Standard deviation of the normal distribution.
C                              REAL SD
C
C     GENNOR <-- Generated normal deviate.
C                              REAL GENNOR
C
C
C                              Method
C
C
C     Renames SNORM from TOMS as slightly modified by BWB to use RANF
C     instead of SUNIF.
C
C     For details see:
C               Ahrens, J.H. and Dieter, U.
C               Extensions of Forsythe's Method for Random
C               Sampling from the Normal Distribution.
C               Math. Comput., 27,124 (Oct. 1973), 927 - 937.
C
C
C**********************************************************************
C     .. Scalar Arguments ..
      REAL av,sd
C     ..
C     .. External Functions ..
      REAL snorm
      EXTERNAL snorm
C     ..
C     .. Executable Statements ..
      gennor = sd*snorm() + av
      RETURN

      END
      SUBROUTINE getcgn(g)
      INTEGER g
C**********************************************************************
C
C      SUBROUTINE GETCGN(G)
C                         Get GeNerator
C
C     Returns in G the number of the current random number generator
C
C
C                              Arguments
C
C
C     G <-- Number of the current random number generator (1..32)
C                    INTEGER G
C
C**********************************************************************
C
      INTEGER curntg,numg
      SAVE curntg
      PARAMETER (numg=32)
      DATA curntg/1/
C
      g = curntg
      RETURN

      ENTRY setcgn(g)
C**********************************************************************
C
C     SUBROUTINE SETCGN( G )
C                      Set GeNerator
C
C     Sets  the  current  generator to G.    All references to a generat
C     are to the current generator.
C
C
C                              Arguments
C
C
C     G --> Number of the current random number generator (1..32)
C                    INTEGER G
C
C**********************************************************************
C
C     Abort if generator number out of range
C
      IF (.NOT. (g.LT.0.OR.g.GT.numg)) GO TO 10
      WRITE (*,*) ' Generator number out of range in SETCGN:',
     +  ' Legal range is 1 to ',numg,' -- ABORT!'
      STOP ' Generator number out of range in SETCGN'

   10 curntg = g
      RETURN

      END
      INTEGER FUNCTION ignlgi()
C**********************************************************************
C
C     INTEGER FUNCTION IGNLGI()
C               GeNerate LarGe Integer
C
C     Returns a random integer following a uniform distribution over
C     (1, 2147483562) using the current generator.
C
C     This is a transcription from Pascal to Fortran of routine
C     Random from the paper
C
C     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
C     with Splitting Facilities." ACM Transactions on Mathematical
C     Software, 17:98-111 (1991)
C
C**********************************************************************
C     .. Parameters ..
      INTEGER numg
      PARAMETER (numg=32)
C     ..
C     .. Scalars in Common ..
      INTEGER a1,a1vw,a1w,a2,a2vw,a2w,m1,m2
C     ..
C     .. Arrays in Common ..
      INTEGER cg1(numg),cg2(numg),ig1(numg),ig2(numg),lg1(numg),
     +        lg2(numg)
      LOGICAL qanti(numg)
C     ..
C     .. Local Scalars ..
      INTEGER curntg,k,s1,s2,z
      LOGICAL qqssd
C     ..
C     .. External Functions ..
      LOGICAL qrgnin
      EXTERNAL qrgnin
C     ..
C     .. External Subroutines ..
      EXTERNAL getcgn,inrgcm,rgnqsd,setall
C     ..
C     .. Common blocks ..
      COMMON /globe/m1,m2,a1,a2,a1w,a2w,a1vw,a2vw,ig1,ig2,lg1,lg2,cg1,
     +       cg2,qanti
C     ..
C     .. Save statement ..
      SAVE /globe/
C     ..
C     .. Executable Statements ..
C
C     IF THE RANDOM NUMBER PACKAGE HAS NOT BEEN INITIALIZED YET, DO SO.
C     IT CAN BE INITIALIZED IN ONE OF TWO WAYS : 1) THE FIRST CALL TO
C     THIS ROUTINE  2) A CALL TO SETALL.
C
      IF (.NOT. (qrgnin())) CALL inrgcm()
      CALL rgnqsd(qqssd)
      IF (.NOT. (qqssd)) CALL setall(1234567890,123456789)
C
C     Get Current Generator
C
      CALL getcgn(curntg)
      s1 = cg1(curntg)
      s2 = cg2(curntg)
      k = s1/53668
      s1 = a1* (s1-k*53668) - k*12211
      IF (s1.LT.0) s1 = s1 + m1
      k = s2/52774
      s2 = a2* (s2-k*52774) - k*3791
      IF (s2.LT.0) s2 = s2 + m2
      cg1(curntg) = s1
      cg2(curntg) = s2
      z = s1 - s2
      IF (z.LT.1) z = z + m1 - 1
      IF (qanti(curntg)) z = m1 - z
      ignlgi = z
      RETURN

      END
      SUBROUTINE initgn(isdtyp)
C**********************************************************************
C
C     SUBROUTINE INITGN(ISDTYP)
C          INIT-ialize current G-e-N-erator
C
C     Reinitializes the state of the current generator
C
C     This is a transcription from Pascal to Fortran of routine
C     Init_Generator from the paper
C
C     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
C     with Splitting Facilities." ACM Transactions on Mathematical
C     Software, 17:98-111 (1991)
C
C
C                              Arguments
C
C
C     ISDTYP -> The state to which the generator is to be set
C
C          ISDTYP = -1  => sets the seeds to their initial value
C          ISDTYP =  0  => sets the seeds to the first value of
C                          the current block
C          ISDTYP =  1  => sets the seeds to the first value of
C                          the next block
C
C                                   INTEGER ISDTYP
C
C**********************************************************************
C     .. Parameters ..
      INTEGER numg
      PARAMETER (numg=32)
C     ..
C     .. Scalar Arguments ..
      INTEGER isdtyp
C     ..
C     .. Scalars in Common ..
      INTEGER a1,a1vw,a1w,a2,a2vw,a2w,m1,m2
C     ..
C     .. Arrays in Common ..
      INTEGER cg1(numg),cg2(numg),ig1(numg),ig2(numg),lg1(numg),
     +        lg2(numg)
      LOGICAL qanti(numg)
C     ..
C     .. Local Scalars ..
      INTEGER g
C     ..
C     .. External Functions ..
      LOGICAL qrgnin
      INTEGER mltmod
      EXTERNAL qrgnin,mltmod
C     ..
C     .. External Subroutines ..
      EXTERNAL getcgn
C     ..
C     .. Common blocks ..
      COMMON /globe/m1,m2,a1,a2,a1w,a2w,a1vw,a2vw,ig1,ig2,lg1,lg2,cg1,
     +       cg2,qanti
C     ..
C     .. Save statement ..
      SAVE /globe/
C     ..
C     .. Executable Statements ..
C     Abort unless random number generator initialized
      IF (qrgnin()) GO TO 10
      WRITE (*,*) ' INITGN called before random number generator ',
     +  ' initialized -- abort!'
      STOP ' INITGN called before random number generator initialized'

   10 CALL getcgn(g)
      IF ((-1).NE. (isdtyp)) GO TO 20
      lg1(g) = ig1(g)
      lg2(g) = ig2(g)
      GO TO 50

   20 IF ((0).NE. (isdtyp)) GO TO 30
      CONTINUE
      GO TO 50
C     do nothing
   30 IF ((1).NE. (isdtyp)) GO TO 40
      lg1(g) = mltmod(a1w,lg1(g),m1)
      lg2(g) = mltmod(a2w,lg2(g),m2)
      GO TO 50

   40 STOP 'ISDTYP NOT IN RANGE'

   50 cg1(g) = lg1(g)
      cg2(g) = lg2(g)
      RETURN

      END
      SUBROUTINE inrgcm()
C**********************************************************************
C
C     SUBROUTINE INRGCM()
C          INitialize Random number Generator CoMmon
C
C
C                              Function
C
C
C     Initializes common area  for random number  generator.  This saves
C     the  nuisance  of  a  BLOCK DATA  routine  and the  difficulty  of
C     assuring that the routine is loaded with the other routines.
C
C**********************************************************************
C     .. Parameters ..
      INTEGER numg
      PARAMETER (numg=32)
C     ..
C     .. Scalars in Common ..
      INTEGER a1,a1vw,a1w,a2,a2vw,a2w,m1,m2
C     ..
C     .. Arrays in Common ..
      INTEGER cg1(numg),cg2(numg),ig1(numg),ig2(numg),lg1(numg),
     +        lg2(numg)
      LOGICAL qanti(numg)
C     ..
C     .. Local Scalars ..
      INTEGER i
      LOGICAL qdum
C     ..
C     .. External Functions ..
      LOGICAL qrgnsn
      EXTERNAL qrgnsn
C     ..
C     .. Common blocks ..
      COMMON /globe/m1,m2,a1,a2,a1w,a2w,a1vw,a2vw,ig1,ig2,lg1,lg2,cg1,
     +       cg2,qanti
C     ..
C     .. Save statement ..
      SAVE /globe/
C     ..
C     .. Executable Statements ..
C     V=20;                            W=30;
C
C     A1W = MOD(A1**(2**W),M1)         A2W = MOD(A2**(2**W),M2)
C     A1VW = MOD(A1**(2**(V+W)),M1)    A2VW = MOD(A2**(2**(V+W)),M2)
C
C   If V or W is changed A1W, A2W, A1VW, and A2VW need to be recomputed.
C    An efficient way to precompute a**(2*j) MOD m is to start with
C    a and square it j times modulo m using the function MLTMOD.
C
      m1 = 2147483563
      m2 = 2147483399
      a1 = 40014
      a2 = 40692
      a1w = 1033780774
      a2w = 1494757890
      a1vw = 2082007225
      a2vw = 784306273
      DO 10,i = 1,numg
          qanti(i) = .FALSE.
   10 CONTINUE
C
C     Tell the world that common has been initialized
C
      qdum = qrgnsn(.TRUE.)
      RETURN

      END
      INTEGER FUNCTION mltmod(a,s,m)
C**********************************************************************
C
C     INTEGER FUNCTION MLTMOD(A,S,M)
C
C                    Returns (A*S) MOD M
C
C     This is a transcription from Pascal to Fortran of routine
C     MULtMod_Decompos from the paper
C
C     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
C     with Splitting Facilities." ACM Transactions on Mathematical
C     Software, 17:98-111 (1991)
C
C
C                              Arguments
C
C
C     A, S, M  -->
C                         INTEGER A,S,M
C
C**********************************************************************
C     .. Parameters ..
      INTEGER h
      PARAMETER (h=32768)
C     ..
C     .. Scalar Arguments ..
      INTEGER a,m,s
C     ..
C     .. Local Scalars ..
      INTEGER a0,a1,k,p,q,qh,rh
C     ..
C     .. Executable Statements ..
C
C     H = 2**((b-2)/2) where b = 32 because we are using a 32 bit
C      machine. On a different machine recompute H
C
      IF (.NOT. (a.LE.0.OR.a.GE.m.OR.s.LE.0.OR.s.GE.m)) GO TO 10
      WRITE (*,*) ' A, M, S out of order in MLTMOD - ABORT!'
      WRITE (*,*) ' A = ',a,' S = ',s,' M = ',m
      WRITE (*,*) ' MLTMOD requires: 0 < A < M; 0 < S < M'
      STOP ' A, M, S out of order in MLTMOD - ABORT!'

   10 IF (.NOT. (a.LT.h)) GO TO 20
      a0 = a
      p = 0
      GO TO 120

   20 a1 = a/h
      a0 = a - h*a1
      qh = m/h
      rh = m - h*qh
      IF (.NOT. (a1.GE.h)) GO TO 50
      a1 = a1 - h
      k = s/qh
      p = h* (s-k*qh) - k*rh
   30 IF (.NOT. (p.LT.0)) GO TO 40
      p = p + m
      GO TO 30

   40 GO TO 60

   50 p = 0
C
C     P = (A2*S*H)MOD M
C
   60 IF (.NOT. (a1.NE.0)) GO TO 90
      q = m/a1
      k = s/q
      p = p - k* (m-a1*q)
      IF (p.GT.0) p = p - m
      p = p + a1* (s-k*q)
   70 IF (.NOT. (p.LT.0)) GO TO 80
      p = p + m
      GO TO 70

   80 CONTINUE
   90 k = p/qh
C
C     P = ((A2*H + A1)*S)MOD M
C
      p = h* (p-k*qh) - k*rh
  100 IF (.NOT. (p.LT.0)) GO TO 110
      p = p + m
      GO TO 100

  110 CONTINUE
  120 IF (.NOT. (a0.NE.0)) GO TO 150
C
C     P = ((A2*H + A1)*H*S)MOD M
C
      q = m/a0
      k = s/q
      p = p - k* (m-a0*q)
      IF (p.GT.0) p = p - m
      p = p + a0* (s-k*q)
  130 IF (.NOT. (p.LT.0)) GO TO 140
      p = p + m
      GO TO 130

  140 CONTINUE
  150 mltmod = p
C
      RETURN

      END
      LOGICAL FUNCTION qrgnin()
C**********************************************************************
C
C     LOGICAL FUNCTION QRGNIN()
C               Q Random GeNerators INitialized?
C
C     A trivial routine to determine whether or not the random
C     number generator has been initialized.  Returns .TRUE. if
C     it has, else .FALSE.
C
C**********************************************************************
C     .. Scalar Arguments ..
      LOGICAL qvalue
C     ..
C     .. Local Scalars ..
      LOGICAL qinit
C     ..
C     .. Entry Points ..
      LOGICAL qrgnsn
C     ..
C     .. Save statement ..
      SAVE qinit
C     ..
C     .. Data statements ..
      DATA qinit/.FALSE./
C     ..
C     .. Executable Statements ..
      qrgnin = qinit
      RETURN

      ENTRY qrgnsn(qvalue)
C**********************************************************************
C
C     LOGICAL FUNCTION QRGNSN( QVALUE )
C               Q Random GeNerators Set whether iNitialized
C
C     Sets state of whether random number generator is initialized
C     to QVALUE.
C
C     This routine is actually an entry in QRGNIN, hence it is a
C     logical function.  It returns the (meaningless) value .TRUE.
C
C**********************************************************************
      qinit = qvalue
      qrgnsn = .TRUE.
      RETURN

      END
      REAL FUNCTION ranf()
C**********************************************************************
C
C     REAL FUNCTION RANF()
C                RANDom number generator as a Function
C
C     Returns a random floating point number from a uniform distribution
C     over 0 - 1 (endpoints of this interval are not returned) using the
C     current generator
C
C     This is a transcription from Pascal to Fortran of routine
C     Uniform_01 from the paper
C
C     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
C     with Splitting Facilities." ACM Transactions on Mathematical
C     Software, 17:98-111 (1991)
C
C**********************************************************************
C     .. External Functions ..
      INTEGER ignlgi
      EXTERNAL ignlgi
C     ..
C     .. Executable Statements ..
C
C     4.656613057E-10 is 1/M1  M1 is set in a data statement in IGNLGI
C      and is currently 2147483563. If M1 changes, change this also.
C
      ranf = ignlgi()*4.656613057E-10
      RETURN

      END
      SUBROUTINE setall(iseed1,iseed2)
C**********************************************************************
C
C      SUBROUTINE SETALL(ISEED1,ISEED2)
C               SET ALL random number generators
C
C     Sets the initial seed of generator 1 to ISEED1 and ISEED2. The
C     initial seeds of the other generators are set accordingly, and
C     all generators states are set to these seeds.
C
C     This is a transcription from Pascal to Fortran of routine
C     Set_Initial_Seed from the paper
C
C     L'Ecuyer, P. and Cote, S. "Implementing a Random Number Package
C     with Splitting Facilities." ACM Transactions on Mathematical
C     Software, 17:98-111 (1991)
C
C
C                              Arguments
C
C
C     ISEED1 -> First of two integer seeds
C                                   INTEGER ISEED1
C
C     ISEED2 -> Second of two integer seeds
C                                   INTEGER ISEED1
C
C**********************************************************************
C     .. Parameters ..
      INTEGER numg
      PARAMETER (numg=32)
C     ..
C     .. Scalar Arguments ..
      INTEGER iseed1,iseed2
      LOGICAL qssd
C     ..
C     .. Scalars in Common ..
      INTEGER a1,a1vw,a1w,a2,a2vw,a2w,m1,m2
C     ..
C     .. Arrays in Common ..
      INTEGER cg1(numg),cg2(numg),ig1(numg),ig2(numg),lg1(numg),
     +        lg2(numg)
      LOGICAL qanti(numg)
C     ..
C     .. Local Scalars ..
      INTEGER g,ocgn
      LOGICAL qqssd
C     ..
C     .. External Functions ..
      INTEGER mltmod
      LOGICAL qrgnin
      EXTERNAL mltmod,qrgnin
C     ..
C     .. External Subroutines ..
      EXTERNAL getcgn,initgn,inrgcm,setcgn
C     ..
C     .. Common blocks ..
      COMMON /globe/m1,m2,a1,a2,a1w,a2w,a1vw,a2vw,ig1,ig2,lg1,lg2,cg1,
     +       cg2,qanti
C     ..
C     .. Save statement ..
      SAVE /globe/,qqssd
C     ..
C     .. Data statements ..
      DATA qqssd/.FALSE./
C     ..
C     .. Executable Statements ..
C
C     TELL IGNLGI, THE ACTUAL NUMBER GENERATOR, THAT THIS ROUTINE
C      HAS BEEN CALLED.
C
      qqssd = .TRUE.
      CALL getcgn(ocgn)
C
C     Initialize Common Block if Necessary
C
      IF (.NOT. (qrgnin())) CALL inrgcm()
      ig1(1) = iseed1
      ig2(1) = iseed2
      CALL initgn(-1)
      DO 10,g = 2,numg
          ig1(g) = mltmod(a1vw,ig1(g-1),m1)
          ig2(g) = mltmod(a2vw,ig2(g-1),m2)
          CALL setcgn(g)
          CALL initgn(-1)
   10 CONTINUE
      CALL setcgn(ocgn)
      RETURN

      ENTRY rgnqsd(qssd)
C**********************************************************************
C
C     SUBROUTINE RGNQSD
C                    Random Number Generator Query SeeD set?
C
C     Returns (LOGICAL) QSSD as .TRUE. if SETALL has been invoked,
C     otherwise returns .FALSE.
C
C**********************************************************************
      qssd = qqssd
      RETURN

      END
      REAL FUNCTION snorm()
C**********************************************************************C
C                                                                      C
C                                                                      C
C     (STANDARD-)  N O R M A L  DISTRIBUTION                           C
C                                                                      C
C                                                                      C
C**********************************************************************C
C**********************************************************************C
C                                                                      C
C     FOR DETAILS SEE:                                                 C
C                                                                      C
C               AHRENS, J.H. AND DIETER, U.                            C
C               EXTENSIONS OF FORSYTHE'S METHOD FOR RANDOM             C
C               SAMPLING FROM THE NORMAL DISTRIBUTION.                 C
C               MATH. COMPUT., 27,124 (OCT. 1973), 927 - 937.          C
C                                                                      C
C     ALL STATEMENT NUMBERS CORRESPOND TO THE STEPS OF ALGORITHM 'FL'  C
C     (M=5) IN THE ABOVE PAPER     (SLIGHTLY MODIFIED IMPLEMENTATION)  C
C                                                                      C
C     Modified by Barry W. Brown, Feb 3, 1988 to use RANF instead of   C
C     SUNIF.  The argument IR thus goes away.                          C
C                                                                      C
C**********************************************************************C
C
      DIMENSION a(32),d(31),t(31),h(31)
C
C     THE DEFINITIONS OF THE CONSTANTS A(K), D(K), T(K) AND
C     H(K) ARE ACCORDING TO THE ABOVEMENTIONED ARTICLE
C
      DATA a/0.0,.3917609E-1,.7841241E-1,.1177699,.1573107,.1970991,
     +     .2372021,.2776904,.3186394,.3601299,.4022501,.4450965,
     +     .4887764,.5334097,.5791322,.6260990,.6744898,.7245144,
     +     .7764218,.8305109,.8871466,.9467818,1.009990,1.077516,
     +     1.150349,1.229859,1.318011,1.417797,1.534121,1.675940,
     +     1.862732,2.153875/
      DATA d/5*0.0,.2636843,.2425085,.2255674,.2116342,.1999243,
     +     .1899108,.1812252,.1736014,.1668419,.1607967,.1553497,
     +     .1504094,.1459026,.1417700,.1379632,.1344418,.1311722,
     +     .1281260,.1252791,.1226109,.1201036,.1177417,.1155119,
     +     .1134023,.1114027,.1095039/
      DATA t/.7673828E-3,.2306870E-2,.3860618E-2,.5438454E-2,
     +     .7050699E-2,.8708396E-2,.1042357E-1,.1220953E-1,.1408125E-1,
     +     .1605579E-1,.1815290E-1,.2039573E-1,.2281177E-1,.2543407E-1,
     +     .2830296E-1,.3146822E-1,.3499233E-1,.3895483E-1,.4345878E-1,
     +     .4864035E-1,.5468334E-1,.6184222E-1,.7047983E-1,.8113195E-1,
     +     .9462444E-1,.1123001,.1364980,.1716886,.2276241,.3304980,
     +     .5847031/
      DATA h/.3920617E-1,.3932705E-1,.3950999E-1,.3975703E-1,
     +     .4007093E-1,.4045533E-1,.4091481E-1,.4145507E-1,.4208311E-1,
     +     .4280748E-1,.4363863E-1,.4458932E-1,.4567523E-1,.4691571E-1,
     +     .4833487E-1,.4996298E-1,.5183859E-1,.5401138E-1,.5654656E-1,
     +     .5953130E-1,.6308489E-1,.6737503E-1,.7264544E-1,.7926471E-1,
     +     .8781922E-1,.9930398E-1,.1155599,.1404344,.1836142,.2790016,
     +     .7010474/
C
   10 u = ranf()
      s = 0.0
      IF (u.GT.0.5) s = 1.0
      u = u + u - s
   20 u = 32.0*u
      i = int(u)
      IF (i.EQ.32) i = 31
      IF (i.EQ.0) GO TO 100
C
C                                START CENTER
C
   30 ustar = u - float(i)
      aa = a(i)
   40 IF (ustar.LE.t(i)) GO TO 60
      w = (ustar-t(i))*h(i)
C
C                                EXIT   (BOTH CASES)
C
   50 y = aa + w
      snorm = y
      IF (s.EQ.1.0) snorm = -y
      RETURN
C
C                                CENTER CONTINUED
C
   60 u = ranf()
      w = u* (a(i+1)-aa)
      tt = (0.5*w+aa)*w
      GO TO 80

   70 tt = u
      ustar = ranf()
   80 IF (ustar.GT.tt) GO TO 50
   90 u = ranf()
      IF (ustar.GE.u) GO TO 70
      ustar = ranf()
      GO TO 40
C
C                                START TAIL
C
  100 i = 6
      aa = a(32)
      GO TO 120

  110 aa = aa + d(i)
      i = i + 1
  120 u = u + u
      IF (u.LT.1.0) GO TO 110
  130 u = u - 1.0
  140 w = u*d(i)
      tt = (0.5*w+aa)*w
      GO TO 160

  150 tt = u
  160 ustar = ranf()
      IF (ustar.GT.tt) GO TO 50
  170 u = ranf()
      IF (ustar.GE.u) GO TO 150
      u = ranf()
      GO TO 140

      END

/* -----------------------------------------------------------------
                             Euler

     This module implements functions for dealing with Euler angles.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2004-2014, Michael T. Duffy II.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
// Redistributions of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGE.

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Math/RVec.hpp"
#include "Math/RMatrix.hpp"
#include "Gfx/Euler.hpp"


// REFACTOR (mduffy) : Determine if ToMatrixShoemaker works, and then make a final
//  decision between ToMatrixShoemaker and and ToMatrix.


// Euler routines, based on the article "Euler Angle Conversions" in Graphic Gems IV

//enum QuatPart {X, Y, Z, W};
// Order type constants, constructors, extractors
// There are 24 possible conventions, designated by:
// o  EulAxI = axis used initially
// o  EulPar = parity of axis permutation
// o  EulRep = repetition of initial axis as last
// o  EulFrm = frame from which axes are taken
// Axes I, J, K will be a permutation of X, Y,Z.
// Axis H will be either I or K, depending on EulRep.
//  Frame S takes axes from initial static frame.
//  If ord = (AxI=X, Par=Even, Rep=No, Frm=S), then
// (a,b,c,ord) means Rz(c)Ry(b)Rx(a), where Rz(c)v
//  rotates v around z by c radians

#define EulFrmS      0
#define EulFrmR      1
#define EulFrm(ord)  ((unsigned)(ord)&1)
#define EulRepNo     0
#define EulRepYes    1
#define EulRep(ord)  (((unsigned)(ord)>>1)&1)
#define EulParEven   0
#define EulParOdd    1
#define EulPar(ord)  (((unsigned)(ord)>>2)&1)
#define EulSafe      "\000\001\002\000"
#define EulNext      "\001\002\000\001"
#define EulAxI(ord)   ((int)(EulSafe[(((unsigned)(ord)>>3)&3)]))
#define EulAxJ(ord)   ((int)(EulNext[EulAxI(ord)+(EulPar(ord)==EulParOdd)]))
#define EulAxK(ord)   ((int)(EulNext[EulAxI(ord)+(EulPar(ord)!=EulParOdd)]))
#define EulAxH(ord)   ((EulRep(ord)==EulRepNo)?EulAxK(ord):EulAxI(ord))
  // EulGetOrd upacks all useful information about order simultaneously
// #define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned o=ord;
#define EulGetOrd(ord,i,j,k,n,s,f)   {unsigned o=ord;\
                                      f=o&1;o>>=1;\
                                      s=o&1;o>>=1;\
                                      n=o&1;o>>=1;\
                                      i=EulSafe[o&3];\
                                      j=EulNext[i+n];\
                                      k=EulNext[i+1-n];}
//                                      h=s?k:i;}
  // EulOrd creates an order value between 0 and 23 from 4-tupple choices
#define EulOrd(i,p,r,f)   (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))
  // static axes
#define EulOrdXYZs      EulOrd(X, EulParEven, EulRepNo,  EulFrmS)
#define EulOrdXYXs      EulOrd(X, EulParEven, EulRepYes, EulFrmS)
#define EulOrdXZYs      EulOrd(X, EulParOdd,  EulRepNo,  EulFrmS)
#define EulOrdXZXs      EulOrd(X, EulParOdd,  EulRepYes, EulFrmS)
#define EulOrdYZXs      EulOrd(Y, EulParEven, EulRepNo,  EulFrmS)
#define EulOrdYZYs      EulOrd(Y, EulParEven, EulRepYes, EulFrmS)
#define EulOrdYXZs      EulOrd(Y, EulParOdd,  EulRepNo,  EulFrmS)
#define EulOrdYXYs      EulOrd(Y, EulParOdd,  EulRepYes, EulFrmS)
#define EulOrdZXYs      EulOrd(Z, EulParEven, EulRepNo,  EulFrmS)
#define EulOrdZXZs      EulOrd(Z, EulParEven, EulRepYes, EulFrmS)
#define EulOrdZYXs      EulOrd(Z, EulParOdd,  EulRepNo,  EulFrmS)
#define EulOrdZYZs      EulOrd(Z, EulParOdd,  EulRepYes, EulFrmS)
  // rotating axes
#define EulOrdZYXr      EulOrd(X, EulParEven, EulRepNo,  EulFrmR)
#define EulOrdXYXr      EulOrd(X, EulParEven, EulRepYes, EulFrmR)
#define EulOrdYZXr      EulOrd(X, EulParOdd,  EulRepNo,  EulFrmR)
#define EulOrdXZXr      EulOrd(X, EulParOdd,  EulRepYes, EulFrmR)
#define EulOrdXZYr      EulOrd(Y, EulParEven, EulRepNo,  EulFrmR)
#define EulOrdYZYr      EulOrd(Y, EulParEven, EulRepYes, EulFrmR)
#define EulOrdZXYr      EulOrd(Y, EulParOdd,  EulRepNo,  EulFrmR)
#define EulOrdYXYr      EulOrd(Y, EulParOdd,  EulRepYes, EulFrmR)
#define EulOrdYXZr      EulOrd(Z, EulParEven, EulRepNo,  EulFrmR)
#define EulOrdZXZr      EulOrd(Z, EulParEven, EulRepYes, EulFrmR)
#define EulOrdXYZr      EulOrd(Z, EulParOdd,  EulRepNo,  EulFrmR)
#define EulOrdZYZr      EulOrd(Z, EulParOdd,  EulRepYes, EulFrmR)


//-----------------------------------------------------------------------------
//  Euler
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Euler::Euler ()
  {
  eOrder = YXZ;
  }

//-----------------------------------------------------------------------------
Euler::Euler (const Euler &  eulIn) : RVec3 ()
  {
  fX = eulIn.fX;
  fY = eulIn.fY;
  fZ = eulIn.fZ;
  eOrder = eulIn.eOrder;
  }

//-----------------------------------------------------------------------------
VOID Euler::ToQuaternion (RVec4 &  vecQuatOut)
  {
  // This code based on Euler Angle Conversions, Graphic Gems IV pp 222-229

  DOUBLE fXAngle = this->fX * R_DEG_TO_RAD;
  DOUBLE fYAngle = this->fY * R_DEG_TO_RAD;
  DOUBLE fZAngle = this->fZ * R_DEG_TO_RAD;

  DOUBLE a[3], ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
  int i, j, k, n, s, f;
  //EulGetOrd(int(this->fW),i,j,k,h,n,s,f);
  EulGetOrd(int(this->fW),i,j,k,n,s,f);
  if (f==EulFrmR)    {RVEC_T t = RVEC_T (fXAngle); fXAngle = fZAngle; fZAngle = t;};
  if (n==EulParOdd)  fYAngle = -fYAngle;
  ti = fXAngle*0.5; tj = fYAngle*0.5; th = fZAngle * 0.5;
  ci = cos(ti*R_DEG_TO_RAD);  cj = cos(tj*R_DEG_TO_RAD);  ch = cos(th*R_DEG_TO_RAD);
  si = sin(ti*R_DEG_TO_RAD);  sj = sin(tj*R_DEG_TO_RAD);  sh = sin(th*R_DEG_TO_RAD);
  cc = ci*ch; cs=ci*sh; sc=si*ch; ss=si*sh;
  if (s==EulRepYes)
    {
    a[i]  = cj*(cs+sc);   // could speed up with
    a[j]  = sj*(cc+ss);   // trig identities
    a[k]  = sj*(cs-sc);
    vecQuatOut.fW = RVEC_T (cj*(cc-ss));
    }
  else
    {
    a[i]  = cj*sc-sj*cs;
    a[j]  = cj*ss+sj*cc;
    a[k]  = cj*cs-sj*sc;
    vecQuatOut.fW = RVEC_T (cj*cc+sj*ss);
    };
  if (n==EulParOdd) a[j] = -a[j];
  vecQuatOut.fX = RVEC_T (a[0]);
  vecQuatOut.fY = RVEC_T (a[1]);
  vecQuatOut.fZ = RVEC_T (a[2]);
  };


//-----------------------------------------------------------------------------
VOID  Euler::ToMatrixShoemaker (RMatrix &  matOut)
  {
  // accepts rotation in radians

  // convert our simplified rotation order list to the more complex/complete
  //  one that Shoemaker used.
  INT  iOrderList [] = {0, 2, 4, 6, 8, 10};
  INT  iMultOrder = iOrderList [(INT)eOrder];

  // This code based on Euler Angle Conversions, Graphic Gems IV pp 222-229

  DOUBLE  ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
  int     i, j, k, n, s, f;
  //EulGetOrd (iMultOrder, i, j, k, h, n, s, f);
  EulGetOrd (iMultOrder, i, j, k, n, s, f);

  DOUBLE fXAngle = this->fX * R_DEG_TO_RAD;
  DOUBLE fYAngle = this->fY * R_DEG_TO_RAD;
  DOUBLE fZAngle = this->fZ * R_DEG_TO_RAD;

  if (f==EulFrmR)    {RVEC_T t = RVEC_T (fXAngle); fXAngle = fZAngle; fZAngle = t;};
  if (n==EulParOdd)  {fXAngle = -fXAngle; fYAngle = -fYAngle; fZAngle = -fZAngle;};

  ti = fXAngle; tj = fYAngle; th = fZAngle;
  ci = cos(ti);  cj = cos(tj);  ch = cos(th);
  si = sin(ti);  sj = sin(tj);  sh = sin(th);
  cc = ci*ch; cs=ci*sh; sc=si*ch; ss=si*sh;

  if (s==EulRepYes)
    {
    matOut.fArray[i+i*4] = cj;     matOut.fArray[i+j*4] = sj*si;     matOut.fArray[i+k*4] = sj*ci;
    matOut.fArray[j+i*4] = sj*sh;  matOut.fArray[j+j*4] = -cj*ss+cc; matOut.fArray[j+k*4] = -cj*cs-sc;
    matOut.fArray[k+i*4] = -sj*ch; matOut.fArray[k+j*4] = cj*sc+cs;  matOut.fArray[k+k*4] = cj*cc-ss;
    }
  else
    {
    matOut.fArray[i+i*4] = cj*ch;  matOut.fArray[i+j*4] = sj*sc-cs;  matOut.fArray[i+k*4] = sj*cc+ss;
    matOut.fArray[j+i*4] = cj*sh;  matOut.fArray[j+j*4] = sj*ss+cc;  matOut.fArray[j+k*4] = sj*cs-sc;
    matOut.fArray[k+i*4] = -sj;    matOut.fArray[k+j*4] = cj*si;     matOut.fArray[k+k*4] = cj*ci;
    };

                                                     matOut.f03=0.0f;
                                                     matOut.f13=0.0f;
                                                     matOut.f23=0.0f;
  matOut.f30=0.0f; matOut.f31=0.0f; matOut.f32=0.0f; matOut.f33=1.0f;
  };

//-----------------------------------------------------------------------------
VOID  Euler::ToMatrix (RMatrix &  matOut)
  {
  RMatrix  matRotX;
  RMatrix  matRotY;
  RMatrix  matRotZ;

  matRotX.SetRotateX (this->fX * R_DEG_TO_RAD);
  matRotY.SetRotateY (this->fY * R_DEG_TO_RAD);
  matRotZ.SetRotateZ (this->fZ * R_DEG_TO_RAD);

  switch (eOrder)
    {
    case XYZ:
          matOut = matRotZ * matRotY * matRotX; break;
    case XZY:
          matOut = matRotY * matRotZ * matRotX; break;
    case YZX:
          matOut = matRotX * matRotZ * matRotY; break;
    case YXZ:
          matOut = matRotZ * matRotX * matRotY; break;
    case ZXY:
          matOut = matRotY * matRotX * matRotZ; break;
    case ZYX:
          matOut = matRotX * matRotY * matRotZ; break;
    };
  };

// This approach to converting Matrix to Euler is derived from the blog post at:
// http://khayyam.kaplinski.com/2011/06/more-about-matrix-to-euler-angles.html

//  f00  f01  f02  f03        r00  r01  r02  tx         0   4   8   12
//  f10  f11  f12  f13   or   r10  r11  r12  ty   or    1   5   9   13
//  f20  f21  f22  f23        r20  r21  r22  tz         2   6  10   14
//  f30  f31  f32  f33         0    0    0    1         3   7  11   15
//
// tan = s/c
// ZYX
//
//   cycz  sxsycz-cxsz cxsycz+sxsz  0
//   cysz  sxsysz+cxcz cxsysz-sxcz  0
//   -sy       sxcy       cxcy      0
//    0          0          0       1


//-----------------------------------------------------------------------------
VOID  Euler::FromMatrix (const RMatrix &  matIn,
                         Euler            eulBase)
  {
  /// eulBase is the previous value from when the matrix was converted to Euler
  ///  angles.  This routine will attempt to decompose the matrix into a set
  ///  of angles that is close to eulBase.

  FLOAT  fX1, fY1, fZ1;
  FLOAT  fX2, fY2, fZ2;

  const RMAT_T *  fArray = matIn.fArray;

  FLOAT  fSY = FLOAT (-fArray[2]);
  fY1 = FLOAT (asin (fSY));
  fY2 = (fY1 >= 0.0) ? R_PI - fY1 : -R_PI - fY1;
  if (fabs (-fArray[2]) < 0.99999f)
    {
    // over 0.25 degrees from right angle
    FLOAT  fCY  = FLOAT (cos (fY1));
    FLOAT  fCY2 = FLOAT (cos (fY2));

    fX1 = FLOAT (atan2 (fArray[6] / fCY,  fArray[10] / fCY));
    fX2 = FLOAT (atan2 (fArray[6] / fCY2, fArray[10] / fCY2));

    fZ1 = FLOAT (atan2 (fArray[1] / fCY,  fArray[0] / fCY));
    fZ2 = FLOAT (atan2 (fArray[1] / fCY2, fArray[0] / fCY2));
    }
  else
    {
    // sin (y) approaching 1, cos (y) approaching 0
    //  therefore a0, a1, a6, and a10 are all approaching 0

    // Less than 0.25 degrees from right angle
    // gimbal lock occured
    fX1 = fX2 = eulBase.fX;
    // keep x rotation the same as base
    FLOAT fSX = FLOAT (sin (fX1));
    FLOAT fCX = FLOAT (cos (fX1));

    if (fabs (fCX) > fabs (fSX))
      {
      // safest to use cx for divides
      // cos (x) is the most useable variable.

      // a4 = SXSYcz-CXsz
      // a5 = SXSYsz+CXcz
      //
      // sz from a4, cz from a5
      // a4 = SXSYcz-CXsz
      // sz = cz(SXSY/CX) - A4/CX
      // cz = A5/CX - SXSYsz/CX
      //
      // sz = (A5/CX)(SXSY/CX) - sz(SXSY/CX)(SXSY/CX) - A4/CX
      // sz = (A5SXSY)/CX2  - sz (SX2SY2/CX2) - A4/CX
      // sz + sz (SX2SY2/CX2) = (A5SXSY)/CX2 - A4/CX
      // sz (1 + (SX2SY2/CX2)) = (A5SXSY)/CX2 - A4/CX
      // sz = ((A5SXSY)/CX2 - A4/CX) / (1 + (SX2SY2/CX2))

      FLOAT  fSZ = FLOAT (((fArray[5]*fSX*fSY)/(fCX*fCX) - fArray[4]/fCX) / (1.0f + (fSX*fSX*fSY*fSY)/(fCX*fCX)));
      fZ1 = fZ2 = FLOAT (asin (fSZ));
      }
    else
      {
      // safest to use sx for divides

      // a8 = CXSYcz+SXsz
      // a9 = CXSYsz-SXcz
      //
      // sz from a8, cz from a9
      //
      // sz = (a8-CXSYcz)/SX
      // cz = (CXSYsz-a9)/SX
      //
      // sz = a8/SX - cz(CXSY/SX)
      // cz = sz(CXSY/SX)-(a9/SX)
      //
      // sz = a8/SX - sz(CX2SY2/SX2)-(A9CXSY/SX2)
      //
      // sz + sz(CX2SY2/SX2) = a8/SX -(A9CXSY/SX2)
      // sz = (a8/SX -(A9CXSY/SX2)) / (1 + (CX2SY2/SX2))

      FLOAT  fSZ = FLOAT ((fArray[8]/fSX - (fArray[9]*fCX*fSY)/(fSX*fSX)) / (1.0f + (fCX*fCX*fSY*fSY)/(fSX*fSX)));
      fZ1 = fZ2 = FLOAT (asin (fSZ));
      };
    };

  FLOAT  d1X = ClampPi (fX1 - eulBase.fX);
  FLOAT  d1Y = ClampPi (fY1 - eulBase.fY);
  FLOAT  d1Z = ClampPi (fZ1 - eulBase.fZ);

  FLOAT  d2X = ClampPi (fX2 - eulBase.fX);
  FLOAT  d2Y = ClampPi (fY2 - eulBase.fY);
  FLOAT  d2Z = ClampPi (fZ2 - eulBase.fZ);

  if ((d1X * d1X + d1Y * d1Y + d1Z * d1Z) <
      (d2X * d2X + d2Y * d2Y + d2Z * d2Z))
    {
    this->fX = fX1 * R_RAD_TO_DEG;
    this->fY = fY1 * R_RAD_TO_DEG;
    this->fZ = fZ1 * R_RAD_TO_DEG;
    }
  else
    {
    this->fX = fX2 * R_RAD_TO_DEG;
    this->fY = fY2 * R_RAD_TO_DEG;
    this->fZ = fZ2 * R_RAD_TO_DEG;
    };
  };


//-----------------------------------------------------------------------------
FLOAT  Euler::ClampPi (FLOAT  fIn)
  {
  //  converts angles to -pi pi range
  while (fIn < -R_PI) {fIn += R_2PI;};
  while (fIn >  R_PI) {fIn -= R_2PI;};

  return (fIn);
  };


//-----------------------------------------------------------------------------
VOID  Euler::FromMatrix (const RMatrix &  matIn,
                         EOrder           eOrderIn,
                         Euler            eulBase)
  {
  INT   aiOrders [6][3] = { {0,1,2}, {0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0} };
  INT * aiAxisOrder = aiOrders [(INT)eOrderIn];

  const RMAT_T *  fArray = matIn.fArray;

  FLOAT  fSign = ((aiAxisOrder[1] == (aiAxisOrder[0] + 1)) || (aiAxisOrder[2] == (aiAxisOrder[1] + 1))) ? 1.0f : -1.0f;
  // Permute matrix to switched axes space
  RMatrix rs;
  for (int row = 0; row < 3; row++)
    {
    for (int col = 0; col < 3; col++)
      {
      rs.fArray [4 * row + col] = fArray [4 * aiAxisOrder[row] + aiAxisOrder[col]];
      }
    }
  rs.fArray [8] *= fSign;
  rs.fArray [9] *= fSign;
  rs.fArray [2] *= fSign;
  rs.fArray [6] *= fSign;

  // New base
  Euler  eulNewBase = eulBase;
  eulNewBase.fZ *= fSign;
  // Decompose to Euler angles
  FromMatrix (rs, eulNewBase);
  this->fZ *= fSign;
  };


//-----------------------------------------------------------------------------
/*
  RotateX

  f00=1.0f; f01= 0.0f; f02=  0.0f; f03=0.0f;
  f10=0.0f; f11=cx;    f12=-sx;    f13=0.0f;
  f20=0.0f; f21=sx;    f22= cx;    f23=0.0f;
  f30=0.0f; f31= 0.0f; f32=  0.0f; f33=1.0f;

  RotateY

  f00= cy;    f01=0.0f; f02=sy;    f03=0.0f;
  f10=  0.0f; f11=1.0f; f12= 0.0f; f13=0.0f;
  f20=-sy;    f21=0.0f; f22=cy;    f23=0.0f;
  f30=  0.0f; f31=0.0f; f32= 0.0f; f33=1.0f;

  RotateZ

  f00=cz;    f01=-sz;    f02=0.0f; f03=0.0f;
  f10=sz;    f11= cz;    f12=0.0f; f13=0.0f;
  f20= 0.0f; f21=  0.0f; f22=1.0f; f23=0.0f;
  f30= 0.0f; f31=  0.0f; f32=0.0f; f33=1.0f;


  RotateX

  1  0   0  0
  0 cx -sx  0
  0 sx  cx  0
  0  0   0  1

  RotateY

   cy  0  sy  0
    0  1   0  0
  -sy  0  cy  0
    0  0   0  1

  RotateZ

  cz -sz  0  0
  sz  cz  0  0
   0   0  1  0
   0   0  0  1

  YX

  cy   sxsy   cxsy   0
  0     cx     -sx   0
  -sy  sxcy   cxcy   0
  0     0      0     1

  ZYX

  cycz  sxsycz-cxsz cxsycz+sxsz  0
  cysz  sxsysz+cxcz cxsysz-sxcz  0
  -sy       sxcy       cxcy      0
   0          0          0       1
*/





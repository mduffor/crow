/* -----------------------------------------------------------------
                            3D Matrix Library

     This module contains mathematic functions for 3D graphics.

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
ASSERTFILE (__FILE__)

// Notes:
//  OpenGL is a right-handed coordinate system, with counter-clockwise polygon windings.
//  Maya is a right-handed coordiante system.
//  OBJ files use a right-handed coordinate system.
//  Real Time Rendering uses a right-handed coordinate system.
//  Computer Graphics: Principles and Practice uses a right-handed coordinate system, but views along the +z axis.

//  DirectX is a left-handed coordinate system, with counter-clockwise polygon windings.
//  Unity is a left-handed coordinate system.
//  Lightwave is a left-handed coordinate system.
//  Blender is a left-handed coordinate system with z-up and y-depth


#include "Math/RMatrix.hpp"

RMatrix  RMatrix::kIdentity (1.0f,0.0f,0.0f,0.0f,
                             0.0f,1.0f,0.0f,0.0f,
                             0.0f,0.0f,1.0f,0.0f,
                             0.0f,0.0f,0.0f,1.0f);


//-----------------------------------------------------------------------------
//  RMatrix
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
RMatrix RMatrix::operator*  (const RMatrix&    m4In) const
  {

  return RMatrix (fArray[0]*m4In.fArray[0] + fArray[4]*m4In.fArray[1] + fArray[ 8]*m4In.fArray[2]  + fArray[12]*m4In.fArray[3],
                  fArray[0]*m4In.fArray[4] + fArray[4]*m4In.fArray[5] + fArray[ 8]*m4In.fArray[6]  + fArray[12]*m4In.fArray[7],
                  fArray[0]*m4In.fArray[8] + fArray[4]*m4In.fArray[9] + fArray[ 8]*m4In.fArray[10] + fArray[12]*m4In.fArray[11],
                  fArray[0]*m4In.fArray[12] + fArray[4]*m4In.fArray[13] + fArray[ 8]*m4In.fArray[14] + fArray[12]*m4In.fArray[15],

                  fArray[1]*m4In.fArray[0] + fArray[5]*m4In.fArray[1] + fArray[ 9]*m4In.fArray[2]  + fArray[13]*m4In.fArray[3],
                  fArray[1]*m4In.fArray[4] + fArray[5]*m4In.fArray[5] + fArray[ 9]*m4In.fArray[6]  + fArray[13]*m4In.fArray[7],
                  fArray[1]*m4In.fArray[8] + fArray[5]*m4In.fArray[9] + fArray[ 9]*m4In.fArray[10] + fArray[13]*m4In.fArray[11],
                  fArray[1]*m4In.fArray[12] + fArray[5]*m4In.fArray[13] + fArray[ 9]*m4In.fArray[14] + fArray[13]*m4In.fArray[15],

                  fArray[2]*m4In.fArray[0] + fArray[6]*m4In.fArray[1] + fArray[10]*m4In.fArray[2]  + fArray[14]*m4In.fArray[3],
                  fArray[2]*m4In.fArray[4] + fArray[6]*m4In.fArray[5] + fArray[10]*m4In.fArray[6]  + fArray[14]*m4In.fArray[7],
                  fArray[2]*m4In.fArray[8] + fArray[6]*m4In.fArray[9] + fArray[10]*m4In.fArray[10] + fArray[14]*m4In.fArray[11],
                  fArray[2]*m4In.fArray[12] + fArray[6]*m4In.fArray[13] + fArray[10]*m4In.fArray[14] + fArray[14]*m4In.fArray[15],

                  fArray[3]*m4In.fArray[0] + fArray[7]*m4In.fArray[1] + fArray[11]*m4In.fArray[2]  + fArray[15]*m4In.fArray[3],
                  fArray[3]*m4In.fArray[4] + fArray[7]*m4In.fArray[5] + fArray[11]*m4In.fArray[6]  + fArray[15]*m4In.fArray[7],
                  fArray[3]*m4In.fArray[8] + fArray[7]*m4In.fArray[9] + fArray[11]*m4In.fArray[10] + fArray[15]*m4In.fArray[11],
                  fArray[3]*m4In.fArray[12] + fArray[7]*m4In.fArray[13] + fArray[11]*m4In.fArray[14] + fArray[15]*m4In.fArray[15]);
  };


//-----------------------------------------------------------------------------
RMatrix& RMatrix::operator*=  (const RMatrix&    m4In)
  {

  Set (fArray[0]*m4In.fArray[0] + fArray[4]*m4In.fArray[1] + fArray[ 8]*m4In.fArray[2]  + fArray[12]*m4In.fArray[3],
       fArray[0]*m4In.fArray[4] + fArray[4]*m4In.fArray[5] + fArray[ 8]*m4In.fArray[6]  + fArray[12]*m4In.fArray[7],
       fArray[0]*m4In.fArray[8] + fArray[4]*m4In.fArray[9] + fArray[ 8]*m4In.fArray[10] + fArray[12]*m4In.fArray[11],
       fArray[0]*m4In.fArray[12] + fArray[4]*m4In.fArray[13] + fArray[ 8]*m4In.fArray[14] + fArray[12]*m4In.fArray[15],

       fArray[1]*m4In.fArray[0] + fArray[5]*m4In.fArray[1] + fArray[ 9]*m4In.fArray[2]  + fArray[13]*m4In.fArray[3],
       fArray[1]*m4In.fArray[4] + fArray[5]*m4In.fArray[5] + fArray[ 9]*m4In.fArray[6]  + fArray[13]*m4In.fArray[7],
       fArray[1]*m4In.fArray[8] + fArray[5]*m4In.fArray[9] + fArray[ 9]*m4In.fArray[10] + fArray[13]*m4In.fArray[11],
       fArray[1]*m4In.fArray[12] + fArray[5]*m4In.fArray[13] + fArray[ 9]*m4In.fArray[14] + fArray[13]*m4In.fArray[15],

       fArray[2]*m4In.fArray[0] + fArray[6]*m4In.fArray[1] + fArray[10]*m4In.fArray[2]  + fArray[14]*m4In.fArray[3],
       fArray[2]*m4In.fArray[4] + fArray[6]*m4In.fArray[5] + fArray[10]*m4In.fArray[6]  + fArray[14]*m4In.fArray[7],
       fArray[2]*m4In.fArray[8] + fArray[6]*m4In.fArray[9] + fArray[10]*m4In.fArray[10] + fArray[14]*m4In.fArray[11],
       fArray[2]*m4In.fArray[12] + fArray[6]*m4In.fArray[13] + fArray[10]*m4In.fArray[14] + fArray[14]*m4In.fArray[15],

       fArray[3]*m4In.fArray[0] + fArray[7]*m4In.fArray[1] + fArray[11]*m4In.fArray[2]  + fArray[15]*m4In.fArray[3],
       fArray[3]*m4In.fArray[4] + fArray[7]*m4In.fArray[5] + fArray[11]*m4In.fArray[6]  + fArray[15]*m4In.fArray[7],
       fArray[3]*m4In.fArray[8] + fArray[7]*m4In.fArray[9] + fArray[11]*m4In.fArray[10] + fArray[15]*m4In.fArray[11],
       fArray[3]*m4In.fArray[12] + fArray[7]*m4In.fArray[13] + fArray[11]*m4In.fArray[14] + fArray[15]*m4In.fArray[15]);
  return *this;
  };


//-----------------------------------------------------------------------------
BOOL  RMatrix::operator==  (const RMatrix&    matIn)
  {
  return ((f00==matIn.f00) && (f01==matIn.f01) && (f02==matIn.f02) && (f03==matIn.f03) &&
          (f10==matIn.f10) && (f11==matIn.f11) && (f12==matIn.f12) && (f13==matIn.f13) &&
          (f20==matIn.f20) && (f21==matIn.f21) && (f22==matIn.f22) && (f23==matIn.f23) &&
          (f30==matIn.f30) && (f31==matIn.f31) && (f32==matIn.f32) && (f33==matIn.f33));
  };


//-----------------------------------------------------------------------------
VOID RMatrix::SetRotateX  (RMAT_T  fAngleIn)
  {
  RMAT_T            sn;
  RMAT_T            cs;

  // right handed coordinate system
  // Rot fAngleIn around X axis

  sn = RMAT_T (sinf (fAngleIn));
  cs = RMAT_T (cosf (fAngleIn));


  f00=1.0f; f01= 0.0f; f02=  0.0f; f03=0.0f;
  f10=0.0f; f11=cs;    f12=-sn;    f13=0.0f;
  f20=0.0f; f21=sn;    f22= cs;    f23=0.0f;
  f30=0.0f; f31= 0.0f; f32=  0.0f; f33=1.0f;
  };


//-----------------------------------------------------------------------------
VOID RMatrix::SetRotateY  (RMAT_T  fAngleIn)
  {
  RMAT_T            sn;
  RMAT_T            cs;

  // a = Rot fAngle around Y

  sn = RMAT_T (sinf (fAngleIn));
  cs = RMAT_T (cosf (fAngleIn));

  f00= cs;    f01=0.0f; f02=sn;    f03=0.0f;
  f10=  0.0f; f11=1.0f; f12= 0.0f; f13=0.0f;
  f20=-sn;    f21=0.0f; f22=cs;    f23=0.0f;
  f30=  0.0f; f31=0.0f; f32= 0.0f; f33=1.0f;
  };


//-----------------------------------------------------------------------------
VOID RMatrix::SetRotateZ  (RMAT_T  fAngleIn)
  {
  RMAT_T            sn;
  RMAT_T            cs;

  // right handed coordinate system
  // a = Rot fAngle around Z

  sn = RMAT_T (sinf (fAngleIn));
  cs = RMAT_T (cosf (fAngleIn));

  f00=cs;    f01=-sn;    f02=0.0f; f03=0.0f;
  f10=sn;    f11= cs;    f12=0.0f; f13=0.0f;
  f20= 0.0f; f21=  0.0f; f22=1.0f; f23=0.0f;
  f30= 0.0f; f31=  0.0f; f32=0.0f; f33=1.0f;
  };


//-----------------------------------------------------------------------------
VOID RMatrix::RigidInvert  (VOID)
  {

  // this routine assumes the original matrix M is calcualted as M = TR
  //  with the vector being post-multiplied against M.

  // calculate the right column
  SetTrans ((f00*(-f03)) + (f10*(-f13)) + (f20*(-f23)),
            (f01*(-f03)) + (f11*(-f13)) + (f21*(-f23)),
            (f02*(-f03)) + (f12*(-f13)) + (f22*(-f23)));

  // transpose the 3x3 rotation matrix
  RotTranspose ();

  // set the bottom row
  f30= 0.0f; f31=  0.0f; f32=0.0f; f33=1.0f;
  };


//-----------------------------------------------------------------------------
RMAT_T  RMatrix::Determinant (VOID)
  {
  // Note:  Code adapted from  http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm

  return (f03 * f12 * f21 * f30-f02 * f13 * f21 * f30 - f03 * f11 * f22 * f30 + f01 * f13 * f22 * f30 +
          f02 * f11 * f23 * f30-f01 * f12 * f23 * f30 - f03 * f12 * f20 * f31 + f02 * f13 * f20 * f31 +
          f03 * f10 * f22 * f31-f00 * f13 * f22 * f31 - f02 * f10 * f23 * f31 + f00 * f12 * f23 * f31 +
          f03 * f11 * f20 * f32-f01 * f13 * f20 * f32 - f03 * f10 * f21 * f32 + f00 * f13 * f21 * f32 +
          f01 * f10 * f23 * f32-f00 * f11 * f23 * f32 - f02 * f11 * f20 * f33 + f01 * f12 * f20 * f33 +
          f02 * f10 * f21 * f33-f00 * f12 * f21 * f33 - f01 * f10 * f22 * f33 + f00 * f11 * f22 * f33);
  };


//-----------------------------------------------------------------------------
VOID  RMatrix::Invert      (VOID)
  {
  // Note:  Code adapted from  http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm

  RMAT_T  fDetInv = 1.0 / this->Determinant();

  // move the current values into temporary storage for calculations so that we don't overwrite them as we go.
  RMatrix  m;

  m.f00 = (f12*f23*f31 - f13*f22*f31 + f13*f21*f32 - f11*f23*f32 - f12*f21*f33 + f11*f22*f33) * fDetInv;
  m.f01 = (f03*f22*f31 - f02*f23*f31 - f03*f21*f32 + f01*f23*f32 + f02*f21*f33 - f01*f22*f33) * fDetInv;
  m.f02 = (f02*f13*f31 - f03*f12*f31 + f03*f11*f32 - f01*f13*f32 - f02*f11*f33 + f01*f12*f33) * fDetInv;
  m.f03 = (f03*f12*f21 - f02*f13*f21 - f03*f11*f22 + f01*f13*f22 + f02*f11*f23 - f01*f12*f23) * fDetInv;
  m.f10 = (f13*f22*f30 - f12*f23*f30 - f13*f20*f32 + f10*f23*f32 + f12*f20*f33 - f10*f22*f33) * fDetInv;
  m.f11 = (f02*f23*f30 - f03*f22*f30 + f03*f20*f32 - f00*f23*f32 - f02*f20*f33 + f00*f22*f33) * fDetInv;
  m.f12 = (f03*f12*f30 - f02*f13*f30 - f03*f10*f32 + f00*f13*f32 + f02*f10*f33 - f00*f12*f33) * fDetInv;
  m.f13 = (f02*f13*f20 - f03*f12*f20 + f03*f10*f22 - f00*f13*f22 - f02*f10*f23 + f00*f12*f23) * fDetInv;
  m.f20 = (f11*f23*f30 - f13*f21*f30 + f13*f20*f31 - f10*f23*f31 - f11*f20*f33 + f10*f21*f33) * fDetInv;
  m.f21 = (f03*f21*f30 - f01*f23*f30 - f03*f20*f31 + f00*f23*f31 + f01*f20*f33 - f00*f21*f33) * fDetInv;
  m.f22 = (f01*f13*f30 - f03*f11*f30 + f03*f10*f31 - f00*f13*f31 - f01*f10*f33 + f00*f11*f33) * fDetInv;
  m.f23 = (f03*f11*f20 - f01*f13*f20 - f03*f10*f21 + f00*f13*f21 + f01*f10*f23 - f00*f11*f23) * fDetInv;
  m.f30 = (f12*f21*f30 - f11*f22*f30 - f12*f20*f31 + f10*f22*f31 + f11*f20*f32 - f10*f21*f32) * fDetInv;
  m.f31 = (f01*f22*f30 - f02*f21*f30 + f02*f20*f31 - f00*f22*f31 - f01*f20*f32 + f00*f21*f32) * fDetInv;
  m.f32 = (f02*f11*f30 - f01*f12*f30 - f02*f10*f31 + f00*f12*f31 + f01*f10*f32 - f00*f11*f32) * fDetInv;
  m.f33 = (f01*f12*f20 - f02*f11*f20 + f02*f10*f21 - f00*f12*f21 - f01*f10*f22 + f00*f11*f22) * fDetInv;

  *this = m;
  };

//-----------------------------------------------------------------------------
RVec4 RMatrix::ToQuat (VOID) const
  {
  // Quaternon code is based off of the Feb 1998 Article in Game Developer Magazine.

  RVec4       vecQuat;
  RVEC_T      tr, s, q[4];


  tr = RMAT_T (f00 + f11 + f22);

  // check the diagonal
  if (tr > 0.0f)
    {
    s           = RVEC_SQRT (tr + 1.0f);
    vecQuat.fW  = RVEC_T (s / 2.0f);
    s           = 0.5f / s;
    vecQuat.fX  = RVEC_T ((f12 - f21) * s);
    vecQuat.fY  = RVEC_T ((f20 - f02) * s);
    vecQuat.fZ  = RVEC_T ((f01 - f10) * s);
    }
  else
    {
    int         i, j, k;
    int         nxt [3] = {1,2,0};

    // diagonal is negative
    i = 0;
    if (f11 > f00)     {i = 1;};
    if (f22 > fArray[i+i*4])  {i = 2;};
    j=nxt[i];
    k=nxt[j];

    s = RVEC_T (RVEC_SQRT ((fArray[i+(i*4)] - (fArray[j+(j*4)] + fArray[k+(k*4)])) + 1.0f));
    q [i] = s * 0.5f;
    if (s != 0.0f)
      {
      s = 0.5f / s;
      };

    q[3] = RVEC_T ((fArray[j+(k*4)] - fArray[k+(j*4)]) * s);
    q[j] = RVEC_T ((fArray[i+(j*4)] + fArray[j+(i*4)]) * s);
    q[k] = RVEC_T ((fArray[i+(k*4)] + fArray[k+(i*4)]) * s);

    vecQuat.fX = q[0];
    vecQuat.fY = q[1];
    vecQuat.fZ = q[2];
    vecQuat.fW = q[3];
    };
  return (vecQuat);
  };


//-----------------------------------------------------------------------------
VOID RMatrix::FromQuat (RVec4  vecQuatIn)
  {
  // Quaternon code is based off of the Feb 1998 Article in Game Developer Magazine.
  RVEC_T      wx,wy,wz,xx,yy,yz,xy,xz,zz,x2,y2,z2;

  //QuatNormalize (v4Quat);


  // calculate coefficients
  x2 = vecQuatIn.fX + vecQuatIn.fX;
  y2 = vecQuatIn.fY + vecQuatIn.fY;
  z2 = vecQuatIn.fZ + vecQuatIn.fZ;

  xx = vecQuatIn.fX * x2;  xy = vecQuatIn.fX * y2; xz = vecQuatIn.fX * z2;
  yy = vecQuatIn.fY * y2;  yz = vecQuatIn.fY * z2; zz = vecQuatIn.fZ * z2;
  wx = vecQuatIn.fW * x2;  wy = vecQuatIn.fW * y2; wz = vecQuatIn.fW * z2;

  f00 = 1.0f - (yy + zz);   f10 = xy - wz;
  f20 = xz + wy;            f30 = 0.0;

  f01 = xy + wz;            f11 = 1.0f - (xx + zz);
  f21 = yz - wx;            f31 = 0.0;

  f02 = xz - wy;            f12 = yz + wx;
  f22 = 1.0f - (xx + yy);   f32 = 0.0;

  f03 = 0.0f;              f13 = 0.0f;
  f23 = 0.0f;              f33 = 1.0f;
  };


//-----------------------------------------------------------------------------
VOID  RMatrix::DebugPrint  (const char *  pszMatrixNameIn) const
  {
  RStr  strDbgOut;
  strDbgOut.AppendFormat ("Matrix: %s\n", pszMatrixNameIn);
  strDbgOut.AppendFormat ("%10f %10f %10f %10f\n", (FLOAT)r00,  (FLOAT)r01,  (FLOAT)r02,  (FLOAT)tx);
  strDbgOut.AppendFormat ("%10f %10f %10f %10f\n", (FLOAT)r10,  (FLOAT)r11,  (FLOAT)r12,  (FLOAT)ty);
  strDbgOut.AppendFormat ("%10f %10f %10f %10f\n", (FLOAT)r20,  (FLOAT)r21,  (FLOAT)r22,  (FLOAT)tz);
  strDbgOut.AppendFormat ("%10f %10f %10f %10f\n", (FLOAT)f30,  (FLOAT)f31,  (FLOAT)f32,  (FLOAT)f33);
  DBG_INFO (strDbgOut.AsChar ());
  };





//-----------------------------------------------------------------------------
VOID  RMatrix::GetFloatArray (FLOAT *  pafIn)
  {
  // copy the values from the matrix into an array of floats.  This is needed
  //  for APIs which must have float pointers.

  // unrolling loop for speed.
  pafIn[ 0] = (FLOAT) fArray [ 0];
  pafIn[ 1] = (FLOAT) fArray [ 1];
  pafIn[ 2] = (FLOAT) fArray [ 2];
  pafIn[ 3] = (FLOAT) fArray [ 3];
  pafIn[ 4] = (FLOAT) fArray [ 4];
  pafIn[ 5] = (FLOAT) fArray [ 5];
  pafIn[ 6] = (FLOAT) fArray [ 6];
  pafIn[ 7] = (FLOAT) fArray [ 7];
  pafIn[ 8] = (FLOAT) fArray [ 8];
  pafIn[ 9] = (FLOAT) fArray [ 9];
  pafIn[10] = (FLOAT) fArray [10];
  pafIn[11] = (FLOAT) fArray [11];
  pafIn[12] = (FLOAT) fArray [12];
  pafIn[13] = (FLOAT) fArray [13];
  pafIn[14] = (FLOAT) fArray [14];
  pafIn[15] = (FLOAT) fArray [15];
  };

//-----------------------------------------------------------------------------
VOID  RMatrix::GetFloatArrayTransposed (FLOAT *  pafIn)
  {
  // copy the values from the matrix into an array of floats.  This is needed
  //  for APIs which must have float pointers.

  // 0   1  2  3
  // 4   5  6  7
  // 8   9 10 11
  // 12 13 14 15

  // unrolling loop for speed.
  pafIn[ 0] = (FLOAT) fArray [ 0];
  pafIn[ 1] = (FLOAT) fArray [ 4];
  pafIn[ 2] = (FLOAT) fArray [ 8];
  pafIn[ 3] = (FLOAT) fArray [12];
  pafIn[ 4] = (FLOAT) fArray [ 1];
  pafIn[ 5] = (FLOAT) fArray [ 5];
  pafIn[ 6] = (FLOAT) fArray [ 9];
  pafIn[ 7] = (FLOAT) fArray [13];
  pafIn[ 8] = (FLOAT) fArray [ 2];
  pafIn[ 9] = (FLOAT) fArray [ 6];
  pafIn[10] = (FLOAT) fArray [10];
  pafIn[11] = (FLOAT) fArray [14];
  pafIn[12] = (FLOAT) fArray [ 3];
  pafIn[13] = (FLOAT) fArray [ 7];
  pafIn[14] = (FLOAT) fArray [11];
  pafIn[15] = (FLOAT) fArray [15];
  };

//-----------------------------------------------------------------------------
// functions for manipulating matrixes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
VOID MatrixPerspective  (RMatrix&       matMatrixOut,
                         RMAT_T  fFOVYIn,
                         RMAT_T  fAspectIn,
                         RMAT_T  fNearIn,
                         RMAT_T  fFarIn)
  {

  RMAT_T  fHalfVert  = RMAT_T (tanf (fFOVYIn * 0.5f) * fNearIn);
  RMAT_T  fHalfHoriz = fHalfVert * fAspectIn;

  DBG_INFO ("MatrixPerspective aspect %f halfVert %f halfHoriz %f near %f far %f", fAspectIn, fHalfVert, fHalfHoriz, fNearIn, fFarIn);

  MatrixPerspective (matMatrixOut,
                     -fHalfHoriz, fHalfHoriz,
                     fHalfVert, -fHalfVert,
                     fNearIn, fFarIn);

  /*
  RMAT_T  fYDelta;

  // create a left-handed projection matrix

  fYDelta =  (1.0f / (tanf (fFOVYIn / 2.0f)));

  matMatrixOut.f00 = fYDelta / fAspectIn;
  matMatrixOut.f10 = 0.0f;
  matMatrixOut.f20 = 0.0f;
  matMatrixOut.f30 = 0.0f;

  matMatrixOut.f01 = 0.0f;
  matMatrixOut.f11 = fYDelta;
  matMatrixOut.f21 = 0.0f;
  matMatrixOut.f31 = 0.0f;

  matMatrixOut.f02 = 0.0f;
  matMatrixOut.f12 = 0.0f;
  matMatrixOut.f22 = (fFarIn + fNearIn) / (fNearIn - fFarIn);
  matMatrixOut.f32 = -1.0f;

  matMatrixOut.f03 = 0.0f;
  matMatrixOut.f13 = 0.0f;
  matMatrixOut.f23 = (2.0f * fFarIn * fNearIn) / (fNearIn - fFarIn);
  matMatrixOut.f33 = 0.0f;

  DBG_INFO ("MatrixPerspective Done %f", matMatrixOut.f33);
  */
  };

//-----------------------------------------------------------------------------
VOID MatrixPerspective  (RMatrix&       matMatrixOut,
                         RMAT_T  fLeft,
                         RMAT_T  fRight,
                         RMAT_T  fTop,
                         RMAT_T  fBottom,
                         RMAT_T  fNear, /// positive value
                         RMAT_T  fFar) /// positive value
  {
  // Create a right-handed matrix, based off of the code in Real-Time Rendering p63 (3.78)
  //   This creates a matrix taking positive near and far values, looking down the negative z
  //   axis.

  matMatrixOut.f00 = (2.0f * fNear) / (fRight - fLeft);
  matMatrixOut.f10 = 0.0f;
  matMatrixOut.f20 = 0.0f;
  matMatrixOut.f30 = 0.0f;

  matMatrixOut.f01 = 0.0f;
  matMatrixOut.f11 = (2.0f * fNear) / (fTop - fBottom);
  matMatrixOut.f21 = 0.0f;
  matMatrixOut.f31 = 0.0f;

  matMatrixOut.f02 = (fRight + fLeft) / (fRight - fLeft);
  matMatrixOut.f12 = (fTop + fBottom) / (fTop - fBottom);
  matMatrixOut.f22 = -(fFar + fNear) / (fFar - fNear);
  matMatrixOut.f32 = -1.0f;

  matMatrixOut.f03 = 0.0f; // tx
  matMatrixOut.f13 = 0.0f; // ty
  matMatrixOut.f23 = -(2.0f * fFar * fNear) / (fFar - fNear); // tz
  matMatrixOut.f33 = 0.0f;
  };




//-----------------------------------------------------------------------------
VOID MatrixOrtho  (RMatrix&             matOut,
                   RMAT_T        fLeft,
                   RMAT_T        fRight,
                   RMAT_T        fBottom,
                   RMAT_T        fTop,
                   RMAT_T        fNear,
                   RMAT_T        fFar)
  {
  // This routine mirrors the glOrtho formula.
  //  The resultant matrix maps the camera-space cube of (fLeft, fBottom, fNear), (fRigth, fTop, fFar)
  //  into the unit cube of the Canonical View Volume (-1,-1,-1) (1,1,1).

  RMAT_T  fTx = -(fRight + fLeft) / (fRight - fLeft);
  RMAT_T  fTy = -(fTop + fBottom) / (fTop - fBottom);
  RMAT_T  fTz = -(fFar + fNear) / (fFar - fNear);

  matOut.f00 = 2.0/(fRight-fLeft); matOut.f01 = 0.0;                matOut.f02 = 0.0;              matOut.f03 = fTx;
  matOut.f10 = 0.0;                matOut.f11 = 2.0/(fTop-fBottom); matOut.f12 = 0.0;              matOut.f13 = fTy;
  matOut.f20 = 0.0;                matOut.f21 = 0.0;                matOut.f22 = 2.0/(fFar-fNear); matOut.f23 = fTz;
  matOut.f30 = 0.0;                matOut.f31 = 0.0;                matOut.f32 = 0.0;              matOut.f33 = 1.0;
  };


//-----------------------------------------------------------------------------
VOID MatrixLookAt  (RMatrix&        matMatrixOut,
                    const RVec3&    v3Position,        // point
                    const RVec3&    v3Target,          // point
                    const RVec3&    v3Up)              // vector
  {
  RVec3             v3Look;
  RVec3             v3U;
  RVec3             v3V;

  // creates a matrix where the z axis points down the look vector, the
  //  X axis is perpendicular to the look and up vectors, and the Y points
  //  towards the up vector direction as much as it can.

  matMatrixOut.Identity ();

  v3Look = v3Target - v3Position;
  v3Look.Normalize ();



//  v3U = v3Look % v3Up;
  v3U = v3Up % v3Look;

  // handle case where look and up are parallel... return identity matrix.
  if (v3U.LengthSquared () < (FLT_EPSILON * 16)) return;

  v3U.Normalize ();

  v3V = v3Look % v3U;
/*
printf ("vPos %g %g %g  ", v3Position.fX, v3Position.fY, v3Position.fZ);
printf ("vTgt %g %g %g  ", v3Target.fX, v3Target.fY, v3Target.fZ);
printf ("vLook %g %g %g  ", v3Look.fX, v3Look.fY, v3Look.fZ);
printf ("vUp %g %g %g  ", v3Up.fX, v3Up.fY, v3Up.fZ);
printf ("vU %g %g %g  ", v3U.fX, v3U.fY, v3U.fZ);
printf ("vV %g %g %g  \n", v3V.fX, v3V.fY, v3V.fZ);
*/
  matMatrixOut.SetRotate (v3U, v3V, v3Look);
  };


//-----------------------------------------------------------------------------
VOID MatrixChangeCoordSystem  (RMatrix&         matMatrixOut,
                               const RVec3&     v3AxisXIn,
                               const RVec3&     v3AxisYIn,
                               const RVec3&     v3AxisZIn,
                               const RVec3&     v3PointIn)
  {

  // The purpose of this routine is to make building a R^-1 * -T matrix quickly.
  //  This is needed when you need to convert a point from one coordinate
  //  system to another

  matMatrixOut.f00=v3AxisXIn.fX; matMatrixOut.f01=v3AxisYIn.fX; matMatrixOut.f02=v3AxisZIn.fX;
  matMatrixOut.f10=v3AxisXIn.fY; matMatrixOut.f11=v3AxisYIn.fY; matMatrixOut.f12=v3AxisZIn.fY;
  matMatrixOut.f20=v3AxisXIn.fZ; matMatrixOut.f21=v3AxisYIn.fZ; matMatrixOut.f22=v3AxisZIn.fZ;
  matMatrixOut.f30=0.0f;         matMatrixOut.f31=0.0f;         matMatrixOut.f32=0.0f;          matMatrixOut.f33=1.0f;

  matMatrixOut.f03 = - (v3AxisXIn.fX * v3PointIn.fX) - (v3AxisYIn.fX * v3PointIn.fY) - (v3AxisZIn.fX * v3PointIn.fZ);
  matMatrixOut.f13 = - (v3AxisXIn.fY * v3PointIn.fX) - (v3AxisYIn.fY * v3PointIn.fY) - (v3AxisZIn.fY * v3PointIn.fZ);
  matMatrixOut.f23 = - (v3AxisXIn.fZ * v3PointIn.fX) - (v3AxisYIn.fZ * v3PointIn.fY) - (v3AxisZIn.fZ * v3PointIn.fZ);
  };


//-----------------------------------------------------------------------------
VOID MatrixPerspectiveOGLtoD3D  (RMatrix&      matProjection)
  {

  // this routine converts an OpenGL style projection matrix to a
  //   Direct 3D style projection matrix.

  matProjection.f22 =  - 0.5f * (matProjection.f22 + matProjection.f32);
  matProjection.f32 =  - matProjection.f32;
  matProjection.f23 =    0.5f * matProjection.f23;
  };


//-----------------------------------------------------------------------------
VOID    MatrixAxisAngle            (RMatrix&      matMatrixOut,
                                    const RVec3&  v3AxisIn,
                                    RMAT_T dAngleIn)  // in radians
  {

  RMAT_T    fSin = sin (dAngleIn);
  RMAT_T    fCos = cos (dAngleIn);
  RMAT_T    fT   = 1.0 - fCos;

  matMatrixOut.Set (fT * v3AxisIn.fX * v3AxisIn.fX + fCos,
                    fT * v3AxisIn.fX * v3AxisIn.fY - fSin * v3AxisIn.fZ,
                    fT * v3AxisIn.fX * v3AxisIn.fZ + fSin * v3AxisIn.fY,
                    0.0,

                    fT * v3AxisIn.fX * v3AxisIn.fY + fSin * v3AxisIn.fZ,
                    fT * v3AxisIn.fY * v3AxisIn.fY + fCos,
                    fT * v3AxisIn.fY * v3AxisIn.fZ - fSin * v3AxisIn.fX,
                    0.0,

                    fT * v3AxisIn.fX * v3AxisIn.fZ - fSin * v3AxisIn.fY,
                    fT * v3AxisIn.fY * v3AxisIn.fZ + fSin * v3AxisIn.fX,
                    fT * v3AxisIn.fZ * v3AxisIn.fZ + fCos,
                    0.0,

                    0.0, 0.0, 0.0, 1.0);

  /*
  RMAT_T    fSin = sin (dAngleIn);
  RMAT_T    fCos = cos (dAngleIn);
  RMAT_T    fT   = 1.0 - fCos;
  RMAT_T    fAx = v3AxisIn.fX;
  RMAT_T    fAy = v3AxisIn.fY;
  RMAT_T    fAz = v3AxisIn.fZ;
  RMAT_T
  RMAT_T


  matMatrixOut.Set (fCos + fT * fAx * fAx,
                    fT * fAx * fAy - fSin * fAz,
                    fT * fAx * fAz + fSin * fAy,
                    0.0f,

                    fT * fAx * fAy + fSin * fAz,
                    fCos + fT * fAy * fAy,


  */


  };

//-----------------------------------------------------------------------------
RMAT_T  MatrixDistSquared (const RMatrix&      matMatrixA,
                           const RMatrix&      matMatrixB)
  {
  FLOAT  fDistX = matMatrixA.tx - matMatrixB.tx;
  FLOAT  fDistY = matMatrixA.ty - matMatrixB.ty;
  FLOAT  fDistZ = matMatrixA.tz - matMatrixB.tz;

  return (fDistX * fDistX) + (fDistY * fDistY) + (fDistZ * fDistZ);
  };

//-----------------------------------------------------------------------------
RMAT_T  MatrixZDistSquared (const RMatrix&      matMatrixA,
                            const RMatrix&      matMatrixB)
  {
  FLOAT  fDistZ = matMatrixA.tz - matMatrixB.tz;

  return (fDistZ * fDistZ);
  };



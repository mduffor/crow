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


#ifndef RMATRIX_HPP
#define RMATRIX_HPP

#include "Sys/Types.hpp"
#include "Math/RVec.hpp"
#include <math.h>
#include <float.h>

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

// NOTE: R_EPSILON is now defined centrally in Sys/Types.hpp
//#ifndef R_EPSILON
//  #define R_EPSILON             0.0000000001f
//#endif

#ifndef RMAT_T
  #define RMAT_T    double
#endif

#ifndef RMAT_SQRT // define as sqrt for double, sqrtf for float
  #define RMAT_SQRT    sqrt
#endif


//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------

class   RMatrix;
typedef RMatrix *  PRMatrix;

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


// Note:  The RMatrix class is set up where the matrix is represented as
//
//  f00  f01  f02  f03        r00  r01  r02  tx         0   4   8   12
//  f10  f11  f12  f13   or   r10  r11  r12  ty   or    1   5   9   13
//  f20  f21  f22  f23        r20  r21  r22  tz         2   6  10   14
//  f30  f31  f32  f33         0    0    0    1         3   7  11   15
//
// and vector operations are performed with a column vector multiplied on
//  the right side of matrix.  column major, same as OpenGL.  The
//  two digit number following the "f" or "r" above, denotes the index
//  order as if you were accessing a two dimensional array in C/C++.
//
// All matrix multiplication through overloaded operators assumes
//  that the bottom row is 0 0 0 1.  All matrix operations ignore the
//  bottom row (row 3).  Row 3 is initialized whenever the full 4x4
//  array is requested, such as when the array would be passed to an
//  API.
//
// Any operations that require row 3 (perspective tranformations, etc)
//  should use the CMat4fFull class.


//------------------------------------------------------------------------
class RMatrix
//------------------------------------------------------------------------
  {
  public:
    union
      {
      RMAT_T   fArray [16];
      struct {RMAT_T   f00,f10,f20,f30,  f01,f11,f21,f31,  f02,f12,f22,f32,  f03,f13,f23,f33;};
      struct {RMAT_T   r00,r10,r20,_30,  r01,r11,r21,_31,  r02,r12,r22,_32,  tx,ty,tz,_33;};
      };

  public:
    static      RMatrix   kIdentity;

                RMatrix  ()                                                    {};

                RMatrix  (const RMatrix &  matIn)                              {f00=matIn.f00; f01=matIn.f01; f02=matIn.f02; f03=matIn.f03;
                                                                                f10=matIn.f10; f11=matIn.f11; f12=matIn.f12; f13=matIn.f13;
                                                                                f20=matIn.f20; f21=matIn.f21; f22=matIn.f22; f23=matIn.f23;
                                                                                f30=matIn.f30; f31=matIn.f31; f32=matIn.f32; f33=matIn.f33;};

                RMatrix  (RMAT_T _f00, RMAT_T _f01, RMAT_T _f02, RMAT_T _f03,
                          RMAT_T _f10, RMAT_T _f11, RMAT_T _f12, RMAT_T _f13,
                          RMAT_T _f20, RMAT_T _f21, RMAT_T _f22, RMAT_T _f23,
                          RMAT_T _f30, RMAT_T _f31, RMAT_T _f32, RMAT_T _f33)

                                                                               {f00=_f00;f10=_f10;f20=_f20;f30=_f30;
                                                                                f01=_f01;f11=_f11;f21=_f21;f31=_f31;
                                                                                f02=_f02;f12=_f12;f22=_f22;f32=_f32;
                                                                                f03=_f03;f13=_f13;f23=_f23;f33=_f33;};

                ~RMatrix ()                                                    {};

    VOID        Set         (RMAT_T _f00, RMAT_T _f01, RMAT_T _f02, RMAT_T _f03,
                             RMAT_T _f10, RMAT_T _f11, RMAT_T _f12, RMAT_T _f13,
                             RMAT_T _f20, RMAT_T _f21, RMAT_T _f22, RMAT_T _f23,
                             RMAT_T _f30, RMAT_T _f31, RMAT_T _f32, RMAT_T _f33)

                                                                               {f00=_f00;f10=_f10;f20=_f20;f30=_f30;
                                                                                f01=_f01;f11=_f11;f21=_f21;f31=_f31;
                                                                                f02=_f02;f12=_f12;f22=_f22;f32=_f32;
                                                                                f03=_f03;f13=_f13;f23=_f23;f33=_f33;};

    VOID        Set         (RMAT_T *  afArrayIn)                              {f00=afArrayIn[ 0]; f01=afArrayIn[ 4]; f02=afArrayIn[ 8]; f03=afArrayIn[12];
                                                                                f10=afArrayIn[ 1]; f11=afArrayIn[ 5]; f12=afArrayIn[ 9]; f13=afArrayIn[13];
                                                                                f20=afArrayIn[ 2]; f21=afArrayIn[ 6]; f22=afArrayIn[10]; f23=afArrayIn[14];
                                                                                f30=afArrayIn[ 3]; f31=afArrayIn[ 7]; f32=afArrayIn[11]; f33=afArrayIn[15];};

    VOID        SetF        (const FLOAT *  afArrayIn)                         {f00=afArrayIn[ 0]; f01=afArrayIn[ 4]; f02=afArrayIn[ 8]; f03=afArrayIn[12];
                                                                                f10=afArrayIn[ 1]; f11=afArrayIn[ 5]; f12=afArrayIn[ 9]; f13=afArrayIn[13];
                                                                                f20=afArrayIn[ 2]; f21=afArrayIn[ 6]; f22=afArrayIn[10]; f23=afArrayIn[14];
                                                                                f30=afArrayIn[ 3]; f31=afArrayIn[ 7]; f32=afArrayIn[11]; f33=afArrayIn[15];};

    VOID        SetTransposedF  (const FLOAT *  afArrayIn)                     {f00=afArrayIn[ 0]; f01=afArrayIn[ 1]; f02=afArrayIn[ 2]; f03=afArrayIn[ 3];
                                                                                f10=afArrayIn[ 4]; f11=afArrayIn[ 5]; f12=afArrayIn[ 6]; f13=afArrayIn[ 7];
                                                                                f20=afArrayIn[ 8]; f21=afArrayIn[ 9]; f22=afArrayIn[10]; f23=afArrayIn[11];
                                                                                f30=afArrayIn[12]; f31=afArrayIn[13]; f32=afArrayIn[14]; f33=afArrayIn[15];};

    VOID        Set         (const RMatrix &  matIn)                           {*this = matIn;};

    VOID        SetTrans    (RMAT_T _f03,
                             RMAT_T _f13,
                             RMAT_T _f23)                                      {f03=_f03; f13=_f13; f23=_f23;};

    VOID        SetTrans    (const RVec3&  v3In)                               {tx=v3In.fX; ty=v3In.fY; tz=v3In.fZ;};

    VOID        AddTrans    (const RVec3&  v3In)                               {tx+=v3In.fX; ty+=v3In.fY; tz+=v3In.fZ;};

    RVec3       GetTrans    (VOID) const                                       {return RVec3 (tx, ty, tz);};

    VOID        GetTrans    (RVec3 &  v3In) const                              {v3In.Set (tx, ty, tz);};

    // GetArray is meant for acquiring the array for passing to an API.
    RMAT_T *    GetArray    (VOID)                                      {return fArray;};

    VOID        GetFloatArray           (FLOAT *  pafIn);
    VOID        GetFloatArrayTransposed (FLOAT *  pafIn);


    // matrix multiplication  (concatenation)
    RMatrix     operator*   (const RMatrix& matIn) const;
    RMatrix&    operator*=  (const RMatrix& matIn);
    void        MultMatrix  (RMatrix *  pmatIn)        {*this *= *pmatIn;};

    // assignment
    RMatrix&    operator=   (const RMatrix& matIn)     {f00=matIn.f00; f01=matIn.f01; f02=matIn.f02; f03=matIn.f03;
                                                        f10=matIn.f10; f11=matIn.f11; f12=matIn.f12; f13=matIn.f13;
                                                        f20=matIn.f20; f21=matIn.f21; f22=matIn.f22; f23=matIn.f23;
                                                        f30=matIn.f30; f31=matIn.f31; f32=matIn.f32; f33=matIn.f33;
                                                        return *this;};

    BOOL        operator==  (const  RMatrix& matIn);
    BOOL        operator!=  (const  RMatrix& matIn)    {return (!(*this == matIn));};

    // vector multiplication (assumes the W element of the vector is 1 when multiplying against vec3)
    RVec4       operator*   (const RVec3& v3In) const  {return RVec4 (v3In.fX*f00 + v3In.fY*f01 + v3In.fZ*f02 + f03,
                                                                      v3In.fX*f10 + v3In.fY*f11 + v3In.fZ*f12 + f13,
                                                                      v3In.fX*f20 + v3In.fY*f21 + v3In.fZ*f22 + f23,
                                                                      v3In.fX*f30 + v3In.fY*f31 + v3In.fZ*f32 + f33);};

    RVec4       operator*   (const RVec4& v4In) const  {return RVec4 (v4In.fX*f00 + v4In.fY*f01 + v4In.fZ*f02 + v4In.fW*f03,
                                                                      v4In.fX*f10 + v4In.fY*f11 + v4In.fZ*f12 + v4In.fW*f13,
                                                                      v4In.fX*f20 + v4In.fY*f21 + v4In.fZ*f22 + v4In.fW*f23,
                                                                      v4In.fX*f30 + v4In.fY*f31 + v4In.fZ*f32 + v4In.fW*f33);};

    // Assumes the W component is 1
    RVec4       MultPoint   (RVEC_T  fX,
                             RVEC_T  fY,
                             RVEC_T  fZ) const         {return RVec4 (fX*f00 + fY*f01 + fZ*f02 + f03,
                                                                      fX*f10 + fY*f11 + fZ*f12 + f13,
                                                                      fX*f20 + fY*f21 + fZ*f22 + f23,
                                                                      fX*f30 + fY*f31 + fZ*f32 + f33);};

    // This function is to make it easier to write x,y,z coordinates into a float array for OpenGL.  W is ignored; not for projections.
    VOID        MultPoint3  (RVEC_T  fX,
                             RVEC_T  fY,
                             RVEC_T  fZ,
                             FLOAT *        pfOut) const  {pfOut[0] = fX*f00 + fY*f01 + fZ*f02 + f03;
                                                           pfOut[1] = fX*f10 + fY*f11 + fZ*f12 + f13;
                                                           pfOut[2] = fX*f20 + fY*f21 + fZ*f22 + f23;};

    // Assumes the W component is 0
    RVec4       MultNormal  (RVEC_T  fX,
                             RVEC_T  fY,
                             RVEC_T  fZ) const         {return RVec4 (fX*f00 + fY*f01 + fZ*f02,
                                                                      fX*f10 + fY*f11 + fZ*f12,
                                                                      fX*f20 + fY*f21 + fZ*f22,
                                                                      fX*f30 + fY*f31 + fZ*f32);};

    RVec3       MultNormal3 (RVEC_T  fX,
                             RVEC_T  fY,
                             RVEC_T  fZ) const         {return RVec3 (fX*f00 + fY*f01 + fZ*f02,
                                                                      fX*f10 + fY*f11 + fZ*f12,
                                                                      fX*f20 + fY*f21 + fZ*f22);};

    // vector multiplication against individual components of the matrix (third row ignored)
    RVec3       MultTrans   (const RVec3& v3In) const  {return RVec3 (v3In.fX + f03, v3In.fY + f13, v3In.fZ + f23);};
    RVec3       MultRotate  (const RVec3& v3In) const  {return RVec3 (v3In.fX*f00 + v3In.fY*f01 + v3In.fZ*f02,
                                                                      v3In.fX*f10 + v3In.fY*f11 + v3In.fZ*f12,
                                                                      v3In.fX*f20 + v3In.fY*f21 + v3In.fZ*f22);};

    // multiply by the full matrix, assuming W is 1.0, then reduce back to homogenous coordinates.
    RVec3       MultHomogenous (const RVec3& v3In) const  {RMAT_T  fW = v3In.fX*f30 + v3In.fY*f31 + v3In.fZ*f32 + f33;
                                                           return RVec3 ((v3In.fX*f00 + v3In.fY*f01 + v3In.fZ*f02 + f03) / fW,
                                                                         (v3In.fX*f10 + v3In.fY*f11 + v3In.fZ*f12 + f13) / fW,
                                                                         (v3In.fX*f20 + v3In.fY*f21 + v3In.fZ*f22 + f23) / fW);};



    VOID        Identity     (VOID)                     {f00=1.0f; f01=0.0f; f02=0.0f; f03=0.0f;
                                                         f10=0.0f; f11=1.0f; f12=0.0f; f13=0.0f;
                                                         f20=0.0f; f21=0.0f; f22=1.0f; f23=0.0f;
                                                         f30=0.0f; f31=0.0f; f32=0.0f; f33=1.0f;};


    // build rotation matrixes

    VOID        SetRotateX   (RMAT_T  fAngleIn);   ///< angle given in radians
    VOID        SetRotateY   (RMAT_T  fAngleIn);   ///< angle given in radians
    VOID        SetRotateZ   (RMAT_T  fAngleIn);   ///< angle given in radians

    VOID        GetRotateX   (RVec3&  v3VecOut) const   {v3VecOut.Set (f00, f10, f20);};
    VOID        GetRotateY   (RVec3&  v3VecOut) const   {v3VecOut.Set (f01, f11, f21);};
    VOID        GetRotateZ   (RVec3&  v3VecOut) const   {v3VecOut.Set (f02, f12, f22);};

    VOID        GetInvRotateX  (RVec3&  v3VecOut) const {v3VecOut.Set (f00, f01, f02);};
    VOID        GetInvRotateY  (RVec3&  v3VecOut) const {v3VecOut.Set (f10, f11, f12);};
    VOID        GetInvRotateZ  (RVec3&  v3VecOut) const {v3VecOut.Set (f20, f21, f22);};

    VOID        SetRotate    (const RVec3&  v3XIn,
                              const RVec3&  v3YIn,
                              const RVec3&  v3ZIn)      {f00=v3XIn.fX; f01=v3YIn.fX; f02=v3ZIn.fX; f03 = 0.0f;
                                                         f10=v3XIn.fY; f11=v3YIn.fY; f12=v3ZIn.fY; f13 = 0.0f;
                                                         f20=v3XIn.fZ; f21=v3YIn.fZ; f22=v3ZIn.fZ; f23 = 0.0f;
                                                         f30=0.0f;     f31=0.0f;     f32=0.0f;     f33 = 1.0f;};

    VOID        SetRotateInv (const RVec3&  v3XIn,
                              const RVec3&  v3YIn,
                              const RVec3&  v3ZIn)      {f00=v3XIn.fX; f01=v3YIn.fX; f02=v3ZIn.fX; f03 = 0.0f;
                                                         f10=v3XIn.fY; f11=v3YIn.fY; f12=v3ZIn.fY; f13 = 0.0f;
                                                         f20=v3XIn.fZ; f21=v3YIn.fZ; f22=v3ZIn.fZ; f23 = 0.0f;
                                                         f30=0.0f;     f31=0.0f;     f32=0.0f;     f33 = 1.0f;};

    VOID        SetScale    (RMAT_T  fScaleIn)          {f00=fScaleIn; f01=0.0f;     f02=0.0f;     f03 = 0.0f;
                                                         f10=0.0f;     f11=fScaleIn; f12=0.0f;     f13 = 0.0f;
                                                         f20=0.0f;     f21=0.0f;     f22=fScaleIn; f23 = 0.0f;
                                                         f30=0.0f;     f31=0.0f;     f32=0.0f;     f33 = 1.0f;};

    VOID        SetScale    (RMAT_T  fScaleXIn,
                             RMAT_T  fScaleYIn,
                             RMAT_T  fScaleZIn)         {f00=fScaleXIn; f01=0.0f;      f02=0.0f;      f03 = 0.0f;
                                                         f10=0.0f;      f11=fScaleYIn; f12=0.0f;      f13 = 0.0f;
                                                         f20=0.0f;      f21=0.0f;      f22=fScaleZIn; f23 = 0.0f;
                                                         f30=0.0f;      f31=0.0f;      f32=0.0f;      f33 = 1.0f;};

    RMAT_T      GetScaleX   (VOID) const                {return (RMAT_SQRT (r00*r00 + r01*r01 + r02*r02));};
    RMAT_T      GetScaleY   (VOID) const                {return (RMAT_SQRT (r10*r10 + r11*r11 + r12*r12));};
    RMAT_T      GetScaleZ   (VOID) const                {return (RMAT_SQRT (r20*r20 + r21*r21 + r22*r22));};

    // transpose the 3x3 rotation sub-matrix
    VOID        RotTranspose (VOID)                     {RMAT_T t;
                                                         t=f10;f10=f01;f01=t; t=f20;f20=f02;f02=t; t=f21;f21=f12;f12=t;};


    VOID        Transpose    (VOID)                     {RMAT_T t;
                                                         t=f10;f10=f01;f01=t; t=f20;f20=f02;f02=t; t=f30;f30=f03;f03=t;
                                                         t=f21;f21=f12;f12=t; t=f31;f31=f13;f13=t; t=f32;f32=f23;f23=t;};

    VOID        RigidInvert     (VOID);

    RMAT_T      Determinant     (VOID);
    VOID        Invert          (VOID);

    //VOID           FromEuler       (RVec4  vecEulerIn);

    //VOID           FromEuler       (FLOAT  fXIn, ///< X rotation in radians
    //                                FLOAT  fYIn, ///< Y rotation in radians
    //                                FLOAT  fZIn, ///< Z rotation in radians
    //                                INT    iOrderIn);

    //RVec4          ToEuler         (int  iOrderIn);

    RVec4       ToQuat          (VOID) const;

    VOID        FromQuat        (RVec4  vecQuatIn);

    VOID        DebugPrint      (const char *  pszMatrixNameIn = "") const;

    //VOID           GetEulerAngles  (RVec3 & vecAngles,
    //                                RVec3   vecBase);

    //VOID           GetEulerAngles  (RVec3 &  vecAngles,
    //                                INT      iOrderIn,
    //                                RVec3    vecBase);

    //FLOAT          ClampPi         (FLOAT  fIn);

  };


//-----------------------------------------------------------------------------
// functions for manipulating matrixes

VOID    MatrixPerspective          (RMatrix&       matMatrixOut,
                                    RMAT_T  fFOVYIn,       // in radians
                                    RMAT_T  fAspectIn,
                                    RMAT_T  fNearIn,
                                    RMAT_T  fFarIn);

VOID    MatrixPerspective          (RMatrix&       matMatrixOut,
                                    RMAT_T  fLeft,
                                    RMAT_T  fRight,
                                    RMAT_T  fTop,
                                    RMAT_T  fBottom,
                                    RMAT_T  fNear, /// positive value
                                    RMAT_T  fFar); /// positive value

VOID    MatrixOrtho                (RMatrix&       matOut,
                                    RMAT_T  fLeft,
                                    RMAT_T  fRight,
                                    RMAT_T  fBottom,
                                    RMAT_T  fTop,
                                    RMAT_T  fNear,
                                    RMAT_T  fFar);

VOID    MatrixLookAt               (RMatrix&      matMatrixOut,
                                    const RVec3&  v3Position,     // point
                                    const RVec3&  v3Target,       // point
                                    const RVec3&  v3Up);          // vector

VOID    MatrixChangeCoordSystem    (RMatrix&      matMatrixOut,
                                    const RVec3&  v3AxisXIn,
                                    const RVec3&  v3AxisYIn,
                                    const RVec3&  v3AxisZIn,
                                    const RVec3&  v3PointIn);

VOID    MatrixPerspectiveOGLtoD3D  (RMatrix&  matProjection);


VOID    MatrixAxisAngle            (RMatrix&      matMatrixOut,
                                    const RVec3&  v3AxisIn,
                                    RMAT_T        dAngleIn);  // in radians

RMAT_T  MatrixDistSquared          (const RMatrix&      matMatrixA,
                                    const RMatrix&      matMatrixB);

RMAT_T  MatrixZDistSquared         (const RMatrix&      matMatrixA,
                                    const RMatrix&      matMatrixB);


#endif // RMATRIX_HPP


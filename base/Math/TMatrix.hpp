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


#ifndef TMATRIX_HPP
#define TMATRIX_HPP

#include "Sys/Types.hpp"
#include <math.h>


// Column-major 4x4 matrix.
typedef struct TMatrix32f_
  {
  union
    {
    float  afM [4][4];
    float  afA [16];
    struct {float   f00,f10,f20,f30,  f01,f11,f21,f31,  f02,f12,f22,f32,  f03,f13,f23,f33;};
    struct {float   r00,r10,r20,_30,  r01,r11,r21,_31,  r02,r12,r22,_32,  tx,ty,tz,_33;};
    };  
  } TMatrix32f;

typedef struct TVector3f_
  {
  float x, y, z;
  } TVector3f;


  
// Returns a 4x4 identity matrix.
//-----------------------------------------------------------------------------
static inline TMatrix32f TMatrix32f_Identity ()
  {
  TMatrix32f out;  
  out.f00=1.0f; out.f01=0.0f; out.f02=0.0f; out.f03=0.0f;
  out.f10=0.0f; out.f11=1.0f; out.f12=0.0f; out.f13=0.0f;
  out.f20=0.0f; out.f21=0.0f; out.f22=1.0f; out.f23=0.0f;
  out.f30=0.0f; out.f31=0.0f; out.f32=0.0f; out.f33=1.0f;
  return out;
  }  
  
//-----------------------------------------------------------------------------
static inline TMatrix32f TMatrix32f_Translation (const float fXIn, const float fYIn, const float fZIn)
  {
  TMatrix32f out;  
  out.f00=1.0f; out.f01=0.0f; out.f02=0.0f; out.tx=fXIn;
  out.f10=0.0f; out.f11=1.0f; out.f12=0.0f; out.ty=fYIn;
  out.f20=0.0f; out.f21=0.0f; out.f22=1.0f; out.tz=fZIn;
  out.f30=0.0f; out.f31=0.0f; out.f32=0.0f; out.f33=1.0f;
  return out;
  }  
  
//-----------------------------------------------------------------------------
static inline TMatrix32f TMatrix32f_Mult  (const TMatrix32f& L, const TMatrix32f& R)
  {
  TMatrix32f out;  

  out.f00 = L.afA[0]*R.afA[ 0] + L.afA[4]*R.afA[ 1] + L.afA[ 8]*R.afA[ 2] + L.afA[12]*R.afA[ 3];
  out.f01 = L.afA[0]*R.afA[ 4] + L.afA[4]*R.afA[ 5] + L.afA[ 8]*R.afA[ 6] + L.afA[12]*R.afA[ 7];
  out.f02 = L.afA[0]*R.afA[ 8] + L.afA[4]*R.afA[ 9] + L.afA[ 8]*R.afA[10] + L.afA[12]*R.afA[11];
  out.f03 = L.afA[0]*R.afA[12] + L.afA[4]*R.afA[13] + L.afA[ 8]*R.afA[14] + L.afA[12]*R.afA[15];

  out.f10 = L.afA[1]*R.afA[ 0] + L.afA[5]*R.afA[ 1] + L.afA[ 9]*R.afA[ 2] + L.afA[13]*R.afA[ 3];
  out.f11 = L.afA[1]*R.afA[ 4] + L.afA[5]*R.afA[ 5] + L.afA[ 9]*R.afA[ 6] + L.afA[13]*R.afA[ 7];
  out.f12 = L.afA[1]*R.afA[ 8] + L.afA[5]*R.afA[ 9] + L.afA[ 9]*R.afA[10] + L.afA[13]*R.afA[11];
  out.f13 = L.afA[1]*R.afA[12] + L.afA[5]*R.afA[13] + L.afA[ 9]*R.afA[14] + L.afA[13]*R.afA[15];

  out.f20 = L.afA[2]*R.afA[ 0] + L.afA[6]*R.afA[ 1] + L.afA[10]*R.afA[ 2] + L.afA[14]*R.afA[ 3];
  out.f21 = L.afA[2]*R.afA[ 4] + L.afA[6]*R.afA[ 5] + L.afA[10]*R.afA[ 6] + L.afA[14]*R.afA[ 7];
  out.f22 = L.afA[2]*R.afA[ 8] + L.afA[6]*R.afA[ 9] + L.afA[10]*R.afA[10] + L.afA[14]*R.afA[11];
  out.f23 = L.afA[2]*R.afA[12] + L.afA[6]*R.afA[13] + L.afA[10]*R.afA[14] + L.afA[14]*R.afA[15];

  out.f30 = L.afA[3]*R.afA[ 0] + L.afA[7]*R.afA[ 1] + L.afA[11]*R.afA[ 2] + L.afA[15]*R.afA[ 3];
  out.f31 = L.afA[3]*R.afA[ 4] + L.afA[7]*R.afA[ 5] + L.afA[11]*R.afA[ 6] + L.afA[15]*R.afA[ 7];
  out.f32 = L.afA[3]*R.afA[ 8] + L.afA[7]*R.afA[ 9] + L.afA[11]*R.afA[10] + L.afA[15]*R.afA[11];
  out.f33 = L.afA[3]*R.afA[12] + L.afA[7]*R.afA[13] + L.afA[11]*R.afA[14] + L.afA[15]*R.afA[15];
  
  return out;
  }; 
 
//-----------------------------------------------------------------------------
static inline TMatrix32f TMatrix32f_Transpose  (const TMatrix32f& matIn)
  {
  TMatrix32f out;  

  out.f00 = matIn.f00; out.f01 = matIn.f10; out.f02 = matIn.f20; out.f03 = matIn.f30;
  out.f10 = matIn.f01; out.f11 = matIn.f11; out.f12 = matIn.f21; out.f13 = matIn.f31;
  out.f20 = matIn.f02; out.f21 = matIn.f12; out.f22 = matIn.f22; out.f23 = matIn.f32;
  out.f30 = matIn.f03; out.f31 = matIn.f13; out.f32 = matIn.f23; out.f33 = matIn.f33;
  
  return out;
  }; 
 
 
  
//-----------------------------------------------------------------------------
static inline TMatrix32f TMatrix32f_CreateProjection (const float fMinX, 
                                                      const float fMaxX,
                                                      float const fMinY, 
                                                      const float fMaxY, 
                                                      const float fNearZ, 
                                                      const float fFarZ)
  {
  // NOTE: This version of the projection matrix is from the VrApi that ships
  //       with the Oculus Mobile SDK for GearVR.
    
  // Returns a projection matrix based on the specified dimensions.
  // The far plane is placed at infinity if fFarZ <= fNearZ.
  // An infinite projection matrix is preferred for rasterization because, except for
  // things *right* up against the near plane, it always provides better precision:
  //    "Tightening the Precision of Perspective Rendering"
  //    Paul Upchurch, Mathieu Desbrun
  //    Journal of Graphics Tools, Volume 16, Issue 1, 2012
    
  const float fWidth   = fMaxX - fMinX;
  const float fHeight  = fMaxY - fMinY;
  const float fOffsetZ = fNearZ;  // set to zero for a [0,1] clip space

  TMatrix32f out;
  if ( fFarZ <= fNearZ )
    {
    // place the far plane at infinity
    out.f00 = 2 * fNearZ / fWidth;
    out.f01 = 0;
    out.f02 = (fMaxX + fMinX) / fWidth;
    out.f03 = 0; // tx

    out.f10 = 0;
    out.f11 = 2 * fNearZ / fHeight;
    out.f12 = (fMaxY + fMinY) / fHeight;
    out.f13 = 0;

    out.f20 = 0;
    out.f21 = 0;
    out.f22 = -1;
    out.f23 = -(fNearZ + fOffsetZ);

    out.f30 = 0;
    out.f31 = 0;
    out.f32 = -1;
    out.f33 = 0;
    }
  else
    {
    // normal projection
    out.f00 = 2 * fNearZ / fWidth;
    out.f01 = 0;
    out.f02 = (fMaxX + fMinX) / fWidth;
    out.f03 = 0;

    out.f10 = 0;
    out.f11 = 2 * fNearZ / fHeight;
    out.f12 = (fMaxY + fMinY) / fHeight;
    out.f13 = 0;

    out.f20 = 0;
    out.f21 = 0;
    out.f22 = -(fFarZ + fOffsetZ) / (fFarZ - fNearZ);
    out.f23 = -(fFarZ * (fNearZ + fOffsetZ)) / (fFarZ - fNearZ);

    out.f30 = 0;
    out.f31 = 0;
    out.f32 = -1;
    out.f33 = 0;
    }
  return out;
  }

//-----------------------------------------------------------------------------
static inline TMatrix32f TMatrix32f_CreateProjectionFov (const float  fFovDegreesX, 
                                                         const float  fFovDegreesY,
                                                         const float  fOffsetX, 
                                                         const float  fOffsetY, 
                                                         const float  fNearZ, 
                                                         const float  fFarZ)
  {
  // Returns a projection matrix based on the given FOV.
    
  const float fHalfWidth  = fNearZ * tanf (fFovDegreesX * (R_DEG_TO_RAD * 0.5f));
  const float fHalfHeight = fNearZ * tanf (fFovDegreesY * (R_DEG_TO_RAD * 0.5f));

  const float fMinX = fOffsetX - fHalfWidth;
  const float fMaxX = fOffsetX + fHalfWidth;

  const float fMinY = fOffsetY - fHalfHeight;
  const float fMaxY = fOffsetY + fHalfHeight;

  return TMatrix32f_CreateProjection (fMinX, fMaxX, fMinY, fMaxY, fNearZ, fFarZ);
  } 

  
  /*
// copies a 4x4 matrix
static inline ovrMatrix4f ovrMatrix4f_Copy (const ovrMatrix4f & from, ovrMatrix4f &  to)
{
    to.M[0][0] = from.M[0][0]; to.M[0][1] = from.M[0][1]; to.M[0][2] = from.M[0][2]; to.M[0][3] = from.M[0][3];
    to.M[1][0] = from.M[1][0]; to.M[1][1] = from.M[1][1]; to.M[1][2] = from.M[1][2]; to.M[1][3] = from.M[1][3];
    to.M[2][0] = from.M[2][0]; to.M[2][1] = from.M[2][1]; to.M[2][2] = from.M[2][2]; to.M[2][3] = from.M[2][3];
    to.M[3][0] = from.M[3][0]; to.M[3][1] = from.M[3][1]; to.M[3][2] = from.M[3][2]; to.M[3][3] = from.M[3][3];
}
*/


  
 
  
  
  
  
  
#endif // TMATRIX_HPP
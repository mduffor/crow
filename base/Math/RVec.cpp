/* -----------------------------------------------------------------
                       3D Vector Library

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


// Note:  Need to redefine sin, cos, tan, atan, and the other trig
//        functions as macros that call the float or double version
//        of the code based on what RVEC_T is set to.
// to be implemented


#include "Math/RVec.hpp"


RVec2  RVec2::kZero  (0.0f, 0.0f);
RVec2  RVec2::kAxisX (1.0f, 0.0f);
RVec2  RVec2::kAxisY (0.0f, 1.0f);

RVec3  RVec3::kZero  (0.0f, 0.0f, 0.0f);
RVec3  RVec3::kAxisX (1.0f, 0.0f, 0.0f);
RVec3  RVec3::kAxisY (0.0f, 1.0f, 0.0f);
RVec3  RVec3::kAxisZ (0.0f, 0.0f, 1.0f);
RVec4  RVec4::kZero  (0.0f, 0.0f, 0.0f, 0.0f);



//-----------------------------------------------------------------------------
//  RVec2
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
RVec2::RVec2  (const RVec3& v3In)
  {
  fX = v3In.fX;
  fY = v3In.fY;
  };

//-----------------------------------------------------------------------------
RVec2::RVec2  (const RVec4& v4In)
  {
  fX = v4In.fX;
  fY = v4In.fY;
  };

//-----------------------------------------------------------------------------
RVec2&  RVec2::operator= (const RVec3& v3In)
  {
  fX = v3In.fX;
  fY = v3In.fY;
  return *this;
  };

//-----------------------------------------------------------------------------
RVec2&  RVec2::operator= (const RVec4& v4In)
  {
  fX = v4In.fX;
  fY = v4In.fY;
  return *this;
  };

//-----------------------------------------------------------------------------
VOID  RVec2::DebugPrint    (const char *  pszLabelIn) const
  {
  DBG_INFO ("%s: (%f %f)\n", pszLabelIn, fX, fY);
  };


//-----------------------------------------------------------------------------
//  RVec3
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
RVec3::RVec3  (const RVec4& v4In)
  {
  fX = v4In.fX;
  fY = v4In.fY;
  fZ = v4In.fZ;
  };


//-----------------------------------------------------------------------------
RVec3&  RVec3::operator= (const RVec4& v4In)
  {
  fX = v4In.fX;
  fY = v4In.fY;
  fZ = v4In.fZ;
  return *this;
  };


//-----------------------------------------------------------------------------
VOID  RVec3::DebugPrint    (const char *  pszLabelIn) const
  {
  DBG_INFO ("%s: (%f %f %f)", pszLabelIn, fX, fY, fZ);
  };


//-----------------------------------------------------------------------------
//  RVec4
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
VOID RVec4::QuatNormalize (VOID)
  {
  FLOAT fFactor =  fX*fX + fY*fY + fZ*fZ + fW*fW;

  ASSERT (fFactor != 0);

  RVEC_T  scaleBy =  (1.0f / RVEC_SQRT (fFactor));

  fX = fX * scaleBy;
  fY = fY * scaleBy;
  fZ = fZ * scaleBy;
  fW = fW * scaleBy;
  };


//-----------------------------------------------------------------------------
VOID RVec4::QuatFromAxisAngle (RVec3   vec3AxisIn,
                               RVEC_T  fAngleIn)  // angle in radians=
  {
  vec3AxisIn = vec3AxisIn.Normalize ();

  SetVector (vec3AxisIn);
  *this *= sinf (fAngleIn / 2.0);
  fW = cosf (fAngleIn / 2.0);
  };


//-----------------------------------------------------------------------------
RVec4 RVec4::QuatSLERP  (const RVec4&   qTo,
                         FLOAT          fTime) const
  {
  FLOAT            to1[4];
  FLOAT            cosom, scale0, scale1;



  // calc cosine
  cosom = fX * qTo.fX + fY * qTo.fY +
          fZ * qTo.fZ + fW * qTo.fW;

  // adjust signs (if necessary)
  if ( cosom < 0.0)
    {
    cosom  = -cosom;
    to1[0] = -qTo.fX;
    to1[1] = -qTo.fY;
    to1[2] = -qTo.fZ;
    to1[3] = -qTo.fW;
    }
  else
    {
    to1[0] = qTo.fX;
    to1[1] = qTo.fY;
    to1[2] = qTo.fZ;
    to1[3] = qTo.fW;
    };

  // calculate coefficients

  if ((1.0 - cosom) > R_QUAT_DELTA)
    {
     FLOAT  omega, sinom;
    // standard case (slerp)

    omega  = acosf (cosom);
    sinom  = sinf (omega);
    scale0 = sinf ((1.0f - fTime) * omega) / sinom;
    scale1 = sinf (fTime * omega) / sinom;
    }
  else
    {
    // "from" and "to" quaternions are very close
    //  ... so we can do a linear interpolation

    scale0 = 1.0f - fTime;
    scale1 = fTime;
    };

  // calculate final values

  return (RVec4 (((scale0 * fX) + (scale1 * to1[0])),
                 ((scale0 * fY) + (scale1 * to1[1])),
                 ((scale0 * fZ) + (scale1 * to1[2])),
                 ((scale0 * fW) + (scale1 * to1[3]))));
  };


//-----------------------------------------------------------------------------
VOID  RVec4::DebugPrint    (const char *  pszLabelIn) const
  {
  printf ("%s: (%f %f %f %f)\n", pszLabelIn, fX, fY, fZ, fW);
  };


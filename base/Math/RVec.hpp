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


#ifndef RVEC_HPP
#define RVEC_HPP


#include "Sys/Types.hpp"
#include <math.h>
#include <float.h>


//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

// NOTE: R_EPSILON is now defined centrally in Sys/Types.hpp
//#ifndef R_EPSILON
//  #define R_EPSILON             0.0000000001f
//#endif

// Note:  If the datatype is something other than Float then you need
//         to change how buffers are passed to OpenGL.
//#define RVEC_T    DOUBLE
#define RVEC_T    FLOAT

#define RVEC_SQRT    sqrtf  // define as sqrt for double, sqrtf for float


#ifndef R_QUAT_DELTA
  #define R_QUAT_DELTA          1.0
#endif



//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------

class   RVec2;
typedef RVec2 *  PRVec2;
class   RVec3;
typedef RVec3 *  PRVec3;
class   RVec4;
typedef RVec4 *  PRVec4;

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


//------------------------------------------------------------------------
class RVec2
//------------------------------------------------------------------------
  {
  public:

    union
      {
      RVEC_T        fX;
      RVEC_T        fW;
      RVEC_T        fU;
      };
    union
      {
      RVEC_T        fY;
      RVEC_T        fH;
      RVEC_T        fV;
      };

  public:
    static  RVec2         kZero;
    static  RVec2         kAxisX;
    static  RVec2         kAxisY;

             RVec2        ()                          {};

             RVec2        (RVEC_T  fXIn,
                           RVEC_T  fYIn)       {fX = fXIn; fY = fYIn;};

             RVec2        (const RVec2& v2In)         {fX = v2In.fX; fY = v2In.fY;};

    explicit RVec2        (const RVec3& v3In);

    explicit RVec2        (const RVec4& v4In);

    virtual  ~RVec2       ()                          {};

    RVec2&  Set           (RVEC_T  fXIn,
                           RVEC_T  fYIn)         {fX = fXIn; fY = fYIn; return (*this);};

    RVec2&  Set           (const RVec2& v2In)           {fX = v2In.fX; fY = v2In.fY; return (*this);};

    RVec2   operator+     (const RVec2& v2In) const   {return (RVec2 (fX + v2In.fX, fY + v2In.fY));};
    RVec2   operator-     (const RVec2& v2In) const   {return (RVec2 (fX - v2In.fX, fY - v2In.fY));};

    RVec2&  operator+=    (const RVec2& v2In)         {fX += v2In.fX; fY += v2In.fY; return *this;};
    RVec2&  operator-=    (const RVec2& v2In)         {fX -= v2In.fX; fY -= v2In.fY; return *this;};

    // scalar multiply and divide
    RVec2   operator*     (const RVEC_T  fIn) const          {return (RVec2 (fX * fIn, fY * fIn));};
    RVec2   operator/     (const RVEC_T  fIn) const          {return (RVec2 (fX / fIn, fY / fIn));};

    RVec2&  operator*=    (const RVEC_T  fIn)                {fX *= fIn; fY *= fIn; return *this;};
    RVec2&  operator/=    (const RVEC_T  fIn)                {fX /= fIn; fY /= fIn; return *this;};

    // dot product
    RVEC_T   operator*     (const RVec2& v2In) const   {return (fX*v2In.fX + fY*v2In.fY);};

    // assignment
    RVec2&  operator=     (const RVec2& v2In)         {fX = v2In.fX; fY = v2In.fY; return *this;};
    RVec2&  operator=     (const RVec3& v3In);
    RVec2&  operator=     (const RVec4& v4In);

    // reverse sign (unary negation)
    RVec2   operator-     (VOID) const                {return (RVec2 (-fX, -fY));};

    // equality (each value within R_EPSILON (0.001) of each other)
    BOOL    operator==    (const RVec2& v2In) const   {return ((fabsf (fX - v2In.fX) <= R_EPSILON) && (fabsf (fY - v2In.fY) <= R_EPSILON));};
    BOOL    operator!=    (const RVec2& v2In) const   {return ((fabsf (fX - v2In.fX) > R_EPSILON) || (fabsf (fY - v2In.fY) > R_EPSILON));};

    RVEC_T   LengthSquared (VOID) const                {return (fX*fX + fY*fY);};

    RVEC_T   Length        (VOID) const                {float  fSquared = LengthSquared ();
                                                               if (fSquared < 0.00000001) return 0.00000001;
                                                               return (RVEC_SQRT (fSquared));};

    RVec2&  Normalize     (VOID)                      {RVEC_T  fLength = Length (); fX /= fLength; fY /= fLength;  return *this;};

    VOID    Zero          (VOID)                      {fX = fY = 0.0f;};

    VOID    Reverse       (VOID)                      {fX = -fX; fY = -fY;};

    VOID    Add           (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn)               {fX += fXIn; fY += fYIn;};

    VOID    Subtract      (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn)               {fX += fXIn; fY += fYIn;};

    // component-wise squaring

    RVec2   Squared       (VOID) const                {return RVec2 (fX * fX, fY * fY);};

    VOID    DebugPrint    (const char *  pszLabelIn) const;

  };


//------------------------------------------------------------------------
class RVec3
//------------------------------------------------------------------------
  {
  public:

    union
      {
      RVEC_T        fX;
      RVEC_T        fR;
      RVEC_T        fU;
      };
    union
      {
      RVEC_T        fY;
      RVEC_T        fG;
      RVEC_T        fV;
      };
    union
      {
      RVEC_T        fZ;
      RVEC_T        fB;
      RVEC_T        fW;
      };

  public:
    static  RVec3         kZero;
    static  RVec3         kAxisX;
    static  RVec3         kAxisY;
    static  RVec3         kAxisZ;

            RVec3         ()                          {};

            RVec3         (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn)               {fX = fXIn; fY = fYIn; fZ = fZIn;};

            RVec3         (const RVec3& v3In)         {fX = v3In.fX; fY = v3In.fY; fZ = v3In.fZ;};

    explicit RVec3        (const RVec4& v4In);

    virtual ~RVec3        ()                          {};

    RVec3&  Set           (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn = 0.0f)  {fX = fXIn; fY = fYIn; fZ = fZIn; return (*this);};

    RVec3&  Set           (const RVec3& v3In)           {fX = v3In.fX; fY = v3In.fY; fZ = v3In.fZ; return (*this);};

    RVec3   operator+     (const RVec3& v3In) const   {return (RVec3 (fX + v3In.fX, fY + v3In.fY, fZ + v3In.fZ));};
    RVec3   operator-     (const RVec3& v3In) const   {return (RVec3 (fX - v3In.fX, fY - v3In.fY, fZ - v3In.fZ));};

    RVec3&  operator+=    (const RVec3& v3In)         {fX += v3In.fX; fY += v3In.fY; fZ += v3In.fZ; return *this;};
    RVec3&  operator-=    (const RVec3& v3In)         {fX -= v3In.fX; fY -= v3In.fY; fZ -= v3In.fZ; return *this;};

    // scalar multiply and divide
    RVec3   operator*     (const RVEC_T  fIn) const          {return (RVec3 (fX * fIn, fY * fIn, fZ * fIn));};
    RVec3   operator/     (const RVEC_T  fIn) const          {return (RVec3 (fX / fIn, fY / fIn, fZ / fIn));};

    RVec3&  operator*=    (const RVEC_T  fIn)                {fX *= fIn; fY *= fIn; fZ *= fIn; return *this;};
    RVec3&  operator/=    (const RVEC_T  fIn)                {fX /= fIn; fY /= fIn; fZ /= fIn; return *this;};

    // cross product
    RVec3   operator%     (const RVec3& v3In) const   {return (RVec3 (fY*v3In.fZ - fZ*v3In.fY, fZ*v3In.fX - fX*v3In.fZ, fX*v3In.fY - fY*v3In.fX));};

    // dot product
    RVEC_T   operator*     (const RVec3& v3In) const   {return (fX*v3In.fX + fY*v3In.fY + fZ*v3In.fZ);};

    // assignment
    RVec3&  operator=     (const RVec3& v3In)         {fX = v3In.fX; fY = v3In.fY; fZ = v3In.fZ; return *this;};
    RVec3&  operator=     (const RVec4& v4In);

    // reverse sign (unary negation)
    RVec3   operator-     (VOID) const                {return (RVec3 (-fX, -fY, -fZ));};

    // equality (each value within R_EPSILON (0.001) of each other)
    BOOL    operator==    (const RVec3& v3In) const   {return ((fabsf (fX - v3In.fX) <= R_EPSILON) && (fabsf (fY - v3In.fY) <= R_EPSILON) && (fabsf (fZ - v3In.fZ) <= R_EPSILON));};
    BOOL    operator!=    (const RVec3& v3In) const   {return ((fabsf (fX - v3In.fX) > R_EPSILON) || (fabsf (fY - v3In.fY) > R_EPSILON) || (fabsf (fZ - v3In.fZ) > R_EPSILON));};

    RVEC_T   LengthSquared (VOID) const                {return (fX*fX + fY*fY + fZ*fZ);};

    RVEC_T   Length        (VOID) const                {float  fSquared = LengthSquared ();
                                                               if (fSquared < 0.00000001) return 0.00000001;
                                                               return (RVEC_SQRT (fSquared));};

    RVec3&  Normalize     (VOID)                      {RVEC_T  fLength = Length (); fX /= fLength; fY /= fLength; fZ /= fLength;  return *this;};

    VOID    Zero          (VOID)                      {fX = fY = fZ = 0.0f;};

    VOID    Reverse       (VOID)                      {fX = -fX; fY = -fY; fZ = -fZ;};

    VOID    Add           (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn)               {fX += fXIn; fY += fYIn; fZ += fZIn;};

    VOID    Subtract      (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn)               {fX += fXIn; fY += fYIn; fZ += fZIn;};

    // component-wise squaring

    RVec3   Squared       (VOID) const                {return RVec3 (fX * fX, fY * fY, fZ * fZ);};

    VOID    DebugPrint    (const char *  pszLabelIn) const;

  };



//------------------------------------------------------------------------
class RVec4
//------------------------------------------------------------------------
  {
  public:

    union
      {
      RVEC_T        fX;
      RVEC_T        fR;
      RVEC_T        fU;
      };
    union
      {
      RVEC_T        fY;
      RVEC_T        fG;
      RVEC_T        fV;
      };
    union
      {
      RVEC_T        fZ;
      RVEC_T        fB;
      };
    union
      {
      RVEC_T        fW;
      RVEC_T        fA;
      };

  public:

    static  RVec4         kZero;

            RVec4        ()                           {};

            RVec4        (RVEC_T  fXIn,
                          RVEC_T  fYIn,
                          RVEC_T  fZIn,
                          RVEC_T  fWIn = 1.0)  {fX = fXIn; fY = fYIn; fZ = fZIn; fW = fWIn;};

            RVec4        (const RVec4& v4In)          {fX = v4In.fX; fY = v4In.fY; fZ = v4In.fZ; fW = v4In.fW;};

    virtual ~RVec4       ()                           {};

    RVec4&  Set           (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn = 0.0f,
                           RVEC_T  fWIn = 1.0)  {fX = fXIn; fY = fYIn; fZ = fZIn; fW = fWIn; return (*this);};

    RVec4&  Set           (const RVec4& v4In)          {fX = v4In.fX; fY = v4In.fY; fZ = v4In.fZ; fW = v4In.fW; return (*this);};

    VOID    SetPoint      (const RVec3& v3In)         {fX = v3In.fX; fY = v3In.fY; fZ = v3In.fZ; fW = 1.0f;};

    VOID    SetVector     (const RVec3& v3In)         {fX = v3In.fX; fY = v3In.fY; fZ = v3In.fZ; fW = 0.0f;};

    RVec3  Homogenize    (VOID)                       {return RVec3 (fX / fW, fY / fW, fZ / fW);};

    RVec4  operator+     (const RVec4& v4In) const   {return (RVec4 (fX + v4In.fX, fY + v4In.fY, fZ + v4In.fZ, fW));};
    RVec4  operator-     (const RVec4& v4In) const   {return (RVec4 (fX - v4In.fX, fY - v4In.fY, fZ - v4In.fZ, fW));};

    RVec4& operator+=    (const RVec4& v4In)         {fX += v4In.fX; fY += v4In.fY; fZ += v4In.fZ; return *this;};
    RVec4& operator-=    (const RVec4& v4In)         {fX -= v4In.fX; fY -= v4In.fY; fZ -= v4In.fZ; return *this;};

    // scalar multiply and divide
    RVec4  operator*     (RVEC_T  fIn) const  {return (RVec4 (fX * fIn, fY * fIn, fZ * fIn, fW));};
    RVec4  operator/     (RVEC_T  fIn) const  {return (RVec4 (fX / fIn, fY / fIn, fZ / fIn, fW));};

    RVec4& operator*=    (RVEC_T  fIn)        {fX *= fIn; fY *= fIn; fZ *= fIn; return *this;};
    RVec4& operator/=    (RVEC_T  fIn)        {fX /= fIn; fY /= fIn; fZ /= fIn; return *this;};

    // cross product (assumes W is 0, since cross product only matters for vectors)
    RVec4  operator%     (const RVec4& v4In) const   {return (RVec4 (fY*v4In.fZ - fZ*v4In.fY, fZ*v4In.fX - fX*v4In.fZ, fX*v4In.fY - fY*v4In.fX, 0.0f));};

    // dot product   (assumes W is 0, since dot product only matters for vectors)
    RVEC_T   operator*     (const RVec4& v4In) const   {return (fX*v4In.fX + fY*v4In.fY + fZ*v4In.fZ);};

    // assignment
    RVec4& operator=     (const RVec4& v4In)         {fX = v4In.fX; fY = v4In.fY; fZ = v4In.fZ; fW = v4In.fW; return *this;};
    RVec4& operator=     (const RVec3& v3In)         {fX = v3In.fX; fY = v3In.fY; fZ = v3In.fZ; fW = 0; return *this;};

    // reverse sign (unary negation) (don't reverse W, since this should only be used on vectors)
    RVec4  operator-     (VOID) const                 {return (RVec4 (-fX, -fY, -fZ, fW));};

    // equality (each value within R_EPSILON (0.001) of each other)
    BOOL    operator==    (const RVec4& v4In) const   {return ((fabsf (fX - v4In.fX) <= R_EPSILON) && (fabsf (fY - v4In.fY) <= R_EPSILON) && (fabsf (fZ - v4In.fZ) <= R_EPSILON) && (fabsf (fW - v4In.fW) <= R_EPSILON));};
    BOOL    operator!=    (const RVec4& v4In) const   {return ((fabsf (fX - v4In.fX) > R_EPSILON) || (fabsf (fY - v4In.fY) > R_EPSILON) || (fabsf (fZ - v4In.fZ) > R_EPSILON) || (fabsf (fW - v4In.fW) > R_EPSILON));};

    RVEC_T   LengthSquared (VOID) const                 {return (fX*fX + fY*fY + fZ*fZ);};

    RVEC_T   Length        (VOID) const                 {return (RVEC_SQRT (fX*fX + fY*fY + fZ*fZ));};

    RVec4&  Normalize     (VOID)                       {RVEC_T  fLength = Length (); fX /= fLength; fY /= fLength; fZ /= fLength;  return *this;};

    VOID    QuatNormalize (VOID);

    VOID    Zero          (VOID)                       {fX = fY = fZ = 0.0f; fW = 1.0f;};

    VOID    Reverse       (VOID)                       {fX = -fX; fY = -fY; fZ = -fZ;};

    VOID    Add           (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn)        {fX += fXIn; fY += fYIn; fZ += fZIn;};

    VOID    Subtract      (RVEC_T  fXIn,
                           RVEC_T  fYIn,
                           RVEC_T  fZIn)        {fX += fXIn; fY += fYIn; fZ += fZIn;};


    VOID    QuatFromAxisAngle (RVec3  vec3AxisIn,
                               RVEC_T  fAngleIn);  // angle in radians


    RVec4   QuatSLERP     (const RVec4&   qTo,
                           FLOAT          fTime) const;

    VOID    DebugPrint    (const char *  pszLabelIn) const;
  };



// functions for manipulating vectors

// find the point midway between this and another point
inline RVec3  Midpoint      (const RVec3&  v3A,
                             const RVec3&  v3B)      {return (RVec3 ((v3A.fX + v3B.fX) / 2.0f, (v3A.fY + v3B.fY) / 2.0f, (v3A.fZ + v3B.fZ) / 2.0f));};

// find the point a fractional amount (fT) of the way from this to another point.
inline RVec3  LERP          (const RVec3&  v3A,
                             const RVec3&  v3B,
                             FLOAT          fT)  {return (RVec3 (v3A.fX + (v3B.fX - v3A.fX) * fT, v3A.fY + (v3B.fY - v3A.fY) * fT, v3A.fZ + (v3B.fZ - v3A.fZ) * fT));};

inline RVec4  LERP          (const RVec4&  v4A,
                             const RVec4&  v4B,
                             FLOAT          fT)  {return (RVec4 (v4A.fX + (v4B.fX - v4A.fX) * fT, v4A.fY + (v4B.fY - v4A.fY) * fT, v4A.fZ + (v4B.fZ - v4A.fZ) * fT, v4A.fW));};


inline RVEC_T Distance        (const RVec3& v3A,
                                      const RVec3& v3B)       {RVec3  v3Diff = v3B - v3A;  return v3Diff.Length ();};

inline RVEC_T DistanceSquared (const RVec3& v3A,
                                      const RVec3& v3B)       {RVec3  v3Diff = v3B - v3A;  return v3Diff.LengthSquared ();};



#endif // RVEC_HPP


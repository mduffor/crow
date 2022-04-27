/* -----------------------------------------------------------------
                               Noise

     This module implements procedural noise.

   ----------------------------------------------------------------- */

// Noise.cpp
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

#include <math.h>
#include <stdlib.h>
#include "Gfx/Noise.hpp"
#include "Gfx/ColorOps.hpp"


// REFACTOR:  Need to replace rand() and srand() with a more reliable RNG

// Note: The following code comes from "Texturing & Modeling, a Procedural Approach",
//        editions 2 and 3.  It has been modified and formatted for my own needs.


BOOL   Noise::bInitialized = 0;


//-----------------------------------------------------------------------------
Noise::Noise ()
  {
  if (! bInitialized)
    {
    // initialize all tables that the noise functions require.  We do this at
    //   a class level, so that the Noise instances themselves can be
    //   created and deleted on the fly without a performance penalty.

    GradientTabInit (665);
    ValueTabInit    (665);
    ImpulseTabInit  (665);
    Catrom2TabInit  ();


    bInitialized = 1;
    };

  };


//-----------------------------------------------------------------------------
Noise::~Noise ()
  {
  };



// Coefficients of basis matrix
#define CR00 -0.5
#define CR01  1.5
#define CR02 -1.5
#define CR03  0.5
#define CR10  1.0
#define CR11 -2.5
#define CR12  2.0
#define CR13 -0.5
#define CR20 -0.5
#define CR21  0.0
#define CR22  0.5
#define CR23  0.0
#define CR30  0.0
#define CR31  1.0
#define CR32  0.0
#define CR33  0.0


//-----------------------------------------------------------------------------
FLOAT Noise::Spline (FLOAT x , INT nknots,  FLOAT *knot)
  {
  INT span;
  INT nspans = nknots - 3;
  FLOAT c0, c1, c2, c3; // coefficients of the cubic
  if (nspans < 1)
    {
    // illegal
    fprintf (stderr, "Spline has too few knots\n");
    return (0);
    };
  // find the appropriate 4-poINT span of the spline
  x = OpClamp (x, 0.0f, 1.0f) * nspans;
  span = (INT) x;
  if (span >= nknots - 3)
    {
    span = nknots - 3;
    };
  x -= FLOAT (span);
  knot += span;

  // evaluate the span cubic at x using Horner's rule

  c3 = FLOAT (CR00*knot[0] + CR01*knot[1] + CR02*knot[2] + CR03*knot[3]);
  c2 = FLOAT (CR10*knot[0] + CR11*knot[1] + CR12*knot[2] + CR13*knot[3]);
  c1 = FLOAT (CR20*knot[0] + CR21*knot[1] + CR22*knot[2] + CR23*knot[3]);
  c0 = FLOAT (CR30*knot[0] + CR31*knot[1] + CR32*knot[2] + CR33*knot[3]);

  return ((c3*x + c2)*x + c1)*x + c0;
  }


//-----------------------------------------------------------------------------
// Internal Tables
//-----------------------------------------------------------------------------


#define TABMASK         (TABSIZE-1)
#define PERM(x)         perm[(x)&TABMASK]
#define INDEX(ix,iy,iz) PERM((ix)+PERM((iy)+PERM(iz)))

static unsigned char perm[TABSIZE] =
  {
  225, 155, 210, 108, 175, 199, 221, 144, 203, 116,  70, 213,  69, 158,  33, 252,
    5,  82, 173, 133, 222, 139, 174,  27,   9,  71,  90, 246,  75, 130,  91, 191,
  169, 138,   2, 151, 194, 235,  81,   7,  25, 113, 228, 159, 205, 253, 134, 142,
  248,  65, 224, 217,  22, 121, 229,  63,  89, 103,  96, 104, 156,  17, 201, 129,
   36,   8, 165, 110, 237, 117, 231,  56, 132, 211, 152,  20, 181, 111, 239, 218,
  170, 163,  51, 172, 157,  47,  80, 212, 176, 250,  87,  49,  99, 242, 136, 189,
  162, 115,  44,  43, 124,  94, 150,  16, 141, 247,  32,  10, 198, 223, 255,  72,
   53, 131,  84,  57, 220, 197,  58,  50, 208,  11, 241,  28,   3, 192,  62, 202,
   18, 215, 153,  24,  76,  41,  15, 179,  39,  46,  55,   6, 128, 167,  23, 188,
  106,  34, 187, 140, 164,  73, 112, 182, 244, 195, 227,  13,  35,  77, 196, 185,
   26, 200, 226, 119,  31, 123, 168, 125, 249,  68, 183, 230, 177, 135, 160, 180,
   12,   1, 243, 148, 102, 166,  38, 238, 251,  37, 240, 126,  64,  74, 161,  40,
  184, 149, 171, 178, 101,  66,  29,  59, 146,  61, 254, 107,  42,  86, 154,   4,
  236, 232, 120,  21, 233, 209,  45,  98, 193, 114,  78,  19, 206,  14, 118, 127,
   48,  79, 147,  85,  30, 207, 219,  54,  88, 234, 190, 122,  95,  67, 143, 109,
  137, 214, 145,  93,  92, 100, 245,   0, 216, 186,  60,  83, 105,  97, 204,  52
  };

// Value Noise


FLOAT Noise::valueTab    [TABSIZE];
FLOAT Noise::gradientTab [TABSIZE*3];
FLOAT Noise::impulseTab  [TABSIZE*4];
FLOAT Noise::CatRomTable [CATROMNENTRIES];


//-----------------------------------------------------------------------------
VOID Noise::ValueTabInit (INT seed)
  {
  FLOAT * table = valueTab;
  INT i;
  srand (seed);
  for (i = 0; i < TABSIZE; i++)
    {
    *table++ = 1.0f - 2.0f * RANDNBR;
    };
  };


//-----------------------------------------------------------------------------
FLOAT Noise::VLattice (INT ix, INT iy, INT iz)
  {
  // Note:  Check this function against the original text.  INDEX is returning
  //         a value between 0 and 255*3, not 0 and 255.
  return valueTab[INDEX(ix, iy, iz) & TABMASK];
  };

//-----------------------------------------------------------------------------
FLOAT Noise::VNoise (FLOAT x, FLOAT y, FLOAT z)
  {
  FLOAT  fix, fiy, fiz;
  INT    ix, iy, iz;
  INT    i, j, k;
  FLOAT  fx, fy, fz;
  FLOAT xknots[4], yknots[4], zknots[4];

  fix = OpFloor(x);
  fx = x - fix;
  ix = INT (fix);

  fiy = OpFloor(y);
  fy = y - fiy;
  iy = INT (fiy);

  fiz = OpFloor(z);
  fz = z - fiz;
  iz = INT (fiz);

  for (k = -1; k <= 2; k++)
    {
    for (j = -1; j <= 2; j++)
      {
      for (i = -1; i <= 2; i++)
        {
        xknots[i+1] = VLattice(ix+i, iy+j, iz+k);
        };
      yknots[j+1] = Spline (fx, 4, xknots);
      };
    zknots[k+1] = Spline (fy, 4, yknots);
    };
  return (Spline (fz, 4, zknots));
  };


//-----------------------------------------------------------------------------
// Gradient Noises
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
VOID Noise::GradientTabInit (INT seed)
  {
  FLOAT * table = gradientTab;
  INT i;

  srand(seed);
  for (i = 0; i < TABSIZE; ++i)
    {
    FLOAT z, r, theta;

    z = 1.0f - 2.0f*RANDNBR;
    // r is the radius of x,y, circle
    r = sqrtf(1 - z*z);
    // theta is angle in (x,y)
    theta = 2.0f * M_PI * RANDNBR;
    *table++ = r * cosf(theta);
    *table++ = r * sinf(theta);
    *table++ = z;
    };
  };


//-----------------------------------------------------------------------------
FLOAT Noise::GLattice (INT ix, INT iy, INT iz, FLOAT fx, FLOAT fy, FLOAT fz)
  {
  FLOAT *  g = &gradientTab[INDEX(ix,iy,iz)*3];
  return (g[0]*fx + g[1]*fy + g[2]*fz);
  };


//-----------------------------------------------------------------------------
FLOAT Noise::GNoise (FLOAT x, FLOAT y, FLOAT z)
  {
  INT ix, iy, iz;
  FLOAT fx0, fx1, fy0, fy1, fz0, fz1;
  FLOAT wx, wy, wz;
  FLOAT vx0, vx1, vy0, vy1, vz0, vz1;

  ix = OpFloor(x);
  fx0 = x - ix;
  fx1 = fx0 - 1;
  wx = OpSmoothStep (fx0);

  iy = OpFloor(y);
  fy0 = y - iy;
  fy1 = fy0 - 1;
  wy = OpSmoothStep (fy0);

  iz = OpFloor(z);
  fz0 = z - iz;
  fz1 = fz0 - 1;
  wz = OpSmoothStep (fz0);

  vx0 = GLattice (ix, iy, iz, fx0, fy0, fz0);
  vx1 = GLattice (ix+1, iy, iz, fx1, fy0, fz0);
  vy0 = OpLerp (vx0, vx1, wx);

  vx0 = GLattice (ix, iy+1, iz, fx0, fy1, fz0);
  vx1 = GLattice (ix+1, iy+1, iz, fx1, fy1, fz0);
  vy1 = OpLerp (vx0, vx1, wx);
  vz0 = OpLerp (vy0, vy1, wy);

  vx0 = GLattice (ix, iy, iz+1, fx0, fy0, fz1);
  vx1 = GLattice (ix+1, iy, iz+1, fx1, fy0, fz1);
  vy0 = OpLerp (vx0, vx1, wx);


  vx0 = GLattice (ix, iy+1, iz+1, fx0, fy1, fz1);
  vx1 = GLattice (ix+1, iy+1, iz+1, fx1, fy1, fz1);
  vy1 = OpLerp (vx0, vx1, wx);
  vz1 = OpLerp (vy0, vy1, wy);

  return (OpLerp (vz0, vz1, wz));
  };

//-----------------------------------------------------------------------------
// Value-Gradient Noises
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// lattice convolution noise


//-----------------------------------------------------------------------------
FLOAT Noise::Catrom2 (FLOAT d)
  {
  INT i;
  if (d >= 4) return 0;

  d = d * CATROMSAMPRATE + 0.5f;
  i = OpFloor(d);
  if (i >= CATROMNENTRIES) return 0;
  return (CatRomTable[i]);
  };


//-----------------------------------------------------------------------------
VOID Noise::Catrom2TabInit (VOID)
  {
  INT i;

  for (i = 0; i < CATROMNENTRIES; ++i)
    {
    FLOAT x;
    x = i/(FLOAT) CATROMSAMPRATE;
    x = sqrtf(x);
    if (x < 1)
      {
      CatRomTable[i] = 0.5f * (2.0f+x*x*(-5.0f+x*3.0f));
      }
    else
      {
      CatRomTable[i] = 0.5f * (4.0f+x*(-8.0f+x*(5.0f-x)));
      };
    };
  };


//-----------------------------------------------------------------------------
FLOAT Noise::VCNoise (FLOAT x, FLOAT y, FLOAT z)
  {
  INT ix, iy, iz;
  INT i, j, k;
  FLOAT fx, fy, fz;
  FLOAT sum = 0.0;

  ix = OpFloor(x);
  fx = x - ix;
  iy = OpFloor(y);
  fy = y - iy;
  iz = OpFloor(z);
  fz = z - iz;

  for (k = -1; k <= 2; k++)
    {
    FLOAT dz;

    dz = k - fz;
    dz = dz * dz;
    for (j = -1; j <= 2; j++)
      {
      FLOAT dy;

      dy = j - fy;
      dy = dy * dy;
      for (i = -1; i <= 2; i++)
        {
        FLOAT dx;

        dx = i - fx;
        dx = dx * dx;
        sum += VLattice (ix+i, iy+j, iz+k) * Catrom2 (dx + dy + dz);
        };
      };
    };
  return (sum);
  };

//-----------------------------------------------------------------------------
// sparse convolution noise


//-----------------------------------------------------------------------------
VOID Noise::ImpulseTabInit (INT seed)
  {
  INT i;
  FLOAT *  f = impulseTab;
  srand(seed); // set random number generator seed
  for (i = 0; i < TABSIZE; i++)
    {
    *f++ = FLOAT (RANDNBR);
    *f++ = FLOAT (RANDNBR);
    *f++ = FLOAT (RANDNBR);
    *f++ = FLOAT (1.0 - 2.0 * RANDNBR);
    };
  };


#define NEXT(h) (((h)+1) & TABMASK)
#define NIMPULSES 3

//-----------------------------------------------------------------------------
FLOAT Noise::SCNoise (FLOAT x, FLOAT y, FLOAT z)
  {
  FLOAT *  fp;
  INT i, j, k, h, n;
  INT ix, iy, iz;
  FLOAT sum = 0;
  FLOAT fx, fy, fz, dx, dy, dz, distsq;

  ix = OpFloor(x); fx = x - ix;
  iy = OpFloor(y); fy = y - iy;
  iz = OpFloor(z); fz = z - iz;

  // perform the sparse convolution
  for (i = -2; i <= 2; ++i)
    {
    for (j = -2; j <= 2; ++j)
      {
      for (k = -2; k <= 2; ++k)
        {
        // compute voxel hash code
        h = INDEX (ix+i, iy+j, iz+k);
        for (n = NIMPULSES; n > 0; n--, h = NEXT(h))
          {
          // convolve filter and impulse

          fp = &impulseTab[h*4];
          dx = fx - (i + *fp++);
          dy = fy - (j + *fp++);
          dz = fz - (k + *fp++);
          distsq = dx*dx + dy*dy + dz*dz;
          sum += Catrom2(distsq) * *fp;
          };
        };
      };
    };

  return (sum / NIMPULSES);
  };




/*
// use OpLerp instead
color mix (color C0, color C1, FLOAT f)
  {
  return (1-f)*C0 + f*C1;
  };

FLOAT gammacorrect (FLOAT gamma, FLOAT x)
  {
  return pow (x, 1/gamma);
  };

FLOAT bias (FLOAT b, FLOAT x)
  {
  return (pow (x, log(b)/log(0.5));
  };

FLOAT gain (FLOAT g, FLOAT x)
  {
  if (x < 0.5)
    return bias (1-g, 2*x)/2;
  else
    return 1 - bias (1-g, 2 - 2*x)/2;
  };

*/

















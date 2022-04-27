/* -----------------------------------------------------------------
                               Noise

     This module implements procedural noise.

   ----------------------------------------------------------------- */

// Noise.hpp
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


#ifndef NOISE_HPP
#define NOISE_HPP

#include <stdlib.h>
#include "Sys/Types.hpp"

// REFACTOR: Need a better RNG than rand()

#define TABSIZE         256
#define CATROMSAMPRATE 100 // table entries per unit distance
#define CATROMNENTRIES (4*CATROMSAMPRATE+1)

// RANDNBR generates a number between 0.0 and 1.0
// RANDSNBR generates a number between -1.0 and 1.0

// TODO: Write a better RNG
#define RANDMASK 0x7fffffff
#define RANDMASKHALF (0x7fffffff / 2)
#define RANDNBR ((rand() & RANDMASK)/(double) RANDMASK)
#define RANDSNBR ((double (rand() & RANDMASK)/double (RANDMASKHALF)) - double (1.0f))

//-----------------------------------------------------------------------------
class Noise
  {
  private:

    static FLOAT  valueTab    [TABSIZE];
    static FLOAT  gradientTab [TABSIZE*3];
    static FLOAT  impulseTab  [TABSIZE*4];
    static FLOAT  CatRomTable [CATROMNENTRIES];

    static BOOL   bInitialized;

  protected:

    VOID    ValueTabInit     (INT seed);

    VOID    GradientTabInit  (INT seed);

    VOID    ImpulseTabInit   (INT seed);

    VOID    Catrom2TabInit   (VOID);


  public:

            Noise            ();

            ~Noise           ();

    FLOAT   Spline           (FLOAT    x ,
                              INT      nknots,
                              FLOAT *  knot);

    FLOAT   Catrom2          (FLOAT d);

    FLOAT   VLattice         (INT ix,
                              INT iy,
                              INT iz);

    FLOAT   GLattice         (INT   ix,
                              INT   iy,
                              INT   iz,
                              FLOAT fx,
                              FLOAT fy,
                              FLOAT fz);

                             /** Value Noise
                             */
    FLOAT   VNoise           (FLOAT x,
                              FLOAT y,
                              FLOAT z);

                             /** Gradient Noise
                             */
    FLOAT   GNoise           (FLOAT x,
                              FLOAT y,
                              FLOAT z);

                             /** Latice-Convolution Noise
                             */
    FLOAT   VCNoise          (FLOAT x,
                              FLOAT y,
                              FLOAT z);

                             /** Sparse-Convolution Noise
                             */
    FLOAT   SCNoise          (FLOAT x,
                              FLOAT y,
                              FLOAT z);



  };


#endif // NOISE_HPP

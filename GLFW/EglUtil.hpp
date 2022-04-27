/* -----------------------------------------------------------------
                         EGL Util

     This module implements helper functions for using OpengGL ES/EGL

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2016, Michael T. Duffy II.  All rights reserved.
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

#ifndef EGLUTIL_H
#define EGLUTIL_H

#include "Sys/Types.hpp"

//#include <EGL/egl.h>
//#include <EGL/eglext.h>
//#include <GLES3/gl3.h>
//#include <GLES3/gl3ext.h>



#include "Gfx/GLUtil.hpp"
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>


// NOTE: Reduced Latency used in ovrRenderer
#define REDUCED_LATENCY      0
#define EXPLICIT_GL_OBJECTS  0



//------------------------------------------------------------------------------
class EglUtil
  {
  private:

    GLFWwindow *  pWindow;

    int           iMainWidth;
    int           iMainHeight;

  public:

  public:
                  EglUtil         ();

                  ~EglUtil        ();

    GLFWwindow *  CreateContext         (const EglUtil *  shareEgl,
                                         UINT             uWidthIn,
                                         UINT             uHeightIn);

    void          DestroyContext        (void);

    void          CreateSurface         (void);

    void          DestroySurface        (void);

    void          SwapBuffers           (void);

    bool          HasMainSurface        (void)  {return (pWindow != NULL);};

    int           GetWidth              (void);

    int           GetHeight             (void);

  };


#endif // EGLUTIL_H

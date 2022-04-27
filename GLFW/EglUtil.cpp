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

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "GLFW/EglUtil.hpp"


//=============================================================================
//  EglUtil
//=============================================================================

//-----------------------------------------------------------------------------
EglUtil::EglUtil ()
  {
  pWindow = NULL;

  iMainWidth = 0;
  iMainHeight = 0;
  }

//-----------------------------------------------------------------------------
EglUtil::~EglUtil ()
  {
  }

//-----------------------------------------------------------------------------
GLFWwindow * EglUtil::CreateContext (const EglUtil *  shareEgl,
                                     UINT             uWidthIn,
                                     UINT             uHeightIn)
  {
  // TODO:  Need to figure out how to request an OpenGL ES2/ES3 context

  // TODO: Figure out how to request a 16 bit depth buffer.

  // Create a windowed mode window and its OpenGL context
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  // TODO: If shareEgl is not null, use the shareEgl window when creating
  //   the new window so they share contexts/resources


  pWindow = glfwCreateWindow (uWidthIn, uHeightIn, "Hello World", NULL, NULL);
  //pWindow = glfwCreateWindow (576, 1024, "Hello World", NULL, NULL);
  if (!pWindow)
    {
    glfwTerminate();
    return NULL;
    }

  // Make the window's context current
  glfwMakeContextCurrent (pWindow);
  glfwGetFramebufferSize (pWindow, &iMainWidth, &iMainHeight);

  DBG_INFO ("Framebuffer created at size %d x %d", iMainWidth, iMainHeight);

  // if needed, load GLFW/GL extensions here.


  return (pWindow);
  }

//-----------------------------------------------------------------------------
void EglUtil::DestroyContext ()
  {
  glfwDestroyWindow (pWindow);
  }

//-----------------------------------------------------------------------------
void EglUtil::CreateSurface ()
  {
  // TODO:  Determine if this needs to be implemented
  }

//-----------------------------------------------------------------------------
void EglUtil::DestroySurface ()
  {
  // TODO:  Determine if this needs to be implemented
  }

//-----------------------------------------------------------------------------
void EglUtil::SwapBuffers (void)
  {
  //DBG_INFO ("EGL: eglSwapBuffers");
  //DBG_INFO ("");

  glfwSwapBuffers (pWindow);
  };

//-----------------------------------------------------------------------------
int  EglUtil::GetWidth (void)
  {
  glfwGetFramebufferSize (pWindow, &iMainWidth, &iMainHeight);
  return iMainWidth;
  };

//-----------------------------------------------------------------------------
int  EglUtil::GetHeight (void)
  {
  glfwGetFramebufferSize (pWindow, &iMainWidth, &iMainHeight);
  return iMainHeight;
  };


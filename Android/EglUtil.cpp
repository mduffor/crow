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

#if defined( ANDROID )

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "Android/EglUtil.hpp"


//=============================================================================
//  EglUtil
//=============================================================================

//-----------------------------------------------------------------------------
EglUtil::EglUtil ()
  {
  MajorVersion = 0;
  MinorVersion = 0;
  Display = 0;
  Config = 0;
  TinySurface = EGL_NO_SURFACE;
  MainSurface = EGL_NO_SURFACE;
  Context = EGL_NO_CONTEXT;
  }

//-----------------------------------------------------------------------------
EglUtil::~EglUtil ()
  {
  }

//-----------------------------------------------------------------------------
void EglUtil::CreateContext (const EglUtil *       shareEgl,
                             struct android_app *  app)
  {
  // return if we've already read the display and are thus initialized.
  if ( Display != 0 )
    {
    return;
    }

  Display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
  DBG_INFO ("        eglInitialize (Display, &MajorVersion, &MinorVersion)");
  eglInitialize (Display, &MajorVersion, &MinorVersion);

  #ifdef GEARVR
    CreateContextGearVR (shareEgl);
  #else
    CreateContextAndroid (shareEgl, app);
  #endif
  }

//-----------------------------------------------------------------------------
void EglUtil::CreateContextAndroid (const EglUtil *       shareEgl,
                                    struct android_app *  app)
  {
  // Specify the attributes of the desired configuration.

  if (Context != EGL_NO_CONTEXT) return;

  const EGLint configAttribs[] =
    {
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_DEPTH_SIZE, 16,
    EGL_NONE
    };
  const EGLint configAttribsNoDepth[] =
    {
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE
    };

  const EGLint contextAttribs[] =
    {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
    };

  Config = ChooseConfig (configAttribs,
                         EGL_OPENGL_ES2_BIT,
                         EGL_WINDOW_BIT); // compatible with the device's screen


  if (Config == 0)
    {
    // If there is no config that matches our desired bit depth, find one without
    //  asking about bit depth.  This step is needed for some phones, such as the
    //  Galaxy S6.
    Config = ChooseConfig (configAttribsNoDepth,
                           EGL_OPENGL_ES2_BIT,
                           EGL_WINDOW_BIT); // compatible with the device's screen

    };

  if (Config == 0)
    {
    return;
    };

  /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
  EGLint  iFormat;
  eglGetConfigAttrib (Display, Config, EGL_NATIVE_VISUAL_ID, &iFormat);

  //DBG_INFO ("explore ANativeWindow_setBuffersGeometry %d %d %d", app, app->window, iFormat);

  ANativeWindow_setBuffersGeometry (app->window, 0, 0, iFormat);

  Context = eglCreateContext (Display,
                              Config,
                              (shareEgl != NULL) ? shareEgl->Context : EGL_NO_CONTEXT, contextAttribs);
  }

//-----------------------------------------------------------------------------
void EglUtil::CreateContextGearVR (const EglUtil * shareEgl)
  {
  const EGLint configAttribs[] =
    {
    EGL_ALPHA_SIZE, 8, // need alpha for the multi-pass timewarp compositor
    EGL_BLUE_SIZE,  8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE,   8,
    EGL_DEPTH_SIZE, 0,
    EGL_SAMPLES,  0,
    EGL_NONE
    };

  Config = ChooseConfig (configAttribs,
                         OPENGL_ES3_BIT_KHR,
                         EGL_WINDOW_BIT | EGL_PBUFFER_BIT);

  if (Config == 0)
    {
    return;
    };


  // Create the context
  EGLint contextAttribs[] =
    {
    EGL_CONTEXT_CLIENT_VERSION, 3,
    EGL_NONE
    };
  Context = eglCreateContext (Display,
                              Config,
                              (shareEgl != NULL) ? shareEgl->Context : EGL_NO_CONTEXT, contextAttribs);
  if ( Context == EGL_NO_CONTEXT )
    {
    DBG_ERROR( "        eglCreateContext(Display, Config, EGL_NO_CONTEXT, contextAttribs) failed: %s", EglErrorString( eglGetError() ) );
    return;
    }

  // Create the PBuffer and make it current.
  const EGLint surfaceAttribs[] =
    {
    EGL_WIDTH, 16,
    EGL_HEIGHT, 16,
    EGL_NONE
    };
  TinySurface = eglCreatePbufferSurface( Display, Config, surfaceAttribs );
  if ( TinySurface == EGL_NO_SURFACE )
    {
    DBG_ERROR( "        TinySurface = eglCreatePbufferSurface(Display, Config, surfaceAttribs) failed: %s", EglErrorString( eglGetError() ) );
    eglDestroyContext( Display, Context );
    Context = EGL_NO_CONTEXT;
    return;
    }
  if ( eglMakeCurrent( Display, TinySurface, TinySurface, Context ) == EGL_FALSE )
    {
    DBG_ERROR( "        eglMakeCurrent(Display, TinySurface, TinySurface, Context) failed: %s", EglErrorString( eglGetError() ) );
    eglDestroySurface( Display, TinySurface );
    eglDestroyContext( Display, Context );
    Context = EGL_NO_CONTEXT;
    return;
    }
  }

//-----------------------------------------------------------------------------
EGLConfig EglUtil::ChooseConfig (const EGLint * pConfigAttribs,
                                 const EGLint   iRenderableTypeFlags,
                                 const EGLint   iSurfaceTypeFlags)
  {
  // This function serves a similar purpose to eglChooseConfig, but gives us
  //   a bit more control over which config is chosen.

  // For Oculus Mobile, we don't use eglChooseConfig because the android EGL
  // code pushes in multisample flags if the user selected the "force 4x MSAA"
  // option in settings, and that is unneeded overhead for the warp target.

  EGLConfig   configOut = 0;

  const int MAX_CONFIGS = 1024;
  EGLConfig aConfigs [MAX_CONFIGS];
  EGLint iNumConfigs = 0;
  if (eglGetConfigs (Display, aConfigs, MAX_CONFIGS, &iNumConfigs) == EGL_FALSE)
    {
    DBG_ERROR ("        eglGetConfigs() failed: %s", EglErrorString (eglGetError()));
    return (0);
    }

  DBG_INFO ("Found %d OPENGLES configs", iNumConfigs);
  for (int iConfigIndex = 0; iConfigIndex < iNumConfigs; iConfigIndex++)
    {
    EGLint iValue = 0;

    // make sure this config supports OpenGL ES 3.x
    eglGetConfigAttrib (Display, aConfigs[iConfigIndex], EGL_RENDERABLE_TYPE, &iValue);
    if ((iValue & iRenderableTypeFlags) != iRenderableTypeFlags)
      {
      continue;
      }

    // The pbuffer config also needs to be compatible with normal window rendering
    // so it can share textures with the window context.
    eglGetConfigAttrib (Display, aConfigs[iConfigIndex], EGL_SURFACE_TYPE, &iValue);
    if ((iValue & iSurfaceTypeFlags) != iSurfaceTypeFlags)
      {
      continue;
      }

    int   iAttribIndex = 0;
    BOOL  bFoundConfig = TRUE;
    int   iR = 0;
    int   iG = 0;
    int   iB = 0;
    int   iD = 0;

    for ( ; pConfigAttribs[iAttribIndex] != EGL_NONE; iAttribIndex += 2)
      {
      eglGetConfigAttrib (Display, aConfigs[iConfigIndex], pConfigAttribs[iAttribIndex], &iValue);
      if (iValue != pConfigAttribs[iAttribIndex + 1])
        {
        bFoundConfig = FALSE;
        }

      if (pConfigAttribs[iAttribIndex] == EGL_BLUE_SIZE)  iB = iValue;
      if (pConfigAttribs[iAttribIndex] == EGL_GREEN_SIZE) iG = iValue;
      if (pConfigAttribs[iAttribIndex] == EGL_RED_SIZE)   iR = iValue;
      if (pConfigAttribs[iAttribIndex] == EGL_DEPTH_SIZE) iD = iValue;
      }

    DBG_INFO ("Config %d is version and window compatible. R %d G %d B %d D %d", iConfigIndex, iR, iG, iB, iD);

    if (!bFoundConfig) continue;

    if (pConfigAttribs[iAttribIndex] == EGL_NONE)
      {
      configOut = aConfigs[iConfigIndex];
      break;
      }
    }
  if (configOut == 0)
    {
    DBG_ERROR ("        eglChooseConfig() failed: %s", EglErrorString (eglGetError()));
    }
  return (configOut);
  };

//-----------------------------------------------------------------------------
void EglUtil::DestroyContext ()
  {
  // tear down the EGL context currently associated with the display.

  if (Display != EGL_NO_DISPLAY)
    {
    EGL( eglMakeCurrent (Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) );
    }
  if (Context != EGL_NO_CONTEXT)
    {
    EGL( eglDestroyContext (Display, Context) );
    Context = EGL_NO_CONTEXT;
    }
  if (TinySurface != EGL_NO_SURFACE)
    {
    EGL( eglDestroySurface (Display, TinySurface) );
    TinySurface = EGL_NO_SURFACE;
    }
  if (MainSurface != EGL_NO_SURFACE)
    {
    EGL( eglDestroySurface (Display, MainSurface) );
    MainSurface = EGL_NO_SURFACE;
    }
  if (Display != 0)
    {
    EGL ( eglTerminate (Display) );
    Display = 0;
    }
  }

//-----------------------------------------------------------------------------
void EglUtil::CreateSurface (ANativeWindow * nativeWindow)
  {
  // NOTE:  This needs to be called again on resume.

  if ( MainSurface != EGL_NO_SURFACE )
    {
    return;
    }
  const EGLint surfaceAttribs[] = {EGL_NONE};
  MainSurface = eglCreateWindowSurface (Display, Config, nativeWindow, surfaceAttribs);
  if (MainSurface == EGL_NO_SURFACE)
    {
    DBG_ERROR( "        eglCreateWindowSurface (Display, Config, nativeWindow, attribs) failed: %s", EglErrorString( eglGetError() ) );
    return;
    }

  #if EXPLICIT_GL_OBJECTS == 0
  DBG_INFO( "        eglMakeCurrent( display, MainSurface, MainSurface, Context )" );
  EGL( eglMakeCurrent (Display, MainSurface, MainSurface, Context) );
  #endif

  EGLint w, h;
  eglQuerySurface (Display, MainSurface, EGL_WIDTH,  &w);
  eglQuerySurface (Display, MainSurface, EGL_HEIGHT, &h);

  iMainWidth  = w;
  iMainHeight = h;

    DBG_INFO("OpenGL Surface Width: %d Height: %d", w, h);

    // Check openGL on the system
    //EGLint opengl_info [] = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
    //const char * opengl_info_names [] = {"Vendor", "Renderer", "Version", "Extensions"};
    //for (int index = 0; index < 4; ++index)
    //  {
    //  DBG_INFO("OpenGL Info: %s: %s", opengl_info_names[index], glGetString(opengl_info[index]));
    //  }


  }

//-----------------------------------------------------------------------------
void EglUtil::DestroySurface ()
  {
  #if EXPLICIT_GL_OBJECTS == 0
  if ( Context != EGL_NO_CONTEXT )
    {
    EGL( eglMakeCurrent (Display, TinySurface, TinySurface, Context) );
    }
  #endif
  if ( MainSurface != EGL_NO_SURFACE )
    {
    EGL( eglDestroySurface (Display, MainSurface) );
    MainSurface = EGL_NO_SURFACE;
    }
  }

//-----------------------------------------------------------------------------
void EglUtil::SwapBuffers (void)
  {
  //DBG_INFO ("EGL: eglSwapBuffers");
  //DBG_INFO ("");
  eglSwapBuffers (Display, MainSurface);
  };

#endif // ANDROID
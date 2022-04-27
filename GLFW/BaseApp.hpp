/* -----------------------------------------------------------------
                          Base App

     This module is the base class for the display type.  It is 
     called from nativeActivity, which interfaces with Android, and 
     it calls SceneDelegate, which wraps the actual game world.  
     The derived children of this class are meant to hold the 
     GearVR, Daydream, or Standard Android / OpenGL specific code.  

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

#ifndef BASEAPP_HPP
#define BASEAPP_HPP

#include "Sys/Types.hpp"
#include "Scene/SceneDelegate.hpp"
#include "GLFW/EglUtil.hpp"


//------------------------------------------------------------------------------
class BaseApp
  {

  public:
    GLFWwindow*         pWindow;
    
    long long           FrameIndex;
    bool                bActive;
    
    SceneDelegate *     pSceneDelegate;
    
  public:
                 BaseApp             (SceneDelegate *  pSceneIn);

    virtual      ~BaseApp            () {};

    virtual void Init         (void) = 0;
         
    virtual void Shutdown             (void) = 0;
    
    virtual GLFWwindow * InitGraphics (void) = 0;

    virtual void ShutdownGraphics    () = 0;
    
    virtual void HandleVrModeChanges (void) = 0;

    virtual void InitShell           (const char *     szFilePath) = 0;

    virtual void UninitShell         (void) = 0;

    void         SetActive           (bool  bActiveIn)  {bActive = bActiveIn;};
    
    virtual bool IsActive            (void) = 0;

    virtual void HandleKeyEvent      (const int  keyCode,
                                      const int  action) = 0;

    virtual void HandleTouchEvent    (const int    action,
                                      const float  x,
                                      const float  y,
                                      const float  pressure) = 0;

    virtual void HandleAxisEvent     (const int    index,
                                      const float  value) = 0;


    virtual void HandleMouseKeyEvent (int iButton, 
                                      int iAction, 
                                      int iMods) = 0;

    virtual void HandleMouseMoveEvent (float  fXIn,
                                       float  fYIn) = 0;
                                      
    virtual void Update              (void) = 0;

    virtual void PollForJoysticks    (void) = 0;
    
  };


#endif // BASEAPP_HPP
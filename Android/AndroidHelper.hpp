/* -----------------------------------------------------------------
                       Android Helper

     This module implements helper functions for interfacing with
   Android NDK apps.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2015, Michael T. Duffy II.  All rights reserved.
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

#ifndef ANDROIDHELPER_HPP
#define ANDROIDHELPER_HPP

#if defined( ANDROID )

#include "Sys/Types.hpp"
#include <stdbool.h>
#include <android_native_app_glue.h>
#include <android/native_activity.h>

//-----------------------------------------------------------------------------
// Java
//-----------------------------------------------------------------------------

#if defined( ANDROID )
  #include <jni.h>
#elif defined( __cplusplus )
  typedef struct _JNIEnv JNIEnv;
  typedef struct _JavaVM JavaVM;
  typedef class _jobject * jobject;
#else
  typedef const struct JNINativeInterface * JNIEnv;
  typedef const struct JNIInvokeInterface * JavaVM;
  void * jobject;
#endif

typedef struct
  {
  JavaVM *  Vm;               // Java Virtual Machine
  JNIEnv *  Env;              // Thread specific environment
  jobject   ActivityObject;   // Java activity object
  } TJava;



//-----------------------------------------------------------------------------
class AndroidHelper
  {
  public:

    static TJava                 java;
    static struct android_app *  app;

  public:
                  AndroidHelper  () {};

                  ~AndroidHelper () {};

    static void   Init          (TJava                 javaIn,
                                 struct android_app *  appIn)  {java = javaIn; app = appIn;};

    static INT    TranslateVKey  (INT  iKeyCodeIn);

    static jclass RetrieveClass  (JNIEnv *     jniEnvIn,
                                  const char * szClassNameIn);

    static VOID   GetJString     (JNIEnv *  jniEnvIn,
                                  jstring & jstrIn,
                                  RStr &    strOut);

  };

#endif // ANDROID
#endif // ANDROIDHELPER_HPP
/* -----------------------------------------------------------------
                           Test TestOpenGLES2

     This is a simple class for drawing a triangle in OpenGL ES2.
   It is for testing the graphics card setup before getting into
   the complexities of a full scene.

                            Michael T. Duffy
   ----------------------------------------------------------------- */

// TestOpenGLES3.hpp
// Copyright 2017, Michael T. Duffy.  All Rights Reserved

#ifndef TESTOPENGLES2_HPP
#define TESTOPENGLES2_HPP


//------------------------------------------------------------------------------
class TestOpenGLES2
//------------------------------------------------------------------------------
  {
  public:
    
  
  public:
    TestOpenGLES2  () {};
    ~TestOpenGLES2 () {};

    static void Init ();
    static void Draw (BOOL  bClearBG);
  };

#endif // TESTOPENGLES2_HPP

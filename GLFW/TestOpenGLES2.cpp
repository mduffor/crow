#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Gfx/GLUtil.hpp"
#include "TestOpenGLES2.hpp"


/* This file is for testing shaders and working out GL-level issues directly
 * before they are wrapped up in the framework itself
 */

GLuint g_ShaderProgram;
GLuint gvPositionHandle;
  
static const char gVertexShader[] = 
    "#version 300 es\n"
    "attribute vec4 a_position;\n"
    "void main() {\n"
    "  gl_Position = a_position;\n"
    "}\n";

static const char gFragmentShader[] = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" // R G B A
    "}\n";



//------------------------------------------------------------------------------
void TestOpenGLES2::Init ()
  {
  // Create the shader
  g_ShaderProgram = GLCreateProgram (gVertexShader, gFragmentShader);
  if (!g_ShaderProgram) 
    {
    DBG_ERROR("Could not create program.");
    return;
    }
  glUseProgram(g_ShaderProgram);

  // Get the vertex position attribute from the shader.
  gvPositionHandle = glGetAttribLocation(g_ShaderProgram, "a_position");
  GLCheckError (__FILE__, "glGetAttribLocation");
  DBG_INFO ("glGetAttribLocation(\"a_position\") = %d\n", gvPositionHandle);
  
  //glEnableClientState(GL_VERTEX_ARRAY); // This is not needed in ES2.  It is an ES1 thing.
  };

//------------------------------------------------------------------------------
void TestOpenGLES2::Draw (BOOL  bClearBG)
  {
  DBG_INFO ("Draw!!!!!!!!!!!!!");
  if (bClearBG) 
    {
    GL( glClearColor(.1f, .2f, .5f, 1.0f) );
    GL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    };
  
  // draw a (red) triangle in the middle of the screen.
  
  static const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, 0.0f, -0.5f, 0.5f, 0.0f };

  GL (glUseProgram(g_ShaderProgram));
  
  glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
  glEnableVertexAttribArray(gvPositionHandle);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  };


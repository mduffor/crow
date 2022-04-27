
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Gfx/GLUtil.hpp"

GLCallProxyLive GLCallProxyLive::sInstance;
GLCallProxyMock GLCallProxyMock::sInstance;

#ifdef GLES
  //------------------------------------------------------------------------------
  void GLPrintString (const char *name, GLenum s)
    {
    const char *v = (const char *) glGetString(s);
    DBG_RAW ("GL %s = %s\n", name, v);
    }
#endif


#ifdef EGLAPI
  //-----------------------------------------------------------------------------
  const char * EglErrorString (const EGLint  iErrorIn)
    {
    switch (iErrorIn)
      {
      case EGL_SUCCESS:              return "EGL_SUCCESS";
      case EGL_NOT_INITIALIZED:      return "EGL_NOT_INITIALIZED";
      case EGL_BAD_ACCESS:           return "EGL_BAD_ACCESS";
      case EGL_BAD_ALLOC:            return "EGL_BAD_ALLOC";
      case EGL_BAD_ATTRIBUTE:        return "EGL_BAD_ATTRIBUTE";
      case EGL_BAD_CONTEXT:          return "EGL_BAD_CONTEXT";
      case EGL_BAD_CONFIG:           return "EGL_BAD_CONFIG";
      case EGL_BAD_CURRENT_SURFACE:  return "EGL_BAD_CURRENT_SURFACE";
      case EGL_BAD_DISPLAY:          return "EGL_BAD_DISPLAY";
      case EGL_BAD_SURFACE:          return "EGL_BAD_SURFACE";
      case EGL_BAD_MATCH:            return "EGL_BAD_MATCH";
      case EGL_BAD_PARAMETER:        return "EGL_BAD_PARAMETER";
      case EGL_BAD_NATIVE_PIXMAP:    return "EGL_BAD_NATIVE_PIXMAP";
      case EGL_BAD_NATIVE_WINDOW:    return "EGL_BAD_NATIVE_WINDOW";
      case EGL_CONTEXT_LOST:         return "EGL_CONTEXT_LOST";
      default:                       return "unknown";
      }
    }
#endif

//-----------------------------------------------------------------------------
const char * GLErrorString (GLenum eError)
  {
  switch (eError)
    {
    case GL_NO_ERROR:                       return "GL_NO_ERROR";
    case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
    default:                                return "unknown";
    }
  }

//------------------------------------------------------------------------------
void GLCheckError (const char * filename, const char* op)
  {
  #ifdef GLES
    for (GLint error = glGetError(); error; error = glGetError())
      {
      DBG_RAW ("File %s : after %s() glError (0x%x) %s\n", filename, op, error, GLErrorString(error));
      }
  #endif
  }

//------------------------------------------------------------------------------
void GLCheckError (const char * filename, int line, const char* op)
  {
  #ifdef GLES
    for (GLint error = glGetError(); error; error = glGetError())
      {
      DBG_RAW ("File %s (%d) : after %s() glError (0x%x) %s\n", filename, line, op, error, GLErrorString(error));
      }
  #endif
  }

//------------------------------------------------------------------------------
void GLPrintInfo ()
  {
  GLPrintString("Version", GL_VERSION);
  GLPrintString("Vendor", GL_VENDOR);
  GLPrintString("Renderer", GL_RENDERER);
  GLPrintString("Extensions", GL_EXTENSIONS);
  }

#ifdef GLES
  //------------------------------------------------------------------------------
  UINT GLLoadShader (GLenum        shaderType,
                     const char *  pSource)
    {
    GLuint shader = GL (glCreateShader (shaderType));
    if (shader)
      {
      GL (glShaderSource(shader, 1, &pSource, NULL));
      GL (glCompileShader(shader));
      GLint compiled = 0;
      GL (glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
      if (!compiled)
        {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen)
          {
          char* buf = (char*) malloc(infoLen);
          if (buf)
            {
            glGetShaderInfoLog(shader, infoLen, NULL, buf);
            DBG_ERROR ("Could not compile shader %s:\n%s\n", ((shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment"), buf);
            free(buf);
            }
          glDeleteShader(shader);
          shader = 0;
          DBG_ERROR (pSource);
          }
        }
      }
    return shader;
    }
#endif

//------------------------------------------------------------------------------
UINT  GLCreateVertexShader (const char *  pSourceIn)
  {
  #ifdef GLES
    UINT  uShader = GLLoadShader (GL_VERTEX_SHADER, pSourceIn);
    GLCheckError (__FILE__, "GLCreateVertexShader");
    return (uShader);
  #else
    return (0);
  #endif
  }

//------------------------------------------------------------------------------
UINT  GLCreateFragmentShader (const char *  pSourceIn)
  {
  #ifdef GLES
    UINT  uShader = GLLoadShader (GL_FRAGMENT_SHADER, pSourceIn);
    GLCheckError (__FILE__, "GLCreateFragmentShader");
    return (uShader);
  #else
    return (0);
  #endif
  }

//------------------------------------------------------------------------------
VOID GLDeleteShader (UINT *  puShaderIn)
  {
  #ifdef GLES
    glDeleteShader (*puShaderIn);
    *puShaderIn = 0;
  #endif
  }

//------------------------------------------------------------------------------
VOID GLDeleteProgram (UINT *  puProgramIn)
  {
  #ifdef GLES
    glDeleteProgram (*puProgramIn);
    *puProgramIn = 0;
  #endif
  }

//------------------------------------------------------------------------------
UINT GLCreateProgram (const char *  pVertexSource,
                      const char *  pFragmentSource)
  {
  #ifdef GLES
  DBG_INFO("GLCreateProgram");

    UINT vertexShader = GLCreateVertexShader (pVertexSource);
    if (!vertexShader)
      {
      return 0;
      }
  DBG_INFO("GLCreateProgram");

    UINT fragmentShader = GLCreateFragmentShader (pFragmentSource);
    if (!fragmentShader)
      {
      return 0;
      }
  DBG_INFO("GLCreateProgram");

    return (GLCreateProgram (vertexShader, fragmentShader));
  #else
    return (0);
  #endif
  }

//------------------------------------------------------------------------------
UINT GLCreateProgram (UINT  uVertexShader,
                      UINT  uFragmentShader)
  {
  #ifdef GLES

    GLuint program = GL (glCreateProgram());
    if (program)
      {
      GL (glAttachShader(program, uVertexShader));
      GL (glAttachShader(program, uFragmentShader));
      GL (glLinkProgram(program));
      GLint linkStatus = GL_FALSE;
      glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
      if (linkStatus != GL_TRUE)
        {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength)
          {
          char* buf = (char*) malloc(bufLength);
          if (buf)
            {
            glGetProgramInfoLog(program, bufLength, NULL, buf);
            DBG_ERROR ("Could not link program:\n%s\n", buf);
            free(buf);
            }
          }
        GL (glDeleteProgram(program));
        program = 0;
        }
      }
    return program;
  #else
    return (0);
  #endif
  }

//------------------------------------------------------------------------------
VOID GLCreateTexture (UINT *  puTextureOut)
  {
  #ifdef GLES
    GLuint texture;
    GL (glGenTextures (1, &texture));
    *puTextureOut = texture;
  #endif
  }

//------------------------------------------------------------------------------
VOID GLDeleteTexture (UINT *  puTextureIn)
  {
  #ifdef GLES
    GLuint texture = *puTextureIn;
    GL (glDeleteTextures (1, &texture));
    *puTextureIn = 0;
  #endif
  }

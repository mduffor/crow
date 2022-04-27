

#ifndef GLUTIL_HPP
#define GLUTIL_HPP

#include "Sys/Types.hpp"


#define GL_CHECK_ERROR(...) GLCheckError(__FILE__, __LINE__, __VA_ARGS__)

#if DEBUG
  #define GL( func )    func; GLCheckError(__FILE__, __LINE__, "func");
#else
  #define GL( func )    func;
#endif

#if !defined( OPENGL_ES3_BIT_KHR )
  #define OPENGL_ES3_BIT_KHR    0x0040
#endif

#ifdef GLES
  void   GLPrintString          (const char *  name,
                                 GLenum        s);
#else
  #define GLPrintString(a,b)
#endif


#ifdef EGLAPI
  const char * EglErrorString (const EGLint error);
#endif


const char * GLErrorString    (GLenum error);

void   GLCheckError           (const char *  filename,
                               const char *  op);

void   GLCheckError           (const char *  filename,
                               int           line,
                               const char *  op);

void   GLPrintInfo            ();

#ifdef GLES
  UINT   GLLoadShader           (GLenum        shaderType,
                                 const char *  pSource);
#else
  #define GLLoadShader(a,b) 0
#endif

UINT   GLCreateProgram        (const char *  pVertexSource,
                               const char *  pFragmentSource);

UINT   GLCreateVertexShader   (const char *  pSourceIn);

UINT   GLCreateFragmentShader (const char *  pSourceIn);

VOID   GLDeleteShader         (UINT *        puShaderIn);

VOID   GLDeleteProgram        (UINT *        puProgramIn);

UINT   GLCreateProgram        (const char *  pVertexSource,
                               const char *  pFragmentSource);

UINT   GLCreateProgram        (UINT          uVertexShader,
                               UINT          uFragmentShader);

VOID   GLDeleteTexture        (UINT *  puTextureIn);

VOID   GLCreateTexture        (UINT *  puTextureOut);



//=============================================================================
class GLCallProxy
  {
  public:


    virtual VOID  GenBuffers      (GLsizei  n,
                                   GLuint * buffers) = 0;

    virtual VOID  SetTexMinFilter (UINT32  uFilterIn) = 0;
    virtual VOID  SetTexMagFilter (UINT32  uFilterIn) = 0;

    virtual VOID  SetTexWrapS     (UINT32  uWrapIn) = 0;
    virtual VOID  SetTexWrapT     (UINT32  uWrapIn) = 0;

  };

//=============================================================================
class GLCallProxyLive : public GLCallProxy
  {
  private:

    static GLCallProxyLive  sInstance;

  public:
    static GLCallProxy &  Instance (VOID)   {return sInstance;};

    VOID                  GenBuffers  (GLsizei  n,
                                       GLuint * buffers)      override {glGenBuffers (n, buffers);  GL_CHECK_ERROR ("glGenBuffers in GLCallProxy");};

    VOID                  SetTexMinFilter (UINT32  uFilterIn) override {GL( glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, uFilterIn));};
    VOID                  SetTexMagFilter (UINT32  uFilterIn) override {GL( glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, uFilterIn));};

    VOID                  SetTexWrapS     (UINT32  uWrapIn)   override {GL( glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uWrapIn));};
    VOID                  SetTexWrapT     (UINT32  uWrapIn)   override {GL( glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, uWrapIn));};

  };


//=============================================================================
class GLCallProxyMock : public GLCallProxy
  {
  private:

    static GLCallProxyMock  sInstance;

  public:

    static GLCallProxy &  Instance (VOID)   {return sInstance;};


    VOID                  GenBuffers      (GLsizei  n,
                                           GLuint * buffers)  override {DBG_INFO ("GL Mock GenBuffers");};

    VOID                  SetTexMinFilter (UINT32  uFilterIn) override {};
    VOID                  SetTexMagFilter (UINT32  uFilterIn) override {};

    VOID                  SetTexWrapS     (UINT32  uWrapIn)   override {};
    VOID                  SetTexWrapT     (UINT32  uWrapIn)   override {};

  };







#endif // GLUTIL_HPP

/* -----------------------------------------------------------------
                            TEdit Standard Types

     This header contains type definitions for platform independence.

                            Michael T. Duffy
   ----------------------------------------------------------------- */

// TTypes.hpp
// Copyright 2004, 2008, Michael T. Duffy.  All Rights Reserved
// contact:  mduffor@gmail.com

// This file is part of TEdit.
//
// TEdit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
//
// TEdit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TEdit.  If not, see <http://www.gnu.org/licenses/>.


// Note:  Be sure to include platform specific header files before including
//         this file, because we will need to detect what has already been
//         defined by the system before trying to create our own types


#ifndef TYPES_HPP
#define TYPES_HPP

#include "stdio.h"
#include "limits.h"
#include "string.h"
#include "math.h"

#ifndef ANDROID_NDK
  #include <stdint.h>
#endif

#define RAVEN_EXPORT

#ifdef RAVEN_WIN32_COMPATIBLE
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  //#include <windowsx.h>
#endif

#ifdef LINUX
  //#define strnicmp strncasecmp
  //#define stricmp strcasecmp
#endif

#ifdef ANDROID_NDK
  #define stricmp strcasecmp
  #define strnicmp strncasecmp
  #define powl pow
  #undef BOOL
#endif

// OpenGL (ES3) includes
#ifdef ANDROID_NDK
  #define GLES
  //#include <GLES2/gl2.h>
  //#include <GLES/egl.h>
  //#include <GLES3/gl3.h>
  #include <GLES3/gl31.h>
  #include <GLES3/gl3ext.h>
  #include <EGL/egl.h>
  #include <EGL/eglext.h>

#else /* !ANDROID_NDK */
// TODO: Ubuntu needs to implement GLES3 before the code will run.
  //#undef GLES

  #define GLES
  #include <GL/glew.h>

  //#define GLFW_INCLUDE_GLCOREARB
  //#define GLFW_INCLUDE_ES2
  #define GLFW_INCLUDE_NONE
  #include <GLFW/glfw3.h>

  //#include <GLES2/gl2.h>
  //#include <GLES3/gl3.h>
  //#include <GLES3/gl3ext.h>

  #include <EGL/egl.h>
  #include <EGL/eglext.h>

//  #include <GL/gl.h>
//  #include <GL/glu.h>
#endif
/**
  @addtogroup base
  @{
*/
#ifndef UINT32_MAX
  #define UINT32_MIN 0
  #define UINT32_MAX 4294967295

  #define INT32_MIN -2147483647
  #define INT32_MAX 2147483647

  #define UINT8_MIN 0
  #define UINT8_MAX 255

  #define INT8_MIN -127
  #define INT8_MAX 127

  #define UINT16_MIN 0
  #define UINT16_MAX 65535

  #define INT16_MIN -32767
  #define INT16_MAX 32767
#endif

// variable types

// If this is being compiled under windows, check for the windows header
//  that would define these same kind of typedefs
#ifndef _BASETSD_H_

  typedef uint64_t           UINT64;  ///< 64 bit unsigned integer.  Use UINT64 when the size of the variable matters, and use UINT if it does not.
  typedef int64_t            INT64;   ///< 64 bit signed integer.  Use INT64 when the size of the variable matters, and use INT if it does not.

  typedef uint32_t           UINT32;  ///< 32 bit unsigned integer.  Use UINT32 when the size of the variable matters, and use UINT if it does not.
  typedef int32_t            INT32;   ///< 32 bit signed integer.  Use INT32 when the size of the variable matters, and use INT if it does not.

  typedef unsigned int       UINT;    ///< 32 bit unsigned integer.  Defined to be the native size for the target platform, and will thus not incur any possible speed penalties.
  typedef          int       INT;     ///< 32 bit signed integer.  Defined to be the native size for the target platform, and will thus not incur any possible speed penalties.

  typedef bool               BOOL;    ///< boolean value
  typedef bool *             PBOOL;   ///< pointer to a boolean

  typedef uint32_t *         PUINT32; ///< pointer to 32-bit unsigned integer
  typedef int32_t *          PINT32;  ///< pointer to 32-bit signed integer

  typedef unsigned int *     PUINT;   ///< pointer to unsigned integer
  typedef          int *     PINT;    ///< pointer to signed integer

  typedef uint8_t            UINT8;   // use UINT8 or INT8 when the size of the
  typedef int8_t             INT8;    //  integer variable matters, and UINT or INT

  typedef uint16_t           UINT16;  // use UINT16 or INT16 when the size of the
  typedef int16_t            INT16;   //  integer variable matters, and UINT or INT

  typedef uint8_t *          PUINT8;  // use UINT8 or INT8 when the size of the
  typedef int8_t *           PINT8;   //  integer variable matters, and UINT or INT

  typedef uint16_t *         PUINT16; // use UINT16 or INT16 when the size of the
  typedef int16_t *          PINT16;  //  integer variable matters, and UINT or INT

#endif // _BASETSD_H_

typedef float              FLOAT;
typedef double             DOUBLE;

typedef float              FLOAT32;
typedef double             FLOAT64;

#define VOID               void

typedef char *             PSZ;
typedef const char *       CPSZ;
typedef char               CHAR;
typedef unsigned char      UCHAR;

// pointer types


typedef float  *           PFLOAT;
typedef double *           PDOUBLE;

typedef void *             PVOID;
typedef char *             PCHAR;
typedef unsigned char *    PUCHAR;

typedef unsigned long      TErrorCode;

const TErrorCode           errorNone    = 0;
const TErrorCode           errorSuccess = 0;
const TErrorCode           errorFailure = 1;

#ifndef TRUE
  #define TRUE  BOOL(1)
  #define FALSE BOOL(0)
#endif

#ifndef NULL
  #define NULL  0x0 // when we move to C++11 compatibility, this should change to nullptr
#endif

//#endif // UINT32

#ifndef RMax
  #define RMax(a,b) ((a)>(b)?(a):(b))
  #define RMin(a,b) ((a)<(b)?(a):(b))
  #define RClamp(x,min,max) ((x)<(min)?(min):((x)>(max)?(max):(x)))
  #define RAbs(x) ((x)<(0)?(-x):(x))
#endif


// Math related defines

#ifndef R_PI
  #define R_PI          3.141592654f   ///< Constant value for Pi, in float precision
#endif

#ifndef R_2PI
  #define R_2PI         6.283185307f   ///< Constant value for 2 * Pi, in float precision
#endif

#ifndef R_DEG_TO_RAD
  #define R_DEG_TO_RAD  0.017453293f   ///< Constant multiplier for converting degrees into radians
  #define R_RAD_TO_DEG  57.29577951f   ///< Constant multiplier for converting radians into degress
#endif

#define SEC_TO_MS(sec)     ((sec)*1000)
#define MS_TO_SEC(ms)      ((ms)/1000)
#define USEC_TO_MS(usec)   ((usec)/1000)
#define SEC_TO_MINUTE(sec) ((sec)/60)
#define SEC_TO_HOUR(sec)   ((sec)/3600)
#define SEC_TO_DAY(sec)    ((sec)/(3600*24))
#define MINUTE_TO_SEC(min) ((min)*60)
#define HOUR_TO_SEC(hour)  ((hour)*3600)
#define DAY_TO_SEC(day)    ((day)*3600*24)
#define DAY_TO_HOUR(day)   ((day)*24)

#define MINUTE_TO_HOUR(min) ((min)/60)
#define HOUR_TO_MINUTE(hour) ((hour)*60)

#define F_SEC_TO_MS(sec)     (FLOAT(sec)*1000.0f)
#define F_MS_TO_SEC(ms)      (FLOAT(ms)/1000.0f)
#define F_USEC_TO_MS(usec)   (FLOAT(usec)/1000.0f)
#define F_SEC_TO_MINUTE(sec) (FLOAT(sec)/60.0f)
#define F_SEC_TO_HOUR(sec)   (FLOAT(sec)/3600.0f)
#define F_SEC_TO_DAY(sec)    (FLOAT(sec)/(3600.0f*24.0f))
#define F_MINUTE_TO_SEC(min) (FLOAT(min)*60.0f)
#define F_HOUR_TO_SEC(hour)  (FLOAT(hour)*3600.0f)
#define F_DAY_TO_SEC(day)    (FLOAT(day)*3600.0f*24.0f)

// Epsilon is used for floating point equivalency tests.
// NOTE:  We use 1e-4 for compatibility with FLOAT16 mobile GPUs
#ifndef R_EPSILON
  #define R_EPSILON       0.0001f
  #define R_EPSILON_D     0.000001
#endif

#define FLT_APPROX_EQUAL(a,b)  (fabsf((a)-(b)) < R_EPSILON)
#define DBL_APPROX_EQUAL(a,b)  (fabs((a)-(b)) < R_EPSILON_D)

#define R_ALMOST_ONE 0.9999999f

//#define MAKE_FOUR_CODE(a,b,c,d) ((UINT32) (((UINT32) a) + (((UINT32) b) << 8) + (((UINT32) c) << 16) + (((UINT32) d) << 24)))
#define MAKE_FOUR_CODE(a) ((UINT32) (((UINT32) a[0]) + (((UINT32) a[1]) << 8) + (((UINT32) a[2]) << 16) + (((UINT32) a[3]) << 24)))

/// Define a fourCC code for a little endian system
#define IFF_MAKE_FOURCC(a,b,c,d) ((__int32_t)((__int32_t)(a)|((__int32_t)(b)<<8)|((__int32_t)(c)<<16)|((__int32_t)(d)<<24)))

// string helpers
#ifndef streq
#define streq(a,b) (strcmp ((a),(b)) == 0)
#endif

#ifndef streqi
#define streqi(a,b) (strcasecmp ((a),(b)) == 0)
#endif

#ifndef strempty
#define strempty(a) ((a == NULL) || (a[0] == 0))
#endif

#define DEFINE_AS_STR(x) _DEFINE_AS_STR(x)
#define _DEFINE_AS_STR(x) #x


#ifdef CROW_VERSION
  const char szCrowVersion[] = CROW_VERSION;
#else
  const char szCrowVersion[] = "UnknownVersion";
#endif


class RStr; // NOTE: Circular ref hack.

//-----------------------------------------------------------------------------
class EStatus
  {
  public:
    enum ECode  {kSuccessCode = 0,
                 kFailureCode = 1};

    static EStatus  kFailure;
    static EStatus  kSuccess;

    RStr *   pstrDescription;


  private:
    ECode        eStatusCode;

  public:
                  EStatus    ();
    explicit      EStatus    (ECode         codeIn);
                  EStatus    (ECode         codeIn,
                              const char *  pszDescriptionIn);
                  EStatus    (const EStatus &  statIn);
                  ~EStatus   ();

    EStatus &     operator=  (const EStatus &  statIn);
    BOOL          operator== (const EStatus &  statIn) const       {return (eStatusCode == statIn.eStatusCode);};
    BOOL          operator== (const EStatus::ECode  codeIn) const  {return (eStatusCode == codeIn);};
    BOOL          operator!= (const EStatus &  statIn) const       {return (eStatusCode != statIn.eStatusCode);};
    BOOL          operator!= (const EStatus::ECode  codeIn) const  {return (eStatusCode != codeIn);};

    BOOL          IsSuccess (VOID)  {return (eStatusCode == kSuccessCode);};
    BOOL          IsFailure (VOID)  {return (eStatusCode == kFailureCode);};

    VOID          SetDescription  (const char *  pszDescriptionIn);
    const char *  GetDescription  (VOID);

                    /** @brief Return a failure status object with the given description.  Helper function.
                        @param pszDescription Description of the error, to be passed back in the object.
                        @return An EStatus object set to type kFailure.
                      */
    static EStatus  Failure (const char *  pszDescription, ...);

  };

/** @} */ // end of base group

#endif // TYPES_HPP















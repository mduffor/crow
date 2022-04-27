

#include <sys/types.h>
#include <stdio.h>

#ifndef IFFCODEC_H
#define IFFCODEC_H

#ifdef __cplusplus
extern "C" {
#endif 

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------


typedef  __int32_t  ifftype4S;
typedef  __int32_t  ifftype4U;
typedef  float      ifftype4F;
typedef  double     ifftype8F;
typedef  char *     ifftypeStrZ;

typedef __int32_t   ifftypeFourCC;




//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

struct _iffCallbackData;
struct _iffStackFrame;
struct _iffCodec;

typedef  void  (*iffCallback) (struct _iffCodec *  pCodec,
                               ifftypeFourCC       ccBlockNamespace,
                               ifftypeFourCC       ccBlockType,
                               ifftype4U           ckSize,
                               void *              pvData);


//------------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------------

extern const ifftypeFourCC  ccForm;
extern const ifftypeFourCC  ccFor4;
extern const ifftypeFourCC  ccFor8;
extern const ifftypeFourCC  ccList;
extern const ifftypeFourCC  ccLis4;
extern const ifftypeFourCC  ccLis8;
extern const ifftypeFourCC  ccCat ;
extern const ifftypeFourCC  ccCat4;
extern const ifftypeFourCC  ccCat8;
extern const ifftypeFourCC  ccParm;
extern const ifftypeFourCC  ccPar4;
extern const ifftypeFourCC  ccPar8;
extern const ifftypeFourCC  ccGen ;
extern const ifftypeFourCC  ccNull;



//------------------------------------------------------------------------------
// Helper Macros
//------------------------------------------------------------------------------

/// Define a fourCC code for a little endian system
#define IFF_MAKE_FOURCC(a,b,c,d) ((__int32_t)((__int32_t)(a)|((__int32_t)(b)<<8)|((__int32_t)(c)<<16)|((__int32_t)(d)<<24)))


//------------------------------------------------------------------------------
// Structure Definitions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
typedef struct _iffCallbackData
//------------------------------------------------------------------------------
  {
  ifftypeFourCC  ccNamespace;
  ifftypeFourCC  ccType;
  iffCallback    pCallback;

  struct _iffCallbackData *  pNext;
  } iffCallbackData;

//------------------------------------------------------------------------------
typedef struct _iffStackFrame
//------------------------------------------------------------------------------
  {

  FILE *                   fp;
  unsigned int             uFileOffset;

  ifftypeFourCC            ccBlockType;
  unsigned char *          pBuffer;
  unsigned int             uBufferOffset;
  unsigned int             uBufferSize;
  struct _iffStackFrame *  pNext;
  }iffStackFrame;


//------------------------------------------------------------------------------
typedef struct _iffCodec
//------------------------------------------------------------------------------
  {
  FILE *             fp;
  
  ifftypeStrZ        szFilename;
  iffCallbackData *  pCallbackList;
  iffStackFrame *    pParseStack;
  } iffCodec;





 


ifftype4U      iffGet4UB (const unsigned char *  pDataIn);
ifftype4U      iffGet4UL (const unsigned char *  pDataIn);
ifftype4F      iffGet4FB (const unsigned char *  pDataIn);
ifftype4F      iffGet4FL (const unsigned char *  pDataIn);
ifftypeFourCC  iffGetCC  (const unsigned char *  pDataIn);


ifftype4U      iffFileGet4UB  (iffCodec *  pCodecIn);
ifftype4U      iffFileGet4UL  (iffCodec *  pCodecIn);
ifftype4F      iffFileGet4FB  (iffCodec *  pCodecIn);
ifftype4F      iffFileGet4FL  (iffCodec *  pCodecIn);
ifftypeFourCC  iffFileGetCC   (iffCodec *  pCodecIn);
void           iffFileGetData (iffCodec *  pCodecIn,
                               unsigned char *  pDataOut,
                               unsigned int     uDataSize);

void           iffFileSetOffset (iffCodec *       pCodecIn,
                                 unsigned int     uOffsetIn);
unsigned int   iffFileGetOffset (iffCodec *       pCodecIn);
                                 
unsigned int   iffFileSetData   (iffCodec *       pCodecIn,
                                 unsigned char *  pDataIn,
                                 unsigned int     uDataSize);


void iffSet4UB   (unsigned char * *  pDataOut,
                  ifftype4U          uDataIn);
void iffSet4UL   (unsigned char * *  pDataOut,
                  ifftype4U          uDataIn);
void iffSet4FB   (unsigned char * *  pDataOut,
                  ifftype4F          fDataIn);
void iffSet4FL   (unsigned char * *  pDataOut,
                  ifftype4F          fDataIn);
void iffSetCC    (unsigned char * *  pDataOut,
                  ifftypeFourCC      ccDataIn);
void iffSetData  (unsigned char * *  pDataOut,
                  unsigned char *    pDataIn,
                  unsigned int       uDataSize);


unsigned int iffPadded4      (unsigned int  uUnpaddedLengthIn);
unsigned int iffNeededToPad4 (unsigned int  uUnpaddedLengthIn);

unsigned int iffPadded8      (unsigned int  uUnpaddedLengthIn);
unsigned int iffNeededToPad8 (unsigned int  uUnpaddedLengthIn);


void               iffRegisterCallback  (iffCodec *     pCodecIn,
                                         ifftypeFourCC  ccNamespaceIn,
                                         ifftypeFourCC  ccTypeIn,
                                         iffCallback    pCallbackIn);

iffCodec *         iffCreateCodec       (void);

void               iffFreeCodec         (iffCodec *  pCodecIn);

void               iffFreeCallbackList  (iffCodec *  pCodecIn);

void               iffFreeParserStack   (iffCodec *  pCodecIn);

void               iffCloseFile         (iffCodec *  pCodecIn);

iffStackFrame *    iffPushParserStack   (iffCodec *     pCodecIn,
                                         ifftypeFourCC  ccBlockTypeIn,
                                         unsigned int   iBlockSizeIn);

void               iffPopParserStack    (iffCodec *     pCodecIn);

int                iffOpenFile          (iffCodec *    pCodecIn,
                                         const char *  pszFilenameIn);

int                iffReOpenFile        (iffCodec *     pCodecIn);

int                iffParseFile         (iffCodec *     pCodecIn,
                                         void *         pvDataIn);

void               iffParseBlock        (iffCodec *       pCodecIn,
                                         ifftypeFourCC    ccNamespaceIn,
                                         iffStackFrame *  pCurrBlock,
                                         void *           pvDataIn);

iffCallbackData *  FindCallback         (iffCodec *       pCodecIn,
                                         ifftypeFourCC    ccNamespaceIn,
                                         ifftypeFourCC    ccTypeIn);


#ifdef __cplusplus
};
#endif 


#endif // IFFCODEC_H


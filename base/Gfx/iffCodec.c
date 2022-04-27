

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include "Gfx/iffCodec.h"


// Note:  This code is currently designed to be compiled and run on a little-endian
//         system, like Intel/AMD/Via 80x86 based processors.

const ifftypeFourCC  ccForm = IFF_MAKE_FOURCC ('F','O','R','M');
const ifftypeFourCC  ccFor4 = IFF_MAKE_FOURCC ('F','O','R','4');
const ifftypeFourCC  ccFor8 = IFF_MAKE_FOURCC ('F','O','R','8');
const ifftypeFourCC  ccList = IFF_MAKE_FOURCC ('L','I','S','T');
const ifftypeFourCC  ccLis4 = IFF_MAKE_FOURCC ('L','I','S','4');
const ifftypeFourCC  ccLis8 = IFF_MAKE_FOURCC ('L','I','S','8');
const ifftypeFourCC  ccCat  = IFF_MAKE_FOURCC ('C','A','T',' ');
const ifftypeFourCC  ccCat4 = IFF_MAKE_FOURCC ('C','A','T','4');
const ifftypeFourCC  ccCat8 = IFF_MAKE_FOURCC ('C','A','T','8');
const ifftypeFourCC  ccParm = IFF_MAKE_FOURCC ('P','A','R','M');
const ifftypeFourCC  ccPar4 = IFF_MAKE_FOURCC ('P','A','R','4');
const ifftypeFourCC  ccPar8 = IFF_MAKE_FOURCC ('P','A','R','8');
const ifftypeFourCC  ccNull = IFF_MAKE_FOURCC (' ',' ',' ',' ');



//------------------------------------------------------------------------------
ifftype4U  iffGet4UB (const unsigned char *  pDataIn)
  {
  return ((ifftype4U) ( ((int)pDataIn[0] << 24) |
                        ((int)pDataIn[1] << 16) |
                        ((int)pDataIn[2] <<  8) |
                        ((int)pDataIn[3]      )));
  };

//------------------------------------------------------------------------------
ifftype4U  iffGet4UL (const unsigned char *  pDataIn)
  {
  return ((ifftype4U) ( ((int)pDataIn[0]      ) |
                        ((int)pDataIn[1] <<  8) |
                        ((int)pDataIn[2] << 16) |
                        ((int)pDataIn[3] << 24)));
  };

//------------------------------------------------------------------------------
ifftype4F  iffGet4FB (const unsigned char *  pDataIn)
  {
  return ((ifftype4F) ( ((int)pDataIn[0] << 24) |
                        ((int)pDataIn[1] << 16) |
                        ((int)pDataIn[2] <<  8) |
                        ((int)pDataIn[3]      )));
  };

//------------------------------------------------------------------------------
ifftype4F  iffGet4FL (const unsigned char *  pDataIn)
  {
  return ((ifftype4F) ( ((int)pDataIn[0]      ) |
                        ((int)pDataIn[1] <<  8) |
                        ((int)pDataIn[2] << 16) |
                        ((int)pDataIn[3] << 24)));
  };

//------------------------------------------------------------------------------
ifftypeFourCC  iffGetCC  (const unsigned char *  pDataIn)
  {
  return ((ifftypeFourCC) ( ((int)pDataIn[0]      ) |
                            ((int)pDataIn[1] <<  8) |
                            ((int)pDataIn[2] << 16) |
                            ((int)pDataIn[3] << 24)));
  };


//------------------------------------------------------------------------------
ifftype4U  iffFileGet4UB (iffCodec *  pCodecIn)
  {
  if (pCodecIn == NULL) return ((ifftype4U)0);

  unsigned char  acReadBuffer [4];

  // read from disk
  fread (acReadBuffer, 1, 4, pCodecIn->fp);

  return (iffGet4UB (acReadBuffer));
  };


//------------------------------------------------------------------------------
ifftype4U  iffFileGet4UL (iffCodec *  pCodecIn)
  {
  if (pCodecIn == NULL) return ((ifftype4U)0);

  unsigned char  acReadBuffer [4];

  // read from disk
  fread (acReadBuffer, 1, 4, pCodecIn->fp);

  return (iffGet4UL (acReadBuffer));
  };


//------------------------------------------------------------------------------
ifftype4F  iffFileGet4FB (iffCodec *  pCodecIn)
  {
  if (pCodecIn == NULL) return ((ifftype4F)0.0f);

  unsigned char  acReadBuffer [4];

  // read from disk
  fread (acReadBuffer, 1, 4, pCodecIn->fp);

  return (iffGet4FB (acReadBuffer));
  };


//------------------------------------------------------------------------------
ifftype4F  iffFileGet4FL (iffCodec *  pCodecIn)
  {
  if (pCodecIn == NULL) return ((ifftype4F)0.0f);

  unsigned char  acReadBuffer [4];

  // read from disk
  fread (acReadBuffer, 1, 4, pCodecIn->fp);

  return (iffGet4FL (acReadBuffer));
  };


//------------------------------------------------------------------------------
ifftypeFourCC  iffFileGetCC  (iffCodec *  pCodecIn)
  {
  if (pCodecIn == NULL) return (ccNull);

  unsigned char  acReadBuffer [4];

  // read from disk
  fread (acReadBuffer, 1, 4, pCodecIn->fp);

  return (iffGetCC (acReadBuffer));
  };


//------------------------------------------------------------------------------
void  iffFileGetData (iffCodec *       pCodecIn,
                      unsigned char *  pDataOut,
                      unsigned int     uDataSize)
  {
  if (pCodecIn == NULL) return;
  fread (pDataOut, 1, uDataSize, pCodecIn->fp);
  };


//------------------------------------------------------------------------------
void  iffFileSetOffset (iffCodec *    pCodecIn,
                        unsigned int  uOffsetIn)
  {
  if (pCodecIn == NULL) return;
  fseek (pCodecIn->fp, uOffsetIn, SEEK_SET);
  };


//------------------------------------------------------------------------------
unsigned int  iffFileGetOffset (iffCodec *  pCodecIn)
  {
  if (pCodecIn == NULL) return 0;
  return (ftell (pCodecIn->fp));
  };


//------------------------------------------------------------------------------
unsigned int  iffFileSetData   (iffCodec *       pCodecIn,
                                unsigned char *  pDataIn,
                                unsigned int     uDataSize)
  {
  if (pCodecIn == NULL) return (0);

  return (fwrite (pDataIn, 1, uDataSize, pCodecIn->fp));
  };





//------------------------------------------------------------------------------
void iffSet4UB (unsigned char * *  pDataOut, ifftype4U  uDataIn)
  {
  (*pDataOut) [0] = (uDataIn >> 24) & 0x0000ff;
  (*pDataOut) [1] = (uDataIn >> 16) & 0x0000ff;
  (*pDataOut) [2] = (uDataIn >>  8) & 0x0000ff;
  (*pDataOut) [3] = (uDataIn      ) & 0x0000ff;
  *pDataOut += sizeof (ifftype4U);
  };

//------------------------------------------------------------------------------
void iffSet4UL (unsigned char * *  pDataOut, ifftype4U  uDataIn)
  {
  (*pDataOut) [0] = (uDataIn      ) & 0x0000ff;
  (*pDataOut) [1] = (uDataIn >>  8) & 0x0000ff;
  (*pDataOut) [2] = (uDataIn >> 16) & 0x0000ff;
  (*pDataOut) [3] = (uDataIn >> 24) & 0x0000ff;
  *pDataOut += sizeof (ifftype4U);
  };

//------------------------------------------------------------------------------
void iffSet4FB (unsigned char * *  pDataOut, ifftype4F  fDataIn)
  {
  // cast without conversion
  unsigned int  uDataIn = *((unsigned int *) &fDataIn);

  (*pDataOut) [0] = (uDataIn >> 24) & 0x0000ff;
  (*pDataOut) [1] = (uDataIn >> 16) & 0x0000ff;
  (*pDataOut) [2] = (uDataIn >>  8) & 0x0000ff;
  (*pDataOut) [3] = (uDataIn      ) & 0x0000ff;
  *pDataOut += sizeof (ifftype4F);
  };

//------------------------------------------------------------------------------
void iffSet4FL (unsigned char * *  pDataOut, ifftype4F  fDataIn)
  {
  // cast without conversion
  unsigned int  uDataIn = *((unsigned int *) &fDataIn);

  (*pDataOut) [0] = (uDataIn      ) & 0x0000ff;
  (*pDataOut) [1] = (uDataIn >>  8) & 0x0000ff;
  (*pDataOut) [2] = (uDataIn >> 16) & 0x0000ff;
  (*pDataOut) [3] = (uDataIn >> 24) & 0x0000ff;
  *pDataOut += sizeof (ifftype4F);
  };

//------------------------------------------------------------------------------
void iffSetCC  (unsigned char * *  pDataOut, ifftypeFourCC ccDataIn)
  {
  (*pDataOut) [0] = ((unsigned int)ccDataIn      ) & 0x0000ff;
  (*pDataOut) [1] = ((unsigned int)ccDataIn >>  8) & 0x0000ff;
  (*pDataOut) [2] = ((unsigned int)ccDataIn >> 16) & 0x0000ff;
  (*pDataOut) [3] = ((unsigned int)ccDataIn >> 24) & 0x0000ff;
  *pDataOut += sizeof (ifftypeFourCC);
  };


//------------------------------------------------------------------------------
void iffSetData  (unsigned char * *  pDataOut, unsigned char * pDataIn, unsigned int  uDataSize)
  {
  memcpy    (*pDataOut, pDataIn, uDataSize);
  *pDataOut += uDataSize;
  };


//------------------------------------------------------------------------------
unsigned int iffPadded4      (unsigned int  uUnpaddedLengthIn)
  {
  unsigned int  uPadded = uUnpaddedLengthIn + 3;
  uPadded -= (uPadded % 4);
  return (uPadded);
  };

//------------------------------------------------------------------------------
unsigned int iffNeededToPad4 (unsigned int  uUnpaddedLengthIn)
  {
  return (iffPadded4 (uUnpaddedLengthIn) - uUnpaddedLengthIn);
  };

//------------------------------------------------------------------------------
unsigned int iffPadded8      (unsigned int  uUnpaddedLengthIn)
  {
  unsigned int  uPadded = uUnpaddedLengthIn + 7;
  uPadded -= (uPadded % 8);
  return (uPadded);
  };

//------------------------------------------------------------------------------
unsigned int iffNeededToPad8 (unsigned int  uUnpaddedLengthIn)
  {
  return (iffPadded8 (uUnpaddedLengthIn) - uUnpaddedLengthIn);
  };












//------------------------------------------------------------------------------
void  iffRegisterCallback  (iffCodec *     pCodecIn,
                            ifftypeFourCC  ccNamespaceIn,
                            ifftypeFourCC  ccTypeIn,
                            iffCallback    pCallbackIn)
  {
  // register the routine that will handle a given fourCC within a given namespace
  iffCallbackData *  pNewCallback = (iffCallbackData *) malloc (sizeof (iffCallbackData));
  memset (pNewCallback, 0, sizeof (iffCallbackData));

  pNewCallback->ccNamespace = ccNamespaceIn;
  pNewCallback->ccType      = ccTypeIn;
  pNewCallback->pCallback   = pCallbackIn;

  iffCallbackData * *  pListEnd = & pCodecIn->pCallbackList;

  while (*pListEnd != NULL)
    {
    pListEnd = &(*pListEnd)->pNext;
    };
  *pListEnd = pNewCallback;
  };


//------------------------------------------------------------------------------
iffCodec *  iffCreateCodec  (void)
  {
  // Create a new object to hold all the info about an encoding/decoding session
  iffCodec *  pNewCodec = (iffCodec *) malloc (sizeof (iffCodec));
  memset (pNewCodec, 0, sizeof (iffCodec));

  pNewCodec->pCallbackList = NULL;
  pNewCodec->pParseStack   = NULL;

  pNewCodec->szFilename = (ifftypeStrZ) malloc (1);
  pNewCodec->szFilename [0] = '\0';

  return (pNewCodec);
  };


//------------------------------------------------------------------------------
void  iffFreeCodec (iffCodec *  pCodecIn)
  {
  // free all memory allocated for the codec.

  // free the callback list
  iffFreeCallbackList (pCodecIn);

  // free the parsing stack
  iffFreeParserStack (pCodecIn);

  iffCloseFile (pCodecIn);

  if (pCodecIn->szFilename) free (pCodecIn->szFilename);

  // free the codec
  free (pCodecIn);
  };


//------------------------------------------------------------------------------
void  iffFreeCallbackList (iffCodec *  pCodecIn)
  {
  // free the callback list
  while (pCodecIn->pCallbackList != NULL)
    {
    iffCallbackData *  pCallbackToDelete = pCodecIn->pCallbackList;
    pCodecIn->pCallbackList = pCodecIn->pCallbackList->pNext;

    free (pCallbackToDelete);
    };
  };


//------------------------------------------------------------------------------
void  iffFreeParserStack (iffCodec *  pCodecIn)
  {
  // free the parsing stack
  while (pCodecIn->pParseStack != NULL)
    {
    iffStackFrame *  pStackToDelete = pCodecIn->pParseStack;
    pCodecIn->pParseStack = pCodecIn->pParseStack->pNext;

    free (pStackToDelete);
    };

  if (pCodecIn->fp != NULL)
    {
    fclose (pCodecIn->fp);
    pCodecIn->fp = NULL;
    };
  };


//------------------------------------------------------------------------------
void  iffCloseFile (iffCodec *  pCodecIn)
  {
  if (pCodecIn == NULL) return;

  // close any file that is currently open, and free the parser stack
  if (pCodecIn->fp != NULL)
    {
    fclose (pCodecIn->fp);
    pCodecIn->fp = NULL;
    iffFreeParserStack (pCodecIn);
    };
  };


//------------------------------------------------------------------------------
iffStackFrame *  iffPushParserStack (iffCodec *     pCodecIn,
                                     ifftypeFourCC  ccBlockTypeIn,
                                     unsigned int   iBlockSizeIn)
  {

  // allocate a new parser frame
  iffStackFrame *  pNew = (iffStackFrame *) malloc (sizeof (iffStackFrame));
  if (! pNew) return (NULL);
  memset (pNew, 0, sizeof (iffStackFrame));

  // push the new frame onto the top of the stack
  pNew->pNext = pCodecIn->pParseStack;
  pCodecIn->pParseStack = pNew;

  // initialize the new frame
  pNew->fp          = pCodecIn->fp;
  pNew->uFileOffset = ftell (pNew->fp);
  pNew->uBufferSize = iBlockSizeIn;
  pNew->ccBlockType = ccBlockTypeIn;

  return (pNew);
  };

//------------------------------------------------------------------------------
void  iffPopParserStack (iffCodec *     pCodecIn)
  {

  // Pull the top frame off the stack
  iffStackFrame *  pTop = pCodecIn->pParseStack;
  if (pTop != NULL)
    {
    // remove from the stack
    pCodecIn->pParseStack = pTop->pNext;

    // free the memory
    free (pTop);
    };
  };

//------------------------------------------------------------------------------
int  iffReOpenFile  (iffCodec *    pCodecIn)
  {
  // open the file with the last provided filename

  if (pCodecIn == NULL) return (0);
  if (pCodecIn->szFilename [0] == '\0') return (0);

  // check to see if the file is already open
  if (pCodecIn->fp != NULL) return (1);

  pCodecIn->fp = fopen (pCodecIn->szFilename, "rwb");
  if (pCodecIn->fp == NULL)
    {
    return (0);
    };
  return (1);
  };


//------------------------------------------------------------------------------
int  iffOpenFile  (iffCodec *    pCodecIn,
                   const char *  pszFilenameIn)
  {
  // open the file with the provided filename

  if (pCodecIn == NULL) return (0);

  // set the new filename
  if (pCodecIn->szFilename) free (pCodecIn->szFilename);
  pCodecIn->szFilename = strdup (pszFilenameIn);

  return (iffReOpenFile (pCodecIn));
  };


//------------------------------------------------------------------------------
int  iffParseFile  (iffCodec *    pCodecIn,
                    void *        pvDataIn)
  {
  // returns a 1 on success, 0 on failure

  // parameter check
  if (pCodecIn == NULL) return (0);

  // close any open files
  iffCloseFile (pCodecIn);


  // open the file
  if (! iffReOpenFile  (pCodecIn))
    {
    return (0);
    };

  pCodecIn->fp = fopen (pCodecIn->szFilename, "rwb");
  if (pCodecIn->fp == NULL)
    {
    return (0);
    };

  // initialize stack with starting position

  unsigned char  acReadBuffer [8];
  ifftypeFourCC  ccBlockType;
  ifftype4U      uBlockSize;


  // get a block
  fread (acReadBuffer, 1, 8, pCodecIn->fp);

  ccBlockType = iffGetCC  (acReadBuffer);
  uBlockSize  = iffGet4UB (acReadBuffer + 4);

  // check to see if the block type is one of the known values, otherwise we are not dealing with an iff file.
  if ((ccBlockType == ccForm) ||
      (ccBlockType == ccCat)  ||
      (ccBlockType == ccList) ||
      (ccBlockType == ccFor4) ||
      (ccBlockType == ccCat4) ||
      (ccBlockType == ccLis4) ||
      (ccBlockType == ccFor8) ||
      (ccBlockType == ccCat8) ||
      (ccBlockType == ccLis8))
    {
    // this appears to be an iff file.  Read the data.

    iffStackFrame *  pFrameCurr = iffPushParserStack (pCodecIn, ccBlockType, uBlockSize);

    iffParseBlock (pCodecIn, ccNull, pFrameCurr, pvDataIn);

    iffPopParserStack (pCodecIn);
    pFrameCurr = NULL;
    };

  return (1);
  };


//------------------------------------------------------------------------------
void  iffParseBlock (iffCodec *       pCodecIn,
                     ifftypeFourCC    ccNamespaceIn,
                     iffStackFrame *  pCurrBlock,
                     void *           pvDataIn)
  {
  unsigned char  acReadBuffer [8];
  ifftypeFourCC  ccChildNamespace = ccNull;
  ifftypeFourCC  ccBlockType;
  ifftype4U      uBlockSize;
  ifftype4U      uBlockSizePadded;
  int            bIsContainer = 0;

  // start reading at the front of this data block
  fseek (pCurrBlock->fp, pCurrBlock->uBufferOffset, SEEK_CUR);



  // read tag, size, and subtype
  // search for tag type on registered list
  // if we have a handler for this type
  //   push new stack position
  //   Call callback for type
  // If this type is a FORM, LIST, or CAT
  //   go to next block, taking padding into account





  // if we have one of the standard list types, read the contents type.
  if ((pCurrBlock->ccBlockType == ccForm) ||
      (pCurrBlock->ccBlockType == ccCat)  ||
      (pCurrBlock->ccBlockType == ccList) ||
      (pCurrBlock->ccBlockType == ccFor4) ||
      (pCurrBlock->ccBlockType == ccCat4) ||
      (pCurrBlock->ccBlockType == ccLis4) ||
      (pCurrBlock->ccBlockType == ccFor8) ||
      (pCurrBlock->ccBlockType == ccCat8) ||
      (pCurrBlock->ccBlockType == ccLis8))
    {
    // get the contents type.  This will be the namespace for the child blocks
    fread (acReadBuffer, 1, 4, pCodecIn->fp);
    ccChildNamespace = iffGetCC  (acReadBuffer);
    pCurrBlock->uBufferOffset += 4;
    bIsContainer = 1;
    };

  // if this is a list, step through the properties
  // to be implemented

  // Perform any callbacks for this type

  iffCallbackData *  pCallbackData = FindCallback (pCodecIn, ccNamespaceIn, ccChildNamespace);

  if (pCallbackData)
    {
    (*pCallbackData->pCallback) (pCodecIn, ccNamespaceIn, ccChildNamespace, pCurrBlock->uBufferSize, pvDataIn);
    };

  if (bIsContainer)
    {
    // step through the blocks in this container
    while (pCurrBlock->uBufferOffset < pCurrBlock->uBufferSize)
      {
      // get a block
      fread (acReadBuffer, 1, 8, pCodecIn->fp);

      ccBlockType = iffGetCC  (acReadBuffer);
      uBlockSize  = iffGet4UB (acReadBuffer + 4);

      // prepare for the next block
      uBlockSizePadded = uBlockSize;

      if ((ccBlockType == ccForm) ||
          (ccBlockType == ccCat)  ||
          (ccBlockType == ccList))
        {
        // align to 2-byte boundary
        uBlockSizePadded += 1;
        uBlockSizePadded -= uBlockSizePadded % 2;
        }
      else if ((ccBlockType == ccFor4) ||
               (ccBlockType == ccCat4) ||
               (ccBlockType == ccLis4))
        {
        // align to 4-byte boundary
        uBlockSizePadded += 3;
        uBlockSizePadded -= uBlockSizePadded % 4;
        }
      else if ((ccBlockType == ccFor8) ||
               (ccBlockType == ccCat8) ||
               (ccBlockType == ccLis8))
        {
        // align to 8-byte boundary
        uBlockSizePadded += 7;
        uBlockSizePadded -= uBlockSizePadded % 8;
        };

      // parse the block
      iffStackFrame *  pFrameCurr = iffPushParserStack (pCodecIn, ccBlockType, uBlockSize);
      iffParseBlock (pCodecIn, ccChildNamespace, pFrameCurr, pvDataIn);

      // free the block
      iffPopParserStack (pCodecIn);
      pFrameCurr = NULL;

      pCurrBlock->uBufferOffset += uBlockSizePadded;

      // prepare the file pointer for reading the next block
      fseek (pCodecIn->fp, pCurrBlock->uBufferOffset, SEEK_CUR);

      };
    };
  };


//------------------------------------------------------------------------------
iffCallbackData *  FindCallback (iffCodec *       pCodecIn,
                                 ifftypeFourCC    ccNamespaceIn,
                                 ifftypeFourCC    ccTypeIn)

  {
  /// Search the list of callbacks, and return the one that matches the given data.
  if (pCodecIn == NULL) return (NULL);

  iffCallbackData *  pCurr = pCodecIn->pCallbackList;
  while (pCurr != NULL)
    {
    if ((pCurr->ccNamespace == ccNamespaceIn) &&
        (pCurr->ccType      == ccTypeIn))
      {
      // found a callback
      return (pCurr);
      };
    pCurr = pCurr->pNext;
    };
  // did not find a callback
  return (NULL);
  };














/*

 1 FOR4 ckSize
     CACH
     FOR4  ckSize
       HEAD
       VERS ckSize
         version (4UB)
       DESC ckSize
         szDescription (strZ)
       DATE ckSize
         szDate (strZ) - optional
       UNAM ckSize
         szUsername (strZ) - optional
     LIS4 ckSize
       SECT
       FOR4 ckSize
         SECT
           FOR4 ckSize
           SHDR
           SHDR ckSize
             FramesPerSecond (4FB)
             NumFrames (4UB)
             PreTangentType (4UB)
             PostTangentType (4UB)
             SectionNamespace (strz)
           TIME ckSize
             RelTimesInSec (NumFrames x 4FB)
           TANG ckSize
             SegmentTangentTypes ((NumFrames - 1) x 4UB)
         LIS4 ckSize
           CHAN
             FOR4 ckSize
             CHDR
             CSIZ ckSize
               ChanType (fourCC)
               ChanNumSamples (4UB)
               ChanDataSize (4UB)
               IsBigEndian (4UB)
             CNAM ckSize
               ChanName (strZ)
             CFIL ckSize
               ChanFile (strZ)
             CLAY ckSize
               ChanLayer (strZ)
         LIS4  ckSize
           CDAT
           CDAT ckSize
             (frame 0 chan data )
           CDAT ckSize
             (frame 1 chan data )
             ...
           CDAT ckSize
             (frame n chan data )






*/






/*


// code for Data Chache

// create a new codec
iffCodec *  pIff = iffCreateCodec ();

// register the routines for processing the different blocks
iffRegisterCallback  (pIff, ccNull, ccCach, &dcCbNewCache)
iffRegisterCallback  (pIff, ccCach, ccHead, &dcCbCacheHeader)

iffRegisterCallback  (pIff, ccHead, ccVers, &dcCbCacheHeader)
iffRegisterCallback  (pIff, ccHead, ccDesc, &dcCbCacheHeader)
iffRegisterCallback  (pIff, ccHead, ccDate, &dcCbCacheHeader)
iffRegisterCallback  (pIff, ccHead, ccUNam, &dcCbCacheHeader)

iffRegisterCallback  (pIff, ccCach, ccSect, &dcCbNewSection)
iffRegisterCallback  (pIff, ccSect, ccSHdr, &dcCbCacheSection)
iffRegisterCallback  (pIff, ccSect, ccTime, &dcCbCacheSection)
iffRegisterCallback  (pIff, ccSect, ccTang, &dcCbCacheSection)

iffRegisterCallback  (pIff, ccSect, ccChan, &dcCbNewChannel)
iffRegisterCallback  (pIff, ccChan, ccCHdr, &dcCbCacheChannel)
iffRegisterCallback  (pIff, ccChan, ccCSiz, &dcCbCacheChannel)
iffRegisterCallback  (pIff, ccChan, ccCNam, &dcCbCacheChannel)
iffRegisterCallback  (pIff, ccChan, ccCFil, &dcCbCacheChannel)
iffRegisterCallback  (pIff, ccChan, ccCLay, &dcCbCacheChannel)

iffRegisterCallback  (pIff, ccSect, ccCDat, &dcCbCacheData)

if (iffParseFile  (piff, pszFilename)
  {
  // success
  };






*/





























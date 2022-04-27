/* -----------------------------------------------------------------
                         Raven Node Script Parser

     This module implements reading and writing Nodes and Attrs
       stored in a Renderman-like attribute definition string.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2006-2014, Michael T. Duffy II.  All rights reserved.
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


#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Composite/SceneLoader.hpp"
#include "Sys/FilePath.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Gfx/Anim.hpp"
#include "Util/ParseTools.hpp"
#include "Net/RC4.hpp"

RStr  SceneLoader::strDecryptKey;

// Reserved words (shouldn't be used as attr names):
//  node, component, include, int, string, float, bool, animclip, curve

// NOTE:  For the ValueRegistry initialization to take place, a root registry
//         needs to be set up first before calling ReadFile().

// NOTE:  I'm not completely happy with this implementation.  It works, but
//         parsing isn't handled consistently across all parsers.  Some have
//         the keyword extracted before passing on to the next stage.  Some
//         are passed with the keyword intact.  Detection of what kind of line
//         we are looking at is inconsistent and happening at a higher level
//         that it should.  The parser is hard coded to recognize Gfx/Anim, though
//         it should probably be written to have installable modules instead.
//         So all in all it works, but could be a lot better.


/*
// Include directive
include: "" "res://scenes/Globals.scn"

// Value Registry variables
int: RegInt = 4
bool: RegTrue = True
bool: RegFalse = False
float: RegFloat = 5.0
string: RegString = "Bilbo"

// Scene graph of Nodes and components
node: "|main_camera"
  component: transform
    tz [5]
    rotateOrder [0]

  component: camera
    focalLength [0.055]
    near [0.5]
    far [10000]

    // set aperture to 35 mm Academy (.864 in x .630 in) 1.37
    //  fInchToMeter = 0.025400;
    apertureX [0.0219456]
    apertureY [0.016002]

    centerOfInterest [5.0]

node: "|Placer"
  component: transform

node: "|Placer|gm_TestSquare"
  component: transform
    tx [0]
    tz [0]
  component: mesh
    file ["gfx/cast/testhead/geometry/square.obj"]

node: "|Placer|Collider"
  component: transform
  component: boxCollider
    x [1]
    y [1]
    z [1]

node: "|MyGizmo
  component: transform
  reference: "templates/gizmo/gizmoA.ref"

// Animation clips
animclip: "curve"
  name ["walk"]
  loop ["none"]
  loopCount [2]
  outLeft ["zero"]
  outRight ["clamp"]
  speed [1.0]
  curve:
    map ["leg.tx"]
    key ["t=0.04166667 in=spline:20.4 out=spline:20.4 v=30.0"]
    key ["t=0.08333333 in=spline:20.4 out=spline:20.4 v=30.0"]
    key ["t=0.12500000 in=spline:20.4 out=spline:20.4 v=30.0"]
    key ["t=0.16666667 in=spline:20.4 out=spline:20.4 v=30.0"]
*/

//-----------------------------------------------------------------------------
//  SceneLoader
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SceneLoader::SceneLoader  ()
  {
  };



//-----------------------------------------------------------------------------
SceneLoader::~SceneLoader ()
  {
  };

//-----------------------------------------------------------------------------
VOID SceneLoader::PrintError (EStatus       statusIn,
                              RStrParser &  parserBufferIn,
                              const char *  szFilenameForErrorIn)
  {
  RStr         strErrorOut;

  if (statusIn == EStatus::kSuccess) return;

  strErrorOut.Format ("%s  Scene file %s at line %d",
                      statusIn.GetDescription (),
                      szFilenameForErrorIn,
                      parserBufferIn.CountChar ('\n', 0, parserBufferIn.GetCursorStart()));
  DBG_ERROR (strErrorOut.AsChar ());
  statusIn.SetDescription (strErrorOut.AsChar ());
  };

//-----------------------------------------------------------------------------
EStatus  SceneLoader::ReadFile  (const char *  szFilenameIn,
                                 const char *  szHeirarchyPrefixIn,
                                 World *       pwldWorldIn,
                                 BOOL          bLoadAnim) // NOTE: Not currently used... anim loading not re-implemented
  {
  EStatus    errorStatus = EStatus::kSuccess;


  // make sure the file exists.
  if (! FilePath::FileExists (szFilenameIn))
    {
    return (EStatus::Failure (RStr("File does not exist: ") + szFilenameIn));
    };
//  DBG_INFO ("SceneLoader::ReadFile (): Reading %s", szFilenameIn);

  RStrParser   parserBuffer;

  // slurp the file in
  errorStatus = parserBuffer.ReadFromFile (szFilenameIn);

  if (errorStatus == EStatus::kSuccess)
    {
    // decode encoded/obfuscated buffers.

    if (!strDecryptKey.IsEmpty ())
      {
      RC4         rc4;
      RStrParser  parserDecoded;

      rc4.InitWithKey (strDecryptKey);
      rc4.Decode (parserBuffer.AsChar (), parserBuffer.Length (), parserDecoded);

      return (ReadBuffer (parserDecoded,
                          szHeirarchyPrefixIn,
                          pwldWorldIn,
                          bLoadAnim,
                          szFilenameIn));

      }
    else
      {
      return (ReadBuffer (parserBuffer,
                          szHeirarchyPrefixIn,
                          pwldWorldIn,
                          bLoadAnim,
                          szFilenameIn));
      };
    };
  return (EStatus::Failure ("Unable to load scene from file."));
  };






//-----------------------------------------------------------------------------
EStatus  SceneLoader::ReadBuffer  (RStrParser &  parserBufferIn,
                                   const char *  szHeirarchyPrefixIn,
                                   World *       pwldWorldIn,
                                   BOOL          bLoadAnim,
                                   const char *  szFilenameIn)
  {
  EStatus      status = EStatus::kSuccess;
  RStr         strKey;
  RStrParser   parserValue;

  // step through buffer

  parserBufferIn.SetSkipComments (RStrParser::kCStyle);
  parserBufferIn.SkipWhitespace ();
  while (! parserBufferIn.IsEOFAscii ())
    {
    // read the key and value pairs
    INT  iSavedPos = parserBufferIn.GetCursorStart ();
    RStr  strLine = parserBufferIn.GetLine ();
    parserBufferIn.SetCursorStart (iSavedPos);
    //DBG_INFO ("ReadBuffer: Parsing line: \"%s\"", strLine.AsChar ());

    if (ParseTools::IsKVPColon (parserBufferIn, strKey, parserValue))
      {
      // The key is a top-level definition
      //DBG_INFO ("ReadBuffer: Keyword Line \"%s\" \"%s\"", strKey.AsChar (), parserValue.AsChar ());

      if (strKey == "include")
        {
        if ((status = ParseInclude (parserBufferIn,
                                    parserValue,
                                    szHeirarchyPrefixIn,
                                    pwldWorldIn,
                                    bLoadAnim)) != EStatus::kSuccess)
          {
          break;
          };
        }
      else if ((strKey == "int") || (strKey == "bool") || (strKey == "float") || (strKey == "string"))
        {
        if ((status = ParseValueRegistry (strKey, parserValue)) != EStatus::kSuccess)
          {
          break;
          };
        }
      else if (strKey == "node")
        {
        if ((status = ReadNode (parserBufferIn, parserValue, szHeirarchyPrefixIn, pwldWorldIn)) != EStatus::kSuccess)
          {
          break;
          };
        }
      else if (strKey == "animclip")
        {
        if ((status = ReadAnimClip (parserBufferIn, "curve", parserValue, szHeirarchyPrefixIn, szFilenameIn)) != EStatus::kSuccess)
          {
          break;
          };
        };
      }
    else
      {
      // keyword not found
      status = EStatus::Failure ("Unable to parse keyword \"%s\"", strKey.AsChar ());
      break;
      }
    parserBufferIn.SkipWhitespace ();
    };

  PrintError (status, parserBufferIn, szFilenameIn);
  return (status);
  };

//-----------------------------------------------------------------------------
EStatus  SceneLoader::ReadAnimFile  (const char *    szFilenameIn)
  {
  EStatus    errorStatus = EStatus::kSuccess;

  // make sure the file exists.
  if (! FilePath::FileExists (szFilenameIn))
    {
    return (EStatus::Failure (RStr("File does not exist: ") + szFilenameIn));
    };
//  DBG_INFO ("SceneLoader::ReadAnimFile (): Reading %s", szFilenameIn);

  RStrParser   parserBuffer;

  // slurp the file in
  errorStatus = parserBuffer.ReadFromFile (szFilenameIn);

  if (errorStatus == EStatus::kSuccess)
    {
    if (!strDecryptKey.IsEmpty ())
      {
      RC4         rc4;
      RStrParser  parserDecoded;

      rc4.InitWithKey (strDecryptKey);
      rc4.Decode (parserBuffer.AsChar (), parserBuffer.Length (), parserDecoded);

      return (ReadAnimBuffer (parserDecoded, szFilenameIn));
      }
    else
      {
      return (ReadAnimBuffer (parserBuffer, szFilenameIn));
      }

    };
  return (EStatus::Failure ("Unable to load anim clips from file."));
  };

//-----------------------------------------------------------------------------
EStatus  SceneLoader::ReadAnimBuffer  (RStrParser &    parserBufferIn,
                                       const char *    szFilenameIn)
  {
  // REFACTOR:  This needs to be a configurable static parameter
  const char *  szClipLibraryBase = "assets/gfx/anim/";
  RStrParser    parserFilename (szFilenameIn);

  // extract the animation library name from the filename
  INT  iBaseDir = parserFilename.Find (szClipLibraryBase);

  if (iBaseDir != -1)
    {
    parserFilename.TruncateLeft (iBaseDir + strlen (szClipLibraryBase));
    INT  iExt = parserFilename.ReverseFindChar ('.');
    if (iExt != -1)
      {
      parserFilename.TruncateRight (iExt - 1);
      };
    };

//  DBG_INFO ("Reading Clip %s", parserFilename.AsChar ());
  return (ReadBuffer (parserBufferIn, parserFilename.AsChar (), NULL, TRUE, szFilenameIn));
  };

//-----------------------------------------------------------------------------
EStatus SceneLoader::ParseInclude (RStrParser &        parserBufferIn,
                                   const RStrParser &  parserValueIn,
                                   const char *        szHeirarchyPrefixIn,
                                   World *             pwldWorldIn,
                                   BOOL                bLoadAnim)
  {
  RStrParser  parserHeirarchyPrefix;
  RStrParser  parserFilePath;

  parserHeirarchyPrefix = parserValueIn;
  // the "include" directive will have a second quoted string on the line.
  parserBufferIn.GetQuoteString (&parserFilePath);

  // concatenate the current heirarchy path with the relative heirarchy path of the include directive.
  parserHeirarchyPrefix.PrependString (szHeirarchyPrefixIn);

  EStatus  statusInclude = ReadFile  (parserFilePath.AsChar (),
                                      parserHeirarchyPrefix.AsChar (),
                                      pwldWorldIn,
                                      bLoadAnim);
  if (statusInclude == EStatus::kFailure)
    {
    // Error!!!
    DBG_ERROR ("Failed to parse file %s", parserFilePath.AsChar ());
    return (statusInclude);
    };
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus SceneLoader::ParseValueRegistry (const RStr &  strKeyIn,
                                         RStrParser &  parserValueIn)
  {
  RStr        strErrorOut;
  RStr        strVarKey;
  RStrParser  parserVarValue;

  if (!ParseTools::IsKVPEquals (parserValueIn, strVarKey, parserVarValue))
    {
    return (EStatus::Failure ("Unable to parse value assignment"));
    };

  if (ValueRegistry::Root() != NULL)
    {
    if (strKeyIn == "int")
      {
      ValueRegistry::Root()->SetInt (strVarKey.AsChar(), parserVarValue.GetInt());
      }
    else if (strKeyIn == "float")
      {
      ValueRegistry::Root()->SetFloat (strVarKey.AsChar(), parserVarValue.GetFloat());
      }
    else if (strKeyIn == "string")
      {
      ValueRegistry::Root()->SetString (strVarKey.AsChar(), parserVarValue.AsChar());
      }
    else if (strKeyIn == "bool")
      {
      if ((parserVarValue.CompareNoCase("true") == 0) || (parserVarValue == "1"))
        {
        ValueRegistry::Root()->SetBool (strVarKey.AsChar(), 1);
        }
      else if ((parserVarValue.CompareNoCase("false") == 0) || (parserVarValue == "0"))
        {
        ValueRegistry::Root()->SetBool (strVarKey.AsChar(), 0);
        }
      else
        {
        return (EStatus::Failure ("Unable to parse boolean expression."));
        }
      }
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus  SceneLoader::ReadComponentParam (const RStr &  strKeyIn,
                                          RStrParser &  parserValueIn,
                                          Component *   pcompCurr)
  {
  //DBG_INFO ("Found param %s", strKeyIn.AsChar ());

  // Make sure we have a component to load the attr onto.
  if (pcompCurr == NULL)
    {
    return (EStatus::Failure ("SceneLoader.ReadComponentParam (): No component defined for attr."));
    };

  RStrArray  arrayStrOut;
  RStr       strOut;

  if (parserValueIn.PeekChar () != '"')
    {
    // non-string
    pcompCurr->SetAttr (strKeyIn, parserValueIn.AsChar ());
    }
  else
    {
    // string or array
    while (!parserValueIn.IsEOL ())
      {
      parserValueIn.GetQuoteString (&strOut);

      arrayStrOut.Append (strOut);
      parserValueIn.SkipWhitespace ();
      if (parserValueIn.PeekChar () == ',')
        {
        parserValueIn.SkipChars (1);
        parserValueIn.SkipWhitespace ();
        continue;
        }
      break;
      };
    INT  iNumStrings = arrayStrOut.Length ();
    if (iNumStrings == 1)
      {
      pcompCurr->SetAttr (strKeyIn, arrayStrOut[0].AsChar ());
      }
    else
      {
      for (INT iIndex = 0; iIndex < iNumStrings; ++iIndex)
        {
        pcompCurr->SetArrayAttr (strKeyIn, iIndex, arrayStrOut[iIndex].AsChar ());
        }
      }
    }

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus SceneLoader::ReadNode (RStrParser &  parserBufferIn,
                               RStrParser &  parserNodeNameIn,
                               const char *  szHeirarchyPrefixIn,
                               World *       pwldWorldIn)
  {
  RStr         strKey;
  RStrParser   parserValue;
  RStr         strErrorOut;
  Node *       pnodeCurr = NULL;
  Component *  pcompCurr = NULL;
  EStatus      status;
  BOOL         bIsActive = TRUE; // nodes default to active


  // create the node and parse out the rest of its definition.
  //DBG_INFO ("ReadNode %s", parserNodeNameIn.AsChar ());

  if (parserNodeNameIn.IsEmpty ())
    {
    return (EStatus::Failure ("Unable to read node name."));
    };

  RStr  strFullNodePath (szHeirarchyPrefixIn);
  strFullNodePath += parserNodeNameIn;
  pnodeCurr = pwldWorldIn->CreateNode (strFullNodePath.AsChar ());

  // parse the rest of this node
  while (! parserBufferIn.IsEOFAscii ())
    {
    // read the key and value pairs
    // we only allow lines with "component:" or attrs

    INT  iSavedPos = parserBufferIn.GetCursorStart ();
    if (ParseTools::IsKVPColon (parserBufferIn, strKey, parserValue))
      {
      if (strKey == "component")
        {
        // component

        //DBG_INFO ("ReadComponent %s", parserValue.AsChar ());

        if (pnodeCurr != NULL)
          {
          if (pcompCurr != NULL) pcompCurr->SetLoading (FALSE);
          pcompCurr = pnodeCurr->AddComponent (parserValue.AsChar ());
          pcompCurr->SetLoading (TRUE);
          }
        else
          {
          return (EStatus::Failure ("No node defined."));
          };

        }
      else if (strKey == "active")
        {
        if      (parserValue.CompareNoCase ("true") == 0)   {bIsActive = TRUE;}
        else if (parserValue.CompareNoCase ("false") == 0)  {bIsActive = FALSE;}
        }
      else
        {
        // Not a line that is part of this node.  Backup to the start of the line and return.
        parserBufferIn.SetCursorStart (iSavedPos);
        break;
        };

      }
    else if (ParseTools::IsKVPBracket (parserBufferIn, strKey, parserValue))
      {
      // attr
      if ((status = ReadComponentParam (strKey, parserValue, pcompCurr)) != EStatus::kSuccess)
        {
        return (status);
        };
      };
    };

  if (pcompCurr != NULL) pcompCurr->SetLoading (FALSE);
  //DBG_INFO ("ReadNode done for %s (is active : %s)", pnodeCurr->Name (), bIsActive ? "true" : "false");

  pwldWorldIn->CreateNodeFinish (pnodeCurr, bIsActive);

  return (EStatus::kSuccess);
  };

/*
animclip: "walk"
  loop ["none"]
  loopCount [2]
  outLeft ["zero"]
  outRight ["clamp"]
  speed [1.0]
  curve: "leg.tx"
    key ["t=0.04166667 in=spline:20.4 out=spline:20.4 v=30.0"]
    key ["t=0.08333333 in=spline:20.4 out=spline:20.4 v=30.0"]
    key ["t=0.12500000 in=spline:20.4 out=spline:20.4 v=30.0"]
    key ["t=0.16666667 in=spline:20.4 out=spline:20.4 v=30.0"]
*/


// NOTE: This code should go into its own SceneLoaderAnim file
//-----------------------------------------------------------------------------
EStatus SceneLoader::ReadAnimClip (RStrParser &  parserBufferIn,
                                   const char *  szClipTypeIn,
                                   RStrParser &  parserAnimNameIn,
                                   const char *  szHeirarchyPrefixIn,
                                   const char *  szFilenameIn)
  {
  RStr            strKey;
  RStrParser      parserValue;
  RStr            strErrorOut;
  AnimClipBase *  pclipBase  = NULL;
  AnimClipCurve * pclipCurve = NULL;
  MappedCurve *   pCurve     = NULL;
  EStatus         status;


  //DBG_INFO ("ReadAnimClip");
  // create the clip and parse its info

  if ((szClipTypeIn == NULL) || (szClipTypeIn[0] == 0))
    {
    return (EStatus::Failure ("Clip type not defined."));
    };

  if (parserAnimNameIn.IsEmpty ())
    {
    return (EStatus::Failure ("Unable to read anim name."));
    };

  RStr  strFullName (szHeirarchyPrefixIn);
  // convert slashes to dots, to keep with ValueElem naming conventions/restrictions.
  strFullName.ReplaceChar ('/', '.');
  // If clip filename (from prefix) and clip name are the same, only keep one copy of the name.
  if (!strFullName.EndsWith (parserAnimNameIn))
    {
    if (strFullName.Length () > 0)
      {
      strFullName.AppendChar ('.');
      };
    strFullName.AppendString (parserAnimNameIn);
    };
//    DBG_INFO ("Clip named %s", strFullName.AsChar ());

  pclipBase = AnimManager::Instance()->NewLibraryClip (szClipTypeIn, strFullName, szFilenameIn);

  if (pclipBase == NULL)
    {
    return (EStatus::Failure ("Unable to create anim clip."));
    };

  // REFACTOR: The following line is hard-coded for "curve" type.
  ASSERT (streq (szClipTypeIn, "curve"));
  pclipCurve = static_cast<AnimClipCurve *>(pclipBase);

  // parse the rest of this curve
  while (! parserBufferIn.IsEOFAscii ())
    {
    // read the key and value pairs
    // we only allow lines with "curve:" or attrs

    INT  iSavedPos = parserBufferIn.GetCursorStart ();
    if (ParseTools::IsKVPColon (parserBufferIn, strKey, parserValue))
      {
//      DBG_INFO ("ReadAnimClip keyword line %s %s", strKey.AsChar (), parserValue.AsChar ());

      if (strKey == "curve")
        {
        // new curve in the clip
        pCurve = pclipCurve->NewCurve (parserValue.AsChar ());
        }
      else
        {
        // Not a line that is part of this node.  Backup to the start of the line and return.
        parserBufferIn.SetCursorStart (iSavedPos);
        break;
        };
      }
    else if (ParseTools::IsKVPBracket (parserBufferIn, strKey, parserValue))
      {
      // attr
//      DBG_INFO ("ReadAnimClip attr line %s %s", strKey.AsChar (), parserValue.AsChar ());
      if (pCurve == NULL)
        {
//        DBG_INFO ("null curve");

        // still reading attrs for clip
        if ((status = ReadAnimClipParams (strKey, parserValue, szHeirarchyPrefixIn, pclipBase)) != EStatus::kSuccess)
          {
          return (status);
          }
        }
      else
        {
//        DBG_INFO ("read for curve");
        if ((status = ReadMappedCurveParams (strKey, parserValue, pCurve)) != EStatus::kSuccess)
          {
          return (status);
          }
        };
      }
    else
      {
      //DBG_INFO ("ReadAnimClip line not part of clip");

      // Not a line that is part of this clip
      break;
      };
    };
  return (EStatus::kSuccess);
  };

// NOTE: This code should go into its own SceneLoaderAnim file
//-----------------------------------------------------------------------------
EStatus  SceneLoader::ReadAnimClipParams (const RStr &    strKeyIn,
                                          RStrParser &    parserValueIn,
                                          const char *    szHeirarchyPrefixIn,
                                          AnimClipBase *  pclipCurr)
  {

  if (pclipCurr == NULL)
    {
    return (EStatus::Failure ("SceneLoader.ReadAnimClipParams : No clip defined."));
    };

  //DBG_INFO ("Found anim clip param %s", strKeyIn.AsChar ());
  RStr       strValue;
  if (parserValueIn.PeekChar () != '"')
    {
    // non-string
    strValue.Set (parserValueIn);
    }
  else
    {
    // string
    parserValueIn.GetQuoteString (&strValue);
    }
  if (strKeyIn == "loop")
    {
    if (strValue == "none")    {pclipCurr->SetLoopType (EAnim::kLoopTypeNone);};
    if (strValue == "repeat")  {pclipCurr->SetLoopType (EAnim::kLoopTypeRepeat);};
    if (strValue == "bounce")  {pclipCurr->SetLoopType (EAnim::kLoopTypeBounce);};
    }
  else if (strKeyIn == "loopCount")
    {
    pclipCurr->SetLoopCount (parserValueIn.GetInt ());
    }
  else if (strKeyIn == "outLeft")
    {
    if (strValue == "ignored") {pclipCurr->SetRangeLeft (EAnim::kOutOfRangeIgnored);};
    if (strValue == "zero")    {pclipCurr->SetRangeLeft (EAnim::kOutOfRangeZero);};
    if (strValue == "clamp")   {pclipCurr->SetRangeLeft (EAnim::kOutOfRangeClamp);};
    }
  else if (strKeyIn == "outRight")
    {
    if (strValue == "ignored") {pclipCurr->SetRangeRight (EAnim::kOutOfRangeIgnored);};
    if (strValue == "zero")    {pclipCurr->SetRangeRight (EAnim::kOutOfRangeZero);};
    if (strValue == "clamp")   {pclipCurr->SetRangeRight (EAnim::kOutOfRangeClamp);};
    }
  else if (strKeyIn == "speed")
    {
    pclipCurr->SetSpeed (parserValueIn.GetFloat ());
    }
  return (EStatus::kSuccess);
  };


// NOTE: This code should go into its own SceneLoaderAnim file
//-----------------------------------------------------------------------------
EStatus  SceneLoader::ReadMappedCurveParams (const RStr &    strKeyIn,
                                             RStrParser &    parserValueIn,
                                             MappedCurve *   pCurveIn)
  {
  static RVec3    vecPoint;
  static RVec3    vecInTan;
  static RVec3    vecOutTan;
  RStr            strCurveKey;
  RStrParser      parserCurveValue;

    //key ["t=0.04166667 in=spline:20.4 out=spline:20.4 v=30.0"]

//  DBG_INFO ("ReadMappedCurveParams \"%s\"", parserValueIn.AsChar ());
  if (pCurveIn == NULL)
    {
    return (EStatus::Failure ("SceneLoader.ReadMappedCurveParams : No curve defined."));
    };

  RStr       strValue;

  if (strKeyIn == "key")
    {
    // init defaults
    vecPoint.Set  (0.0f, 0.0f, 0.0f);
    vecInTan.Set  (0.0f, 0.0f, 0.0f);
    vecOutTan.Set (0.0f, 0.0f, 0.0f);
    static RVec3 *  pvecInTanPtr  = NULL;
    static RVec3 *  pvecOutTanPtr = NULL;

    // The parser values are in a quoted string.  Let's just read straight out of that string.
    if (parserValueIn.PeekChar () == '"')
      {
      parserValueIn.SkipChars (1);
      parserValueIn.SkipWhitespace ();
      };

    while ((!parserValueIn.IsEOFAscii ()) && (parserValueIn.PeekChar () != '"'))
      {
      if (ParseTools::IsKVPEquals (parserValueIn, strCurveKey, parserCurveValue))
        {
//        DBG_INFO ("Parsed apart curve params \"%s\" \"%s\"", strCurveKey.AsChar (), parserCurveValue.AsChar ());

        if (strCurveKey == "t")
          {
          vecPoint.fX = parserCurveValue.GetFloat ();
//          DBG_INFO ("Setting key time %f", vecPoint.fX);
          }
        else if (strCurveKey == "in")
          {
          vecInTan.fX = parserCurveValue.GetFloat ();
          if (parserCurveValue.PeekChar () == ':')
            {
            parserCurveValue.SkipChars (1);
            parserCurveValue.SkipWhitespace ();
            vecInTan.fY = parserCurveValue.GetFloat ();
            };
          pvecInTanPtr = &vecInTan;
          }
        else if (strCurveKey == "out")
          {
          vecOutTan.fX = parserCurveValue.GetFloat ();
          if (parserCurveValue.PeekChar () == ':')
            {
            parserCurveValue.SkipChars (1);
            parserCurveValue.SkipWhitespace ();
            vecOutTan.fY = parserCurveValue.GetFloat ();
            };
          pvecOutTanPtr = &vecOutTan;
          };
        };
      };
    pCurveIn->AddPoint (vecPoint, pvecInTanPtr, pvecOutTanPtr, Curve::kInsertByX);
    }
  return (EStatus::kSuccess);
  };

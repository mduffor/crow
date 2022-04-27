
#ifndef ANIMMANAGER_HPP
#define ANIMMANAGER_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Containers/TList.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Gfx/Curve.hpp"
#include "Script/Expression.hpp"


/*

//  TODO: Play clip
//  TODO: When clip plays, attempt to use channel and curve names to map to
//         existing ValueElem.  ValueElem must exist before-hand.
//  TODO: Mapping of MappedCurve to ValueElem.  Drive ValueElem.
//        Use AnimChan name as part of map name.

//  TODO: Add ability to run expression when clip advances past end of anim.
//  TODO: Make sure you can find curve clips, and curves within them.
//         Promote up keyframe commands for ease of use.
//  TODO: Deserialize is part of SceneLoader.  Write a Serialize (ToString) method in
            SceneLoader for anim curves.

// FUTURE:
//  TODO: Drive Spriter animations with AnimMaster
//  TODO: Drive sound playback with AnimMaster

//  TODO: Build animation clips on the fly for character positions.

//  TODO: May need to build in string look up tables.
//  TODO: May need to support setting of strings and const values period.
//  TODO: Need to figure out how it interfaces with state machine
//  TODO: May need to track velocity of curves for secondary effects.
//  TODO: Need to figure out how to merge animation system with instancing and tracking of models/rigs.
*/



//-----------------------------------------------------------------------------
class EAnim
  {
  public:
    enum LoopType
      {
      kLoopTypeNone   = 0,
      kLoopTypeRepeat = 1,
      kLoopTypeBounce = 2
      };

    // NOTE:  Not currently handling "maintain trajectory" out of range.
    enum OutOfRange
      {
      kOutOfRangeIgnored  = 0,  ///< out of range results should be ignored
      kOutOfRangeZero     = 1,  ///< returns a 0.0f if out of range
      kOutOfRangeClamp    = 2   ///< returns the value at normTime==0 if left of range, and normTime==1 if right out of range.
      };

    enum QueuePos
      {
      kQueuePosBack      = 0,
      kQueuePosXFadeBack = 1,
      kQueuePosFront     = 2,
      kQueuePosOverlap   = 3,
      kQueuePosReplace   = 4
      };
  };

//-----------------------------------------------------------------------------
class AnimClipBase
  {
  private:
    // NOTE:  These are clip attributes, that exist on the library clip and do
    //         not change when played.  They are shared by all playing instances
    //         of the clip.
    RStr                          strName; // Try to remove this variable in the
                                           //   long run, so we can have pools instead
                                           //   of allocation.

    UINT32                        uNameHash;

    EAnim::OutOfRange             eOutOfRangeLeft;  // Not sure if this is a clip-level thing or a curve-level thing.  Determine after you use it.
    EAnim::OutOfRange             eOutOfRangeRight;
    RStr                          strLibraryFile;

    RStr *                        pstrExprOnClipDone;  ///< Expression to run when clip is done playing.
    TList<Token*>  *              plistExprOnClipDone;

  protected:
    // NOTE:  These are instance attributes, that exist on instanced clips once they are played.
    FLOAT                         fStartSec;
    INT                           iTimesToLoop;  // -1 is infinite looping, otherwise gives the number of times clip should loop.
    FLOAT                         fTime;  // count from 0, including start sec offset.  Not clip normalized.  Given in Seconds.
    FLOAT                         fSpeed; // speed of clip.  2.0 == plays twice as fast.
    AnimClipBase *                pLibraryClip;  // Library template this was instantiated from.
    EAnim::LoopType               eLoopType;
    FLOAT                         fFadeIn;  // TODO: Implement
    FLOAT                         fFadeOut; // TODO: Implement
    UINT32                        uType;

  protected:

    VOID           Init           (VOID);


  public:
    // These calls are made on clips in the library

    explicit       AnimClipBase   (const char *    szAnimNameIn);

    explicit       AnimClipBase   (AnimClipBase *  pLibClipIn);

    virtual        ~AnimClipBase  ();

    VOID           SetName        (const char *  szAnimNameIn);

    const char *   Name           (VOID)                          {return strName.AsChar ();};

    AnimClipBase * Ref            (VOID)                          {return ((pLibraryClip == NULL) ? this : pLibraryClip);};

    UINT32         Type           (VOID)                          {return uType;};

    UINT32         NameHash       (VOID)                          {ASSERT (pLibraryClip == NULL);
                                                                   return uNameHash;};

    VOID           SetLibraryFile (const char *  szFilenameIn)    {ASSERT (pLibraryClip == NULL);
                                                                   strLibraryFile.Set (szFilenameIn, TRUE);};

    const char *   GetLibraryFile (VOID)                          {ASSERT (pLibraryClip == NULL);
                                                                   return (strLibraryFile.AsChar ());};

    UINT32         LibNameHash    (VOID)                          {ASSERT (pLibraryClip == NULL);
                                                                   return (strLibraryFile.GetHash ());};

    BOOL           InLibraryFile  (UINT32        uLibNameHashIn)  {ASSERT (pLibraryClip == NULL);
                                                                   return (strLibraryFile.GetHash () == uLibNameHashIn);};

    VOID           SetLoopType    (EAnim::LoopType    eTypeIn)    {eLoopType = eTypeIn;};

    VOID           SetLoopCount   (INT                iCountIn)   {iTimesToLoop = iCountIn;};

    VOID           SetRangeLeft   (EAnim::OutOfRange  eOutIn)     {ASSERT (pLibraryClip == NULL);
                                                                   eOutOfRangeLeft  = eOutIn;};

    VOID           SetRangeRight  (EAnim::OutOfRange  eOutIn)     {ASSERT (pLibraryClip == NULL);
                                                                   eOutOfRangeRight = eOutIn;};

    virtual AnimClipBase *  LinkedInstance        (VOID)          {return NULL;};

    public:
      // These calls are for instantiated, playing clips

    VOID           SetSpeed       (FLOAT  fSpeedIn)          {fSpeed = fSpeedIn;};

    virtual FLOAT  GetLength      (VOID)                     {return 0.0f;};  // returns the length of a single loop

    VOID           SetStartSec    (FLOAT  fStartIn)          {fStartSec = fStartIn;};

    FLOAT          GetStartSec    (VOID)                     {return fStartSec;};

    FLOAT          GetEndSec      (VOID);

    FLOAT          GetTime        (VOID)                     {return fTime;};

    VOID           SetTime        (FLOAT  fTimeIn)           {fTime = fTimeIn;};

    virtual VOID   IncTime        (FLOAT            fTimeDeltaIn,
                                   ValueRegistry *  pRegIn);

    FLOAT          GetClippedTime (VOID);  // clipped and transformed to 0.0f - GetEndSec () range.

    BOOL           IsTimeInClip   (VOID);

    BOOL           IsBeforeClip   (VOID);

    BOOL           IsAfterClip    (VOID);

    VOID           SetFadeIn      (FLOAT  fSecIn)   {fFadeIn  = fSecIn;};

    VOID           SetFadeOut     (FLOAT  fSecIn)   {fFadeOut = fSecIn;};

    FLOAT          GetFadeIn      (VOID)            {return (fFadeIn);};

    FLOAT          GetFadeOut     (VOID)            {return (fFadeOut);};

    virtual VOID   PlaybackInit   (const char *     szChannelNameIn,
                                   ValueRegistry *  pRegIn)              {};

    VOID           SetOnDoneExpr  (const char *  pExpIn);

    VOID           OnClipDone     (ValueRegistry *  pRegIn);
  };


//-----------------------------------------------------------------------------
class MappedCurve : public Curve
  {
  // NOTE:  A Mapped Curve is a Curve that connects to a ValueElem by name and
  //          then drives its value when the curve animates/solves.
  private:
    RStr                 strBaseMapping;
    ValueElem *          pelemTarget;
    MappedCurve *        pRef; // use the mapping from this object, but the keyframes from the ref, if present

  public:
    explicit             MappedCurve       (const char *  szMapIn);


    virtual              ~MappedCurve      ();

    MappedCurve *        Ref               (VOID)                  {return (pRef == NULL) ? this : pRef;};

    VOID                 SetRef            (MappedCurve *  pcurveIn);

    BOOL                 HasMapping        (VOID)                  {return (!strBaseMapping.IsEmpty ());};

    const char *         GetMapping        (VOID)                  {return (strBaseMapping.AsChar ());};

    UINT32               GetMappingHash    (VOID)                  {return (strBaseMapping.GetHash ());};

    ValueElem *          GetTarget         (VOID)                  {return pelemTarget;};

    VOID                 AttachToTarget    (ValueElem *  pelemIn);

    VOID                 DetachFromTarget  (ValueElem *  pelemIn);

    VOID                 SetTime           (FLOAT        fTimeIn);


  };

//-----------------------------------------------------------------------------
class AnimClipCurve : public AnimClipBase
  {
  // NOTE:  AnimClipCurve stores one or more multiple named/mapped curves, and
  //         allows playing them as a unit.

  private:

    TList<MappedCurve*>    listCurves;

    // multiple curves and multiple mappings over a single time period.
    //  all clips start at 0, and clip goes until last keyframe of longest curve.

  public:

    explicit        AnimClipCurve  (const char *    szAnimNameIn);

    explicit        AnimClipCurve  (AnimClipBase *  pLibClipIn);

    virtual         ~AnimClipCurve ();

    AnimClipCurve * CurveRef       (VOID)                    {return ((pLibraryClip == NULL) ? this : dynamic_cast<AnimClipCurve *>(pLibraryClip));};

    MappedCurve *   NewCurve       (const char *  szMapIn)   {return listCurves.PushBack (new MappedCurve (szMapIn));};

    MappedCurve *   FindCurve      (const char *  szMapIn);

    AnimClipBase *  LinkedInstance (VOID) override;

    FLOAT           GetLength      (VOID) override;

    VOID            PlaybackInit   (const char *     szChannelNameIn,
                                    ValueRegistry *  pRegIn)          override;

    VOID            UnmapCurves    (VOID);

    VOID            DeleteCurves   (VOID);

    virtual VOID    IncTime        (FLOAT            fTimeDeltaIn,
                                    ValueRegistry *  pRegIn)         override;

  };

//-----------------------------------------------------------------------------
class AnimChan
  {
  private:
    RStr                          strName; // Try to remove this variable in the
                                           //   long run, so we can have pools instead
                                           //   of allocation.

    UINT32                        uNameHash;

    TList<AnimClipBase*>          listClips;

    BOOL                          bDiscardPlayedClips;
    BOOL                          bPaused;
    FLOAT                         fFForwardScale; // if fast forwarding, this is larger than 1.0
    BOOL                          bFForwardClip;
    FLOAT                         fTime;

  public:
    explicit         AnimChan        (const char *  szChanNameIn);

                     ~AnimChan       ();

    VOID             DeleteAllClips  (VOID);

    //AnimClipBase *   FindClip        (const char *  szAnimNameIn); // is this needed in channels?

    //AnimClipBase *   FindClip        (UINT32  uAnimNameHashIn);// is this needed in channels?

    INT              NumAnims        (VOID)                     {return (listClips.Size ());};

    AnimClipBase *   GetClipByIndex  (INT     iIndexIn);

    VOID             PushClipFront   (AnimClipBase *  pClipIn)  {listClips.PushFront (pClipIn);};

    VOID             PushClipBack    (AnimClipBase *  pClipIn)  {listClips.PushBack  (pClipIn);};

    const char *     Name            (VOID)                     {return (strName.AsChar ());};

    UINT32           NameHash        (VOID)                     {return uNameHash;};

    FLOAT            GetTime         (VOID)                     {return fTime;};

    VOID             IncTime         (FLOAT            fTimeDeltaIn,
                                      ValueRegistry *  pRegIn);

    VOID             SetTime         (FLOAT   fTimeIn);

    FLOAT            GetLength       (VOID); // returns the end time of the last clip

    VOID             Pause           (VOID);  // pause playback of current channel

    VOID             Play            (VOID);  // start/resume playback of current channel

    VOID             Stop            (VOID);  // stop current clip, discard it and all future clips

    VOID             Skip            (ValueRegistry *  pRegIn);  // jump time ahead to the end of the current clip

    VOID             FForward        (FLOAT  fSpeedMultIn);  // play at a different speed for the remainder of the channel

    VOID             FForwardClip    (FLOAT  fSpeedMultIn);  // play at a different speed for the duration of the current clip

    VOID             Clear           (VOID);  // delete all clips from this channel.

  };

//-----------------------------------------------------------------------------
class AnimManager
  {
  private:

    TList<AnimChan*>             listChans;

    static AnimManager *         pInstance;

    static TList<AnimClipBase*>  listClipLibrary; // clips that are loaded from disk and can be instantiated.

    ValueRegistry *              pReg;

  public:

    AnimChan *            FindChan         (const char *  szChanNameIn);

    AnimChan *            FindChan         (UINT32        uChanNameHash);

    //AnimClipBase *        FindClip         (const char *  szAnimNameIn,
    //                                        const char *  szChanNameIn);

    //AnimClipBase *        FindClip         (UINT32        uAnimNameHashIn,
    //                                        UINT32        uChanNameHashIn);

  public:

    static AnimClipBase * FindLibraryClip  (UINT32        uAnimNameHashIn);


                           AnimManager          ();

                           ~AnimManager         ();

    static AnimManager*    Instance             (VOID)    {if (pInstance == NULL) {pInstance = new AnimManager;}; return pInstance;};

    static VOID            DestroyInstance      (VOID)    {if (pInstance != NULL) {delete pInstance;}; pInstance = NULL;};

    VOID                   SetValueRegistry     (ValueRegistry *  pRegIn)    {pReg = pRegIn;};

    // NOTE: Anim with no chan go in default, unnamed chan.

    VOID                   IncTime              (FLOAT            fTimeDeltaIn);

    VOID                   PlayClip             (const char *     szAnimNameIn,
                                                 const char *     szChanNameIn,
                                                 FLOAT            fDelaySecIn  = 0.0f,
                                                 FLOAT            fSpeedIn     = 1.0f,
                                                 EAnim::LoopType  eLoopingIn   = EAnim::kLoopTypeNone,
                                                 EAnim::QueuePos  eQueueIn     = EAnim::kQueuePosBack,
                                                 INT              iLoopCountIn = -1,
                                                 FLOAT            fFadeInSec   = 0,
                                                 FLOAT            fFadeOutSec  = 0);

    // XFade : (can scale chans separeately, and sum all results)

    VOID                   PauseChan            (const char *  szChanNameIn);

    VOID                   PlayChan             (const char *  szChanNameIn); // unpause

    VOID                   StopChan             (const char *  szChanNameIn);

    VOID                   SkipClip             (const char *  szChanNameIn);  // skip current clip to next one

    VOID                   FForwardChan         (const char *  szChanNameIn,
                                                 FLOAT         fSpeedMultIn); // possibly also as time, rather than speed mult.

    VOID                   FForwardClip         (const char *  szChanNameIn,
                                                 FLOAT         fSpeedMultIn); // possibly also as time, rather than speed mult.

    AnimChan *             NewChan              (const char *  szChanNameIn);

    INT                    NumChans             (VOID);

    INT                    NumAnims             (const char *  szChanNameIn);

    FLOAT                  GetClipLengthSec     (INT           iClipIndex,
                                                 const char *  szChanNameIn);

    FLOAT                  GetChanLengthSec     (const char *  szChanNameIn);

    FLOAT                  GetClipRemainingSec  (INT           iClipIndex,
                                                 const char *  szChanNameIn);

    FLOAT                  GetChanRemainingSec  (const char *  szChanNameIn);

    FLOAT                  GetClipTimeSec       (INT           iClipIndex,
                                                 const char *  szChanNameIn);  // gets current time

    FLOAT                  GetChanTimeSec       (const char *  szChanNameIn);

    UINT32                 GetClipByIndex       (INT           iIndexIn,
                                                 const char *  szChanNameIn);

    UINT32                 GetChanByIndex       (INT           iIndexIn);

    VOID                   ClearChan            (const char *  szChanNameIn);  // remove all clips from channel

    VOID                   DeleteChan           (const char *  szChanNameIn);  // clear chan, and then delete it from list of channels

    VOID                   DeleteAllChans       (VOID);

    // TODO:  Need method to unload all clip libraries

    static VOID            UnloadClipLibrary    (const char *  szLibNameIn);

    static VOID            UnloadAllClipLibraries (VOID);

    static AnimClipBase *  NewLibraryClip       (const char *  szTypeIn,
                                                 const char *  szNameIn,
                                                 const char *  szLibNameIn);

    VOID                   BuildLibraryList     (RStrArray &     arrayLibsOut);

    AnimClipBase *         GetNextClipInLib     (UINT32          uLibNameHashIn,
                                                 AnimClipBase *  pPrevIn);


    // Need way to set up ValueElem mapping

    // need event association/setting here.
    // need keyframe commands here


  };







/*

Define clip structure.  seralize / deserialize.  load & discard libraries of clips.
Preload and auto=load

events.

Base clip definition.  Overlay on top of sprite rigs/clips and sound clips. (future)

Build animation clips on the fly for character positions.

Set keyframes.  Set curves.

May need to build in string look up tables.
May need to support setting of strings and const values period.
Need to figure out how it interfaces with state machine
May need to track velocity of curves for secondary effects.
Need to figure out how to merge animation system with instancing and tracking of models/rigs.
*/



#endif // ANIMMANAGER_HPP

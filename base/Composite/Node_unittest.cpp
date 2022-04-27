#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Util/CalcHash.hpp"
#include "Composite/Attr.hpp"
#include "Composite/Component.hpp"
#include "Composite/Attr.hpp"
#include "Composite/Node.hpp"


// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (Node, Basic)
  {

  // test out AddTemplate
  AttrTemplatesInitialize ();

  // test creation and identifiers
  Node* pNode = new Node();

  pNode->SetName ("Baggins");

  ASSERT_STREQ (pNode->Name (), "Baggins");
  ASSERT_TRUE (pNode->NameEquals (CalcHashValue("Baggins"), "Baggins"));
  ASSERT_FALSE (pNode->NameEquals (CalcHashValue("Bilbo"), "Bilbo"));

  ASSERT_TRUE (pNode->Equals (pNode));

  ASSERT_NE (pNode->ResourceID(), 0x12345);
  pNode->SetResourceID (0x12345);
  ASSERT_EQ (pNode->ResourceID(), 0x12345);

  // test parenting and pathing

  Node *  pnodeBilbo = new Node();
  Node *  pnodeShire = new Node();

  pnodeBilbo->SetName("Bilbo");
  pnodeShire->SetName("Shire");

  pNode->ParentTo (pnodeShire);
  pnodeBilbo->ParentTo (pNode);

  // Now we should have Shire->Baggins->Bilbo

  TListItr<Node*>  itrSearch = pnodeShire->FirstChild ();
  ASSERT_STREQ ((*itrSearch)->Name(), "Baggins");
  itrSearch = (*itrSearch)->FirstChild ();
  ASSERT_STREQ ((*itrSearch)->Name(), "Bilbo");

  ASSERT_EQ (pnodeShire->NumChildren (), 1);
  ASSERT_TRUE (pnodeShire->FindChild ("Baggins") != NULL);
  ASSERT_TRUE (pnodeShire->FindChild ("Frodo") == NULL);
  ASSERT_TRUE (pnodeShire->FindChild ("Bilbo") == NULL);
  ASSERT_TRUE (pnodeShire->FindByName ("Bilbo") != NULL);
  ASSERT_TRUE (pnodeShire->FindByPath ("Baggins|Bilbo") != NULL);

  // NOTE:  Could use more advanced tests for FindByName and FindByPath
  ASSERT_STREQ (pnodeShire->FindByName ("Bilbo")->Name (), "Bilbo");
  ASSERT_STREQ (pnodeShire->FindByPath ("Baggins|Bilbo")->Name (), "Bilbo");

  ASSERT_TRUE (pnodeBilbo->RootParent () != NULL);
  ASSERT_STREQ (pnodeBilbo->RootParent ()->Name (), "Shire");

  RStr  strFullPath;
  pnodeBilbo->CalcFullPath (strFullPath);
  ASSERT_STREQ (strFullPath.AsChar (), "|Shire|Baggins|Bilbo");

  ASSERT_STREQ (pnodeShire->FindByName (H("Bilbo", 0))->Name (), "Bilbo");
  ASSERT_STREQ (pnodeShire->FindByPath (H("Baggins|Bilbo", 0))->Name (), "Bilbo");

  // deleting the top node should delete all children.
  delete pnodeShire;
  AttrTemplatesUninitialize ();
  };


  // TODO:  AttrFloatArray, AttrIntArray, AttrStringArray
  // TODO:  Node, SceneLoader,
  // TODO:  ComponentDefaultLooper, NodeDelegate,
  // TODO:  NamedAttr,
  // TODO:  Resource, TriggerTracker, World

 /*





    VOID                  CalcResourcePath  (RStr &   strPathOut) const;

    Node *                FindByName        (const char *  pszNameIn);

    Node *                FindByPath        (const char *  pszPathIn);


    Component *           FirstComponent    (VOID) const              {return listComponents.Next ();};

    Component *           AddComponent      (const char *  szTypeIn);

    Component *           FindComponent     (const char *  szTypeIn,
                                             Component *   pSearchStartIn = NULL);

    Component *           FindComponentInParents  (const char *  szTypeIn,
                                                   Component *   pSearchStartIn = NULL);
    static VOID           AddComponentTemplate (Component *  componentIn);

    // To be implemented
    EStatus               Load              (RStrParser &  parserIn);

    EStatus               Save              (RStrParser &  parserIn,
                                             RStr          strLineIndentIn) const;




    BOOL                  IsAwake              (VOID) const   {return bAwake;};

    VOID                  SetAwake             (BOOL  bIn)    {bAwake = bIn;};

    BOOL                  IsActive             (VOID) const   {return bActive;};

    VOID                  SetActive            (BOOL  bIn);

    VOID                  SetComponentsActive  (BOOL  bIn);

    BOOL                  IsStarted            (VOID) const   {return bStarted;};

    VOID                  SetStarted           (BOOL  bIn)    {bStarted = bIn;};

    VOID                  Update               (VOID);

    VOID                  Awake                (VOID);

    VOID                  OnDelete     (VOID)  {sigOnDelete(this);};

    VOID                  SetCollider          (VOID *  pColliderIn)   {pCollider = pColliderIn;};

    VOID *                Collider             (BOOL  bRecurseTree = FALSE);

    TransformComponent *  Transform            (VOID)                  {return pcmpTransform;};

    VOID                  MarkTransformsDirty  (VOID);


  **/















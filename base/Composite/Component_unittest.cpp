#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Composite/Attr.hpp"
#include "Composite/Component.hpp"
#include "Composite/AttrInt.hpp"
#include "Composite/AttrFloat.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/AttrStringArray.hpp"
#include "Composite/Attr.hpp"
#include "Composite/SceneLoader.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md


//-----------------------------------------------------------------------------
class TestComponent : public Component
  {
  public:
                          TestComponent    ();
    virtual               ~TestComponent   ();

    static  const char *  Identifier       (VOID)                            {return "test";};

    virtual Component *   GetInterface     (const char *  szTypeIn)  override {if streq(szTypeIn, "test") return (dynamic_cast<Component *>(this)); return (NULL);};

    virtual Component *   Instantiate      (VOID) const              override {return new TestComponent;};

    virtual Attr *        SetAttr          (const char *  szNameIn,
                                            const char *  szValueIn) override;
  };


//------------------------------------------------------------------------------
TestComponent::TestComponent    ()
  {
  strType = "test";

  AddAttr ("MyInt", "int")->SetByString ("1");
  AddAttr ("MyFloat", "float")->SetByString ("2.000000");
  AddAttr ("MyString", "string")->SetByString ("three");
  AddAttr ("MyStringArray", "stringarray")->SetByString ("\"4\", \"5\"");
  AddAttr ("MyFloatArray", "floatarray")->SetByString ("45, 67");
  AddAttr ("MyIntArray", "intarray")->SetByString ("45, 67");
  };

//------------------------------------------------------------------------------
TestComponent::~TestComponent   ()
  {
  };


//-----------------------------------------------------------------------------
Attr *  TestComponent::SetAttr (const char *  szNameIn,
                                     const char *  szValueIn)
  {
  Attr *  pattrChanged = Component::SetAttr (szNameIn, szValueIn);
  return pattrChanged;
  }



//------------------------------------------------------------------------------
TEST (Component, Basic)
  {

  // test AddTemplate
  AttrTemplatesInitialize ();

  Component *  pNew = new TestComponent ();

  // Test Type, Version, and MarkAsDirty
  ASSERT_STREQ (pNew->Type(), "test");
  ASSERT_EQ (pNew->Version(), 1);

  pNew->MarkAsDirty ();
  ASSERT_NE (pNew->Version(), 1);

  // Test AddAttr, GetAttr, DeleteAttr, SetAttr, SetArrayAttr

  RStr  strTemp;

  ASSERT_STREQ (pNew->GetAttr ("MyInt")->GetAsString (&strTemp), "1");
  strTemp.Reset ();
  ASSERT_STREQ (pNew->GetAttr ("MyFloat")->GetAsString (&strTemp), "2.000000");
  strTemp.Reset ();
  ASSERT_STREQ (pNew->GetAttr ("MyString")->GetAsString (&strTemp), "three");
  strTemp.Reset ();
  ASSERT_STREQ (pNew->GetAttr ("MyStringArray")->GetAsString (&strTemp), "\"4\", \"5\"");
  strTemp.Reset ();
  ASSERT_STREQ (pNew->GetAttr ("MyFloatArray")->GetAsString (&strTemp), "45.000000, 67.000000");
  strTemp.Reset ();
  ASSERT_STREQ (pNew->GetAttr ("MyIntArray")->GetAsString (&strTemp), "45, 67");
  strTemp.Reset ();


  ASSERT_STREQ (pNew->SetAttr ("MyInt", "2")->GetAsString (&strTemp), "2");
  strTemp.Reset ();

  ASSERT_STREQ (pNew->SetArrayAttr ("MyIntArray", 0, "89")->GetAsString (&strTemp), "89, 67");
  strTemp.Reset ();


  pNew->DeleteAttr ("MyInt");
  ASSERT_TRUE (pNew->GetAttr ("MyInt") == NULL);

  // Test

  pNew->SetActive (FALSE);
  ASSERT_FALSE (pNew->IsActive());
  pNew->SetActive (TRUE);
  ASSERT_TRUE (pNew->IsActive());

  pNew->SetLoading (TRUE);
  ASSERT_TRUE (pNew->IsLoading());
  pNew->SetLoading (FALSE);
  ASSERT_FALSE (pNew->IsLoading());

  // TODO:  Make sure that Save/Load restores everything it needs to.
  //          Currently not implemented


  // Test list manipulation
  Component  listComponents;

  listComponents.MakeListSentinel ();


  Component *  pFirst  = listComponents.Instantiate ();
  Component *  pSecond = listComponents.Instantiate ();
  Component *  pThird  = listComponents.Instantiate ();


  pFirst->AddAttr ("Order", "int")->SetByString ("1");
  pSecond->AddAttr ("Order", "int")->SetByString ("2");
  pThird->AddAttr ("Order", "int")->SetByString ("3");

  listComponents.InsertAfter (pThird);
  listComponents.InsertAfter (pSecond);
  pSecond->InsertBefore (pFirst);

  Component *  pSearch = listComponents.GetFirstSibling ();
  ASSERT_TRUE (pSearch != NULL);

  ASSERT_STREQ (pSearch->GetAttr ("Order")->GetAsString (&strTemp), "1");
  strTemp.Reset ();

  pSearch = pSearch->Next ();
  ASSERT_STREQ (pSearch->GetAttr ("Order")->GetAsString (&strTemp), "2");
  strTemp.Reset ();

  pSearch = pSearch->Next ();
  ASSERT_STREQ (pSearch->GetAttr ("Order")->GetAsString (&strTemp), "3");
  strTemp.Reset ();

  pSearch = pSearch->Prev ();
  ASSERT_STREQ (pSearch->GetAttr ("Order")->GetAsString (&strTemp), "2");
  strTemp.Reset ();

  pSearch = pSearch->Next ();
  pSearch = pSearch->Next ();
  ASSERT_FALSE (pSearch->IsValid ());

  pSearch = listComponents.GetFirstSibling ();
  pSearch->Remove ();

  pSearch = listComponents.GetFirstSibling ();
  ASSERT_STREQ (pSearch->GetAttr ("Order")->GetAsString (&strTemp), "2");

  delete (pNew);
  delete (pFirst);
  delete (pSecond);
  delete (pThird);


  // TODO:  Test the following:
  //  virtual Component *   GetInterface (const char *  szTypeIn)  {return NULL;};
  //  virtual VOID          GetActions   (PtrArray &  apActionsOut);

  AttrTemplatesUninitialize ();
  };



//------------------------------------------------------------------------------
TEST (Component, SceneLoader)
  {
  AttrTemplatesInitialize ();
  Node::AddComponentTemplate (new TestComponent);

  // Instantiate
  ValueRegistry::SetRoot (new ValueRegistrySimple);

  SceneLoader loader;

  RStrParser  parserBufferScene (
    "// Comment line \n" \
    "\n" \
    "int: RegInt = 4\n" \
    "bool: RegTrue = True\n" \
    "bool: RegFalse = False\n" \
    "float: RegFloat = 5.0\n" \
    "string: RegString = \"Bilbo\"\n" \
    "\n" \
    "\n" \
    "node: \"|Placer\"\n" \
    "\n" \
    "node: \"|Placer|TestNode\"\n" \
    "  component: test\n" \
    "    MyInt [2]\n" \
    "    MyFloat [3.0]\n" \
    "    MyString [\"four\"]\n" \
    "    MyStringArray [\"5\", \"6\"]\n" \
    "    MyFloatArray [\"67, 78\"]\n" \
    "    MyIntArray [\"67, 78\"]\n" \
    "\n" \
    "animclip: \"curve\"\n" \
    "  name [\"walk\"]\n" \
    "  loop [\"none\"]\n" \
    "  loopCount [2]\n" \
    "  outLeft [\"zero\"]\n" \
    "  outRight [\"clamp\"]\n" \
    "  speed [1.0]\n" \
    "  curve: \"leg.tx\"\n" \
    "    key [\"t=0.04166667 in=spline:20.4 out=spline:20.4 v=30.0\"]\n" \
    "    key [\"t=0.08333333 in=spline:20.4 out=spline:20.4 v=30.0\"]\n" \
    "    key [\"t=0.12500000 in=spline:20.4 out=spline:20.4 v=30.0\"]\n" \
    "    key [\"t=0.16666667 in=spline:20.4 out=spline:20.4 v=30.0\"]\n" \
    "\n");

  RStrParser  parserBufferAnim (
    "// Comment line \n" \
    "\n" \
    "animclip: \"curve\"\n" \
    "  name [\"walk\"]\n" \
    "  loop [\"none\"]\n" \
    "  loopCount [2]\n" \
    "  outLeft [\"zero\"]\n" \
    "  outRight [\"clamp\"]\n" \
    "  speed [1.0]\n" \
    "  curve: \"leg.tx\"\n" \
    "    key [\"t=0.04166667 in=spline:20.4 out=spline:20.4 v=30.0\"]\n" \
    "    key [\"t=0.08333333 in=spline:20.4 out=spline:20.4 v=30.0\"]\n" \
    "    key [\"t=0.12500000 in=spline:20.4 out=spline:20.4 v=30.0\"]\n" \
    "    key [\"t=0.16666667 in=spline:20.4 out=spline:20.4 v=30.0\"]\n" \
    "\n");



  World *  pwldTheWorld = World::Instance();
  loader.ReadBuffer (parserBufferScene, "", pwldTheWorld);

  Node *  pNode = pwldTheWorld->FindNodeByPath ("|Placer|TestNode");
  ASSERT_TRUE (pNode != NULL);
  Component * pCmp = pNode->FindComponent ("test");
  ASSERT_TRUE (pCmp != NULL);

  RStr  strTemp;

  ASSERT_STREQ (pCmp->GetAttr ("MyInt")->GetAsString (&strTemp), "2");
  strTemp.Reset ();
  ASSERT_STREQ (pCmp->GetAttr ("MyFloat")->GetAsString (&strTemp), "3.000000");
  strTemp.Reset ();
  ASSERT_STREQ (pCmp->GetAttr ("MyString")->GetAsString (&strTemp), "four");
  strTemp.Reset ();
  ASSERT_STREQ (pCmp->GetAttr ("MyStringArray")->GetAsString (&strTemp), "\"5\", \"6\"");
  strTemp.Reset ();
  ASSERT_STREQ (pCmp->GetAttr ("MyFloatArray")->GetAsString (&strTemp), "67.000000, 78.000000");
  strTemp.Reset ();
  ASSERT_STREQ (pCmp->GetAttr ("MyIntArray")->GetAsString (&strTemp), "67, 78");
  strTemp.Reset ();

  ASSERT_EQ    (ValueRegistry::Root()->GetInt ("RegInt"), 4);
  ASSERT_EQ    (ValueRegistry::Root()->GetFloat ("RegFloat"), 5.0f);
  ASSERT_TRUE  (ValueRegistry::Root()->GetBool ("RegTrue"));
  ASSERT_FALSE (ValueRegistry::Root()->GetBool ("RegFalse"));
  ASSERT_STREQ (ValueRegistry::Root()->GetString ("RegString"), "Bilbo");



  loader.ReadAnimBuffer (parserBufferAnim, "res://assets/gfx/anim/avatar/run.anim");


  World::DestroyInstance();
  AnimManager::DestroyInstance();

  delete (ValueRegistry::Root ());
  Node::DeleteAllComponentTemplates ();
  AttrTemplatesUninitialize ();
  };












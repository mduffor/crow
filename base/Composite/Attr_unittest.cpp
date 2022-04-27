#include <gtest/gtest.h>
#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Composite/Attr.hpp"
#include "Composite/Component.hpp"
#include "Composite/AttrInt.hpp"
#include "Composite/AttrFloat.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/AttrStringArray.hpp"
#include "Composite/Attr.hpp"


// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (Attr, Attr)
  {

  // test out AddTemplate
  AttrTemplatesInitialize ();

  // Test creation from templates, constructors, and type strings.
  Attr *  pattrInt = Attr::New ("MyInt", "int");
  Attr *  pattrFloat = Attr::New ("MyFloat", "float");
  Attr *  pattrString = Attr::New ("MyString", "string");
  Attr *  pattrStringArray = Attr::New ("MyStringArray", "stringarray");

  ASSERT_TRUE (pattrInt != NULL);
  ASSERT_TRUE (pattrFloat != NULL);
  ASSERT_TRUE (pattrString != NULL);
  ASSERT_TRUE (pattrStringArray != NULL);

  ASSERT_STREQ (pattrInt->Type (), "int");
  ASSERT_STREQ (pattrFloat->Type (), "float");
  ASSERT_STREQ (pattrString->Type (), "string");
  ASSERT_STREQ (pattrStringArray->Type (), "stringarray");

  ASSERT_STREQ (pattrInt->Name (), "MyInt");
  ASSERT_STREQ (pattrFloat->Name (), "MyFloat");
  ASSERT_STREQ (pattrString->Name (), "MyString");
  ASSERT_STREQ (pattrStringArray->Name (), "MyStringArray");

  ASSERT_TRUE (pattrInt->IsCCType (AttrInt::CCIdentifier()));
  ASSERT_TRUE (pattrFloat->IsCCType (AttrFloat::CCIdentifier()));
  ASSERT_TRUE (pattrString->IsCCType (AttrString::CCIdentifier()));
  ASSERT_TRUE (pattrStringArray->IsCCType (AttrStringArray::CCIdentifier()));

  // Test SetByString/GetByString on Attr
  RStr  strTemp;

  pattrInt->SetByString ("3"); pattrInt->GetAsString (&strTemp); ASSERT_STREQ (strTemp.AsChar (), "3");
  strTemp.Reset ();
  pattrFloat->SetByString ("3"); pattrFloat->GetAsString (&strTemp); ASSERT_STREQ (strTemp.AsChar (), "3.000000");
  strTemp.Reset ();
  pattrString->SetByString ("3"); pattrString->GetAsString (&strTemp); ASSERT_STREQ (strTemp.AsChar (), "3");
  strTemp.Reset ();
  pattrStringArray->SetByString ("3"); pattrStringArray->GetAsString (&strTemp); ASSERT_STREQ (strTemp.AsChar (), "\"3\"");
  strTemp.Reset ();
  pattrStringArray->SetArrayByString (1, "4"); pattrStringArray->GetAsString (&strTemp); ASSERT_STREQ (strTemp.AsChar (), "\"3\", \"4\"");

  delete pattrInt;
  delete pattrFloat;
  delete pattrString;
  delete pattrStringArray;
  AttrTemplatesUninitialize ();
  };

//------------------------------------------------------------------------------
TEST (Attr, AttrInt)
  {
  AttrTemplatesInitialize ();

  // Instantiate
  AttrInt *  pattrInt = dynamic_cast<AttrInt *>(Attr::New ("MyInt", "int"));

  // Setters and getters
  pattrInt->Set (2);      ASSERT_EQ (pattrInt->Value (), 2);
  pattrInt->SetByInt (3); ASSERT_EQ (pattrInt->Value (), 3);

  // initialization
  AttrInt attrIntB ("");
  ASSERT_EQ (attrIntB.Value (), 0);

  AttrInt attrIntC ("", 7);
  ASSERT_EQ (attrIntC.Value (), 7);

  // assignment
  attrIntB = *pattrInt;
  ASSERT_EQ (attrIntB.Value (), 3);

  // set by assignment from object
  pattrInt->Set (4);
  attrIntB.Set (pattrInt);
  ASSERT_EQ (attrIntB.Value (), 4);

  // clear
  attrIntB.Clear ();
  ASSERT_EQ (attrIntB.Value (), 0);

  delete pattrInt;
  AttrTemplatesUninitialize ();
  };

//------------------------------------------------------------------------------
TEST (Attr, AttrFloat)
  {
  AttrTemplatesInitialize ();

  // Instantiate
  AttrFloat *  pattrFloat = dynamic_cast<AttrFloat *>(Attr::New ("MyFloat", "float"));

  // Setters and getters
  pattrFloat->Set (2.0f);      ASSERT_EQ (pattrFloat->Value (), 2.0f);

  // initialization
  AttrFloat attrFloatB ("");
  ASSERT_EQ (attrFloatB.Value (), 0.0f);

  AttrFloat attrFloatC ("", 7.0f);
  ASSERT_EQ (attrFloatC.Value (), 7.0f);

  // assignment
  attrFloatB = *pattrFloat;
  ASSERT_EQ (attrFloatB.Value (), 2.0f);

  // set by assignment from object
  pattrFloat->Set (4.0f);
  attrFloatB.Set (pattrFloat);
  ASSERT_EQ (attrFloatB.Value (), 4.0f);

  // clear
  attrFloatB.Clear ();
  ASSERT_EQ (attrFloatB.Value (), 0.0f);

  delete pattrFloat;
  AttrTemplatesUninitialize ();
  };

//------------------------------------------------------------------------------
TEST (Attr, AttrString)
  {
  AttrTemplatesInitialize ();

  // Instantiate
  AttrString *  pattrString = dynamic_cast<AttrString *>(Attr::New ("MyString", "string"));

  // Setters and getters
  pattrString->Set ("Hello World");      ASSERT_STREQ (pattrString->Value (), "Hello World");

  // initialization
  AttrString attrStringB ("");
  ASSERT_STREQ (attrStringB.Value (), "");

  AttrString attrFloatC ("", "FooBar");
  ASSERT_STREQ (attrFloatC.Value (), "FooBar");

  // assignment
  attrStringB = *pattrString;
  ASSERT_STREQ (attrStringB.Value (), "Hello World");

  // set by assignment from object
  pattrString->Set ("Cracker Jack");
  attrStringB.Set (pattrString);
  ASSERT_STREQ (attrStringB.Value (), "Cracker Jack");
  ASSERT_STREQ (attrStringB.GetString (), "Cracker Jack");

  // clear
  attrStringB.Clear ();
  ASSERT_STREQ (attrStringB.Value (), "");


  delete pattrString;
  AttrTemplatesUninitialize ();
  };


  // TODO:  AttrFloatArray, AttrIntArray, AttrStringArray
  // TODO:  Node, SceneLoader,
  // TODO:  ComponentDefaultLooper, NodeDelegate,
  // TODO:  NamedAttr,
  // TODO:  Resource, TriggerTracker, World

















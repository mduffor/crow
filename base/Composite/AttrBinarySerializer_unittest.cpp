#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Composite/Component.hpp"
#include "Composite/AttrBinarySerializer.hpp"


// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (AttrBinarySerializer, Basic)
  {

  // test out AddTemplate
  AttrTemplatesInitialize ();

  TList<Attr*>  listAttrs;

  listAttrs.PushBack(Attr::New ("MyInt", "int"))->SetByString ("1");
  listAttrs.PushBack(Attr::New ("MyFloat", "float"))->SetByString ("2.000000");
  listAttrs.PushBack(Attr::New ("MyString", "string"))->SetByString ("three");
  listAttrs.PushBack(Attr::New ("MyStringArray", "stringarray"))->SetByString ("\"4\", \"5\"");
  listAttrs.PushBack(Attr::New ("MyFloatArray", "floatarray"))->SetByString ("45, 67");
  listAttrs.PushBack(Attr::New ("MyIntArray", "intarray"))->SetByString ("45, 67");

  AttrBinarySerializer  serializer;
  RStrParser  parserBuffer;

  ASSERT_TRUE (serializer.SaveList (parserBuffer, &listAttrs) == EStatus::kSuccess);

  // reset the cursor to the start of the buffer, so we read from the start.
  parserBuffer.ResetCursor();
  TList<Attr*>  listRestored;

  ASSERT_TRUE (serializer.LoadList (parserBuffer, &listRestored) == EStatus::kSuccess);

  TListItr<Attr*>  itr;
  RStr  strValue;
  ASSERT_EQ (listRestored.Size(), 6);
  for (itr = listRestored.First(); itr.IsValid(); ++itr)
    {
    strValue.Reset ();
    (*itr)->GetAsString (&strValue);

    if (streq ((*itr)->Name (), "MyInt"))
      {
      ASSERT_STREQ (strValue.AsChar (), "1");
      }
    else if (streq ((*itr)->Name (), "MyFloat"))
      {
      ASSERT_STREQ (strValue.AsChar (), "2.000000");
      }
    else if (streq ((*itr)->Name (), "MyString"))
      {
      ASSERT_STREQ (strValue.AsChar (), "three");
      }
    else if (streq ((*itr)->Name (), "MyStringArray"))
      {
      ASSERT_STREQ (strValue.AsChar (), "\"4\", \"5\"");
      }
    else if (streq ((*itr)->Name (), "MyIntArray"))
      {
      ASSERT_STREQ (strValue.AsChar (), "45, 67");
      }
    else if (streq ((*itr)->Name (), "MyFloatArray"))
      {
      ASSERT_STREQ (strValue.AsChar (), "45.000000, 67.000000");
      }
    else
      {
      // List contains extra info that shouldn't be there.
      ASSERT_TRUE (FALSE);
      }
    }

  // cleanup
  for (itr = listAttrs.First(); itr.IsValid(); ++itr)
    {
    delete (*itr);
    };
  for (itr = listRestored.First(); itr.IsValid(); ++itr)
    {
    delete (*itr);
    };
  AttrTemplatesUninitialize ();
  };















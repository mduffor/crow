#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "ValueRegistry/ValueRegistrySimple.hpp"
#include "Script/Expression.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md


//------------------------------------------------------------------------------
TEST (Expression, SimpleOperators)
  {
  RStr  strInput ("1 + 2 - 3 + 4");

  ValueRegistrySimple  registry;
  FLOAT                fResult;

  TList<Token*> *  pTokenized = Expression::Tokenize (strInput.AsChar ());
  //Expression::DebugTokenList (pTokenized);
  TList<Token*> *  pCompiled  = Expression::Compile (pTokenized);
  //Expression::DebugTokenList (pCompiled);
  fResult = Expression::Execute (pCompiled, &registry);

  Expression::FreeTokenList (&pTokenized);
  Expression::FreeTokenList (&pCompiled);

  ASSERT_EQ (fResult, 4.0f);
  //ASSERT_STREQ (strOut.AsChar (), szEncA);
  };


//------------------------------------------------------------------------------
TEST (Expression, Parenthesis)
  {
  RStr  strInput ("1 + (2 - 3) * 4");

  ValueRegistrySimple  registry;

  TList<Token*> *  pTokenized = Expression::Tokenize (strInput.AsChar ());
  TList<Token*> *  pCompiled  = Expression::Compile (pTokenized);
  FLOAT fResult = Expression::Execute (pCompiled, &registry);

  Expression::FreeTokenList (&pTokenized);
  Expression::FreeTokenList (&pCompiled);

  ASSERT_EQ (fResult, -3.0f);
  };

//------------------------------------------------------------------------------
TEST (Expression, Registry)
  {
  RStr  strInput ("four");

  ValueRegistrySimple  registry;

  registry.SetFloat ("four", 4.0f);

  TList<Token*> *  pTokenized = Expression::Tokenize (strInput.AsChar ());
  TList<Token*> *  pCompiled  = Expression::Compile (pTokenized);
  FLOAT fResult = Expression::Execute (pCompiled, &registry);

  Expression::FreeTokenList (&pTokenized);
  Expression::FreeTokenList (&pCompiled);

  ASSERT_EQ (fResult, 4.0f);
  };

//------------------------------------------------------------------------------
TEST (Expression, Variables)
  {
  RStr  strInput ("1 + (2 - 3) * four");

  ValueRegistrySimple  registry;
  registry.SetFloat ("four", 4.0f);

  TList<Token*> *  pTokenized = Expression::Tokenize (strInput.AsChar ());
  TList<Token*> *  pCompiled  = Expression::Compile (pTokenized);
  FLOAT fResult = Expression::Execute (pCompiled, &registry);

  Expression::FreeTokenList (&pTokenized);
  Expression::FreeTokenList (&pCompiled);

  ASSERT_EQ (fResult, -3.0f);
  };


//------------------------------------------------------------------------------
TEST (Expression, Returns)
  {
  ValueRegistrySimple  registry;
  registry.SetFloat ("four", 4.0f);
  TList<Token*> *  pCompiled  = NULL;


  pCompiled = Expression::Compile ("5");
  ASSERT_EQ (5.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("four");
  ASSERT_EQ (4.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5;");
  ASSERT_EQ (5.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("four;");
  ASSERT_EQ (4.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5; four");
  ASSERT_EQ (4.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("four; 5");
  ASSERT_EQ (5.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);
  };


//------------------------------------------------------------------------------
TEST (Expression, Comparisons)
  {
  ValueRegistrySimple  registry;
  registry.SetFloat ("four", 4.0f);
  TList<Token*> *  pCompiled  = NULL;


  pCompiled = Expression::Compile ("5 == 5");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("4 != 5");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("4 < 5");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 > 4");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("4 <= 5");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 >= 4");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 <= 5");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 >= 5");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);


  pCompiled = Expression::Compile ("5 == 5.0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("4 != 5.0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("4 < 5.0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 > 4.0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("4 <= 5.0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 >= 4.0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 <= 5.0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 >= 5.0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);
  };


//------------------------------------------------------------------------------
TEST (Expression, LogicalOperations)
  {
  ValueRegistrySimple  registry;
  registry.SetInt ("four", 4);
  registry.SetInt ("five", 5);
  TList<Token*> *  pCompiled  = NULL;


  pCompiled = Expression::Compile ("1 && 1");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("1 || 1");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("1 && 0");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("1 || 0");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("0 && 1");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("0 || 1");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);


  pCompiled = Expression::Compile ("5 > 4 && 2 < 3");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 > 4 && 2 > 3");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 < 4 && 2 > 3");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);


  pCompiled = Expression::Compile ("5 > 4 || 2 < 3");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 > 4 || 2 > 3");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("5 < 4 || 2 > 3");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);


  pCompiled = Expression::Compile ("five == 5");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("four == 4");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("five == 5 && four == 4");
  ASSERT_EQ (1, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("(five == 5) && (four == 4)");
  ASSERT_EQ (1, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);
  };

//------------------------------------------------------------------------------
TEST (Expression, Assignment)
  {
  ValueRegistrySimple  registry;
  registry.SetFloat ("myvar", 4.0f);
  TList<Token*> *  pCompiled  = NULL;


  pCompiled = Expression::Compile ("myvar = 5; myvar;");
  ASSERT_EQ (5.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = 3 + 3; myvar;");
  ASSERT_EQ (6.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = 4; myvar += 3; myvar;");
  ASSERT_EQ (7.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = 4; myvar -= 3; myvar;");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = 4; myvar *= 2; myvar;");
  ASSERT_EQ (8.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = 4; myvar /= 2; myvar;");
  ASSERT_EQ (2.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);
  };

//------------------------------------------------------------------------------
TEST (Expression, StringAssignment)
  {
  ValueRegistrySimple  registry;
  registry.SetString ("mystr", "Hello");
  TList<Token*> *  pCompiled  = NULL;
  Token            tokResult;

  ASSERT_STREQ ("Hello", registry.GetString("mystr"));

  pCompiled = Expression::Compile ("mystr = \"World\"; mystr;");
  Expression::Execute (pCompiled, &registry, &tokResult);
  ASSERT_STREQ ("World", registry.GetString("mystr"));
  ASSERT_STREQ ("World", tokResult.AsString(&registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("mystr = \"Hello\"; mystr += \" World\"; mystr;");
  Expression::Execute (pCompiled, &registry, &tokResult);
  ASSERT_STREQ ("Hello World", registry.GetString("mystr"));
  ASSERT_STREQ ("Hello World", tokResult.AsString(&registry));
  Expression::FreeTokenList (&pCompiled);

  registry.SetString ("hello", "Hello");
  registry.SetString ("world", "World");


  pCompiled = Expression::Compile ("mystr = hello + \" \" + world; mystr;");
  Expression::Execute (pCompiled, &registry, &tokResult);
  ASSERT_STREQ ("Hello World", registry.GetString("mystr"));
  ASSERT_STREQ ("Hello World", tokResult.AsString(&registry));
  Expression::FreeTokenList (&pCompiled);
  };

//------------------------------------------------------------------------------
TEST (Expression, Functions)
  {
  ValueRegistrySimple  registry;
  registry.SetFloat ("myvar", 4.0f);
  TList<Token*> *  pCompiled  = NULL;


  pCompiled = Expression::Compile ("Min (3,4);");
  ASSERT_EQ (3.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = Min (5, 6); myvar;");
  ASSERT_EQ (5.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = Min (6, 5); myvar;");
  ASSERT_EQ (5.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = Lerp (0, 10, 0.5); myvar;");
  ASSERT_TRUE (FLT_APPROX_EQUAL (5.0f, Expression::Execute (pCompiled, &registry)));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myvar = Lerp (3, 10, 0.3); myvar;");

  ASSERT_TRUE (FLT_APPROX_EQUAL (5.1f, Expression::Execute (pCompiled, &registry)));
  Expression::FreeTokenList (&pCompiled);


  ASSERT_EQ (2.0f, Expression::Execute ("Eval ('1+1')", &registry));
  };

//------------------------------------------------------------------------------
TEST (Expression, Identifiers)
  {
  TList<Token*> *  pCompiled  = NULL;
  TListItr<Token*> itrCurr;

  pCompiled = Expression::Compile ("myIdentifier = true;");
  itrCurr = pCompiled->First ();
  ASSERT_TRUE ((*itrCurr) != NULL);
  ASSERT_EQ ((*itrCurr)->eType, TokenType::kVariable);
  ASSERT_STREQ ((*itrCurr)->strRaw.AsChar (), "myIdentifier");
  Expression::FreeTokenList (&pCompiled);


  pCompiled = Expression::Compile ("$myIdentifier = true;");
  itrCurr = pCompiled->First ();
  ASSERT_TRUE ((*itrCurr) != NULL);
  ASSERT_EQ ((*itrCurr)->eType, TokenType::kVariable);
  ASSERT_STREQ ((*itrCurr)->strRaw.AsChar (), "$myIdentifier");
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("${myIdentifier} = true;");
  itrCurr = pCompiled->First ();
  ASSERT_TRUE ((*itrCurr) != NULL);
  ASSERT_EQ ((*itrCurr)->eType, TokenType::kVariable);
  ASSERT_STREQ ((*itrCurr)->strRaw.AsChar (), "${myIdentifier}");
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("${myIdentifier}.Visible = true;");
  itrCurr = pCompiled->First ();
  ASSERT_TRUE ((*itrCurr) != NULL);
  ASSERT_EQ ((*itrCurr)->eType, TokenType::kVariable);
  ASSERT_STREQ ((*itrCurr)->strRaw.AsChar (), "${myIdentifier}.Visible");
  Expression::FreeTokenList (&pCompiled);


  // Env Var Expansion

  KVPArray  envVars;
  envVars.SetAt ("myIdentifier", "replaced");

  ValueRegistrySimple  registry;
  registry.SetFloat ("myIdentifier", 0.0f);
  registry.SetFloat ("replaced", 0.0f);


  pCompiled = Expression::Compile ("myIdentifier = 1.0f; myIdentifier;");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry, NULL, NULL, &envVars));
  ASSERT_EQ (0.0f, registry.GetFloat ("replaced"));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("$myIdentifier = 2.0f; myIdentifier;");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry, NULL, NULL, &envVars));
  ASSERT_EQ (1.0f, registry.GetFloat ("myIdentifier"));
  ASSERT_EQ (2.0f, registry.GetFloat ("replaced"));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("$myIdentifier = 3.0f; $myIdentifier;");
  ASSERT_EQ (3.0f, Expression::Execute (pCompiled, &registry, NULL, NULL, &envVars));
  ASSERT_EQ (1.0f, registry.GetFloat ("myIdentifier"));
  ASSERT_EQ (3.0f, registry.GetFloat ("replaced"));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("$myIdentifier = 4.0f; replaced;");
  ASSERT_EQ (4.0f, Expression::Execute (pCompiled, &registry, NULL, NULL, &envVars));
  ASSERT_EQ (1.0f, registry.GetFloat ("myIdentifier"));
  ASSERT_EQ (4.0f, registry.GetFloat ("replaced"));
  Expression::FreeTokenList (&pCompiled);

  //Expression::DebugTokenList (pCompiled);

  //DBG_INFO ((*itrCurr)->strRaw.AsChar ());
  };

//------------------------------------------------------------------------------
TEST (Expression, Booleans)
  {
  ValueRegistrySimple  registry;
  registry.SetBool ("myBool", False);
  TList<Token*> *  pCompiled  = NULL;


  pCompiled = Expression::Compile ("true");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("false");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("True;");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("False;");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("True;");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("False;");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myBool = True;");
  ASSERT_EQ (1.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);

  pCompiled = Expression::Compile ("myBool = False;");
  ASSERT_EQ (0.0f, Expression::Execute (pCompiled, &registry));
  Expression::FreeTokenList (&pCompiled);
  };



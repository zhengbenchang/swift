//===--- TypeChecker.h - Type Checking Class --------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
//  This file defines the TypeChecking class.
//
//===----------------------------------------------------------------------===//

#ifndef TYPECHECKING_H
#define TYPECHECKING_H

#include "swift/AST/AST.h"
#include "swift/AST/Diagnostics.h"

namespace swift {

class TypeChecker {
public:
  TranslationUnit &TU;
  ASTContext &Context;
  TypeChecker(TranslationUnit &TU) : TU(TU), Context(TU.Ctx) {}
  
  template<typename ...ArgTypes>
  InFlightDiagnostic diagnose(ArgTypes... Args) {
    return Context.Diags.diagnose(Args...);
  }
  
  bool validateType(ValueDecl *VD);
  bool validateType(Type T);

  bool semaFunctionSignature(FuncExpr *FE);
  bool semaTupleExpr(TupleExpr *TE);
  Expr *semaApplyExpr(ApplyExpr *E);
  Expr *semaUnresolvedDotExpr(UnresolvedDotExpr *E);
  void typeCheckIgnoredExpr(Expr *E);
  
  bool typeCheckExpression(Expr *&E, Type ConvertType = Type());
  void typeCheckDecl(Decl *D);
  bool typeCheckPattern(Pattern *P);
  bool coerceToType(Pattern *P, Type Ty);
  bool typeCheckCondition(Expr *&E);

  /// coerceToType - Do semantic analysis of an expression in a context that
  /// expects a particular type.  This performs a conversion to that type if
  /// the types don't match and diagnoses cases where the conversion cannot be
  /// performed.
  ///
  /// This emits a diagnostic and returns null on error.
  Expr *coerceToType(Expr *E, Type Ty);
  
  /// isCoercibleToType - Determine whether the given expression can be 
  /// coerced to the given type.
  bool isCoercibleToType(Expr *E, Type Ty);
  
  Expr *buildDeclRefRValue(ValueDecl *D, SourceLoc loc);
  Expr *convertToRValue(Expr *E);
  Expr *convertLValueToRValue(LValueType *SrcLT, Expr *E);
  Expr *convertToMaterializable(Expr *E);

  Expr *foldSequence(SequenceExpr *E);

  /// \name Overload resolution
  ///
  /// Routines that perform overload resolution or provide diagnostics related
  /// to overload resolution.
  /// @{
  
  /// diagnoseEmptyOverloadSet - Diagnose a case where we disproved all of the
  /// possible candidates in an overload set of a call.
  void diagnoseEmptyOverloadSet(Expr *E, ArrayRef<ValueDecl *> Candidates);
  void printOverloadSetCandidates(ArrayRef<ValueDecl *> Candidates);
  
  /// filterOverloadSet - Filter a set of overload candidates based on the 
  /// the given argument type (for a call) or result type (if the context 
  /// provides such a type). 
  /// 
  /// This routine is used in both type-checking directions (root-to-leaf and
  /// leaf-to-root). The call argument allows leaf-to-root type checking
  /// to filter out candidates that can't be called as functions or have
  /// incompatible arguments. Conversely, when a destination type is provided,
  /// root-to-leaf type checking filters out candidates that cannot produce a
  /// result of an acceptable type. Both argument and destination type may be
  /// provided, which provides overload resolution based on both the call
  /// arguments and the expected result of the call.
  ///
  /// \param Candidates The set of overloaded candidates that should be 
  /// considered
  ///
  /// \param Arg The argument type, for a call to the overloaded functions.
  /// This argument is required.
  ///
  /// \param DestTy The type to which the result should be coerced, or null if
  /// not known.
  ///
  /// \param Viable Output vector to which all of the viable candidates will be
  /// added.
  ///
  /// \returns The best candidate, if there is one.
  ValueDecl *filterOverloadSet(ArrayRef<ValueDecl *> Candidates,
                               Expr *Arg,
                               Type DestTy,
                               SmallVectorImpl<ValueDecl *> &Viable);
  /// @}
};

  
  
} // end namespace swift

#endif

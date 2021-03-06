//===- DeclOpenMP.h - Classes for representing OpenMP directives -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines OpenMP nodes for declarative directives.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_DECLOPENMP_H
#define LLVM_CLANG_AST_DECLOPENMP_H

#include "clang/AST/DeclBase.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/TrailingObjects.h"

namespace clang {
class Expr;

/// \brief This represents '#pragma omp threadprivate ...' directive.
/// For example, in the following, both 'a' and 'A::b' are threadprivate:
///
/// \code
/// int a;
/// #pragma omp threadprivate(a)
/// struct A {
///   static int b;
/// #pragma omp threadprivate(b)
/// };
/// \endcode
///
class OMPThreadPrivateDecl final
    : public Decl,
      private llvm::TrailingObjects<OMPThreadPrivateDecl, Expr *> {
  friend class ASTDeclReader;
  friend TrailingObjects;

  unsigned NumVars;

  virtual void anchor();

  OMPThreadPrivateDecl(Kind DK, DeclContext *DC, SourceLocation L) :
    Decl(DK, DC, L), NumVars(0) { }

  ArrayRef<const Expr *> getVars() const {
    return llvm::makeArrayRef(getTrailingObjects<Expr *>(), NumVars);
  }

  MutableArrayRef<Expr *> getVars() {
    return MutableArrayRef<Expr *>(getTrailingObjects<Expr *>(), NumVars);
  }

  void setVars(ArrayRef<Expr *> VL);

public:
  static OMPThreadPrivateDecl *Create(ASTContext &C, DeclContext *DC,
                                      SourceLocation L,
                                      ArrayRef<Expr *> VL);
  static OMPThreadPrivateDecl *CreateDeserialized(ASTContext &C,
                                                  unsigned ID, unsigned N);

  typedef MutableArrayRef<Expr *>::iterator varlist_iterator;
  typedef ArrayRef<const Expr *>::iterator varlist_const_iterator;
  typedef llvm::iterator_range<varlist_iterator> varlist_range;
  typedef llvm::iterator_range<varlist_const_iterator> varlist_const_range;

  unsigned varlist_size() const { return NumVars; }
  bool varlist_empty() const { return NumVars == 0; }

  varlist_range varlists() {
    return varlist_range(varlist_begin(), varlist_end());
  }
  varlist_const_range varlists() const {
    return varlist_const_range(varlist_begin(), varlist_end());
  }
  varlist_iterator varlist_begin() { return getVars().begin(); }
  varlist_iterator varlist_end() { return getVars().end(); }
  varlist_const_iterator varlist_begin() const { return getVars().begin(); }
  varlist_const_iterator varlist_end() const { return getVars().end(); }

  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classofKind(Kind K) { return K == OMPThreadPrivate; }
};

/// Pseudo declaration for capturing of non-static data members in non-static
/// member functions.
///
/// Clang supports capturing of variables only, but OpenMP 4.5 allows to
/// privatize non-static members of current class in non-static member
/// functions. This pseudo-declaration allows properly handle this kind of
/// capture by wrapping captured expression into a variable-like declaration.
class OMPCapturedFieldDecl final : public VarDecl {
  friend class ASTDeclReader;
  void anchor() override;

  OMPCapturedFieldDecl(ASTContext &C, DeclContext *DC, IdentifierInfo *Id,
                       QualType Type)
      : VarDecl(OMPCapturedField, C, DC, SourceLocation(), SourceLocation(), Id,
                Type, nullptr, SC_None) {
    setImplicit();
  }

public:
  static OMPCapturedFieldDecl *Create(ASTContext &C, DeclContext *DC,
                                      IdentifierInfo *Id, QualType T);

  static OMPCapturedFieldDecl *CreateDeserialized(ASTContext &C, unsigned ID);

  // Implement isa/cast/dyncast/etc.
  static bool classof(const Decl *D) { return classofKind(D->getKind()); }
  static bool classofKind(Kind K) { return K == OMPCapturedField; }
};

} // end namespace clang

#endif

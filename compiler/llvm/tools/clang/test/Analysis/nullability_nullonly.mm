// RUN: %clang_cc1 -analyze -fobjc-arc -analyzer-checker=core,nullability.NullPassedToNonnull,nullability.NullReturnedFromNonnull -verify %s

#define nil 0
#define BOOL int

@protocol NSObject
+ (id)alloc;
- (id)init;
@end

@protocol NSCopying
@end

__attribute__((objc_root_class))
@interface
NSObject<NSObject>
@end

int getRandom();

typedef struct Dummy { int val; } Dummy;

void takesNullable(Dummy *_Nullable);
void takesNonnull(Dummy *_Nonnull);
Dummy *_Nullable returnsNullable();

void testBasicRules() {
  // The tracking of nullable values is turned off.
  Dummy *p = returnsNullable();
  takesNonnull(p); // no warning
  Dummy *q = 0;
  if (getRandom()) {
    takesNullable(q);
    takesNonnull(q); // expected-warning {{Null passed to a callee that requires a non-null 1st parameter}}
  }
}

Dummy *_Nonnull testNullReturn() {
  Dummy *p = 0;
  return p; // expected-warning {{Null is returned from a function that is expected to return a non-null value}}
}

void onlyReportFirstPreconditionViolationOnPath() {
  Dummy *p = 0;
  takesNonnull(p); // expected-warning {{Null passed to a callee that requires a non-null 1st parameter}}
  takesNonnull(p); // No warning.
  // Passing null to nonnull is a sink. Stop the analysis.
  int i = 0;
  i = 5 / i; // no warning
  (void)i;
}

Dummy *_Nonnull doNotWarnWhenPreconditionIsViolatedInTopFunc(
    Dummy *_Nonnull p) {
  if (!p) {
    Dummy *ret =
        0; // avoid compiler warning (which is not generated by the analyzer)
    if (getRandom())
      return ret; // no warning
    else
      return p; // no warning
  } else {
    return p;
  }
}

Dummy *_Nonnull doNotWarnWhenPreconditionIsViolated(Dummy *_Nonnull p) {
  if (!p) {
    Dummy *ret =
        0; // avoid compiler warning (which is not generated by the analyzer)
    if (getRandom())
      return ret; // no warning
    else
      return p; // no warning
  } else {
    return p;
  }
}

void testPreconditionViolationInInlinedFunction(Dummy *p) {
  doNotWarnWhenPreconditionIsViolated(p);
}

void inlinedNullable(Dummy *_Nullable p) {
  if (p) return;
}
void inlinedNonnull(Dummy *_Nonnull p) {
  if (p) return;
}
void inlinedUnspecified(Dummy *p) {
  if (p) return;
}

Dummy *_Nonnull testDefensiveInlineChecks(Dummy * p) {
  switch (getRandom()) {
  case 1: inlinedNullable(p); break;
  case 2: inlinedNonnull(p); break;
  case 3: inlinedUnspecified(p); break;
  }
  if (getRandom())
    takesNonnull(p);
  return p;
}

@interface TestObject : NSObject
@end

TestObject *_Nonnull getNonnullTestObject();

void testObjCARCImplicitZeroInitialization() {
  TestObject * _Nonnull implicitlyZeroInitialized; // no-warning
  implicitlyZeroInitialized = getNonnullTestObject();
}

void testObjCARCExplicitZeroInitialization() {
  TestObject * _Nonnull explicitlyZeroInitialized = nil; // expected-warning {{Null is assigned to a pointer which is expected to have non-null value}}
}

// Under ARC, returned expressions of ObjC objects types are are implicitly
// cast to _Nonnull when the functions return type is _Nonnull, so make
// sure this doesn't implicit cast doesn't suppress a legitimate warning.
TestObject * _Nonnull returnsNilObjCInstanceIndirectly() {
  TestObject *local = 0;
  return local; // expected-warning {{Null is returned from a function that is expected to return a non-null value}}
}

TestObject * _Nonnull returnsNilObjCInstanceIndirectlyWithSupressingCast() {
  TestObject *local = 0;
  return (TestObject * _Nonnull)local; // no-warning
}

TestObject * _Nonnull returnsNilObjCInstanceDirectly() {
  return nil; // expected-warning {{Null is returned from a function that is expected to return a non-null value}}
}

TestObject * _Nonnull returnsNilObjCInstanceDirectlyWithSuppressingCast() {
  return (TestObject * _Nonnull)nil; // no-warning
}

@interface SomeClass : NSObject
@end

@implementation SomeClass (MethodReturn)
- (SomeClass * _Nonnull)testReturnsNilInNonnull {
  SomeClass *local = nil;
  return local; // expected-warning {{Null is returned from a method that is expected to return a non-null value}}
}

- (SomeClass * _Nonnull)testReturnsCastSuppressedNilInNonnull {
  SomeClass *local = nil;
  return (SomeClass * _Nonnull)local; // no-warning
}

- (SomeClass * _Nonnull)testReturnsNilInNonnullWhenPreconditionViolated:(SomeClass * _Nonnull) p {
  SomeClass *local = nil;
  if (!p) // Pre-condition violated here.
    return local; // no-warning
  else
    return p; // no-warning
}
@end

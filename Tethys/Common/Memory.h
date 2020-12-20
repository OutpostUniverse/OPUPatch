
#pragma once

#include "Tethys/Common/Types.h"
#include <type_traits>

namespace Tethys {

constexpr uintptr  OP2Base      = 0x00400000;  ///< Preferred load address of Outpost2.exe.
constexpr uintptr  OP2ShellBase = 0x13000000;  ///< Preferred load address of OP2Shell.dll.
inline    HMODULE  GetOP2Handle()              ///< Returns HMODULE of Outpost2.exe.  Can be used with global variables.
  { static const auto h = [] { auto h = GetModuleHandleA(nullptr);  return h ? h : HMODULE(OP2Base); }();  return h; }
inline    HMODULE  g_hOP2 = GetOP2Handle();    ///< HMODULE of Outpost2.exe.  Lightweight, but UB if used with globals.


namespace TethysImpl {
// Helper template metafunctions for OP2Mem() implementation.
template <typename T>  constexpr bool PtrArg    = std::is_pointer_v<T> || std::is_array_v<T> || std::is_function_v<T>;
template <typename T>  using          FnToPfn   = std::conditional_t<std::is_function_v<T>, T*, T>;
template <typename T>  using          SelectPtr = std::enable_if_t<PtrArg<T>, FnToPfn<T>>;
}

// Functions to reference memory in Outpost2.exe.

/// Reference OP2 memory by pointer.
template <typename T = void*, bool Global = false, typename R = TethysImpl::SelectPtr<T>>
R OP2Mem(uintptr address) { return   R((uint8*)(Global ? GetOP2Handle() : g_hOP2) + (address - OP2Base));  }

/// Reference OP2 memory by reference.
template <typename T, bool Global = false, typename = std::enable_if_t<std::is_reference_v<T>>>
T OP2Mem(uintptr address) { return T(*((uint8*)(Global ? GetOP2Handle() : g_hOP2) + (address - OP2Base))); }

/// Reference OP2 memory, with built-in init-once for the pointer or reference.  Always safe to use for globals.
template <uintptr Address, typename T = void*>
T OP2Mem() { static const T p = OP2Mem<T, true>(Address);  return p; }

///@{ Call OP2 function, with built-in init-once for the function pointer.  Always safe to use for globals.
template <uintptr Address, typename Fn = void(), typename... Args>
auto OP2Thunk(Args&&... args) { return OP2Mem<Address, TethysImpl::FnToPfn<Fn>>()(std::forward<Args>(args)...); }

template <uintptr Address, auto Pfn, typename... Args>
auto OP2Thunk(Args&&... args) { return OP2Mem<Address, decltype(Pfn)>()(std::forward<Args>(args)...); }
///@}


///@{ OP2 cstdlib malloc(), calloc(), realloc(), and free() functions, using Outpost2.exe's memory allocation heap.
inline void* CDECL OP2Alloc(size_t  size)                 { return OP2Thunk<0x4C21F0, &OP2Alloc>(size);            }
inline void* CDECL OP2Calloc(size_t count,   size_t size) { return OP2Thunk<0x4C2CC0, &OP2Calloc>(count, size);    }
inline void* CDECL OP2Realloc(void* pMemory, size_t size) { return OP2Thunk<0x4C21F0, &OP2Realloc>(pMemory, size); }
inline void  CDECL OP2Free(void*    pMemory)              { return OP2Thunk<0x4C1380, &OP2Free>(pMemory);          }
///@}

/// Gets the OS handle to Outpost2.exe's memory allocation heap.
inline HANDLE GetOP2HeapHandle() { return OP2Mem<0x582F8C, HANDLE&>(); }

namespace TethysImpl { struct OP2HeapTag { constexpr explicit OP2HeapTag() { } }; }
/// Tag to select operator new/delete overloads using Outpost2.exe's memory allocation heap.
constexpr TethysImpl::OP2HeapTag OP2Heap{};

} // Tethys

/// Operator new overload using Outpost2.exe's memory allocation heap.
inline void* CDECL operator new(size_t  s, Tethys::TethysImpl::OP2HeapTag) noexcept
  { return Tethys::OP2Thunk<0x4C0F40, &Tethys::OP2Alloc>(s); }
/// Operator delete overload using Outpost2.exe's memory allocation heap.
inline void CDECL operator delete(void* p, Tethys::TethysImpl::OP2HeapTag) noexcept
  { return Tethys::OP2Thunk<0x4C0F30, &Tethys::OP2Free>(p);  }

namespace Tethys {

namespace TethysImpl {
///@{ @internal  Helper metafunctions to get this and function pointer types from pointers-to-member-functions.
template <typename T, typename = void>  struct PmfTraitsImpl{};
template <auto Pmf>                     using  PmfThisPtr   = typename PmfTraitsImpl<decltype(Pmf)>::This;
template <auto Pmf>                     using  PmfToPfnType = typename PmfTraitsImpl<decltype(Pmf)>::Pfn;
template <typename Fn, typename T>      using  ToMemPfnType = typename PmfTraitsImpl<Fn, T>::MemPfn;

template <typename R, typename T, typename X, typename... A>
struct PmfTraitsImpl<R(T::*)(A...),       X> { using This =       T*;  using Pfn = R(THISCALL*)(This, A...); };
template <typename R, typename T, typename X, typename... A>
struct PmfTraitsImpl<R(T::*)(A...) const, X> { using This = const T*;  using Pfn = R(THISCALL*)(This, A...); };
template <typename R, typename T, typename... A>
struct PmfTraitsImpl<R(A...), T>             { using MemPfn = R(THISCALL*)(T, A...);                         };
///@}
}

/// CRTP class that defines templated member function helpers to thunk to internal OP2 code with specialized helpers
/// for constructors, and allows exposing virtual function table internals (@see DEFINE_VTBL_TYPE, DEFINE_VTBL_GETTER).
template <typename Derived>
class OP2Class {
private:
  /// Base type all VtblType structs inherit from; this is the terminator case of the "recursive" interitance hierarchy.
  struct VtblType{};

protected:
  /// Typedef required for DEFINE_VTBL_TYPE() to work, and can also be used as a convenience shorthand for @ref Thunk.
  using $ = Derived;

  /// Thunks to an internal member function.  Example:  void Func(int a) { return Thunk<&$::Func>(0x4200AF, a); }
  template <auto Pmf, typename... Args>
  auto Thunk(uintptr address, Args&&... args) const {
    return OP2Mem<TethysImpl::PmfToPfnType<Pmf>>(address)(
      TethysImpl::PmfThisPtr<Pmf>(this), std::forward<Args>(args)...);
  }

  /// Thunks to an internal member function.  Example:  void Func(int a) { return Thunk<void(int)>(0x4200AF, a); }
  template <typename Fn = void(), typename... Args>
  auto Thunk(uintptr address, Args&&... args) const
    { return OP2Mem<TethysImpl::ToMemPfnType<Fn, decltype(this)>>(address)(this, std::forward<Args>(args)...); }

  /// Thunks to an internal member function.  Example:  void Func(int a) { return Thunk<0x4200AF, &$::Func>(a); }
  template <uintptr Address, auto Pmf, typename... Args>
  auto Thunk(Args&&... args) const {
    return OP2Thunk<Address, TethysImpl::PmfToPfnType<Pmf>>(
      TethysImpl::PmfThisPtr<Pmf>(this), std::forward<Args>(args)...);
  }

  /// Thunks to an internal member function.  Example:  void Func(int a) { return Thunk<0x4200AF, void(int)>(a); }
  template <uintptr Address, typename Fn = void(), typename... Args>
  auto Thunk(Args&&... args) const
    { return OP2Thunk<Address, TethysImpl::ToMemPfnType<Fn, decltype(this)>>(this, std::forward<Args>(args)...); }

  /// Thunks to an internal constructor.  This implicitly chains to all internal parent constructors.
  /// Example:  BaseClass()                  { InternalCtor<0x470000>(); }
  ///           BaseClass(InternalCtorChain) {                           }
  ///           SubClass()                  : BaseClass(UseInternalCtorChain) { InternalCtor<0x500000>(); }
  ///           SubClass(InternalCtorChain) : BaseClass(UseInternalCtorChain) {                           }
  template <uintptr Address, typename... Args>
  Derived* InternalCtor(Args... args)
    { return static_cast<Derived*>((Address != 0) ? Thunk<Address, void*(Args...)>(args...) : this); }

  /// Tag constant that can be used to define no-op chained constructors for @ref InternalCtor use.  Calling an internal
  /// constructor implementation calls the whole internal chain, so our shim parent constructors need to be no-ops.
  struct InternalCtorChain { explicit constexpr InternalCtorChain() { } }  static constexpr UseInternalCtorChain{};

  /// Dummy placeholder used to represent a virtual destructor in DEFINE_VTBL_TYPE().
  void* DestroyVirtual(ibool freeMem = false);

  /// @internal  A dummy overridden function declaration is used to get a base class's @ref VtblType rather than
  /// referring to VtblType directly, since C++ dominance rules are more robust for functions than types.
  static constexpr VtblType _GetBaseVtblType();
};
static_assert(std::is_empty_v<OP2Class<void>>, "OP2Class<> should be empty.");


/// Macro that allows a class's virtual functions to be accessed via a struct of function pointers, allowing them to be
/// hooked via Vtbl() (static) or Vfptr() (non-static).  Vfptr() also allows an object's vfptr to be reassigned.
///
/// @note  Do not specify virtual function overrides.  It is assumed the visibility where the macro is used is public.
///
/// Example:
/// ========
/// class MyClass : public OP2Class<MyClass> {
///   using $ = MyClass;  // Not needed for class directly inheriting from OP2Class, but is needed in subclasses.
/// public:
///   virtual       ~MyClass()           { ... }
///   virtual void* Func1(void*, size_t) { ... }
///   virtual bool  Func2() const        { ... }
///
/// #define MYCLASS_VTBL($)  $(DestroyVirtual)  $(Func1)  $(Func2)
///   DEFINE_VTBL_TYPE(MYCLASS_VTBL);  // Or DEFINE_VTBL_TYPE(macro, address) to also define a static vtbl getter.
/// };
///
/// Expands to:
/// ===========
/// class MyClass : public OP2Class<MyClass> {
/// public:
///   virtual       ~MyClass()           { ... }
///   virtual void* Func1(void*, size_t) { ... }
///   virtual bool  Func2() const        { ... }
///
///   struct VtblType : public (BaseClass::)VtblType {
///     void* (__thiscall*  pfnDestroyVirtual)(MyClass* pThis, ibool freeMem);
///     void* (__thiscall*  pfnFunc1)(MyClass* pThis, void*, size_t);
///     bool  (__thiscall*  pfnFunc2)(const MyClass* pThis);
///   };
///
///   VtblType*& Vfptr()       { return *reinterpret_cast<VtblType**>(this);      }
///   VtblType*  Vfptr() const { return *reinterpret_cast<VtblType*const*>(this); }
///
///   // If you used DEFINE_VTBL_TYPE(macro, address):
///   static VtblFuncs* Vtbl() { return OP2Mem<address, VtblType*>(); }
/// }; */
#define DEFINE_VTBL_TYPE(vtbl, ...)                                              \
  struct VtblType : public decltype(_GetBaseVtblType()) {                        \
    vtbl(VTBL_GENERATE_PFN_DEFS_IMPL)                                            \
  };                                                                             \
protected:                                                                       \
  static constexpr VtblType _GetBaseVtblType();                                  \
public:                                                                          \
  VtblType*& Vfptr()       { return *reinterpret_cast<VtblType**>(this);      }  \
  VtblType*  Vfptr() const { return *reinterpret_cast<VtblType*const*>(this); }  \
  DEFINE_VTBL_GETTER(__VA_ARGS__)
#define VTBL_GENERATE_PFN_DEFS_IMPL(method)  TethysImpl::PmfToPfnType<&$::method> pfn##method;

/// Defines a static member function getting the class's vtbl.
/// This can be used by itself if a base class has used DEFINE_VTBL_TYPE().
#define DEFINE_VTBL_GETTER(...)  template <size_t Address = size_t{__VA_ARGS__}>  static auto Vtbl()  \
  -> std::enable_if_t<Address != 0, VtblType*> { return OP2Mem<Address, VtblType*>(); }

} // Tethys

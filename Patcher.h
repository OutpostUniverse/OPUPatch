
#pragma once

#include <list>
#include <vector>
#include <unordered_map>

#include "PatcherUtil.h"

namespace Patcher {

/// @brief  RAII memory patch context class.  Allows for safe writes and insertion of code hooks into process memory.
///
/// The first time some memory is modified, the original data is tracked, and is automatically restored when the context
/// object instance is destroyed.
///
/// Methods that return a PatcherStatus also update an internally-tracked status.  If the internal status is an error,
/// all subsequent calls become a no-op and return the last error until it is reset by ResetStatus() or RevertAll().
///
/// @note  Calling methods with address provided as uintptr will cause address to be relocated.
///        Calling methods with address provided as a pointer type will not relocate.
class PatchContext {
public:
  template <typename T>  using Span = Impl::Span<T>;
  using TargetPtr   = Impl::TargetPtr;
  using FunctionPtr = Impl::FunctionPtr;
  using Offset      = Impl::Offset;
  using Register    = Registers::Register;

  /// Default constructor creates a patcher context for the process's base module.
  PatchContext() : PatchContext(static_cast<const char*>(nullptr), false) { }

  /// Constructor to create a patcher context for the given process module name, and (optionally) loads the module.
  explicit PatchContext(const char* pModuleName, bool loadModule   = false);

  /// Constructor to create a patcher context given a pointer within a module, and (optionally) adds a reference to it.
  explicit PatchContext(const void* hModule,     bool addReference = false);

  PatchContext(const PatchContext&)            = delete;
  PatchContext& operator=(const PatchContext&) = delete;

  ~PatchContext();  ///< Destructor.  Reverts all patches owned by this context and releases any owned module reference.

  /// Gets the status of this context.  This can be called once after multiple Write/Memcpy/Hook/etc. calls, rather than
  /// checking the returned status of each call individually.
  PatcherStatus GetStatus() const { return status_; }

  PatcherStatus ResetStatus();  ///< Resets the tracked status of this context for non-fatal errors (for user handling).

  void* GetModule() const { return hModule_; }  ///< Gets the OS module handle associated with this context.

  ///@{ Reverts all patches, releases any module reference held, and sets the module associated with this context.
  PatcherStatus SetModule() { return SetModule(static_cast<const char*>(nullptr), false); }
  PatcherStatus SetModule(const char* pModuleName, bool loadModule   = false);
  PatcherStatus SetModule(const void* hModule,     bool addReference = false);
  ///@}

  ///@{ Fixes up a raw address, adjusting it for module base relocation.
  template <typename T>         T* FixPtr(T*     pAddress) const
    { return (pAddress == nullptr) ? pAddress : static_cast<T*>(Util::PtrInc(pAddress, moduleRelocDelta_)); }
  template <typename T = void>  T* FixPtr(uintptr address) const { return FixPtr(reinterpret_cast<T*>(address)); }
  ///@}

  ///@{ Writes the given value to module memory.
  template <typename T>
  PatcherStatus Write(TargetPtr pAddress, const T& value) { return Memcpy<sizeof(T)>(pAddress, &value); }
  template <size_t Size, typename T>
  PatcherStatus Write(TargetPtr pAddress, const T& value) { return Memcpy<Size>(pAddress, &value); }
  ///@}

  /// Writes the given bytes to module memory.
  PatcherStatus WriteBytes(TargetPtr pAddress, Span<uint8> bytes)
    { return Memcpy(pAddress, bytes.Data(), bytes.Length()); }

  /// Writes no-ops to module memory up to size in bytes.  If @ref size is 0, then overwrite a whole single instruction.
  /// @note  32-bit x86 only.
  PatcherStatus WriteNop(TargetPtr pAddress, size_t size = 0);

  ///@{ Adds the specified module memory to the history tracker so it can be restored via Revert().
  PatcherStatus Touch(TargetPtr pAddress, size_t size);
  template <typename T>  PatcherStatus Touch(T* pAddress) { return Touch(pAddress, sizeof(T)); };
  ///@}

  ///@{ Hooks the beginning of a function in module memory, and optionally returns a pointer to a trampoline function
  ///   that can be used to call the original function.  New function's signature must match the original's.
  ///
  /// @param [in]  pAddress             Function or address of where to insert the hook.
  /// @param [in]  pfnNewFunction       The hook function or address to call instead.
  /// @param [out] pPfnTrampoline       (Optional) Pointer to where to store a callback pointer to the original code.
  /// @param [in]  pfnTrampolineOffset  (Optional) Offset or pointer-to-member into pfnNewFunction's functor object
  ///                                   state to pfnTrampoline.  Use SetCapturedTrampoline for the first lambda capture.
  ///
  /// Examples:  Hook(&Function,   &NewFunction)
  ///            Hook(0x439AB0,    0x439AF2,                      &global_pfnOriginal)
  ///            Hook(&Fn,         Util::SetCapturedTrampoline,   [F = decltype(&Fn)0](int a) -> int { return F(a*2); })
  ///            Hook(&StdcallFn,  &Functor::member_pfnOriginal,  Util::StdcallFunctor(Functor{}))
  ///         ** Hook(&Class::Fn,  &HookClass::Fn,                &HookClass::static_pfnOriginal)
  ///         ** Hook(&Class::Fn,  Util::ThiscallLambdaPtr([](Class* pThis, int a) { return pThis->b - a; }))
  ///
  /// ** Consider using MFN_PTR() instead of implicit class method pointer conversion, especially for virtual methods.
  ///
  /// @note  32-bit x86 only.
  PatcherStatus Hook(TargetPtr pAddress, const FunctionPtr& pfnNewFunction, void* pPfnTrampoline = nullptr);
  template <typename T>  PatcherStatus Hook(TargetPtr pAddress, const FunctionPtr& pfnNewFunction, T** pPfnTrampoline)
    { return Hook(pAddress, pfnNewFunction,    static_cast<void*>(pPfnTrampoline)); }
  template <typename T = void>  PatcherStatus Hook(TargetPtr pAddress, uintptr toAddress, T** pPfnTrampoline = nullptr)
    { return Hook(pAddress, FixPtr(toAddress), static_cast<void*>(pPfnTrampoline)); }
  PatcherStatus Hook(TargetPtr pAddress, Offset pfnTrampolineOffset, const FunctionPtr& pfnNewFunction) {
    void*const pState = pfnNewFunction.FunctorState();
    return Hook(pAddress, pfnNewFunction, (pState == nullptr) ? nullptr : Util::PtrInc(pState, pfnTrampolineOffset));
  }
  ///@}

  ///@{ Hooks a call instruction in module memory, replacing its original target function, and optionally returns a
  ///   pointer to the original function if possible, else nullptr.  New function's signature must match the original's.
  ///
  /// @param [in]  pAddress           Address of the call instruction to fix up.
  /// @param [in]  pfnNewFunction     The hook function or address to call instead.
  /// @param [out] pPfnOriginal       (Optional) Pointer to where to store a callback pointer to the original function.
  /// @param [in]  pfnOriginalOffset  (Optional) Offset or pointer-to-member into pfnNewFunction's functor object
  ///                                 state to pfnOriginal.  Use SetCapturedTrampoline for the first lambda capture.
  ///
  /// @see   Comments of @ref Hook for examples, which has similar usage.
  /// @note  32-bit x86 only.
  PatcherStatus HookCall(TargetPtr pAddress, const FunctionPtr& pfnNewFunction, void* pPfnOriginal = nullptr);
  template <typename T>  PatcherStatus HookCall(TargetPtr pAddress, const FunctionPtr& pfnNewFunction, T** pPfnOriginal)
    { return HookCall(pAddress, pfnNewFunction,    static_cast<void*>(pPfnOriginal)); }
  template <typename T = void> PatcherStatus HookCall(TargetPtr pAddress, uintptr toAddress, T** pPfnOriginal = nullptr)
    { return HookCall(pAddress, FixPtr(toAddress), static_cast<void*>(pPfnOriginal)); }
  PatcherStatus HookCall(TargetPtr pAddress, Offset pfnOriginalOffset, const FunctionPtr& pfnNewFunction) {
    void*const pState = pfnNewFunction.FunctorState();
    return HookCall(pAddress, pfnNewFunction, (pState == nullptr) ? nullptr : Util::PtrInc(pState, pfnOriginalOffset));
  }

  ///@{ Hooks an instruction (almost) anywhere in module memory.  Read and write access to the state of standard
  ///   registers is provided via function args, and control flow can be manipulated via the returned value.
  ///
  /// Examples:  LowLevelHook(0x402044,  [](Registers::Eax<int>& a, Esi<bool> b) { ++a;  return b ? 0 : 0x402107; })
  ///            LowLevelHook(0x5200AF,  { Registers::Register::Eax, Register::Edx },  [](int64& val) { val = -val; })
  ///
  /// Available registers: [Eax, Ecx, Edx, Ebx, Esi, Edi, Ebp, Esp, Eflags].  Arg types must fit within register size.
  /// To write to registers, declare args with >& or >*, e.g. Eax<int>&, Ecx<int>*, Ebp<char*>&, Edi<int*>*
  /// Hook must use cdecl, and return either void (with template deduction or @ref LowLevelHookOpt::NoCustomReturnAddr)
  /// or an address to jump to, where nullptr = original address (addresses within the overwritten area are allowed).
  ///
  /// @warning  This requires 5 bytes at pAddress; if the last 4 bytes overlap any jump targets, this could crash.
  /// @note     32-bit x86 only.
  PatcherStatus LowLevelHook(TargetPtr           pAddress,      ///< [in] Address of where to insert the hook.
                             Span<Register>      registers,     ///< [in] Registers to pass to the hook function.
                             uint32              byRefMask,     ///< [in] Bitmask of args to pass by reference.
                             const FunctionPtr&  pfnHookCb,     ///< [in] User hook callback (any callable).
                             uint32              options = 0);  ///< [in] Options.  See @ref LowLevelHookOpt.
  ///< Insert a low-level hook with a callback function that takes RegisterArgs or no args.
  template <typename T, typename Enable = decltype(FunctionPtr(std::declval<T>()))>
  PatcherStatus LowLevelHook(TargetPtr pAddress, T&& pfnHookCb, uint32 options = 0) {
    options |= LowLevelHookOpt::GetDefaults(Impl::FuncTraits<T>{});
    return LowLevelHook(pAddress, Impl::RegIds<T>::Ids, Impl::ByRefMask<T>::Mask, std::forward<T>(pfnHookCb), options);
  }
  ///< Insert a low-level hook with a callback function that takes a struct pointer or reference as a single parameter.
  template <typename T, typename Enable = decltype(FunctionPtr(std::declval<T>()))>
  PatcherStatus LowLevelHook(TargetPtr pAddress, Span<Register> registers, T&& pfnHookCb, uint32 options = 0) {
    options |= (LowLevelHookOpt::ArgsAsStructPtr | LowLevelHookOpt::GetDefaults(Impl::FuncTraits<T>{}));
    return LowLevelHook(pAddress, registers, 0, std::forward<T>(pfnHookCb), options);
  }
  ///@}

  ///@{ Replaces all static, direct pointer references to a global by scanning the module's .reloc section for any
  ///   references to it.
  ///
  /// @param [in]  pOldGlobal  Pointer to the old global we want to replace.
  /// @param [in]  size        Size in bytes of the global.
  /// @param [in]  pNewGlobal  Pointer to the new global we want to replace all references to pOldGlobal with.
  /// @param [out] pRefsOut    (Optional) Pointer to a vector to contain all locations that have been patched up.
  PatcherStatus ReplaceReferencesToGlobal(
    TargetPtr pOldGlobal, size_t size, const void* pNewGlobal, std::vector<void*>* pRefsOut = nullptr);
  template <typename T>
  PatcherStatus ReplaceReferencesToGlobal(
    TargetPtr pOldGlobal, const T* pNewGlobal, std::vector<void*>* pRefsOut = nullptr)
      { return ReplaceReferencesToGlobal(pOldGlobal, sizeof(T), pNewGlobal, pRefsOut); }
  ///@}

  /// Adds or modifies export table entries in the module.
  ///
  /// There are 3 modes of exporting:
  ///  - Export by name:     Exports by (possibly decorated) symbol name.
  ///  - Export by ordinal:  Exports by ordinal (index);  used by older exes or for anonymizing exports.
  ///  - Forwarded export:   Forwards an import (by name) from another module.  Often used by OS and shim libraries.
  ///                        Injecting extra forwarded exports is currently not supported.
  ///
  /// Injecting exports with the same name or ordinal as existing exports overwrites them.  Otherwise, they are added as
  /// new export entries.  If the export address is nullptr, the entry will be deleted instead.
  ///
  /// Examples:  EditExports({ { 0x401260, "AddUndecoratedExport" },  { 0x402000, "_AddDecoratedCFastcallExport@8"  } })
  ///            EditExports({ { 0x404000, 1 /* By ordinal */     },  { nullptr, "?DeleteDecoratedCppExport@@YAXXZ" } })
  PatcherStatus EditExports(Span<ExportInfo> exportInfos);
  
  PatcherStatus Memcpy(TargetPtr pAddress, const void* pSrc, size_t size);            ///< Safe memcpy to module memory.
  PatcherStatus Memset(TargetPtr pAddress, uint8      value, size_t count);           ///< Safe memset to module memory.
  template <size_t Size>  PatcherStatus Memcpy(TargetPtr pAddress, const void* pSrc); ///< Safe memcpy w/constexpr size.
  template <size_t Count> PatcherStatus Memset(TargetPtr pAddress, uint8      value); ///< Safe memset w/constexpr size.

  ///@{ Reassigns an object within module memory.
  template <typename T1, typename T2>  PatcherStatus Assign(T1*    pAddress, T2&& value);
  template <typename T>                PatcherStatus Assign(uintptr address,  T&& value)
    { return Assign(FixPtr<T*>(address), std::forward<T>(value)); }
  ///@}

  ///@{ In-place constructs an object within module memory.
  template <typename T, typename... Args>  PatcherStatus Construct(T*     pAddress, Args&&... args);
  template <typename T, typename... Args>  PatcherStatus Construct(uintptr address, Args&&... args)
    { return Construct<T>(FixPtr<T*>(address), std::forward<Args>(args)...); }
  ///@}

  PatcherStatus LockThreads();    ///< Freezes all other process threads to avoid races between writing and executing.
  PatcherStatus UnlockThreads();  ///< Unfreezes all other process threads after having used LockThreads().

  PatcherStatus Revert(TargetPtr pAddress);  ///< Reverts a patch that was previously written at the given address.
  PatcherStatus RevertExports();             ///< Reverts exports that had been injected by EditExports().
  PatcherStatus RevertAll();                 ///< Reverts all patches this context had applied, and resets the status.

  PatcherStatus ReleaseModule();  ///< If this PatchContext has loaded a module, releases its active handle to it.

  uint32 NumPatches() const { return history_.size(); }  ///< Returns the number of active patches.

  /// Returns true if the given address had previously been touched or patched.
  bool HasPatched(TargetPtr pAddress) const { return historyAt_.count(MaybeFixTargetPtr(pAddress)) != 0; }

private:
  /// Initializes associated module information.
  void InitModule();

  /// Returns the address referred to by the given TargetPtr, possibly relocated if it was requested.
  void* MaybeFixTargetPtr(const TargetPtr& pAddress) const
    { return pAddress.ShouldRelocate() ? FixPtr(pAddress) : static_cast<void*>(pAddress); }

  ///@{ Helper functions for barriering around memory writes.
  uint32        BeginDeProtect(void* pAddress, size_t size);
  void          EndDeProtect(void*   pAddress, size_t size, uint32 oldAttr);
  PatcherStatus AdvanceThreads(void* pAddress, size_t size);
  ///@}

  void*          hModule_;          ///< Handle to the module this PatchContext operates on.
  bool           hasModuleRef_;     ///< If set, this PatchContext has acquired a reference to @ref hModule_.
  intptr_t       moduleRelocDelta_; ///< Delta between the module's preferred base address and its loaded address.
  uint32         moduleHash_;       ///< Hash identifying the module based on its header attributes.
  PatcherStatus  status_;           ///< Status of this PatchContext.  If this is an error, most methods become a no-op.

  static constexpr size_t StorageSize = 8;

  /// @internal  Struct for storing mappings of patch addresses to infomation required for cleanup and other purposes.
  struct PatchInfo {
    void*                             pAddress;           ///< Actual patch address (may differ from requested address)
    Impl::ByteArray<StorageSize>      oldData;            ///< Old memory copy
    void*                             pTrackedAlloc;      ///< Tracked allocation, e.g. trampoline code (optional)
    size_t                            trackedAllocSize;   ///< Tracked allocation size (optional)
    void*                             pFunctorObj;        ///< Pointer to functor object (optional)
    FunctionPtr::FunctorDeleterFunc*  pfnFunctorDeleter;  ///< Pointer to functor object deleter function (optional)
  };

  /// Patch history, sorted from newest to oldest.
  std::list<PatchInfo>                                     history_;
  /// Mappings of user-requested patch addresses to PatchInfos.
  std::unordered_map<void*, decltype(history_)::iterator>  historyAt_;

  /// Threads locked by LockThreads() (pair of handle, program counter).  AdvanceThreads() may temporarily resume these.
  std::vector<std::pair<uint32, uintptr>>  frozenThreads_;
};

// =====================================================================================================================
template <size_t Size>
PatcherStatus PatchContext::Memcpy(
  TargetPtr    pAddress,
  const void*  pSrc)
{
  assert((pAddress != nullptr) && (pSrc != nullptr));
  pAddress             = MaybeFixTargetPtr(pAddress);
  const uint32 oldAttr = BeginDeProtect(pAddress, Size);

  if (status_ == PatcherStatus::Ok) {
    memcpy(pAddress, pSrc, Size);
    EndDeProtect(pAddress, Size, oldAttr);
  }

  return status_;
}

// =====================================================================================================================
template <size_t Count>
PatcherStatus PatchContext::Memset(
  TargetPtr  pAddress,
  uint8      value)
{
  assert(pAddress != nullptr);
  pAddress             = MaybeFixTargetPtr(pAddress);
  const uint32 oldAttr = BeginDeProtect(pAddress, Count);

  if (status_ == PatcherStatus::Ok) {
    memset(pAddress, value, Count);
    EndDeProtect(pAddress, Count, oldAttr);
  }

  return status_;
}

// =====================================================================================================================
template <typename T1, typename T2>
PatcherStatus PatchContext::Assign(
  T1*   pAddress,
  T2&&  value)
{
  assert(pAddress != nullptr);
  const uint32 oldAttr = BeginDeProtect(pAddress, sizeof(T1));

  if (status_ == PatcherStatus::Ok) {
    *pAddress = value;
    EndDeProtect(pAddress, sizeof(T1), oldAttr);
  }

  return status_;
}

// =====================================================================================================================
template <typename T, typename... Args>
PatcherStatus PatchContext::Construct(
  T*         pAddress,
  Args&&...  args)
{
  assert(pAddress != nullptr);
  const uint32 oldAttr = BeginDeProtect(pAddress, sizeof(T));

  if (status_ == PatcherStatus::Ok) {
    new(pAddress) T(std::forward<Args>(args)...);
    EndDeProtect(pAddress, sizeof(T), oldAttr);
  }

  return status_;
}

} // Patcher

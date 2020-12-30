
#pragma once

#include "Tethys/Game/ScBase.h"

namespace Tethys::TethysAPI {

/// Exported interface wrapping ScBase.  Represents mission objects such as AI unit groups, Triggers, etc.
class ScStub : public OP2Class<ScStub> {
public:
  static constexpr int NilIndex = ScStubList::NilIndex;

   constexpr ScStub()           : id_(NilIndex) { }
   explicit  ScStub(int stubID) : id_(stubID)   { }
  ~ScStub() { Thunk<0x478C50>(); }

  ///@{ Allow conversion from ScBase pointers/references.
  explicit ScStub(ScBase* pStub) : id_((pStub != nullptr) ? pStub->index_ : NilIndex) { }
  explicit ScStub(ScBase&  stub) : id_(stub.index_) { }
  ///@}

  bool IsValid()  const { return (id_ >= 0) && (id_ != NilIndex); } // (0x478CB0) ///< This ScStub is a valid reference?
  operator bool() const { return IsValid();                       }               ///< This ScStub is a valid reference?

  ///@{ Gets the internal ScBase implementation this object wraps.
  template <typename T = ScBase>       T* GetImpl()       { return IsValid() ? T::GetInstance(id_) : nullptr; }
  template <typename T = ScBase> const T* GetImpl() const { return IsValid() ? T::GetInstance(id_) : nullptr; }
  ///@}

  ///@{ [Trigger]
  ibool IsEnabled() const { return Thunk<0x478CA0, &$::IsEnabled>(); }  ///< Is this ScStub enabled and active?
  void  Enable()          { return Thunk<0x478C60, &$::Enable>();    }  ///< Enable this ScStub.
  void  Disable()         { return Thunk<0x478C70, &$::Disable>();   }  ///< Disable this ScStub.
  ///@}

  void Destroy() { return Thunk<0x478C80, &$::Destroy>(); }  ///< Destroy the underlying ScBase object.

  int  GetID() const     { return id_;   }  ///< Gets the ScBase ID this object wraps.
  void SetID(int stubID) { id_ = stubID; }  ///< Sets the ScBase ID this object wraps.

public:
  int id_;
};

} // Tethys::TethysAPI

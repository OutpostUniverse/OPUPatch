
#pragma once

#include "Tethys/Game/ScBase.h"

/// Exported interface wrapping ScBase.  Represents mission objects such as AI unit groups, Triggers, etc.
class ScStub : public OP2Class<ScStub> {
public:
  static constexpr int NilIndex = ScStubList::NilIndex;

   ScStub()                    : id_(NilIndex) { }
   explicit ScStub(int stubID) : id_(stubID)   { }
  ~ScStub() { Thunk<0x478C50>(); }

  ///@{ Allow conversion from ScBase pointers/references.
  explicit ScStub(ScBase* pStub) : id_((pStub != nullptr) ? pStub->index_ : NilIndex) { }
  explicit ScStub(ScBase&  stub) : id_(stub.index_) { }
  ///@}

  bool IsValid()  const { return (id_ >= 0) && (id_ != NilIndex); }
  operator bool() const { return IsValid();                       }

        ScBase* GetImpl()       { return IsValid() ? ScBase::GetInstance(id_) : nullptr; }
  const ScBase* GetImpl() const { return IsValid() ? ScBase::GetInstance(id_) : nullptr; }

  ibool IsInitialized() const { return Thunk<0x478CB0, &$::IsInitialized>(); }
  ibool IsEnabled()     const { return Thunk<0x478CA0, &$::IsEnabled>();     }

  void Enable()  { return Thunk<0x478C60, &$::Enable>();  }
  void Disable() { return Thunk<0x478C70, &$::Disable>(); }
  void Destroy() { return Thunk<0x478C80, &$::Destroy>(); }

  int  GetID() const     { return id_;   }
  void SetID(int stubID) { id_ = stubID; }

public:
  int id_;
};

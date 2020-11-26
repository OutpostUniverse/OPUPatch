
#pragma once

#include "Tethys/Common/Memory.h"

struct ButtonDisplayInfo;
class  MapObject;

// ** TODO Finish filling in
enum class CursorType : int {
  // System cursors
  Wait          =  0,  ///< Standard Windows Hourglass cursor
  Cross         =  1,  ///< Standard Windows Crosshair cursor
  Normal        =  2,  ///< Standard Windows Arrow cursor

  // Out2Res.dll or ".ani" file cursors
  Destruct      =  3,  ///< File "destruct.ani"
  //NoDestruct? =  4,  ///< Out2Res.dll: 0xCC
  Move          =  5,  ///< File "move.ani"
  NoMove        =  6,  ///< File "no_move.ani"
  Attack        =  7,  ///< File "attack.ani"
  NoAttack      =  8,  ///< File "noattack.ani"
  Bulldoze      =  9,  ///< File "bulldoze.ani"
  NoBulldoze    = 10,  ///< Out2Res.dll: 0xCC
  Build         = 11,  ///< File "build.ani"
  NoBuild       = 12,  ///< File "nobuild.ani"
  Dock          = 13,  ///< File "dock.ani"
  NoDock        = 14,  ///< File "nodock.ani"
  Repair        = 15,  ///< File "repair.ani"
  NoRepair      = 16,  ///< File "norepair.ani"
  Reprog        = 17,  ///< File "reprog.ani"
  NoReprog      = 18,  ///< File "no_repro.ani"
  Guard         = 19,  ///< File "guard.ani"
  //NoGuard?    = 20,  ///< Out2Res.dll: 0xCC
  //            = 21,  ///< Out2Res.dll: 0xB0
  //            = 22,  ///< Out2Res.dll: 0xCC
  Salvage       = 23,  ///< File "salvage.ani"
  //NoSalvage   = 24,  ///< Out2Res.dll: 0xCC
  Waypoint      = 25,  ///< File "waypoint.ani"
  NoWaypoint    = 26,  ///< Out2Res.dll: 0xCC
  //            = 27,  ///< Out2Res.dll: 0x71
  //            = 28,  ///< Out2Res.dll: 0x6F
  //            = 29,  ///< Out2Res.dll: 0x75
  //            = 30,  ///< Out2Res.dll: 0x6D
  //            = 31,  ///< Out2Res.dll: 0x73
  //            = 32,  ///< Out2Res.dll: 0x6E
  //            = 33,  ///< Out2Res.dll: 0x70
  //            = 34,  ///< Out2Res.dll: 0x74
  Count         = 35
};

/// UI Command abstract class.
class UICommand : public OP2Class<UICommand> {
public:
  virtual ibool IsEnabled(DWORD param) = 0;
  virtual int   Execute(DWORD param)   = 0;
  virtual void  GetButtonDisplayInfo(ButtonDisplayInfo* pButtonDisplayInfo, int a)
    { return Thunk<0x455C40, &$::GetButtonDisplayInfo>(pButtonDisplayInfo, a); }

#define OP2_UICOMMAND_VTBL($)  $(IsEnabled)  $(Execute)  $(GetButtonDisplayInfo)
  DEFINE_VTBL_TYPE(OP2_UICOMMAND_VTBL);
};

/// Mouse Command abstract class.
class MouseCommand : public UICommand {
  using $ = MouseCommand;
public:
  virtual void OnClick() { return Thunk<0x455C50, &$::OnClick>(); }

  virtual int GetMouseCursor(int pixelX, int pixelY, HCURSOR* phCursor)
    { return Thunk<0x455C60, &$::GetMouseCursor>(pixelX, pixelY, phCursor); }
  virtual int GetBuildCursor(
    int pixelX1, int pixelY1, int pixelX2, int pixelY2, HCURSOR* phCursor, int* pHighlight, MapObject* pMapObj)
      { return Thunk<0x455C70, &$::GetBuildCursor>(pixelX1, pixelY1, pixelX2, pixelY2, phCursor, pHighlight, pMapObj); }

  virtual void  OnCancel()        { return Thunk<0x4512B0, &$::OnCancel>();        }
  virtual void  OnChangeCommand() { return Thunk<0x4512D0, &$::OnChangeCommand>(); }

  virtual ibool IsDefaultCommand(int pixelX, int pixelY)
    { return Thunk<0x455C80, &$::IsDefaultCommand>(pixelX, pixelY); }

  virtual HCURSOR GetCursor(int pixelX, int pixelY) { return Thunk<0x455C90, &$::GetCursor>(pixelX, pixelY); }

#define OP2_MOUSECOMMAND_VTBL($)  \
  $(OnClick)  $(GetMouseCursor)  $(GetBuildCursor)  $(OnCancel)  $(OnChangeCommand) $(IsDefaultCommand)  $(GetCursor)
  DEFINE_VTBL_TYPE(OP2_MOUSECOMMAND_VTBL);
};

namespace UICmd {
// UI Commands (non-targeted)

/// Factory::DoProduce UI command
class CommandProduce : public UICommand {
  using $ = CommandProduce;
public:
  ibool IsEnabled(DWORD param) override { return Thunk<0x4568D0, &$::IsEnabled>(param); }
  int   Execute(DWORD param)   override { return Thunk<0x456C70, &$::Execute>(param);   }
  void  GetButtonDisplayInfo(ButtonDisplayInfo* pButtonDisplayInfo, int a) override
    { return Thunk<0x455C40, &$::GetButtonDisplayInfo>(pButtonDisplayInfo, a); }

  DEFINE_VTBL_GETTER(0x4D5000);

  static CommandProduce* GetInstance() { return OP2Mem<0x565680, CommandProduce*>(); }

public:
  // ** TODO member variables?
};

// Mouse Commands (targeted)

/// Unit::Attack mouse command
class CommandAttack : public MouseCommand {
  using $ = CommandAttack;
public:
  ibool IsEnabled(DWORD param)              override { return Thunk<0x4530B0, &$::IsEnabled>(param);          }
  int   Execute(DWORD param)                override { return Thunk<0x4531F0, &$::Execute>(param);            }
  void  GetButtonDisplayInfo(ButtonDisplayInfo* pButtonDisplayInfo, int a) override
    { return Thunk<0x453160, &$::GetButtonDisplayInfo>(pButtonDisplayInfo, a); }
  void  OnClick()                           override { return Thunk<0x453250, &$::OnClick>();                 }
  int   GetMouseCursor(int pixelX, int pixelY, HCURSOR* phCursor) override
    { return Thunk<0x453480, &$::GetMouseCursor>(pixelX, pixelY, phCursor); }
  ibool IsDefaultCommand(int pixelX, int pixelY) override
    { return Thunk<0x453510, &$::IsDefaultCommand>(pixelX, pixelY); }
  HCURSOR GetCursor(int pixelX, int pixelY) override { return Thunk<0x4533B0, &$::GetCursor>(pixelX, pixelY); }

  virtual int F2(int pixelX, int pixelY, int* pA) { return Thunk<0x4533D0, &$::F2>(pixelX, pixelY, pA); }

#define OP2_COMMANDATTACK_VTBL($)  $(F2)
  DEFINE_VTBL_TYPE(OP2_COMMANDATTACK_VTBL, 0x4D4C98);

  static CommandAttack* GetInstance() { return OP2Mem<0x56560C, CommandAttack*>(); }

public:
  // ** TODO member variables?
};

/// Unit::Build mouse command
class CommandBuild : public MouseCommand {
public:
  int  IsEnabled(DWORD param) override { return Thunk<0x451B40, &$::IsEnabled>(param); }
  int  Execute(DWORD param)   override { return Thunk<0x451BE0, &$::Execute>(param);   }
  void GetButtonDisplayInfo(ButtonDisplayInfo* pButtonDisplayInfo, int a) override
    { return Thunk<0x451BA0, &$::GetButtonDisplayInfo>(pButtonDisplayInfo, a); }
  void OnClick()              override { return Thunk<0x451C50, &$::OnClick>();        }
  int  GetMouseCursor(int pixelX, int pixelY, HCURSOR* mouseCursor) override
    { return Thunk<0x451D50, &$::GetMouseCursor>(pixelX, pixelY, mouseCursor); }
  int  GetBuildCursor(
    int pixelX1, int pixelY1, int pixelX2, int pixelY2, HCURSOR* phCursor, int* pHighlight, MapObject* pMapObj) override
      { return Thunk<0x451DC0, &$::GetBuildCursor>(pixelX1, pixelY1, pixelX2, pixelY2, phCursor, pHighlight, pMapObj); }

  DEFINE_VTBL_GETTER(0x4D4B50);

public:
  // ** TODO member variables?
};

} // UICmd

// ** TODO
// extern HCURSOR mouseCursorTable[size_t(CursorType::Count)];        // 0x56EA08
// extern char* mouseCursorResourceTable[size_t(CursorType::Count)];  // 0x4E9878 union{ WORD resID, LPCTSTR pFilename }

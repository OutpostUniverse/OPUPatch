
#pragma once

namespace Tethys {

/**
 * TApp::Run loops forever doing:
 *   peek for messages, call GameFrame::OnIdle() if none waiting
 *   GetMessage(), check for errors and exit if so
 *   if GameFrame::TranslateAccelerator() returns false:
 *     if IDlgWnd::PretranslateModeless() returns true:
 *       continue
 *     TranslateMessage()
 *     DispatchMessage()
 *
 * GameFrame::TranslateAccelerator() calls TranslateAccelerator with
 * hPauseAccel (a table containing only commands relating to unpausing) if the
 * game is paused, otherwise attempts to translate using hViewAccel, and
 * additionally hUnitAccel if the user is not typing chat into the status bar.
 *
 * GameFrame::WndProc() handles WM_COMMAND messages generated from the
 * accelerator translation.
 * Messages 40091..40106 (creating and recalling location bookmarks) are handled
 * directly, calling DetailPane methods accordingly.
 * TODO: what about CallFilters() at the top?
 *
 * if none of these, then control from the WM_COMMAND handler passes to
 * TFrame::WndProc().
 * If IsWindowEnabled(this.hwnd) returns true, call
 * GameFrame::OnCommand(). If this returns false, call
 * TApp::HandleCommand().
 *
 * GameFrame::OnCommand():
 * TODO: something to do with calling a virtual method of *pActiveMouseFilter if
 * not null.
 * TODO: something to do with GetUiState() with a temporary UIState
 * TODO: something to do with the UIState's control ID
 * Messages are then handled:
 *   40001: <Ctrl+P>, <Pause>: (un)pause game
 *   40057: local player quit (TODO how is this sent?)
 *   40077: toggle music (TODO)
 *   40078: toggle sound effects (TODO)
 *   40082: move minimap buttons (TODO)
 *   40090: TODO (something to do with lava?)
 *   40109: Set frameskip to 1
 *   40110: Set frameskip to 2
 *   40111: Set frameskip to 4
 *   40113..40118: ally player (1..N)
 *   40122: toggle unit paths
 *   40124: TODO
 *   40125: TODO
 *   40126: toggle showing all units on minimap
 *
 *   40128: set blight spread speed to 100
 *   40133: toggle showing ambient animations
 *   40171: toggle force enabling RCC effect
 *   40172: toggle force disabling RCC effect
 *   40174: toggle computer overlay
 *   40175: toggle showing shadows
 *   40177: TODO
 *   40178: pause if in singleplayer, and call TApp::GetHelp()
 *   40183: set zoom to 1
 *   40184: set zoom to 2
 *   40185: set zoom to 3
 *   40199: TODO center detail pane on (somewhere? TODO) if TODO
 *   None of the following (40200.. TODO) are done if it is a unit mission
 *   40200: show resources report and set unknown to 1
 *   40201: show resources report and set unknown to 2
 *   40202: show resources report and set unknown to 3
 *   40203: show resources report and set unknown to 4
 *   40204: show resources report and set unknown to 5
 *   40205: check for player having tech (TODO, something inside a string?) and
 *   if so, show resources report and set unknown to 6
 *   40206: show game settings report
 *   40207: pause, TApp::DoNetGame(), unpause
 *   40218: set send chat messages to allies only
 *   40219: set send chat messages to all players
 *   40224: open vehicles report if num RCCs > 0
 * return 1
 *
 * TApp::HandleCommand():
 *   TODO: something to do with SetUIState
 * Messages are then handled:
 *   40017: <Ctrl+S> save the game
 *   40011: Open about dialog (not the regular one)
 *   40042: activate shell
 *   40018: <Ctrl+L> load game
 *   40057: exit the game
 *   40044: activate game
 *   40142: <Shift+Tab> select previous structure/vehicle
 *   40119: load script
 *   40157: <Ctrl+9> bookmark selected unit group
 *   40158..40167: <0..9> recall bookmarked unit group
 *   40179: <Ctrl+Backspace> select previous unit without centering
 *   40168: <Backspace> select previous unit
 *   40169: <Shift+Backspace> next selected unit
 *   40214: <+> zoom in detail pane
 *   40180: <Ctrl+Shift+Backspace> next selected unit without centering
 *   40181: <Ctrl+Tab> next unit without centering
 *   40182: <Ctrl+Shift+Tab> previous unit without centering
 *   40225..TODO: <shift+0..9> select unit group without centering
 *   40215: <-> zoom out detail pane
 */


enum class AcceleratorTableID : int {
  View  = 130,
  Unit  = 206,
  Pause = 212,
};

enum class WMCommandID : int {
  TogglePause = 40001,  ///< (Un)pause the game <Ctrl+P or Pause>

  GotoBookmarkedLocation0 = 40091,  ///< Center the detail pane on bookmarked location 0 <F5>
  GotoBookmarkedLocation1 = 40092,  ///< Center the detail pane on bookmarked location 1 <F6>
  GotoBookmarkedLocation2 = 40093,  ///< Center the detail pane on bookmarked location 2 <F7>
  GotoBookmarkedLocation3 = 40094,  ///< Center the detail pane on bookmarked location 3 <F8>
  GotoBookmarkedLocation4 = 40095,  ///< Center the detail pane on bookmarked location 4 <F9>
  GotoBookmarkedLocation5 = 40096,  ///< Center the detail pane on bookmarked location 5 <F10>
  GotoBookmarkedLocation6 = 40097,  ///< Center the detail pane on bookmarked location 6 <F11>
  GotoBookmarkedLocation7 = 40098,  ///< Center the detail pane on bookmarked location 7 <F12>
  BookmarkLocation0       = 40102,  ///< Bookmark location 0 <Ctrl+F5>
  BookmarkLocation1       = 40103,  ///< Bookmark location 1 <Ctrl+F6>
  BookmarkLocation2       = 40104,  ///< Bookmark location 2 <Ctrl+F7>
  BookmarkLocation3       = 40105,  ///< Bookmark location 3 <Ctrl+F8>
  BookmarkLocation4       = 40106,  ///< Bookmark location 4 <Ctrl+F9>
  BookmarkLocation5       = 40099,  ///< Bookmark location 5 <Ctrl+F10>
  BookmarkLocation6       = 40100,  ///< Bookmark location 6 <Ctrl+F11>
  BookmarkLocation7       = 40101,  ///< Bookmark location 7 <Ctrl+F12>

  // -------------------- Various "hidden" functionality that is not necessarily exposed in the UI ---------------------
  MoveMinimapButtons          = 40082,  ///< Reposition minimap buttons (i.e. right click on minimap)
  SetFrameskip1               = 40109,  ///< Set frameskip ratio to 1 (no frameskip)
  SetFrameskip2               = 40110,  ///< Set frameskip ratio to 2 (skip rendering every other frame)
  SetFrameskip4               = 40111,  ///< Set frameskip ratio to 4 (skip rendering 3/4 frames)
  AllyPlayer1                 = 40113,  ///< Ally with player 1
  AllyPlayer2                 = 40114,  ///< Ally with player 2
  AllyPlayer3                 = 40115,  ///< Ally with player 3
  AllyPlayer4                 = 40116,  ///< Ally with player 4
  AllyPlayer5                 = 40117,  ///< Ally with player 5
  AllyPlayer6                 = 40118,  ///< Ally with player 6
  LocalPlayerQuit             = 40057,  ///< Sent in response to the local player quitting
  ToggleMusic                 = 40077,  ///< Sent to toggle music
  ToggleSoundEffects          = 40078,  ///< Sent to toggle sound effects
  DebugSetEruption            = 40090,  ///< ctGameOpt SetEruption debug command
  ToggleShowUnitPaths         = 40122,  ///< Toggle unit paths
  ToggleUnknown1              = 40124,
  ToggleUnknown2              = 40125,
  ToggleShowAllUnitsOnMinimap = 40126,  ///< Toggle showing all units on minimap
};

} // Tethys

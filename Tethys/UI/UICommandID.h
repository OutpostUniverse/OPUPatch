
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


enum AcceleratorTableId : int {
  VIEW_ACCEL  = 130,
  UNIT_ACCEL  = 206,
  PAUSE_ACCEL = 212,
};

enum WmCommandId : int {
  // <Ctrl+P>, <Pause> (Un)pause the game
  TOGGLE_PAUSE = 40001,

  // Various "hidden" functionality that is not necessarily exposed in the UI
  // Appears to be sent in response to the local player quitting
  LOCAL_PLAYER_QUIT = 40057,
  // Appears to be sent to toggle sound effects / music
  TOGGLE_MUSIC         = 40077,
  TOGGLE_SOUND_EFFECTS = 40078,
  // Reposition minimap buttons (i.e. right click on minimap)
  MOVE_MINIMAP_BUTTONS = 40082,
  // TODO: something unknown to do with lava, and sets tickOfLastctGameOpt to
  // the current tick
  LAVA_UNKNOWN = 40090,
  // Set frameskip to 1/2/4
  SET_FRAMESKIP_1 = 40109,
  SET_FRAMESKIP_2 = 40110,
  SET_FRAMESKIP_4 = 40111,
  // Ally with player 1..6 
  ALLY_PLAYER_1 = 40113,
  ALLY_PLAYER_2 = 40114,
  ALLY_PLAYER_3 = 40115,
  ALLY_PLAYER_4 = 40116,
  ALLY_PLAYER_5 = 40117,
  ALLY_PLAYER_6 = 40118,
  // Toggle unit paths
  TOGGLE_SHOW_UNIT_PATHS = 40122,
  // TODO
  TOGGLE_UNKNOWN1 = 40124,
  TOGGLE_UNKNOWN2 = 40125,
  // Toggle showing all units on minimap
  TOGGLE_SHOW_ALL_UNITS_ON_MINIMAP = 40126,


  // <F5>..<F12> Center the detail pane on bookmarked location N
  GOTO_BOOKMARKED_LOCATION_0 = 40091,
  GOTO_BOOKMARKED_LOCATION_1 = 40092,
  GOTO_BOOKMARKED_LOCATION_2 = 40093,
  GOTO_BOOKMARKED_LOCATION_3 = 40094,
  GOTO_BOOKMARKED_LOCATION_4 = 40095,
  GOTO_BOOKMARKED_LOCATION_5 = 40096,
  GOTO_BOOKMARKED_LOCATION_6 = 40097,
  GOTO_BOOKMARKED_LOCATION_7 = 40098,
  // <Ctrl+F5>..<Ctrl+F12> Bookmark location N
  BOOKMARK_LOCATION_5 = 40099,
  BOOKMARK_LOCATION_6 = 40100,
  BOOKMARK_LOCATION_7 = 40101,
  BOOKMARK_LOCATION_0 = 40102,
  BOOKMARK_LOCATION_1 = 40103,
  BOOKMARK_LOCATION_2 = 40104,
  BOOKMARK_LOCATION_3 = 40105,
  BOOKMARK_LOCATION_4 = 40106,
};

} // Tethys

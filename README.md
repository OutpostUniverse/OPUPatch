# OPUPatch

OPUPatch is a collection of patches to Outpost 2 for the Outpost Universe community update. It is implemented as an op2ext mod DLL, and patches the game's code at runtime.

Whereas op2ext adds mod loader functionality to Outpost 2, OPUPatch implements bugfixes and new features for the game, and increments the game version. It will not, however, contain direct game balance changes; those would (eventually) be put within their own mod.

# Requirements

* Outpost 2 v1.2.7 or newer
* op2ext v3.1.0 or newer
* TethysAPI
* Patcher v4
* Capstone
* C++17

# Change log

## 1.4.2

### General changes

* Saved games from older versions of Outpost 2 can now be loaded.
* Add an exclusive fullscreen mode.  This may be useful to people with high DPI displays whose game window appears too small.
  * Fullscreen mode can be configured by the `GAME_WINDOW.FULLSCREEN`, `GAME_WINDOW.H_SIZE`, and `GAME_WINDOW.V_SIZE` INI settings.

### Bug fixes

* Fix an issue where the Enhanced Foam Evaporation tech increased Stickyfoam duration when your own units are hit by it, instead of when your Stickyfoam hits other units.
* (GOG) Fix crash bugs caused by the IPX emulator `winsock32.dll` wrapper.
* Fix an issue where the detail pane wouldn't display more than a 64x64 tile area at once when playing on world maps (512+ tiles wide) at 1440p and greater resolutions, or when loading a saved game from the main menu immediately after launching the game.
* Fix a minor bug with the double unit limit patch where the last unit's ID was set to 1024 instead of 2048.
* Fix the campaign mission menu dialog's width being too short and cutting off the title text.
* Fix Vortex and Electrical Storm spawns crashing the game when the unit limit is exceeded.
* Fix an issue where researching more than 2 techs that upgrade the same unit would corrupt random unit stats for the following player.

### Developer-related changes

* Changes to facilitate future Python support.
* Add super speed testing mode, configured by the `Game.SuperSpeed` INI setting.
* Add fields to `OnTriggerArgs`: `triggeredBy` and `prevTriggeredBy`.
* Add some new mission API callbacks:
  * `void OnSaveGame(OnSaveGameArgs*)` - Called when the game is saved; passed a file write stream positioned at the end of normal data. Return 1 = success, 0 = failure.
  * `void OnLoadSavedGame(OnLoadSavedGameArgs*)` - Called when a saved game is loaded; passed a file read stream positioned at the end of normal data. Return 1 = success, 0 = failure.
  * `void OnDamageUnit(OnDamageUnitArgs*)` - Called when a unit is damaged by a weapon or disaster (or Tokamak self-damage).
  * `void OnTransferUnit(OnTransferUnitArgs*)` - Called when a unit is transferred from one player to another.
  * `void OnGameCommand(OnGameCommandArgs*)` - Called whenever a command packet is processed (excluding `CommandType::Nop` and `CommandType::InvalidCommand`).
* Rename some of the 1.4.0 mission API callbacks:
  * `OnLoad`   => `OnLoadMission`
  * `OnUnload` => `OnUnloadMission`
  * `OnEnd`    => `OnEndMission`
  * The old names will continue to be backwards-compatible.
* Fix the `OnDestroyUnit` mission API callback to be triggered when units self-destruct, when weapons fire expires, and when units are poofed.

## 1.4.1

### Bug fixes

* Fix an issue where mission objectives of having empty Cargo Trucks could not be completed.
* Mission DLLs can now specify trigger functions of `nullptr` instead of having to define a dummy function.

## 1.4.0

Changes specific to the CD or GOG releases are annotated with `(CD)` or `(GOG)` accordingly.  In all other cases, the CD and GOG versions of the game are treated as equivalent.

### Gameplay-related changes and bug fixes

* Various spammy or broken Savant notifications are silenced/modified:
  * Don't trigger "alliance formed/broken" alerts at the start of the game (e.g. in co-op maps where all players are allied).
  * Don't trigger "transfer received/complete" alerts if the transfer is to/from AI or Gaia players.
  * Only trigger "power levels optimal" and "metals storage needed" alerts once, rather than repeating them frequently.
  * Fix a logic bug that incorrectly caused "food stores are plentiful" to trigger only when population is zero (the intended behavior is to trigger when there is enough food for 15 food calculation cycles (~3.8 marks) but storage is decreasing).
  * Avoid repeating "food production in surplus" alerts (only alert once when food production rate becomes positive).
  * Avoid repeating "morale is ..." alerts when there are no changes, or if morale is forced to a specific value.
* Light Towers now play "enemy unit sighted!" alerts similar to Scouts when an enemy unit is nearby. 
* Double the maximum number of units on the map at once to 2048.
* Surveyed mining beacons now animate differently based on their "variant" (lesser modifier to yield on top of the 1-3 bar modifier): low variant has no animation, medium variant (best peak yield) has a slower animation, and high variant (best minimum yield) is the same as original.
* Fix a crash when transferring units to the Gaia player.
* Fix various issues with EMP missiles:
  * Fix an issue that causes missiles to not land for Eden players.
  * Fix an issue that caused nearby Meteor Defenses to ignore missiles that were targeted directly on top of a Meteor Defense.
  * Prevent Meteor Defenses from shooting down allies' missiles.
* Fix some issues relating to building walls:
  * Invisible "perma-walls" are no longer left behind when an Earthworker dies during wall construction.
  * Walls no longer fail to build if built in a 3x3 area around another wall being built, Blight, or lava.
  * Refund metals and cancel the wall-building operation if another unit is on the wall tile at the same time the wall is supposed to finish.
* Fix a bug that caused wreckage to be turned into rubble if the wreckage was placed on certain terrain types.
* Structure docks no longer damage allies' vehicles.
* When a player deploys an EDWARD Satellite, all mining beacons will be automatically surveyed by their allies (as if they had deployed their own EDWARD Satellite).
* Cargo Trucks can now be partially loaded with food or metals if your storage is less than the truck's capacity (1000 by default).
  * Mission objectives like "have 5000 Common Metals in Cargo Trucks" are based on the total cargo amount.  If you had 5 Cargo Trucks with metals but one truck only contained 900 units of metals, then you would need to load a 6th truck with at least 100 metals to fulfill the objective.

### Interface-related changes

* Rearrange some dialogs, such as the multiplayer lobby, for better experience (such as increasing the size of some UI elements).
* Replace the program icon with a redesigned, modern one.
* Replace the game main menu background image, fonts, dialogs, and text color schemes.
* Don't force-uppercase the contents of the status bar for better readability.
* Fix a bug that causes radio buttons to not appear selected unless `Alt` is pressed (or accelerator-key underlines are set to always display in Windows).
* Default focus to "looking at the IP address" in the "Find Session" dialog.
* Fix a small bug with minimap clipping.
* Display vehicle cargo and amount in mouseover tooltips and destroyed notifications in the message log.
* Shorten stock multiplayer mission names to improve readability (e.g. '6P, LoS, 'Pie Chart').
* Invert the behavior of holding `Shift` when using control-group hotkeys so that holding `Shift` centers the player's view.
* Allow repair commands to be given to multiple selected ConVecs or Repair Vehicles.
* Allow setting ore routes on Cargo Trucks that have ore in them.
* Allow localization to be applied to the game.
  * Built-in localized string tables for `Outpost2.exe` and `OP2Shell.dll` are configurable via `language.ini` located under each language mod subfolder under `OPU\lang`.
  * Other aspects of localization are handled by the installer copying UI, cutscene, voice clip, techtrees, story, etc. files from a localized Outpost 2 CD version to the appropriate language mod subfolder.
  * Known issues: mission names, mission objectives and some other in-game messages, the "About Outpost 2" dialog, and multiplayer dialogs are not localized.

### Graphics-related changes

* Force the use of windowed (pure GDI) mode, allowing the game to run at any resolution, rather than forcing a resolution change to 640x480 (CD) or 1024x768 (GOG) via DirectDraw.
* Fix a crash that could occur if the game window was made too big (for example, if the game was maximized on a 4K screen).
* Fix a potential crash/drawing glitch if the game window is larger than the current map, centering the detail pane within the available space.
* Fix various issues related to desktop compositing on Vista and up that caused graphical stuttering and the Aero window frame to be drawn incorrectly, on top of the existing game window frame.
  * Graphical stuttering fix can be manually toggled via the `Game.ForceFullRedraw` INI setting.  It is enabled by default on Win7 if DWM is enabled, in all circumstances on Win8-10+, or if the game is running in a virtual machine.
* On Win10, disable DPI awareness as this caused the entire UI to appear tiny.
  * GDI scaling is still enabled so text in the menus should appear crisp.  Unfortunately this doesn't work for most in-game text due to how the game renders text internally.
  * DPI mode can be configured via the `Game.DPIAwareness` INI setting.  The default setting is `-5` (`DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED`).  See [Microsoft's docs](https://docs.microsoft.com/en-us/windows/win32/hidpi/dpi-awareness-context) for all possible settings, or use a setting of `0` to use built-in behavior.
* Draw the Acid Cloud translucency effect using 50% alpha blending instead of a checkerboard pattern.
* Fix a crash that could occur in some circumstances when drawing Thor's Hammer lightning.

### Audio-related changes

* Allow music to continue playing when the game window doesn't have focus.

### Network and multiplayer-related changes

* Fix "replicating players list failed" error that occurs with TCP/IP mode when behind a NAT, as is the case with most home networks.
* Bind to all network adapters instead of the highest priority one, which improves behavior with virtual network adapters such as Hamachi and other VPNs and doesn't require manually configuring network adapter priority in Windows.
* Disabled IPX, modem, and serial netplay protocols as they are no longer practical on modern systems (DirectPlay is no longer supported).  Opening the "Multiplayer" menu no longer stalls for several seconds.
* Don't halve the game speed in multiplayer, allowing multiplayer games to run as fast as single player.
* Fix game start checksum validation to checksum the techtree file actually used by the map, instead of always just the built-in techtrees.
  * Techtree checksum logic now factors out localization and other non-function-related contents.
  * Other OPU game files such as `op2ext.dll` and `OPUPatch.dll` now also get checksummed.
* Disable all "Dan's" cheats in multiplayer.  They are allowed in single player.
* Increase maximum chat message length.
* Allow players to ping locations on the map by sending chat messages of the form "`@X,Y`" (can be followed by other text).  Receipients can double-click on the message log entry to jump to the pinged location.

### Mapmaking/modding and developer-related changes

* Sound files can now be loaded from loose files instead of requiring them to be in VOL files.
* Game files can now (preferably) be loaded from a `OPU` subfolder under the game folder, and from asset-specific subfolders under `OPU`.
* Allow DLLs to be placed in this folder hierarchy or within a `libs` subfolder, to avoid cluttering up the main directory.
* Mods and maps now live in their own subfolders, and the search path logic has received an overhaul to prefer these first over looking at other folders:
  * For example, all of the mod-installed content that overrides the base game files lives under `OPU\base`, and custom maps (under `OPU\maps`) are free to replace files as well.
  * This also sets up for support for mods having their own folders under the `OPU` dir in order to prefer these (such as localization mods or other large/game-breaking mods) over the base game files.
  * Maps now live in named folders for each map (e.g. `OPU\base\maps\PieChart`), which have precedence over other folders when that map is being loaded, allowing custom maps to override base game files or include additional content.
  * The `OPU` directory is searched next, followed by the main game directory, `.vol` files, and CD (if present).
  * If the `DEBUG.art_path` INI setting is specified, treat this directory as the highest precedence for all files.
* Trigger function callbacks are now of the form `void TrigFunc(OnTriggerArgs*)`. Legacy trigger callbacks of the form `void TrigFunc()` are still supported.
  * For more information, refer to TethysAPI: `Tethys/API/Trigger.h`
* Added new mission API callbacks:
  * `int OnLoad(OnLoadArgs*)` - Called when mission DLL is first loaded. Return 1 = success, 0 = failure.
  * `int OnUnload(OnUnloadArgs*)` - Called when mission DLL is unloaded. Unlike `InitProc()`, this is called earlier, and is also called when loading from a saved game. Return 1 = success, 0 = failure; failure is intended to cause the app to restart (not yet implemented).
  * `void OnEnd(OnEndArgs*)` - Called on mission end (does not include map restart or app exit). Unlike `OnUnload()`, this gets passed mission results data.
  * `void OnChat(OnChatArgs*)` - Called when any player sends a chat message.
  * `void OnCreateUnit(OnCreateUnitArgs*)` - Called when a unit is created.
  * `void OnDestroyUnit(OnDestroyUnitArgs*)` - Called when a unit is destroyed.
  * For more information, refer to TethysAPI: `Tethys/API/Mission.h` and `Tethys/API/Unit.h`
* Fix an issue with `TethysGame::ForceMorale*()` functions that caused them to have no effect when called for a specific player (unless called twice).
* Fix a bug with `ScStub` recycling that affected some custom missions, causing the game to crash if too many ScStubs were created.
* Double the size of the light-level adjustment table, allowing a potential map size of 1024x512. 
* Extend the map file header fields to allow specifying world-map wraparound on maps smaller than 512x256, or disabling wraparound on 512x256 maps.
  * Known issues: forcing wraparound on < 512-width maps causes the pathfinder to hang, the minimap is offset incorrectly on 128-width wraparound maps, and there are major rendering issues with 64-width wraparound maps.
* Force old dock vehicle damage mechanic that allows damaging allied vehicles for buildings that are set as special targets using `CreateSpecialTarget()`.
* Correctly draw single or dual turret sprites for Lynx, Panthers, and Tigers based whether the double-fire-rate flag is set.
* Fixed `Unit::DoInfect()` to work on maps with more than 1 player (human or AI).
* A debug "Run Script" dialog where arbitrary missions can be loaded may now be accessed by pressing the `D` key on the menu menu.
  * Known issue: game settings in this dialog are non-functional.
  * Multiplayer maps run offline via this method no longer immediately trigger victory and exit, and now have the "Restart Game" option.
* Fixed a buffer overflow bug caused by `ScGroup::TakeUnit()` leaking PathContext object memory.

### Miscellaneous changes

* OP2 internal version number is incremented to `1.4.0` (to ensure modded games cannot join multiplayer matches with the unmodded game by mistake).
* (CD) The CD is no longer required to be inserted to play the game.
* (CD) Configure the Indeo video codec to be used if the codec isn't installed or is disabled system-wide (as is the case in most modern Windows versions for security reasons), allowing game cutscenes to play.
* Fix a float conversion issue with the `sprintf` family of functions.
* (GOG) Reenable checking for the SIGS DLLs (not currently used, but allows overriding the SIGS DLLs in a future update which will be used to further improve netplay).
* Game speed defaults to 10 instead of 5.


#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/Resource/SoundID.h"

namespace Tethys {

struct DirectSoundBuffer;

struct SoundBufferInfo {
  SoundID            soundID;
  int                field_04;
  int                pixelY;
  int                pixelX;
  int                endTime;             ///< Milliseconds (timeGetTime)
  DirectSoundBuffer* pDirectSoundBuffer;
  int                volume;
  int                pan;
  int                field_20;
};
static_assert(sizeof(SoundBufferInfo) == 0x24, "Incorrect SoundBufferInfo size.");

// ** TODO
class SoundManager : public OP2Class<SoundManager> {
public:
  int  Init()   { return Thunk<0x47DFD0, &$::Init>();   }
  void Deinit() { return Thunk<0x47E070, &$::Deinit>(); }
  // 0x47E0B0 **
  // 0x47E170 **
  // 0x47E1B0 **
  // 0x47E1F0 **
  void SetPause(ibool pause) { return Thunk<0x47E370, &$::SetPause>(pause);    }
  void ShutDownAndClose()    { return Thunk<0x47E4C0, &$::ShutDownAndClose>(); }
  void StartGame()           { return Thunk<0x47E5A0, &$::StartGame>();        }
  void Pause()               { return Thunk<0x47E6A0, &$::Pause>();            }
  void Unpause()             { return Thunk<0x47E700, &$::Unpause>();          }
  // 0x47E710 **
  // 0x47E7A0 **
  // 0x47E830 **
  // 0x47E9F0 **
  // 0x47EC90 **
  void AddMapSound(int pixelX, int pixelY, SoundID soundID)
    { return Thunk<0x47EDB0, &$::AddMapSound>(pixelX, pixelY, soundID); }
  void AddGameSound(SoundID soundID, int playerMask)
    { return Thunk<0x47EFD0, &$::AddGameSound>(soundID, playerMask); }
  void AddSavantSound(SoundID soundID, int pixelX, int pixelY, int playerMask)
    { return Thunk<0x47F010, &$::AddSavantSound>(soundID, pixelX, pixelY, playerMask); }
  // 0x47F050 **
  // 0x47F200 **

  static SoundManager* GetInstance()           { return OP2Mem<0x56D250, SoundManager*>();                    }
  static auto&         GetSoundFilenameTable() { return OP2Mem<0x4E0170, char*(&)[size_t(SoundID::Count)]>(); }

public:
  // ** TODO member variables
  // 0x0
  // 0x4
  //int             numBuffersInUse_;     // 0x8 ** maybe?
  //SoundBufferInfo soundBufferInfo_[1];  // 0xC **

  // 0x8FC
  // 
  //int   field_90C;       // 0x90C **
  //int   field_910;       // 0x910 **
  //int   soundVolume_;    // 0x914 [Game: SoundVol]
  //ibool playSounds_;    // 0x918 [Game: Sound]

  // 0x91C
  //int bDirectSoundInit_;  // 0x920
  // 0x924 ???[]?  [0x80 bytes]
  //int time_;              // 0x9A4 [timeGetTime()]
  // 0x9A8
  // 0x9AC
};

class MusicManager : public OP2Class<MusicManager> {
public:
   MusicManager() { InternalCtor<0x450180>(); }
  ~MusicManager() { Thunk<0x4501D0>();         }

  void Init()               { return Thunk<0x4508D0, &$::Init>();               }
  void ReleaseSoundBuffer() { return Thunk<0x450760, &$::ReleaseSoundBuffer>(); }
  void Deinit()             { return Thunk<0x4507E0, &$::Deinit>();             }
  void ShutdownAndClose()   { return Thunk<0x450250, &$::ShutdownAndClose>();   }
  void ShutdownAndClose2()  { return Thunk<0x450AC0, &$::ShutdownAndClose2>();  }  // ** TODO what is this?

  int  OpenClmFile()  { return Thunk<0x450340, &$::OpenClmFile>();  }
  void CloseClmFile() { return Thunk<0x450620, &$::CloseClmFile>(); }

  int LoadIniMusicSettings()  { return Thunk<0x450210, &$::LoadIniMusicSettings>(); }
  int InitSongIndexes()       { return Thunk<0x450570, &$::InitSongIndexes>();      }
  int InitDirectSound()       { return Thunk<0x450680, &$::InitDirectSound>();      }

  void Pause()      { return Thunk<0x450BB0, &$::Pause>();      }
  void Unpause()    { return Thunk<0x450C20, &$::Unpause>();    }
  void Stop()       { return Thunk<0x450C60, &$::Stop>();       }
  void StartTimer() { return Thunk<0x450CC0, &$::StartTimer>(); }

  void SetPause(ibool on)         { return Thunk<0x450FB0, &$::SetPause>(on);           }
  void SetVolume(int volumeIndex) { return Thunk<0x451170, &$::SetVolume>(volumeIndex); }

  void SetMusicPlaylist(int numSongs, int repeatStartIndex, const SongID* pSongs)
    { return Thunk<0x4511B0, &$::SetMusicPlaylist>(numSongs, repeatStartIndex, pSongs); }

  static void MusicTimerCallback() { return OP2Thunk<0x450CF0, &$::MusicTimerCallback>(); }

  static MusicManager* GetInstance()      { return OP2Mem<0x565390, MusicManager*>();                   }
  static auto&         GetSongNameTable() { return OP2Mem<0x4E2700, char*(&)[size_t(SongID::Count)]>(); }

public:
  DirectSoundBuffer* pDirectSoundBuffer_;

  int   timerID_;
  ibool playing_;
  int   field_0C;
  int   volumeIndex_;
  int   pauseLockCount_;
  ibool playMusic_;

  void*  pHeaderData_;
  HANDLE hClmFile_;
  int    totalFileHeaderSize_;
  int    songIndex_[size_t(SongID::Count)];  ///< Initialized to -1

  CRITICAL_SECTION timerCriticalSection_;
  int              field_A8;
  CRITICAL_SECTION criticalSection_;       ///< Controls access to field_A8

  int     numPlaylistEntries_;
  int     repeatStartIndex_;
  SongID* pPlaylist_;
  int     currentPlayingSongIndex_;

  ibool hasBegunPlayback_;      ///< Has Playback begun at the first song yet
  int   currentSongFileIndex_;  ///< Clm file index
  int   currentSongPosition_;   ///< Byte offset into WAV data
};

inline auto& g_soundManager = *SoundManager::GetInstance();
inline auto& g_musicManager = *MusicManager::GetInstance();

} // Tethys

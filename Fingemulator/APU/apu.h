#pragma once

#ifdef ENABLE_APU

#include <cstdint>
using Byte = uint8_t;
using Word = uint16_t;

#include "nes_apu/Nes_Apu.h"
#include "nes_apu/apu_snapshot.h"
#include "Sound_Queue.h"
#include <string>

namespace APU {
    // Blargg Audio
    void OutputSamples(const blip_sample_t* samples, size_t count);
    int DMCRead(void*, cpu_addr_t address);

    // Read|Write Functions
    Byte write8(long elapsed, Word address, Byte val);
    Byte read8(long elapsed);

    // Managing APU
    void Init();
    void Reset();
    void ResetState();
    void RunFrame(long length);
    void ApuSaveState(apu_snapshot_t& save);
    void ApuLoadstate(apu_snapshot_t save);

    // Emulator Settings
    bool ToggleMuteAudio();
    bool ToggleOneChannel(int channel);
    void AdjustVolume(float adjust);

    std::string PrintVolume();
} // namespace APU

#else // Dummy Implementation

#include <cstdint>
using Byte = uint8_t;
using Word = uint16_t;

#include <string>

namespace APU {
    inline int DMCRead(void*, cpu_addr_t) { return 0; }

    // Read|Write Functions
    inline Byte write8(long, Word, Byte) { return 0; }
    inline Byte read8(long) { return 0; }

    // Managing APU
    inline void Init() {}
    inline void Reset() {}
    inline void ResetState() {}
    inline void RunFrame(long) {}
    inline void ApuLoadstate(apu_snapshot_t) {}

    // Emulator Settings
    inline bool ToggleMuteAudio() { return false; }
    inline bool ToggleOneChannel(int) { return false; }
    inline void AdjustVolume(float) {}

    inline std::string PrintVolume() { return "Muted"; }
} // namespace APU

#endif
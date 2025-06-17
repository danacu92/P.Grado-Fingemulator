#pragma once

#include "Sistema.h"
#include "NES.h"
class IEmulatorAPI
{
public:
    virtual void setSystem(int system) = 0;
    virtual bool load_rom(const char* path) = 0;
    virtual void updateInputs(uint16_t status) = 0;
    virtual uint32_t* advanceFrameOffline(bool muted = false) = 0;
    virtual uint8_t readMemory(uint16_t address) = 0;
    virtual ~IEmulatorAPI() {}
};
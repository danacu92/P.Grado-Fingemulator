#pragma once


#ifdef FINGEMULATORAPI_EXPORTS
#define FINGEMULATORAPI_API __declspec(dllexport)
#else
#define FINGEMULATORAPI_API __declspec(dllimport)
#endif

#include "iEmulator.h"
class EmuAPI : public IEmulatorAPI
{
private:
    std::shared_ptr<Sistema> consola = nullptr;

public:
    EmuAPI(){
      
    }

    void setSystem(int system) override
    {
        switch (system) {
        case 1:
            consola = std::make_shared<NES>();
            break;

        default:
            std::cout << "Valor de sistema no reconocido" << std::endl; 
            break;
        }
        
    
    }

    bool load_rom(const char* path) override {
        std::string pathStr = path;
        int i = consola->load_rom(pathStr);
        if (i > 0) {
            consola->turnON();
            return true;
        }
        else return false;
    }

    void updateInputs(uint16_t status) override
    {
        consola->UpdateInputs(status);
    }

    uint32_t* advanceFrameOffline(bool muted = false) override
    {
       return consola->run_frame(true);
    }

    uint8_t readMemory(uint16_t address) override
    {
        return consola->cpu_Read(address,true);
    }
};
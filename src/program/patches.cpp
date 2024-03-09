#include "patches.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void costumeRoomPatches() {
    patch::CodePatcher p(0x262850);
    p.WriteInst(inst::Movz(reg::W0, 0));
    p.Seek(0x2609B4);
    p.WriteInst(inst::Movz(reg::W0, 0));

    p.Seek(0x25FF74);
    p.WriteInst(inst::Movz(reg::W0, 1));
    p.Seek(0x25FF74);
    p.WriteInst(inst::Movz(reg::W0, 0));
}

void stubSocketInit() {
    patch::CodePatcher p(0x95C498);
    p.WriteInst(inst::Nop());
}

void enableDebugNvn() {
    patch::CodePatcher p(0x7312CC);
    p.WriteInst(inst::Nop());
}

void qualityOfLifePatches() {
    
    patch::CodePatcher p(0x1B3F0C);
    p.WriteInst(inst::Nop()); // disables call to open HTML viewer during first time odyssey flight

    p.Seek(0x1F2A2C); // patches checkpoint system to always allow warping
    p.WriteInst(inst::Movz(reg::W0, 1));

    p.Seek(0x216FAC); // disables AppearSwitchTimer's camera switch
    p.WriteInst(inst::Movz(reg::W0, 0));
}

void runCodePatches() {
    costumeRoomPatches();
    qualityOfLifePatches();
    stubSocketInit();
    enableDebugNvn();
}
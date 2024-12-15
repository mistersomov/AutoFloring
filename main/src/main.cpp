#include "IrrigationSystem.hpp"

extern "C" void app_main(void) {
    auto& system = autflr::IrrigationSystem::getInstance();
    system.launch();
}
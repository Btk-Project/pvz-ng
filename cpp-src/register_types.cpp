#include "register_types.hpp"

#include "plant.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using godot::MODULE_INITIALIZATION_LEVEL_SCENE;

void initialize_example_module(godot::ModuleInitializationLevel pLevel) {
    if (pLevel != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    GDREGISTER_CLASS(godot::Plant);
}

void uninitialize_example_module(godot::ModuleInitializationLevel pLevel) {
    if (pLevel != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT example_library_init(GDExtensionInterfaceGetProcAddress pGetProcAddress,
                                                const GDExtensionClassLibraryPtr pLibrary,
                                                GDExtensionInitialization* rInitialization) {
    godot::GDExtensionBinding::InitObject initObj(pGetProcAddress, pLibrary, rInitialization);

    initObj.register_initializer(initialize_example_module);
    initObj.register_terminator(uninitialize_example_module);
    initObj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return initObj.init();
}
}
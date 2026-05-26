/**
 * @file test_gui_manager.cpp
 * @brief Tests for GuiManager lifecycle and subsystem association.
 *
 * Headless-safe: constructor and accessor paths only — initialize() and
 * run_frame() require SDL+OpenGL and are not exercised here.
 */
#include "test_framework.h"
#include "test_fixtures.h"
#include "gui/gui_manager.h"

using namespace Amplitron;

TEST(gui_manager_basic_lifecycle) {
    AudioEngine engine;
    engine.initialize();

    // Construct GuiManager — safe without SDL/GL (constructor defers window setup)
    GuiManager gui(engine);

    // Audio engine reference is correctly stored
    ASSERT_EQ(&gui.audio_engine(), &engine);

    // MIDI manager is reachable through GuiManager
    auto& mm = gui.midi_manager();
    (void)mm;

    // Explicit shutdown before engine teardown
    gui.shutdown();
    engine.shutdown();
}

TEST(gui_manager_double_shutdown_is_safe) {
    AudioEngine engine;
    engine.initialize();

    GuiManager gui(engine);

    // shutdown() must guard against being called twice (initialized_ flag)
    gui.shutdown();
    gui.shutdown();  // Must not crash

    engine.shutdown();
}

TEST(gui_manager_midi_manager_association) {
    AudioEngine engine;
    engine.initialize();

    GuiManager gui(engine);

    // midi_manager() must return a stable reference (same address each call)
    ASSERT_EQ(&gui.midi_manager(), &gui.midi_manager());

    gui.shutdown();
    engine.shutdown();
}
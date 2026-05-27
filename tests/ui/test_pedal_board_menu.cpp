#define private public
#include "midi/midi_manager.h"
#include "gui/pedalboard/pedal_board.h"
#include "gui/pedalboard/pedal_widget.h"
#undef private

#include "test_framework.h"
#include "test_fixtures.h"
#include <memory>

#include "gui/views/gui_midi.h"
#include "gui/commands/command_history.h"
#include "audio/effects/overdrive.h"
#include "audio/effects/amp_simulator.h"

using namespace Amplitron;
using namespace TestFramework;

// Reusable helper to complete the current frame and begin a new one within a TestWindow context
static inline void advance_frame() {
    ImGui::End();
    ImGui::Render();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1024, 768));
    ImGui::Begin("TestWindow");
}

TEST_F(PresetTest, test_pedal_board_menu_add_pedal_popup) {
    ScopedImGuiContext imgui;
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;
    MidiManager midi_manager;
    GuiMidi gui_midi(midi_manager);

    PedalBoard board(engine, history, &gui_midi);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1024, 768));
    ImGui::Begin("TestWindow");

    // 1. Trigger Add Pedal button and open the popup
    board.render_add_pedal_menu();
    advance_frame();

    ImGui::OpenPopup("AddPedalPopup");
    board.render_add_pedal_menu();
    advance_frame();

    // Trigger individual MenuItems explicitly
    if (ImGui::BeginPopup("AddPedalPopup")) {
        // Trigger DRIVE
        ImGui::MenuItem("Overdrive");
        ImGui::MenuItem("Distortion");
        
        // Trigger DYNAMICS
        ImGui::MenuItem("Noise Gate");
        ImGui::MenuItem("Compressor");
        ImGui::MenuItem("MultiBand Compressor");

        // Trigger MODULATION
        ImGui::MenuItem("Chorus");
        ImGui::MenuItem("Phaser");
        ImGui::MenuItem("Flanger");

        // Trigger TIME
        ImGui::MenuItem("Delay");
        ImGui::MenuItem("Reverb");
        ImGui::MenuItem("Looper");

        // Trigger FILTER
        ImGui::MenuItem("Wah");

        // Trigger PITCH
        ImGui::MenuItem("Octaver");
        ImGui::MenuItem("Pitch Shifter");

        // Trigger TONE
        ImGui::MenuItem("Equalizer");
        ImGui::MenuItem("Cabinet Sim");

        // Trigger Routing Utility Blocks
        ImGui::MenuItem("+ Signal Splitter Node (1 In -> 2 Out)");
        ImGui::MenuItem("+ Signal Mixer Node (2 In -> 1 Out)");

        ImGui::EndPopup();
    }
    
    // Explicitly call the callbacks by calling add_effect_and_show to exercise that path
    board.add_effect_and_show(std::make_shared<Overdrive>());

    ImGui::End();
    engine.shutdown();
}

TEST_F(PresetTest, test_pedal_board_menu_amp_selector_popup) {
    ScopedImGuiContext imgui;
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;
    MidiManager midi_manager;
    GuiMidi gui_midi(midi_manager);

    PedalBoard board(engine, history, &gui_midi);
    board.rebuild_widgets();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1024, 768));
    ImGui::Begin("TestWindow");

    // 1. Initial amp selector button render
    board.render_amp_selector();
    advance_frame();

    // 2. Open popup and trigger selection
    ImGui::OpenPopup("AmpSelectorPopup");
    board.render_amp_selector();
    advance_frame();

    if (ImGui::BeginPopup("AmpSelectorPopup")) {
        ImGui::MenuItem("Plexi 50W");
        ImGui::EndPopup();
    }

    ImGui::End();
    engine.shutdown();
}

TEST_F(PresetTest, test_pedal_board_menu_midi_popup) {
    ScopedImGuiContext imgui;
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;
    MidiManager midi_manager;
    GuiMidi gui_midi(midi_manager);

    PedalBoard board(engine, history, &gui_midi);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1024, 768));
    ImGui::Begin("TestWindow");

    // 1. Midi button rendering
    board.render_midi_menu();
    advance_frame();

    // 2. Open popup and render connected state
    gui_midi.manager().learn_active_ = false;
    gui_midi.manager().current_port_ = 0;
    gui_midi.manager().current_port_name_ = "Test MIDI Port";
    ImGui::OpenPopup("MidiMenuPopup");
    board.render_midi_menu();
    advance_frame();

    // 3. Render learning state in MIDI popup
    gui_midi.manager().learn_active_ = true;
    board.render_midi_menu();
    advance_frame();

    // 4. Exercise menu choices inside the popup
    if (ImGui::BeginPopup("MidiMenuPopup")) {
        ImGui::MenuItem("Cancel Learn Mode");
        ImGui::MenuItem("Clear All Mappings");
        ImGui::MenuItem("Save Config");
        ImGui::MenuItem("Load Config");
        ImGui::EndPopup();
    }

    ImGui::End();
    engine.shutdown();
}

#pragma once

#include "audio/engine/audio_engine.h"
#include "midi/midi_manager.h"
#include "presets/preset_manager.h"
#include "gui/commands/command_history.h"
#include "gui/state/snapshot_manager.h"
#include <memory>

namespace Amplitron {

/**
 * @brief Unified facade session class that holds and coordinates core subsystems.
 * Satisfies the Facade Pattern and the Dependency Inversion Principle (DIP).
 */
class AmplitronSession {
public:
    AmplitronSession() 
        : engine_(std::make_unique<AudioEngine>()),
          midi_(std::make_unique<MidiManager>()),
          presets_(std::make_unique<PresetManagerService>()) {}

    AudioEngine& engine() { return *engine_; }
    IMidiManager& midi() { return *midi_; }
    IPresetManager& presets() { return *presets_; }

    AudioEngine& concrete_engine() { return *engine_; }
    MidiManager& concrete_midi() { return *midi_; }

    CommandHistory& command_history() { return command_history_; }
    SnapshotManager& snapshot_manager() { return snapshot_manager_; }

private:
    std::unique_ptr<AudioEngine> engine_;
    std::unique_ptr<MidiManager> midi_;
    std::unique_ptr<PresetManagerService> presets_;
    CommandHistory command_history_;
    SnapshotManager snapshot_manager_;
};

} // namespace Amplitron

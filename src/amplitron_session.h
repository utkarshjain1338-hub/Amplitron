#pragma once

#include "audio/engine/i_audio_engine.h"
#include "midi/i_midi_manager.h"
#include "presets/i_preset_manager.h"
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
    AmplitronSession(std::unique_ptr<IAudioEngine> engine = std::make_unique<AudioEngine>(),
                     std::unique_ptr<IMidiManager> midi = std::make_unique<MidiManager>(),
                     std::unique_ptr<IPresetManager> presets = std::make_unique<PresetManagerService>())
        : engine_(std::move(engine)),
          midi_(std::move(midi)),
          presets_(std::move(presets)) {}

    IAudioEngine& engine() { return *engine_; }
    IMidiManager& midi() { return *midi_; }
    IPresetManager& presets() { return *presets_; }

    AudioEngine& concrete_engine() { return static_cast<AudioEngine&>(*engine_); }
    MidiManager& concrete_midi() { return static_cast<MidiManager&>(*midi_); }

    CommandHistory& command_history() { return command_history_; }
    SnapshotManager& snapshot_manager() { return snapshot_manager_; }

private:
    std::unique_ptr<IAudioEngine> engine_;
    std::unique_ptr<IMidiManager> midi_;
    std::unique_ptr<IPresetManager> presets_;
    CommandHistory command_history_;
    SnapshotManager snapshot_manager_;
};

} // namespace Amplitron

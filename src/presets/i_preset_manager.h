#pragma once

#include <string>
#include <vector>

namespace Amplitron {

class IAudioEngine;
class IMidiManager;
struct PresetData;
struct MidiMapping;

/**
 * @brief Abstract interface for the Preset Manager.
 * Satisfies the Dependency Inversion Principle (DIP).
 */
class IPresetManager {
public:
    virtual ~IPresetManager() = default;

    virtual bool save_preset_data(const std::string& filepath, const PresetData& preset) = 0;
    virtual bool save_preset(const std::string& filepath, const std::string& preset_name,
                             const std::string& description, IAudioEngine& engine,
                             const std::vector<MidiMapping>& midi_mappings = {}) = 0;
    virtual bool load_preset(const std::string& filepath, IAudioEngine& engine,
                             IMidiManager* midi_manager = nullptr) = 0;
    virtual std::vector<std::string> list_presets() = 0;
};

} // namespace Amplitron


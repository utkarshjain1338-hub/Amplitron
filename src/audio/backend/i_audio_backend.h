#pragma once

#include "audio/backend/audio_device_info.h"
#include <vector>
#include <string>

namespace Amplitron {

class AudioEngine;

class IAudioBackend {
public:
    virtual ~IAudioBackend() = default;

    virtual bool initialize(AudioEngine* engine) = 0;
    virtual void shutdown() = 0;
    virtual bool start() = 0;
    virtual void stop() = 0;

    virtual std::vector<AudioDeviceInfo> get_input_devices() const = 0;
    virtual std::vector<AudioDeviceInfo> get_output_devices() const = 0;

    virtual bool set_input_device(int device_index) = 0;
    virtual bool set_output_device(int device_index) = 0;

    virtual std::string get_input_device_name() const = 0;
    virtual std::string get_output_device_name() const = 0;
};

} // namespace Amplitron

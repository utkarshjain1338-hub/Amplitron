#pragma once

#include <string>
#include <vector>

namespace Amplitron {

class AudioEngine;

struct AudioDeviceInfo {
    int index;
    std::string name;
    int max_input_channels;
    int max_output_channels;
    double default_sample_rate;
    bool is_usb_device;
};

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

// Forward declare the opaque backend state struct
struct AudioBackendState;

// Standard factory functions
AudioBackendState* create_audio_backend();
void destroy_audio_backend(AudioBackendState* state);

} // namespace Amplitron

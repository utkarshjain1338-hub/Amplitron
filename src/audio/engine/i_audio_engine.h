#pragma once

#include <vector>
#include <string>
#include "audio/backend/audio_device_info.h"

namespace Amplitron {

/**
 * @brief Abstract interface for audio device, latency, and sample rate management.
 * Satisfies the Interface Segregation Principle (ISP) and Dependency Inversion Principle (DIP).
 */
class IDeviceManager {
public:
    virtual ~IDeviceManager() = default;

    virtual std::vector<AudioDeviceInfo> get_input_devices() const = 0;
    virtual std::vector<AudioDeviceInfo> get_output_devices() const = 0;

    virtual bool set_input_device(int device_index) = 0;
    virtual bool set_output_device(int device_index) = 0;

    virtual int get_input_device() const = 0;
    virtual int get_output_device() const = 0;

    virtual std::string get_input_device_name() const = 0;
    virtual std::string get_output_device_name() const = 0;

    virtual int get_buffer_size() const = 0;
    virtual int get_sample_rate() const = 0;

    virtual void set_buffer_size(int size) = 0;
    virtual void set_sample_rate(int rate) = 0;

    virtual bool is_running() const = 0;
};

/**
 * @brief Abstract interface for audio level, RMS, clipping, and CPU load metrics.
 * Satisfies the Interface Segregation Principle (ISP) and Dependency Inversion Principle (DIP).
 */
class IAudioMetricsService {
public:
    virtual ~IAudioMetricsService() = default;

    virtual float get_input_level() const = 0;
    virtual float get_output_level() const = 0;

    virtual float get_input_rms() const = 0;
    virtual float get_output_rms() const = 0;

    virtual bool consume_input_clipped() = 0;
    virtual bool consume_output_clipped() = 0;

    virtual float get_cpu_load() const = 0;
};

} // namespace Amplitron

#pragma once

#include <string>

namespace Amplitron {

struct AudioDeviceInfo {
    int index;
    std::string name;
    int max_input_channels;
    int max_output_channels;
    double default_sample_rate;
    bool is_usb_device;
};

} // namespace Amplitron

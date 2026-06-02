#pragma once

#include <string>

namespace Amplitron {

// Runtime-editable effect parameter exposed to the UI and preset system.
struct EffectParam {
    std::string name;
    float value;
    float min_val;
    float max_val;
    float default_val;
    std::string unit;
    std::string tooltip;
};

} // namespace Amplitron

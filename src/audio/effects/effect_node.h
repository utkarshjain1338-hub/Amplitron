#pragma once

#include "audio/effects/effect.h"
#include <memory>
#include <atomic>
#include <vector>
#include <algorithm>
#include <cstring>

namespace Amplitron {

/**
 * @brief Decorator/wrapper class that wraps any IProcessor (or Effect) 
 * to handle bypass (enabled/disabled) and wet/dry mix computation in a clean, unified way.
 * 
 * Satisfies the Single Responsibility Principle (SRP) by offloading bypass/mixing
 * math from individual DSP algorithms.
 */
class EffectNode : public IProcessor {
public:
    explicit EffectNode(std::shared_ptr<IProcessor> processor)
        : processor_(std::move(processor)), enabled_(true), mix_(1.0f) {}

    void process(float* buffer, int num_samples) override {
        if (!enabled_.load(std::memory_order_relaxed)) {
            return; // bypassed
        }

        if (mix_.load(std::memory_order_relaxed) >= 1.0f) {
            processor_->process(buffer, num_samples);
            return;
        }

        std::vector<float> dry(num_samples);
        std::memcpy(dry.data(), buffer, static_cast<size_t>(num_samples) * sizeof(float));

        processor_->process(buffer, num_samples);

        float current_mix = mix_.load(std::memory_order_relaxed);
        for (int i = 0; i < num_samples; ++i) {
            buffer[i] = dry[i] * (1.0f - current_mix) + buffer[i] * current_mix;
        }
    }

    void process_stereo(float* left, float* right, int num_samples) override {
        if (!enabled_.load(std::memory_order_relaxed)) {
            return; // bypassed
        }

        if (mix_.load(std::memory_order_relaxed) >= 1.0f) {
            processor_->process_stereo(left, right, num_samples);
            return;
        }

        std::vector<float> dry_l(num_samples);
        std::vector<float> dry_r(num_samples);
        std::memcpy(dry_l.data(), left, static_cast<size_t>(num_samples) * sizeof(float));
        std::memcpy(dry_r.data(), right, static_cast<size_t>(num_samples) * sizeof(float));

        processor_->process_stereo(left, right, num_samples);

        float current_mix = mix_.load(std::memory_order_relaxed);
        for (int i = 0; i < num_samples; ++i) {
            left[i] = dry_l[i] * (1.0f - current_mix) + left[i] * current_mix;
            right[i] = dry_r[i] * (1.0f - current_mix) + right[i] * current_mix;
        }
    }

    void set_sample_rate(int sample_rate) override {
        processor_->set_sample_rate(sample_rate);
    }

    void reset() override {
        processor_->reset();
    }

    void set_enabled(bool enabled) {
        enabled_.store(enabled, std::memory_order_relaxed);
    }

    bool is_enabled() const {
        return enabled_.load(std::memory_order_relaxed);
    }

    void set_mix(float mix) {
        mix_.store(std::max(0.0f, std::min(1.0f, mix)), std::memory_order_relaxed);
    }

    float get_mix() const {
        return mix_.load(std::memory_order_relaxed);
    }

    IProcessor* get_processor() const {
        return processor_.get();
    }

private:
    std::shared_ptr<IProcessor> processor_;
    std::atomic<bool> enabled_;
    std::atomic<float> mix_;
};

} // namespace Amplitron

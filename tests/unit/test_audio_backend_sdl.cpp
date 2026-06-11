#include <SDL.h>

#include <cstdlib>
#include <vector>

#include "audio/backend/sdl_backend.h"
#include "audio/engine/audio_engine.h"
#include "test_framework.h"

// Declare the non-static callback we just exposed
namespace Amplitron {
void sdl_audio_callback(void* userdata, Uint8* stream, int len);
}

TEST(SdlBackend_LifecycleAndCallback) {
    // Set SDL to use the dummy driver
#ifdef _WIN32
    _putenv_s("SDL_AUDIODRIVER", "dummy");
#else
    setenv("SDL_AUDIODRIVER", "dummy", 1);
#endif

    Amplitron::AudioEngine engine;
    Amplitron::SdlBackend backend;

    // Check pre-init getters
    ASSERT_TRUE(backend.get_engine() == nullptr);
    ASSERT_EQ(backend.get_capture_device(), 0);
    ASSERT_EQ(backend.get_input_device(), 0);
    ASSERT_EQ(backend.get_output_device(), 0);

    // Initialize
    ASSERT_TRUE(backend.initialize(&engine));
    ASSERT_TRUE(backend.get_engine() == &engine);

    // Check device lists and names
    auto inputs = backend.get_input_devices();
    ASSERT_EQ(inputs.size(), 1u);
    ASSERT_EQ(inputs[0].name, "Browser Microphone");

    auto outputs = backend.get_output_devices();
    ASSERT_EQ(outputs.size(), 1u);
    ASSERT_EQ(outputs[0].name, "Browser Audio Output");

    ASSERT_EQ(backend.get_input_device_name(), "Browser Microphone");
    ASSERT_EQ(backend.get_output_device_name(), "Browser Audio Output");

    ASSERT_TRUE(backend.set_input_device(0));
    ASSERT_TRUE(backend.set_output_device(0));

    // Start streaming (under dummy audio driver, this succeeds)
    ASSERT_TRUE(backend.start());
    ASSERT_EQ(backend.get_sample_rate(), 48000);
    ASSERT_GT(backend.get_buffer_size(), 0);

    // Start again when already running should return false
    ASSERT_FALSE(backend.start());

    SDL_AudioDeviceID cap_dev = backend.get_capture_device();
    ASSERT_NE(cap_dev, 0u);

    // 1. Normal callback test with queued capture data
    std::vector<float> input_data(512, 1.0f);
    SDL_QueueAudio(cap_dev, input_data.data(), input_data.size() * sizeof(float));

    std::vector<float> output_buffer(1024, 0.0f);
    int bytes_len = 512 * 2 * sizeof(float);  // 512 stereo frames = 1024 floats = 4096 bytes
    Amplitron::sdl_audio_callback(&backend, reinterpret_cast<Uint8*>(output_buffer.data()),
                                  bytes_len);

    // 2. Test callback with junk/excess queue clearing
    std::vector<float> excess_input(4096 * 4, 1.0f);
    SDL_QueueAudio(cap_dev, excess_input.data(), excess_input.size() * sizeof(float));
    Amplitron::sdl_audio_callback(&backend, reinterpret_cast<Uint8*>(output_buffer.data()),
                                  bytes_len);

    // 3. Test callback with insufficient queued data (triggers partial memset)
    SDL_ClearQueuedAudio(cap_dev);
    std::vector<float> short_input(100, 1.0f);
    SDL_QueueAudio(cap_dev, short_input.data(), short_input.size() * sizeof(float));
    Amplitron::sdl_audio_callback(&backend, reinterpret_cast<Uint8*>(output_buffer.data()),
                                  bytes_len);

    // 4. Test callback when capture device is missing (cap_dev = 0, should memset 0)
    // We can simulate this by calling callback with userdata that has capture_device = 0.
    // However, the capture device is private in SdlBackend. But we can stop the backend,
    // which resets capture_device_ to 0, and then call callback (with engine still set).
    backend.stop();
    Amplitron::sdl_audio_callback(&backend, reinterpret_cast<Uint8*>(output_buffer.data()),
                                  bytes_len);

    // Cleanup
    backend.shutdown();
}

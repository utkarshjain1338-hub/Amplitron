#pragma once

#include <string>
#include <vector>

#include "audio/backend/audio_device_info.h"
#include "audio/backend/i_audio_backend.h"
#include "audio/backend/audio_backend_state.h"

namespace Amplitron {

// Standard factory functions
AudioBackendState* create_audio_backend();
void destroy_audio_backend(AudioBackendState* state);

} // namespace Amplitron

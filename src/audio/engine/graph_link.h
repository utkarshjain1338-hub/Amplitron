#pragma once

namespace Amplitron {

struct GraphLink {
  int id;
  int source_pin_id; // Mapping from Output Pin
  int dest_pin_id;   // Mapping to Input Pin
};

} // namespace Amplitron

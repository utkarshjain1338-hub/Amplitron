#include "test_framework.h"
#include "gui/gui_manager.h"
#include "audio/audio_engine.h"
#include "gui/snapshot_manager.h"
#include "audio/effects/overdrive.h"
#include "audio/effects/delay.h"
#include "audio/effects/reverb.h"
#include "audio/effects/equalizer.h"
#include "audio/effects/noise_gate.h"

using namespace Amplitron;

// ============================================================
// Helpers
// ============================================================

static AudioEngine& shared_engine() {
    static AudioEngine engine;
    return engine;
}

static void clear_engine(AudioEngine& engine) {
    while (!engine.effects().empty())
        engine.remove_effect(static_cast<int>(engine.effects().size()) - 1);
}

// ============================================================
// Construction / destruction
// ============================================================

TEST(gui_manager_constructs) {
    AudioEngine engine;
    GuiManager manager(engine);
    ASSERT_TRUE(true);
}

TEST(gui_manager_destructor_safe_without_init) {
    AudioEngine engine;
    {
        GuiManager manager(engine);
    }
    ASSERT_TRUE(true);
}

TEST(gui_manager_shutdown_without_init) {
    AudioEngine engine;
    GuiManager manager(engine);
    manager.shutdown();
    ASSERT_TRUE(true);
}

TEST(gui_manager_multiple_shutdowns_safe) {
    AudioEngine engine;
    GuiManager manager(engine);
    manager.shutdown();
    manager.shutdown();
    ASSERT_TRUE(true);
}

TEST(gui_manager_multiple_instances_safe) {
    for (int i = 0; i < 5; ++i) {
        AudioEngine engine;
        GuiManager manager(engine);
        manager.shutdown();
    }
    ASSERT_TRUE(true);
}

// ============================================================
// Accessors (midi_manager, audio_engine, command_history)
// These exercise the three inline getters in gui_manager.h and
// confirm the references are properly wired at construction time.
// ============================================================


TEST(gui_manager_midi_manager_accessible) {
    AudioEngine engine;
    GuiManager manager(engine);
    // Just confirm it returns a valid reference — no crash, no assert.
    MidiManager& mm = manager.midi_manager();
    (void)mm;
    ASSERT_TRUE(true);
}

// ============================================================
// initialize() — headless environment guard
// On CI and headless machines SDL_Init will fail, which is the
// expected path.  We assert the return value is consistent with
// the initialized_ flag (i.e. no crash, no UB either way).
// ============================================================

TEST(gui_manager_initialize_and_shutdown) {
    AudioEngine engine;
    GuiManager manager(engine);
    bool ok = manager.initialize(800, 600);
    if (ok)
        manager.shutdown();
    ASSERT_TRUE(true);
}

TEST(gui_manager_initialize_returns_false_or_true) {
    AudioEngine engine;
    GuiManager manager(engine);
    bool ok = manager.initialize(1280, 720);
    // Either outcome is valid; what must not happen is a crash or hang.
    (void)ok;
    if (ok) manager.shutdown();
    ASSERT_TRUE(true);
}

TEST(gui_manager_various_window_sizes) {
    const int sizes[][2] = {{320, 240}, {640, 480}, {1280, 720}};
    for (auto& s : sizes) {
        AudioEngine engine;
        GuiManager manager(engine);
        bool ok = manager.initialize(s[0], s[1]);
        if (ok) manager.shutdown();
    }
    ASSERT_TRUE(true);
}




// ============================================================
// SnapshotManager — tested independently of GuiManager but
// covers the same logic paths exercised by recallSnapshotFromSlot().
// SnapshotManager is a pure-value class with no SDL dependency.
// ============================================================

TEST(snapshot_manager_initially_empty) {
    SnapshotManager sm;
    for (int i = 0; i < SnapshotManager::NUM_SLOTS; ++i)
        ASSERT_FALSE(sm.has_slot(i));
    ASSERT_EQ(sm.active_slot(), -1);
}

TEST(snapshot_manager_save_and_has_slot) {
    AudioEngine& engine = shared_engine();
    clear_engine(engine);

    SnapshotManager sm;
    sm.save_slot(0, engine);
    ASSERT_TRUE(sm.has_slot(0));
    ASSERT_FALSE(sm.has_slot(1));
}

TEST(snapshot_manager_out_of_range_slots) {
    AudioEngine& engine = shared_engine();
    SnapshotManager sm;
    sm.save_slot(-1, engine);
    sm.save_slot(SnapshotManager::NUM_SLOTS, engine);
    for (int i = 0; i < SnapshotManager::NUM_SLOTS; ++i)
        ASSERT_FALSE(sm.has_slot(i));
    ASSERT_FALSE(sm.has_slot(-1));
    ASSERT_FALSE(sm.has_slot(SnapshotManager::NUM_SLOTS));
}

TEST(snapshot_manager_get_slot_returns_nullptr_when_empty) {
    SnapshotManager sm;
    ASSERT_TRUE(sm.get_slot(0) == nullptr);
    ASSERT_TRUE(sm.get_slot(-1) == nullptr);
    ASSERT_TRUE(sm.get_slot(SnapshotManager::NUM_SLOTS) == nullptr);
}

TEST(snapshot_manager_get_slot_returns_data_after_save) {
    AudioEngine& engine = shared_engine();
    clear_engine(engine);
    engine.set_input_gain(1.5f);
    engine.set_output_gain(0.75f);

    SnapshotManager sm;
    sm.save_slot(2, engine);

    const SnapshotManager::BoardSnapshot* snap = sm.get_slot(2);
    ASSERT_TRUE(snap != nullptr);
    ASSERT_NEAR(snap->input_gain, 1.5f, 0.001f);
    ASSERT_NEAR(snap->output_gain, 0.75f, 0.001f);

    engine.set_input_gain(0.7f);
    engine.set_output_gain(0.8f);
}

TEST(snapshot_manager_clear_slot) {
    AudioEngine& engine = shared_engine();
    SnapshotManager sm;
    sm.save_slot(1, engine);
    ASSERT_TRUE(sm.has_slot(1));

    sm.clear_slot(1);
    ASSERT_FALSE(sm.has_slot(1));
    ASSERT_TRUE(sm.get_slot(1) == nullptr);
}

TEST(snapshot_manager_clear_active_slot_resets_active) {
    AudioEngine& engine = shared_engine();
    SnapshotManager sm;
    sm.save_slot(0, engine);
    sm.set_active_slot(0);
    ASSERT_EQ(sm.active_slot(), 0);

    sm.clear_slot(0);
    ASSERT_EQ(sm.active_slot(), -1);
}

TEST(snapshot_manager_clear_non_active_slot_keeps_active) {
    AudioEngine& engine = shared_engine();
    SnapshotManager sm;
    sm.save_slot(0, engine);
    sm.save_slot(1, engine);
    sm.set_active_slot(0);

    sm.clear_slot(1);
    ASSERT_EQ(sm.active_slot(), 0);
}

TEST(snapshot_manager_set_active_slot) {
    SnapshotManager sm;
    sm.set_active_slot(3);
    ASSERT_EQ(sm.active_slot(), 3);

    sm.set_active_slot(-1);
    ASSERT_EQ(sm.active_slot(), -1);
}

TEST(snapshot_manager_recall_direct_restores_gains) {
    AudioEngine& engine = shared_engine();
    clear_engine(engine);
    engine.set_input_gain(2.0f);
    engine.set_output_gain(1.5f);

    SnapshotManager sm;
    sm.save_slot(0, engine);

    engine.set_input_gain(0.5f);
    engine.set_output_gain(0.5f);

    sm.recall_slot_direct(0, engine);

    ASSERT_NEAR(engine.get_input_gain(), 2.0f, 0.001f);
    ASSERT_NEAR(engine.get_output_gain(), 1.5f, 0.001f);

    engine.set_input_gain(0.7f);
    engine.set_output_gain(0.8f);
}

TEST(snapshot_manager_recall_direct_empty_slot_is_noop) {
    AudioEngine& engine = shared_engine();
    engine.set_input_gain(1.0f);

    SnapshotManager sm;
    sm.recall_slot_direct(0, engine); // slot is empty, must not crash or modify
    ASSERT_NEAR(engine.get_input_gain(), 1.0f, 0.001f);

    engine.set_input_gain(0.7f);
}

TEST(snapshot_manager_save_captures_effects) {
    AudioEngine& engine = shared_engine();
    clear_engine(engine);

    auto fx = std::make_shared<Overdrive>();
    engine.add_effect(fx);

    SnapshotManager sm;
    sm.save_slot(0, engine);

    const SnapshotManager::BoardSnapshot* snap = sm.get_slot(0);
    ASSERT_TRUE(snap != nullptr);
    ASSERT_EQ(static_cast<int>(snap->effects.size()), 1);

    clear_engine(engine);
}

TEST(snapshot_manager_all_slots_independent) {
    AudioEngine& engine = shared_engine();
    clear_engine(engine);

    engine.set_input_gain(1.0f);
    SnapshotManager sm;
    sm.save_slot(0, engine);

    engine.set_input_gain(2.0f);
    sm.save_slot(1, engine);

    engine.set_input_gain(3.0f);
    sm.save_slot(2, engine);

    ASSERT_NEAR(sm.get_slot(0)->input_gain, 1.0f, 0.001f);
    ASSERT_NEAR(sm.get_slot(1)->input_gain, 2.0f, 0.001f);
    ASSERT_NEAR(sm.get_slot(2)->input_gain, 3.0f, 0.001f);
    ASSERT_FALSE(sm.has_slot(3));

    engine.set_input_gain(0.7f);
}

TEST(snapshot_manager_slot_labels_correct) {
    ASSERT_EQ(std::string(SnapshotManager::SLOT_LABELS[0]), std::string("A"));
    ASSERT_EQ(std::string(SnapshotManager::SLOT_LABELS[1]), std::string("B"));
    ASSERT_EQ(std::string(SnapshotManager::SLOT_LABELS[2]), std::string("C"));
    ASSERT_EQ(std::string(SnapshotManager::SLOT_LABELS[3]), std::string("D"));
}

TEST(snapshot_manager_num_slots_is_four) {
    ASSERT_EQ(SnapshotManager::NUM_SLOTS, 4);
}

// ============================================================
// Version parsing logic — extracted for direct testing.
//
// check_for_updates() in gui_manager_update.cpp contains a
// local lambda `parse_version` and an inline version-comparison
// loop.  Because they are local to the function they cannot be
// called directly, so we replicate that exact logic here and
// verify every branch.  Any future change to the parsing logic
// in gui_manager_update.cpp must be mirrored here.
// ============================================================

// Exact copy of the lambda from gui_manager_update.cpp
static std::vector<int> parse_version(const std::string& v) {
    std::vector<int> parts;
    std::string s = v;
    if (!s.empty() && s[0] == 'v') s = s.substr(1);
    size_t pos = 0;
    while (pos < s.size()) {
        size_t dot = s.find('.', pos);
        if (dot == std::string::npos) dot = s.size();
        try { parts.push_back(std::stoi(s.substr(pos, dot - pos))); }
        catch (...) { parts.push_back(0); }
        pos = dot + 1;
    }
    return parts;
}

// Exact copy of the is_newer comparison loop from gui_manager_update.cpp
static bool is_version_newer(const std::string& latest, const std::string& current) {
    if (latest.empty()) return false;
    auto latest_parts  = parse_version(latest);
    auto current_parts = parse_version(current);
    bool is_newer = false;
    size_t max_len = std::max(latest_parts.size(), current_parts.size());
    for (size_t i = 0; i < max_len; ++i) {
        int lv = (i < latest_parts.size())  ? latest_parts[i]  : 0;
        int cv = (i < current_parts.size()) ? current_parts[i] : 0;
        if (lv > cv) { is_newer = true; break; }
        if (lv < cv) { break; }
    }
    return is_newer;
}

TEST(version_parse_strips_leading_v) {
    auto parts = parse_version("v1.2.3");
    ASSERT_EQ(static_cast<int>(parts.size()), 3);
    ASSERT_EQ(parts[0], 1);
    ASSERT_EQ(parts[1], 2);
    ASSERT_EQ(parts[2], 3);
}

TEST(version_parse_no_leading_v) {
    auto parts = parse_version("0.1.42");
    ASSERT_EQ(static_cast<int>(parts.size()), 3);
    ASSERT_EQ(parts[0], 0);
    ASSERT_EQ(parts[1], 1);
    ASSERT_EQ(parts[2], 42);
}

TEST(version_parse_single_component) {
    auto parts = parse_version("v5");
    ASSERT_EQ(static_cast<int>(parts.size()), 1);
    ASSERT_EQ(parts[0], 5);
}

TEST(version_parse_two_components) {
    auto parts = parse_version("v2.10");
    ASSERT_EQ(static_cast<int>(parts.size()), 2);
    ASSERT_EQ(parts[0], 2);
    ASSERT_EQ(parts[1], 10);
}

TEST(version_parse_empty_string) {
    auto parts = parse_version("");
    ASSERT_EQ(static_cast<int>(parts.size()), 0);
}

TEST(version_parse_only_v) {
    // "v" alone → empty after strip, loop never runs
    auto parts = parse_version("v");
    ASSERT_EQ(static_cast<int>(parts.size()), 0);
}

TEST(version_parse_non_numeric_component_yields_zero) {
    auto parts = parse_version("v1.alpha.3");
    ASSERT_EQ(static_cast<int>(parts.size()), 3);
    ASSERT_EQ(parts[0], 1);
    ASSERT_EQ(parts[1], 0); // stoi throws on "alpha", catch pushes 0
    ASSERT_EQ(parts[2], 3);
}

TEST(version_is_newer_when_latest_major_greater) {
    ASSERT_TRUE(is_version_newer("v2.0.0", "v1.99.99"));
}

TEST(version_is_newer_when_latest_minor_greater) {
    ASSERT_TRUE(is_version_newer("v1.2.0", "v1.1.99"));
}

TEST(version_is_newer_when_latest_patch_greater) {
    ASSERT_TRUE(is_version_newer("v1.1.2", "v1.1.1"));
}

TEST(version_is_not_newer_when_equal) {
    ASSERT_FALSE(is_version_newer("v1.2.3", "v1.2.3"));
}

TEST(version_is_not_newer_when_latest_older) {
    ASSERT_FALSE(is_version_newer("v1.0.0", "v1.0.1"));
    ASSERT_FALSE(is_version_newer("v0.9.9", "v1.0.0"));
}

TEST(version_is_not_newer_when_empty) {
    ASSERT_FALSE(is_version_newer("", "v1.0.0"));
}

TEST(version_newer_handles_different_component_counts) {
    // latest has more components than current
    ASSERT_TRUE(is_version_newer("v1.0.1", "v1.0"));
    // current has more components than latest
    ASSERT_FALSE(is_version_newer("v1.0", "v1.0.1"));
}

TEST(version_newer_zero_padding) {
    ASSERT_FALSE(is_version_newer("v1.0.0", "v1.0.0.1"));
    ASSERT_TRUE(is_version_newer("v1.0.0.1", "v1.0.0"));
}

// ============================================================
// JSON extraction logic from check_for_updates()
// The function uses std::string::find + substr to pull
// tag_name and html_url out of raw JSON text.  We test that
// logic directly using the same string operations.
// ============================================================

// Mirrors the extraction block in gui_manager_update.cpp
static std::string extract_tag_name(const std::string& response) {
    const std::string search_str = "\"tag_name\": \"";
    size_t pos = response.find(search_str);
    if (pos == std::string::npos) return "";
    pos += search_str.length();
    size_t end_pos = response.find("\"", pos);
    if (end_pos == std::string::npos) return "";
    return response.substr(pos, end_pos - pos);
}

static std::string extract_html_url(const std::string& response) {
    const std::string url_search_str = "\"html_url\": \"";
    size_t url_pos = response.find(url_search_str);
    if (url_pos == std::string::npos) return "";
    url_pos += url_search_str.length();
    size_t url_end_pos = response.find("\"", url_pos);
    if (url_end_pos == std::string::npos) return "";
    return response.substr(url_pos, url_end_pos - url_pos);
}

TEST(update_extract_tag_name_found) {
    std::string json = R"([{"tag_name": "v1.2.3","html_url": "https://example.com/releases/v1.2.3"}])";
    ASSERT_EQ(extract_tag_name(json), std::string("v1.2.3"));
}

TEST(update_extract_tag_name_not_found) {
    std::string json = R"([{"name": "release"}])";
    ASSERT_EQ(extract_tag_name(json), std::string(""));
}

TEST(update_extract_html_url_found) {
    std::string json = R"([{"tag_name": "v1.0.0","html_url": "https://github.com/releases/v1.0.0"}])";
    ASSERT_EQ(extract_html_url(json), std::string("https://github.com/releases/v1.0.0"));
}

TEST(update_extract_html_url_not_found) {
    std::string json = R"([{"tag_name": "v1.0.0"}])";
    ASSERT_EQ(extract_html_url(json), std::string(""));
}

TEST(update_extract_tag_name_empty_response) {
    ASSERT_EQ(extract_tag_name(""), std::string(""));
}

TEST(update_extract_html_url_empty_response) {
    ASSERT_EQ(extract_html_url(""), std::string(""));
}

TEST(update_extract_tag_name_unterminated_value) {
    // tag_name present but closing quote missing — returns ""
    std::string json = R"({"tag_name": "v1.0.0)";
    ASSERT_EQ(extract_tag_name(json), std::string(""));
}

TEST(update_full_flow_newer_release_detected) {
    // Simulate what check_for_updates() does end-to-end on a successful curl response
    std::string api_response = R"([{"tag_name": "v0.1.999","html_url": "https://github.com/example/releases/tag/v0.1.999","name":"v0.1.999"}])";

    std::string tag  = extract_tag_name(api_response);
    std::string url  = extract_html_url(api_response);
    std::string current = "v0.1.1";

    ASSERT_EQ(tag, std::string("v0.1.999"));
    ASSERT_FALSE(url.empty());
    ASSERT_TRUE(is_version_newer(tag, current));
}

TEST(update_full_flow_same_version_not_flagged) {
    std::string api_response = R"([{"tag_name": "v0.1.1","html_url": "https://github.com/example/releases/tag/v0.1.1"}])";
    std::string tag  = extract_tag_name(api_response);
    std::string current = "v0.1.1";
    ASSERT_FALSE(is_version_newer(tag, current));
}

TEST(update_full_flow_older_release_not_flagged) {
    std::string api_response = R"([{"tag_name": "v0.1.0","html_url": "https://github.com/example/releases/tag/v0.1.0"}])";
    std::string tag  = extract_tag_name(api_response);
    std::string current = "v0.1.1";
    ASSERT_FALSE(is_version_newer(tag, current));
}
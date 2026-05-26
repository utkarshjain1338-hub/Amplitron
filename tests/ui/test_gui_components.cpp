/**
 * @file test_gui_components.cpp
 * @brief Headless-safe tests for UI components (Theme, EffectColor, GuiSettings).
 *
 * Tests unit-aware parameter formatting, effect color lookups, and basic
 * settings class instantiation.
 */
#include "test_framework.h"
#include "test_fixtures.h"
#include "gui/theme.h"
#include "gui/gui_settings.h"
#include <string>

using namespace Amplitron;

// ============================================================
// Theme Parameter Value Formatting
// ============================================================

TEST(theme_format_parameter_value_hz_low) {
    std::string formatted = Theme::formatParameterValue(440.0f, "Hz");
    ASSERT_EQ(formatted, "440 Hz");
}

TEST(theme_format_parameter_value_hz_high) {
    std::string formatted = Theme::formatParameterValue(1500.0f, "Hz");
    ASSERT_EQ(formatted, "1.5 kHz");
}

TEST(theme_format_parameter_value_db) {
    std::string formatted = Theme::formatParameterValue(-6.0f, "dB");
    ASSERT_EQ(formatted, "-6.0 dB");
}

TEST(theme_format_parameter_value_percent) {
    std::string formatted = Theme::formatParameterValue(75.2f, "%");
    ASSERT_EQ(formatted, "75%");
}

TEST(theme_format_parameter_value_pct) {
    std::string formatted = Theme::formatParameterValue(50.8f, "pct");
    ASSERT_EQ(formatted, "51%");
}

TEST(theme_format_parameter_value_ms) {
    std::string formatted = Theme::formatParameterValue(250.0f, "ms");
    ASSERT_EQ(formatted, "250.0 ms");
}

TEST(theme_format_parameter_value_seconds) {
    std::string formatted = Theme::formatParameterValue(1.234f, "s");
    ASSERT_EQ(formatted, "1.23 s");
}

TEST(theme_format_parameter_value_custom_unit) {
    std::string formatted = Theme::formatParameterValue(3.5f, "oct");
    ASSERT_EQ(formatted, "3.5 oct");
}

TEST(theme_format_parameter_value_empty_unit) {
    std::string formatted = Theme::formatParameterValue(5.0f, "");
    ASSERT_EQ(formatted, "5.0");
}

// ============================================================
// Theme RecBlink Oscillation
// ============================================================

TEST(theme_rec_blink_values) {
    // std::sin(t * 4.0f) > 0.0f triggers alpha = 1.0f, else 0.3f
    ImVec4 col_on = Theme::RecBlink(0.2f); // sin(0.8) > 0
    ASSERT_EQ(col_on.w, 1.0f);

    ImVec4 col_off = Theme::RecBlink(1.0f); // sin(4.0) < 0
    ASSERT_EQ(col_off.w, 0.3f);
}

// ============================================================
// get_effect_color Table Lookups
// ============================================================

TEST(get_effect_color_known_effects) {
    const char* effects[] = {
        "Distortion", "Overdrive", "Delay", "Reverb", "Looper",
        "Chorus", "Phaser", "Flanger", "Equalizer", "Noise Gate",
        "Compressor", "MultiBand Compressor", "Cabinet", "Octaver",
        "Pitch Shifter", "Tuner"
    };

    for (const char* name : effects) {
        const EffectColorEntry* entry = get_effect_color(name);
        ASSERT_STR_EQ(entry->name, name);
    }
}

TEST(get_effect_color_fallback) {
    const EffectColorEntry* entry = get_effect_color("NonExistentFX");
    ASSERT_STR_EQ(entry->name, "Default");
}

// ============================================================
// GuiSettings Construction
// ============================================================

TEST_F(PresetTest, gui_settings_construction_no_crash) {
    GuiSettings settings(engine);
    (void)settings;
}

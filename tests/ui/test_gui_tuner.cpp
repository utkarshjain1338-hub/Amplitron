/**
 * @file test_gui_tuner.cpp
 * @brief Headless-safe tests for GuiTuner state management.
 *
 * Tests toggle() and tuner_instance() — render() is not called as it requires ImGui.
 */
#include "test_framework.h"
#include "test_fixtures.h"
#include "gui/gui_tuner.h"
#include "audio/effects/tuner.h"
#include <memory>

using namespace Amplitron;

TEST_F(PresetTest, gui_tuner_construction_no_crash) {
    auto tuner = std::make_shared<TunerPedal>();
    GuiTuner gt(engine, tuner);
    (void)gt;
}

TEST_F(PresetTest, gui_tuner_tuner_instance_returns_correct_shared_ptr) {
    auto tuner = std::make_shared<TunerPedal>();
    GuiTuner gt(engine, tuner);
    ASSERT_EQ(gt.tuner_instance(), tuner);
}

TEST_F(PresetTest, gui_tuner_toggle_on_enables_tuner_and_sets_tap) {
    auto tuner = std::make_shared<TunerPedal>();
    GuiTuner gt(engine, tuner);

    bool show = false;
    gt.toggle(show);

    // show should toggle to true
    ASSERT_TRUE(show);
    // tuner should be enabled
    ASSERT_TRUE(tuner->is_enabled());
    // engine should have the tuner tap set
    ASSERT_EQ(engine.get_tuner_tap(), tuner);
}

TEST_F(PresetTest, gui_tuner_toggle_off_disables_tuner_and_clears_tap) {
    auto tuner = std::make_shared<TunerPedal>();
    GuiTuner gt(engine, tuner);

    bool show = true;
    gt.toggle(show);

    // show should toggle to false
    ASSERT_FALSE(show);
    // tuner should be disabled
    ASSERT_FALSE(tuner->is_enabled());
    // engine should have tuner tap cleared
    ASSERT_EQ(engine.get_tuner_tap(), nullptr);
}

TEST_F(PresetTest, gui_tuner_multiple_toggles_are_consistent) {
    auto tuner = std::make_shared<TunerPedal>();
    GuiTuner gt(engine, tuner);

    bool show = false;

    // Toggle On
    gt.toggle(show);
    ASSERT_TRUE(show);
    ASSERT_TRUE(tuner->is_enabled());
    ASSERT_EQ(engine.get_tuner_tap(), tuner);

    // Toggle Off
    gt.toggle(show);
    ASSERT_FALSE(show);
    ASSERT_FALSE(tuner->is_enabled());
    ASSERT_EQ(engine.get_tuner_tap(), nullptr);

    // Toggle On Again
    gt.toggle(show);
    ASSERT_TRUE(show);
    ASSERT_TRUE(tuner->is_enabled());
    ASSERT_EQ(engine.get_tuner_tap(), tuner);
}

/**
 * @file test_gui_recording.cpp
 * @brief Headless-safe tests for GuiRecording logic.
 *
 * Tests properties, setters, and state variables — render functions (calling ImGui)
 * are excluded.
 */
#include "test_framework.h"
#include "test_fixtures.h"
#include "gui/gui_recording.h"
#include "audio/recorder.h"
#include <string>

using namespace Amplitron;

TEST_F(PresetTest, gui_recording_construction_no_crash) {
    GuiRecording gr(engine);
    (void)gr;
}

TEST_F(PresetTest, gui_recording_initial_save_pending_is_false) {
    GuiRecording gr(engine);
    ASSERT_FALSE(gr.is_save_pending());
}

TEST_F(PresetTest, gui_recording_set_save_pending_works) {
    GuiRecording gr(engine);
    gr.set_save_pending(true);
    ASSERT_TRUE(gr.is_save_pending());
    gr.set_save_pending(false);
    ASSERT_FALSE(gr.is_save_pending());
}

TEST_F(PresetTest, gui_recording_initial_show_save_is_false) {
    GuiRecording gr(engine);
    ASSERT_FALSE(gr.show_save());
}

TEST_F(PresetTest, gui_recording_show_save_reference_can_be_modified) {
    GuiRecording gr(engine);
    bool& ref = gr.show_save();
    ref = true;
    ASSERT_TRUE(gr.show_save());
}

TEST_F(PresetTest, gui_recording_status_message_initially_empty) {
    GuiRecording gr(engine);
    ASSERT_TRUE(gr.status_message().empty());
}

TEST_F(PresetTest, gui_recording_status_message_can_be_written) {
    GuiRecording gr(engine);
    gr.status_message() = "Test message";
    ASSERT_EQ(gr.status_message(), "Test message");
}

TEST_F(PresetTest, gui_recording_render_save_dialog_early_return_when_not_pending) {
    GuiRecording gr(engine);
    bool show = true;
    gr.set_save_pending(false);
    
    // When not pending, should set show to false and return immediately (no dialog)
    gr.render_save_dialog(show);
    ASSERT_FALSE(show);
}

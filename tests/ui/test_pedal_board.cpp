/**
 * @file test_pedal_board.cpp
 * @brief Tests for PedalBoard and PedalWidget (headless, no ImGui rendering).
 *
 * Covers: widget rebuild logic, active-only filtering, AmpSimulator detection,
 * PedalWidget index/effect/history accessors.
 */
#include "test_framework.h"
#include "test_fixtures.h"
#include "gui/pedal_board.h"
#include "gui/pedal_widget.h"
#include "gui/command_history.h"
#include "audio/effects/overdrive.h"
#include "audio/effects/reverb.h"
#include "audio/effects/amp_simulator.h"
#include <memory>

using namespace Amplitron;

TEST(pedal_board_construction_empty_engine) {
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;

    PedalBoard board(engine, history);
    ASSERT_TRUE(board.show_active_only());

    engine.shutdown();
}

TEST(pedal_board_rebuild_after_add_effect) {
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;

    PedalBoard board(engine, history);

    auto od = std::make_shared<Overdrive>();
    engine.add_effect(od);
    board.rebuild_widgets();   // Must not crash

    engine.shutdown();
}

TEST(pedal_board_rebuild_with_amp_simulator) {
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;

    PedalBoard board(engine, history);

    auto od  = std::make_shared<Overdrive>();
    auto amp = std::make_shared<AmpSimulator>();
    engine.add_effect(od);
    engine.add_effect(amp);
    board.rebuild_widgets();

    // Post-amp effects are excluded from visible indices — board must remain stable
    auto rev = std::make_shared<Reverb>();
    engine.add_effect(rev);
    board.rebuild_widgets();

    engine.shutdown();
}

TEST(pedal_board_multiple_rebuilds_are_stable) {
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;

    PedalBoard board(engine, history);

    auto od = std::make_shared<Overdrive>();
    engine.add_effect(od);

    for (int i = 0; i < 10; ++i) {
        board.rebuild_widgets();
    }

    engine.shutdown();
}

TEST(pedal_board_nullptr_gui_midi_is_safe) {
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;

    // Explicit nullptr for gui_midi — must not crash during rebuild
    PedalBoard board(engine, history, nullptr);
    auto od = std::make_shared<Overdrive>();
    engine.add_effect(od);
    board.rebuild_widgets();

    engine.shutdown();
}

TEST(pedal_widget_index_accessor) {
    AudioEngine engine;
    engine.initialize();

    auto od = std::make_shared<Overdrive>();
    PedalWidget widget(engine, od, 0);

    ASSERT_EQ(widget.get_index(), 0);
    widget.set_index(5);
    ASSERT_EQ(widget.get_index(), 5);

    engine.shutdown();
}

TEST(pedal_widget_effect_accessor_returns_same_ptr) {
    AudioEngine engine;
    engine.initialize();

    auto od = std::make_shared<Overdrive>();
    PedalWidget widget(engine, od, 2);

    ASSERT_EQ(widget.get_effect(), od);

    engine.shutdown();
}

TEST(pedal_widget_set_history) {
    AudioEngine engine;
    engine.initialize();

    auto od = std::make_shared<Overdrive>();
    CommandHistory history;
    PedalWidget widget(engine, od, 0);

    widget.set_history(&history);    // Must not crash
    widget.set_history(nullptr);     // nullptr also safe

    engine.shutdown();
}

TEST(pedal_widget_set_gui_midi_nullptr_is_safe) {
    AudioEngine engine;
    engine.initialize();

    auto od = std::make_shared<Overdrive>();
    PedalWidget widget(engine, od, 0);

    widget.set_gui_midi(nullptr);    // Must not crash

    engine.shutdown();
}

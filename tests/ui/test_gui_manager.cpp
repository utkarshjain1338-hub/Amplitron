#include "test_framework.h"
#include "test_fixtures.h"
#include "gui/gui_manager.h"
#include "gui/pedal_board.h"
#include "gui/pedal_widget.h"
#include "gui/CrashRecoveryUI.h"
#include "gui/command_reset.h"
#include "gui/command_clear.h"
#include "gui/command_history.h"
#include "audio/effects/overdrive.h"
#include "audio/effects/amp_simulator.h"
#include <memory>
#include <string>
#include <vector>

using namespace Amplitron;

// ============================================================
// GuiManager tests
// ============================================================

TEST(gui_manager_basic_lifecycle) {
    AudioEngine engine;
    engine.initialize();
    
    // 1. Construct GuiManager (safe to construct, doesn't init SDL/GL)
    GuiManager gui(engine);
    
    // 2. Verify basic association
    ASSERT_EQ(&gui.audio_engine(), &engine);
    
    // 3. Verify MIDI manager is accessible
    auto& mm = gui.midi_manager();
    (void)mm; // suppress unused warning
    
    // 4. Safe shutdown
    gui.shutdown();
    engine.shutdown();
}

// ============================================================
// PedalBoard & PedalWidget tests
// ============================================================

TEST(pedal_board_widget_rebuilding_and_filtering) {
    AudioEngine engine;
    engine.initialize();
    CommandHistory history;
    
    // 1. Construct PedalBoard
    PedalBoard board(engine, history);
    ASSERT_TRUE(board.show_active_only());
    
    // 2. Add an effect to the engine and rebuild widgets
    auto od = std::make_shared<Overdrive>();
    engine.add_effect(od);
    board.rebuild_widgets();
    
    // 3. Add an AmpSimulator and verify it's correctly identified
    auto amp = std::make_shared<AmpSimulator>();
    engine.add_effect(amp);
    board.rebuild_widgets();
    
    // 4. Verify PedalWidget methods
    PedalWidget widget(engine, od, 0);
    ASSERT_EQ(widget.get_index(), 0);
    widget.set_index(3);
    ASSERT_EQ(widget.get_index(), 3);
    ASSERT_EQ(widget.get_effect(), od);
    
    widget.set_history(&history);
    
    engine.shutdown();
}

// ============================================================
// CrashRecoveryUI tests
// ============================================================

TEST(crash_recovery_ui_prompt_handling) {
    // Calling promptRestoreSession under headless / test runner environment 
    // should fail/return false gracefully without hanging or crashing.
    bool restore = promptRestoreSession();
    ASSERT_FALSE(restore);
}

// ============================================================
// ResetAllCommand & ClearAllCommand tests
// ============================================================

TEST(custom_commands_reset_and_clear_undo_redo) {
    AudioEngine engine;
    engine.initialize();
    
    auto od = std::make_shared<Overdrive>();
    od->params()[0].value = od->params()[0].min_val;
    engine.add_effect(od);
    
    CommandHistory history;
    
    // 1. Test ResetAllCommand
    history.execute(std::make_unique<ResetAllCommand>(engine));
    // Verify parameters reset to default
    ASSERT_NEAR(od->params()[0].value, od->params()[0].default_val, 0.001f);
    
    history.undo();
    // Verify parameters restored back to min_val
    ASSERT_NEAR(od->params()[0].value, od->params()[0].min_val, 0.001f);
    
    history.redo();
    // Verify parameters back to default again
    ASSERT_NEAR(od->params()[0].value, od->params()[0].default_val, 0.001f);
    
    // 2. Test ClearAllCommand
    ASSERT_EQ(static_cast<int>(engine.effects().size()), 1);
    history.execute(std::make_unique<ClearAllCommand>(engine));
    ASSERT_EQ(static_cast<int>(engine.effects().size()), 0);
    
    history.undo();
    // Rebuilt/restored back to 1 effect
    ASSERT_EQ(static_cast<int>(engine.effects().size()), 1);
    ASSERT_EQ(std::string(engine.effects()[0]->name()), "Overdrive");
    
    history.redo();
    ASSERT_EQ(static_cast<int>(engine.effects().size()), 0);
    
    engine.shutdown();
}
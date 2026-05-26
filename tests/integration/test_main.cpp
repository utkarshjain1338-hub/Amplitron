#include "test_framework.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <csignal>

// Redefine main to prevent link collisions and test the main function directly
#define main app_main
#include "main.cpp"
#undef main

TEST(main_cli_help_exits_with_zero) {
    char* argv[] = { (char*)"amplitron", (char*)"--help" };
    
    // Redirect std::cout and std::cerr to keep test suite output clean
    std::streambuf* old_out = std::cout.rdbuf();
    std::streambuf* old_err = std::cerr.rdbuf();
    std::stringstream ss_out, ss_err;
    std::cout.rdbuf(ss_out.rdbuf());
    std::cerr.rdbuf(ss_err.rdbuf());
    
    int exit_code = app_main(2, argv);
    
    std::cout.rdbuf(old_out);
    std::cerr.rdbuf(old_err);
    
    ASSERT_EQ(exit_code, 0);
    ASSERT_TRUE(ss_out.str().find("Usage: amplitron") != std::string::npos);
}

TEST(main_cli_version_exits_with_zero) {
    char* argv[] = { (char*)"amplitron", (char*)"--version" };
    
    std::streambuf* old_out = std::cout.rdbuf();
    std::streambuf* old_err = std::cerr.rdbuf();
    std::stringstream ss_out, ss_err;
    std::cout.rdbuf(ss_out.rdbuf());
    std::cerr.rdbuf(ss_err.rdbuf());
    
    int exit_code = app_main(2, argv);
    
    std::cout.rdbuf(old_out);
    std::cerr.rdbuf(old_err);
    
    ASSERT_EQ(exit_code, 0);
    ASSERT_TRUE(ss_out.str().find("Amplitron v1.0") != std::string::npos);
}

TEST(main_signal_handler_resets_running_flag) {
    // Call the signal_handler directly to verify execution stability
    signal_handler(SIGINT);
    ASSERT_FALSE(g_running);
    
    // Reset g_running to true so subsequent frames or main loops aren't disrupted
    g_running = true;
    signal_handler(SIGTERM);
    ASSERT_FALSE(g_running);
    
    g_running = true; // reset
}

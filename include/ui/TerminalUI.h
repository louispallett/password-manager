#pragma once

#include <vector>
#include <string>

#include "app/Action.h"
#include "app/State.h"
#include "vault/Entry.h"
#include "util/Expected.h"
#include "util/SecureString.h"

namespace app { class State; }

namespace ui
{

class TerminalUI
{
public:
    TerminalUI() 
    {
        initialize();
    }

    ~TerminalUI()
    {
        shutdown();
    }

    void initialize();

    void display_logo();

    void show_message(const std::string& message);
    void show_error(const std::string& error);

    // Left side
    app::Action prompt_action(const std::vector<app::MenuOption>& options);

    // Right side
    void list_entries(const std::vector<vault::Entry>& entries);
    util::Expected<util::SecureString, std::string> prompt_master_password();
    util::Expected<util::SecureString, std::string> prompt_input(std::string prompt);
    void display_entry(const vault::Entry& entry);
    void remove_entry (const size_t index);

private:
    void shutdown();
    int m_content_start_row_ = 0;
    int message_content_height_ = 3;
    int prompt_input_height_ = 5;
};

}

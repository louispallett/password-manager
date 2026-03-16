#pragma once

#include <memory>
#include <optional>
#include <string>

#include "ui/TerminalUI.h"
#include "vault/VaultSession.h"

namespace app { enum class Action; }
namespace app { class State; }

namespace app
{

class Application
{
public:
    explicit Application(std::string vault_path);

    void run(Application& app);

    bool vault_exists();
    void change_state(std::unique_ptr<State> new_state);
    
private:
    bool handle_action(Action action);
    void transition_state(Action action);

    bool handle_create_vault();
    bool handle_unlock();
    bool handle_add_entry();
    bool handle_remove_entry();
    bool handle_list_entries();
    bool handle_save_only();
    bool handle_save_and_close();
    bool handle_quit();

private:
    std::string vault_path_;
    std::unique_ptr<State> current_state_;
    std::optional<vault::VaultSession> session_;
    ui::TerminalUI ui_;
    bool running_ { true };
};

}

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
    void handle_action(Action action);
    void transition_state(Action action);

    void handle_create_vault();
    void handle_unlock();
    void handle_add_entry();
    void handle_remove_entry();
    void handle_list_entries();
    void handle_save_only();
    void handle_save_and_close();
    void handle_quit();

private:
    std::string vault_path_;
    std::unique_ptr<State> current_state_;
    std::optional<vault::VaultSession> session_;
    ui::TerminalUI ui_;
    bool running_ { true };
};

}

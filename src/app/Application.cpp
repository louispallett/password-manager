#include "app/Application.h"
#include "app/BootstrapState.h"
#include "crypto/CryptoContext.h"
#include "vault/Entry.h"
#include "vault/VaultFile.h"
#include "ui/TerminalUI.h"
#include "app/State.h"
#include "app/Action.h"
#include "vault/VaultFileError.h"
#include <filesystem>
#include <memory>

namespace app
{

Application::Application(std::string vault_path)
    : vault_path_(std::move(vault_path))
{

}

void Application::run(Application& app)
{
    crypto::CryptoContext::init();
    ui_.initialize();
    current_state_ = std::make_unique<BootstrapState>();
    if (auto next = current_state_->on_enter(*this))
    {
        current_state_ = std::move(next);
    }

    ui_.display_logo();

    while (running_)
    {
        auto options = current_state_->menu_options();

        Action action = ui_.prompt_action(options);

        if (!current_state_->allows(action))
        {
            ui_.show_error("Action not allowed in current state");
            continue;
        }

        handle_action(action);
        transition_state(action);
    }
}

void Application::handle_action(Action action)
{
    switch (action) 
    {
        case Action::CreateVault:
            handle_create_vault();
            break;
        case Action::Unlock:
            handle_unlock();
            break;
        case Action::AddEntry:
            handle_add_entry();
            break;
        case Action::AlterEntry:
            handle_alter_entry();
            break;
        case Action::RemoveEntry:
            handle_remove_entry();
            break;
        case Action::ListEntries:
            handle_list_entries();
            break;
        case Action::SaveAndClose:
            handle_save_and_close();
            break;
        case Action::Quit:
            handle_quit();
        default:
            break;
    }
}

void Application::transition_state(Action action)
{
    auto new_state = current_state_->transition(action);
    if (!new_state)
    {
        return;
    }
    current_state_ = std::move(new_state);
}

bool Application::vault_exists()
{
    return std::filesystem::exists(vault_path_);
}

void Application::change_state(std::unique_ptr<State> new_state)
{
    current_state_ = std::move(new_state); 
}

void Application::handle_create_vault()
{
    auto password = ui_.prompt_master_password();

    auto result = vault::VaultFile::create_new(
        vault_path_,
        std::move(password.value())
    );

    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    ui_.show_message("Vault created successfully.");
}

void Application::handle_unlock()
{
    auto password = ui_.prompt_master_password();

    auto loaded = vault::VaultFile::load(vault_path_, std::move(password.value()));
    
    if (!loaded)
    {
        ui_.show_error(vault::to_string(loaded.error()));
        return;
    }

    vault_ = std::move(loaded.value());
    ui_.show_message("Vault Unlocked");
}

void Application::handle_add_entry()
{
    if (!vault_)
    {
        ui_.show_error("Vault not unlocked.");
        return;
    }

    auto name = ui_.prompt_input("Name");
    auto username = ui_.prompt_input("Username");
    auto password = ui_.prompt_input("Password");
    if (!name || !username || !password)
    {
        return;
    }

    vault::Entry new_entry {
        std::move(name.value()), 
        std::move(username.value()), 
        std::move(password.value())
    };

    auto result = vault_->add_entry(std::move(new_entry));
    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    ui_.show_message("Entry added successfully.");
}

void Application::handle_alter_entry()
{
    if (!vault_)
    {
        ui_.show_error("Vault not unlocked");
        return;
    }

    auto index = ui_.select_entry(vault_->entries());
    if (!index)
    {
        return;
    }

    auto name = ui_.prompt_input("Name");
    auto username = ui_.prompt_input("Username");
    auto password = ui_.prompt_input("Password");
    if (!name || !username || !password)
    {
        return;
    }

    vault::Entry new_entry {
        std::move(name.value()), 
        std::move(username.value()), 
        std::move(password.value())
    };

    auto result = vault_->update_entry(*index, std::move(new_entry));
    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    ui_.show_message("Entry updated successfully.");
}

void Application::handle_remove_entry()
{
    if (!vault_)
    {
        ui_.show_error("Vault not unlocked");
        return;
    }

    auto index = ui_.select_entry(vault_->entries());
    if (!index)
    {
        return;
    }

    auto result = vault_->remove_entry(index.value());
    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    ui_.show_message("Entry deleted successfully.");
}

void Application::handle_list_entries()
{
    if (!vault_)
    {
        ui_.show_error("Vault not unlocked.");
        return;
    }

    if(vault_->entries().size() < 1)
    {
        ui_.show_message("No entries");
        return;
    }

    ui_.list_entries(vault_->entries());
}

void Application::handle_save_and_close()
{
    if (!vault_)
    {
        ui_.show_error("Vault not unlocked.");
        return;
    }

    auto password = ui_.prompt_master_password();
    auto result = vault::VaultFile::save(vault_path_, *vault_, std::move(password.value()));
    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    vault_.reset();
    ui_.show_message("Vault Saved and Closed.");
}

void Application::handle_quit()
{
    ui_.show_message("Quitting");

    vault_.reset();
    running_ = false;
}


}

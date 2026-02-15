#include "app/Application.h"
#include "crypto/CryptoContext.h"
#include "vault/VaultFile.h"
#include "ui/TerminalUI.h"

namespace app
{

void Application::run()
{
    crypto::CryptoContext::init();

    while (running_)
    {
        Action action = ui_.prompt_action();

        if (!current_state_->allows(action))
        {
            ui_.show_error("Action not allowed in current state");
            continue;
        }

        handle_action(action);
        transition_state(action);
    }
}

void Application::handle_create_vault()
{
    auto password = ui_.prompt_master_password();

    auto result = vault::VaultFile::create_new(
        vault_path_,
        password
    );

    if (!result)
    {
        ui_.show_error(result.error().message());
        return;
    }

    ui_.show_message("Vault created successfully.");
}

void Application::handle_unlock()
{
    auto password = ui_.prompt_master_password();

    auto loaded = vault::VaultFile::load(vault_path_, password);
    
    if (!loaded)
    {
        ui_.show_error(loaded.error().message());
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

    auto entry = ui_.prompt_entry();

    auto result = vault_->add_entry(std::move(entry));

    if (!result)
    {
        ui_.show_error(result.error());
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

    auto updated = ui_.prompt_entry();

    auto result = vault_->update_entry(*index, std::move(updated));
    if (!result)
    {
        ui_.show_error(result.error());
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

    auto result = vault_->remove_entry(index);
    if (!result)
    {
        ui_.show_error(result.error());
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
    auto result = vault::VaultFile::save(vault_path_, vault_, password);
    if (!result)
    {
        ui_.show_error(result.error());
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

void transition_state(Action action)
{
    auto new_state = current_state_->transition(action);
    if (!new_state)
    {
        return;
    }
}

}

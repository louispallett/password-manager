#include "app/Application.h"
#include "app/BootstrapState.h"
#include "crypto/CryptoContext.h"
#include "crypto/CryptoTypes.h"
#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/Entry.h"
#include "vault/VaultError.h"
#include "vault/VaultFile.h"
#include "ui/TerminalUI.h"
#include "app/State.h"
#include "app/Action.h"
#include "vault/VaultFileError.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <memory>
#include <random>

namespace app
{

Application::Application(std::string vault_path)
    : vault_path_(std::move(vault_path))
{}

void Application::run(Application& app)
{
    crypto::CryptoContext::init();
    ui_.initialize();
    ui_.display_logo();
    
    current_state_ = std::make_unique<BootstrapState>();
    if (auto next = current_state_->on_enter(*this))
    {
        ui_.show_message("Existing vault found");
        current_state_ = std::move(next);
    }
    else 
    {
        ui_.show_message("No vault found. Please create a vault to continue");
    }

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
        case Action::Save:
            handle_save_only();
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

    ui_.display_loading();
    auto result = vault::VaultFile::create_new(
        vault_path_,
        std::move(password.value())
    );
    ui_.wipe_loading();

    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    ui_.show_message("Vault created successfully");
}

void Application::handle_unlock()
{
    auto password = ui_.prompt_master_password();
    ui_.display_loading();
    auto loaded = vault::VaultFile::load(vault_path_, std::move(password.value()));
    ui_.wipe_loading();
    if (!loaded)
    {
        ui_.show_error(vault::to_string(loaded.error()));
        return;
    }

    session_.emplace(std::move(loaded.value()));
    ui_.show_message("Vault Unlocked");
}

char generate_random_char(const std::string& char_set) 
{
    crypto::ByteBuffer buffer(1);

    crypto::CryptoContext::random_bytes(buffer);
    unsigned char random_byte = buffer[0];
    int random_index = random_byte % char_set.size();
    
    return char_set[random_index];
}

std::string generate_password_string() 
{
    const std::string lowercase = "abcdefghijklmnopqrstuvwxyz";
    const std::string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string digits = "0123456789";
    const std::string specials = "!@#$%&-_+;:,.?";
    const std::string all_chars = lowercase + uppercase + digits + specials;
    const size_t NUM_OF_CHARS = 32;
    std::string password(NUM_OF_CHARS, '\0');

    password[0] = generate_random_char(lowercase);
    password[1] = generate_random_char(uppercase);
    password[2] = generate_random_char(digits);
    password[3] = generate_random_char(specials);

    for (size_t i = 4; i < NUM_OF_CHARS; ++i) 
    {
        password[i] = generate_random_char(all_chars);
    }

    std::shuffle(password.begin(), password.end(), std::mt19937{static_cast<std::uint32_t>(std::chrono::system_clock::now().time_since_epoch().count())});

    return password;
}

util::Expected<util::SecureString, std::string> handle_generate_password()
{
    try 
    {
        std::string password = generate_password_string();
        return util::SecureString(password.c_str());
    } 
    catch (const std::exception& e) 
    {
        return std::string("Error generating password: ") + e.what();
    }
}

void Application::handle_add_entry()
{
    if (!session_)
    {
        ui_.show_error("Vault not unlocked");
        return;
    }

    auto name = ui_.prompt_input("Name");
    auto username = ui_.prompt_input("Username/Email");
    bool generate_password = ui_.generate_password();
    auto password = generate_password ? handle_generate_password() : ui_.prompt_input("Password");
    if (!name || !username || !password)
    {
        return;
    }

    vault::Entry new_entry {
        std::move(name.value()), 
        std::move(username.value()), 
        std::move(password.value())
    };

    auto result = session_->add_entry(std::move(new_entry));
    if (!result)
    {
        ui_.show_error(vault::to_string(static_cast<vault::VaultError>(result.error())));
        return;
    }

    ui_.show_message("Entry added successfully");
}

void Application::handle_alter_entry()
{
    // if (!vault_)
    // {
    //     ui_.show_error("Vault not unlocked");
    //     return;
    // }
    //
    // auto index = ui_.select_entry(vault_->entries());
    // if (!index)
    // {
    //     return;
    // }
    //
    // auto name = ui_.prompt_input("Name");
    // auto username = ui_.prompt_input("Username");
    // auto password = ui_.prompt_input("Password");
    // if (!name || !username || !password)
    // {
    //     return;
    // }
    //
    // vault::Entry new_entry {
    //     std::move(name.value()), 
    //     std::move(username.value()), 
    //     std::move(password.value())
    // };
    //
    // auto result = vault_->update_entry(*index, std::move(new_entry));
    // if (!result)
    // {
    //     ui_.show_error(vault::to_string(result.error()));
    //     return;
    // }
    //
    // ui_.show_message("Entry updated successfully");
}

void Application::handle_remove_entry()
{
    if (!session_)
    {
        ui_.show_error("Vault not unlocked");
        return;
    }

    auto index = ui_.remove_entry(session_->entries());
    if (!index)
    {
        return;
    }

    auto result = session_->remove_entry(index.value());
    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    ui_.show_message("Entry successfully deleted");
}

void Application::handle_list_entries()
{
    if (!session_)
    {
        ui_.show_error("Vault not unlocked");
        return;
    }

    if(session_->entries().size() < 1)
    {
        ui_.show_message("No entries");
        return;
    }

    ui_.list_entries(session_->entries());
}

void Application::handle_save_only()
{
    if (!session_)
    {
        ui_.show_error("Vault not unlocked");
        return;
    }

    ui_.display_loading();
    auto result = session_->save();
    ui_.wipe_loading();
    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    ui_.show_message("Vault Saved");
}

void Application::handle_save_and_close()
{
    if (!session_)
    {
        ui_.show_error("Vault not unlocked");
        return;
    }

    ui_.display_loading();
    auto result = session_->save();
    ui_.wipe_loading();
    if (!result)
    {
        ui_.show_error(vault::to_string(result.error()));
        return;
    }

    session_.reset();
    ui_.show_message("Vault Saved and Closed");
}

void Application::handle_quit()
{
    ui_.show_message("Quitting");

    session_.reset();
    running_ = false;
}

}

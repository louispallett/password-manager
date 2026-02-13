#include "app/Application.h"
#include "app/State.h"
#include <doctest/doctest.h>
#include <memory>
#include "app/BootstrapState.h"
#include "app/LockedState.h"
#include "app/UnlockedState.h"
#include "app/ShutdownState.h"

template<typename T>
bool is_state (const std::unique_ptr<app::State>& ptr)
{
    return dynamic_cast<T*>(ptr.get()) != nullptr;
}

// --- BootstrapState ---
// NOTE: This requires Application.h
// TEST_CASE("Transitions to Locked when Vault exists")
// {
//     // We'll also need to create a vault first.
//     app::Application app;
//     app.create_vault();

//     app::BootstrapState state;

//     // In this test, we'll need to define Application, because we will have to run on_enter.
//     state.on_enter(app);

//     CHECK(is_state<app::LockedState>(app.current_state()));
// }

TEST_CASE("Transitions to Locked when Vault is Created")
{
    app::BootstrapState state;
    REQUIRE(state.allows(app::Action::CreateVault));

    std::unique_ptr<app::State> result = state.transition(app::Action::CreateVault);
    CHECK(is_state<app::LockedState>(result));
}


// --- LockedState ---
TEST_CASE("Rejects CRUD Actions")
{
    app::LockedState state;

    REQUIRE_FALSE(state.allows(app::Action::ListEntries));
    REQUIRE_FALSE(state.allows(app::Action::AddEntry));
    REQUIRE_FALSE(state.allows(app::Action::AlterEntry));
    CHECK_FALSE(state.allows(app::Action::RemoveEntry));
}

TEST_CASE("Unlock Success Transitions to Unlocked")
{
    app::LockedState state;

    std::unique_ptr<app::State> result = state.transition(app::Action::Unlock);
    REQUIRE(result);
    CHECK(is_state<app::UnlockedState>(result));
};

// --- UnlockedState ---
TEST_CASE("Allows CRUD Actions")
{
    app::UnlockedState state;

    REQUIRE(state.allows(app::Action::ListEntries));
    REQUIRE(state.allows(app::Action::AddEntry));
    REQUIRE(state.allows(app::Action::AlterEntry));
    CHECK(state.allows(app::Action::RemoveEntry));
};

TEST_CASE("AddEntry Does Not Transition")
{
    app::UnlockedState state;

    std::unique_ptr<app::State> result = state.transition(app::Action::AddEntry);
    CHECK_FALSE(is_state<app::LockedState>(result));
}

TEST_CASE("SaveAndClose Transitions to Locked")
{
    app::UnlockedState state;

    std::unique_ptr<app::State> result = state.transition(app::Action::SaveAndClose);

    REQUIRE(result); // unique_ptr converts to boolean
    CHECK(is_state<app::LockedState>(result));
};

// --- ShutdownState ---
TEST_CASE("Rejects all Actions")
{
    app::ShutdownState state;

    REQUIRE_FALSE(state.allows(app::Action::CreateVault));
    REQUIRE_FALSE(state.allows(app::Action::Unlock));
    REQUIRE_FALSE(state.allows(app::Action::ListEntries));
    REQUIRE_FALSE(state.allows(app::Action::AddEntry));
    REQUIRE_FALSE(state.allows(app::Action::AlterEntry));
    REQUIRE_FALSE(state.allows(app::Action::RemoveEntry));
    REQUIRE_FALSE(state.allows(app::Action::SaveAndClose));
    CHECK_FALSE(state.allows(app::Action::Quit));
};

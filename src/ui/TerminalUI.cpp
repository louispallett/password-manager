#include "ui/TerminalUI.h"
#include "app/Action.h"
#include "app/State.h"
#include "util/Expected.h"
#include "util/SecureString.h"

#include <ncurses.h>
#include <vector>

namespace ui
{

void TerminalUI::initialize()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE); 
    curs_set(0);

    if (has_colors())
    {
        start_color();
        use_default_colors(); 
        init_pair(1, COLOR_GREEN, -1); 
        init_pair(2, COLOR_WHITE, COLOR_RED);
    }

    refresh(); 
}

void TerminalUI::display_logo()
{
    // Your ASCII art lines
    const std::vector<std::string> logo = {
        "██████╗  █████╗ ███████╗███████╗",
        "██╔══██╗██╔══██╗██╔════╝██╔════╝",
        "██████╔╝███████║███████╗███████╗",
        "██╔═══╝ ██╔══██║╚════██║╚════██║",
        "██║     ██║  ██║███████║███████║",
        "╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝",
    };

    for (int i = 0; i < static_cast<int>(logo.size()); ++i)
        mvprintw(i, 0, "%s", logo[i].c_str());

    refresh();

    // Store this so callers know where the content area begins
    m_content_start_row_ = static_cast<int>(logo.size());
}

void TerminalUI::show_message (const std::string& message)
{
    const int win_height = message_content_height_;
    const int win_width = COLS;

    WINDOW* message_win = newwin(win_height, win_width, m_content_start_row_ + message_content_height_, 0);
    if (!message_win) return;

    mvwprintw(message_win, 1, 1, "%s", message.c_str());

    wgetch(message_win);                     

    werase(message_win);
    wrefresh(message_win);
    delwin(message_win);
}

void TerminalUI::show_error(const std::string& error)
{
    const int win_height = message_content_height_;
    const int win_width  = COLS;

    WINDOW* err_win = newwin(win_height, win_width, m_content_start_row_, 0);
    if (!err_win) return;

    wbkgd(err_win, COLOR_PAIR(2));       
    wattron(err_win, COLOR_PAIR(2));     

    box(err_win, 0, 0);

    mvwprintw(err_win, 2, 1, "%s", error.c_str());

    wattroff(err_win, COLOR_PAIR(1));
    wrefresh(err_win);

    wgetch(err_win);                     

    werase(err_win);
    wrefresh(err_win);
    delwin(err_win);
}

app::Action TerminalUI::prompt_action(
    const std::vector<app::MenuOption>& options)
{
    if (options.empty())
        return app::Action::None;

    const int content_start =
        m_content_start_row_ + (message_content_height_ * 2);

    const int win_height = LINES - content_start;
    const int win_width  = COLS;

    WINDOW* menu_win = newwin(win_height, win_width, content_start, 0);
    if (!menu_win)
        return app::Action::None;

    keypad(menu_win, TRUE);

    int selected = 0;

    auto render = [&]()
    {
        werase(menu_win);
        box(menu_win, 0, 0);

        for (size_t i = 0; i < options.size(); ++i)
        {
            if (static_cast<int>(i) == selected)
                wattron(menu_win, A_REVERSE);

            mvwprintw(menu_win,
                      1 + static_cast<int>(i),
                      2,
                      "%s",
                      options[i].label.c_str());

            if (static_cast<int>(i) == selected)
                wattroff(menu_win, A_REVERSE);
        }

        wrefresh(menu_win);
    };

    render();

    while (true)
    {
        int ch = wgetch(menu_win);

        if (ch == KEY_UP && selected > 0)
        {
            --selected;
        }
        else if (ch == KEY_DOWN &&
                 selected < static_cast<int>(options.size()) - 1)
        {
            ++selected;
        }
        else if (ch == '\n' || ch == KEY_ENTER)
        {
            app::Action chosen = options[selected].action;
            delwin(menu_win);
            return chosen;
        }

        render();
    }
}

void TerminalUI::list_entries(const std::vector<vault::Entry>& entries)
{
    if (entries.empty()) return;

    const int num_entries    = static_cast<int>(entries.size());
    const int content_start  = m_content_start_row_ + (message_content_height_ * 2);
    const int viewport_top   = content_start;
    const int viewport_left  = 0;
    const int viewport_bottom = LINES - 1;
    const int viewport_right  = COLS - 1;
    const int viewport_height = viewport_bottom - viewport_top + 1;

    // Pad is as tall as the full entry list, full terminal width
    WINDOW* pad = newpad(num_entries, COLS);
    if (!pad) 
    {
        return;
    }

    keypad(pad, TRUE); // enable arrow keys on the pad

    int selected   = 0; // currently highlighted entry
    int pad_scroll = 0; // topmost visible row in the pad

    // Helper lambda to render all entries onto the pad
    auto render = [&]()
    {
        for (int i = 0; i < num_entries; ++i)
        {
            const auto& name = entries[i].name;
            const char* name_str = reinterpret_cast<const char*>(name.data());
            const int   name_len = static_cast<int>(name.size());

            if (i == selected)
            {
                wattron(pad, A_REVERSE); // highlight selected row
            }

            mvwhline(pad, i, 0, ' ', COLS);  // blank the row first
            mvwprintw(pad, i, 1, "%.*s", name_len, name_str);

            if (i == selected)
            {
                wattroff(pad, A_REVERSE);
            }
        }

        // Blit the visible portion of the pad to the screen
        prefresh(pad, pad_scroll, 0, viewport_top, viewport_left, viewport_bottom, viewport_right);
    };

    render(); // initial draw

    while (true)
    {
        int ch = wgetch(pad);

        if (ch == KEY_UP && selected > 0)
        {
            --selected;
            // Scroll up if selected has gone above the viewport
            if (selected < pad_scroll)
                --pad_scroll;
        }
        else if (ch == KEY_DOWN && selected < num_entries - 1)
        {
            ++selected;
            // Scroll down if selected has gone below the viewport
            if (selected >= pad_scroll + viewport_height)
                ++pad_scroll;
        }

        render();
    }

    delwin(pad);
}

util::Expected<util::SecureString, std::string> TerminalUI::prompt_master_password ()
{
    const int win_height = message_content_height_;
    const int win_width = COLS;
    const int content_start  = m_content_start_row_ + (message_content_height_ * 2);

    WINDOW* password_input_win = newwin(win_height, win_width, content_start, 0);
    if (!password_input_win)
    {
        return std::string("");
    }
    
    keypad(password_input_win, TRUE);
    box(password_input_win, 0, 0);
    mvwprintw(password_input_win, 0, 1, "%s", "Enter Master Password:");
    wrefresh(password_input_win);

    std::vector<char> buf;
    buf.reserve(64);

    const int input_col_start = 1;
    int ch;

    while ((ch = wgetch(password_input_win)) != '\n' && ch != KEY_ENTER)
    {
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') && !buf.empty())
        {
            buf.pop_back();
            // Erase the last asterisk from the screen
            const int cursor_x = input_col_start + static_cast<int>(buf.size());
            mvwaddch(password_input_win, 1, cursor_x, ' ');
        }
        else if (ch >= 32 && ch < 127) // printable ASCII only
        {
            buf.push_back(static_cast<char>(ch));
            // Print asterisk at current cursor position
            const int cursor_x = input_col_start + static_cast<int>(buf.size()) - 1;
            mvwaddch(password_input_win, 1, cursor_x, '*');
        }

        wrefresh(password_input_win);
    }

    // Construct SecureString before wiping the buffer
    util::SecureString result(std::string_view(buf.data(), buf.size()));
    buf.clear();
    buf.shrink_to_fit();
    // Wipe the intermediate buffer
    sodium_memzero(buf.data(), buf.size());

    werase(password_input_win);
    wrefresh(password_input_win);
    delwin(password_input_win);

    return result;
}

util::Expected<util::SecureString, std::string> TerminalUI::prompt_input (std::string prompt)
{
    const int win_height = message_content_height_;
    const int win_width = COLS;
    const int content_start  = m_content_start_row_ + (message_content_height_ * 2);

    WINDOW* prompt_input_win = newwin(win_height, win_width, content_start, 0);
    if (!prompt_input_win)
    {
        return std::string("");
    }
    
    keypad(prompt_input_win, TRUE);
    box(prompt_input_win, 0, 0);
    mvwprintw(prompt_input_win, 0, 1, "%s", prompt.c_str());
    wrefresh(prompt_input_win);

    std::vector<char> buf;
    buf.reserve(64);

    const int input_col_start = 1;
    int ch;

    while ((ch = wgetch(prompt_input_win)) != '\n' && ch != KEY_ENTER)
    {
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') && !buf.empty())
        {
            buf.pop_back();
            const int cursor_x = input_col_start + static_cast<int>(buf.size());
            mvwaddch(prompt_input_win, 1, cursor_x, ' ');
        }
        else if (ch >= 32 && ch < 127) // printable ASCII only
        {
            buf.push_back(static_cast<char>(ch));
            const int cursor_x = input_col_start + static_cast<int>(buf.size()) - 1;
            mvwaddch(prompt_input_win, 1, cursor_x, static_cast<char>(ch));
        }

        wrefresh(prompt_input_win);
    }

    // Construct SecureString before wiping the buffer
    util::SecureString result(std::string_view(buf.data(), buf.size()));

    // Wipe the intermediate buffer
    sodium_memzero(buf.data(), buf.capacity());

    werase(prompt_input_win);
    wrefresh(prompt_input_win);
    delwin(prompt_input_win);

    return result;
}

// Ignore for now
std::optional<size_t> TerminalUI::select_entry (const std::vector<vault::Entry>& entries)
{

}

void TerminalUI::shutdown()
{
    if (!isendwin())
    {
        nocbreak();
        echo();
        curs_set(1);
        keypad(stdscr, FALSE);

        endwin(); // Always the last NCurses call
    }
}

}

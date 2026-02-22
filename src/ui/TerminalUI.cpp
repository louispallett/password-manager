#include "ui/TerminalUI.h"
#include "app/Action.h"
#include "app/State.h"
#include "util/Expected.h"
#include "util/SecureString.h"

#include <chrono>
#include <cstddef>
#include <ncurses.h>
#include <thread>
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
        init_pair(3, COLOR_WHITE, COLOR_BLUE);
    }

    refresh(); 
}

void TerminalUI::display_logo()
{
    const std::vector<std::string> logo = {
        " _________  _______   ________  _____ ______   ___  ________   ________  ___       ___           ___    ___ ", 
        "|\\___   ___\\\\  ___ \\ |\\   __  \\|\\   _ \\  _   \\|\\  \\|\\   ___  \\|\\   __  \\|\\  \\     |\\  \\         |\\  \\  /  /|",
        "\\|___ \\  \\_\\ \\   __/|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\  \\ \\  \\\\ \\  \\ \\  \\|\\  \\ \\  \\    \\ \\  \\        \\ \\  \\/  / /",
        "     \\ \\  \\ \\ \\  \\_|/_\\ \\   _  _\\ \\  \\\\|__| \\  \\ \\  \\ \\  \\\\ \\  \\ \\   __  \\ \\  \\    \\ \\  \\        \\ \\    / / ",
        "      \\ \\  \\ \\ \\  \\_|\\ \\ \\  \\\\  \\\\ \\  \\    \\ \\  \\ \\  \\ \\  \\\\ \\  \\ \\  \\ \\  \\ \\  \\____\\ \\  \\____    \\/  /  /  ",
        "       \\ \\__\\ \\ \\_______\\ \\__\\\\ _\\\\ \\__\\    \\ \\__\\ \\__\\ \\__\\\\ \\__\\ \\__\\ \\__\\ \\_______\\ \\_______\\__/  / /    ",
        "        \\|__|  \\|_______|\\|__|\\|__|\\|__|     \\|__|\\|__|\\|__| \\|__|\\|__|\\|__|\\|_______|\\|_______|\\___/ /     ",
        "                                                                                               \\|___|/      ",
        "              ___         ___       ________  ________  ___  __    _______   ________                       ",
        "             |\\  \\       |\\  \\     |\\   __  \\|\\   ____\\|\\  \\|\\  \\ |\\  ___ \\ |\\   ___ \\                      ",
        " ____________\\ \\  \\      \\ \\  \\    \\ \\  \\|\\  \\ \\  \\___|\\ \\  \\/  /|\\ \\   __/|\\ \\  \\_|\\ \\                     ",
        "|\\____________\\ \\  \\      \\ \\  \\    \\ \\  \\\\\\  \\ \\  \\    \\ \\   ___  \\ \\  \\_|/_\\ \\  \\ \\\\ \\                    ",
        "\\|____________|\\/  /|      \\ \\  \\____\\ \\  \\\\\\  \\ \\  \\____\\ \\  \\\\ \\  \\ \\  \\_|\\ \\ \\  \\_\\\\ \\                   ",
        "               /  //        \\ \\_______\\ \\_______\\ \\_______\\ \\__\\\\ \\__\\ \\_______\\ \\_______\\                  ",
        "              /_ //          \\|_______|\\|_______|\\|_______|\\|__| \\|__|\\|_______|\\|_______|                  ",
        "             |__|/                                                                                          ",
    };

    attron(A_BOLD);
    int i = 0;
    while (i < static_cast<int>(logo.size()))
    {
        mvprintw(i, 0, "%s", logo[i].c_str());
        ++i;
    }

    mvprintw(i + 2, 1, "%s", "Terminally -> Locked is licensed under the Apache-2.0 license.");
    mvprintw(i + 3, 1, "%s", "If you haven't yet created a vault, you will first have to create one. Otherwise, you can unlock your existing vault.");
    mvprintw(i + 4, 1, "%s", "For more information on the technology used, please visit www.github.com/louispallett/password-manager.");
    attroff(A_BOLD);

    refresh();

    m_content_start_row_ = static_cast<int>(logo.size() + 5);
}

void TerminalUI::show_message (const std::string& message)
{
    const int win_height = message_content_height_;
    const int win_width = COLS;

    WINDOW* message_win = newwin(win_height, win_width, m_content_start_row_ + message_content_height_, 0);
    if (!message_win) 
    {
        return;
    }

    wbkgd(message_win, COLOR_PAIR(3));
    wattron(message_win, COLOR_PAIR(3));     

    box(message_win, 0, 0);

    mvwprintw(message_win, 1, 1, "%s.", message.c_str());

    wrefresh(message_win);
    delwin(message_win);
}

void TerminalUI::show_error(const std::string& error)
{
    const int win_height = message_content_height_;
    const int win_width  = COLS;

    WINDOW* save_win = newwin(win_height, win_width, m_content_start_row_, 0);
    if (!save_win) return;
    wrefresh(save_win);

    WINDOW* err_win = newwin(win_height, win_width, m_content_start_row_, 0);
    if (!err_win) 
    {
        delwin(save_win);
        return;
    }

    wbkgd(err_win, COLOR_PAIR(2));
    wattron(err_win, COLOR_PAIR(2) | A_BOLD);     

    box(err_win, 0, 0);

    mvwprintw(err_win, 1, 1, "Error: %s. Press ANY key to continue.", error.c_str());
    wattroff(err_win, COLOR_PAIR(2) | A_BOLD);

    wrefresh(err_win);

    wgetch(err_win);

    werase(err_win);
    wrefresh(err_win);
    delwin(err_win);

    touchwin(save_win);
    wrefresh(save_win);
    delwin(save_win);
}

app::Action TerminalUI::prompt_action(
    const std::vector<app::MenuOption>& options)
{
    if (options.empty())
        return app::Action::None;

    const int content_start = m_content_start_row_ + (message_content_height_ * 2);

    const int win_height = options.size() + 5;
    const int win_width  = COLS / 3;

    WINDOW* menu_win = newwin(win_height, win_width, content_start, 0);
    if (!menu_win)
    {
        return app::Action::None;
    }


    keypad(menu_win, TRUE);

    int selected = 0;

    auto render = [&]()
    {
        werase(menu_win);
        box(menu_win, 0, 0);
        mvwprintw(menu_win, 0, 2, "%s", "Menu"); 

        for (size_t i = 0; i < options.size(); ++i)
        {
            if (static_cast<int>(i) == selected)
            {
                wattron(menu_win, A_REVERSE);
                
                mvwhline(
                    menu_win,
                    1 + static_cast<int>(i),
                    1,
                    ' ',
                    win_width - 2
                );

                mvwprintw(
                    menu_win,
                    1 + static_cast<int>(i),
                    2,
                    "%s",
                    options[i].label.c_str()
                );

                wattroff(menu_win, A_REVERSE);
            }
            else
            {
                mvwprintw(
                    menu_win,
                    1 + static_cast<int>(i),
                    1,
                    "%s",
                    options[i].label.c_str()
                );
            }
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

void TerminalUI::display_entry(const vault::Entry& entry)
{
    const int win_height = message_content_height_;
    const int win_width = COLS / 3;
    const int content_start = m_content_start_row_ + (message_content_height_ * 2);

    WINDOW* entry_name = newwin(
        win_height, 
        win_width, 
        content_start, 
        win_width * 2
    );
    WINDOW* entry_username = newwin(
        win_height, 
        win_width, 
        content_start + win_height, 
        win_width * 2
    );
    WINDOW* entry_secret = newwin(
        win_height, 
        win_width, 
        content_start + win_height * 2, 
        win_width * 2
    );
    WINDOW* menu = newwin(
        win_height + 1,
        win_width, 
        content_start + win_height * 3, 
        win_width * 2
    );
    if (!entry_name 
        || !entry_username 
        || !entry_secret
        || !menu
    )
    {
        return;
    }

    box(entry_name, 0, 0);
    mvwprintw(entry_name, 0, 1, "%s", "Name");
    wattron(entry_name, A_BOLD | A_BLINK);
    mvwprintw(entry_name, 1, 1, "%s", entry.name.c_str());
    wattroff(entry_name, A_BOLD | A_BLINK);

    box(entry_username, 0, 0);
    mvwprintw(entry_username, 0, 1, "%s", "Username");
    wattron(entry_username, A_BOLD);
    mvwprintw(entry_username, 1, 1, "%s", entry.username.c_str());
    wattroff(entry_username, A_BOLD);
    
    box(entry_secret, 0, 0);
    mvwprintw(entry_secret, 0, 1, "%s", "Password");
    wattron(entry_secret, A_BOLD);
    mvwprintw(entry_secret, 1, 1, "%s", entry.secret.c_str());
    wattroff(entry_secret, A_BOLD);

    keypad(menu, true);
    std::vector<std::string> options = { "REMOVE ENTRY", "COPY PASSWORD TO CLIPBOARD", "BACK" };
    int selected = options.size() - 1;

    auto render = [&]()
    {
      werase(menu);
      for (size_t i = 0; i < options.size(); ++i)
      {
            if (static_cast<int>(i) == selected)
            {
                wattron(menu, A_REVERSE);
                
                mvwhline(
                    menu,
                    1 + static_cast<int>(i),
                    1,
                    ' ',
                    win_width - 2
                );
                mvwprintw(
                    menu,
                    1 + static_cast<int>(i),
                    2,
                    "%s",
                    options[i].data()
                );

                wattroff(menu, A_REVERSE);
            }
            else
            {
                mvwprintw(
                    menu,
                    1 + static_cast<int>(i),
                    1,
                    "%s",
                    options[i].data()
                );
            }
        }
        wrefresh(menu);
    };

    wrefresh(entry_username);
    wrefresh(entry_secret);
    wrefresh(entry_name);

    render();

    while (true)
    {
        int ch = wgetch(menu);

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
            if (selected == 0) 
            {
              // remove_entry(selected); // FIXME: selected is the menu index, not the entry index!
            }
            else if (selected == 1)
            {
                auto copy_to_clipboard = [](const std::string& text) -> bool
                {
                    const char* cmds[] = {
                        "xclip -selection clipboard 2>/dev/null",
                        "wl-copy 2>/dev/null",
                        "pbcopy 2>/dev/null",
                        nullptr
                    };
                    for (int i = 0; cmds[i]; ++i)
                    {
                        FILE* pipe = popen(cmds[i], "w");
                        if (pipe)
                        {
                            fputs(text.c_str(), pipe);
                            int ret = pclose(pipe);
                            if (ret == 0) return true;
                        }
                    }
                    return false;
                };
            
                if (copy_to_clipboard(entry.secret.c_str()))
                {
                    std::thread([secret = entry.secret.c_str()]() {
                        std::this_thread::sleep_for(std::chrono::seconds(30));
                        const char* cmds[] = {
                            "xclip -selection clipboard 2>/dev/null",
                            "wl-copy 2>/dev/null",
                            "pbcopy 2>/dev/null",
                            nullptr
                        };
                        for (int i = 0; cmds[i]; ++i)
                        {
                            FILE* pipe = popen(cmds[i], "w");
                            if (pipe)
                            {
                                fputs("", pipe);
                                if (pclose(pipe) == 0) break;
                            }
                        }
                    }).detach();
                }
            } 
            else 
            {
                break;
            }
        }

        render();
    }

    werase(entry_name);
    werase(entry_username);
    werase(entry_secret);
    werase(menu);

    wrefresh(entry_username);
    wrefresh(entry_secret);
    wrefresh(entry_name);
    wrefresh(menu);

    delwin(entry_name);
    delwin(entry_username);
    delwin(entry_secret);
    delwin(menu);
}

void TerminalUI::remove_entry(const size_t index) 
{
  const int win_width = COLS / 3;
  const int content_start = m_content_start_row_ + (message_content_height_ * 2);
  
  WINDOW* remove_message = newwin(
      1, 
      win_width, 
      content_start + (message_content_height_ * 4), 
      win_width * 2 
  );
  WINDOW* menu = newwin(
      1,
      win_width,
      content_start + (message_content_height_ * 4) + 1,
      win_width * 2 
  );

  keypad(menu, true);
  int selected = 0;
  std::vector<std::string> options = { "REMOVE ENTRY", "BACK" };

  auto render = [&]()
  {
    werase(menu);
    for (size_t i = 0; i < options.size(); ++i)
    {
          if (static_cast<int>(i) == selected)
          {
              wattron(menu, A_REVERSE);
              
              mvwhline(
                  menu,
                  1 + static_cast<int>(i),
                  1,
                  ' ',
                  win_width - 2
              );

              mvwprintw(
                  menu,
                  1 + static_cast<int>(i),
                  1,
                  "%s",
                  options[i].data()
              );

              wattroff(menu, A_REVERSE);
          }
          else
          {
              mvwprintw(
                  menu,
                  1 + static_cast<int>(i),
                  1,
                  "%s",
                  options[i].data()
              );
          }
      }
      wrefresh(menu);
  };

  wrefresh(remove_message);
  render();

  while (true)
  {
      int ch = wgetch(menu);

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
        if (selected == 0) 
        {
          // handle delete

        }
          break;
      }

      render();
  }

  werase(remove_message);
  werase(menu);
  wrefresh(remove_message);
  wrefresh(menu);
  delwin(remove_message);
  delwin(menu);
}

void TerminalUI::list_entries(const std::vector<vault::Entry>& entries)
{
    if (entries.empty()) return;

    const int num_entries = static_cast<int>(entries.size()) + 1;
    const int content_start = m_content_start_row_ + (message_content_height_ * 2);
    const int viewport_top = content_start + 1;
    const int viewport_left = COLS / 3;
    const int viewport_bottom = LINES - 1;
    const int viewport_right = COLS - 1;
    const int viewport_height = viewport_bottom - viewport_top + 1;

    WINDOW* pad = newpad(num_entries, COLS / 3);
    if (!pad) 
    {
        return;
    }

    keypad(pad, TRUE); 

    int selected   = 0; 
    int pad_scroll = 0; 

    auto render = [&]()
    {
        werase(pad);
        for (int i = 0; i < num_entries; ++i)
        {
            if (i < entries.size())
            {
                const auto& name = entries[i].name;
                const char* name_str = reinterpret_cast<const char*>(name.data());
                const int   name_len = static_cast<int>(name.size());

                if (i == selected)
                {
                    wattron(pad, A_REVERSE); 
                    
                    mvwhline(pad, i, 0, ' ', COLS/3);
                    mvwprintw(pad, i, 2, "%s", entries[i].name.c_str());
                    
                    wattroff(pad, A_REVERSE);
                }
                else 
                {
                  mvwprintw(pad, i, 1, "%s", entries[i].name.c_str());
                }
            }
            else 
            {
                if (i == selected)
                {
                    wattron(pad, A_REVERSE);
                }

                mvwhline(pad, i, 0, ' ', COLS/3);
                mvwprintw(pad, i, COLS/6 - 2, "BACK");

                if (i == selected)
                {
                    wattroff(pad, A_REVERSE);
                }
            }
        }

        prefresh(pad, pad_scroll, 0, viewport_top, viewport_left, viewport_bottom, viewport_right);
    };

    render();

    while (true)
    {
        int ch = wgetch(pad);

        if (ch == KEY_UP && selected > 0)
        {
            --selected;
            if (selected < pad_scroll)
                --pad_scroll;
        }
        else if (ch == KEY_DOWN && selected < num_entries - 1)
        {
            ++selected;
            if (selected >= pad_scroll + viewport_height)
                ++pad_scroll;
        }
        else if (ch == '\n' || ch == KEY_ENTER)
        {
            if (selected == num_entries - 1)
            {
                werase(pad);
                wrefresh(pad);
                delwin(pad);
                return;
            }
            else
            {
                // List specific entry
                display_entry(entries[selected]);
            }
        }

        render();
    }

    delwin(pad);
}

util::Expected<util::SecureString, std::string> TerminalUI::prompt_master_password ()
{
    const int win_height = message_content_height_;
    const int win_width = COLS / 3;
    const int content_start  = m_content_start_row_ + (message_content_height_ * 2);

    WINDOW* password_input_win = newwin(win_height, win_width, content_start, win_width);
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
            const int cursor_x = input_col_start + static_cast<int>(buf.size());
            mvwaddch(password_input_win, 1, cursor_x, ' ');
        }
        else if (ch >= 32 && ch < 127) 
        {
            buf.push_back(static_cast<char>(ch));
            const int cursor_x = input_col_start + static_cast<int>(buf.size()) - 1;
            mvwaddch(password_input_win, 1, cursor_x, '*');
        }

        wrefresh(password_input_win);
    }

    util::SecureString result(std::string_view(buf.data(), buf.size()));
    buf.clear();
    buf.shrink_to_fit();
    sodium_memzero(buf.data(), buf.size());

    werase(password_input_win);
    wrefresh(password_input_win);
    delwin(password_input_win);

    return result;
}

util::Expected<util::SecureString, std::string> TerminalUI::prompt_input (std::string prompt)
{
    const int win_height = message_content_height_;
    const int win_width = COLS / 3;
    const int content_start  = m_content_start_row_ + (message_content_height_ * 2);

    WINDOW* prompt_input_win = newwin(win_height, win_width, content_start, win_width);
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
        else if (ch >= 32 && ch < 127) 
        {
            buf.push_back(static_cast<char>(ch));
            const int cursor_x = input_col_start + static_cast<int>(buf.size()) - 1;
            mvwaddch(prompt_input_win, 1, cursor_x, static_cast<char>(ch));
        }

        wrefresh(prompt_input_win);
    }

    util::SecureString result(std::string_view(buf.data(), buf.size()));

    sodium_memzero(buf.data(), buf.capacity());

    werase(prompt_input_win);
    wrefresh(prompt_input_win);
    delwin(prompt_input_win);

    return result;
}

bool TerminalUI::generate_password()
{
    const int win_height = message_content_height_;
    const int win_width = COLS / 3;
    const int content_start  = m_content_start_row_ + (message_content_height_ * 2);
    
    WINDOW* question = newwin(2, win_width, content_start, win_width);
    WINDOW* menu = newwin(2, win_width, content_start + 2, win_width);
    if (!question || !menu)
    {
      return true;
    }

    mvwprintw(question, 1, 1, "%s", "AUTO-GENERATE PASSWORD?"); 
    keypad(menu, true);
    int selected = 0;
    std::vector<std::string> options = { "YES", "NO (MANUAL ENTRY)" };
    
    auto render = [&]()
    {
        werase(menu);
        for (size_t i = 0; i < options.size(); ++i)
        {
            if (static_cast<int>(i) == selected)
            {
                wattron(menu, A_REVERSE);
                
                mvwhline(
                    menu,
                    static_cast<int>(i),
                    1,
                    ' ',
                    win_width - 2
                );

                mvwprintw(
                    menu,
                    static_cast<int>(i),
                    2,
                    "%s",
                    options[i].data()
                );

                wattroff(menu, A_REVERSE);
            }
            else
            {
                mvwprintw(
                    menu,
                    static_cast<int>(i),
                    1,
                    "%s",
                    options[i].data()
                );
            }
        }
        wrefresh(menu);
    };

    render();
    wrefresh(question);

    while (true)
    {
        int ch = wgetch(menu);

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
            werase(question);
            werase(menu);
            wrefresh(question);
            wrefresh(menu);
            delwin(question);
            delwin(menu);
            if (selected == 0) 
            {
                return true; 
            }
            return false;
        }
        render();
    }

  werase(question);
  werase(menu);
  wrefresh(question);
  wrefresh(menu);
  delwin(question);
  delwin(menu);
  return false;
}

void TerminalUI::shutdown()
{
    if (!isendwin())
    {
        nocbreak();
        echo();
        curs_set(1);
        keypad(stdscr, FALSE);

        endwin(); 
    }
}

}

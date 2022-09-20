#include "pch.h"
#include <cstdlib>
#include <string.h>
#include "console.h"

using namespace ark;
extern bool fullscreen_mode;
extern int window_width;
extern int window_height;

extern ui::UIConsole console;
bool show_console = true;

void
ui::init()
{
    
}

void
ui::tick(float dt)
{
    console.draw("Arkane console", &show_console);
}

int64_t 
ui::get_cmd_int(std::string_view str)
{
    if (str == "window_fullscreen") {
        return fullscreen_mode;
    }
    else if (str == "window_width") {
        return window_width;
    }
    else if (str == "window_height") {
        return window_height;
    }
}

void 
ui::push_console_string(std::string_view str)
{
    console.push_log_item(str);
}

void
ui::destroy()
{
   console.clear_log();
}

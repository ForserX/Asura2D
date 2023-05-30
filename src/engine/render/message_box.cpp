#include "pch.h"

using Asura::MessageBox::Buttons;
using Asura::MessageBox::Selection;
using Asura::MessageBox::Style;

#if defined(OS_MACOS) || defined(OS_LINUX) || defined(OS_BSD) || defined(OS_SOLARIS)
namespace Asura::MessageBox {
Selection Show(const char* message, const char* title, Style style, Buttons buttons)
{
#ifndef OS_MACOS
    std::string message_app = "xmessage";
#else
    std::string message_app = "osascript -e"; // TODO: Check this
#endif
    std::string command = fmt::format("{} \"{}\"", message_app, message);

    std::system(command.c_str());

    return Selection::OK;
}
}
#elif defined(OS_WINDOWS)

UINT getIcon(Style style)
{
    switch (style) {
    case Style::Info:
        return MB_ICONINFORMATION;
    case Style::Warning:
        return MB_ICONWARNING;
    case Style::Error:
        return MB_ICONERROR;
    case Style::Question:
        return MB_ICONQUESTION;
    default:
        return MB_ICONINFORMATION;
    }
}

UINT getButtons(Buttons buttons)
{
    switch (buttons) {
    case Buttons::OK:
        return MB_OK;
    case Buttons::OKCancel:
        return MB_OKCANCEL;
    case Buttons::YesNo:
        return MB_YESNO;
    default:
        return MB_OK;
    }
}

Selection getSelection(int response)
{
    switch (response) {
    case IDOK:
        return Selection::OK;
    case IDCANCEL:
        return Selection::Cancel;
    case IDYES:
        return Selection::Yes;
    case IDNO:
        return Selection::No;
    default:
        return Selection::None;
    }
}

namespace Asura::MessageBox {
Selection Show(const char* message, const char* title, Style style, Buttons buttons)
{
    UINT flags = MB_TASKMODAL;

    flags |= getIcon(style);
    flags |= getButtons(buttons);

    return getSelection(MessageBoxA(nullptr, message, title, flags));
}
}
#endif

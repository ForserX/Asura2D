#include "pch.h"

using Asura::MessageBox::Style;
using Asura::MessageBox::Buttons;
using Asura::MessageBox::Selection;

#if defined(OS_MACOS) || defined(OS_LINUX) || defined(OS_FREEBSD)
namespace Asura::MessageBox
{
    Selection Show(const char* message, const char* title, Style style, Buttons buttons) 
    {
#if 0
        NSAlert* alert = [[NSAlert alloc]init];

        [alert setMessageText : [NSString stringWithCString : title
            encoding : [NSString defaultCStringEncoding] ] ] ;
        [alert setInformativeText : [NSString stringWithCString : message
            encoding : [NSString defaultCStringEncoding] ] ] ;

        [alert setAlertStyle : getAlertStyle(style)] ;
        setButtons(alert, buttons);

        Selection selection = getSelection([alert runModal], buttons);
        [alert release] ;
#endif
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

namespace Asura::MessageBox
{
    Selection Show(const char* message, const char* title, Style style, Buttons buttons)
    {
        UINT flags = MB_TASKMODAL;

        flags |= getIcon(style);
        flags |= getButtons(buttons);

        return getSelection(MessageBoxA(nullptr, message, title, flags));
    }
}
#endif


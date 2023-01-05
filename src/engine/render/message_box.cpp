#include "pch.h"

using Asura::MessageBox::Style;
using Asura::MessageBox::Buttons;
using Asura::MessageBox::Selection;

#if defined(OS_LINUX) || defined(OS_FREEBSD)
#include <gtk/gtk.h>

GtkMessageType getMessageType(Style style) 
{
    switch (style) 
    {
    case Style::Info:
        return GTK_MESSAGE_INFO;
    case Style::Warning:
        return GTK_MESSAGE_WARNING;
    case Style::Error:
        return GTK_MESSAGE_ERROR;
    case Style::Question:
        return GTK_MESSAGE_QUESTION;
    default:
        return GTK_MESSAGE_INFO;
    }
}

GtkButtonsType getButtonsType(Buttons buttons) 
{
    switch (buttons) 
    {
    case Buttons::OK:
        return GTK_BUTTONS_OK;
    case Buttons::OKCancel:
        return GTK_BUTTONS_OK_CANCEL;
    case Buttons::YesNo:
        return GTK_BUTTONS_YES_NO;
    default:
        return GTK_BUTTONS_OK;
    }
}

Selection getSelection(gint response) 
{
    switch (response)
    {
    case GTK_RESPONSE_OK:
        return Selection::OK;
    case GTK_RESPONSE_CANCEL:
        return Selection::Cancel;
    case GTK_RESPONSE_YES:
        return Selection::Yes;
    case GTK_RESPONSE_NO:
        return Selection::No;
    default:
        return Selection::None;
    }
}

namespace Asura::MessageBox
{
    Selection Show(const char* message, const char* title, Style style, Buttons buttons) 
    {
        if (!gtk_init_check(0, NULL)) {
            return Selection::None;
        }

        GtkWidget* dialog = gtk_message_dialog_new(nullptr,
            GTK_DIALOG_MODAL,
            getMessageType(style),
            getButtonsType(buttons),
            "%s",
            message);
        gtk_window_set_title(GTK_WINDOW(dialog), title);
        Selection selection = getSelection(gtk_dialog_run(GTK_DIALOG(dialog)));

        gtk_widget_destroy(GTK_WIDGET(dialog));
        while (g_main_context_iteration(NULL, false));

        return selection;
    }
}
#elif defined(OS_MAC)
#include <cocoa/cocoa.h>

NSString* const OK_STR = @"OK";
NSString* const CANCEL_STR = @"Cancel";
NSString* const YES_STR = @"Yes";
NSString* const NO_STR = @"No";

NSAlertStyle getAlertStyle(Style style) 
{
    switch (style)
    {
    case Style::Info:
        return NSInformationalAlertStyle;
    case Style::Warning:
        return NSWarningAlertStyle;
    case Style::Error:
        return NSCriticalAlertStyle;
    case Style::Question:
        return NSWarningAlertStyle;
    default:
        return NSInformationalAlertStyle;
    }
}

void setButtons(NSAlert* alert, Buttons buttons) 
{
#if 0
    switch (buttons) {
    case Buttons::OK:
        [alert addButtonWithTitle : OK_STR] ;
        break;
    case Buttons::OKCancel:
        [alert addButtonWithTitle : OK_STR] ;
        [alert addButtonWithTitle : CANCEL_STR] ;
        break;
    case Buttons::YesNo:
        [alert addButtonWithTitle : YES_STR] ;
        [alert addButtonWithTitle : NO_STR] ;
        break;
    default:
        [alert addButtonWithTitle : OK_STR] ;
    }
#endif
}

Selection getSelection(int index, Buttons buttons)
{
    switch (buttons) 
    {
    case Buttons::OK:
    {
        return index == NSAlertFirstButtonReturn ? Selection::OK : Selection::None;
    }
    case Buttons::OKCancel:
    {
        if (index == NSAlertFirstButtonReturn)
        {
            return Selection::OK;
        }
        else if (index == NSAlertSecondButtonReturn)
        {
            return Selection::Cancel;
        }
        else
        {
            return Selection::None;
        }
    }
    case Buttons::YesNo:
    {
        if (index == NSAlertFirstButtonReturn)
        {
            return Selection::Yes;
        }
        else if (index == NSAlertSecondButtonReturn)
        {
            return Selection::No;
        }
        else
        {
            return Selection::None;
        }
    }
    default: return Selection::None;
    }
}

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
        return selection;
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


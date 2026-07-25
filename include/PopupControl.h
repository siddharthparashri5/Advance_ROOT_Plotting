#ifndef POPUPCONTROL_H
#define POPUPCONTROL_H

#include <TGMsgBox.h>
#include <TGClient.h>
#include <TGWindow.h>
#include <iostream>

// ============================================================================
// PopupControl — a global, app-wide toggle for whether informational popups
// (TGMsgBox dialogs) are shown to the user, or just logged to the console
// instead. Flipped by the "Show popup messages" checkbox in the main GUI, or
// programmatically via PopupControl::SetEnabled(false).
//
// ShowMsgBox(...) below is a drop-in replacement for `new TGMsgBox(...)` --
// identical argument order, so every call site just swaps "new TGMsgBox(" for
// "ShowMsgBox(". When popups are disabled:
//   - Purely informational dialogs (only kMBOk, i.e. no real decision
//     required) are skipped and printed to the console instead of shown
//     modally.
//   - Dialogs offering Yes/No ALWAYS show regardless of the toggle, since
//     auto-answering them could silently change what the app does next
//     (e.g. "Load this into the GUI too?").
// ============================================================================
namespace PopupControl {

    inline bool& EnabledRef() {
        static bool enabled = true;   // popups shown by default
        return enabled;
    }

    inline void SetEnabled(bool value) { EnabledRef() = value; }
    inline bool IsEnabled()            { return EnabledRef(); }

} // namespace PopupControl

// Templated on IconT so this works regardless of the exact enum type ROOT
// uses for TGMsgBox's icon parameter (EMsgBoxIcon in current ROOT) -- the
// value is simply forwarded through to the real TGMsgBox constructor.
template <typename IconT>
inline void ShowMsgBox(const TGWindow* p, const TGWindow* main,
                        const char* title, const char* msg,
                        IconT icon, Int_t buttons,
                        Int_t* ret_code = nullptr)
{
    // Only kMBYes/kMBNo are used anywhere in this codebase's actual dialogs
    // (e.g. "Load into GUI?"), so those are the only buttons treated as
    // "needs a real decision, always show". (Other EMsgBoxButton names like
    // kMBRetry/kMBIgnore/kMBAbort vary across ROOT versions -- kMBAbort
    // doesn't even exist in some -- so we don't rely on them here.)
    bool needsDecision = (buttons & (kMBYes | kMBNo)) != 0;

    if (!PopupControl::IsEnabled() && !needsDecision) {
        std::cout << "[" << title << "] " << msg << std::endl;
        if (ret_code) *ret_code = kMBOk;
        return;
    }

    new TGMsgBox(p, main, title, msg, icon, buttons, ret_code);
}

#endif // POPUPCONTROL_H

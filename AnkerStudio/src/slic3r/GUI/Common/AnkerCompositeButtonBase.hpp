#pragma once
#include <wx/button.h>
#include <wx/panel.h>

namespace Slic3r {
	namespace GUI {

		// A GTK wxButton has no client area and cannot host the child image/text
		// controls that AnkerCombinButton and AnkerSwitchButton create, so they derive
		// from a panel there. Both emit wxEVT_BUTTON themselves, so callers see no difference.
#ifdef __WXGTK__
		using AnkerCompositeButtonBase = wxPanel;
#define ANKER_COMPOSITE_BUTTON_INIT(parent, label, ...) AnkerCompositeButtonBase(parent, wxID_ANY)
#else
		using AnkerCompositeButtonBase = wxButton;
#define ANKER_COMPOSITE_BUTTON_INIT(parent, label, ...) AnkerCompositeButtonBase(parent, wxID_ANY, label, ##__VA_ARGS__)
#endif

	}
}

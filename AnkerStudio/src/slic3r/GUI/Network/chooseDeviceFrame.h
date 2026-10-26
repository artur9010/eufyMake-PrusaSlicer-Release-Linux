#ifndef ANKER_NETWORK_CHOOSE_DEVICE_FRAME_H
#define ANKER_NETWORK_CHOOSE_DEVICE_FRAME_H

#include <wx/dialog.h>
#include <wx/frame.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <vector>
#include <wx/sizer.h>
#include <wx/timer.h>
#include <boost/signals2.hpp>
#include <boost/bind/bind.hpp>
#include "../Common/AnkerLine.hpp"
#include "../Common/AnkerButton.hpp"
#include "../GUI_App.hpp"
#include "../AnkerBtn.hpp"
#include "AnkerNetDefines.h"

namespace AnkerNet
{
	DEF_PTR(DeviceObjectBase)
}
using namespace AnkerNet;

class AnkerChooseDeviceItem : public wxControl
{
public:
	AnkerChooseDeviceItem(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	~AnkerChooseDeviceItem();

	enum DeviceSelectMode {
		Device_Selected_None,
		Device_Selected,
	};
	enum ChooseDeviceStat {
		Available,
		Busy,
		Offline,
	};

	struct ChooseDeviceItemData {
		std::string m_deviceName = "";
		std::string m_statusText = "";
		std::string m_sn = "";
		wxImage m_deviceImage;
		wxImage m_selectedImage;
		ChooseDeviceStat m_deviceStatus = Offline;
		anker_device_type m_deviceType = DEVICE_V8111_TYPE;
		DeviceSelectMode m_selected = Device_Selected_None;
		wxFont m_font;
		int m_index = -1;
		bool m_gcodeMatch = true;

		ChooseDeviceItemData& operator=	(const ChooseDeviceItemData& data) {
			m_deviceName = data.m_deviceName;
			m_statusText = data.m_statusText;	
			m_sn = data.m_sn;
			m_deviceImage = data.m_deviceImage;
			m_selectedImage = data.m_selectedImage;
			m_deviceStatus = data.m_deviceStatus;
			m_deviceType = data.m_deviceType;
			m_selected = data.m_selected;
			m_font = data.m_font;
			m_index = data.m_index;
			m_gcodeMatch = data.m_gcodeMatch;
			return *this;
		}
	};

	void setData(const ChooseDeviceItemData& data);
	void setFont(const wxFont& font);
	void setDeviceTextState();
	void setSelected(DeviceSelectMode selected);
	void updateSelectedStatus(DeviceSelectMode selected);
	DeviceSelectMode getCurrentDeviceSelectMode() const;
	
	bool IsGcodeMatch() const;
	bool isAvailable() const;
	int getIndex() const;
	std::string getDeviceSn() const;

	boost::signals2::connection updateSelctedConnect(const boost::signals2::slot<void(int)>& slot);
	void updateSelectedSignal();

private:
	void initGUI();
	void OnPaint(wxPaintEvent& event);
	void OnClickItem(wxMouseEvent& event);

private:
	ChooseDeviceItemData m_data;
	boost::signals2::signal<void(int)> selectedSignal;
};


class AnkerChooseDevicePanel;

#ifdef __WXGTK__
// GTK's wxListBox is a GtkTreeView and cannot host the custom device rows that
// AnkerChooseDeviceListBox places inside it, so use a panel that keeps the rows'
// data itself and provides the few wxListBox calls the list box relies on.
class AnkerChooseDeviceListBoxBase : public wxPanel
{
public:
	AnkerChooseDeviceListBoxBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
		int /*n*/, const wxString* /*choices*/, long /*style*/)
		: wxPanel(parent, id, pos, size, wxNO_BORDER) {}

	int Append(const wxString& /*label*/, void* clientData)
	{
		m_clientData.push_back(clientData);
		return int(m_clientData.size()) - 1;
	}
	void* GetClientData(unsigned int n) const { return n < m_clientData.size() ? m_clientData[n] : nullptr; }
	unsigned int GetCount() const { return unsigned(m_clientData.size()); }
	void Delete(unsigned int n)
	{
		if (n < m_clientData.size())
			m_clientData.erase(m_clientData.begin() + n);
	}

private:
	std::vector<void*> m_clientData;
};
#else
using AnkerChooseDeviceListBoxBase = wxListBox;
#endif

class AnkerChooseDeviceListBox : public AnkerChooseDeviceListBoxBase
{
public:
	struct ListBoxItemData {
		wxString label;
		wxControl* control;
	};
	AnkerChooseDeviceListBox(wxWindow* parent, wxWindowID id, 
		const wxPoint& pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);
	void setChooseDevicePanel(AnkerChooseDevicePanel* window);

	void AddChooseDeviceItem(const AnkerChooseDeviceItem::ChooseDeviceItemData& data);
	void selectedChooseDeviceItem(int index = 0);

	std::string getSelcetedSn() const;
	
private:
	void AddControlItem(wxControl* control, const wxString& label);
	wxControl* GetControlItem(int index) const;
	void DeleteItemData(size_t index);
	void updateSelectedStatus(int index);

private:
	std::string m_selectedSn;
	AnkerChooseDevicePanel* m_chooseDevicePanel = nullptr;
};


class AnkerChooseDevicePanel : public wxPanel 
{
public:
	AnkerChooseDevicePanel(wxWindow* parent, const wxString& title = "");

	void initDeviceListGui();
	void initNoDeviceGui();
	std::string getSelectedDeviceSn() const;
	void UpdateGcodeHintInfo(bool gcodeMatch = true);
	void EnableOkBtn(bool enable = true);

private:	
	void AddGcodeHintPanel();
	void RemoveGcodeHintPanel();
	void OnOkBtnClicked(wxCommandEvent& event);

private:
	wxWindow* m_parent{ nullptr };
	wxStaticText* m_title = nullptr;
	wxButton* closeBtn = nullptr;
	wxStaticText* chooseDeviceContent = nullptr;
	AnkerChooseDeviceListBox* deviceListBox = nullptr;
	AnkerBtn* cancelBtn = nullptr;
	AnkerBtn* okBtn = nullptr;
	wxSizer* m_mainSizer = nullptr;
	// the index is the pos gocode hint sizer in mainsizer
	const int m_gcodeHintIndex = 3;	
	bool m_haveGcodeHint = false;
};

class AnkerLoadingMask;
class AnkerChooseDeviceDialog : public wxDialog
{
public:
	AnkerChooseDeviceDialog(wxWindow* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxBORDER_NONE,
		const wxString& name = wxASCII_STR(wxFrameNameStr));
	~AnkerChooseDeviceDialog();

	void UpdateGui();
	void SetLoadingVisible(bool visible);

private:
	void InitGui();
	void OnLoadMaskRectUpdate(wxCommandEvent& event);
	void OnLoadingTimeout(wxCommandEvent& event);

private:
	AnkerChooseDevicePanel* m_panel = nullptr;
	AnkerLoadingMask* m_pLoadingMask = nullptr;
	int scaled_height;
	wxString m_title;
	bool m_uiUpdated = false;
	bool m_isVisible;
};


#endif // !ANKER_NETWORK_CHOOSE_DEVICE_FRAME_H

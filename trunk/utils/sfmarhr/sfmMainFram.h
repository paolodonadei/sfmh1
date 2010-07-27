///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __sfmMainFram__
#define __sfmMainFram__

#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SFMMainfram
///////////////////////////////////////////////////////////////////////////////
class SFMMainfram : public wxFrame
{
	private:

	protected:
		wxStatusBar* m_statusBar1;
		wxMenuBar* m_menubar1;
		wxMenu* m_menu1;
		wxMenu* m_menu3;
		wxMenu* m_menu2;

		// Virtual event handlers, overide them in your derived class
		virtual void framCharPressed( wxKeyEvent& event ){ event.Skip(); }
		virtual void framKeyPressed( wxKeyEvent& event ){ event.Skip(); }
		virtual void framMouseClicked( wxMouseEvent& event ){ event.Skip(); }
		virtual void OpenPlyFile( wxCommandEvent& event ){ event.Skip(); }
		virtual void OpenCamCenter( wxCommandEvent& event ){ event.Skip(); }
		virtual void openOBJ( wxCommandEvent& event ){ event.Skip(); }
		virtual void ExitFunc( wxCommandEvent& event ){ event.Skip(); }
		virtual void toggleTriangles( wxCommandEvent& event ){ event.Skip(); }
		virtual void toggleNormals( wxCommandEvent& event ){ event.Skip(); }
		virtual void togglePointCloud( wxCommandEvent& event ){ event.Skip(); }
		virtual void toggleAxis( wxCommandEvent& event ){ event.Skip(); }
		virtual void increasePtSize( wxCommandEvent& event ){ event.Skip(); }
			virtual void decreasePtSize( wxCommandEvent& event ){ event.Skip(); }

virtual void increaseTriSize( wxCommandEvent& event ){ event.Skip(); }
			virtual void decreaseTriSize( wxCommandEvent& event ){ event.Skip(); }

	public:
		SFMMainfram( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("SFM AR toolkit"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~SFMMainfram();

};

#endif //__sfmMainFram__

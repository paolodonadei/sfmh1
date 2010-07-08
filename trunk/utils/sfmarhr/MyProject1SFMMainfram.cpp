#include "MyProject1SFMMainfram.h"
#include "wx/wx.h"
MyProject1SFMMainfram::MyProject1SFMMainfram( wxWindow* parent )
:
SFMMainfram( parent )
{


    m_canvas = new TestGLCanvas(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

}

void MyProject1SFMMainfram::framCharPressed( wxKeyEvent& event )
{
	 event.Skip();

}

void MyProject1SFMMainfram::framKeyPressed( wxKeyEvent& event )
{
 event.Skip();

}

void MyProject1SFMMainfram::framMouseClicked( wxMouseEvent& event )
{
 event.Skip();
}

void MyProject1SFMMainfram::OpenPlyFile( wxCommandEvent& event )
{
  wxString mystring;
    wxFileDialog* OpenDialog = new wxFileDialog(
        this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
        _("Text files (*.ply)|*.ply"),
        wxFD_OPEN, wxDefaultPosition);

    // Creates a "open file" dialog with 4 file types
    if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
    {

        mystring=OpenDialog->GetPath(); // Set the Title to reflect the file open
    }

    // Clean up after ourselves
    OpenDialog->Destroy();
    char buf[300];

    strcpy( buf, (const char*)mystring.mb_str(wxConvUTF8) );
    m_canvas->myply=new HRply(string(buf));
    m_canvas->myply->normalizePts(-m_canvas->rangeBound,m_canvas->rangeBound);
    m_canvas->myply->formTriangles(0.015);
                Refresh(false);
}

void MyProject1SFMMainfram::OpenCamCenter( wxCommandEvent& event )
{
	 event.Skip();
}

void MyProject1SFMMainfram::ExitFunc( wxCommandEvent& event )
{
 event.Skip();
}

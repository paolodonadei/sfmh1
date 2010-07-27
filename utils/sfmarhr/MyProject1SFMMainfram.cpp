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
        _("Ply files (*.ply)|*.ply"),
        wxFD_OPEN, wxDefaultPosition);

    // Creates a "open file" dialog with 4 file types
    if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
    {

        mystring=OpenDialog->GetPath(); // Set the Title to reflect the file open
    }
        else
    {
        OpenDialog->Destroy();
        return ;
    }

    // Clean up after ourselves
    OpenDialog->Destroy();
    char buf[300];

    strcpy( buf, (const char*)mystring.mb_str(wxConvUTF8) );
    m_canvas->myply=new HRply(string(buf));
    m_canvas->myply->normalizePts(-m_canvas->rangeBound,m_canvas->rangeBound);
    m_canvas->myply->formTriangles(m_canvas->triangleSize);
    m_canvas->rendertriangles=1;
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
void MyProject1SFMMainfram::openOBJ( wxCommandEvent& event )
{
    wxString mystring;
    wxFileDialog* OpenDialog = new wxFileDialog(
        this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
        _("Obj files (*.obj)|*.obj"),
        wxFD_OPEN, wxDefaultPosition);

    // Creates a "open file" dialog with 4 file types
    if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
    {

        mystring=OpenDialog->GetPath(); // Set the Title to reflect the file open
    }
    else
    {
        OpenDialog->Destroy();
        return ;
    }

    // Clean up after ourselves
    OpenDialog->Destroy();
    char buf[300];

    strcpy( buf, (const char*)mystring.mb_str(wxConvUTF8) );
    m_canvas->objmodel=new HRModel(string(buf));
    m_canvas->objmodel->openOBJ();
    Refresh(false);
}
void MyProject1SFMMainfram::toggleTriangles( wxCommandEvent& event )
{
	m_canvas->rendertriangles=(m_canvas->rendertriangles==0?1:0);

  Refresh(false);


}

void MyProject1SFMMainfram::toggleNormals( wxCommandEvent& event )
{
m_canvas->shownormals=(m_canvas->shownormals==0?1:0);
  Refresh(false);
}

void MyProject1SFMMainfram::togglePointCloud( wxCommandEvent& event )
{
m_canvas->renderpoints=(m_canvas->renderpoints==0?1:0);
  Refresh(false);
}

void MyProject1SFMMainfram::toggleAxis( wxCommandEvent& event )
{
m_canvas->showaxis=(m_canvas->showaxis==0?1:0);
  Refresh(false);
}

void MyProject1SFMMainfram::increasePtSize( wxCommandEvent& event )
{

m_canvas->pointSize++;
  Refresh(false);
}
void MyProject1SFMMainfram::decreasePtSize( wxCommandEvent& event )
{
m_canvas->pointSize--;
  Refresh(false);
}
void MyProject1SFMMainfram::increaseTriSize( wxCommandEvent& event )
{

m_canvas->triangleSize=m_canvas->triangleSize+0.005;
    m_canvas->myply->formTriangles(m_canvas->triangleSize);
  Refresh(false);
}
void MyProject1SFMMainfram::decreaseTriSize( wxCommandEvent& event )
{
m_canvas->triangleSize=m_canvas->triangleSize-0.005;
    m_canvas->myply->formTriangles(m_canvas->triangleSize);
  Refresh(false);
}

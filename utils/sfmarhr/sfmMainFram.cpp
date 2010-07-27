///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "sfmMainFram.h"

///////////////////////////////////////////////////////////////////////////

SFMMainfram::SFMMainfram( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_statusBar1 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Open PLY") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );

	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Open Cam Centers") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem2 );

	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Open Object File") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem4 );

	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem3 );

	m_menubar1->Append( m_menu1, wxT("File") );

	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Toggle Triangles") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem5 );

	wxMenuItem* m_menuItem6;
	m_menuItem6 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Toggle Normals") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem6 );

	wxMenuItem* m_menuItem7;
	m_menuItem7 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Toggle Point Cloud") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem7 );

	wxMenuItem* m_menuItem8;
	m_menuItem8 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Toggle Axis") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem8 );

	wxMenuItem* m_menuItem9;
	m_menuItem9 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Increase Point Size") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem9 );

	wxMenuItem* m_menuItem10;
	m_menuItem10 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Decrease Point Size") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem10 );

		wxMenuItem* m_menuItem11;
	m_menuItem11 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Increase Triangle Size") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem11 );

	wxMenuItem* m_menuItem12;
	m_menuItem12 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("Decrease Point Size") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem12 );


	m_menubar1->Append( m_menu3, wxT("PLY Options") );

	m_menu2 = new wxMenu();
	m_menubar1->Append( m_menu2, wxT("MyMenu") );

	this->SetMenuBar( m_menubar1 );


	// Connect Events
	this->Connect( wxEVT_CHAR, wxKeyEventHandler( SFMMainfram::framCharPressed ) );
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( SFMMainfram::framKeyPressed ) );
	this->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SFMMainfram::framMouseClicked ) );
	this->Connect( m_menuItem1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::OpenPlyFile ) );
	this->Connect( m_menuItem2->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::OpenCamCenter ) );
	this->Connect( m_menuItem4->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::openOBJ ) );
	this->Connect( m_menuItem3->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::ExitFunc ) );
	this->Connect( m_menuItem5->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::toggleTriangles ) );
	this->Connect( m_menuItem6->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::toggleNormals ) );
	this->Connect( m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::togglePointCloud ) );
	this->Connect( m_menuItem8->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::toggleAxis ) );
	this->Connect( m_menuItem9->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::increasePtSize ) );
		this->Connect( m_menuItem10->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::decreasePtSize ) );

	this->Connect( m_menuItem11->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::increaseTriSize ) );
		this->Connect( m_menuItem12->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::decreaseTriSize ) );
}

SFMMainfram::~SFMMainfram()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CHAR, wxKeyEventHandler( SFMMainfram::framCharPressed ) );
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( SFMMainfram::framKeyPressed ) );
	this->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SFMMainfram::framMouseClicked ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::OpenPlyFile ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::OpenCamCenter ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::openOBJ ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::ExitFunc ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::toggleTriangles ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::toggleNormals ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::togglePointCloud ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::toggleAxis ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::increasePtSize ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::decreasePtSize ) );

	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::increaseTriSize ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::decreaseTriSize ) );

}

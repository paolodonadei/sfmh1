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

	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem3 );

	m_menubar1->Append( m_menu1, wxT("File") );

	m_menu3 = new wxMenu();
	m_menubar1->Append( m_menu3, wxT("MyMenu") );

	m_menu2 = new wxMenu();
	m_menubar1->Append( m_menu2, wxT("MyMenu") );

	this->SetMenuBar( m_menubar1 );


	// Connect Events
	this->Connect( wxEVT_CHAR, wxKeyEventHandler( SFMMainfram::framCharPressed ) );
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( SFMMainfram::framKeyPressed ) );
	this->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SFMMainfram::framMouseClicked ) );
	this->Connect( m_menuItem1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::OpenPlyFile ) );
	this->Connect( m_menuItem2->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::OpenCamCenter ) );
	this->Connect( m_menuItem3->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::ExitFunc ) );
}

SFMMainfram::~SFMMainfram()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CHAR, wxKeyEventHandler( SFMMainfram::framCharPressed ) );
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( SFMMainfram::framKeyPressed ) );
	this->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SFMMainfram::framMouseClicked ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::OpenPlyFile ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::OpenCamCenter ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SFMMainfram::ExitFunc ) );
}

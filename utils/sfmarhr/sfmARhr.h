/////////////////////////////////////////////////////////////////////////////
// Name:        cube.h
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: cube.h 33105 2005-03-27 18:08:08Z VZ $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CUBE_H_
#define _WX_CUBE_H_

#include "wx/glcanvas.h"
#include "hrply.h"
#include "hrmodel.h"
// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit();
};

// Define a new frame type
class TestGLCanvas;


#if wxUSE_GLCANVAS

class TestGLCanvas: public wxGLCanvas
{
    friend class MyFrame;
public:
    TestGLCanvas( wxWindow *parent, wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxWANTS_CHARS, const wxString& name = _T("TestGLCanvas") );

    TestGLCanvas( wxWindow *parent, const TestGLCanvas *other,
                  wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = 0,
                  const wxString& name = _T("TestGLCanvas") );

    ~TestGLCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnterWindow(wxMouseEvent& event);

    void Render();
    void InitGL();
    void Rotate(GLfloat deg);
       void Rotatez(GLfloat deg);
    static GLfloat CalcRotateSpeed(unsigned long acceltime);
    static GLfloat CalcRotateAngle( unsigned long lasttime,
                                    unsigned long acceltime );
    void Action( long code, unsigned long lasttime,
                 unsigned long acceltime );
    HRply* myply;
    HRModel* objmodel;
    double rangeBound;
    int trinum;
    int shownormals;
int showaxis;
int rendertriangles;
int renderpoints;
double pointSize;
double triangleSize;

private:
    bool   m_init;
    GLuint m_gllist;
    long   m_rleft;
    long   m_rright;
    long  m_rup;
    long  m_rdown;
    static unsigned long  m_secbase;
    static int            m_TimeInitialized;
    static unsigned long  m_xsynct;
    static unsigned long  m_gsynct;

    long           m_Key;
    unsigned long  m_StartTime;
    unsigned long  m_LastTime;
    unsigned long  m_LastRedraw;

    DECLARE_EVENT_TABLE()
};

#endif // #if wxUSE_GLCANVAS

#endif // #ifndef _WX_CUBE_H_


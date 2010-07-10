
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "MyProject1SFMMainfram.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;


#include "hrply.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "sfmARhr.h"


#ifndef __WXMSW__     // for StopWatch, see remark below
#if defined(__WXMAC__) && !defined(__DARWIN__)
#include <utime.h>
#include <unistd.h>
#else
#include <sys/time.h>
#include <sys/unistd.h>
#endif
#else
#include <sys/timeb.h>
#endif


/*----------------------------------------------------------------------
  Utility function to get the elapsed time (in msec) since a given point
  in time (in sec)  (because current version of wxGetElapsedTime doesn´t
  works right with glibc-2.1 and linux, at least for me)
-----------------------------------------------------------------------*/
unsigned long StopWatch( unsigned long *sec_base )
{
    unsigned long secs,msec;

#if defined(__WXMSW__)
    struct timeb tb;
    ftime( &tb );
    secs = tb.time;
    msec = tb.millitm;
#elif defined(__WXMAC__) && !defined(__DARWIN__)
    wxLongLong tl = wxGetLocalTimeMillis();
    secs = (unsigned long) (tl.GetValue() / 1000);
    msec = (unsigned long) (tl.GetValue() - secs*1000);
#else
    // think every unice has gettimeofday
    struct timeval tv;
    gettimeofday( &tv, (struct timezone *)NULL );
    secs = tv.tv_sec;
    msec = tv.tv_usec/1000;
#endif

    if( *sec_base == 0 )
        *sec_base = secs;

    return( (secs-*sec_base)*1000 + msec );
}

/*----------------------------------------------------------------
  Implementation of Test-GLCanvas
-----------------------------------------------------------------*/

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
    EVT_KEY_DOWN( TestGLCanvas::OnKeyDown )
    EVT_KEY_UP( TestGLCanvas::OnKeyUp )
    EVT_ENTER_WINDOW( TestGLCanvas::OnEnterWindow )
END_EVENT_TABLE()

unsigned long  TestGLCanvas::m_secbase = 0;
int            TestGLCanvas::m_TimeInitialized = 0;
unsigned long  TestGLCanvas::m_xsynct;
unsigned long  TestGLCanvas::m_gsynct;

TestGLCanvas::TestGLCanvas(wxWindow *parent, wxWindowID id,
                           const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
{
    m_init = false;
    m_gllist = 0;
    m_rleft = WXK_LEFT;
    m_rright = WXK_RIGHT;
    m_rup = WXK_UP;
    m_rdown = WXK_DOWN;
    myply=NULL;
    objmodel=NULL;
    rangeBound=0.5;
    trinum=0;
}

TestGLCanvas::TestGLCanvas(wxWindow *parent, const TestGLCanvas *other,
                           wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                           const wxString& name )
    : wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name)
{
    m_init = false;
    m_gllist = other->m_gllist; // share display list
    m_rleft = WXK_LEFT;
    m_rright = WXK_RIGHT;
    myply=NULL;
    rangeBound=0.5;
    trinum=0;
    objmodel=NULL;
}

TestGLCanvas::~TestGLCanvas()
{
}

void TestGLCanvas::Render()
{
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();
    // Init OpenGL once, but after SetCurrent
    if (!m_init)
    {
        InitGL();
        m_init = true;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
    glMatrixMode(GL_MODELVIEW);

    /* clear color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(myply!=NULL)
    {
        //myply->renderpoints();
        myply->rendertriangles();
        myply->rendernormals();
        myply->draw_axis( 3 );

        //   myply-> rendertrianglesingle( trinum);

    }
    if(objmodel!=NULL)
    {
        printf("render object\n");
        objmodel->drawOBJ();
    }
    glFlush();
    SwapBuffers();
}

void TestGLCanvas::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
    SetFocus();
}

void TestGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    Render();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);
#ifndef __WXMOTIF__
    if (GetContext())
#endif
    {
        SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void TestGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing.
}

void TestGLCanvas::InitGL()
{
    SetCurrent();

    /* set viewing projection */
    glMatrixMode(GL_PROJECTION);
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

    /* position viewer */
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0f, 0.0f, -2.0f);

    /* position object */
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);


    //  glEnable(GL_LIGHTING);
//   glEnable(GL_LIGHT0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
//    glDepthFunc(GL_LESS);			        // The Type Of Depth Test To Do
//    glEnable(GL_DEPTH_TEST);		        // Enables Depth Testing
//    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
//glEnable(GL_TEXTURE_2D);


}

GLfloat TestGLCanvas::CalcRotateSpeed( unsigned long acceltime )
{
    GLfloat t,v;

    t = ((GLfloat)acceltime) / 1000.0f;

    if( t < 0.5f )
        v = t;
    else if( t < 1.0f )
        v = t * (2.0f - t);
    else
        v = 0.75f;

    return(v);
}

GLfloat TestGLCanvas::CalcRotateAngle( unsigned long lasttime,
                                       unsigned long acceltime )
{
    GLfloat t,s1,s2;

    t = ((GLfloat)(acceltime - lasttime)) / 1000.0f;
    s1 = CalcRotateSpeed( lasttime );
    s2 = CalcRotateSpeed( acceltime );

    return( t * (s1 + s2) * 135.0f );
}

void TestGLCanvas::Action( long code, unsigned long lasttime,
                           unsigned long acceltime )
{

    GLfloat angle = CalcRotateAngle( lasttime, acceltime );

    printf("action %f\n",angle);

    if (code == m_rleft)
        Rotate( angle );
    else if (code == m_rright)
        Rotate( -angle );
    else if (code == m_rup)
        Rotatez( -angle );
    else if (code == m_rdown)
        Rotatez( -angle );
}

void TestGLCanvas::OnKeyDown( wxKeyEvent& event )
{
    long evkey = event.GetKeyCode();


    if(myply!=NULL)
    {


        if(evkey == WXK_PAGEUP)
        {
            rangeBound+=0.1;
            myply->normalizePts(-rangeBound,rangeBound);
            myply->formTriangles(0.015);
            Refresh(false);
        }
        if(evkey ==WXK_PAGEDOWN)
        {
            rangeBound-=0.1;
            myply->normalizePts(-rangeBound,rangeBound);
            myply->formTriangles(0.015);
            Refresh(false);
        }
        if(evkey == WXK_SPACE)
        {
            trinum++;
            glTranslatef(0.0f, 0.0f, -.5f);
            Refresh(false);
        }
    }

    if (evkey == 0) return;
    cout<<evkey<<endl;

    if (!m_TimeInitialized)
    {
        m_TimeInitialized = 1;
        m_xsynct = event.GetTimestamp();
        m_gsynct = StopWatch(&m_secbase);

        m_Key = evkey;
        m_StartTime = 0;
        m_LastTime = 0;
        m_LastRedraw = 0;
    }

    unsigned long currTime = event.GetTimestamp() - m_xsynct;

    if (evkey != m_Key)
    {
        m_Key = evkey;
        m_LastRedraw = m_StartTime = m_LastTime = currTime;
    }

    if (currTime >= m_LastRedraw)      // Redraw:
    {
        Action( m_Key, m_LastTime-m_StartTime, currTime-m_StartTime );

#if defined(__WXMAC__) && !defined(__DARWIN__)
        m_LastRedraw = currTime;    // StopWatch() doesn't work on Mac...
#else
        m_LastRedraw = StopWatch(&m_secbase) - m_gsynct;
#endif
        m_LastTime = currTime;
    }

    event.Skip();
}

void TestGLCanvas::OnKeyUp( wxKeyEvent& event )
{
    m_Key = 0;
    m_StartTime = 0;
    m_LastTime = 0;
    m_LastRedraw = 0;

    event.Skip();
}

void TestGLCanvas::Rotate( GLfloat deg )
{
    SetCurrent();

    glMatrixMode(GL_MODELVIEW);
    glRotatef((GLfloat)deg, 0.0f, 0.0f, 1.0f);
    Refresh(false);
}
void TestGLCanvas::Rotatez( GLfloat deg )
{
    SetCurrent();

    glMatrixMode(GL_MODELVIEW);
    glRotatef((GLfloat)deg, 0.0f,  1.0f, 0.0f);
    Refresh(false);
}


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    // Create the main frame window
    MyProject1SFMMainfram* myfram=new MyProject1SFMMainfram(NULL);
    myfram->Show(true);
    return true;
}

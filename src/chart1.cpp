/******************************************************************************
 * $Id: chart1.cpp,v 1.61 2009/09/25 15:11:18 bdbcat Exp $
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Main wxWidgets Program
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) $YEAR$ by $AUTHOR$   *
 *   $EMAIL$   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 *
 * $Log: chart1.cpp,v $
 * Revision 1.61  2009/09/25 15:11:18  bdbcat
 * Implement chart Activate/Deactivate
 *
 * Revision 1.60  2009/09/18 02:48:33  bdbcat
 * Various
 *
 * Revision 1.59  2009/09/11 23:19:46  bdbcat
 * Implement png graphics
 *
 * Revision 1.58  2009/09/04 02:00:53  bdbcat
 * Improve debug messages
 *
 * Revision 1.57  2009/09/01 22:18:33  bdbcat
 * Change status bar background
 *
 * Revision 1.56  2009/08/31 02:37:11  bdbcat
 * Improve Status Bar
 *
 * Revision 1.55  2009/08/30 03:34:58  bdbcat
 * Optimize for first time use, correct ENC preview logic
 *
 * Revision 1.54  2009/08/29 23:29:11  bdbcat
 * Implement HDT, HDG, and HDM messages
 *
 * Revision 1.53  2009/08/25 21:38:03  bdbcat
 * Various, including GDK SIGSEGV workaround
 *
 * Revision 1.52  2009/08/22 01:26:18  bdbcat
 * Tracks
 *
 * Revision 1.51  2009/08/03 03:26:16  bdbcat
 * Improve data dir finder for MSW
 *
 * Revision 1.50  2009/07/29 00:54:59  bdbcat
 * Update for gcc 4.2.4, improve linux port detection
 *
 * Revision 1.49  2009/07/17 03:55:15  bdbcat
 * Move hotkey handler to ChartCanvas.
 *
 * Revision 1.48  2009/07/16 02:44:53  bdbcat
 * Whell zoom to point.
 *
 * Revision 1.47  2009/07/08 03:38:59  bdbcat
 * Cleanup.
 *
 * Revision 1.46  2009/07/08 01:54:46  bdbcat
 * Convert AISDecoder to wxEvtHandler.
 *
 * Revision 1.45  2009/07/03 02:59:41  bdbcat
 * Improve AIS Dialogs.
 *
 * Revision 1.44  2009/06/30 03:01:35  bdbcat
 * Add configurable GDAL debug messages.
 *
 * Revision 1.43  2009/06/28 03:08:55  bdbcat
 * Add MouseEvent to dummy TextCtrl.
 *
 * Revision 1.42  2009/06/28 02:04:08  bdbcat
 * Convert "About" dialog to non-modal.
 *
 * Revision 1.41  2009/06/20 03:40:55  bdbcat
 * Add setlocale() to avoid "," vs "." problem.
 *
 * Revision 1.40  2009/06/18 01:34:36  bdbcat
 * Cleanup.
 *
 * Revision 1.39  2009/06/17 02:43:54  bdbcat
 * Update hotkeys
 *
 * Revision 1.38  2009/06/14 03:33:02  bdbcat
 * More HotKeys
 *
 * Revision 1.37  2009/06/14 01:51:04  bdbcat
 * AIS Alert Dialog, Update toolbar
 *
 * Revision 1.36  2009/06/03 03:13:28  bdbcat
 * Implement HotKey support
 *
 * Revision 1.35  2009/05/05 03:56:30  bdbcat
 * Force Edge Priority update on dialog
 *
 * Revision 1.34  2009/03/26 22:29:03  bdbcat
 * Opencpn 1.3.0 Update
 *
 * Revision 1.33  2008/12/22 18:40:04  bdbcat
 * Add NMEA Debug flag
 *
 * Revision 1.32  2008/12/19 04:15:43  bdbcat
 * Constrain log file length
 *
 * Revision 1.31  2008/12/05 23:05:49  bdbcat
 * *** empty log message ***
 *
 * Revision 1.30  2008/11/12 04:13:24  bdbcat
 * Support Garmin Devices / Cleanup
 *
 * Revision 1.29  2008/11/01 16:03:08  bdbcat
 * Improve data file location logic
 *
 * Revision 1.28  2008/08/29 02:25:58  bdbcat
 * Add compiler #ifdef to support ConvertToGreyscale
 *
 * Revision 1.27  2008/08/26 13:46:25  bdbcat
 * Better color scheme support
 *
 * Revision 1.26  2008/08/09 23:58:40  bdbcat
 * Numerous revampings....
 *
 * Revision 1.24  2008/04/14 19:38:00  bdbcat
 * Add quitflag++ to OnExit()
 *
 * Revision 1.23  2008/04/11 03:25:08  bdbcat
 * Implement Auto Anchor Mark
 *
 * Revision 1.22  2008/04/10 01:06:38  bdbcat
 * Cleanup
 *
 * Revision 1.21  2008/03/31 00:23:06  bdbcat
 * Correct merge problems
 *
 * Revision 1.20  2008/03/30 21:51:57  bdbcat
 * Update for Mac OSX/Unicode
 *
 * Revision 1.19  2008/01/12 06:24:20  bdbcat
 * Update for Mac OSX/Unicode
 *
 * Revision 1.18  2008/01/11 01:39:32  bdbcat
 * Update for Mac OSX
 *
 * Revision 1.17  2008/01/10 03:35:45  bdbcat
 * Update for Mac OSX
 *
 * Revision 1.15  2007/06/15 03:07:15  bdbcat
 * Improve toolbar color scheme support
 *
 * Revision 1.14  2007/06/10 03:19:29  bdbcat
 * Fix Leaks
 *
 * Revision 1.13  2007/06/10 02:24:17  bdbcat
 * Implement global color scheme
 *
 * Revision 1.12  2007/05/03 13:23:55  dsr
 * Major refactor for 1.2.0
 *
 * Revision 1.11  2007/03/02 02:09:06  dsr
 * Cleanup, convert to UTM Projection
 *
 * Revision 1.10  2007/02/06 02:07:39  dsr
 * Pause event generators during Settings Dialog
 *
 * Revision 1.9  2006/11/01 02:15:58  dsr
 * AIS Support
 *
 *
 */
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/artprov.h"
#include "wx/stdpaths.h"
#include "scrollingdialog.h"



#include "dychart.h"

#ifdef __WXMSW__
  #include <stdlib.h>
  #include <math.h>
  #include <time.h>
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>
#endif

#include "chart1.h"
#include "chcanv.h"
#include "chartdb.h"
#include "navutil.h"
#include "routeman.h"
#include "statwin.h"
#include "concanv.h"
#include "nmea.h"
#include "options.h"
#include "about.h"
#include "thumbwin.h"
#include "tcmgr.h"
#include "cpl_error.h"
#include "ais.h"
#include "chartimg.h"               // for ChartBaseBSB
#include "routeprop.h"
#include "cm93.h"

#include <wx/image.h>
#include "wx/apptrait.h"

// begin rms
#ifdef __WXOSX__
#include "macutils.h"
#endif
// end rms

#ifdef USE_S57
#include "s52plib.h"
#include "s57chart.h"
#include "s57.h"
#include "cpl_csv.h"
#endif

#ifdef USE_WIFI_CLIENT
#include "wificlient.h"
#endif

//#include <mcheck.h>


//------------------------------------------------------------------------------
//      Static variable definition
//------------------------------------------------------------------------------
CPL_CVSID("$Id: chart1.cpp,v 1.61 2009/09/25 15:11:18 bdbcat Exp $");


FILE            *flog;                  // log file
wxLog           *logger;
wxLog           *Oldlogger;

ComPortManager  *g_pCommMan;

MyFrame         *gFrame;

ChartCanvas     *cc1;
ConsoleCanvas   *console;
NMEAWindow      *g_pnmea;
StatWin         *stats;

wxToolBar       *toolBar;

MyConfig        *pConfig;

ChartBase       *Current_Vector_Ch;
ChartBase       *Current_Ch;
ChartDB         *ChartData;
ChartStack      *pCurrentStack;
wxString        *pdir_list[20];

RouteList       *pRouteList;

Select          *pSelect;
Select          *pSelectTC;
Select          *pSelectAIS;

Routeman        *pRouteMan;
WayPointman     *pWayPointMan;
MarkProp        *pMarkPropDialog;
RouteProp       *pRoutePropDialog;
MarkInfo        *pMarkInfoDialog;         // toh, 2009.02.08


double          gLat, gLon, gCog, gSog, gHdt, gHDg, gVar;
double          vLat, vLon;
double          initial_scale_ppm;

wxArrayString   *pChartDirArray;
bool            bDBUpdateInProgress;

ThumbWin        *pthumbwin;
TCMgr           *ptcmgr;

bool            bDrawCurrentValues;

wxString        g_SData_Locn;
wxString        *pChartListFileName;
wxString        *pTC_Dir;
wxString        *pHome_Locn;
wxString        *pWVS_Locn;
wxString        *pInit_Chart_Dir;
wxString        *g_pcsv_locn;
wxString        *g_pSENCPrefix;
wxString        *g_pPresLibData;

int             user_user_id;
int             file_user_id;

int             quitflag;
int             g_tick;
int             mem_total, mem_initial;

bool            s_bSetSystemTime;

wxString        *phost_name;

static unsigned int malloc_max;

OCP_NMEA_Thread   *pNMEA_Thread;
OCP_GARMIN_Thread *pGARMIN_Thread;
wxString        *pNMEADataSource;
wxString        *pNMEA_AP_Port;

wxDateTime      g_start_time;
bool            g_bCruising;

ChartDummy      *pDummyChart;

wxString        *pWIFIServerName;

AutoPilotWindow *pAPilot;

// Global print data, to remember settings during the session
wxPrintData *g_printData = (wxPrintData*) NULL ;

// Global page setup data
wxPageSetupData* g_pageSetupData = (wxPageSetupData*) NULL;

bool              g_bShowPrintIcon;
bool              g_bShowOutlines;
bool              g_bShowDepthUnits;
bool              g_bGarminPersistance;
int               g_nNMEADebug;

bool              g_bShowGPXIcons;  // toh, 2009.02.14
bool              g_bNavAidShowRadarRings;            // toh, 2009.02.24
int               g_iNavAidRadarRingsNumberVisible;   // toh, 2009.02.24
float             g_fNavAidRadarRingsStep;            // toh, 2009.02.24
int               g_pNavAidRadarRingsStepUnits;       // toh, 2009.02.24
bool              g_bWayPointPreventDragging;         // toh, 2009.02.24

FontMgr         *pFontMgr;

ColorScheme     global_color_scheme;
int             Usercolortable_index;
wxArrayPtrVoid  *UserColorTableArray;
wxArrayPtrVoid  *UserColourHashTableArray;
ColourHash      *pcurrent_user_color_hash;

int             gsp_watchdog_timeout_ticks;
int             gGPS_Watchdog;
bool            bGPSValid;

int             gHDx_Watchdog;
bool            g_bHDxValid;

bool            g_bDebugCM93;
bool            g_bDebugS57;

#ifdef USE_S57
s52plib           *ps52plib;
S57ClassRegistrar *g_poRegistrar;

// begin rms
#elif defined __WXOSX__
s52plib         *ps52plib;
// end rms
#endif

#ifdef USE_WIFI_CLIENT
WIFIWindow      *pWIFI;
#endif



static wxString *pval;          // Private environment temp storage

#ifdef __WXOSX__
#include "macutils.h"
#endif

// begin rms
#if defined( USE_S57) || defined ( __WXOSX__ )
#ifdef __WXMSW__
#ifdef USE_GLU_TESS
#ifdef USE_GLU_DLL
// end rms
extern bool           s_glu_dll_ready;
extern HINSTANCE      s_hGLU_DLL;                   // Handle to DLL
#endif
#endif
#endif
#endif


OCP_AIS_Thread  *pAIS_Thread;
AIS_Decoder     *g_pAIS;
wxString        *pAIS_Port;
bool             g_bGPSAISMux;
bool             g_bAIS_CPA_Alert;
bool             g_bAIS_CPA_Alert_Audio;
AISTargetAlertDialog    *g_pais_alert_dialog_active;
AISTargetQueryDialog    *g_pais_query_dialog_active;

int               g_ais_alert_dialog_x, g_ais_alert_dialog_y;
int               g_ais_alert_dialog_sx, g_ais_alert_dialog_sy;
int               g_ais_query_dialog_x, g_ais_query_dialog_y;

bool            s_socket_test_running;
bool            s_socket_test_passed;
wxSocketClient  *s_t_sock;
wxSocketServer  *s_s_sock;


int              g_nframewin_x;
int              g_nframewin_y;
bool             g_bframemax;

bool             g_bAutoAnchorMark;

wxRect           g_blink_rect;
double           g_PlanSpeed;

wxArrayString    *pMessageOnceArray;

FILE             *s_fpdebug;
bool             bAutoOpen;
bool             bFirstAuto;

int              g_nCacheLimit;
bool             g_bGDAL_Debug;

//-----------------------------------------------------------------------------------------------------
//      OCP_NMEA_Thread Static data store
//-----------------------------------------------------------------------------------------------------
char                            rx_share_buffer[MAX_RX_MESSSAGE_SIZE];
unsigned int                    rx_share_buffer_length;
ENUM_BUFFER_STATE               rx_share_buffer_state;


#ifndef __WXMSW__
struct sigaction sa_all;
struct sigaction sa_all_old;
#endif




#ifdef __WXMSW__
//    System color control support

typedef DWORD (WINAPI *SetSysColors_t)(DWORD, DWORD *, DWORD *);
typedef DWORD (WINAPI *GetSysColor_t)(DWORD);

SetSysColors_t pSetSysColors;
GetSysColor_t  pGetSysColor;

void SaveSystemColors(void);
void RestoreSystemColors(void);

DWORD       color_3dface;
DWORD       color_3dhilite;
DWORD       color_3dshadow;
DWORD       color_3ddkshadow;
DWORD       color_3dlight;
DWORD       color_activecaption;
DWORD       color_gradientactivecaption;
DWORD       color_captiontext;
DWORD       color_windowframe;
DWORD       color_inactiveborder;

#endif

cm93manager       *s_pcm93mgr;

//    AIS Global configuration
bool             g_bCPAMax;
double           g_CPAMax_NM;
bool             g_bCPAWarn;
double           g_CPAWarn_NM;
bool             g_bTCPA_Max;
double           g_TCPA_Max;
bool             g_bMarkLost;
double           g_MarkLost_Mins;
bool             g_bRemoveLost;
double           g_RemoveLost_Mins;
bool             g_bShowCOG;
double           g_ShowCOG_Mins;
bool             g_bShowTracks;
double           g_ShowTracks_Mins;
bool             g_bShowMoored;
double           g_ShowMoored_Kts;
wxString         g_sAIS_Alert_Sound_File;
bool             g_bAIS_CPA_Alert_Suppress_Moored;


DummyTextCtrl    *g_pDummyTextCtrl;
bool             g_bEnableZoomToCursor;

wxString         g_CM93DictDir;

bool             g_bShowTrackIcon;
bool             g_bTrackActive;
Track            *g_pActiveTrack;
double           g_TrackIntervalSeconds;
double           g_TrackDeltaDistance;
bool             g_bTrackTime;
bool             g_bTrackDistance;

int              g_total_NMEAerror_messages;

int              g_cm93_zoom_factor;
CM93DSlide       *pCM93DetailSlider;
bool             g_bShowCM93DetailSlider;
int              g_cm93detail_dialog_x, g_cm93detail_dialog_y;

bool             g_bUseGreenShip;


static char nmea_tick_chars[] = {'|', '/', '-', '\\', '|', '/', '-', '\\'};
static int tick_idx;

#ifndef __WXMSW__
jmp_buf           env;                    // the context saved by setjmp();
#endif

// {2C9C45C2-8E7D-4C08-A12D-816BBAE722C0}
#ifdef  __WXMSW__
DEFINE_GUID(GARMIN_DETECT_GUID, 0x2c9c45c2L, 0x8e7d, 0x4c08, 0xa1, 0x2d, 0x81, 0x6b, 0xba, 0xe7, 0x22, 0xc0);
#endif

#ifdef __MSVC__66
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef __MSVC__
     extern  "C" long  __stdcall MyUnhandledExceptionFilter( struct _EXCEPTION_POINTERS *ExceptionInfo );
#endif

//    Some static helpers
void appendOSDirSlash(wxString* pString);
void InitializeUserColors(void);
void DeInitializeUserColors(void);
void SetSystemColors(ColorScheme cs);


//------------------------------------------------------------------------------
//    PNG Icon resources
//------------------------------------------------------------------------------
#ifdef OCPN_USE_PNGICONS

#include "bitmaps/icons.h"
#include "bitmaps/icons.cpp"

#endif

// ----------------------------------------------------------------------------
// Icon resources
// ----------------------------------------------------------------------------
#ifndef OCPN_USE_PNGICONS
 #include "bitmaps/settings.xpm"
 #include "bitmaps/zoomin.xpm"
 #include "bitmaps/zoomout.xpm"
 #include "bitmaps/scin.xpm"
 #include "bitmaps/scout.xpm"
 #include "bitmaps/tide.xpm"
 #include "bitmaps/text.xpm"
 #include "bitmaps/route.xpm"
 #include "bitmaps/exitt.xpm"
 #include "bitmaps/follow.xpm"
 #include "bitmaps/current.xpm"
 #include "bitmaps/print.xpm"
 #include "bitmaps/help.xpm"
 #include "bitmaps/colscheme.xpm"
 #include "bitmaps/gpx_import.xpm"
 #include "bitmaps/gpx_export.xpm"
 #include "bitmaps/track.xpm"
#endif

#include "bitmaps/opencpn.xpm"

//------------------------------------------------------------------------------
//              Fwd Refs
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------
//      Signal Handlers
//-----------------------------------------------------------------------
#ifndef __WXMSW__

//These are the signals possibly expected
//      SIGUSR1
//      Raised externally to cause orderly termination of application
//      Intended to act just like pushing the "EXIT" button

//      SIGSEGV
//      Some undefined segfault......

static void
catch_signals(int signo)
{
      switch(signo)
      {
            case SIGUSR1:
                  quitflag++;                             // signal to the timer loop
                  break;

            case SIGSEGV:
                  siglongjmp(env, 1);                       // jump back to the setjmp() point
                  break;

            default:
                  break;
      }

}
#endif




// `Main program' equivalent, creating windows and returning main app frame
//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

#include "wx/dynlib.h"

bool MyApp::OnInit()
{
      if (!wxApp::OnInit())
            return false;


//      CALLGRIND_STOP_INSTRUMENTATION

//      Establish the locale
//Fulup: force floating point to use dot as separation.
#ifdef __POSIX__
      setlocale(LC_NUMERIC,"C");
#endif

      g_start_time = wxDateTime::Now();

#ifdef __WXMSW__
    //testing
//    _controlfp( _EM_INVALID | _EM_DENORMAL /*| _EM_ZERODIVIDE*/
//    | _EM_OVERFLOW | _EM_UNDERFLOW |_EM_INEXACT, _MCW_EM );

      //    Handle any Floating Point Exceptions which may leak thru from other
      //    processes.  The exception filter is in cutil.c
    //  Dunno why it wont link in MSVC.....

    SetUnhandledExceptionFilter( &MyUnhandledExceptionFilter );
#endif

#ifdef __WXMSW__
//      _CrtSetBreakAlloc(173707);
#endif


#ifndef __WXMSW__
//      Setup Linux SIGNAL handling, for external program control

//      Build the sigaction structure
        sa_all.sa_handler = catch_signals ;             // point to my handler
        sigemptyset(&sa_all.sa_mask);                  // make the blocking set
                                                        // empty, so that all
                                                        // other signals will be
                                                        // unblocked during my handler
        sa_all.sa_flags = 0;

//      Register my request for some signals
        sigaction(SIGUSR1, &sa_all, NULL);
#endif

//      Initialize memory tracer
#ifndef __WXMSW__
//        mtrace();
#endif

//      Here is some experimental code for wxTheme support
//      Not also these lines included above....
//      They are necessary to ensure that the themes are statically loaded

//      #ifdef __WXUNIVERSAL__
//      WX_USE_THEME(gtk);
//      WX_USE_THEME(Metal);
//      #endif

#ifdef __WXUNIVERSAL__
//        wxTheme* theme = wxTheme::Create("gtk");
//        wxTheme::Set(theme);
#endif


//      Init the private memory manager
        malloc_max = 0;

//      wxHandleFatalExceptions(true);

//      Init the private environment handler
        pval = new wxString;

// Set up default FONT encoding, which should have been done by wxWidgets some time before this......
        wxFont temp_font(10, wxDEFAULT ,wxNORMAL, wxNORMAL, FALSE, wxString(_T("")), wxFONTENCODING_SYSTEM );
        temp_font.SetDefaultEncoding(wxFONTENCODING_SYSTEM );

//  Init my private font manager
        pFontMgr = new FontMgr();

//    Init the XPM image handler
        wxImage t;
        wxXPMHandler *th = new wxXPMHandler;
        t.AddHandler(th);

//      Establish a "home" location
        wxStandardPathsBase& std_path = wxApp::GetTraits()->GetStandardPaths();
        std_path.Get();

        pHome_Locn= new wxString;
        pHome_Locn->Append(std_path.GetUserConfigDir());          // on w98, produces "/windows/Application Data"
        appendOSDirSlash(pHome_Locn) ;


#if defined( __WXMAC__) || defined ( __WXMSW__ )
        pHome_Locn->Append(_T("opencpn"));
        appendOSDirSlash(pHome_Locn) ;
#endif

        // create the opencpn "home" directory if we need to
        wxFileName wxHomeFiledir(*pHome_Locn) ;
        if(true != wxHomeFiledir.DirExists(wxHomeFiledir.GetPath()))
            if(!wxHomeFiledir.Mkdir(wxHomeFiledir.GetPath()))
                  {
                        wxASSERT_MSG(false,_T("Cannot create opencpn home directory"));
                        return false ;
                  }

//      Establish Log File location
        wxString log;
        log.Append(std_path.GetUserConfigDir());
        appendOSDirSlash(&log);

#ifdef __WXMAC__
        log.Append(_T("Logs/"));
#elif defined __WXMSW__
        log.Append(_T("opencpn\\"));
#endif

        // create the opencpn "log" directory if we need to
        wxFileName wxLogFiledir(log) ;
        if(true != wxLogFiledir.DirExists(wxLogFiledir.GetPath()))
              if(!wxLogFiledir.Mkdir(wxLogFiledir.GetPath()))
        {
              wxASSERT_MSG(false,_T("Cannot create opencpn log directory"));
              return false ;
        }

        log.Append(_T("opencpn.log"));

        //  Constrain the size of the log file
        if(wxFileName::GetSize(log) > 1000000)
              ::wxRemoveFile(log);

        const char *mode = "a";
        flog = fopen(log.mb_str(), mode);
        logger=new wxLogStderr(flog);

        Oldlogger = wxLog::SetActiveTarget(logger);

//        wxLog::AddTraceMask("timer");               // verbose message traces to log output

//      Send init message
        wxLogMessage(_T("\n\n"));

        wxDateTime now = wxDateTime::Now();
        now.MakeGMT(true);                    // no DST

        wxString imsg = now.FormatISODate();
        imsg += _T("  ");
        imsg += now.FormatISOTime();

        imsg += _T(" -------Starting opencpn-------");
        wxLogMessage(imsg);

#ifdef USE_CPL
//      Set up a useable CPL library error handler
        CPLSetErrorHandler( MyCPLErrorHandler );
#endif


//    Initialize embedded PNG icon graphics
#ifdef OCPN_USE_PNGICONS
        ::wxInitAllImageHandlers();
//        wxImage::AddHandler(wxPNGHandler);
        initialize_images();
#endif


//      Create some static strings
        pNMEADataSource = new wxString();
        pNMEA_AP_Port = new wxString();
        pWIFIServerName = new wxString();
        pAIS_Port = new wxString();
        g_pcsv_locn = new wxString();
        pInit_Chart_Dir = new wxString();
        g_pSENCPrefix = new wxString();
        g_pPresLibData = new wxString;

//      Create an array string to hold repeating messages, so they don't
//      overwhelm the log
        pMessageOnceArray = new wxArrayString;

//      Init the Chart Dir Array(s)
        pChartDirArray = new wxArrayString();

//      Init the Selectable Route Items List
        pSelect = new Select();

//      Init the Selectable Tide/Current Items List
        pSelectTC = new Select();

//      Init the Selectable AIS Target List
        pSelectAIS = new Select();

//      Init the Route Manager
        pRouteMan = new Routeman();

//      Init the WayPoint Manager
        pWayPointMan = new WayPointman();

//      Who am I?
        phost_name = new wxString(::wxGetHostName());


//      (Optionally) Capture the user and file(effective) ids
//  Some build environments may need root privileges for hardware
//  port I/O, as in the NMEA data input class.  Set that up here.


#ifndef __WXMSW__
#ifdef PROBE_PORTS__WITH_HELPER
        user_user_id = getuid ();
        file_user_id = geteuid ();
#endif
#endif


//      Establish a "shared data" location
/*  From the wxWidgets documentation...

        wxStandardPaths::GetDataDir
        wxString GetDataDir() const
        Return the location of the applications global, i.e. not user-specific, data files.
            * Unix: prefix/share/appname
            * Windows: the directory where the executable file is located
            * Mac: appname.app/Contents/SharedSupport bundle subdirectory
*/

        g_SData_Locn = std_path.GetDataDir();         // where the application is located
        appendOSDirSlash(&g_SData_Locn) ;


//      Establish the location of the config file
#ifdef __WXMSW__
        wxString Config_File("opencpn.ini");
        Config_File.Prepend(*pHome_Locn);

#elif defined __WXMAC__
        wxString Config_File = std_path.GetUserConfigDir(); // should be ~/Library/Preferences
        appendOSDirSlash(&Config_File) ;
        Config_File.Append("opencpn.ini");
#else
        wxString Config_File = std_path.GetUserDataDir(); // should be ~/.opencpn
        appendOSDirSlash(&Config_File) ;
        Config_File.Append(_T("opencpn.conf"));
#endif

        bool b_novicemode = false;

        wxFileName config_test_file_name(Config_File);
        if(config_test_file_name.FileExists())
            wxLogMessage(_T("Using existing Config_File: ") + Config_File);
        else
        {
/*
              wxString msg1(_T("Cannot find Config File "));
              msg1.Append(Config_File);
              msg1.Append(_T("...OK to create? (will exit if No)"));

              wxMessageDialog mdlg(gFrame, msg1, wxString(_T("OpenCPN")),wxYES_NO  );
              if(mdlg.ShowModal() == wxID_YES)
*/
              {
                  wxLogMessage(_T("Creating new Config_File: ") + Config_File);

                  //    Flag to preset some options for initial config file creation
                  b_novicemode = true;

                  if(true != config_test_file_name.DirExists(config_test_file_name.GetPath()))
                       if(!config_test_file_name.Mkdir(config_test_file_name.GetPath()))
                               wxLogMessage(_T("Cannot create config file directory for ") + Config_File);
              }
/*
              else
              {
                   Config_File.Clear();
                   return false;                    // Probably will provoke some memory leakage....
              }
*/
        }

//      Open/Create the Config Object
        MyConfig *pCF = new MyConfig(wxString(_T("")), wxString(_T("")), Config_File);
        pConfig = (MyConfig *)pCF;
        pConfig->LoadMyConfig(0);


        //        A special case for windows, which has some trouble finding the data files....
#ifdef __WXMSW__
        wxString sfd(g_SData_Locn);
        appendOSDirSlash(&sfd);
        sfd.Append(_T("s57data"));

        if(!wxDir::Exists(sfd))                 // not found
        {
              wxString s_exe = std_path.GetExecutablePath();
              wxFileName fn_look(s_exe);
              while(fn_look.GetDirCount())
              {
                          wxString cpath = fn_look.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

                          // look for  path/s57data/S52RAZDS.RLE
                          wxString cp1 = cpath;
                          wxString cp1d = cp1;
                          cp1 += _T("s57data");
                          appendOSDirSlash(&cp1);
                          cp1 += _T("S52RAZDS.RLE");
                          if(wxFileName::FileExists(cp1))
                          {
                                g_SData_Locn = cp1d;
                                break;
                          }

                          // look for  path/data/s57data/S52RAZDS.RLE
                          cp1 = cpath;
                          cp1 += _T("data");
                          appendOSDirSlash(&cp1);
                          cp1d = cp1;
                          cp1 += _T("s57data");
                          appendOSDirSlash(&cp1);
                          cp1 += _T("S52RAZDS.RLE");
                          if(wxFileName::FileExists(cp1))
                          {
                                g_SData_Locn = cp1d;
                                break;
                          }

                          fn_look.RemoveLastDir();
              }
        }

#endif


#ifdef USE_S57
//      Init the s57 chart object, specifying the location of the required csv files

//      If the config file contains an entry for s57 .csv files,
//      confirm that the required files are available, and use them.
//      Otherwise, default to [shared data dir]/s57_data
        if(!g_pcsv_locn->IsEmpty())
        {
              wxString fd(*g_pcsv_locn);
              appendOSDirSlash(&fd);

              bool tflag = true;          // assume true
              wxFileName f1;
              f1 = wxFileName(fd); fd.Append(_T("s57attributes.csv"));
              tflag &= f1.FileExists();
              f1 = wxFileName(fd); fd.Append(_T("attdecode.csv"));
              tflag &= f1.FileExists();
              f1 = wxFileName(fd); fd.Append(_T("s57expectedinput.csv"));
              tflag &= f1.FileExists();
              f1 = wxFileName(fd); fd.Append(_T("s57objectclasses.csv"));
              tflag &= f1.FileExists();

              if(!tflag)
              {
                    g_pcsv_locn->Clear();
                    g_pcsv_locn->Append(g_SData_Locn);
                    g_pcsv_locn->Append(_T("s57data"));
              }

        }
        else
        {
              g_pcsv_locn->Append(g_SData_Locn);
              g_pcsv_locn->Append(_T("s57data"));
        }

//      If the config file contains an entry for SENC file prefix, use it.
//      Otherwise, default to std_path.GetUserDataDir()
        if(g_pSENCPrefix->IsEmpty())
        {
              g_pSENCPrefix->Append(std_path.GetUserDataDir());
              appendOSDirSlash(g_pSENCPrefix);
              g_pSENCPrefix->Append(_T("SENC"));
        }

        // S52RAZDS.RLE

        wxString plib_data;

//      If the config file contains an entry for PresentationLibraryData, use it.
//      Otherwise, default to conditionally set spot under g_pcsv_locn
        if(g_pPresLibData->IsEmpty())
        {
              plib_data = *g_pcsv_locn;
              appendOSDirSlash(&plib_data);
              plib_data.Append(_T("S52RAZDS.RLE"));
        }
        else
        {
              plib_data = *g_pPresLibData;
        }



        ps52plib = new s52plib(plib_data);

        //  If the library load failed, try looking for the s57 data elsewhere

        //  First, look in UserDataDir
/*    From wxWidgets documentation

        wxStandardPaths::GetUserDataDir
                    wxString GetUserDataDir() const
        Return the directory for the user-dependent application data files:
                    * Unix: ~/.appname
                    * Windows: C:\Documents and Settings\username\Application Data\appname
                    * Mac: ~/Library/Application Support/appname
*/

        if(!ps52plib->m_bOK)
        {
            delete ps52plib;

            wxString look_data_dir;
            look_data_dir.Append(std_path.GetUserDataDir());
            appendOSDirSlash(&look_data_dir) ;
            wxString tentative_SData_Locn = look_data_dir;
            look_data_dir.Append(_T("s57data"));

            plib_data = look_data_dir;
            appendOSDirSlash(&plib_data) ;
            plib_data.Append(_T("S52RAZDS.RLE"));

            wxLogMessage(_T("Looking for s57data in ") + look_data_dir);
            ps52plib = new s52plib(plib_data);

            if(ps52plib->m_bOK)
            {
                  *g_pcsv_locn = look_data_dir;
                  g_SData_Locn = tentative_SData_Locn;
            }
        }

        //  And if that doesn't work, look again in the original SData Location
        //  This will cover the case in which the .ini file entry is corrupted or moved

        if(!ps52plib->m_bOK)
        {
              delete ps52plib;

              wxString look_data_dir;
              look_data_dir = g_SData_Locn;
              look_data_dir.Append(_T("s57data"));

              plib_data = look_data_dir;
              appendOSDirSlash(&plib_data) ;
              plib_data.Append(_T("S52RAZDS.RLE"));

              wxLogMessage(_T("Looking for s57data in ") + look_data_dir);
              ps52plib = new s52plib(plib_data);

              if(ps52plib->m_bOK)
              {
                    *g_pcsv_locn = look_data_dir;
              }
        }



        if(ps52plib->m_bOK)
            wxLogMessage(_T("Using s57data in ") + *g_pcsv_locn);
        else
            wxLogMessage(_T("   S52PLIB Initialization failed, disabling S57 charts."));


// Todo Maybe initialize only when an s57 chart is actually opened???
        if(ps52plib->m_bOK)
              s57_initialize(*g_pcsv_locn, flog);


#endif  // S57


// Set default color scheme
        global_color_scheme = GLOBAL_COLOR_SCHEME_DAY;


//      Establish location and name of chart database
#ifdef __WXMSW__
        pChartListFileName = new wxString(_T("CHRTLIST.DAT"));
        pChartListFileName->Prepend(*pHome_Locn);

#else
        pChartListFileName = new wxString(_T(""));
        pChartListFileName->Append(std_path.GetUserDataDir());
        appendOSDirSlash(pChartListFileName) ;
        pChartListFileName->Append(_T("chartlist.dat"));
#endif


//      Establish location of Tide and Current data
        pTC_Dir = new wxString(_T("tcdata"));
        pTC_Dir->Prepend(g_SData_Locn);
        pTC_Dir->Append(wxFileName::GetPathSeparator());

        wxLogMessage(_T("Using Tide/Current data from:  ") + *pTC_Dir);




//      Establish guessed location of chart tree
        if(pInit_Chart_Dir->IsEmpty())
        {
            pInit_Chart_Dir->Append(_T("charts"));
            pInit_Chart_Dir->Prepend(g_SData_Locn);
        }


//      Establish the WorldVectorShoreline Dataset location
        pWVS_Locn = new wxString(_T("wvsdata"));
        pWVS_Locn->Prepend(g_SData_Locn);
        pWVS_Locn->Append(wxFileName::GetPathSeparator());

//      Reload the config data, to pick up any missing data class configuration info
//      e.g. s52plib, which could not be created until first config load completes
//      Think catch-22
        pConfig->LoadMyConfig(1);

        //  Override some config options for inital user startup with empty config file
        if(b_novicemode)
        {
              g_bShowOutlines = true;
        }


//  Set up the frame initial visual parameters
//      Default size, resized later
        wxSize new_frame_size(-1, -1);
        int cx, cy, cw, ch;
        ::wxClientDisplayRect(&cx, &cy, &cw, &ch);

        InitializeUserColors();

//  Create the global instance of the CommPortManager
        g_pCommMan = new ComPortManager;

//  For Windows and GTK, provide the expected application Minimize/Close bar
       long app_style = wxDEFAULT_FRAME_STYLE;

#ifdef __WXX11__
        app_style = (wxSIMPLE_BORDER | wxCLIP_CHILDREN | wxRESIZE_BORDER);

        //      Here is a specific size set for my (dsr) specific imbedded X11 environment
        new_frame_size.Set(cw, 735);
///        new_frame_size.Set(cw, ch);
#else
        if((g_nframewin_x > 100) && (g_nframewin_y > 100) && (g_nframewin_x <= cw) && (g_nframewin_y <=ch ))
            new_frame_size.Set(g_nframewin_x, g_nframewin_y);
        else
            new_frame_size.Set(cw * 7/10, ch * 7/10);
#endif


        app_style |= wxWANTS_CHARS;

// Create the main frame window
        gFrame = new MyFrame(NULL, _T("OpenCPN"), wxPoint(0, 0), new_frame_size, app_style );


//      Create Children of Frame
//              n.b.  if only one child exists, wxWindows expands the child
//                        to the parent client area automatically, (as a favor?)
//                        Here, we'll do explicit sizing on SIZE events

        cc1 =  new ChartCanvas(gFrame);                         // the chart display canvas
        if(cc1)
        {
            cc1->m_bFollow = pConfig->st_bFollow;               // set initial state
            cc1->VPoint.view_scale_ppm = initial_scale_ppm;
        }

        cc1->SetFocus();


        console = new ConsoleCanvas(gFrame);                    // the console

        stats = new StatWin(gFrame);

        pAPilot = new AutoPilotWindow(gFrame, *pNMEA_AP_Port);

#ifdef USE_WIFI_CLIENT
        pWIFI = new WIFIWindow(gFrame, *pWIFIServerName );
#endif

        pthumbwin = new ThumbWin(gFrame);

        gFrame->ApplyGlobalSettings(1, true);               // done once on init with resize

        gFrame->SetAndApplyColorScheme(global_color_scheme);

// Show the frame
        gFrame->Show(TRUE);

        if(g_bframemax)
            gFrame->Maximize(true);


//      Try to load the current chart list Data file
        ChartData = new ChartDB(gFrame);
        if(!ChartData->LoadBinary(pChartListFileName))
        {
                bDBUpdateInProgress = true;

                if(pChartDirArray->GetCount())
                {
//              Create and Save a new Chart Database based on the hints given in the config file

                        delete ChartData;
                        ChartData = new ChartDB(gFrame);

                        ChartData->Create(pChartDirArray, true);
                        ChartData->SaveBinary(pChartListFileName, pChartDirArray);

                }

                else                                            // No chart database, no config hints, so bail....
                {
                  wxLogMessage(_T("Chartlist file not found, config chart dir array is empty.  Chartlist target file is:") +
                              *pChartListFileName);

                  wxString msg1(_T("           No Charts Installed.\nPlease select chart folders in ToolBox->Charts."));

                  wxMessageDialog mdlg(gFrame, msg1, wxString(_T("OpenCPN")),wxICON_INFORMATION | wxOK );
                  int dlg_ret;
                  dlg_ret = mdlg.ShowModal();

                  gFrame->DoOptionsDialog();

                  //    As a favor to new users, poll the database and move the inital viewport so that a chart will come up.
                  double clat, clon;
                  if(ChartData->GetCentroidOfLargestScaleChart(&clat, &clon, CHART_FAMILY_RASTER))
                  {
                        gLat = clat;
                        gLon = clon;
                        gFrame->ClearbFollow();
                  }
                  else
                  {
                        if(ChartData->GetCentroidOfLargestScaleChart(&clat, &clon, CHART_FAMILY_VECTOR))
                        {
                              gLat = clat;
                              gLon = clon;
                              gFrame->ClearbFollow();
                        }
                  }

                }

                bDBUpdateInProgress = false;
        }

        pCurrentStack = new ChartStack;


//      All set to go.....

#ifndef __WXMSW__
//      Record initial memory status
        gFrame->GetMemoryStatus(mem_total, mem_initial);
        wxLogMessage(_T("MemoryStatus:  mem_total: %d kb,  mem_initial: %d kb"),
                                        mem_total, mem_initial);
#endif

//      establish GPS timeout value as multiple of frame timer
//      This will override any nonsense or unset value from the config file
        if((gsp_watchdog_timeout_ticks > 60) || (gsp_watchdog_timeout_ticks <= 0))
            gsp_watchdog_timeout_ticks = (GPS_TIMEOUT_SECONDS * 1000) / TIMER_GFRAME_1;

        wxString dogmsg;
        dogmsg.Printf(_T("GPS Watchdog Timeout is: %d sec."), gsp_watchdog_timeout_ticks);
        wxLogMessage(dogmsg);

        gGPS_Watchdog = 2;
        gHDx_Watchdog = 2;

//      Start up the ticker....
        gFrame->FrameTimer1.Start(TIMER_GFRAME_1, wxTIMER_CONTINUOUS);

   return TRUE;
}



int MyApp::OnExit()
{
      wxLogMessage(_T("opencpn::MyApp exiting cleanly...\n"));
        delete pConfig;
        delete pSelect;
        delete pSelectTC;
        delete pSelectAIS;

        delete pRouteMan;
        delete pWayPointMan;

        delete pChartDirArray;

#ifdef USE_S57
        delete ps52plib;
#endif

        delete pDummyChart;

        if(ptcmgr)
                delete ptcmgr;

        if(logger)
        {
                wxLog::SetActiveTarget(Oldlogger);
                delete logger;
        }

        delete pChartListFileName;
        delete pHome_Locn;
        delete g_pcsv_locn;
        delete g_pSENCPrefix;
        delete pTC_Dir;
        delete phost_name;
        delete pInit_Chart_Dir;
        delete pWVS_Locn;
        delete pval;

        delete pNMEADataSource;
        delete pNMEA_AP_Port;
        delete pAIS_Port;

        delete pWIFIServerName;

        delete pFontMgr;

        delete pMessageOnceArray;

        delete s_pcm93mgr;

        DeInitializeUserColors();

        delete g_pCommMan;

#ifdef USE_S57
        delete g_poRegistrar;
        CSVDeaccess(NULL);
#endif

#ifdef USE_S57
#ifdef __WXMSW__
#ifdef USE_GLU_TESS
#ifdef USE_GLU_DLL
        if(s_glu_dll_ready)
            FreeLibrary(s_hGLU_DLL);           // free the glu32.dll
#endif
#endif
#endif
#endif

//        _CrtDumpMemoryLeaks( );

        //      Restore any changed system colors
#ifdef __WXMSW__
    RestoreSystemColors();
#endif

        return TRUE;
}


void MyApp::TestSockets(void)
{
//      wxWidgets for X11, starting with version 2.6 and extending however
//      long, has a problem handling TCP/IP socket events.
//      The problem relates to wxHashTable::Next()
//      There is a workaround:  configure wxWidgets with --enable-compat24
//      Anyway, we'll test here to be sure sockets work HERE.
//      If they don't, we shall have to disable GPSD data input support


//  Create a dynamic event handler in MyApp for wxSocketServer events
        Connect( wxID_ANY, wxEVT_SOCKET, wxSocketEventHandler(MyApp::OnSocketEvent) );

//      Make a localhost server
        wxIPV4address serv_addr;
        serv_addr.Service(3001);

        s_s_sock = new wxSocketServer(serv_addr);
        s_s_sock->SetEventHandler(*this, wxID_ANY);

        s_s_sock->SetNotify(wxSOCKET_CONNECTION_FLAG);
        s_s_sock->Notify(TRUE);

// Create the test client socket
        s_t_sock = new wxSocketClient();

//      Connect() the test socket

        wxIPV4address addr;
        addr.AnyAddress();
        addr.Service(3001);

        s_socket_test_running = true;
        s_socket_test_passed = false;

        s_t_sock->Connect(addr, FALSE);       // Non-blocking connect

//    Sleep and loop for N seconds
#define SLEEP_TEST_SEC  1
        for(int is=0 ; is<SLEEP_TEST_SEC * 10 ; is++)
{
    wxMilliSleep(100);
}

// Test requires at least one pass thru event loop.  Test will finish in first MyFrame::OnTimer

}



void MyApp::OnSocketEvent(wxSocketEvent& event)
//  Used for testing wxSocketClient event handling
{

    switch(event.GetSocketEvent())
    {
        case wxSOCKET_CONNECTION :
        case wxSOCKET_INPUT:
        case wxSOCKET_OUTPUT:
        case wxSOCKET_LOST:

            s_socket_test_passed = true;
            break;
    }
}

/*
int MyApp::FilterEvent(wxEvent& event)
{
      if ( event.GetEventType() == wxEVT_CHAR )
      {
            if(cc1->Do_Hotkeys((wxKeyEvent&)event))
            return true;
      }
      return -1;
}
*/

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------


//      Frame implementation
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_CLOSE(MyFrame::OnCloseWindow)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_SIZE(MyFrame::OnSize)
  EVT_MENU(-1, MyFrame::OnToolLeftClick)
  EVT_TIMER(FRAME_TIMER_1, MyFrame::OnFrameTimer1)
  EVT_TIMER(FRAME_TC_TIMER, MyFrame::OnFrameTCTimer)
  EVT_ACTIVATE(MyFrame::OnActivate)
  EVT_COMMAND(wxID_ANY, EVT_NMEA, MyFrame::OnEvtNMEA)
  EVT_COMMAND(wxID_ANY, EVT_THREADMSG, MyFrame::OnEvtTHREADMSG)
  EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
END_EVENT_TABLE()


// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
  wxFrame(frame, -1, title, pos, size, style)  //wxSIMPLE_BORDER | wxCLIP_CHILDREN | wxRESIZE_BORDER)
      //wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER
{
        m_ulLastNEMATicktime = 0;
        m_pStatusBar = NULL;

        PrepareToolbarBitmaps();

        //      Redirect the global heartbeat timer to this frame
        FrameTimer1.SetOwner(this, FRAME_TIMER_1);

        //      Redirect the Tide/Current update timer to this frame
        FrameTCTimer.SetOwner(this, FRAME_TC_TIMER);

        //      Set up some assorted member variables
        nRoute_State = 0;
        m_bTimeIsSet = false;

        m_ptool_ct_dummy = NULL;
        m_phash = NULL;

//    Establish my children
#ifdef __WXOSX__
        if (false == ValidateSerialPortName(pNMEADataSource->mb_str(),MAX_SERIAL_PORTS))
              *pNMEADataSource = _T("NONE") ;
        if (false == ValidateSerialPortName(pAIS_Port->mb_str(),MAX_SERIAL_PORTS))
              *pAIS_Port = _T("NONE") ;
        if (false == ValidateSerialPortName(pNMEA_AP_Port->mb_str(), MAX_SERIAL_PORTS))
              *pNMEA_AP_Port = _T("NONE") ;
#endif

        //    If the selected port is the same as AIS port, override the name to force the
       //    NMEA class to expect muxed data from AIS decoder
        if(pNMEADataSource->IsSameAs(*pAIS_Port))
           g_pnmea = new NMEAWindow(ID_NMEA_WINDOW, this, _T("AIS Port (Shared)"), &m_mutexNMEAEvent );
        else
           g_pnmea = new NMEAWindow(ID_NMEA_WINDOW, this, *pNMEADataSource, &m_mutexNMEAEvent );


//        pAIS = new AIS_Decoder(ID_AIS_WINDOW, gFrame, wxString("TCP/IP:66.235.48.168"));  // a test
        g_pAIS = new AIS_Decoder(ID_AIS_WINDOW, this, *pAIS_Port, &m_mutexNMEAEvent);


        //  Create/connect a dynamic event handler slot for OCPN_NMEAEvent(s) coming from NMEA or AIS threads
        Connect(wxEVT_OCPN_NMEA, (wxObjectEventFunction)(wxEventFunction)&MyFrame::OnEvtOCPN_NMEA);

//  Initialize the Printer data structures

//wxGTK 2.8.0 enables gnomeprint by default.  No workee under KDevelop....

#ifdef __WXDEBUG
        if( wxPORT_GTK != pPlatform->GetPortId())
        {
            g_printData = new wxPrintData;
            g_printData->SetOrientation(wxLANDSCAPE);
            g_pageSetupData = new wxPageSetupDialogData;
        }
#else
        g_printData = new wxPrintData;
        g_printData->SetOrientation(wxLANDSCAPE);
        g_pageSetupData = new wxPageSetupDialogData;

#endif

        bFirstAuto = true;

 //        Establish the system icons for the frame.

#ifdef __WXMSW__
       SetIcon(wxICON(0));                // this grabs the first icon in the integrated MSW resource file
#endif

#ifdef __WXGTK__
       wxIcon app_icon(opencpn);          // This comes from opencpn.xpm inclusion above
       SetIcon(app_icon);
#endif



#ifdef __WXMSW__

//    Establish the entry points in USER32.DLL for system color control

        wxDynamicLibrary dllUser32(_T("user32.dll"));

        pSetSysColors = (SetSysColors_t)dllUser32.GetSymbol(wxT("SetSysColors"));
        pGetSysColor = (GetSysColor_t)dllUser32.GetSymbol(wxT("GetSysColor"));

        SaveSystemColors();
#endif


}

MyFrame::~MyFrame()
{
        FrameTimer1.Stop();
        delete ChartData;
        delete pCurrentStack;

//      Free the Route List
        wxRouteListNode *node = pRouteList->GetFirst();

        while(node)
        {
            Route *pRouteDelete = node->GetData();
            delete pRouteDelete;

            node = node->GetNext();
        }
        delete pRouteList;
}


void MyFrame::OnEraseBackground(wxEraseEvent& event)
{
}

void MyFrame::OnActivate(wxActivateEvent& event)
{
//    Code carefully in this method.
//    It is called in some unexpected places,
//    such as on closure of dialogs, etc.

//      Activating?
/*
    if(event.GetActive())
    {
        if(!bDBUpdateInProgress)
            if(Current_Ch)
                Current_Ch->InvalidateCache();

        Refresh(false);     // This frame Refresh() invalidates children (i.e.canvas) on MSW!!
    }
*/
    event.Skip();
}


ColorScheme MyFrame::GetColorScheme()
{
       return global_color_scheme;
}


void MyFrame::SetAndApplyColorScheme(ColorScheme cs)
{
      global_color_scheme = cs;

      SetSystemColors(cs);

      wxString SchemeName;
      switch(cs)
      {
            case GLOBAL_COLOR_SCHEME_DAY:
                  SchemeName = _T("DAY");
                  break;
            case GLOBAL_COLOR_SCHEME_DUSK:
                  SchemeName = _T("DUSK");
                  break;
            case GLOBAL_COLOR_SCHEME_NIGHT:
                  SchemeName = _T("NIGHT");
                  break;
            default:
                  SchemeName = _T("DAY");
                  break;
      }

      if(ps52plib)
             ps52plib->SetPLIBColorScheme(SchemeName);


        //Search the user color table array to find the proper hash table
      Usercolortable_index = 0;
      for ( unsigned int i=0 ; i< UserColorTableArray->GetCount() ; i++ )
      {
            colTable *ct = ( colTable * ) UserColorTableArray->Item ( i );
            if ( SchemeName.IsSameAs(*ct->tableName ))
            {
                  Usercolortable_index = i;
                  break;
            }
      }

      //    Set up a pointer to the proper hash table
      pcurrent_user_color_hash = ( ColourHash * ) UserColourHashTableArray->Item ( Usercolortable_index );

      if(cc1)
            cc1->SetColorScheme(cs);

      if(pWayPointMan)
            pWayPointMan->SetColorScheme(cs);

      if(ChartData)
            ChartData->ApplyColorSchemeToCachedCharts(cs);

      if(stats)
            stats->SetColorScheme(cs);

      if(console)
            console->SetColorScheme(cs);

      if(pRouteMan)
            pRouteMan->SetColorScheme(cs);

      if(pMarkPropDialog)
            pMarkPropDialog->SetColorScheme(cs);

      if(pRoutePropDialog)
            pRoutePropDialog->SetColorScheme(cs);

            // toh, 2009.02.08
      if(pMarkInfoDialog)
            pMarkInfoDialog->SetColorScheme(cs);

      //    For the AIS target query dialog, we must rebuild it to incorporate the style desired for the colorscheme selected
      if(g_pais_query_dialog_active)
      {
            bool b_isshown = g_pais_query_dialog_active->IsShown();
            int n_mmsi = g_pais_query_dialog_active->GetMMSI();
            if(b_isshown)
                  g_pais_query_dialog_active->Show(false);              // dismiss it


            g_pais_query_dialog_active->Close();

            g_pais_query_dialog_active = new AISTargetQueryDialog();
            g_pais_query_dialog_active->Create ( this, -1, wxT ( "AIS Target Query" ), wxPoint(g_ais_query_dialog_x, g_ais_query_dialog_y));
            g_pais_query_dialog_active->SetMMSI(n_mmsi);
            g_pais_query_dialog_active->UpdateText();
            if(b_isshown)
                  g_pais_query_dialog_active->Show();
      }

      ApplyGlobalColorSchemetoStatusBar();

      UpdateToolbar(cs);
      UpdateToolbarStatusWindow(Current_Ch, true);
}


void MyFrame::ApplyGlobalColorSchemetoStatusBar(void)
{
      if(m_pStatusBar != NULL)
      {
            m_pStatusBar->SetBackgroundColour(GetGlobalColor(_T("UIBDR")));    //UINFF
            m_pStatusBar->ClearBackground();

          //    As an optimization, if color scheme is anything other than GLOBAL_COLOR_SCHEME_DAY,
          //    adjust the status bar field styles to be simple flat fields, with no unmanageable 3-D
          //    effects.

            int sb_style;
            if((global_color_scheme == GLOBAL_COLOR_SCHEME_DAY) ||
                (global_color_scheme == GLOBAL_COLOR_SCHEME_RGB))
                  sb_style = wxSB_NORMAL;
            else
                  sb_style = wxSB_FLAT;

            int sb_styles[N_STATUS_BAR_FIELDS_MAX];
            for(int i=0 ; i<m_StatusBarFieldCount ; i++)
            {
                  if(i < N_STATUS_BAR_FIELDS_MAX)
                        sb_styles[i] = sb_style;
            }

            m_pStatusBar->SetStatusStyles(m_StatusBarFieldCount, (const int *)&sb_styles[0]);

//          m_pStatusBar->Refresh(false);
      }

}

void MyFrame::DestroyMyToolbar()
{
    delete m_ptool_ct_dummy;
    delete toolBar;
}


wxToolBar *MyFrame::CreateAToolbar()
{
   wxToolBar *tb;

    long style = 0;
    style |= wxTB_HORIZONTAL ;
    m_ptool_ct_dummy = NULL;

    tb = new wxToolBar(this, -1, wxPoint(-1, -1), wxSize(-1, -1), wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT);

    tb->SetToolBitmapSize(wxSize(32,32));
    tb->SetRows(1);


//Set up the ToolBar


//      Set up the margins, etc...
//      All this is poorly documented, and very clumsy
//      see the wx src files

//  On MSW, ToolPacking does nothing
    tb->SetToolPacking(1);
    int tool_packing = tb->GetToolPacking();


//  On MSW, ToolMargins does nothing
    wxSize defMargins = tb->GetMargins();
    tb->SetMargins(6, defMargins.y);
    wxSize tool_margin = tb->GetMargins();


    tb->SetToolSeparation(5);                  // width of separator
    int tool_sep = tb->GetToolSeparation();

//      Calculate the tool and separator pitches
    wxSize toolsize = tb->GetToolSize();

    int x = 0;                                          // running position
    int pitch_tool, pitch_sep;

    pitch_tool = toolsize.x + tool_packing + tool_margin.x;
    pitch_sep =  tool_sep   + tool_packing + tool_margin.x;
    x =tool_packing + tool_margin.x;

      // Some platform specific fixups needed here
#ifdef __WXMSW__
      pitch_tool = toolsize.x;
      pitch_sep =  tool_sep + 1;
      x = 3;
#endif

#ifdef __WXGTK__
    {
        pitch_tool += 5;  //2

      //    More hacks to this sorry class.
        if((wxMAJOR_VERSION == 2) && (wxMINOR_VERSION >= 8))
            x = 0;
    }
#endif

//    printf("toolsize.x: %d    tool_packing: %d    tool_margin.x %d  tool_sep: %d x0: %d\n", toolsize.x, tool_packing, tool_margin.x, tool_sep, x);

    //  Fetch a sample bitmap for a placeholder
    string_to_pbitmap_hash *phash;
    phash = m_phash;

    tb->AddTool( ID_ZOOMIN, _T(""), *(*phash)[wxString(_T("zoomin"))], _T(""), wxITEM_NORMAL);
    x += pitch_tool;
    tb->AddTool( ID_ZOOMOUT, _T(""), *(*phash)[wxString(_T("zoomout"))], _T(""), wxITEM_NORMAL);
    x += pitch_tool;

//    tb->AddSeparator();
//    x += pitch_sep;

    tb->AddTool( ID_STKDN, _T(""), *(*phash)[wxString(_T("scin"))], _T("Shift to Larger Scale Chart"), wxITEM_NORMAL);
    x += pitch_tool;
    tb->AddTool( ID_STKUP, _T(""),*(*phash)[wxString(_T("scout"))], _T("Shift to Smaller Scale Chart"), wxITEM_NORMAL);
    x += pitch_tool;

//    tb->AddSeparator();
//    x += pitch_sep;

    tb->AddTool( ID_ROUTE, _T(""), *(*phash)[wxString(_T("route"))], _T("Create Route"), wxITEM_NORMAL);
    x += pitch_tool;
    tb->AddTool( ID_FOLLOW, _T(""), *(*phash)[wxString(_T("follow"))], _T("Auto Follow"), wxITEM_CHECK);
    x += pitch_tool;

//    tb->AddSeparator();
 //   x += pitch_sep;

    tb->AddTool( ID_SETTINGS, _T(""), *(*phash)[wxString(_T("settings"))], _T("ToolBox"), wxITEM_NORMAL);
    x += pitch_tool;
    tb->AddTool( ID_TEXT, _T(""), *(*phash)[wxString(_T("text"))], _T("Show ENC Text"), wxITEM_CHECK);
    x += pitch_tool;

//    tb->AddSeparator();
//    x += pitch_sep;

    tb->AddTool( ID_CURRENT, _T(""), *(*phash)[wxString(_T("current"))], _T("Show Currents"), wxITEM_CHECK);
    x += pitch_tool;
    tb->AddTool( ID_TIDE, _T(""), *(*phash)[wxString(_T("tide"))], _T("Show Tides"), wxITEM_CHECK);
    x += pitch_tool;

//    tb->AddSeparator();
//    x += pitch_sep;

    if(g_bShowPrintIcon)
    {
        tb->AddTool( ID_PRINT, _T(""), *(*phash)[wxString(_T("print"))], _T("Print Chart"), wxITEM_NORMAL);
        x += pitch_tool;
        x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.
    }

       // toh, 2009.02.14
    if (g_bShowGPXIcons)
    {
          tb->AddTool( ID_GPXIMPORT, _T(""), *(*phash)[wxString(_T("gpx_import"))], _T("Import GPX file"), wxITEM_NORMAL);
          x += pitch_tool;
          x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.

          tb->AddTool( ID_GPXEXPORT, _T(""), *(*phash)[wxString(_T("gpx_export"))], _T("Export GPX file"), wxITEM_NORMAL);
          x += pitch_tool;
          x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.
    }

    if (g_bShowTrackIcon)
    {
          tb->AddTool( ID_TRACK, _T(""), *(*phash)[wxString(_T("track"))], *(*phash)[wxString(_T("track"))],
                       wxITEM_CHECK,_T("Toggle Tracking"));
          x += pitch_tool;
          x += 1;                     // now why in the heck is this necessary?????? Grrrrrrrr.
    }

    tb->AddTool( ID_COLSCHEME, _T(""), *(*phash)[wxString(_T("colorscheme"))], _T("Change Color Scheme"), wxITEM_NORMAL);
    x += pitch_tool;
    tb->AddTool( ID_HELP, _T(""), *(*phash)[wxString(_T("help"))], _T("About OpenCPN"), wxITEM_NORMAL);
    x += pitch_tool;

    // The status tool will be here
    m_statTool_pos = tb->GetToolPos(ID_HELP) + 1;

//      Create and add a dummy control, to arrange for the "exit" tool to be
//      at right margin

    int tx,ty;
    GetClientSize(&tx, &ty);                    // of the frame

    int filler_pad = tool_margin.x + tool_packing;
    int filler_width = tx - x - pitch_tool - filler_pad;

    toolbar_width_without_static = x + pitch_tool + filler_pad;     // used by Resize()

#define DUMMY_HEIGHT    40
    m_tool_dummy_size_x = filler_width;
    m_tool_dummy_size_y = DUMMY_HEIGHT;
//    printf("x: %d   filler_pad: %d  filler_width: %d  m_tool_dummy_size_x: %d \n", x, filler_pad, filler_width, m_tool_dummy_size_x);

    //  Make sure to always create a reasonable dummy
    if(m_tool_dummy_size_x <= 0)
        m_tool_dummy_size_x = 1;

    wxImage tool_image_dummy(m_tool_dummy_size_x,32);
    wxBitmap tool_bm_dummy(tool_image_dummy);

    delete m_ptool_ct_dummy;
    m_ptool_ct_dummy = new wxStaticBitmap(tb, ID_TBSTAT, tool_bm_dummy, wxPoint(2000,0),
                                        wxSize(m_tool_dummy_size_x,DUMMY_HEIGHT),
                                        wxSIMPLE_BORDER, _T("staticBitmap"));

    tb->AddControl(m_ptool_ct_dummy);

//      And add the "Exit" tool
    tb->AddTool( ID_TBEXIT, _T(""), *(*phash)[wxString(_T("exitt"))], _T("Exit OpenCPN"), wxITEM_NORMAL);

// Realize() the toolbar
    tb->Realize();


//      Set up the toggle states
    if(pConfig)
        tb->ToggleTool(ID_FOLLOW, pConfig->st_bFollow);

#ifdef USE_S57
    if((pConfig) && (ps52plib))
        if(ps52plib->m_bOK)
            tb->ToggleTool(ID_TEXT, ps52plib->GetShowS57Text());
#endif

    if (g_bShowTrackIcon)
          tb->ToggleTool(ID_TRACK, g_bTrackActive);


        SetStatusBarPane(-1);                   // don't show help on status bar

        return tb;
}


//      Some helpers functions for Toolbar support
void MyFrame::PrepareToolbarBitmaps(void)
{

#ifdef OCPN_USE_PNGICONS
    // Load up all the toolbar bitmap xpm data pointers into a hash map
    tool_xpm_hash.clear();
    tool_xpm_hash[_T("settings")]     = (char *)_img_settings;
    tool_xpm_hash[_T("zoomin")]       = (char *)_img_zoomin;
    tool_xpm_hash[_T("zoomout")]      = (char *)_img_zoomout;
    tool_xpm_hash[_T("scin")]         = (char *)_img_scin;
    tool_xpm_hash[_T("scout")]        = (char *)_img_scout;
    tool_xpm_hash[_T("tide")]         = (char *)_img_tide;
    tool_xpm_hash[_T("route")]        = (char *)_img_route;
    tool_xpm_hash[_T("current")]      = (char *)_img_current;
    tool_xpm_hash[_T("text")]         = (char *)_img_text;
    tool_xpm_hash[_T("print")]        = (char *)_img_print;
    tool_xpm_hash[_T("exitt")]        = (char *)_img_exitt;
    tool_xpm_hash[_T("follow")]       = (char *)_img_follow;
    tool_xpm_hash[_T("help")]         = (char *)_img_help;
    tool_xpm_hash[_T("colorscheme")]  = (char *)_img_colscheme;
    tool_xpm_hash[_T("gpx_import")]   = (char *)_img_gpx_import; // toh, 2009.02.14
    tool_xpm_hash[_T("gpx_export")]   = (char *)_img_gpx_export; // toh, 2009.02.14
    tool_xpm_hash[_T("track")]        = (char *)_img_track;

        //  Process all members of the XPM hashmap

    string_to_pchar_hash::iterator it;
    for( it = tool_xpm_hash.begin(); it != tool_xpm_hash.end(); ++it )
    {
          wxImage *pimg;

          wxString index = it->first;
          wxBitmap *px1 = (wxBitmap*)tool_xpm_hash[index];

//  Build Day Bitmap
          pimg = new wxImage(px1->ConvertToImage());
          BuildToolBitmap(pimg, 230, index, tool_bitmap_hash_day);
          delete pimg;

//  Build Dusk Bitmap
          pimg = new wxImage(px1->ConvertToImage());
          BuildToolBitmap(pimg, 128, index, tool_bitmap_hash_dusk);
          delete pimg;

//  Build Night Bitmap
          pimg = new wxImage(px1->ConvertToImage());
          BuildToolBitmap(pimg,  32, index, tool_bitmap_hash_night);
          delete pimg;
    }

#else
    // Load up all the toolbar bitmap xpm data pointers into a hash map
    tool_xpm_hash.clear();
    tool_xpm_hash[_T("settings")]     = (char *)settings;
    tool_xpm_hash[_T("zoomin")]       = (char *)zoomin;
    tool_xpm_hash[_T("zoomout")]      = (char *)zoomout;
    tool_xpm_hash[_T("scin")]         = (char *)scin;
    tool_xpm_hash[_T("scout")]        = (char *)scout;
    tool_xpm_hash[_T("tide")]         = (char *)tide;
    tool_xpm_hash[_T("route")]        = (char *)route;
    tool_xpm_hash[_T("current")]      = (char *)current;
    tool_xpm_hash[_T("text")]         = (char *)text;
    tool_xpm_hash[_T("print")]        = (char *)print;
    tool_xpm_hash[_T("exitt")]        = (char *)exitt;
    tool_xpm_hash[_T("follow")]       = (char *)follow;
    tool_xpm_hash[_T("help")]         = (char *)help;
    tool_xpm_hash[_T("colorscheme")]  = (char *)colscheme;
    tool_xpm_hash[_T("gpx_import")]   = (char *)gpx_import; // toh, 2009.02.14
    tool_xpm_hash[_T("gpx_export")]   = (char *)gpx_export; // toh, 2009.02.14
    tool_xpm_hash[_T("track")]        = (char *)track;


    //  Process all members of the XPM hashmap

    string_to_pchar_hash::iterator it;
    for( it = tool_xpm_hash.begin(); it != tool_xpm_hash.end(); ++it )
    {
          wxImage *pimg;

          wxString index = it->first;
          char **px1 = (char **)tool_xpm_hash[index];

//  Build Day Bitmap
          pimg = new wxImage(px1);
          BuildToolBitmap(pimg, 230, index, tool_bitmap_hash_day);
          delete pimg;

//  Build Dusk Bitmap
          pimg = new wxImage(px1);
          BuildToolBitmap(pimg, 128, index, tool_bitmap_hash_dusk);
          delete pimg;

//  Build Night Bitmap
          pimg = new wxImage(px1);
          BuildToolBitmap(pimg,  32, index, tool_bitmap_hash_night);
          delete pimg;
    }
#endif

}

void MyFrame::BuildToolBitmap(wxImage *pimg, unsigned char back_color, wxString &index, string_to_pbitmap_hash &hash)
{
        wxImage img_dup(*pimg);

        if(back_color < 200)
        {
#if wxCHECK_VERSION(2, 8, 0)
              wxImage img_dupG = img_dup.ConvertToGreyscale();
#else // replacement code for old version
              wxImage img_dupG = img_dup;
#endif
              img_dup = img_dupG;
        }

        // Substitute the mask color with a fixed background color
        unsigned char mask_r, mask_g, mask_b;
        img_dup.GetOrFindMaskColour(&mask_r, &mask_g, &mask_b);

        img_dup.Replace(mask_r, mask_g, mask_b, back_color,back_color,back_color);

        //Remove the mask from the image
        img_dup.SetMask(false);

        //  Make a bitmap
        wxBitmap tbmp(img_dup.GetWidth(),img_dup.GetHeight(),-1);
        wxMemoryDC dwxdc;
        dwxdc.SelectObject(tbmp);
        wxBitmap *ptoolBarBitmap;

#ifdef __WXMSW__
        ptoolBarBitmap = new wxBitmap(img_dup, (wxDC &)dwxdc);
#else
        ptoolBarBitmap = new wxBitmap(img_dup);
#endif

        // store it
        hash[index] = ptoolBarBitmap;
}

void MyFrame::ReSizeToolbar(void)
{

    int tx, ty;
    GetClientSize(&tx, &ty);                    // of the frame

    m_tool_dummy_size_x = tx - toolbar_width_without_static;

    if(Current_Ch)
    {
        if(m_tool_dummy_size_x != tool_dummy_size_x_last)     // only update if size has changed
        {
            UpdateToolbarStatusWindow(Current_Ch, true);
        }
    }


}

//      Update inplace the current toolbar with bitmaps corresponding to the current color scheme
void MyFrame::UpdateToolbar(ColorScheme cs)
{

    //  Select the correct bitmap hash table and background color
    switch(cs)
    {
        case GLOBAL_COLOR_SCHEME_DAY:
            m_phash = &tool_bitmap_hash_day;
            break;
        case GLOBAL_COLOR_SCHEME_DUSK:
            m_phash = &tool_bitmap_hash_dusk;
            break;
        case GLOBAL_COLOR_SCHEME_NIGHT:
            m_phash = &tool_bitmap_hash_night;
            break;
        default:
            m_phash = &tool_bitmap_hash_day;
            break;
    }

#ifdef __WXOSX__
    // RMS Problems with 2.8.3 on the mac and Destroy toolbar
    //DestroyMyToolbar();
    if (0 == toolBar)
{
      toolBar = CreateAToolbar();
      SetToolBar((wxToolBar *)toolBar);
}
#else
    DestroyMyToolbar();
    toolBar = CreateAToolbar();
    SetToolBar((wxToolBar *)toolBar);
#endif

#ifdef __WXMSW__
    wxColour back_color = GetGlobalColor(_T("GREY2"));            // Was GREY1, switched on v 1.3.4 with transparent toolbar icons
#else
    wxColour back_color = GetGlobalColor(_T("GREY2"));
#endif

    //  Set background
    toolBar->SetBackgroundColour(back_color);
    toolBar->ClearBackground();

#ifdef __WXGTK__
#ifdef ocpnUSE_GTK_OPTIMIZE
#ifndef __WXUNIVERSAL__
    //  On GTK, need to be more explicit
    //  This code changes the background(transparent) colors on the toolbar icons
    GdkColor color;

    color.red = back_color.Red() << 8;
    color.green = back_color.Green() << 8;
    color.blue = back_color.Blue() << 8;

    gtk_widget_modify_bg (GTK_WIDGET(toolBar->m_toolbar), GTK_STATE_NORMAL, &color);

//    wxToolBarToolBase *tool_base = toolBar->FindById(ID_FOLLOW);
//    wxToolBarTool* tool = wx_static_cast(wxToolBarTool*, tool_base);
//    GtkWidget *pthis_tool_item = tool_base->m_item;
//    gtk_widget_modify_bg (GTK_WIDGET(toolBar->m_toolbar), GTK_STATE_ACTIVE, &color);
#endif
#endif
#endif

    //  Re-establish toggle states
    toolBar->ToggleTool(ID_FOLLOW, cc1->m_bFollow);
    toolBar->ToggleTool(ID_CURRENT, cc1->GetbShowCurrent());
    toolBar->ToggleTool(ID_TIDE, cc1->GetbShowTide());

    return;
}





// Intercept menu commands
void MyFrame::OnExit(wxCommandEvent& event)
{
//#ifdef __WXOSX__
        quitflag++;                             // signal to the timer loop
//#endif

}

void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
      wxLogMessage(_T("opencpn::MyFrame exiting cleanly..."));

      quitflag++ ;

      FrameTimer1.Stop();

    /*
          Automatically drop an anchorage waypoint, if enabled
          On following conditions:
          1.  In "Cruising" mode, meaning that speed has at some point exceeded 3.0 kts.
          2.  Current speed is less than 0.5 kts.
          3.  Opencpn has been up at least 30 minutes
          4.  And, of course, opencpn is going down now.
    */
      if(g_bAutoAnchorMark)
      {
            wxDateTime now = wxDateTime::Now();
            wxTimeSpan uptime = now.Subtract(g_start_time);

            if((g_bCruising) && (gSog < 0.5) && (uptime.IsLongerThan(wxTimeSpan(0,30,0,0))))
            {
                  //    First, delete any single anchorage waypoint closer than 0.25 NM from this point
                  //    This will prevent clutter and database congestion....

                  wxRoutePointListNode *node = pWayPointMan->m_pWayPointList->GetFirst();
                  while(node)
                  {
                        RoutePoint *pr = node->GetData();
                        if(pr->m_MarkName.StartsWith(_T("Anchorage")))
                        {
                              double a = gLat - pr->m_lat;
                              double b = gLon - pr->m_lon;
                              double l = sqrt((a*a) + (b*b));

                              if((l * 60. * 1852.) < (.25 *1852.))
                              {
                                    pConfig->DeleteWayPoint ( pr );
                                    pSelect->DeleteSelectablePoint ( pr, SELTYPE_ROUTEPOINT );
                                    delete pr;
                                   break;
                              }
                        }

                        node = node->GetNext();
                  }

                  wxString name = now.Format();
                  name.Prepend(_T("Anchorage created "));
                  RoutePoint *pWP = new RoutePoint(gLat, gLon, wxString(_T("anchorage")), name, NULL);
                  pWP->m_bShowName = false;

                  pConfig->AddNewWayPoint(pWP, -1);       // use auto next num
            }
      }

      FrameTimer1.Stop();

      g_bframemax = IsMaximized();

      pConfig->UpdateSettings();

      delete g_printData;
      delete g_pageSetupData;

//      Explicitely Close some children, especially the ones with event handlers
//      or that call GUI methods

    cc1->Destroy();
    if(g_pnmea)
    {
          g_pnmea->Close();
          g_pnmea = NULL;                        // This will be a signal to TCP/IP socket event handler
                                            // that any remaining events in queue are to be ignored
    }

#ifdef USE_WIFI_CLIENT
    if(pWIFI)
    {
        pWIFI->Close();
        pWIFI = NULL;
    }
#endif

    if(g_pAIS)
    {
        delete g_pAIS;        //g_pAIS->Close();
        g_pAIS = NULL;
    }

    if(NULL != console)
          console->Destroy();

    stats->Destroy();

//    pthumbwin->Destroy();
    delete pthumbwin;

    delete pAPilot;

    // Delete the toolbar bitmaps

    string_to_pbitmap_hash::iterator it;

    for( it = tool_bitmap_hash_day.begin(); it != tool_bitmap_hash_day.end(); ++it )
    {
        wxBitmap *pbm = it->second;
        delete pbm;
    }

    for( it = tool_bitmap_hash_dusk.begin(); it != tool_bitmap_hash_dusk.end(); ++it )
    {
        wxBitmap *pbm = it->second;
        delete pbm;
    }

    for( it = tool_bitmap_hash_night.begin(); it != tool_bitmap_hash_night.end(); ++it )
    {
        wxBitmap *pbm = it->second;
        delete pbm;
    }

    this->Destroy();

}

void MyFrame::OnSize(wxSizeEvent& event)
{
        int x,y;
        GetClientSize(&x, &y);

//      Resize the children

        if(m_pStatusBar)
        {
              //  Maybe resize the font
              wxRect stat_box;
              m_pStatusBar->GetFieldRect(0, stat_box);
              int font_size = stat_box.width / 28;                // 30 for linux
              wxFont *pstat_font = wxTheFontList->FindOrCreateFont(font_size,
                                    wxFONTFAMILY_DEFAULT,
                                    wxFONTSTYLE_NORMAL,
                                    wxFONTWEIGHT_BOLD);
              m_pStatusBar->SetFont(*pstat_font);
        }

        int stat_height = 0;
        if(stats)
        {
              int yt = stats->GetFontHeight();
              stat_height = (yt + 6) * stats->GetRows();
        }


        int cccw, ccch;
        if(cc1)
        {
                cccw = x * 10/10;               // constrain to mod 4
                int wr = cccw/4;
                cccw = wr*4;
                cccw += 2;                              // account for simple border

                ccch = y - stat_height;

                int cur_width, cur_height;
                cc1->GetSize(&cur_width, &cur_height);
                if((cur_width != cccw) || (cur_height != ccch))
                {
                    if(Current_Ch)
                        Current_Ch->InvalidateCache();

                    cc1->SetSize(0,0,cccw, ccch);
                }


        }

        if(console)
              PositionConsole();


        if(stats)
        {
                stats->Size_X = cccw;
                stats->Size_Y = stat_height;
                stats->Pos_X = 0;
                stats->Pos_Y = ccch;
                stats->SetSize(stats->Pos_X,stats->Pos_Y,stats->Size_X, stats->Size_Y);
                stats->Show(true);
                stats->FormatStat();
                stats->Refresh(true);
        }

        if(pthumbwin)
        {
              pthumbwin->SetMaxSize(cc1->GetSize());
        }


//  Rebuild the Toolbar
    if(toolBar)
        ReSizeToolbar();

//  Update the stored window size
    GetSize(&x, &y);
    g_nframewin_x = x;
    g_nframewin_y = y;

}

void MyFrame::PositionConsole(void)
{
      //    Reposition console based on its size and chartcanvas size
      int ccx, ccy, consx, consy;
      cc1->GetSize(&ccx, &ccy);
      console->GetSize(&consx, &consy);
      console->SetSize(ccx - consx, 0, -1, -1);
}


void MyFrame::UpdateAllFonts()
{
      if(console)
      {
            console->UpdateFonts();

            //    Reposition console
            PositionConsole();
            cc1->Refresh();
      }

      if(g_pais_query_dialog_active)
      {
            g_pais_query_dialog_active->Destroy();
            g_pais_query_dialog_active = NULL;
      }
}

void MyFrame::OnToolLeftClick(wxCommandEvent& event)
{
  switch(event.GetId())
  {
    case ID_STKUP:
            DoStackUp();
            break;


    case ID_STKDN:
            DoStackDown();
            break;

    case ID_ZOOMIN:
    {
            cc1->ZoomCanvasIn();
            break;
    }

    case ID_ZOOMOUT:
    {
            cc1->ZoomCanvasOut();
            break;
    }

    case ID_ROUTE:
        {
            nRoute_State = 1;
            cc1->SetMyCursor(cc1->pCursorPencil);
            break;
        }

    case ID_FOLLOW:
        {
           TogglebFollow();
            break;
        }

#ifdef USE_S57
    case ID_TEXT:
        {

            ps52plib->SetShowS57Text(!ps52plib->GetShowS57Text());
            toolBar->ToggleTool(ID_TEXT, ps52plib->GetShowS57Text());
            Current_Ch->InvalidateCache();
            cc1->Refresh(false);
            break;
        }
#endif

    case ID_SETTINGS:
        {

            bool bnewtoolbar = !(DoOptionsDialog() == 0);

//              Apply various system settings
            ApplyGlobalSettings(true, bnewtoolbar);                 // flying update

//  The chart display options may have changed, especially on S57 ENC,
//  So, flush the cache and redraw

            if(Current_Ch)
                Current_Ch->InvalidateCache();
            cc1->ReloadVP();
            cc1->Refresh(false);
            break;
        }


    case ID_CURRENT:
        {
            if(!ptcmgr)                                                     // First time, init the manager
                ptcmgr = new TCMgr(*pTC_Dir, *pHome_Locn);

            if(ptcmgr->IsReady())
            {
                  cc1->SetbShowCurrent(!cc1->GetbShowCurrent());
                  toolBar->ToggleTool(ID_CURRENT, cc1->GetbShowCurrent());
                  Current_Ch->InvalidateCache();
            }
            else
            {
                wxLogMessage(_T("Chart1::Event...TCMgr Not Available"));
                cc1->SetbShowCurrent(false);
                toolBar->ToggleTool(ID_CURRENT, false);
            }

            if(cc1->GetbShowCurrent())
            {
                  FrameTCTimer.Start(TIMER_TC_VALUE_SECONDS * 1000, wxTIMER_CONTINUOUS);
                  cc1->SetbTCUpdate(true);                        // force immediate update
            }
            else
                  FrameTCTimer.Stop();

            cc1->Refresh(false);

            break;

        }

    case ID_TIDE:
        {
                if(!ptcmgr)                                                     // First time, init the manager
                      ptcmgr = new TCMgr(*pTC_Dir, *pHome_Locn);

                if(ptcmgr->IsReady())
                {
                      cc1->SetbShowTide(!cc1->GetbShowTide());
                      toolBar->ToggleTool(ID_TIDE, cc1->GetbShowTide());
                      Current_Ch->InvalidateCache();
                }
                else
                {
                    wxLogMessage(_T("Chart1::Event...TCMgr Not Available"));
                    cc1->SetbShowTide(false);
                    toolBar->ToggleTool(ID_TIDE, false);
                }

                if(cc1->GetbShowTide())
                {
                      FrameTCTimer.Start(TIMER_TC_VALUE_SECONDS * 1000, wxTIMER_CONTINUOUS);
                      cc1->SetbTCUpdate(true);                        // force immediate update
                }
                else
                      FrameTCTimer.Stop();

                cc1->Refresh(false);

                break;

        }

    case ID_HELP:
      {
            about *pAboutDlg = new about(this, &g_SData_Locn);
            pAboutDlg->Show();

            break;
      }

    case ID_PRINT:
      {
        DoPrint();
        break;
      }

     case ID_COLSCHEME:
        {
              ToggleColorScheme();
              break;
        }

    case ID_TBEXIT:
       {
         Close();
         break;
       }

    case ID_GPXIMPORT:       // toh, 2009.02.14
       {
             DoImportGPX();
             break;
       }
    case ID_GPXEXPORT:        // toh, 2009.02.14
       {
             DoExportGPX();
             break;
       }

    case ID_TRACK:
    {
          if(!g_bTrackActive)
                TrackOn();
          else
                TrackOff();

          break;
    }

  }         // switch
}

void MyFrame::ToggleColorScheme()
{
      ColorScheme s = GetColorScheme();
      int is = (int)s;
      is++;
      s = (ColorScheme)is;
      if(s == N_COLOR_SCHEMES)
            s = GLOBAL_COLOR_SCHEME_RGB;

      SetAndApplyColorScheme(s);
}

void MyFrame::TrackOn(void)
{
      g_bTrackActive = true;
      g_pActiveTrack = new Track();

      g_pActiveTrack->SetTrackTimer(g_TrackIntervalSeconds);
      g_pActiveTrack->SetTrackDeltaDistance(g_TrackDeltaDistance);
      g_pActiveTrack->SetTPTime(g_bTrackTime);
      g_pActiveTrack->SetTPDist(g_bTrackDistance);

      pRouteList->Append ( g_pActiveTrack );
      g_pActiveTrack->Start();

      toolBar->ToggleTool(ID_TRACK, g_bTrackActive);

}

void MyFrame::TrackOff(void)
{
      if(g_pActiveTrack)
      {
            g_pActiveTrack->Stop();

            if ( g_pActiveTrack->GetnPoints() < 2 )
                  pRouteMan->DeleteRoute ( g_pActiveTrack );
      }

      g_pActiveTrack = NULL;

      g_bTrackActive = false;

      toolBar->ToggleTool(ID_TRACK, g_bTrackActive);
}









void MyFrame::ToggleENCText(void)
{
#ifdef USE_S57
      if(ps52plib)
      {
            ps52plib->SetShowS57Text(!ps52plib->GetShowS57Text());
            toolBar->ToggleTool(ID_TEXT, ps52plib->GetShowS57Text());
            Current_Ch->InvalidateCache();
            cc1->Refresh(false);
      }

#endif
}

void MyFrame::TogglebFollow(void)
{
      if(!cc1->m_bFollow)
      {
            cc1->m_bFollow = true;
//      Warp speed jump to current position
            cc1->SetViewPoint(gLat, gLon, cc1->GetVPScale(),
                              Current_Ch->GetChartSkew() * PI / 180., FORCE_SUBSAMPLE);
            cc1->Refresh(false);
      }
      else
      {
            //    Center the screen on the GPS position, for lack of a better place
            vLat = gLat;
            vLon = gLon;

            cc1->m_bFollow = false;
      }

      toolBar->ToggleTool(ID_FOLLOW, cc1->m_bFollow);
}

void MyFrame::SetbFollow(void)
{
      cc1->m_bFollow = true;
//      Warp speed jump to current position

      if(NULL != Current_Ch)
            cc1->SetViewPoint(gLat, gLon, cc1->GetVPScale(),
                              Current_Ch->GetChartSkew() * PI / 180., FORCE_SUBSAMPLE);
      cc1->Refresh(false);
      toolBar->ToggleTool(ID_FOLLOW, cc1->m_bFollow);
}

void MyFrame::ClearbFollow(void)
{
      //    Center the screen on the GPS position, for lack of a better place
      vLat = gLat;
      vLon = gLon;
      cc1->m_bFollow = false;
      toolBar->ToggleTool(ID_FOLLOW, cc1->m_bFollow);
}

void MyFrame::ToggleChartOutlines(void)
{
      if(!g_bShowOutlines)
            g_bShowOutlines = true;
      else
            g_bShowOutlines = false;

      cc1->Refresh(false);

}

void MyFrame::ApplyGlobalSettings(bool bFlyingUpdate, bool bnewtoolbar)
{
 //             ShowDebugWindow as a wxStatusBar
        m_StatusBarFieldCount = 5;

#ifdef __WXMSW__
        UseNativeStatusBar(false);              // better for MSW, undocumented in frame.cpp
#endif

        if(pConfig->m_bShowDebugWindows)
        {
                if(!m_pStatusBar)
                {
                    m_pStatusBar = CreateStatusBar(m_StatusBarFieldCount, 0);       // No wxST_SIZEGRIP needed
                    ApplyGlobalColorSchemetoStatusBar();
                }

        }
        else
        {
                if(m_pStatusBar)
                {
                    m_pStatusBar->Destroy();
                    m_pStatusBar = NULL;
                    SetStatusBar(NULL);

//    Since the chart canvas will need to be resized, we need
//    to refresh the entire frame.
                    Refresh();
                }
        }

        SendSizeEvent();

      if(bnewtoolbar)
          UpdateToolbar(global_color_scheme);



}

int MyFrame::DoOptionsDialog()
{
      options *pSetDlg = new options(this, -1, _T("ToolBox") );

//    Set initial Chart Dir
      pSetDlg->SetInitChartDir(*pInit_Chart_Dir);

//      Pass two working pointers for Chart Dir Dialog
      pSetDlg->SetCurrentDirListPtr(pChartDirArray);
      wxArrayString *pWorkDirArray = new wxArrayString;
      pSetDlg->SetWorkDirListPtr(pWorkDirArray);

//  Grab a copy of the current NMEA source and AP Port and AIS Port
      wxString previous_NMEA_source(*pNMEADataSource);
      wxString previous_NMEA_APPort(*pNMEA_AP_Port);
      wxString previous_AIS_Port(*pAIS_Port);

//      Pass a ptr to MyConfig, for updates
      pSetDlg->SetConfigPtr(pConfig);

      pSetDlg->SetInitialSettings();

      bDBUpdateInProgress = true;

      bool bPrevPrintIcon = g_bShowPrintIcon;

      bool bPrevGPXIcon = g_bShowGPXIcons;      // toh, 2009.02.14

      bool bPrevTrackIcon = g_bShowTrackIcon;

//    Pause all of the async classes
#ifdef USE_WIFI_CLIENT
      if(pWIFI)
          pWIFI->Pause();
#endif

      if(g_pAIS)
          g_pAIS->Pause();
      if(g_pnmea)
            g_pnmea->Pause();

// And here goes the (modal) dialog
      int rr = pSetDlg->ShowModal();
      if(rr)
      {
            if(*pChartDirArray != *pWorkDirArray)
            {
                  FrameTimer1.Stop();                  // stop other asynchronous activity

                  cc1->SetCursor(wxCURSOR_WAIT);
                  Current_Ch = NULL;

                  delete pCurrentStack;
                  pCurrentStack = NULL;

                  *pChartDirArray = *pWorkDirArray;

                  delete ChartData;
                  ChartData = new ChartDB(gFrame);
                  ChartData->Update(pChartDirArray);
                  ChartData->SaveBinary(pChartListFileName, pChartDirArray);

                  pConfig->UpdateChartDirs(pChartDirArray);

                  cc1->SetCursor(wxCURSOR_ARROW);
                  FrameTimer1.Start(TIMER_GFRAME_1,wxTIMER_CONTINUOUS);
            }

            if(*pNMEADataSource != previous_NMEA_source)
            {
                  if(g_pnmea)
                        g_pnmea->Close();
                  delete g_pnmea;

                  //    If the selected port is the same as AIS port, override the name to force the
                  //    NMEA class to expect muxed data from AIS decoder
                  if(pNMEADataSource->IsSameAs(*pAIS_Port))
                     g_pnmea = new NMEAWindow(ID_NMEA_WINDOW, gFrame, _T("AIS Port (Shared)"), &m_mutexNMEAEvent );
                  else
                     g_pnmea = new NMEAWindow(ID_NMEA_WINDOW, gFrame, *pNMEADataSource, &m_mutexNMEAEvent );

                  SetbFollow();
           }


            if(*pNMEA_AP_Port != previous_NMEA_APPort)
            {
                if(pAPilot)
                    pAPilot->Close();
                delete pAPilot;
                pAPilot = new AutoPilotWindow(gFrame, *pNMEA_AP_Port );
            }

            if(*pAIS_Port != previous_AIS_Port)
            {
                delete g_pAIS;
                g_pAIS = new AIS_Decoder(ID_AIS_WINDOW, gFrame, *pAIS_Port, &m_mutexNMEAEvent );
            }

#ifdef USE_S57
            if(rr & S52_CHANGED)
            {
                // Traverse the database of open charts.
                // Finding S57 chart, execute UpdateLUPs to link objects
                // to possibly new symbology style.
                unsigned int nCache = ChartData->pChartCache->GetCount();
                for(unsigned int i=0 ; i<nCache ; i++)
                {
                    CacheEntry *pce = (CacheEntry *)(ChartData->pChartCache->Item(i));
                    ChartBase *Ch = (ChartBase *)pce->pChart;
                    if(Ch->m_ChartFamily == CHART_FAMILY_VECTOR)
                    {
                        s57chart *S57_Ch = dynamic_cast<s57chart *>(Ch);
                        S57_Ch->UpdateLUPs(S57_Ch);
                        S57_Ch->ForceEdgePriorityEvaluate();             // force re-evaluation of line segment (edge) priorities

                    }
                }
            }
#endif

            pConfig->UpdateSettings();

            if(g_pActiveTrack)
            {
                  g_pActiveTrack->SetTrackTimer(g_TrackIntervalSeconds);
                  g_pActiveTrack->SetTrackDeltaDistance(g_TrackDeltaDistance);
                  g_pActiveTrack->SetTPTime(g_bTrackTime);
                  g_pActiveTrack->SetTPDist(g_bTrackDistance);
            }
      }

//    Restart the async classes
#ifdef USE_WIFI_CLIENT
      if(pWIFI)
          pWIFI->UnPause();
#endif

      if(g_pAIS)
          g_pAIS->UnPause();
      if(g_pnmea)
            g_pnmea->UnPause();

      delete pWorkDirArray;

      bDBUpdateInProgress = false;

      delete pSetDlg;

      if((bPrevPrintIcon       != g_bShowPrintIcon)     ||
         (bPrevGPXIcon         != g_bShowGPXIcons)      ||
         (bPrevTrackIcon       != g_bShowTrackIcon)
        )
            return true;    // indicate a refresh is necessary;

      return false;
}



void MyFrame::ClearRouteTool()
{
        toolBar->ToggleTool(ID_ROUTE, false);
}


void MyFrame::DoStackDown(void)
{
      if(pCurrentStack->CurrentStackEntry)                   // not below 0
            SelectChartFromStack(pCurrentStack->CurrentStackEntry - 1);
}


void MyFrame::DoStackUp(void)
{
      if(pCurrentStack->CurrentStackEntry < pCurrentStack->nEntry-1)
            SelectChartFromStack(pCurrentStack->CurrentStackEntry + 1);
}


void MyFrame::OnFrameTimer1(wxTimerEvent& event)
{
      g_tick++;

//      Finish TCP/IP Sockets test
// See the code in MyApp::TestSockets()
/*
      if(s_socket_test_running)
      {
          s_socket_test_running  = false;
//  Clean up
          wxTheApp->Disconnect(wxID_ANY, wxID_ANY, (wxEventType)wxEVT_SOCKET, (wxObjectEventFunction)NULL, NULL, NULL);
          delete s_s_sock;
          delete s_t_sock;

          if(!s_socket_test_passed)
          {
              wxString msg1("\
This version of wxWidgets cannot process TCP/IP socket traffic.\n\
     So, NMEA TCP/IP data input support will be disabled.\n\
            You may need a wxWidgets upgrade, or\n\
    rebuild wxWidgets with --enable-compat24 config flag.");

              wxMessageDialog mdlg(gFrame, msg1, wxString("OpenCPN"),wxICON_ERROR | wxOK );
              int dlg_ret;
              dlg_ret = mdlg.ShowModal();
          }
      }
*/


//      Listen for quitflag to be set, requesting application close
      if(quitflag)
      {
          wxLogMessage(_T("Got quitflag from SIGUSR1"));
          FrameTimer1.Stop();
          Close();
          return;
      }

      if(bDBUpdateInProgress)
            return;

      FrameTimer1.Stop();

//  Update and check watchdog timer for GPS data source
      gGPS_Watchdog--;
      if(gGPS_Watchdog <= 0)
      {
          bGPSValid = false;
          if(g_nNMEADebug && (gGPS_Watchdog == 0))
                wxLogMessage(_T("   ***GPS Watchdog timeout..."));
      }

//  Update and check watchdog timer for Heading data source
      gHDx_Watchdog--;
      if(gHDx_Watchdog <= 0)
      {
            g_bHDxValid = false;
            if(g_nNMEADebug && (gHDx_Watchdog == 0))
                  wxLogMessage(_T("   ***HDx Watchdog timeout..."));
      }


//      Update the Toolbar Status window the first time watchdog times out
      if(gGPS_Watchdog == 0)
          UpdateToolbarStatusWindow(Current_Ch, false);

//    Poll for updates to Toolbar status window
//    in order to handle especially the CM93Composite chart
      if(Current_Ch)
      {
            wxString name = Current_Ch->GetName();
            wxString pub_date = Current_Ch->GetPubDate();

            if((!name.IsSameAs(m_last_reported_chart_name)) || (!pub_date.IsSameAs(m_last_reported_chart_pubdate)))
            {
                  UpdateToolbarStatusWindow(Current_Ch, false);
                  m_last_reported_chart_name = name;
                  m_last_reported_chart_pubdate = pub_date;
            }
      }


//      Update the chart database and displayed chart
      bool bnew_chart = DoChartUpdate();

//      Update the active route, if any
      if(pRouteMan->UpdateProgress())
      {
          nBlinkerTick++;
          //    This RefreshRect will cause any active routepoint to blink
          if(pRouteMan->GetpActiveRoute())
            cc1->RefreshRect(g_blink_rect, false);
      }

//      Update the memory status, and display
#ifdef MEMDIAG
#ifdef __WXGTK__

      int mem_current;
      GetMemoryStatus(mem_total, mem_current);

      if(m_pStatusBar)
      {
            char buf[40];
            sprintf(buf, "%3d/%3d/%3d", mem_initial/1024, mem_current/1024, mem_total/1024);

            SetStatusText(wxString(buf,  wxConvUTF8), STAT_FIELD_MEM5);
      }
//      printf("%d\n", mem_current);
#endif
#endif

#ifdef AUTO_CONFIG_SAVE_MINUTES
//  Possibly save the current configuration
      if(0 == (g_tick % (AUTO_CONFIG_SAVE_MINUTES * 60)))
      {
        pConfig->UpdateSettings();
      }
#endif

//  Force own-ship drawing parameters
      cc1->SetOwnShipState(SHIP_NORMAL);

        if(Current_Ch)
        {
           if(Current_Ch->Chart_Error_Factor > 0.02)
               cc1->SetOwnShipState(SHIP_LOWACCURACY);
        }

        if(!bGPSValid)
            cc1->SetOwnShipState(SHIP_INVALID);



        FrameTimer1.Start(TIMER_GFRAME_1, wxTIMER_CONTINUOUS);

//  Invalidate the ChartCanvas window appropriately
        cc1->UpdateShips();

        cc1->UpdateAIS();

        if(g_pais_query_dialog_active && g_pais_query_dialog_active->IsShown())
              g_pais_query_dialog_active->UpdateText();


        if(bnew_chart)
            cc1->Refresh(false);

        if(NULL != console)
            if(console->IsShown())
                 console->Refresh(false);

}

//    Cause refresh of active Tide/Current data, if displayed
void MyFrame::OnFrameTCTimer(wxTimerEvent& event)
{
      cc1->SetbTCUpdate(true);
      cc1->Refresh(false);
}


void MyFrame::UpdateChartStatusField(int i)
{
        char buf[80], buf1[80];
        ChartData->GetChartID(pCurrentStack, pCurrentStack->CurrentStackEntry, buf, sizeof(buf));
        sprintf (buf1, "  %d/%d", pCurrentStack->CurrentStackEntry, pCurrentStack->nEntry-1);
        strcat(buf, "  ");
        strcat(buf, buf1);
        strcat(buf, "  ");

        ChartData->GetStackChartScale(pCurrentStack, pCurrentStack->CurrentStackEntry, buf1, sizeof(buf1));
        strcat(buf, buf1);

        if(m_pStatusBar)
            SetStatusText(wxString(buf, wxConvUTF8), i);

        stats->Refresh(false);
}


void RenderShadowText(wxDC *pdc, wxFont *pFont, wxString& str, int x, int y)
{
#ifdef DrawText
#undef DrawText
#define FIXIT
#endif

        wxFont oldfont = pdc->GetFont(); // save current font

        pdc->SetFont(*pFont);
        pdc->SetTextForeground(GetGlobalColor(_T("CHGRF")));
        pdc->SetBackgroundMode(wxTRANSPARENT);

        pdc->DrawText(str, x, y+1);
        pdc->DrawText(str, x, y-1);
        pdc->DrawText(str, x+1, y);
        pdc->DrawText(str, x-1, y);

        pdc->SetTextForeground(GetGlobalColor(_T("CHBLK")));

        pdc->DrawText(str, x, y);

        pdc->SetFont(oldfont);                  // restore last font

#ifdef FIXIT
#undef FIXIT
#define DrawText DrawTextA
#endif


}

#include "wx/encconv.h"

void MyFrame::UpdateToolbarStatusWindow(ChartBase *pchart, bool bUpdate)
{
    if(NULL == pchart)
        return;

    if(m_tool_dummy_size_x <= 0)
        return;

    int iSysDescent = 0 ;
#ifdef __WXMSW__
      int font2_size = 10;
      int font3_size = 14;
#elif defined(__WXOSX__)
      int font2_size = 10;
      int font3_size = 12;
#else
      int font2_size = 12;
      int font3_size = 14;
#endif


//      Create a wxBitmap for the window
      wxBitmap tool_bm_dummy(m_tool_dummy_size_x, m_tool_dummy_size_y);
      tool_dummy_size_x_last = m_tool_dummy_size_x;       // record for change tracking during resize

      int size_x = m_tool_dummy_size_x;
      int size_y = m_tool_dummy_size_y;

//      Draw the graphics
      wxMemoryDC dc;
      dc.SelectObject(tool_bm_dummy);

// First, clear background
// Using a color depending on the state  Chart_Error_Factor
      wxBrush *p_brush;
      p_brush = wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("GREEN3")), wxSOLID);   // quiet green


      if(Current_Ch->Chart_Error_Factor > .02)                                       // X percent error
            p_brush = wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("CHYLW")), wxSOLID);   // loud yellow

      if(!bGPSValid)
      {
          if(Current_Ch->Chart_Error_Factor > .02)                                       // X percent error
                p_brush = wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("UINFO")), wxSOLID);  // orange
          else
              p_brush = wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("RED1")), wxSOLID);  // soft red
      }

      dc.SetBackground(*p_brush);
      dc.Clear();

// Show Pub date
// Get a Font
      wxFontFamily family = wxFONTFAMILY_ROMAN;
      wxString font_name(_T("Roman"));
#ifdef __WXMSW__
      family = (enum wxFontFamily)wxDEFAULT;
      font_name = _T("Arial");
#endif

      wxFont *pSWFont1;
      pSWFont1 = wxTheFontList->FindOrCreateFont(font3_size, family, wxNORMAL, wxNORMAL,
        FALSE, font_name, wxFONTENCODING_SYSTEM );

      dc.SetFont(*pSWFont1);

//      Get and show the Chart Publish Date
      wxString pub_date = pchart->GetPubDate();

      int w, h, descent;
      GetTextExtent(pub_date, &w, &h, &descent, NULL, pSWFont1);

      int date_locn_x = size_x - w - 2;
      int date_locn_y = size_y - h;

      //    GTK appears to return height as total height, including descenders.
      //    Other platforms return height as height above baseline...
#ifndef __WXGTK__
      iSysDescent = descent ;
#endif

//      RenderShadowText(&dc, pSWFont1, pub_date, date_locn_x, date_locn_y - iSysDescent);

      dc.SetFont(*pSWFont1);
      dc.DrawText(pub_date, date_locn_x, date_locn_y - iSysDescent);


//    Show File Name
      wxFont *pSWFont2;

      pSWFont2 = wxTheFontList->FindOrCreateFont(font2_size, wxDEFAULT ,wxNORMAL, wxNORMAL,
              FALSE, wxString(_T("")), wxFONTENCODING_SYSTEM );
      dc.SetFont(*pSWFont2);

//   Get and show the Chart FullPath
      wxString full_path = pchart->GetFullPath();

      int height_font_2;
      GetTextExtent(full_path, &w, &height_font_2, NULL, NULL, pSWFont2);

      dc.DrawText(full_path, 4, 0);


//    Show Chart Nice Name

//   Get and show the Chart Nice Name
      wxFont *pSWFont3;
      pSWFont3 = wxTheFontList->FindOrCreateFont(font3_size, wxDEFAULT,wxNORMAL, wxBOLD,
              FALSE, wxString(_T("Arial")), wxFONTENCODING_SYSTEM );
      dc.SetFont(*pSWFont3);


      /*************************************************************************************/
      /*        All of this name encoding has been rendered redundant with conversion      */
      /*        of opencpn to Unicode internally.  The chart name is reported as a         */
      /*        properly converted wxString, printable directly with the appropriate font. */
      /*************************************************************************************/

      /*
      //    The Chart Nice Name may be encoded with 8-bit ascii encoding set,
      //    especially some French chart names in NDI data sets.
      //    Use an Encoding Converter to ensure that the name string can be
      //    rendered in the selected font.  A simple brute force way is to
      //    convert the string to ISO8859_1, which will be renderable by
      //    all (??) installed system fonts.  There may be a more elegant way,
      //    for example, try to figure out the string encoding BEFORE specifying
      //    the font, and then FindOrCreate an appropriate font...  Todo Later.

      wxString name_possibly_intl;
      pchart->GetName(name_possibly_intl);

      wxEncodingConverter ec;
      ec.Init( wxFONTENCODING_ISO8859_2,  wxFONTENCODING_ISO8859_1, wxCONVERT_SUBSTITUTE);
      wxString name = ec.Convert(name_possibly_intl);
      */

      wxString name = pchart->GetName();

      int x_offset = 4;

//    Possibly adjust the font?
      GetTextExtent(name, &w, &h, NULL, NULL, pSWFont3);
      if(w + x_offset > date_locn_x)
      {
            dc.SetFont(*pSWFont2);
            GetTextExtent(name, &w, &h, NULL, NULL, pSWFont2);
            if(w + x_offset > date_locn_x)                   // still too long, so shorten it
            {
              wxString nameshort;
              int l = name.Len();
              l -= 3;
              while(l)              //ell
              {
                nameshort = name.Mid(0, l);
                nameshort.Append(_T("..."));
                GetTextExtent(nameshort, &w, &h, NULL, NULL, pSWFont2);
                if(w + x_offset < date_locn_x)
                  break;
                l -= 1;
              }
              dc.DrawText(nameshort, x_offset, size_y - h - iSysDescent);    // properly placed
            }
            else
                  dc.DrawText(name, x_offset, size_y - h - iSysDescent);
      }
      else
        dc.DrawText(name, x_offset, size_y - h - iSysDescent);

//   Delete the current status tool, if present
      int ct_pos = toolBar->GetToolPos(ID_TBSTAT);
      if(ct_pos != -1)
      {
//      Delete the EXIT tool
        toolBar->DeleteTool(ID_TBEXIT);

//      Delete the current status tool
//  begin rms
#ifndef __WXOSX__
        toolBar->DeleteTool(ID_TBSTAT);
#endif
// end rms
       }

//      Create the new control tool
#ifdef __WXMSW__
//      delete m_ptool_ct_dummy;        // Gets deleted by DeleteTool?
#endif
      m_ptool_ct_dummy = new wxStaticBitmap(toolBar, ID_TBSTAT, tool_bm_dummy,
                                            wxPoint(2000,10), wxSize(size_x, size_y),wxSIMPLE_BORDER, _T("staticBitmap"));
//      Insert the new control
      toolBar->InsertControl(m_statTool_pos, m_ptool_ct_dummy);

//      Re-insert the EXIT tool
      toolBar->AddTool( ID_TBEXIT, _T(""), *(*m_phash)[wxString(_T("exitt"))], _T("Exit OpenCPN"), wxITEM_NORMAL);

 //     Realize the toolbar to reflect changes
      toolBar->Realize();

//      if(bUpdate)
      SendSizeEvent();
}


void MyFrame::SelectChartFromStack(int index)
{
        if(index < pCurrentStack->nEntry)
        {
//      Open the new chart
            ChartBase *pTentative_Chart = ChartData->OpenChartFromStack(pCurrentStack, index);

            if(pTentative_Chart)
            {
                  if(Current_Ch)
                        Current_Ch->Deactivate();

                  Current_Ch = pTentative_Chart;
                  Current_Ch->Activate();

                  pCurrentStack->CurrentStackEntry = index;
            }
            else
                SetChartThumbnail(index);       // need to reset thumbnail on failed chart open



//      Update the Status Line
//            UpdateChartStatusField(0);

//      Update the Toolbar Status window
            UpdateToolbarStatusWindow(Current_Ch, false);

//      Setup the view
            float zLat, zLon;
            if(cc1->m_bFollow)
                { zLat = gLat; zLon = gLon;}
            else
                { zLat = vLat; zLon = vLon;}


//  If the cache is invalid, as in new chart load, force a fast sub-sample render
            int new_sample_mode;
            if(Current_Ch->IsCacheValid())
                new_sample_mode = CURRENT_RENDER;
            else
                new_sample_mode = FORCE_SUBSAMPLE;


            double proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / cc1->GetVPScale();

//            printf(" current scale_onscreen: %g\n",  proposed_scale_onscreen);

            if((Current_Ch->m_ChartType == CHART_TYPE_KAP) || (Current_Ch->m_ChartType == CHART_TYPE_GEO))
            {
                //  New chart is raster type
                // try to match new viewport scale to the previous view scale,
                // understanding that raster charts have binary scale requirements

                  ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *>(Current_Ch);
                  double target_scale_ppm = cc1->GetVPScale();

                  double new_scale = Cur_BSB_Ch->GetClosestValidNaturalScalePPM(target_scale_ppm);
                  proposed_scale_onscreen = cc1->GetCanvasScaleFactor() / new_scale;
            }

            else
            {

                  if(proposed_scale_onscreen > Current_Ch->GetNormalScaleMax(cc1->GetCanvasScaleFactor()))
                        proposed_scale_onscreen = Current_Ch->GetNormalScaleMax(cc1->GetCanvasScaleFactor());
                  if(proposed_scale_onscreen < Current_Ch->GetNormalScaleMin(cc1->GetCanvasScaleFactor()))
                        proposed_scale_onscreen = Current_Ch->GetNormalScaleMin(cc1->GetCanvasScaleFactor());
            }

            cc1->SetViewPoint(zLat, zLon, cc1->GetCanvasScaleFactor() / proposed_scale_onscreen,
                                  Current_Ch->GetChartSkew() * PI / 180., new_sample_mode);

            cc1->Refresh(FALSE);
        }
}

void MyFrame::SetChartThumbnail(int index)
{
        if(bDBUpdateInProgress)
            return;

        if(NULL == pCurrentStack)
            return;

        if(NULL == pthumbwin)
            return;

        if(index == -1)
        {
            wxRect thumb_rect_in_parent = pthumbwin->GetRect();;
            pthumbwin->pThumbChart = NULL;
            pthumbwin->Show(false);
            cc1->RefreshRect(thumb_rect_in_parent, FALSE);
        }

        else if(index < pCurrentStack->nEntry)
        {
                if((ChartData->GetCSChartType(pCurrentStack, index) == CHART_TYPE_KAP) ||
                                  (ChartData->GetCSChartType(pCurrentStack, index) == CHART_TYPE_GEO))
                {
                        ChartBase *new_pThumbChart = ChartData->OpenChartFromStack(pCurrentStack, index);
                        if(new_pThumbChart)         // chart opened ok
                        {

                                ThumbData *pTD = new_pThumbChart->GetThumbData(200, 200, gLat, gLon);
                                if(pTD)
                                {
                                        pthumbwin->pThumbChart = new_pThumbChart;

                                        pthumbwin->Resize();
                                        pthumbwin->Show(true);
                                        pthumbwin->Refresh(FALSE);
                                }



                                else
                                {
                                        wxString fp = ChartData->GetFullPath(pCurrentStack, index);
                                        wxString msg(_T("Chart file corrupt.. disabling this chart \n"));
                                        msg.Append(fp);
                                        wxLogMessage(msg);
                                        wxMessageDialog dlg(gFrame, msg, _T("OpenCPN Message"), wxOK);
                                        dlg.ShowModal();
                                        ChartData->DisableChart(fp);

                                        wxLogMessage(_T("chart1.cpp:SetChartThumbnail...\
                                                Could not create thumbnail"));
                                        pthumbwin->pThumbChart = NULL;
                                        pthumbwin->Show(false);
                                        cc1->Refresh(FALSE);
                                }

                        }
                        else                            // some problem opening chart
                        {
                                wxString fp = ChartData->GetFullPath(pCurrentStack, index);
                                fp.Prepend(_T("chart1.cpp:SetChartThumbnail...Could not open chart "));
                                wxLogMessage(fp);
                                pthumbwin->pThumbChart = NULL;
                                pthumbwin->Show(false);
                                cc1->Refresh(FALSE);
                        }

                }
                else
                {
                        ChartBase *new_pThumbChart = ChartData->OpenChartFromStack(pCurrentStack, index,
                                        THUMB_ONLY);

                        pthumbwin->pThumbChart = new_pThumbChart;

                        if(new_pThumbChart)
                        {
                            ThumbData *pTD = new_pThumbChart->GetThumbData(200, 200, gLat, gLon);
                            if(pTD)
                            {
                                pthumbwin->Resize();
                                pthumbwin->Show(true);
                                pthumbwin->Refresh(true);
                            }
                            else
                                pthumbwin->Show(false);

                            cc1->Refresh(FALSE);
                        }
                }
        }

}


//----------------------------------------------------------------------------------
//      DoChartUpdate
//      Create a chartstack based on current lat/lon.
//      Update Current_Ch, using either current chart, if still in stack, or
//      smallest scale new chart in stack if not.
//      Maybe Additionally, if not in bFollow mode, do NOT automatically open charts.
//      This improves useability for large scale pans
//      Return true if Current_Ch has been changed, implying need for a full redraw
//----------------------------------------------------------------------------------
bool MyFrame::DoChartUpdate(void)
{
        float tLat, tLon;
        ChartStack LastStack;
        bool bNewChart = false;
        bool bNewPiano = false;
        ChartBase *pLast_Ch = Current_Ch;
        ChartTypeEnum new_open_type;
        ChartFamilyEnum new_open_family;
        if(pLast_Ch)
        {
              new_open_type = pLast_Ch->m_ChartType;
              new_open_family = pLast_Ch->m_ChartFamily;
        }
        else
        {
              new_open_type = CHART_TYPE_KAP;
              new_open_family = CHART_FAMILY_RASTER;
        }


        bool bOpenSmallest =  bFirstAuto;
        bFirstAuto = false;                           // Auto open smallest once, on program start
        bAutoOpen = true;                             // debugging

        if(bDBUpdateInProgress)
                return false;

//      If in auto-follow mode, use the current glat,glon to build chart stack.
//      Otherwise, use vLat, vLon gotten from click on chart canvas, or other means

        if(cc1->m_bFollow == true)
        {
                tLat = gLat;
                tLon = gLon;
        }
        else
        {
                tLat = vLat;
                tLon = vLon;
        }

        //  Make sure the target stack is valid
        if(NULL == pCurrentStack)
               pCurrentStack = new ChartStack;

        // Build a chart stack based on tLat, tLon
        ChartStack *pWorkStack = new ChartStack;
        int l = ChartData->BuildChartStack(pWorkStack, tLat, tLon);

        if(l == 0)                                    // Bogus Lat, Lon?
        {
                if(NULL == pDummyChart)
                {
                    pDummyChart = new ChartDummy;
                    bNewChart = true;
                }

                if(Current_Ch)
                    if(Current_Ch->m_ChartType != CHART_TYPE_DUMMY)
                        bNewChart = true;

                Current_Ch = pDummyChart;

//    If the current viewpoint is invalid, set the default scale to something reasonable.
                double set_scale = cc1->GetVPScale();
                if(!cc1->VPoint.bValid)
                    set_scale = 1./200000.;

                cc1->SetViewPoint(tLat, tLon, set_scale, 0, CURRENT_RENDER);

        //      If the chart stack has just changed, there is new status
                if(!ChartData->EqualStacks(pWorkStack, pCurrentStack))
                {
                  bNewPiano = true;
                  bNewChart = true;
                }

        //      Copy the new (by definition empty) stack into the target stack
                ChartData->CopyStack(pCurrentStack, pWorkStack);

                goto update_finish;
        }


        //              Check to see if Chart Stack has changed
        if(!ChartData->EqualStacks(pWorkStack, pCurrentStack))
        {
        //      New chart stack, so...
                bNewPiano = true;

        //      Save a copy of the current stack
                ChartData->CopyStack(&LastStack, pCurrentStack);

        //      Copy the new stack into the target stack
                ChartData->CopyStack(pCurrentStack, pWorkStack);

        //  Is Current Chart in new stack?

                int tEntry = -1;
                if(NULL != Current_Ch)                                  // this handles startup case
                        tEntry = ChartData->GetStackEntry(pCurrentStack, Current_Ch->m_pFullPath);

                if(tEntry != -1)                // Current_Ch is in the new stack
                {
                        pCurrentStack->CurrentStackEntry = tEntry;
                        bNewChart = false;
                }

                else                            // Current_Ch is NOT in new stack
                {                                       // So, need to open a new chart
                                                        //      Find the smallest scale raster chart that opens OK

                  ChartBase *pProposed = NULL;

                  if(bAutoOpen)
                  {
                        bool search_direction = false;            // default is to search from lowest to highest

                        //    A special case:  If panning at high scale, open largest scale chart first
                        if((LastStack.CurrentStackEntry == LastStack.nEntry - 1) || (LastStack.nEntry == 0))
                              search_direction = true;

                        //    Another special case, open smallest on program start
                        if(bOpenSmallest)
                              search_direction = false;

                        pProposed = ChartData->OpenStackChartConditional(pCurrentStack, search_direction, new_open_type, new_open_family);

//    Try to open other types/families of chart in some priority
                        if(NULL == pProposed)
                              pProposed = ChartData->OpenStackChartConditional(pCurrentStack, search_direction,
                                    CHART_TYPE_CM93COMP, CHART_FAMILY_VECTOR);

                        if(NULL == pProposed)
                              pProposed = ChartData->OpenStackChartConditional(pCurrentStack, search_direction,
                                    CHART_TYPE_CM93COMP, CHART_FAMILY_RASTER);

                        bNewChart = true;

                  }     // bAutoOpen

                  else
                        pProposed = NULL;



//  If no go, then
//  Open a Dummy Chart
                  if(NULL == pProposed)
                  {
                      if(NULL == pDummyChart)
                      {
                          pDummyChart = new ChartDummy;
                          bNewChart = true;
                      }

                      if(pLast_Ch)
                         if(pLast_Ch->m_ChartType != CHART_TYPE_DUMMY)
                               bNewChart = true;

                      pProposed = pDummyChart;
                   }

// Arriving here, pProposed points to an opened chart, or NULL.
                   if(Current_Ch)
                         Current_Ch->Deactivate();
                   Current_Ch = pProposed;

                   if(Current_Ch)
                         Current_Ch->Activate();
                }   // need new chart


// Arriving here, Current_Ch is opened and OK, or NULL
                if(NULL != Current_Ch)
                {
//      Update the Status Line
//                    UpdateChartStatusField(0);

//      Setup the view using the current scale
                    double set_scale = cc1->GetVPScale();

//    If the current viewpoint is invalid, set the default scale to something reasonable.
                    if(!cc1->VPoint.bValid)
                        set_scale = 1./200000.;

                  // However, if current chart is a raster chart, set scale to a "nice" (i.e. binary) scale
                    if(Current_Ch->m_ChartFamily == CHART_FAMILY_RASTER)
                    {
                      ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *>(Current_Ch);

                      double new_scale = Cur_BSB_Ch->GetClosestValidNaturalScalePPM(set_scale);
                      set_scale = new_scale;
                    }

//  If the chart cache is invalid, as in new chart load, force a fast sub-sample render
                    int new_sample_mode = CURRENT_RENDER;
                    if(!Current_Ch->IsCacheValid())
                           new_sample_mode = FORCE_SUBSAMPLE;

                    cc1->SetViewPoint(tLat, tLon, set_scale, Current_Ch->GetChartSkew() * PI / 180., new_sample_mode);

                }
        }         // new stack

        else                                                                    // No change in Chart Stack
        {
                if(cc1->m_bFollow)
                    cc1->SetViewPoint(tLat, tLon, cc1->GetVPScale(), Current_Ch->GetChartSkew() * PI / 180., CURRENT_RENDER);
        }



update_finish:
        if(bNewChart)
            UpdateToolbarStatusWindow(Current_Ch, false);

        if(bNewPiano)
        {
            stats->FormatStat();
            stats->Refresh(true);
        }

        //  Update the ownship position on thumbnail chart, if shown
        if(pthumbwin->IsShown())
        {
              if(pthumbwin->pThumbChart)
                  if(pthumbwin->pThumbChart->UpdateThumbData(gLat, gLon))
                        pthumbwin->Refresh(TRUE);
        }

        delete pWorkStack;
        return bNewChart;
}



void MyFrame::MouseEvent(wxMouseEvent& event)
{
        int x,y;
        event.GetPosition(&x, &y);


}


//      Linux memory monitor support

bool MyFrame::GetMemoryStatus(int& mem_total, int& mem_used)
{

//      Use filesystem /proc/meminfo to determine memory status

    char *p;
    char buf[2000];
    int len;

//      Open and read the file
    int     fd = open("/proc/meminfo", O_RDONLY);

    if (fd == -1)
    {
        mem_total = 0;
        mem_used = 0;
        return false;
    }

    len = read(fd, buf, sizeof(buf) - 1);
    if (len <= 0) {
        close(fd);
        return false;
    }
    close(fd);
    buf[len] = 0;

    p = buf;

//      Parse the file contents
//      Here is an example....at least vaild for kernel 2.6
/*
MemTotal:       255684 kB
MemFree:          5996 kB
Buffers:         15260 kB
Cached:          99840 kB
SwapCached:         48 kB
Active:         176736 kB
Inactive:        45696 kB
HighTotal:           0 kB
HighFree:            0 kB
LowTotal:       255684 kB
LowFree:          5996 kB
SwapTotal:      610460 kB
SwapFree:       610412 kB
.
.
.
Hugepagesize:     4096 kB
*/

        char *s;
        char sbuf[100];
        char stoken[20];
        char skb[20];
        int sval, val_cnt;

        int m_total, m_active, m_inactive, m_buffers, m_cached, m_free;

        val_cnt = 0;

//      Loop on the whole file
        while(*p)
        {
//      Extract line by line
                s=sbuf;
                while ((*p != '\n') && *p)
                        *s++ = *p++;
                *s = 0;
                p++;                    // skip \n

//      Parse  the sbuf line
                sscanf(sbuf, "%s %d %s\n", stoken, &sval, skb);

//      Look for and extract the required numbers

                if(!strcmp(stoken, "MemTotal:"))
                {
                        m_total = sval;
                        val_cnt++;
                }
                else if(!strcmp(stoken, "Active:"))
                {
                        m_active = sval;
                        val_cnt++;
                }
                else if(!strcmp(stoken, "Inactive:"))
                {
                        m_inactive = sval;
                        val_cnt++;
                }
                else if(!strcmp(stoken, "Buffers:"))
                {
                        m_buffers = sval;
                        val_cnt++;
                }
                else if(!strcmp(stoken, "Cached:"))
                {
                        m_cached = sval;
                        val_cnt++;
                }

                else if(!strcmp(stoken, "MemFree:"))
                {
                        m_free = sval;
                        val_cnt++;
                }

                if(val_cnt == 6)
                        break;
        }

//      Do the math....

//      int m_used = m_active + m_inactive - (m_buffers + m_cached);
//      mem_used = m_used;

        //      Maybe a good estimate of actual RAM available
        mem_used = (m_total - m_free) - (m_cached + m_buffers);

        mem_total = m_total;

        return true;

}

void MyFrame::DoPrint(void)
{

    wxPrintDialogData printDialogData(* g_printData);
    printDialogData.EnablePageNumbers(false);

    wxPrinter printer(& printDialogData);

    MyPrintout printout(_T("Chart Print"));
    if (!printer.Print(this, &printout, true ))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_T("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _T("OpenCPN"), wxOK);
//        else
//            wxMessageBox(_T("Print Cancelled"), _T("OpenCPN"), wxOK);
    }
    else
    {
        (*g_printData) = printer.GetPrintDialogData().GetPrintData();
    }


// Pass two printout objects: for preview, and possible printing.
/*
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout, & printDialogData);
    if (!preview->Ok())
    {
        delete preview;
        wxMessageBox(_T("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _T("Previewing"), wxOK);
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, _T("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
*/

}

// toh, 2009.02.15
void MyFrame::DoExportGPX(void)
{
      if (pConfig)
            pConfig->ExportGPX(this);
}

// toh, 2009.02.16
void MyFrame::DoImportGPX(void)
{
      if (pConfig)
      {
            pConfig->ImportGPX(this);
            Refresh();
      }
}


//---------------------------------------------------------------------------------------------------------
//
//              Private Memory Management
//
//---------------------------------------------------------------------------------------------------------


void *x_malloc(size_t t)
{
        void *pr = malloc(t);

        //      malloc fails
        if(NULL == pr)
        {
                wxLogMessage(_T("x_malloc...malloc fails with request of %d bytes."), t);

                // Cat the /proc/meminfo file

                char *p;
                char buf[2000];
                int len;

                int     fd = open("/proc/meminfo", O_RDONLY);

                if (fd == -1)
                        exit(1);

                len = read(fd, buf, sizeof(buf) - 1);
                if (len <= 0) {
                        close(fd);
                        exit(1);
                }
                close(fd);
                buf[len] = 0;

                p=buf;
                while(*p)
                {
//                        printf("%c", *p++);
                }


                exit(0);
                return NULL;                            // for MSVC
        }

        else
        {
                if(t > malloc_max)
                {
                        malloc_max = t;
//                      wxLogMessage(_T("New malloc_max: %d", malloc_max));
                }

                return pr;                                      // good return
        }

}

void MyFrame::OnEvtTHREADMSG(wxCommandEvent & event)
{
      wxLogMessage(event.GetString());
}


void MyFrame::OnEvtOCPN_NMEA(OCPN_NMEAEvent & event)
{
      wxString sfixtime;
      bool bshow_tick = false;

      wxString str_buf = event.GetNMEAString();

      if( g_nNMEADebug && (g_total_NMEAerror_messages < g_nNMEADebug) )
      {
            g_total_NMEAerror_messages++;
            wxString msg(_T("MEH.NMEA Sentence received..."));
            msg.Append(str_buf);
            wxLogMessage(msg);
      }

      m_NMEA0183 << str_buf;
      if(m_NMEA0183.PreParse())
      {
            if(m_NMEA0183.LastSentenceIDReceived == _T("RMC"))
            {
                  if(m_NMEA0183.Parse())
                  {
                        if(m_NMEA0183.Rmc.IsDataValid == NTrue)
                        {
                              float llt = m_NMEA0183.Rmc.Position.Latitude.Latitude;
                              int lat_deg_int = (int)(llt / 100);
                              float lat_deg = lat_deg_int;
                              float lat_min = llt - (lat_deg * 100);
                              gLat = lat_deg + (lat_min/60.);
                              if(m_NMEA0183.Rmc.Position.Latitude.Northing == South)
                                    gLat = -gLat;

                              float lln = m_NMEA0183.Rmc.Position.Longitude.Longitude;
                              int lon_deg_int = (int)(lln / 100);
                              float lon_deg = lon_deg_int;
                              float lon_min = lln - (lon_deg * 100);
                              gLon = lon_deg + (lon_min/60.);
                              if(m_NMEA0183.Rmc.Position.Longitude.Easting == West)
                                    gLon = -gLon;

                              gSog = m_NMEA0183.Rmc.SpeedOverGroundKnots;
                              gCog = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;

                              gVar = m_NMEA0183.Rmc.MagneticVariation;
                              if(m_NMEA0183.Rmc.MagneticVariationDirection == West)
                                    gVar = -gVar;

                              sfixtime = m_NMEA0183.Rmc.UTCTime;

                              gGPS_Watchdog = gsp_watchdog_timeout_ticks;

                              bshow_tick = true;

                        }
                  }
            }
            else if(m_NMEA0183.LastSentenceIDReceived == _T("HDT"))
            {
                  if(m_NMEA0183.Parse())
                  {
                        gHdt = m_NMEA0183.Hdt.DegreesTrue;
                        g_bHDxValid = true;
                        gHDx_Watchdog = gsp_watchdog_timeout_ticks;
                  }
            }

            else if(m_NMEA0183.LastSentenceIDReceived == _T("HDG"))
            {
                  if(m_NMEA0183.Parse())
                  {
                        gHdt = m_NMEA0183.Hdg.MagneticSensorHeadingDegrees;
                        if(m_NMEA0183.Hdg.MagneticVariationDirection == East)
                              gHdt += m_NMEA0183.Hdg.MagneticVariationDegrees;
                        else
                              gHdt -= m_NMEA0183.Hdg.MagneticVariationDegrees;

                        g_bHDxValid = true;
                        gHDx_Watchdog = gsp_watchdog_timeout_ticks;
                  }
            }

            else if(m_NMEA0183.LastSentenceIDReceived == _T("HDM"))
            {
                  if(m_NMEA0183.Parse())
                  {
                        gHdt = m_NMEA0183.Hdm.DegreesMagnetic;
                        gHdt += gVar;

                        g_bHDxValid = true;
                        gHDx_Watchdog = gsp_watchdog_timeout_ticks;
                  }
            }

      }
      else
      {
            if( g_nNMEADebug && (g_total_NMEAerror_messages < g_nNMEADebug) )
            {
                  g_total_NMEAerror_messages++;
                  wxString msg(_T("   Unrecognized NMEA Sentence..."));
                  msg.Append(str_buf);
                  wxLogMessage(msg);
            }
      }


      PostProcessNNEA(bshow_tick, sfixtime);
}





void MyFrame::OnEvtNMEA(wxCommandEvent & event)
{
    bool bshow_tick = false;
    time_t fixtime;

    switch(event.GetExtraLong())
    {
        case EVT_NMEA_DIRECT:
        {
                wxMutexLocker stateLocker(m_mutexNMEAEvent);          // scope is this case

                GenericPosDat *pGPSData = (GenericPosDat *)event.GetClientData();
                gLat = pGPSData->kLat;
                gLon = pGPSData->kLon;
                gCog = pGPSData->kCog;
                gSog = pGPSData->kSog;
                gVar = pGPSData->kVar;

                fixtime = pGPSData->FixTime;

                bool last_bGPSValid = bGPSValid;
                bGPSValid = true;
                if(!last_bGPSValid)
                {
                    UpdateToolbarStatusWindow(Current_Ch, false);
                    cc1->Refresh(false);            // cause own-ship icon to redraw
                }

                gGPS_Watchdog = gsp_watchdog_timeout_ticks;

                bshow_tick = true;
            break;
        }

    }           // switch

    wxString sfixtime(_T(""));
    PostProcessNNEA(bshow_tick, sfixtime);

}

void MyFrame::PostProcessNNEA(bool brx_rmc, wxString &sfixtime)
{
      //    If gSog is greater than some threshold, we determine that we are "cruising"
      if(gSog > 3.0)
            g_bCruising = true;

      if(brx_rmc)
      {
            //      Maintain the validity flags
            bool last_bGPSValid = bGPSValid;
            bGPSValid = true;
            if(!last_bGPSValid)
                  UpdateToolbarStatusWindow(Current_Ch, false);


            //      Show a little heartbeat tick in StatusWindow0 on NMEA events
            //      But no faster than 10 hz.
            unsigned long uiCurrentTickCount ;
            m_MMEAeventTime.SetToCurrent() ;
            uiCurrentTickCount = m_MMEAeventTime.GetMillisecond() / 100 ;           // tenths of a second
            uiCurrentTickCount += m_MMEAeventTime.GetTicks() * 10 ;
            if(uiCurrentTickCount > m_ulLastNEMATicktime + 1)
            {
                  m_ulLastNEMATicktime = uiCurrentTickCount ;

                  if(tick_idx++ > 6)
                        tick_idx = 0;
            }
      }

//    Show gLat/gLon in StatusWindow0

      if(NULL != GetStatusBar())
      {
            char tick_buf[2];
            tick_buf[0] = nmea_tick_chars[tick_idx];
            tick_buf[1] = 0;

            wxString s1(tick_buf, wxConvUTF8);
            s1 += _T(" Ship: ");
            s1 += toSDMM(1, gLat);
            s1 += _T("   ");
            s1 += toSDMM(2, gLon);
            SetStatusText ( s1, STAT_FIELD_TICK );
      }

      wxString sogcog;
      sogcog.Printf(_T("SOG: %5.2f kts  COG: %5.0f Deg"), gSog, gCog);
      SetStatusText ( sogcog, STAT_FIELD_SOGCOG );

#ifdef ocpnUPDATE_SYSTEM_TIME
//      Use the fix time to update the local system clock, only once per session
      if((sfixtime.Len()) && s_bSetSystemTime && (m_bTimeIsSet == false))
      {
            wxDateTime Fix_Time;

            if(6 == sfixtime.Len())                   // perfectly recognised format?
            {
                  wxString a;
                  long b;
                  int hr = 0;
                  int min = 0;
                  int sec = 0;

                  a = sfixtime.Mid(0,2);
                  if(a.ToLong(&b))
                        hr = b;
                  a = sfixtime.Mid(2,2);
                  if(a.ToLong(&b))
                        min = b;
                  a = sfixtime.Mid(4,2);
                  if(a.ToLong(&b))
                        sec = b;

                  Fix_Time.Set(hr, min, sec);
            }
            wxString fix_time_format = Fix_Time.Format(_T("%Y-%m-%dT%H:%M:%S"));  // this should show as LOCAL


      //          Compare the server (fix) time to the current system time
            wxDateTime sdt;
            sdt.SetToCurrent();
            wxDateTime cwxft = Fix_Time;                  // take a copy
            wxTimeSpan ts;
            ts = cwxft.Subtract(sdt);

            int b = (ts.GetSeconds()).ToLong();

      //          Correct system time if necessary
      //      Only set the time if wrong by more than 1 minute, and less than 2 hours
      //      This should eliminate bogus times which may come from faulty GPS units

            if((abs(b) > 60) && (abs(b) < (2 * 60 * 60)))
            {

      #ifdef __WXMSW__
      //      Fix up the fix_time to convert to GMT
                  Fix_Time = Fix_Time.ToGMT();

      //    Code snippet following borrowed from wxDateCtrl, MSW

                  const wxDateTime::Tm tm(Fix_Time.GetTm());


                  SYSTEMTIME stm;
                  stm.wYear = (WXWORD)tm.year;
                  stm.wMonth = (WXWORD)(tm.mon - wxDateTime::Jan + 1);
                  stm.wDay = tm.mday;

                  stm.wDayOfWeek = 0;
                  stm.wHour = Fix_Time.GetHour();
                  stm.wMinute = tm.min;
                  stm.wSecond = tm.sec;
                  stm.wMilliseconds = 0;

                        ::SetSystemTime(&stm);            // in GMT


      #else


      //      This contortion sets the system date/time on POSIX host
      //      Requires the following line in /etc/sudoers
      //          nav ALL=NOPASSWD:/bin/date -s *

                        wxString msg;
                        msg.Printf(_T("Setting system time, delta t is %d seconds"), b);
                        wxLogMessage(msg);

                        wxString sdate(Fix_Time.Format(_T("%D")));
                        sdate.Prepend(_T("sudo /bin/date -s \""));

                        wxString stime(Fix_Time.Format(_T("%T")));
                        stime.Prepend(_T(" "));
                        sdate.Append(stime);
                        sdate.Append(_T("\""));

                        msg.Printf(_T("Linux command is:"));
                        msg += sdate;
                        wxLogMessage(msg);
                        wxExecute(sdate, wxEXEC_ASYNC);

      #endif      //__WXMSW__
                        m_bTimeIsSet = true;

            }           // if needs correction
      }               // if valid time

#endif            //ocpnUPDATE_SYSTEM_TIME
}



void MyFrame::StopSockets(void)
{
#ifdef USE_WIFI_CLIENT
    if(pWIFI)
       pWIFI->Pause();
#endif

    if(g_pnmea)
          g_pnmea->Pause();
}

void MyFrame::ResumeSockets(void)
{
#ifdef USE_WIFI_CLIENT
    if(pWIFI)
       pWIFI->UnPause();
#endif

    if(g_pnmea)
          g_pnmea->UnPause();
}

//----------------------------------------------------------------------------------------------------------
//      Application-wide CPL Error handler
//----------------------------------------------------------------------------------------------------------

void MyCPLErrorHandler( CPLErr eErrClass, int nError,
                             const char * pszErrorMsg )

{
    char msg[256];

    if( eErrClass == CE_Debug )
        snprintf(msg, 255,"CPL: %s", pszErrorMsg);
    else if( eErrClass == CE_Warning )
        snprintf(msg, 255,"CPL Warning %d: %s", nError, pszErrorMsg);
    else
        snprintf(msg, 255,"CPL ERROR %d: %s", nError, pszErrorMsg);

    wxString str(msg, wxConvUTF8);
    wxLogMessage(str);
}

//----------------------------------------------------------------------------------------------------------
//      Printing Framework Support
//----------------------------------------------------------------------------------------------------------

bool MyPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page == 0)
            DrawPageOne(dc);

        return true;
    }
    else
        return false;
}

bool MyPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void MyPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 0;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

bool MyPrintout::HasPage(int pageNum)
{
    return (pageNum == 0);
}

void MyPrintout::DrawPageOne(wxDC *dc)
{

    // Get the Size of the Chart Canvas
    int sx, sy;
    cc1->GetClientSize(&sx, &sy);                       // of the canvas

    float maxX = sx;
    float maxY = sy;

    // Let's have at least 50 device units margin
    float marginX = 50;
    float marginY = 50;

    // Add the margin to the graphic size
    maxX += (2*marginX);
    maxY += (2*marginY);

    // Get the size of the DC in pixels
    int w, h;
    dc->GetSize(&w, &h);

    // Calculate a suitable scaling factor
    float scaleX=(float)(w/maxX);
    float scaleY=(float)(h/maxY);

    // Use x or y scaling factor, whichever fits on the DC
    float actualScale = wxMin(scaleX,scaleY);

    // Calculate the position on the DC for centring the graphic
    float posX = (float)((w - (maxX*actualScale))/2.0);
    float posY = (float)((h - (maxY*actualScale))/2.0);

    // Set the scale and origin
    dc->SetUserScale(actualScale, actualScale);
    dc->SetDeviceOrigin( (long)posX, (long)posY );

//  Get the latest bitmap as rendered by the ChartCanvas
//  And Blit/scale it onto the Printer DC
    wxMemoryDC mdc;
    mdc.SelectObject(*(cc1->pscratch_bm));

    dc->Blit(0,0,cc1->pscratch_bm->GetWidth(),cc1->pscratch_bm->GetHeight(),&mdc, 0, 0);
    mdc.SelectObject(wxNullBitmap);

}

//---------------------------------------------------------------------------------------
//
//		GPS Positioning Device Detection
//
//---------------------------------------------------------------------------------------

/*
*     Enumerate all the serial ports on the system
*
*     wxArrayString *EnumerateSerialPorts(void)

*     Very system specific, unavoidably.
*/

#ifdef __WXGTK__
extern "C" int wait(int *);                     // POSIX wait() for process
#endif

wxArrayString *EnumerateSerialPorts(void)
{
      wxArrayString *preturn = new wxArrayString;

#ifdef __WXGTK__

      //    Looking for user privilege openable devices in /dev

      wxString sdev;

      for(int idev=0 ; idev < 8 ; idev++)
      {
            sdev.Printf(_T("/dev/ttyS%1d"), idev);

            int fd = open(sdev.mb_str(), O_RDWR|O_NDELAY|O_NOCTTY);
            if(fd > 0)
            {
                  /*  add to the output array  */
                  preturn->Add(wxString(sdev));
                  close(fd);
            }
      }

      for(int idev=0 ; idev < 8 ; idev++)
      {
            sdev.Printf(_T("/dev/ttyUSB%1d"), idev);

            int fd = open(sdev.mb_str(), O_RDWR|O_NDELAY|O_NOCTTY);
            if(fd > 0)
            {
                  /*  add to the output array  */
                  preturn->Add(wxString(sdev));
                  close(fd);
            }
      }

      //    Looking for BlueTooth GPS devices
      for(int idev=0 ; idev < 8 ; idev++)
      {
             sdev.Printf(_T("/dev/rfcomm%1d"), idev);

             int fd = open(sdev.mb_str(), O_RDWR|O_NDELAY|O_NOCTTY);
             if(fd > 0)
             {
                    /*  add to the output array  */
                    preturn->Add(wxString(sdev));
                    close(fd);
             }
      }



      //    A Fallback position, in case udev has failed or something.....
      if(preturn->IsEmpty())
      {
            preturn->Add( _T("/dev/ttyS0"));
            preturn->Add( _T("/dev/ttyS1"));
            preturn->Add( _T("/dev/ttyUSB0"));
            preturn->Add( _T("/dev/ttyUSB1"));
      }


#endif



#ifdef PROBE_PORTS__WITH_HELPER

/*
*     For modern Linux/(Posix??) systems, we may use
*     the system files /proc/tty/driver/serial
*     and /proc/tty/driver/usbserial to identify
*     available serial ports.
*     A complicating factor is that most (all??) linux
*     systems require root privileges to access these files.
*     We will use a helper program method here, despite implied vulnerability.
*/

char buf[256]; // enough to hold one line from serial devices list
char left_digit;
char right_digit;
int port_num;
FILE *f;

      pid_t pID = vfork();

      if (pID == 0)                // child
      {
//    Temporarily gain root privileges
            seteuid(file_user_id);

//  Execute the helper program
            execlp("ocpnhelper", "ocpnhelper", "-SB", NULL);

//  Return to user privileges
            seteuid(user_user_id);

            wxLogMessage(_T("Warning: ocpnhelper failed...."));
            _exit(0); // If exec fails then exit forked process.
       }


       wait(NULL);                  // for the child to quit

//    Read and parse the files

/*
      * see if we have any traditional ttySx ports available
*/
      f = fopen("/var/tmp/serial", "r");

      if (f != NULL)
      {
            wxLogMessage(_T("Parsing copy of /proc/tty/driver/serial..."));

            /* read in each line of the file */
            while(fgets(buf, sizeof(buf), f) != NULL)
            {
                  wxString sm(buf, wxConvUTF8);
                  sm.Prepend(_T("   "));
                  sm.Replace(_T("\n"), _T(" "));
                  wxLogMessage(sm);

                  /* if the line doesn't start with a number get the next line */
                  if (buf[0] < '0' || buf[0] > '9')
                        continue;

      /*
                  * convert digits to an int
      */
                  left_digit = buf[0];
                  right_digit = buf[1];
                  if (right_digit < '0' || right_digit > '9')
                        port_num = left_digit - '0';
                  else
                        port_num = (left_digit - '0') * 10 + right_digit - '0';

                  /* skip if "unknown" in the string */
                  if (strstr(buf, "unknown") != NULL)
                        continue;

                  /* upper limit of 15 */
                  if (port_num > 15)
                        continue;

                  /* create string from port_num  */

                  wxString s;
                  s.Printf(_T("/dev/ttyS%d"), port_num);

                  /*  add to the output array  */
                  preturn->Add(wxString(s));

            }

            fclose(f);
      }


/*
      * Same for USB ports
*/
      f = fopen("/var/tmp/usbserial", "r");

      if (f != NULL)
      {
            wxLogMessage(_T("Parsing copy of /proc/tty/driver/usbserial..."));

            /* read in each line of the file */
            while(fgets(buf, sizeof(buf), f) != NULL)
            {

                  wxString sm(buf, wxConvUTF8);
                  sm.Prepend(_T("   "));
                  sm.Replace(_T("\n"), _T(" "));
                  wxLogMessage(sm);

                  /* if the line doesn't start with a number get the next line */
                  if (buf[0] < '0' || buf[0] > '9')
                        continue;

      /*
                  * convert digits to an int
      */
                  left_digit = buf[0];
                  right_digit = buf[1];
                  if (right_digit < '0' || right_digit > '9')
                        port_num = left_digit - '0';
                  else
                        port_num = (left_digit - '0') * 10 + right_digit - '0';

                  /* skip if "unknown" in the string */
                  if (strstr(buf, "unknown") != NULL)
                        continue;

                  /* upper limit of 15 */
                  if (port_num > 15)
                        continue;

                  /* create string from port_num  */

                  wxString s;
                  s.Printf(_T("/dev/ttyUSB%d"), port_num);

                  /*  add to the output array  */
                  preturn->Add(wxString(s));

            }

            fclose(f);
      }


      //    As a fallback, in case seteuid doesn't work....
      //    provide some defaults
      //    This is currently the case for GTK+, which
      //    refuses to run suid.  sigh...

      if(preturn->IsEmpty())
      {
            preturn->Add( _T("/dev/ttyS0"));
            preturn->Add( _T("/dev/ttyS1"));
            preturn->Add( _T("/dev/ttyUSB0"));
            preturn->Add( _T("/dev/ttyUSB1"));
      }

//    Clean up the temporary files created by helper.
      pid_t cpID = vfork();

      if (cpID == 0)                // child
      {
//    Temporarily gain root privileges
            seteuid(file_user_id);

//  Execute the helper program
            execlp("ocpnhelper", "ocpnhelper", "-U", NULL);

//  Return to user privileges
            seteuid(user_user_id);
            _exit(0); // If exec fails then exit forked process.
      }

#endif      // __WXGTK__

#ifdef __WXOSX__
#include "macutils.h"
      char* paPortNames[MAX_SERIAL_PORTS] ;
      int iPortNameCount ;

      memset(paPortNames,0x00,sizeof(paPortNames)) ;
      iPortNameCount = FindSerialPortNames(&paPortNames[0],MAX_SERIAL_PORTS) ;
      for (int iPortIndex=0;iPortIndex<iPortNameCount;iPortIndex++)
      {
            preturn->Add( _T(paPortNames[iPortIndex]));
            free(paPortNames[iPortIndex]) ;
      }
#endif      //__WXOSX__


#ifdef __WXMSW__
/*************************************************************************
 * Windows provides no system level enumeration of available serial ports
 * There are several ways of doing this.
 *
 *************************************************************************/

#include <windows.h>



      //    Method 1:  Use GetDefaultCommConfig()
      // Try first 16 possible COM ports, check for a default configuration
      for (int i=1; i<16; i++)
      {
            char s[20];
            sprintf(s, "COM%d", i);

            COMMCONFIG cc;
            DWORD dwSize = sizeof(COMMCONFIG);
            if (GetDefaultCommConfig(s, &cc, &dwSize))
                  preturn->Add(wxString(s));
      }


#if 0
      // Method 2:  Use FileOpen()
      // Try all 255 possible COM ports, check to see if it can be opened, or if
      // not, that an expected error is returned.

      BOOL bFound;
      for (int j=1; j<256; j++)
{
      char s[20];
      sprintf(s, "\\\\.\\COM%d", j);

          // Open the port tentatively
      BOOL bSuccess = FALSE;
      HANDLE hComm = ::CreateFile(s, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

            //  Check for the error returns that indicate a port is there, but not currently useable
      if (hComm == INVALID_HANDLE_VALUE)
      {
            DWORD dwError = GetLastError();

            if (dwError == ERROR_ACCESS_DENIED ||
                dwError == ERROR_GEN_FAILURE ||
                dwError == ERROR_SHARING_VIOLATION ||
                dwError == ERROR_SEM_TIMEOUT)
                  bFound = TRUE;
      }
      else
      {
            bFound = TRUE;
            CloseHandle(hComm);
      }

      if (bFound)
            preturn->Add(wxString(s));
}
#endif

//    Search for (any?) Garmin device on Windows platforms

      HDEVINFO hdeviceinfo = INVALID_HANDLE_VALUE;
      SP_DEVICE_INTERFACE_DATA deviceinterface;

 //     wxLogMessage(_T("In EnumerateSerialPorts(), searching for Garmin DeviceInterface..."));

      hdeviceinfo = SetupDiGetClassDevs( (GUID *) &GARMIN_DETECT_GUID,
                                                      NULL, NULL,
                                                      DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

      if (hdeviceinfo != INVALID_HANDLE_VALUE)
            wxLogMessage(_T("Found Garmin USB Driver."));


    deviceinterface.cbSize = sizeof(deviceinterface);

    if (SetupDiEnumDeviceInterfaces(hdeviceinfo,
                                                      NULL,
                                                      (GUID *) &GARMIN_DETECT_GUID,
                                                      0,
                                                      &deviceinterface))
      {
            wxLogMessage(_T("Found Garmin Device."));

            preturn->Add(_T("GARMIN"));         // Add generic Garmin selectable device
            g_bGarminPersistance = true;        // And record the existance

      }

#endif      //__WXMSW__

      return preturn;
}

 void appendOSDirSlash(wxString* pString)
 {
       wxChar sep = wxFileName::GetPathSeparator();
       if (pString->Last() != sep)
         pString->Append(sep);
 }


/*************************************************************************
 * Global color management routines
 *
 *************************************************************************/


wxColour GetGlobalColor(wxString colorName)
{
      wxColour ret_color;
      //    Use the S52 Presentation library if present
      if(ps52plib)
      {
            ret_color = ps52plib->S52_getwxColour(colorName);

            if(!ret_color.Ok())           //261 likes Ok(), 283 likes IsOk()...
            {
                  if(NULL != pcurrent_user_color_hash)
                        ret_color = ( *pcurrent_user_color_hash ) [colorName];
            }
      }

      else
      {
            if(NULL != pcurrent_user_color_hash)
                  ret_color = ( *pcurrent_user_color_hash ) [colorName];
      }

      //    Default
      if(!ret_color.Ok())
            ret_color.Set(128,128,128);  // Simple Grey

      return ret_color;
}


static const char *usercolors[] = {
"Table:DAY",
"GREEN1;120;255;120;",
"GREEN2; 45;150; 45;",
"GREEN3;200;220;200;",
"GREEN4;  0;255;  0;",
"BLUE1; 170;170;255;",
"BLUE2;  45; 45;170;",
"BLUE3;   0;  0;255;",
"GREY1; 150;150;150;",
"GREY2; 230;230;230;",
"RED1;  220;200;200;",
"UBLCK;   0;  0;  0;",
"UWHIT; 255;255;255;",
"URED;  255;  0;  0;",
"UGREN;   0;255;  0;",
"YELO1; 243;229; 47;",
"YELO2; 128; 80;  0;",
"DILG0; 238;239;242;",              // Dialog Background white
"DILG1; 212;208;200;",              // Dialog Background
"DILG2; 255;255;255;",              // Control Background
"DILG3;   0;  0;  0;",              // Text
"UDKRD; 124; 16;  0;",

"Table:DUSK",
"GREEN1; 60;128; 60;",
"GREEN2; 22; 75; 22;",
"GREEN3; 80;100; 80;",
"GREEN4;  0;128;  0;",
"BLUE1;  80; 80;160;",
"BLUE2;  30; 30;120;",
"BLUE3;   0;  0;128;",
"GREY1; 100;100;100;",
"GREY2; 128;128;128;",
"RED1;  150;100;100;",
"UBLCK;   0;  0;  0;",
"UWHIT; 255;255;255;",
"URED;  120; 54; 11;",
"UGREN;  35;110; 20;",
"YELO1; 120;115; 24;",
"YELO2;  64; 40;  0;",
"DILG0; 110;110;110;",              // Dialog Background
"DILG1; 110;110;110;",              // Dialog Background
"DILG2; 100;100;100;",              // Control Background
"DILG3; 130;130;130;",              // Text
"UDKRD;  80;  0;  0;",

"Table:NIGHT",
"GREEN1; 30; 80; 30;",
"GREEN2; 15; 60; 15;",
"GREEN3; 12; 23;  9;",
"GREEN4;  0;100;  0;",
"BLUE1;  60; 60;100;",
"BLUE2;  22; 22; 85;",
"BLUE3;   0;  0; 40;",
"GREY1;  64; 64; 64;",
"GREY2;  64; 64; 64;",
"RED1;  100; 50; 50;",
"UWHIT; 255;255;255;",
"UBLCK;   0;  0;  0;",
"URED;   60; 27;  5;",
"UGREN;  17; 55; 10;",
"YELO1;  60; 65; 12;",
"YELO2;  32; 20;  0;",
"DILG0;  80; 80; 80;",              // Dialog Background
"DILG1;  80; 80; 80;",              // Dialog Background
"DILG2;  52; 52; 52;",              // Control Background
"DILG3;  65; 65; 65;",              // Text
"UDKRD;  50;  0;  0;",

"*****"
};


int get_static_line(char *d, const char **p, int index, int n)
{
      if(!strcmp(p[index], "*****"))
            return 0;

      strncpy(d, p[index], n);
      return strlen(d);
}

void InitializeUserColors(void)
{
      const char **p = usercolors;
      char buf[80];
      int index = 0;
      char TableName[20];
      colTable *ctp;
      colTable *ct;
      int colIdx = 0;
      int R,G,B;

      UserColorTableArray = new wxArrayPtrVoid;
      UserColourHashTableArray = new wxArrayPtrVoid;

      //    Create 3 color table entries
      ct = new colTable;
      ct->tableName = new wxString ( _T("DAY"));
      ct->color     = new wxArrayPtrVoid;
      UserColorTableArray->Add ( ( void * ) ct );

      ct = new colTable;
      ct->tableName = new wxString ( _T("DUSK"));
      ct->color     = new wxArrayPtrVoid;
      UserColorTableArray->Add ( ( void * ) ct );

      ct = new colTable;
      ct->tableName = new wxString ( _T("NIGHT"));
      ct->color     = new wxArrayPtrVoid;
      UserColorTableArray->Add ( ( void * ) ct );


      while((get_static_line(buf, p, index,80)))
      {
            if(!strncmp(buf, "Table", 5))
            {
                  sscanf(buf, "Table:%s", TableName);

                  for(unsigned int it=0 ; it < UserColorTableArray->GetCount() ; it++)
                  {
                        ctp = (colTable *)(UserColorTableArray->Item(it));
                        if(!strcmp(TableName, ctp->tableName->mb_str()))
                        {
                              ct = ctp;
                              colIdx = 0;
                              break;
                        }
                  }

            }
            else
            {
                  char name[80];
                  int j=0;
                  while(buf[j] != ';')
                  {
                        name[j] = buf[j];
                        j++;
                  }
                  name[j] = 0;

                  S52color *c = new S52color;
                  strcpy(c->colName, name);


                  sscanf(&buf[j], ";%i;%i;%i", &R, &G, &B);
                  c->R = (char)R;
                  c->G = (char)G;
                  c->B = (char)B;

                  ct->color->Add ( c );

            }

           index ++;
      }

      //    Now create the Hash tables

            for ( unsigned int its=0 ; its < UserColorTableArray->GetCount() ; its++ )
            {
                  ColourHash *phash = new ColourHash;
                  UserColourHashTableArray->Add ( ( void * ) phash );

                  colTable *ctp = ( colTable * ) ( UserColorTableArray->Item ( its ) );

                  for ( unsigned int ic=0 ; ic < ctp->color->GetCount() ; ic++ )
                  {
                        S52color *c2 = ( S52color * ) ( ctp->color->Item ( ic ) );

                        wxColour c ( c2->R, c2->G, c2->B );
                        wxString key ( c2->colName, wxConvUTF8 );
                        ( *phash ) [key] = c;

                  }
            }

            //    Establish a default hash table pointer
            //    in case a color is needed before ColorScheme is set
            pcurrent_user_color_hash = (ColourHash *)UserColourHashTableArray->Item(0);
}

void DeInitializeUserColors(void)
{
      unsigned int i;
      for( i = 0 ; i< UserColorTableArray->GetCount() ; i++)
      {
            colTable *ct = (colTable *)UserColorTableArray->Item(i);

            for(unsigned int j = 0 ; j<ct->color->GetCount() ; j++)
            {
                  S52color *c = (S52color *)ct->color->Item(j);
                  delete c;                     //color
            }

            delete ct->tableName;               // wxString
            delete ct->color;                   // wxArrayPtrVoid

            delete ct;                          // colTable
      }

      delete UserColorTableArray;

      for( i = 0 ; i< UserColourHashTableArray->GetCount() ; i++)
      {
            ColourHash *phash = (ColourHash *)UserColourHashTableArray->Item(i);
            delete phash;
      }

      delete UserColourHashTableArray;

}


#ifdef __WXMSW__
void SaveSystemColors()
{
      color_3dface = pGetSysColor(COLOR_3DFACE);
      color_3dhilite = pGetSysColor(COLOR_3DHILIGHT);
      color_3dshadow = pGetSysColor(COLOR_3DSHADOW);
      color_3ddkshadow = pGetSysColor(COLOR_3DDKSHADOW);
      color_3dlight = pGetSysColor(COLOR_3DLIGHT);
	  color_activecaption = pGetSysColor(COLOR_ACTIVECAPTION);
      color_gradientactivecaption = pGetSysColor(27); //COLOR_3DLIGHT);
      color_captiontext = pGetSysColor(COLOR_CAPTIONTEXT);
      color_windowframe = pGetSysColor(COLOR_WINDOWFRAME);
      color_inactiveborder = pGetSysColor(COLOR_INACTIVEBORDER);

}

void RestoreSystemColors()
{
      int element[20];
      int rgbcolor[20];
      int i=0;

      element[i] = COLOR_3DFACE;
      rgbcolor[i] = color_3dface;
      i++;

      element[i] = COLOR_3DHILIGHT;
      rgbcolor[i] = color_3dhilite;
      i++;

      element[i] = COLOR_3DSHADOW;
      rgbcolor[i] = color_3dshadow;
      i++;

      element[i] = COLOR_3DDKSHADOW;
      rgbcolor[i] = color_3ddkshadow;
      i++;

      element[i] = COLOR_3DLIGHT;
      rgbcolor[i] = color_3dlight;
      i++;

      element[i] = COLOR_ACTIVECAPTION;
      rgbcolor[i] = color_activecaption;
      i++;

      element[i] = 27; //COLOR_GRADIENTACTIVECAPTION;
      rgbcolor[i] = color_gradientactivecaption;
      i++;

      element[i] = COLOR_CAPTIONTEXT;
      rgbcolor[i] = color_captiontext;
      i++;

      element[i] = COLOR_WINDOWFRAME;
      rgbcolor[i] = color_windowframe;
      i++;

      element[i] = COLOR_INACTIVEBORDER;
      rgbcolor[i] = color_inactiveborder;
      i++;

      pSetSysColors(i, (unsigned long *)&element[0], (unsigned long *)&rgbcolor[0]);

}

#endif

void SetSystemColors ( ColorScheme cs )
{
//---------------
#ifdef __WXMSW__
        int element[20];
        int rgbcolor[20];
        if ( ( GLOBAL_COLOR_SCHEME_DUSK == cs ) || ( GLOBAL_COLOR_SCHEME_NIGHT == cs ) )
        {
                int i=0;
                element[i] = COLOR_3DFACE;
                rgbcolor[0] = 0x00404040;
                i++;

                element[i] = COLOR_3DHILIGHT;
                rgbcolor[i] = 0x00505050;
                i++;

                element[i] = COLOR_3DSHADOW;
                rgbcolor[i] = 0x00505050;
                i++;

                element[i] = COLOR_3DDKSHADOW;
                rgbcolor[i] = 0x00505050;
                i++;

                element[i] = COLOR_3DLIGHT;
                rgbcolor[i] = 0x00505050;
                i++;

                element[i] = COLOR_ACTIVECAPTION;
                rgbcolor[i] = 0x00505050;
                i++;

                element[i] = 27; //COLOR_GRADIENTACTIVECAPTION;
                rgbcolor[i] = 0x00555555;
                i++;

                element[i] = COLOR_CAPTIONTEXT;
                rgbcolor[i] = 0x00606060;
                i++;

                element[i] = COLOR_WINDOWFRAME;
                rgbcolor[i] = 0x00303030;
                i++;

                element[i] = COLOR_INACTIVEBORDER;
                rgbcolor[i] = 0x00303030;
                i++;

                pSetSysColors ( i, ( unsigned long * ) &element[0], ( unsigned long * ) &rgbcolor[0] );


        }
        else
        {
                int i=0;
                element[i] = COLOR_3DFACE;
                rgbcolor[i] = color_3dface;
                i++;

                element[i] = COLOR_3DHILIGHT;
                rgbcolor[i] = color_3dhilite;
                i++;

                element[i] = COLOR_3DSHADOW;
                rgbcolor[i] = color_3dshadow;
                i++;

                element[i] = COLOR_3DDKSHADOW;
                rgbcolor[i] = color_3ddkshadow;
                i++;

                element[i] = COLOR_3DLIGHT;
                rgbcolor[i] = color_3dlight;
                i++;

                element[i] = COLOR_ACTIVECAPTION;
                rgbcolor[i] = color_activecaption;
                i++;

                element[i] = 27; //COLOR_GRADIENTACTIVECAPTION;
                rgbcolor[i] = color_gradientactivecaption;
                i++;

                element[i] = COLOR_CAPTIONTEXT;
                rgbcolor[i] = color_captiontext;
                i++;

                element[i] = COLOR_WINDOWFRAME;
                rgbcolor[i] = color_windowframe;
                i++;

                element[i] = COLOR_INACTIVEBORDER;
                rgbcolor[i] = color_inactiveborder;
                i++;

                pSetSysColors ( i, ( unsigned long * ) &element[0], ( unsigned long * ) &rgbcolor[0] );

        }
#endif
}



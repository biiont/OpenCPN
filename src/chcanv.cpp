/******************************************************************************
 * $Id: chcanv.cpp,v 1.11 2006/10/08 14:15:00 dsr Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Chart Canvas
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
 * $Log: chcanv.cpp,v $
 * Revision 1.11  2006/10/08 14:15:00  dsr
 * no message
 *
 * Revision 1.10  2006/10/08 14:40:49  dsr
 * *** empty log message ***
 *
 * Revision 1.9  2006/10/08 13:49:14  dsr
 * no message
 *
 * Revision 1.8  2006/10/08 04:37:38  dsr
 * no message
 *
 * Revision 1.7  2006/10/08 02:40:58  dsr
 * *** empty log message ***
 *
 * Revision 1.6  2006/10/08 00:36:44  dsr
 * no message
 *
 * Revision 1.5  2006/10/07 03:50:27  dsr
 * *** empty log message ***
 *
 * Revision 1.4  2006/10/05 03:48:40  dsr
 * ocpcursor update
 *
 * Revision 1.3  2006/10/01 03:22:58  dsr
 * no message
 *
 * Revision 1.2  2006/09/21 01:37:36  dsr
 * Major refactor/cleanup
 *
 * Revision 1.1.1.1  2006/08/21 05:52:19  dsr
 * Initial import as opencpn, GNU Automake compliant.
 *
 * Revision 1.8  2006/08/04 11:42:01  dsr
 * no message
 *
 * Revision 1.7  2006/07/28 20:35:28  dsr
 * Skew chart pan, cleanup
 *
 * Revision 1.6  2006/06/15 02:41:57  dsr
 * Cleanup
 *
 * Revision 1.5  2006/06/02 02:11:46  dsr
 * Implement PolyGeo Query
 *
 * Revision 1.4  2006/05/28 00:49:35  dsr
 * Floats to Doubles in ViewPort
 *
 * Revision 1.3  2006/05/19 19:20:44  dsr
 * New S57 viewport logic, incomplete....
 *
 * Revision 1.2  2006/04/23 03:55:00  dsr
 * Add S57 Query
 *
 * Revision 1.1.1.1  2006/04/19 03:23:28  dsr
 * Rename/Import to OpenCPN
 *
 * Revision 1.14  2006/04/19 00:42:39  dsr
 * Update some georeferencing errors in vector charts
 *
 * Revision 1.13  2006/03/25 20:28:51  dsr
 * Revise TC logic in OnPaint for performance
 * Correct for error in wxDateTime in wx2.6.1
 *
 * Revision 1.12  2006/03/16 03:07:59  dsr
 * Cleanup tabs
 *
 * Revision 1.11  2006/03/13 05:48:34  dsr
 * Cleanup
 *
 * Revision 1.10  2006/03/13 05:43:23  dsr
 * Cleanup
 *
 * Revision 1.9  2006/03/13 05:04:55  dsr
 * Disable rescale thread
 *
 * Revision 1.8  2006/03/04 21:19:58  dsr
 * Implement thread-based chart rescale logic
 *
 * Revision 1.7  2006/02/24 03:03:32  dsr
 * Cleanup cursor creation
 *
 * Revision 1.6  2006/02/23 01:36:34  dsr
 * Cleanup, especially cursors and chart skew
 *
 * Revision 1.5  2006/02/10 03:18:35  dsr
 * *** empty log message ***
 *
 *
 */


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/image.h"

#include "dychart.h"

#include <wx/listimpl.cpp>

#include "chcanv.h"

#include "routeman.h"
#include "navutil.h"
#include "concanv.h"
#include "thumbwin.h"
#include "chartdb.h"
#include "wvschart.h"
#include "georef.h"
#include "chartimg.h"
#include "chart1.h"

#ifdef USE_S57
#include "s57chart.h"               // for ArrayOfS57Obj
#include "s52plib.h"
#endif


extern ChartBase        *Current_Ch;
extern float            gLat, gLon, gCog, gSog;
extern float            vLat, vLon;
extern ChartDB          *ChartData;
extern int              CurrentStackEntry;
extern bool             bDBUpdateInProgress;


extern "C" void toDMS(double a, char *bufp, int bufplen);
extern "C" void toDMM(double a, char *bufp, int bufplen);
extern "C" void DegToUTM(float lat, float lon, char *zone, float *x, float *y, float lon0);
extern "C" void UTMtoDeg(double long0, short southernHemisphere, double x, double y, double *lat, double *lon);

extern ConsoleCanvas    *console;

extern RouteList        *pRouteList;
extern MyConfig         *pConfig;
extern Select           *pSelect;
extern wxToolBarBase    *toolBar;
extern Routeman         *pRouteMan;
extern ThumbWin         *pthumbwin;
extern TCMgr            *ptcmgr;
extern Select           *pSelectTC;


extern bool             bDrawCurrentValues;
extern wxString         *pWVS_Locn;

#ifdef USE_S57
extern s52plib          *ps52plib;
#endif

extern bool             bGPSValid;
extern bool             g_bShowOutlines;

CPL_CVSID("$Id: chcanv.cpp,v 1.11 2006/10/08 14:15:00 dsr Exp $");


//  These are xpm images used to make cursors for this class.
//  The relevant static identifying label is the same as the file name
//  e.g. down.xpm contains a line .....static char *down[]....

#include "bitmaps/down.xpm"
#include "bitmaps/up.xpm"
#include "bitmaps/left.xpm"
#include "bitmaps/right.xpm"
#include "bitmaps/pencil.xpm"

//    Constants for right click menus
enum
{
      ID_DEF_MENU_MAX_DETAIL,
      ID_DEF_MENU_SCALE_IN,
      ID_DEF_MENU_SCALE_OUT,
      ID_DEF_MENU_QUERY,
      ID_RT_MENU_ACTIVATE,
      ID_RT_MENU_DEACTIVATE,
      ID_RT_MENU_INSERT,
      ID_RT_MENU_APPEND,
      ID_RT_MENU_DELETE,
      ID_RT_MENU_DELPOINT,
      ID_RT_MENU_ACTPOINT,
      ID_RT_MENU_DEACTPOINT,
      ID_RT_MENU_ACTNXTPOINT,
      ID_RC_MENU_SCALE_IN,
      ID_RC_MENU_SCALE_OUT,
      ID_RC_MENU_ZOOM_IN,
      ID_RC_MENU_ZOOM_OUT,
      ID_RC_MENU_FINISH
};


//------------------------------------------------------------------------------
//    ChartCanvas Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ChartCanvas, wxWindow)
  EVT_PAINT(ChartCanvas::OnPaint)
  EVT_ACTIVATE(ChartCanvas::OnActivate)
  EVT_SIZE(ChartCanvas::OnSize)
  EVT_MOUSE_EVENTS(ChartCanvas::MouseEvent)
  EVT_TIMER( RESCALE_TIMER, ChartCanvas::RescaleTimerEvent)

  EVT_TIMER( PAN_TIMER, ChartCanvas::PanTimerEvent)

  EVT_MENU(ID_DEF_MENU_MAX_DETAIL,  ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_DEF_MENU_SCALE_IN,    ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_DEF_MENU_SCALE_OUT,   ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_DEF_MENU_QUERY,       ChartCanvas::PopupMenuHandler)

  EVT_MENU(ID_RT_MENU_ACTIVATE,     ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RT_MENU_DEACTIVATE,   ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RT_MENU_INSERT,       ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RT_MENU_APPEND,       ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RT_MENU_DELETE,       ChartCanvas::PopupMenuHandler)

  EVT_MENU(ID_RT_MENU_DELPOINT,     ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RT_MENU_ACTPOINT,     ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RT_MENU_DEACTPOINT,   ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RT_MENU_ACTNXTPOINT,  ChartCanvas::PopupMenuHandler)

  EVT_MENU(ID_RC_MENU_SCALE_IN,     ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RC_MENU_SCALE_OUT,    ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RC_MENU_ZOOM_IN,      ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RC_MENU_ZOOM_OUT,     ChartCanvas::PopupMenuHandler)
  EVT_MENU(ID_RC_MENU_FINISH,       ChartCanvas::PopupMenuHandler)

END_EVENT_TABLE()

// Define a constructor for my canvas
ChartCanvas::ChartCanvas(wxFrame *frame):
 wxWindow(frame, wxID_ANY,    wxPoint(20,20), wxSize(5,5), wxSIMPLE_BORDER )



{
      parent_frame = (MyFrame *)frame;          // save a pointer to parent


      SetBackgroundColour(wxColour(255,255,255));

      bDrawingRoute = false;
      m_test = 0;
      m_bRouteEditing = false;
      m_bFollow = false;
      m_bForceReDraw = false;
      pSelectedRoute = NULL;
      m_bAppendingRoute = false;          // was true in MSW, why??
      pThumbDIBShow = NULL;
      bShowCurrent = false;
      bShowTide = false;
      bShowingCurrent = false;
      pCwin = NULL;
      warp_flag = false;
      pMouseRoute = NULL;
      pss_overlay_bmp = NULL;
      pss_overlay_mask = NULL;

//    Build the cursors

#ifndef __WXGTK__
      pCursorLeft =    new ocpCursor(left,  0, 00, 15);
      pCursorRight =   new ocpCursor(right, 0, 31, 15);
      pCursorUp =      new ocpCursor(up,    0, 15, 00);
      pCursorDown =    new ocpCursor(down,  0, 15, 31);
      pCursorPencil =  new ocpCursor(pencil, 0, 00, 20);
#endif



#ifdef __WXGTK__
      wxImage ICursorLeft(left);
      wxImage ICursorRight(right);
      wxImage ICursorUp(up);
      wxImage ICursorDown(down);
      wxImage ICursorPencil(pencil);

      if(ICursorLeft.Ok())
      {
        ICursorLeft.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
        ICursorLeft.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
        pCursorLeft =  new wxCursor(ICursorLeft);
      }
      else
        pCursorLeft =  new wxCursor(wxCURSOR_ARROW);

      if(ICursorRight.Ok())
      {
        ICursorRight.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 31);
        ICursorRight.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
        pCursorRight =  new wxCursor(ICursorRight);
      }
      else
        pCursorRight =  new wxCursor(wxCURSOR_ARROW);

      if(ICursorUp.Ok())
      {
        ICursorUp.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 15);
        ICursorUp.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0 );
        pCursorUp =  new wxCursor(ICursorUp);
      }
      else
        pCursorUp =  new wxCursor(wxCURSOR_ARROW);

      if(ICursorDown.Ok())
      {
        ICursorDown.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 15);
        ICursorDown.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 31);
        pCursorDown =  new wxCursor(ICursorDown);
      }
      else
        pCursorDown =  new wxCursor(wxCURSOR_ARROW);

      if(ICursorPencil.Ok())
      {
        ICursorPencil.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
        ICursorPencil.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 20);
        pCursorPencil =  new wxCursor(ICursorPencil);
      }
      else
       pCursorPencil =  new wxCursor(wxCURSOR_ARROW);

#endif

      pCursorArrow = new wxCursor(wxCURSOR_ARROW);

      SetMyCursor(pCursorArrow);

      current_scale_method = SCALE_BILINEAR;          // default
      m_bSubsamp = true;
      m_rescale_timer_msec = 1000;

      pRescaleTimer = new wxTimer(this, RESCALE_TIMER);
      pRescaleTimer->Stop();

      pPanTimer = new wxTimer(this, PAN_TIMER);
      pPanTimer->Stop();

      pBM = NULL;

//    Set up current arrow drawing factors
      int mmx, mmy;
      wxDisplaySizeMM(&mmx, &mmy);

      int sx, sy;
      wxDisplaySize(&sx, &sy);

      int mm_per_knot = 25;
      current_draw_scaler =  mm_per_knot * (((float)sx) / ((float)mmx));

      pscratch_bm = NULL;

      wxRect trect(0,0,0,0);
      latest_bfollow_overlay_rect = trect;

// Set some benign initial values

      VPoint.clat = 0;
      VPoint.clon = 0;
      VPoint.view_scale = 0;
//      VPoint.Raster_Scale = 1.0;

      canvas_scale_factor = 1000.;

//    Create the default wvs chart
      pwvs_chart = new WVSChart(this, (char *)pWVS_Locn->c_str());

}

ChartCanvas::~ChartCanvas()
{

      if(pThumbDIBShow)
            delete pThumbDIBShow;

//    Delete Cursors
      delete pCursorLeft;
      delete pCursorRight;
      delete pCursorUp;
      delete pCursorDown;
      delete pCursorArrow;
      delete pCursorPencil;


      delete pRescaleTimer;
      delete pPanTimer;

      delete pBM;

      if(pscratch_bm)
            delete pscratch_bm;

      delete pwvs_chart;
      delete pss_overlay_bmp;

}

void ChartCanvas::OnEvtRescale(wxCommandEvent & event)
{


//  Tell the chart to take and use the alternate pix buffer
      pCBSB->AssumeAlternateCacheParms();

//    And force a redraw
      m_bForceReDraw = true;
      Refresh(false);
      Update();

}

void ChartCanvas::RescaleTimerEvent(wxTimerEvent& event)
{
//    Reset the scale method to bi-linear
      current_scale_method = SCALE_BILINEAR;

//    And redraw now
      m_bForceReDraw = true;
      Refresh(false);
      Update();
}



void ChartCanvas::GetPointPix(float rlat, float rlon, wxPoint *r)
{
      int  rpixxd, rpixyd;
      if(Current_Ch)
      {
            Current_Ch->latlong_to_pix_vp(rlat, rlon, rpixxd, rpixyd, VPoint);
            r->x = rpixxd;
            r->y = rpixyd;


//   Geolocation on small scale charts is poor if the requested point is very far away from the chart centroid
//    So, in this case use a linear estimator
//    Of course, cannot use linear estimator on skewed charts

            if(Current_Ch->GetChartSkew() ==  0.0)
            {

                  int x = (int)((rlon - VPoint.lon_left) * VPoint.ppd_lon);
                  int y = (int)((rlat - VPoint.lat_top) * VPoint.ppd_lat * -1);
//Todo maybe change to something like
//if((absf(VPoint.clat - rlat) > (VPoint.lat_top - VPoint.lat_bot) * n)
                  if((abs(r->x - x) > 1000) || (abs(r->y - y ) > 1000))
                  {
//                  wxString path;
//                  Current_Ch->GetFullPath(path);
//                  printf("GetPointPix Substitute on chart %s... %d %d %d %d\n", path.c_str(),
//                        r->x, x, r->y, y);
                        r->x = x;
                        r->y = y;
                  }

                  Extent ext;
                  Current_Ch->GetChartExtent(&ext);
                  if((rlat > ext.NLAT) || (rlat < ext.SLAT) || (rlon > ext.ELON) || (rlon < ext.WLON))
                  {
                        r->x = x;
                        r->y = y;
                  }
            }
      }
}

void ChartCanvas::GetPixPoint(int x, int y, float &lat, float &lon)
{
  if(Current_Ch)
  {
//  Linear estimation is no good on large charts, skewed or not...........
    if(1/*0.0 != Current_Ch->GetChartSkew()*/)
    {

      double slat, slon;
      Current_Ch->vp_pix_to_latlong(VPoint, x, y, &slat, &slon);
      lat = slat;
      lon = slon;
    }
    else
    {
        lat =  VPoint.lat_top - (y/VPoint.ppd_lat);
        lon =  VPoint.lon_left + (x/VPoint.ppd_lon);
    }
  }
}




void ChartCanvas::SetVPScale(double scale)
{
    SetViewPoint(VPoint.clat, VPoint.clon, scale, 1, CURRENT_RENDER);
/*
      if(m_bSubsamp)
      {
            current_scale_method = SCALE_SUBSAMP;
            pRescaleTimer->Start(m_rescale_timer_msec, wxTIMER_ONE_SHOT);
      }
    */
}

void ChartCanvas::SetViewPoint(double lat, double lon, double scale, int mode, int sample_mode)
{
      int pixxd, pixyd;
      int pixx, pixy;

//      printf("Scale: %.1f %.1f\n", scale, VPoint.view_scale);
      bNewVP = false;
      if(VPoint.clat != lat)
            bNewVP = true;
      if(VPoint.clon != lon)
            bNewVP = true;

      bool bNeedRescale = false;

      //    If chart scale has changed, force a cache flush and make the first render a SUB_SAMPLE
      if(VPoint.view_scale != scale)
      {
            bNewVP = true;
            if(Current_Ch)
                  Current_Ch->InvalidateCache();
            bNeedRescale = true;
      }

      //    If requested by sample_mode = FORCE_SUBSAMPLE, and subsampling is enabled, then
      //    force the scale method to SUBSAMPLE, rescale enabled

      if(FORCE_SUBSAMPLE == sample_mode)
          bNeedRescale = true;


      //    Of course, the BILINEAR Rescale is only sensible for raster charts displayed at
      //    something other than their native (1x) resolution.  Check it...

      if(Current_Ch)
      {
        if((Current_Ch->ChartType == CHART_TYPE_GEO) || (Current_Ch->ChartType == CHART_TYPE_KAP))
        {
            double sc = scale / Current_Ch->GetNativeScale();      // native (1X) scale
            if(sc !=  1)                                           //((int)rint(sc) != 1)
            {
                if(bNeedRescale)
                {
                    if(m_bSubsamp)
                    {
                        current_scale_method = SCALE_SUBSAMP;
                        pRescaleTimer->Start(m_rescale_timer_msec, wxTIMER_ONE_SHOT);
                    }
                }
            }
        }
      }



//      Save present values for necessary corrections

      double last_lat = VPoint.clat;
      double last_lon = VPoint.clon;
      float last_scale = VPoint.view_scale;

      VPoint.clat = lat;
      VPoint.clon = lon;
      VPoint.view_scale = scale;

      VPoint.ppd_lat = canvas_scale_factor / scale;
      VPoint.ppd_lon = canvas_scale_factor / scale;


      if(!Current_Ch)
            return;

//      float sc = scale / Current_Ch->GetNativeScale();      // native (1X) scale
//      VPoint.Raster_Scale = 1.0 / sc;

      if(Current_Ch->ChartType == CHART_TYPE_S57)
      {

//      If this viewpoint is same scale as last...
          if(last_scale == VPoint.view_scale)
          {
//      then require this viewport to be exact integral pixel number different from last
              double delta_pix_x = ( VPoint.clon - last_lon) * VPoint.ppd_lon;
              int dpix_x = (int)floor(delta_pix_x);
              double dpx = dpix_x;

              float delta_pix_y = ( VPoint.clat - last_lat) * VPoint.ppd_lat;
              int dpix_y = (int)floor(delta_pix_y);
              double dpy = dpix_y;

              double clond =  (dpx / VPoint.ppd_lon) + last_lon;
              VPoint.clon = clond;

              double clatd = (dpy / VPoint.ppd_lat) + last_lat;
              VPoint.clat = clatd;

          }



//    Ensure accuracy in case width or height are odd numbers
            float pwidth = VPoint.pix_width;
            float pheight = VPoint.pix_height;

            VPoint.lat_top =   VPoint.clat + ((pheight/2) / VPoint.ppd_lat);
            VPoint.lon_left =  VPoint.clon - ((pwidth/2)  / VPoint.ppd_lon);
            VPoint.lat_bot =   VPoint.lat_top  - ((pheight) / VPoint.ppd_lat);
            VPoint.lon_right = VPoint.lon_left + ((pwidth)  / VPoint.ppd_lon);
      }

      else if((Current_Ch->ChartType == CHART_TYPE_GEO) || (Current_Ch->ChartType == CHART_TYPE_KAP))

      {
            ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *>(Current_Ch);

            float sc = scale / Current_Ch->GetNativeScale();      // native (1X) scale
            Current_Ch->latlong_to_pix(lat, lon, pixxd, pixyd);
            pixx = pixxd;
            pixy = pixyd;

            wxRect source;
            if(mode == 1)           // mod 4
            {
                  int xmod = (pixx - (int)(VPoint.pix_width  * sc / 2))/4;
                  xmod *= 4;
                  source.x = xmod;
                  int ymod = (pixy - (int)(VPoint.pix_height * sc / 2))/4;
                  ymod *= 4;
                  source.y = ymod;

                  Current_Ch->SetVPParms(&VPoint);

                  //    Possible adjustment to clat/clon
                  double alat, alon;
                  Current_Ch->pix_to_latlong((int)(((VPoint.pix_width /2) * sc) + source.x),
                                        (int)(((VPoint.pix_height/2) * sc) + source.y),
                                        &alat, &alon);
                  VPoint.clat = alat;
                  VPoint.clon = alon;
            }
            else
            {
                  source.x = pixx - (int)(VPoint.pix_width  * sc / 2);
                  source.y = pixy - (int)(VPoint.pix_height * sc / 2);
            }


            source.width = (int)(VPoint.pix_width * sc) ;
            source.height = (int)(VPoint.pix_height * sc) ;

            VPoint.ppd_lat = Cur_BSB_Ch->GetPpd_lat_1x() / sc;
            VPoint.ppd_lon = Cur_BSB_Ch->GetPpd_lon_1x() / sc;


            //    Compute VPoint bounding box
            //    For skewed ( i.e. non-North-up) charts, use max and min on corners
            //    N.b. this results in the VPoint bbox being the minimum lat/lon
            //    bbox which fully contains the screen contents.

            //    Also, for charts displayed at high zoom-out, compute viewport boundaries
            //    using native pixel-per-degree rates, since georef algorithms sometimes fail
            //    disastrously when applied too far from chart centroid.
            if(((source.x < 0) && (source.x + source.width  > Cur_BSB_Ch->GetSize_X())) ||
                ((source.y < 0) && (source.y + source.height > Cur_BSB_Ch->GetSize_Y())) )
            {                                         // hi zoom out
                  float pwidth = VPoint.pix_width;
                  float pheight = VPoint.pix_height;

                  VPoint.lat_top =   lat + ((pheight/2) / VPoint.ppd_lat);
                  VPoint.lon_left =  lon - ((pwidth/2)  / VPoint.ppd_lon);
                  VPoint.lat_bot =   VPoint.lat_top  - ((pheight) / VPoint.ppd_lat);
                  VPoint.lon_right = VPoint.lon_left + ((pwidth)  / VPoint.ppd_lon);


            }
            else                                      // reasonable zoom
            {
                  double lonul, lonur, lonll, lonlr;
                  double latul, latur, latll, latlr;

            Current_Ch->pix_to_latlong((int)(((0) * sc) + source.x),
                               (int)(((0) * sc) + source.y),
                               &latul, &lonul);
            Current_Ch->pix_to_latlong((int)(((canvas_width) * sc) + source.x),
                               (int)(((0) * sc) + source.y),
                               &latur, &lonur);
            Current_Ch->pix_to_latlong((int)(((canvas_width) * sc) + source.x),
                               (int)(((canvas_height) * sc) + source.y),
                               &latlr, &lonlr);
            Current_Ch->pix_to_latlong((int)(((0) * sc) + source.x),
                               (int)(((canvas_height) * sc) + source.y),
                               &latll, &lonll);

            VPoint.lat_top = fmax(latul, latur);
                  VPoint.lat_bot = fmin(latll, latlr);
                  VPoint.lon_right = fmax(lonur, lonlr);
                  VPoint.lon_left  = fmin(lonul, lonll);

            }
      }

      else if(Current_Ch->ChartType == CHART_TYPE_DUMMY)
      {

            float pwidth = VPoint.pix_width;
            float pheight = VPoint.pix_height;

            VPoint.lat_top =   lat + ((pheight/2) / VPoint.ppd_lat);
            VPoint.lon_left =  lon - ((pwidth/2)  / VPoint.ppd_lon);
            VPoint.lat_bot =   VPoint.lat_top  - ((pheight) / VPoint.ppd_lat);
            VPoint.lon_right = VPoint.lon_left + ((pwidth)  / VPoint.ppd_lon);
      }


//      Calculate and store some metrics
      long0 = (VPoint.lon_right + VPoint.lon_left) / 2.;

//      Set the VP Bounding Box
      VPoint.vpBBox.SetMin(fmin(VPoint.lon_right, VPoint.lon_left),  fmin(VPoint.lat_bot, VPoint.lat_top));
      VPoint.vpBBox.SetMax(fmax(VPoint.lon_right, VPoint.lon_left),  fmax(VPoint.lat_bot, VPoint.lat_top));



      //    Calculate the conventional scale

      float ppdl = canvas_width / (VPoint.lon_right - VPoint.lon_left);
      VPoint.chart_scale = canvas_scale_factor / ppdl;

      //    Update the vp parameters private to the chart type
      Current_Ch->SetVPParms(&VPoint);

      if(parent_frame->pStatusBar)
      {
            char buf[20];
            sprintf(buf, "Scale: %8.0f", VPoint.chart_scale);
            parent_frame->SetStatusText(buf, 4);
      }

      VPoint.bValid = true;                     // Mark this ViewPoint as OK

}





void ChartCanvas::ShipDraw(wxDC& dc, wxPoint& iShipPoint, wxPoint& iPredPoint)
{

      int drawit = 0;
      wxPoint lShipPoint, lPredPoint;

//    Is ship in Vpoint?
      if(VPoint.vpBBox.PointInBox(gLon, gLat, 0))
            drawit++;                                 // yep


//    Calculate 5 Minute Position Predictor

      float east, north;
      double plat, plon;
      float pred_lat, pred_lon;

      DegToUTM(gLat, gLon, NULL, &east, &north, long0);

      east += (sin(gCog * PI / 180) * gSog * (1852.) / 12.);  // 5 mins, 1852 m/nmi
      north += (cos(gCog * PI / 180) * gSog * (1852.) / 12.);  // 5 mins

      UTMtoDeg(long0, 0, east, north, &plat, &plon);
      pred_lat = plat;
      pred_lon = plon;

//    Is predicted point in the VPoint?
      if(VPoint.vpBBox.PointInBox(pred_lon, pred_lat, 0))
            drawit++;                                 // yep


//    Do the draw if either the ship or prediction is within the current VPoint
    if(drawit)
      {
            int pixxd, pixyd;
            Current_Ch->latlong_to_pix_vp(gLat, gLon, pixxd, pixyd, VPoint);
            lShipPoint.x = pixxd;
            lShipPoint.y = pixyd;

            Current_Ch->latlong_to_pix_vp(pred_lat, pred_lon, pixxd, pixyd, VPoint);
            lPredPoint.x = pixxd;
            lPredPoint.y = pixyd;

/*
            if(bGPSValid)
                dc.SetBrush(*wxRED_BRUSH);
            else
                dc.SetBrush(*wxWHITE_BRUSH);
*/
            dc.SetBrush(wxBrush(Ship_Color));

            wxPen ppPen(wxColour(255,0,0), 2, wxSOLID);
            dc.SetPen(ppPen);
            dc.DrawLine(lShipPoint.x, lShipPoint.y, lPredPoint.x, lPredPoint.y);
            dc.DrawCircle(lPredPoint.x, lPredPoint.y, Ship_Size);

            dc.DrawCircle(lShipPoint.x, lShipPoint.y, 10);
      }
}


void ChartCanvas::OnActivate(wxActivateEvent& event)
{
      int x,y;
      this->GetClientSize(&x, &y);
      if(Current_Ch)
            Current_Ch->InvalidateCache();
}

void ChartCanvas::OnSize(wxSizeEvent& event)
{
//    int canvas_width, canvas_height;
      GetClientSize(&canvas_width, &canvas_height);

//    Get some canvas metrics

//          Rescale to current value, in order to rebuild VPoint data structures
//          for new canvas size
      SetVPScale(GetVPScale());

      float x_mm = wxGetDisplaySizeMM().GetWidth();         // gives client width in mm
      float mm_per_deg = (60 * 1.15 * 5280 *25.4 * 12);

      canvas_scale_factor = mm_per_deg / (x_mm / canvas_width);

      float pix_per_mm = canvas_width / x_mm;

#ifdef USE_S57
      if(ps52plib)
        ps52plib->SetPPMM(pix_per_mm);
#endif


//    Set up the scroll margins
      xr_margin = canvas_width  * 95/100;
      xl_margin = canvas_width  * 5/100;
      yt_margin = canvas_height * 5/100;
      yb_margin = canvas_height * 95/100;


//    Resize the current viewport

      VPoint.pix_width = canvas_width;
      VPoint.pix_height = canvas_height;

      if(Current_Ch)
            Current_Ch->InvalidateCache();

      if(pscratch_bm)
            delete pscratch_bm;

      pscratch_bm = new wxBitmap(VPoint.pix_width, VPoint.pix_height,  -1);

//  Rescale again, to cpture all the changes for new canvas size
      SetVPScale(GetVPScale());

}



static int mouse_x;
static int mouse_y;
static bool mouse_leftisdown;

void ChartCanvas::PanTimerEvent(wxTimerEvent& event)
{
      wxMouseEvent ev(wxEVT_MOTION);
      ev.m_x = mouse_x;
      ev.m_y = mouse_y;
      ev.m_leftDown = mouse_leftisdown;

      wxEvtHandler *evthp = GetEventHandler();

      ::wxPostEvent(evthp, ev);


}


void ChartCanvas::MouseEvent(wxMouseEvent& event)
{

      int x,y;
      char buf[80];
      int mx, my;
      float cursor_lon ;
      float cursor_lat ;

//    May get spurious mouse events during "settings" dialog
      if(!Current_Ch)
            return;

      event.GetPosition(&x, &y);

      mouse_x = x;
      mouse_y = y;
      mouse_leftisdown = event.LeftIsDown();

//    Retrigger the rescale timer if it is currently running

      if(pRescaleTimer->IsRunning())
            pRescaleTimer->Start(m_rescale_timer_msec, wxTIMER_ONE_SHOT);


      mx = x;
      my = y;

#ifdef __WXMSW__
      if(console->IsShown())
      {
            wxRegion rgn_console(console->GetRect());
            if(rgn_console.Contains(x,y)  == wxInRegion)
            {
//                wxLogMessage("chcanv::MouseEvent invoking concanv::CaptureMouse");
                  console->CaptureMouse();
                  return;
            }
      }
#endif




      if(Current_Ch)
      {
            GetPixPoint(x, y, cursor_lat, cursor_lon);

            strcpy(buf, "                        ");        // ugly
            toDMM(cursor_lat, buf, 20);
            int i = strlen(buf);
            buf[i++] = ' ';
            buf[i++] = ' ';

            toDMM(cursor_lon, &buf[i], 20);
            if(parent_frame->pStatusBar)
                  parent_frame->SetStatusText(buf, 1);
      }




//    Route Creation Rubber Banding
      if(parent_frame->nRoute_State >= 2)
      {
            r_rband.x = x;
            r_rband.y = y;
            bDrawingRoute = true;

//          Manage Auto-pan on Route Create
            double chart_extent_lon = VPoint.lon_right - VPoint.lon_left;
            double chart_extent_lat = VPoint.lat_top - VPoint.lat_bot;

            double new_lat = VPoint.clat;
            double new_lon = VPoint.clon;

            bool bft = false;
            if(x > canvas_width - 6)
                  {
                        new_lon += chart_extent_lon / 10;
                        bft = true;
                  }


            if(x < 6)
                  {
                        new_lon -= chart_extent_lon / 10;
                        bft = true;
                  }

            if(y < 6)
                  {
                        new_lat += chart_extent_lat / 10;
                        bft = true;
                  }

            if(y > canvas_height - 6)
                  {
                        new_lat -= chart_extent_lat / 10;
                        bft = true;
                  }



            if((bft) && !pPanTimer->IsRunning())
            {
                SetViewPoint(new_lat, new_lon, VPoint.view_scale, 1, FORCE_SUBSAMPLE);

                vLat = new_lat;
                vLon = new_lon;

                m_bFollow = false;      // update the follow flag
                toolBar->ToggleTool(ID_FOLLOW, false);
                pPanTimer->Start(500, wxTIMER_ONE_SHOT);

            }
            Refresh(false);

      }


//          Mouse Clicks

//    Manage canvas panning
      if(event.LeftDClick())
      {
            m_bFollow = false;      // update the follow flag
            toolBar->ToggleTool(ID_FOLLOW, false);

            vLat = cursor_lat;
            vLon = cursor_lon;

            SetViewPoint((double)vLat, (double)vLon, VPoint.view_scale, 1, FORCE_SUBSAMPLE);

            this->Refresh(FALSE);
      }


      if(event.LeftDown())
      {
            last_drag.x = mx;
            last_drag.y = my;

            if(parent_frame->nRoute_State)                        // creating route?
            {
                  double rlat, rlon;


                  SetMyCursor(pCursorPencil);
                  rlat = cursor_lat;
                  rlon = cursor_lon;

                  if(parent_frame->nRoute_State == 1)
                  {
                        pMouseRoute = new Route();
                        pRouteList->Append(pMouseRoute);
                        pMouseRoute->m_nPoints = 1;
                  }
                  else
                        pMouseRoute->m_nPoints += 1;



                  RoutePoint *pMousePoint = pMouseRoute->AddPoint(rlat, rlon);
                  pSelect->AddSelectablePoint(rlat, rlon, pMousePoint);

                  if(parent_frame->nRoute_State > 1)
                  {
                        pSelect->AddSelectableRouteSegment(prev_rlat, prev_rlon, rlat, rlon,
                                                                              prev_pMousePoint, pMousePoint);
                  }

                  prev_rlat = rlat;
                  prev_rlon = rlon;
                  prev_pMousePoint = pMousePoint;

                  parent_frame->nRoute_State++;

                  this->Refresh(false);
            }

            else                                // Not creating Route
            {                                         // So look for selectable route point

                  double slat, slon;
                  float SelectRadius;
      //    Calculate meaningful SelectRadius
                  int sel_rad_pix = 10;

                  SelectRadius = sel_rad_pix/VPoint.ppd_lat;

                  slat = cursor_lat;
                  slon = cursor_lon;
                  SelectItem *pFind = pSelect->FindSelection(slat, slon,
                                                                                    SELTYPE_ROUTEPOINT,
                                                                                    SelectRadius);

                  if(pFind)
                  {
                        RoutePoint *frp = (RoutePoint *)pFind->m_pData1;
                        frp->m_bPtIsSelected = true;
                        m_pEditRoute = frp->m_pParentRoute;
                        m_pEditRoute->m_bIsBeingEdited = true;
                        this->m_bRouteEditing = true;
                        this->m_pRoutePointEditTarget = frp;
                        this->m_pFoundPoint = pFind;
                        m_bForceReDraw = true;
                  }


                  else
                  {
//          Chart Panning
                        float dlat, dlon;
                        wxPoint p;
                        GetPointPix(VPoint.clat, VPoint.clon, &p);

                        int pan_pixels = 100;

                        bool bft = true;
                        switch (cursor_region)
                        {
                        case MID_RIGHT:
                        {
//                              new_lon += chart_extent_lon / 5;
                              p.x += pan_pixels;            // pixels
                              bft = false;
                              break;
                        }


                        case MID_LEFT:
                        {
//                              new_lon -= chart_extent_lon / 5;
                              p.x -= pan_pixels;
                              bft = false;
                              break;
                        }

                        case MID_TOP:
                        {
//                              new_lat -= chart_extent_lat / 5;
                              p.y += pan_pixels;
                              bft = false;
                              break;
                        }

                        case MID_BOT:
                        {
//                              new_lat += chart_extent_lat / 5;
                              p.y -= pan_pixels;
                              bft = false;
                              break;
                        }

                        case CENTER:
                              break;
                        }                             // switch


                        if(!bft)
                        {
                            GetPixPoint(p.x, p.y, dlat, dlon);
                            SetViewPoint(dlat, dlon, VPoint.view_scale, 1, FORCE_SUBSAMPLE);
                            vLat = dlat;
                            vLon = dlon;

// Doesn't work for skewed charts
//                            SetViewPoint(new_lat, new_lon, VPoint.view_scale, 1, FORCE_SUBSAMPLE);
//                            vLat = new_lat;
//                            vLon = new_lon;

                            m_bFollow = false;      // update the follow flag
                            toolBar->ToggleTool(ID_FOLLOW, false);

                            Update();
                            Refresh(false);
                        }
                  }                             // else
            }                                   // else
      }



      if(event.Dragging())
      {
            if(m_bRouteEditing)
            {
                  m_pRoutePointEditTarget->rlat = cursor_lat;     // update the RoutePoint entry
                  m_pRoutePointEditTarget->rlon = cursor_lon;
                  m_pFoundPoint->m_slat = cursor_lat;             // update the SelectList entry
                  m_pFoundPoint->m_slon = cursor_lon;


//          Manage Auto-pan on Route Edit
                  double chart_extent_lon = VPoint.lon_right - VPoint.lon_left;
                  double chart_extent_lat = VPoint.lat_top - VPoint.lat_bot;

                  double new_lat = VPoint.clat;
                  double new_lon = VPoint.clon;

                  bool bft = false;
                  int pan_margin = 20;
                  double pan_factor = .02;
                  int pan_timer_set = 100;

                  if(x > canvas_width - pan_margin)
                  {
                        new_lon += chart_extent_lon * pan_factor;
                        bft = true;
                  }

                  if(x < pan_margin)
                  {
                        new_lon -= chart_extent_lon * pan_factor;
                        bft = true;
                  }

                  if(y < pan_margin)
                  {
                        new_lat += chart_extent_lat * pan_factor;
                        bft = true;
                  }

                  if(y > canvas_height - pan_margin)
                  {
                        new_lat -= chart_extent_lat * pan_factor;
                        bft = true;
                  }



                  if((bft) && !pPanTimer->IsRunning())
                  {
                      SetViewPoint(new_lat, new_lon, VPoint.view_scale, 1, FORCE_SUBSAMPLE);

                        float new_cursor_lat, new_cursor_lon;
                        GetPixPoint(x, y, new_cursor_lat, new_cursor_lon);
                        m_pRoutePointEditTarget->rlat = new_cursor_lat;     // update the RoutePoint entry
                        m_pRoutePointEditTarget->rlon = new_cursor_lon;
                        m_pFoundPoint->m_slat = new_cursor_lat;             // update the SelectList entry
                        m_pFoundPoint->m_slon = new_cursor_lon;

                        vLat = new_lat;
                        vLon = new_lon;


                        m_bFollow = false;      // update the follow flag
                        toolBar->ToggleTool(ID_FOLLOW, false);
                        pPanTimer->Start(pan_timer_set, wxTIMER_ONE_SHOT);

                  }
                  Refresh(false);
            }     // if Route Editing

            else                                            // must be chart dragging...
            {
                  if((last_drag.x != mx) || (last_drag.y != my))
                  {

                        float dlat, dlon;
                        wxPoint p;
                        GetPointPix(VPoint.clat, VPoint.clon, &p);
                        p.x -= (mx - last_drag.x);
                        p.y -= (my - last_drag.y);

                        GetPixPoint(p.x, p.y, dlat, dlon);
                        SetViewPoint(dlat, dlon, GetVPScale(), 1, FORCE_SUBSAMPLE);
                        vLat = dlat;
                        vLon = dlon;

                        /*  Does not work for skewed charts....
                        float nlat = (my - last_drag.y) / VPoint.ppd_lat;
                        float nlon = (mx - last_drag.x) / VPoint.ppd_lon;

                        SetViewPoint(VPoint.clat + nlat, VPoint.clon - nlon, GetVPScale(), 1);
                        vLat = VPoint.clat + nlat;
                        vLon = VPoint.clon - nlon;
                        */

                        m_bFollow = false;
                        toolBar->ToggleTool(ID_FOLLOW, false);

                        last_drag.x = mx;
                        last_drag.y = my;

//    if subsampled panning is enabled....
                        if(m_bSubsamp)
                        {
                              current_scale_method = SCALE_SUBSAMP;
//    Retrigger the rescale timer
                              pRescaleTimer->Start(m_rescale_timer_msec, wxTIMER_ONE_SHOT);
                        }

                        Refresh(false);
                  }

            }


      }


      if(event.LeftUp())
      {
            if(m_bRouteEditing)
            {

//    Purge all selectable segments belonging to route

                  pSelect->DeleteAllSelectableRouteSegments(m_pRoutePointEditTarget->m_pParentRoute);

//    Walk the route, adding selectable segments
                  Route *rt = m_pRoutePointEditTarget->m_pParentRoute;

                  wxRoutePointListNode *node = rt->pRoutePointList->GetFirst();
                  RoutePoint *prp = node->GetData();

                  float rlat, rlon, prev_rlat, prev_rlon;
                  RoutePoint *prev_pUpdatePoint, *pUpdatePoint;

                  prev_rlat = prp->rlat;
                  prev_rlon = prp->rlon;
                  prev_pUpdatePoint = prp;

                  node = node->GetNext();

                  while (node)
                  {

                        RoutePoint *prp = node->GetData();
                        rlat = prp->rlat;
                        rlon = prp->rlon;
                        pUpdatePoint = prp;
                        pSelect->AddSelectableRouteSegment(prev_rlat, prev_rlon, rlat, rlon,
                                                                        prev_pUpdatePoint, pUpdatePoint);

                        prev_rlat = rlat;
                        prev_rlon = rlon;
                        prev_pUpdatePoint = pUpdatePoint;

                        node = node->GetNext();
                  }

                  rt->CalculateBBox();
                  rt->UpdateSegmentDistances();

                  pConfig->UpdateRoute(m_pRoutePointEditTarget->m_pParentRoute);

                  m_bRouteEditing = false;
                  m_pEditRoute->m_bIsBeingEdited = false;
                  m_pEditRoute = NULL;
                  m_pRoutePointEditTarget->m_bPtIsSelected = false;
                  m_pRoutePointEditTarget = NULL;


                  m_bForceReDraw = true;

                  Refresh(false);
            }
      }




      if(event.RightDown())
      {
            last_drag.x = mx;
            last_drag.y = my;

            if(parent_frame->nRoute_State)                        // creating route?
                  CanvasPopupMenu(x,y, SELTYPE_ROUTECREATE);
            else                                                  // General Right Click
            {
                                                                  // Look for selectable RouteParts
                  float slat, slon;
                  float SelectRadius;
      //    Calculate meaningful SelectRadius
                  int sel_rad_pix = 10;

                  SelectRadius = sel_rad_pix/VPoint.ppd_lat;
                  slat = cursor_lat;
                  slon = cursor_lon;
                  SelectItem *pFind;
                  wxClientDC dc(this);

                  if((pFind = pSelect->FindSelection(slat, slon,SELTYPE_ROUTEPOINT,SelectRadius)) != NULL)
                  {
                        pFoundRoutePoint = (RoutePoint *)pFind->m_pData1;

                        if(pSelectedRoute)
                        {
                              pSelectedRoute->m_bRtIsSelected = false;        // Only one selection at a time
                              pSelectedRoute->DeSelectRoute();
                              pSelectedRoute->DrawRoute(dc);
                        }
                        pSelectedRoute = pFoundRoutePoint->m_pParentRoute;
                        pSelectedRoute->m_bRtIsSelected = false;
                        pFoundRoutePoint->m_bPtIsSelected = true;
                        pSelectedRoute->m_nRouteActivePoint = pFoundRoutePoint->nPoint;

                        pSelectedRoute->DrawRoute(dc);

                        CanvasPopupMenu(x,y, SELTYPE_ROUTEPOINT);

                        m_bForceReDraw = true;
                        Refresh(false);
                  }


                  else if((pFind = pSelect->FindSelection(slat, slon,SELTYPE_ROUTESEGMENT,SelectRadius)) != NULL)
                  {
                        pFoundRoutePoint = (RoutePoint *)pFind->m_pData1;

                        if(pSelectedRoute)
                        {
                              pSelectedRoute->m_bRtIsSelected = false;        // Only one selection at a time
                              pSelectedRoute->DeSelectRoute();
                              pSelectedRoute->DrawRoute(dc);
                        }
                        pSelectedRoute = pFoundRoutePoint->m_pParentRoute;
                        pSelectedRoute->m_bRtIsSelected = true;

                        pSelectedRoute->DrawRoute(dc);

                        CanvasPopupMenu(x,y, SELTYPE_ROUTESEGMENT);

                        m_bForceReDraw = true;
                        Refresh(false);

                  }

                  else
                  {
                      bool bseltc = false;

                      if(bShowCurrent)
                      {
                        if(NULL != (pFind = pSelectTC->FindSelection(slat, slon,SELTYPE_CURRENTPOINT,SelectRadius)) )
                        {
                           DrawTCWindow(x, y, (void *)pFind->m_pData1);
                           Refresh(false);
                           bseltc = true;
                        }
                      }

                      if(bShowTide)                                   // look for tide stations
                      {
                        if(NULL != (pFind = pSelectTC->FindSelection(slat, slon,SELTYPE_TIDEPOINT,SelectRadius)) )
                        {
                           DrawTCWindow(x, y, (void *)pFind->m_pData1);
                           Refresh(false);
                           bseltc = true;
                        }
                      }

                      if(!bseltc)
                           CanvasPopupMenu(x,y, SELTYPE_UNKNOWN);
                  }
            }
      }


//    Switch to the appropriate cursor on mouse movement
      if(!parent_frame->nRoute_State)
      {
            if(x > xr_margin)
            {
            SetMyCursor(pCursorRight);
            cursor_region = MID_RIGHT;
            }
            else if(x < xl_margin)
            {
            SetMyCursor(pCursorLeft);
            cursor_region = MID_LEFT;
            }
            else if(y > yb_margin)
            {
            SetMyCursor(pCursorDown);
            cursor_region = MID_TOP;
            }
            else if(y < yt_margin)
            {
            SetMyCursor(pCursorUp);
            cursor_region = MID_BOT;
            }
            else
            {
            SetMyCursor(pCursorArrow);
            cursor_region = CENTER;
            }
      }
      else
            SetMyCursor(pCursorPencil);

}


//-------------------------------------------------------------------------------
//          Popup Menu Handling
//-------------------------------------------------------------------------------

void ChartCanvas::CanvasPopupMenu(int x, int y, int seltype)

{
      popx = x;
      popy = y;

      switch(seltype)
      {
      case SELTYPE_ROUTESEGMENT:
            {
                  wxMenu *pdef_menu = new wxMenu();

                  pdef_menu->Append(ID_RT_MENU_ACTIVATE,          _T("Activate Route"));
                  pdef_menu->Append(ID_RT_MENU_DEACTIVATE,        _T("Deactivate Route"));
                  pdef_menu->Append(ID_RT_MENU_INSERT,            _T("Insert Waypoint"));
                  pdef_menu->Append(ID_RT_MENU_APPEND,            _T("Append Waypoint"));
                  pdef_menu->Append(ID_RT_MENU_DELETE,            _T("Delete Route"));

                  if(pSelectedRoute && pSelectedRoute->m_bRtIsActive)
                  {
                        pdef_menu->Enable(ID_RT_MENU_ACTIVATE, 0);
                        pdef_menu->Enable(ID_RT_MENU_DEACTIVATE, 1);
                  }
                  else
                  {
                        pdef_menu->Enable(ID_RT_MENU_ACTIVATE, 1);
                        pdef_menu->Enable(ID_RT_MENU_DEACTIVATE, 0);
                  }

                  this->PopupMenu(pdef_menu, x, y);

                  if(pSelectedRoute && pSelectedRoute->m_bRtIsActive)
                        pSelectedRoute->m_bRtIsSelected = false;

                  delete pdef_menu;

                  break;
            }
      case SELTYPE_ROUTEPOINT:
            {
                  wxMenu *pdef_menu = new wxMenu();

                  pdef_menu->Append(ID_RT_MENU_ACTPOINT,    _T("Activate Waypoint"));
                  pdef_menu->Append(ID_RT_MENU_DEACTPOINT,  _T("Deactivate Waypoint"));
                  pdef_menu->Append(ID_RT_MENU_DELPOINT,    _T("Delete Waypoint"));
                  pdef_menu->Append(ID_RT_MENU_ACTNXTPOINT, _T("Activate Next in Route"));

                  this->PopupMenu(pdef_menu, x, y);

                  if(pSelectedRoute && pSelectedRoute->m_bRtIsActive)
                        pSelectedRoute->m_bRtIsSelected = false;

                  delete pdef_menu;
                  break;
            }
      case SELTYPE_UNKNOWN:
            {
                  wxMenu *pdef_menu = new wxMenu();

                  pdef_menu->Append(ID_DEF_MENU_MAX_DETAIL, _T("Max Detail Here"));
                  pdef_menu->Append(ID_DEF_MENU_SCALE_IN,   _T("Scale In"));
                  pdef_menu->Append(ID_DEF_MENU_SCALE_OUT,  _T("Scale Out"));

                  if(1/*Current_Ch->ChartType == CHART_TYPE_S57*/)
                        pdef_menu->Append(ID_DEF_MENU_QUERY,  _T("Object Query"));

                  this->PopupMenu(pdef_menu, x, y);

                  delete pdef_menu;
                  break;
            }
      case SELTYPE_ROUTECREATE:
            {
                  wxMenu *pdef_menu = new wxMenu();

                  pdef_menu->Append(ID_DEF_MENU_MAX_DETAIL, _T("Max Detail Here"));
                  pdef_menu->Append(ID_RC_MENU_SCALE_IN,    _T("Scale In"));
                  pdef_menu->Append(ID_RC_MENU_SCALE_OUT,   _T("Scale Out"));
                  pdef_menu->Append(ID_RC_MENU_ZOOM_IN,     _T("Zoom In"));
                  pdef_menu->Append(ID_RC_MENU_ZOOM_OUT,    _T("Zoom Out"));
                  pdef_menu->Append(ID_RC_MENU_FINISH,      _T("End Route"));

                  this->PopupMenu(pdef_menu, x, y);

                  delete pdef_menu;
                  break;
            }
      }

      Update();

}

void ChartCanvas::PopupMenuHandler(wxCommandEvent& event)
{
      int np;
      RoutePoint *pLast;
      RoutePoint *pNew_Point;
      float f,g;

      wxPoint r;
      float zlat, zlon;
      unsigned int i;
      float SelectRadius;
      int sel_rad_pix;

#ifdef USE_S57
      S57QueryDialog *pdialog;
      ArrayOfS57Obj *array;
      s57chart *Chs57;
      wxString *description;
      wxString *QueryResult;
#endif

      GetPixPoint(popx, popy, zlat, zlon);


      switch( event.GetId() )
      {
      case ID_DEF_MENU_MAX_DETAIL:
                        vLat = zlat;
                        vLon = zlon;
                        toolBar->ToggleTool(ID_FOLLOW, false);
                        m_bFollow = false;

                        parent_frame->DoChartUpdate(0);

                        parent_frame->SelectChartFromStack(0);
            break;

      case ID_DEF_MENU_SCALE_IN:
            parent_frame-> DoStackDown();
            break;

      case ID_DEF_MENU_SCALE_OUT:
            parent_frame-> DoStackUp();
            break;

#ifdef USE_S57
      case ID_DEF_MENU_QUERY:
          {
            if(Current_Ch->ChartType == CHART_TYPE_S57)
            {
//    Got get the array of all objects at the cursor lat/lon
                Chs57 = dynamic_cast<s57chart*>(Current_Ch);

                sel_rad_pix = 10;
                SelectRadius = sel_rad_pix/VPoint.ppd_lat;

                QueryResult = new wxString;
                array = Chs57->GetObjArrayAtLatLon( zlat, zlon, SelectRadius);

                if(!array->IsEmpty())
                {
                      for(i = 0 ; i < array->GetCount() ; i++)
                      {
                            description = Chs57->CreateObjDescription(array->Item(i));
                            QueryResult->Append(*description);
                            delete description;
                      }
                }

                pdialog = new S57QueryDialog();
                pdialog->SetText(*QueryResult);

                pdialog->Create(NULL, -1, wxT("Object Query"));
                pdialog->ShowModal();

                delete pdialog;
                delete QueryResult;

                m_bForceReDraw = true;
            }
            break;
          }
#endif
      case ID_RT_MENU_DELETE:
            if(pRouteMan->GetpActiveRoute())
                  pRouteMan->DeactivateRoute();

            pSelect->DeleteAllSelectableRouteSegments(pSelectedRoute);
            pSelect->DeleteAllSelectableRoutePoints(pSelectedRoute);

            pConfig->DeleteRoute(pSelectedRoute);
            pRouteList->DeleteObject(pSelectedRoute);
            delete pSelectedRoute;
            pSelectedRoute = NULL;

            m_bForceReDraw = true;

            break;

      case ID_RT_MENU_ACTIVATE:
            if(pRouteMan->GetpActiveRoute())
                  pRouteMan->DeactivateRoute();

            pRouteMan->ActivateRoute(pSelectedRoute);
            pSelectedRoute->m_bRtIsSelected = false;

            m_bForceReDraw = true;
            break;

      case ID_RT_MENU_DEACTIVATE:
            pRouteMan->DeactivateRoute();
            pSelectedRoute->m_bRtIsSelected = false;

            m_bForceReDraw = true;
            break;

      case ID_RT_MENU_INSERT:
            np = pFoundRoutePoint->nPoint;

            pSelectedRoute->InsertPoint(np, zlat, zlon);

            pSelect->DeleteAllSelectableRoutePoints(pSelectedRoute);
            pSelect->DeleteAllSelectableRouteSegments(pSelectedRoute);

            pSelect->AddAllSelectableRouteSegments(pSelectedRoute);
            pSelect->AddAllSelectableRoutePoints(pSelectedRoute);

            if(pSelectedRoute->m_bRtIsActive)
            {
                  if(pSelectedRoute->m_nRouteActivePoint == np + 1)
                  {
                        pNew_Point = pSelectedRoute->GetPoint(np + 2, &f, &g);
                        pRouteMan->ActivateRoutePoint(pSelectedRoute, pNew_Point);
                  }
            }
            pConfig->UpdateRoute(pSelectedRoute);

            break;

      case ID_RT_MENU_APPEND:
            pMouseRoute = pSelectedRoute;
            parent_frame->nRoute_State = pSelectedRoute->m_nPoints + 1;

            pLast = pSelectedRoute->GetLastPoint();

            prev_rlat = pLast->rlat;
            prev_rlon = pLast->rlon;
            prev_pMousePoint = pLast;

            m_bAppendingRoute = true;

            SetMyCursor(pCursorPencil);

            break;

      case ID_RT_MENU_DELPOINT:
//    n.b. must delete Selectables before deleting the point
            pSelect->DeleteAllSelectableRoutePoints(pSelectedRoute);
            pSelect->DeleteAllSelectableRouteSegments(pSelectedRoute);

            pSelectedRoute->DeletePoint(pFoundRoutePoint);

            if(pSelectedRoute->m_nPoints)
            {
                  pSelect->AddAllSelectableRouteSegments(pSelectedRoute);
                  pSelect->AddAllSelectableRoutePoints(pSelectedRoute);

                  pConfig->UpdateRoute(pSelectedRoute);
            }
            else
            {
                  pConfig->DeleteRoute(pSelectedRoute);
                  pRouteList->DeleteObject(pSelectedRoute);
                  delete pSelectedRoute;
                  pSelectedRoute = NULL;
            }

            m_bForceReDraw = true;
            break;

      case ID_RT_MENU_ACTPOINT:
            if(pRouteMan->GetpActiveRoute() == pSelectedRoute)
            {
                  pRouteMan->ActivateRoutePoint(pSelectedRoute, pFoundRoutePoint);
                  pSelectedRoute->m_bRtIsSelected = false;
            }


            m_bForceReDraw = true;
            break;

      case ID_RT_MENU_DEACTPOINT:

            m_bForceReDraw = true;
            break;

      case ID_RT_MENU_ACTNXTPOINT:
            if(pRouteMan->GetpActiveRoute() == pSelectedRoute)
            {
                  pRouteMan->ActivateNextPoint(pSelectedRoute);
                  pSelectedRoute->m_bRtIsSelected = false;
            }

            m_bForceReDraw = true;
            break;

      case ID_RC_MENU_SCALE_IN:
      {
             parent_frame-> DoStackDown();
            GetPointPix(zlat, zlon, &r);
            WarpPointer(r.x, r.y);
            break;
      }

      case ID_RC_MENU_SCALE_OUT:
      {
            parent_frame-> DoStackUp();
            GetPointPix(zlat, zlon, &r);
            WarpPointer(r.x, r.y);

            break;
      }

      case ID_RC_MENU_ZOOM_IN:
      {
            SetVPScale(GetVPScale() / 2);
//            Current_Ch->InvalidateCache();
            GetPointPix(zlat, zlon, &r);
            WarpPointer(r.x, r.y);

            break;
      }

      case ID_RC_MENU_ZOOM_OUT:
      {
            SetVPScale(GetVPScale() * 2);
            GetPointPix(zlat, zlon, &r);
            WarpPointer(r.x, r.y);

            break;
      }

      case ID_RC_MENU_FINISH:
      {
            parent_frame->nRoute_State = 0;
            toolBar->ToggleTool(ID_ROUTE, false);
            SetMyCursor(pCursorArrow);
            bDrawingRoute = false;

            if(pMouseRoute)
            {
                  if(m_bAppendingRoute)
                        pConfig->UpdateRoute(pMouseRoute);
                  else
                        if(pMouseRoute->m_nPoints)
                           pConfig->AddNewRoute(pMouseRoute, -1);       // use auto next num

            }
            m_bAppendingRoute = false;

            m_bForceReDraw = true;
            Refresh(false);

            break;
      }

      }           // switch

}


void ChartCanvas::RenderAllChartOutlines(wxDC *pdc, ViewPort& vp, bool bdraw_mono)
{
    for(int i=0 ; i < ChartData->nEntry ; i++)
    {
          RenderChartOutline(pdc, i, vp, bdraw_mono);
    }
}

extern "C" {
      typedef enum { Visible, Invisible } ClipResult;
      ClipResult cohen_sutherland_line_clip_i (int *x0, int *y0, int *x1, int *y1,
                                   int xmin_, int xmax_, int ymin_, int ymax_);
}


void ChartCanvas::RenderChartOutline(wxDC *pdc, int dbIndex, ViewPort& vp, bool bdraw_mono)
{
        float plylat, plylon, plylat1, plylon1;

        int pixx, pixy, pixx1, pixy1;

        wxPen gPen(wxColour(0,255,0), 2, wxSOLID);
        wxPen rPen(wxColour(255,0,0), 2, wxSOLID);

        int nPly = ChartData->GetDBPlyPoint(dbIndex, 0, &plylat, &plylon);

        if(ChartData->GetDBChartType(dbIndex) == CHART_TYPE_S57)
              pdc->SetPen(gPen);
        else
              pdc->SetPen(rPen);

        if(bdraw_mono)
        {
#ifdef __WXX11__
          wxPen pp(*wxBLACK, 2, wxSOLID);
          pdc->SetPen(pp);
#else
          wxPen pp(*wxWHITE, 2, wxSOLID);
          pdc->SetPen(pp);
#endif
        }


        int i;


        wxPoint r, r1;

        ChartData->GetDBPlyPoint(dbIndex, 0, &plylat, &plylon);
        GetPointPix(plylat, plylon, &r);
        pixx = r.x;
        pixy = r.y;

        for( i=0 ; i<nPly-1 ; i++)
        {
              ChartData->GetDBPlyPoint(dbIndex, i+1, &plylat1, &plylon1);

              GetPointPix(plylat1, plylon1, &r1);
              pixx1 = r1.x;
              pixy1 = r1.y;

              int pixxs1 = pixx1;
              int pixys1 = pixy1;

              ClipResult res = cohen_sutherland_line_clip_i (&pixx, &pixy, &pixx1, &pixy1,
                        0, vp.pix_width, 0, vp.pix_height);
              if(res != Invisible)
                    pdc->DrawLine(pixx, pixy, pixx1, pixy1);

              plylat = plylat1;
              plylon = plylon1;
              pixx = pixxs1;
              pixy = pixys1;
        }

        ChartData->GetDBPlyPoint(dbIndex, 0, &plylat1, &plylon1);
        GetPointPix(plylat1, plylon1, &r1);
        pixx1 = r1.x;
        pixy1 = r1.y;

        ClipResult res = cohen_sutherland_line_clip_i (&pixx, &pixy, &pixx1, &pixy1,
                        0, vp.pix_width, 0, vp.pix_height);
        if(res != Invisible)
                  pdc->DrawLine(pixx, pixy, pixx1, pixy1);

}


void ChartCanvas::WarpPointerDeferred(int x, int y)
{
      warp_x = x;
      warp_y = y;
      warp_flag = true;
}

static int in_paint=0;
void ChartCanvas::OnPaint(wxPaintEvent& event)
{
      if(in_paint)
      {
//          wxLogMessage("OnPaint Recursion...");
            return;
      }

      in_paint++;

      wxPaintDC dc(this);

      wxRegion ru = GetUpdateRegion();

      if(!Current_Ch)
      {
//          wxLogMessage("OnPaint with CurrentCh == NULL");
            in_paint--;
            return;
      }

      if(!VPoint.IsValid())
      {
//            wxLogMessage("OnPaint with Invalid VPoint");
            in_paint--;
            return;
      }


      wxPoint ShipPoint(0,0);
      wxPoint PredPoint(0,0);

      wxBoundingBox BltBBox;
      ChartTypeEnum type;

#ifdef dyUSE_DIBSECTION
      dyMemDC temp_dc;
#else
      wxMemoryDC temp_dc;
#endif

      if(Current_Ch)
            type = Current_Ch->ChartType;


//    In case Console is shown, set up dc clipper and blt iterator regions

      wxRegion rgn_chart(0,0,VPoint.pix_width, VPoint.pix_height);
      int conx, cony;
      console->GetPosition(&conx, &cony);
      wxRegion rgn_console(conx, cony, console->Size_X, console->Size_Y);

      if(console->IsShown())
      {
            rgn_chart.Subtract(rgn_console);
#ifdef __WXMSW__                                // Todo dunno if this ifdef necessary, try on X11 and GTK.....
            ru.Subtract(rgn_console);
#endif
      }

//    Same for Thumbnail window
      if(pthumbwin)
      {
          int thumbx, thumby, thumbsx, thumbsy;
          pthumbwin->GetPosition(&thumbx, &thumby);
          pthumbwin->GetSize(&thumbsx, &thumbsy);
          wxRegion rgn_thumbwin(thumbx, thumby, thumbsx, thumbsy);

          if(pthumbwin->IsShown())
          {
                rgn_chart.Subtract(rgn_thumbwin);
#ifdef __WXMSW__
                ru.Subtract(rgn_thumbwin);
#endif
          }

      }

      bool bNewMap;

      if((bNewVP) || (m_bForceReDraw))
            bNewMap = true;
      else
            bNewMap = false;




//    Use an offscreen canvas, to protect the chart bitmap which may be cached by the Current_Ch object

//    Grab a fresh copy of the chart object's image data, and render to a temp dc`
//      wxStopWatch st;
      Current_Ch->RenderViewOnDC(temp_dc, VPoint, current_scale_method);
//      int time = st.Time();
//      if(time > 2)
//        printf("Render %ldms to execute\n", st.Time());

//    Arrange to render the WVSChart vector data ..BEHIND.. the rendered current chart
//    So that uncovered canvas areas show at least the WVS chart

//    Make a region covering the current chart on the canvas
      wxRegion CValidRegion;
      Current_Ch->GetValidCanvasRegion(VPoint, &CValidRegion);

//    Copy current chart region
      wxRegion WVSRegion(rgn_chart);

//    Remove the valid chart area
      WVSRegion.Subtract(CValidRegion);

//    Associate with temp_dc
      temp_dc.DestroyClippingRegion();
      temp_dc.SetClippingRegion(WVSRegion);

//    Draw the WVSChart only in the areas NOT covered by the current chart view
//    And, only if the region is ..not.. empty
      if(!WVSRegion.IsEmpty() && (fabs(Current_Ch->GetChartSkew()) < 1.0))
            pwvs_chart->RenderViewOnDC(temp_dc, VPoint);


//    Draw the overlay objects on a scratch DC, to calculate update regions
      wxMemoryDC scratch_dc;
      scratch_dc.SelectObject(*pscratch_bm);

      scratch_dc.ResetBoundingBox();
      scratch_dc.DestroyClippingRegion();
      scratch_dc.SetClippingRegion(rgn_chart);

      DrawAllRoutesInBBox(scratch_dc, VPoint.vpBBox);
      ShipDraw(scratch_dc, ShipPoint, PredPoint);


      //  Route Creating
      wxPoint rpt;
      if(parent_frame->nRoute_State >= 2)
      {
            pMouseRoute->DrawPointWhich(scratch_dc, parent_frame->nRoute_State - 1,  &rpt);
            pMouseRoute->DrawSegment(scratch_dc, &rpt, &r_rband);
      }

//    Record the bounding rectangle of the overlay drawing
      wxRect this_bfollow_overlay_rect;

      this_bfollow_overlay_rect.x = scratch_dc.MinX() - 1;
      this_bfollow_overlay_rect.y = scratch_dc.MinY() - 1;
      this_bfollow_overlay_rect.width  = scratch_dc.MaxX() - scratch_dc.MinX() + 2; // Mouse Poop?
      this_bfollow_overlay_rect.height = scratch_dc.MaxY() - scratch_dc.MinY() + 2;

      wxRegion rgn_overlay(this_bfollow_overlay_rect);
//    Clip the overlay region to the chart
      rgn_overlay.Intersect(rgn_chart);

//    Make a region out of the last overlay objects drawn
      wxRegion rgn_last(latest_bfollow_overlay_rect);

//    Also clip the last overlay region to the chart
      rgn_last.Intersect(rgn_chart);

//    So, the required redraw region is the union of the last region, in order to effectively erase the last drawing, and
//    the new drawing region
      rgn_overlay.Union(rgn_last);

//    Record the overlay bounding box for next time
      latest_bfollow_overlay_rect = this_bfollow_overlay_rect;


//    Blit all or part of the chart dc onto the scratch dc.
//    If there is a new map image, or other reason signaled by m_bForceReDraw,
//    we need to blit the whole thing.
//    Otherwise, it is sufficient to blit only the area with overlay object area

      wxRegion rgn_blit;
      if(bNewMap)
            rgn_blit = rgn_chart;         // whole thing
      else
            rgn_blit = rgn_overlay;       // only the overlays

 //    Also, must add in the window invalid region as maintained by window class
      rgn_blit.Union(ru);

      wxRegionIterator upd(rgn_blit); // get the update rect list
      while (upd)
      {
            wxRect rect = upd.GetRect();
            bool b = scratch_dc.Blit(rect.x, rect.y, rect.width, rect.height,
                  &temp_dc, rect.x, rect.y);
            if(b != true)
                wxLogMessage("scratchblitfalse");
//            printf("   Scratch Blit %d %d %d %d\n",rect.x, rect.y, rect.width, rect.height);
            upd ++ ;
      }

//    Now draw the overlay objects directly on the scratch dc, again.

      scratch_dc.SetClippingRegion(rgn_chart);

      DrawAllRoutesInBBox(scratch_dc, VPoint.vpBBox);
      ShipDraw(scratch_dc, ShipPoint, PredPoint);

      if(parent_frame->nRoute_State >= 2)
      {
        pMouseRoute->DrawPointWhich(scratch_dc, parent_frame->nRoute_State - 1,  &rpt);
        pMouseRoute->DrawSegment(scratch_dc, &rpt, &r_rband);
      }


//  Using yet another bitmap and DC, draw semi-static overlay objects if necessary
      if(bNewMap && (bShowTide || bShowCurrent || g_bShowOutlines))
      {
        delete pss_overlay_bmp;
        pss_overlay_bmp = DrawTCCBitmap(bNewMap);
      }

//    blit the semi-static overlay onto the scratch DC if it is needed
      if(bShowTide || bShowCurrent || g_bShowOutlines)
      {
          wxMemoryDC ssdc_r;
          ssdc_r.SelectObject(*pss_overlay_bmp);

          wxRegionIterator upd_final(rgn_blit);
          while (upd_final)
          {
            wxRect rect = upd_final.GetRect();
            scratch_dc.Blit(rect.x, rect.y, rect.width, rect.height,
                  &ssdc_r, rect.x, rect.y, wxCOPY, true);       // Blit with mask
            upd_final ++ ;
          }

          ssdc_r.SelectObject(wxNullBitmap);
      }


//    And finally, blit the scratch dc onto the physical dc
      wxRegionIterator upd_final(rgn_blit);
      while (upd_final)
      {
            wxRect rect = upd_final.GetRect();
            dc.Blit(rect.x, rect.y, rect.width, rect.height,
                  &scratch_dc, rect.x, rect.y);
            upd_final ++ ;
      }

//    Deselect the chart bitmap from the temp_dc, so that it will not be destroyed in the temp_dc dtor
      temp_dc.SelectObject(wxNullBitmap);
//    And for the scratch bitmap
      scratch_dc.SelectObject(wxNullBitmap);



//    Handle the current graphic window, if present

      if(pCwin)
      {
            pCwin->Show();
            if(bNewMap)
            {
                  pCwin->bForceRedraw = true;
                  pCwin->Refresh();
                  pCwin->Update();
            }
      }

//    And set flags for next time
      bNewVP = false;
      m_bForceReDraw = false;

      dc.DestroyClippingRegion();

      in_paint--;

//    Handle deferred WarpPointer
      if(warp_flag)
      {
            WarpPointer(warp_x, warp_y);
            warp_flag = false;
      }


}


void ChartCanvas::SetMyCursor(wxCursor *c)
{
      pPriorCursor = c;
      SetCursor(*c);
}


//----------------------------------------------------------------------------
//  Get a wxBitmap with wxMask associated containing the semi-static overlays
//----------------------------------------------------------------------------

wxBitmap *ChartCanvas::DrawTCCBitmap(bool bAddNewSelpoints)
{
    wxBitmap *p_bmp = new wxBitmap(VPoint.pix_width, VPoint.pix_height, -1);

    //      Here is the drawing DC
    wxMemoryDC ssdc;
    ssdc.SelectObject(*p_bmp);
    ssdc.SetBackground(*wxBLACK_BRUSH);
    ssdc.Clear();

    //      Believe it or not, it is faster to REDRAW the overlay objects
    //      onto a mon bitmap, and then convert it into a mask bitmap
    //      than it is to create a mask from a colour bmp.
    //      Look at the wx code.  It goes through wxImage conversion, etc...
    //      So, create a mono DC, drawing white-on-black
    wxMemoryDC ssdc_mask;
    wxBitmap mask_bmp(VPoint.pix_width, VPoint.pix_height, 1);
    ssdc_mask.SelectObject(mask_bmp);

    //      On X11, the drawing is Black on White, and the mask bitmap is inverted before
    //      making into a mask.
    //      On MSW and GTK, the drawing is White on Black, and no inversion is required
    //      Todo....  Some wxWidgets problem with this....
#ifndef __WXX11__
    ssdc_mask.SetBackground(*wxBLACK_BRUSH);
#endif

    ssdc_mask.Clear();

//    Maybe draw the Tide Points

      if(bShowTide)
      {
                                                      // Rebuild Selpoints list on new map
            DrawAllTidesInBBox(ssdc,      VPoint.vpBBox, bAddNewSelpoints);
            DrawAllTidesInBBox(ssdc_mask, VPoint.vpBBox, false, true);       // onto the mask
            bShowingTide = true;
      }
      else
            bShowingTide = false;

//    Maybe draw the current arrows
      if(bShowCurrent)
      {
            if(bShowingCurrent)
            {
                                                      // Rebuild Selpoints list on new map
                                                      // and force redraw
                  DrawAllCurrentsInBBox(ssdc,      VPoint.vpBBox, bAddNewSelpoints, true);
                  DrawAllCurrentsInBBox(ssdc_mask, VPoint.vpBBox, false,            true, true);  // onto the mask
            }
            else
            {
                  DrawAllCurrentsInBBox(ssdc, VPoint.vpBBox, true, true); // Force Selpoints add first time after
                  DrawAllCurrentsInBBox(ssdc_mask, VPoint.vpBBox, false, true, true);       // onto the mask
            }
            bShowingCurrent = true;
      }
      else
            bShowingCurrent = false;

//    Chart Outlines
      if(g_bShowOutlines)
      {
          //    Todo... Speed this up....
        RenderAllChartOutlines(&ssdc, VPoint) ;
        RenderAllChartOutlines(&ssdc_mask, VPoint, true) ;       // onto the mask
      }
    ssdc.SelectObject(wxNullBitmap);

#ifdef __WXX11__
   //      Invert the mono bmp, to make a useable mask bmp
    wxMemoryDC ssdc_mask_invert;
    wxBitmap mask_bmp_invert(VPoint.pix_width, VPoint.pix_height, 1);
    ssdc_mask_invert.SelectObject(mask_bmp_invert);
    ssdc_mask_invert.Blit(0, 0, VPoint.pix_width, VPoint.pix_height,
                          &ssdc_mask, 0, 0, wxSRC_INVERT);

    ssdc_mask_invert.SelectObject(wxNullBitmap);
    pss_overlay_mask = new wxMask(mask_bmp_invert);
    ssdc_mask.SelectObject(wxNullBitmap);
#else
    ssdc_mask.SelectObject(wxNullBitmap);
    pss_overlay_mask = new wxMask(mask_bmp);
#endif

    //      Create and associate the mask
    p_bmp->SetMask(pss_overlay_mask);

    return p_bmp;
}






void ChartCanvas::DrawAllRoutesInBBox(wxDC& dc, wxBoundingBox& BltBBox)

{
      wxBoundingBox bbx;
      wxRouteListNode *node = pRouteList->GetFirst();
      while(node)
      {
            Route *ppp = NULL;
            Route *pRouteDraw = node->GetData();
            wxObject *pp1 = pRouteDraw;

//    MSVC40D throws an exception if the cast is bad, so catch it here
//    just so that the dialog box doesn't pop up.
            try
            {
                  ppp = dynamic_cast<Route *>(pp1);
            }
            catch(...)
            {
                  wxLogMessage("chcanv::DrawAllRoutesInBBox...Exception!");
            }

            pRouteDraw = (Route *)ppp;
            if(pRouteDraw)
            {
                  if(1/*!pRouteDraw->m_bIsBeingEdited*/)    // Todo dunno why??
                  {

                        if(BltBBox.GetValid())
                        {
                              if(BltBBox.Intersect(pRouteDraw->BBox, 0) != _OUT)
                                    pRouteDraw->DrawRoute(dc);
                              else
                              {
                                    if(pRouteDraw->m_bIsBeingEdited)
                                          pRouteDraw->DrawRoute(dc);
                              }
                        }
                        else
                              pRouteDraw->DrawRoute(dc);
                  }
            }
            else
                  wxLogError("Route* in OnPaint is bad, skipping...");

            node = node->GetNext();
      }
}

//------------------------------------------------------------------------------------------
//    Tides and Current Chart Canvas Interface
//------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------
//    Tides Support
//------------------------------------------------------------------------------------------




void ChartCanvas::DrawAllTidesInBBox(wxDC& dc, wxBoundingBox& BBox,
                              bool bRebuildSelList, bool bdraw_mono)
{

      //Todo move these pens and brushes to ChartCanvas ctor
      wxPen *pgreen_pen = wxThePenList->FindOrCreatePen(wxColour(0,255,0), 1, wxSOLID);
      wxPen *pblack_pen = wxThePenList->FindOrCreatePen(wxColour(0,0,0), 1, wxSOLID);
      wxBrush *pgreen_brush = wxTheBrushList->FindOrCreateBrush(wxColour(0,255,0), wxSOLID);
      wxBrush *pgray_brush = wxTheBrushList->FindOrCreateBrush(wxColour(96,96,96), wxSOLID);
      wxBrush *pblack_brush = wxTheBrushList->FindOrCreateBrush(wxColour(0,0,0), wxSOLID);


      if(bdraw_mono)
      {
#ifdef __WXX11__
          wxPen *pmono_pen = wxBLACK_PEN;
          wxBrush *pmono_brush = wxBLACK_BRUSH;
#else
          wxPen *pmono_pen = wxWHITE_PEN;
          wxBrush *pmono_brush = wxWHITE_BRUSH;
#endif

          pgreen_pen = pmono_pen;
          pgreen_brush = pmono_brush;
          pgray_brush = pmono_brush;
      }


      if(bRebuildSelList)
            pSelectTC->DeleteAllSelectableTypePoints(SELTYPE_TIDEPOINT);


      if(1/*BBox.GetValid()*/)
      {

            for(int i=1 ; i<ptcmgr->Get_max_IDX()+1 ; i++)
            {
                  IDX_entry *pIDX = ptcmgr->GetIDX_entry(i);

                  char type = pIDX->IDX_type;             // Entry "TCtcIUu" identifier
                  if((type == 't') || (type == 'T'))  // only Tides
                  {
                        float lon = pIDX->IDX_lon;
                        float lat = pIDX->IDX_lat;
                        if(BBox.PointInBox(lon, lat, 0))
                        {

//    Manage the point selection list
                              if(bRebuildSelList)
                                    pSelectTC->AddSelectableTCPoint(lat, lon, pIDX, SELTYPE_TIDEPOINT );

                              wxPoint r;
                              GetPointPix(lat, lon, &r);

                              wxPoint d[4];
                              int dd = 6;
                              d[0].x = r.x; d[0].y = r.y+dd;
                              d[1].x = r.x+dd; d[1].y = r.y;
                              d[2].x = r.x; d[2].y = r.y-dd;
                              d[3].x = r.x-dd; d[3].y = r.y;


                              if(1/*ptcmgr->GetTideOrCurrent15(now, i, tcvalue, dir, bnew_val)*/)
                              {
                                          pgreen_pen->SetWidth(1);
                                          dc.SetPen(*pgreen_pen);
                                          dc.SetBrush(*pgreen_brush);
                                          dc.DrawPolygon(4, d);

                                          if(type == 'T')
                                          {
                                                dc.SetPen(*pblack_pen);
                                                dc.SetBrush(*pblack_brush);
                                                dc.DrawCircle(r, 2);
                                          }
                              }
                              else
                              {
                                    dc.SetPen(*pgreen_pen);
                                    dc.SetBrush(*pgray_brush);
                                    dc.DrawPolygon(4, d);
                              }
                        }
                  }
            }
      }
}







//------------------------------------------------------------------------------------------
//    Currents Support
//------------------------------------------------------------------------------------------


void ChartCanvas::DrawAllCurrentsInBBox(wxDC& dc, wxBoundingBox& BBox,
                              bool bRebuildSelList,   bool bforce_redraw_currents, bool bdraw_mono)
{
      float tcvalue, dir;
      bool bnew_val;
      char sbuf[20];
      wxFont *pTCFont;
      float lon_last;
      float lat_last;

      wxPen *porange_pen = wxThePenList->FindOrCreatePen(wxColour(255,108,0), 1, wxSOLID);
      wxBrush *porange_brush = wxTheBrushList->FindOrCreateBrush(wxColour(255,108,0), wxSOLID);
      wxBrush *pgray_brush = wxTheBrushList->FindOrCreateBrush(wxColour(96,96,96), wxSOLID);


      if(bdraw_mono)
      {
#ifdef __WXX11__
          wxPen *pmono_pen = wxBLACK_PEN;
          wxBrush *pmono_brush = wxBLACK_BRUSH;
#else
          wxPen *pmono_pen = wxWHITE_PEN;
          wxBrush *pmono_brush = wxWHITE_BRUSH;
#endif

          porange_pen = pmono_pen;
          porange_brush = pmono_brush;
          pgray_brush = pmono_brush;
      }


      pTCFont = wxTheFontList->FindOrCreateFont(12, wxDEFAULT,wxNORMAL, wxBOLD,
                                                      FALSE, wxString("Eurostile Extended"));
      int now = time(NULL);

      if(bRebuildSelList)
            pSelectTC->DeleteAllSelectableTypePoints(SELTYPE_CURRENTPOINT);



      if(1/*BBox.GetValid()*/)
      {

            for(int i=1 ; i<ptcmgr->Get_max_IDX()+1 ; i++)
            {
                  IDX_entry *pIDX = ptcmgr->GetIDX_entry(i);
                  float lon = pIDX->IDX_lon;
                  float lat = pIDX->IDX_lat;

                  char type = pIDX->IDX_type;             // Entry "TCtcIUu" identifier
                  if((type == 'c'))             // only subordinate currents are useful
                  {                             // with directions known



//  This is a ---HACK---
//  try to avoid double current arrows.  Select the first in the list only
//  Proper fix is to correct the TCDATA index file for depth indication

                      if((BBox.PointInBox(lon, lat, 0)) && (lat != lat_last) && (lon != lon_last))
                        {

                          lon_last = lon;
                          lat_last = lat;

//    Manage the point selection list
                              if(bRebuildSelList)
                                    pSelectTC->AddSelectableTCPoint(lat, lon, pIDX, SELTYPE_CURRENTPOINT);

                              wxPoint r;
                              GetPointPix(lat, lon, &r);

                              wxPoint d[4];
                              int dd = 6;
                              d[0].x = r.x; d[0].y = r.y+dd;
                              d[1].x = r.x+dd; d[1].y = r.y;
                              d[2].x = r.x; d[2].y = r.y-dd;
                              d[3].x = r.x-dd; d[3].y = r.y;



                              if(ptcmgr->GetTideOrCurrent15(now, i, tcvalue, dir, bnew_val))
                              {
                                          porange_pen->SetWidth(1);
                                          dc.SetPen(*porange_pen);
                                          dc.SetBrush(*porange_brush);
                                          dc.DrawPolygon(4, d);

                                    if(VPoint.chart_scale < 1000000)
                                    {
                                          if(bnew_val || bforce_redraw_currents)
                                          {

//    Get the display pixel location of the current station
                                                int pixxc, pixyc;
                                                Current_Ch->latlong_to_pix_vp(lat, lon,
                                                                        pixxc, pixyc, VPoint);
//    Draw arrow using preset parameters, see mm_per_knot variable
                                                float scale = fabs(tcvalue) * current_draw_scaler;

                                                porange_pen->SetWidth(2);
                                                dc.SetPen(*porange_pen);
                                                DrawArrow(dc, pixxc, pixyc, dir - 90, scale/100);



// Draw text, if enabled

                                                if(bDrawCurrentValues)
                                                {
                                                      dc.SetFont(*pTCFont);
                                                      sprintf(&sbuf[0], "%3.1f", fabs(tcvalue));
                                                      dc.DrawText(wxString(sbuf), pixxc, pixyc);
                                                }
                                          }
                                    }           // scale
                              }
                              else
                              {
                                    dc.SetPen(*porange_pen);
                                    dc.SetBrush(*pgray_brush);
                                    dc.DrawPolygon(4, d);
                              }


                        }
                  }
            }
      }
}


void ChartCanvas::DrawTCWindow(int x, int y, void *pvIDX)
{
      pCwin = new TCWin(this, x, y, pvIDX);

}

#define NUM_CURRENT_ARROW_POINTS 9
       static wxPoint CurrentArrowArray[NUM_CURRENT_ARROW_POINTS] =
      {wxPoint(0, 0),
       wxPoint(0, -10),
       wxPoint(55, -10),
       wxPoint(55, -25),
       wxPoint(100, 0),
       wxPoint(55, 25),
       wxPoint(55, 10),
       wxPoint(0, 10),
       wxPoint(0,0)};

void ChartCanvas::DrawArrow(wxDC& dc, int x, int y, float rot_angle, float scale)
  {

      float sin_rot = sin(rot_angle * PI / 180.);
      float cos_rot = cos(rot_angle * PI / 180.);

      // Move to the first point

      float xt = CurrentArrowArray[0].x;
      float yt = CurrentArrowArray[0].y;

      float xp = (xt * cos_rot) - (yt * sin_rot);
      float yp = (xt * sin_rot) + (yt * cos_rot);
      int x1 = (int)(xp * scale);
      int y1 = (int)(yp * scale);

      // Walk thru the point list
      for(int ip=1 ; ip < NUM_CURRENT_ARROW_POINTS ; ip++)
      {
            xt = CurrentArrowArray[ip].x;
            yt = CurrentArrowArray[ip].y;

            float xp = (xt * cos_rot) - (yt * sin_rot);
            float yp = (xt * sin_rot) + (yt * cos_rot);
            int x2 = (int)(xp * scale);
            int y2 = (int)(yp * scale);

            dc.DrawLine(x1 + x, y1 + y, x2 + x, y2 + y);

            x1 = x2;
            y1 = y2;
      }
  }





//------------------------------------------------------------------------------
//    TCwin Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(TCWin, wxWindow)
  EVT_PAINT(TCWin::OnPaint)
  EVT_SIZE(TCWin::OnSize)
  EVT_MOUSE_EVENTS(TCWin::MouseEvent)
  EVT_BUTTON(wxID_OK, TCWin::OKEvent)
  EVT_BUTTON(ID_TCWIN_NX, TCWin::NXEvent)
  EVT_BUTTON(ID_TCWIN_PR, TCWin::PREvent)
  EVT_CLOSE(TCWin::OnCloseWindow)
END_EVENT_TABLE()

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(SplineList);



// Define a constructor
TCWin::TCWin(ChartCanvas *parent, int x, int y, void *pvIDX):
 wxDialog(parent, wxID_ANY,   wxString("Current"), wxPoint(x,y), wxSize(500,400),
                  wxCLIP_CHILDREN | wxDEFAULT_DIALOG_STYLE )
 {
      pParent = parent;

      pIDX = (IDX_entry *)pvIDX;

//    Set up plot type
      if(strchr("Tt", pIDX->IDX_type))
      {
            plot_type = TIDE_PLOT;
            SetLabel(wxString("Tide"));
      }
      else
      {
            plot_type = CURRENT_PLOT;
            SetLabel(wxString("Current"));
      }

      int sx,sy;
      GetClientSize(&sx, &sy);
      int swx,swy;
      GetSize(&swx, &swy);
      int parent_sx, parent_sy;
      pParent->GetClientSize(&parent_sx, &parent_sy);

      int xc=x;
      int yc=y;

//  Arrange for tcWindow to be always totally visible
      if((x + swx) > parent_sx)
          xc = xc-swx;
      if((y + swy) > parent_sy)
          yc = yc-swy;



      pParent->ClientToScreen(&xc, &yc);
      wxPoint r(xc,yc);
      Move(r);

      SetBackgroundColour(wxColour(200,200,200));

//    Figure out this computer timezone minute offset
      wxDateTime this_now = wxDateTime::Now();
      wxDateTime this_gmt = this_now.ToGMT();

#if wxCHECK_VERSION(2, 6, 2)
      wxTimeSpan diff = this_now.Subtract(this_gmt);
#else
      wxTimeSpan diff = this_gmt.Subtract(this_now);
#endif

      int diff_mins = diff.GetMinutes();

      int station_offset = pIDX->IDX_time_zone;
      if(this_now.IsDST())
            station_offset += 60;
      corr_mins = station_offset - diff_mins;

//    Make the station time-zone string, brutally by hand
      switch(pIDX->IDX_time_zone)
      {
            case -240:
                  strcpy(stz, "AST");
                  break;
            case -300:
                  strcpy(stz, "EST");
                  break;
            case -360:
                  strcpy(stz, "CST");
                  break;
      }
      if(this_now.IsDST())
                  stz[1] = 'D';


//    Establish the inital drawing day as today
      graphday = wxDateTime::Now();
      wxDateTime graphday_00 = wxDateTime::Today();
      int t_graphday_00 = graphday_00.GetTicks();
      t_graphday_00_at_station = t_graphday_00 - (corr_mins * 60);


      bForceRedraw = true;
      btc_valid = false;

      psList = NULL;

      OK_button = new wxButton(this, wxID_OK, wxT("OK"),
            wxPoint(sx - 100, sy - 25), wxDefaultSize);

      PR_button = new wxButton(this, ID_TCWIN_PR, wxT("Prev"),
            wxPoint(10 , sy - 25), wxSize(30, -1));

      int bsx, bsy, bpx, bpy;
      PR_button->GetSize(&bsx, &bsy);
      PR_button->GetPosition(&bpx, &bpy);

      NX_button = new wxButton(this, ID_TCWIN_NX, wxT("Next"),
            wxPoint(bpx + bsx + 5, bpy), wxSize(30, -1));

}


TCWin::~TCWin()
{
      if(psList)
      {
            psList->DeleteContents(TRUE);
            psList->Clear();
            delete psList;
      }

      pParent->m_bForceReDraw = true;
      pParent->Refresh(false);

}


void TCWin::OKEvent(wxCommandEvent& event)
{
      Hide();
      pParent->pCwin = NULL;
      pParent->m_bForceReDraw = true;
      pParent->Refresh(false);
//    pParent->Update();
      Destroy();                          // that hurts
}

void TCWin::OnCloseWindow(wxCloseEvent& event)
{
      Hide();
      pParent->pCwin = NULL;
      pParent->m_bForceReDraw = true;
//    pParent->Refresh(false);
//    pParent->Update();
      Destroy();                          // that hurts
}

void TCWin::NXEvent(wxCommandEvent& event)
{
      wxTimeSpan dm;
      wxTimeSpan dt(24,0,0,0);
      dm = dt;

      graphday += dm;

      wxDateTime graphday_00 = graphday.ResetTime();
      int t_graphday_00 = graphday_00.GetTicks();
      t_graphday_00_at_station = t_graphday_00 - (corr_mins * 60);


      bForceRedraw = true;
      btc_valid = false;
      Refresh();

}

void TCWin::PREvent(wxCommandEvent& event)
{
      wxTimeSpan dm;
      wxTimeSpan dt(-24,0,0,0);
      dm = dt;

      graphday += dm;

      wxDateTime graphday_00 = graphday.ResetTime();
      int t_graphday_00 = graphday_00.GetTicks();
      t_graphday_00_at_station = t_graphday_00 - (corr_mins * 60);


      bForceRedraw = true;
      btc_valid = false;
      Refresh();
}


void TCWin::Resize(void)
{
}

void TCWin::RePosition(void)
{
//    Position the window
      float lon = pIDX->IDX_lon;
      float lat = pIDX->IDX_lat;

      wxPoint r;
      pParent->GetPointPix(lat, lon, &r);
      pParent->ClientToScreen(&r.x, &r.y);
      Move(r);
}

void TCWin::OnPaint(wxPaintEvent& event)
{
      int x,y;
      int i;
      char sbuf[100];
      int w, h;
      float tcmax, tcmin;

      GetClientSize(&x, &y);

      wxPaintDC dc(this);

      wxString tlocn(pIDX->IDX_station_name);

      if(1/*bForceRedraw*/)
      {

      //    Make pens, etc...
            wxPen *pblack_1 = wxThePenList->FindOrCreatePen(wxColour(0,0,0), 1, wxSOLID);
            wxPen *pblack_2 = wxThePenList->FindOrCreatePen(wxColour(0,0,0), 2, wxSOLID);
            wxPen *pred_2   = wxThePenList->FindOrCreatePen(wxColour(255,0,0), 2, wxSOLID);
            wxBrush *pltgray = wxTheBrushList->FindOrCreateBrush(wxColour(150,150,150), wxSOLID);
            wxFont *pSFont = wxTheFontList->FindOrCreateFont(8, wxFONTFAMILY_SWISS,wxNORMAL,  wxFONTWEIGHT_NORMAL,
                                                      FALSE, wxString("Arial"));
            wxFont *pMFont = wxTheFontList->FindOrCreateFont(14, wxFONTFAMILY_SWISS,wxNORMAL,  wxFONTWEIGHT_NORMAL,
                                                      FALSE, wxString("Arial"));
            wxFont *pLFont = wxTheFontList->FindOrCreateFont(18, wxFONTFAMILY_SWISS,wxNORMAL, wxBOLD,
                                                      FALSE, wxString("Arial"));


            int x_graph = x * 1/10;
            int y_graph = y * 2/10;
            int x_graph_w = x * 8/10;
            int y_graph_h = y * 6/10;

      //    Box the graph
            dc.SetPen(*pblack_1);
            dc.SetBrush(*pltgray);
            dc.DrawRectangle(x_graph, y_graph, x_graph_w, y_graph_h);


      //    Horizontal axis
            dc.SetFont(*pSFont);
            for(i=0 ; i<25 ; i++)
            {
                  int xd = x_graph + ((i) * x_graph_w / 25);
                  dc.DrawLine(xd, y_graph, xd, y_graph + y_graph_h + 5);

                  char sbuf[5];
                  sprintf(sbuf, "%02d", i);
#ifdef __WXMSW__
                  dc.DrawRotatedText(wxString(sbuf), xd + (x_graph_w/25)/2, y_graph + y_graph_h + 8, 270.);
#else
                  int x_shim = -12;
                  dc.DrawText(wxString(sbuf), xd + x_shim + (x_graph_w/25)/2, y_graph + y_graph_h + 8);
#endif
            }

      //    Make a line for "right now"
            int t_now = wxDateTime::Now().GetTicks();       // now, in ticks

            float t_ratio = x_graph_w * (t_now - t_graphday_00_at_station) / (25 * 3600);

            int xnow = x_graph + (int)t_ratio;
            dc.SetPen(*pred_2);
            dc.DrawLine(xnow, y_graph, xnow, y_graph + y_graph_h);
            dc.SetPen(*pblack_1);


      //    Build the array of values, capturing max and min

            if(!btc_valid)
            {

                  float dir;
                  tcmax = -10;
                  tcmin = 10;

                  for(i=0 ; i<25 ; i++)
                  {
                        int tt = t_graphday_00_at_station + (i * 3600);
                        ptcmgr->GetTideOrCurrent(tt, pIDX->IDX_rec_num, tcv[i], dir);
                        if(tcv[i] > tcmax)
                              tcmax = tcv[i];
                        if(tcv[i] < tcmin)
                              tcmin = tcv[i];
                  }


                  ib = (int)tcmin - 1;
                  it = (int)tcmax + 1;


//    Set up the vertical parameters based on Tide or Current plot
                  if(CURRENT_PLOT == plot_type)
                  {
                        im = 2 * __max(abs(ib), abs(it));
                        plot_y_offset = y_graph_h/2;
                  }
                  else
                  {
                        im = abs(ib) + abs(it);
                        plot_y_offset = (y_graph_h * it)/im;
                  }


//    Build spline list of points
                  if(psList)                    // destroy any current list
                  {
                        psList->DeleteContents(TRUE);
                        psList->Clear();
                        delete psList;
                  }

                  psList = new SplineList;;

                  for(i = 0 ; i<25 ; i++)
                  {
                        wxPoint *pp = new wxPoint;
                        pp->x =  x_graph + ((i) * x_graph_w / 25);
                        pp->y = y_graph + (plot_y_offset) - (int)(tcv[i] * y_graph_h / im);

                        psList->Append(pp);
                  }

                  btc_valid = true;
            }


      //    Vertical Axis
            i = ib;
            while( i < it + 1)
            {
                  int yd = y_graph + (plot_y_offset) - (i * y_graph_h / im);

                  if((plot_y_offset + y_graph) == yd)
                        dc.SetPen(*pblack_2);
                  else
                        dc.SetPen(*pblack_1);

                  dc.DrawLine(x_graph, yd, x_graph+x_graph_w, yd);

                  sprintf(sbuf, "%d", i);
                  dc.DrawText(wxString(sbuf), x_graph - 20, yd - 10);
                  i++;

            }
      //    Units
            if(pIDX->pref_sta_data)
            {
                  wxString units(pIDX->pref_sta_data->units_abbrv);
                  dc.DrawText(units, x_graph - 40, y_graph + y_graph_h/2);
            }

      //  Location text
            wxString locn(pIDX->IDX_station_name);
            wxString locna, locnb;
            if(locn.Contains(wxString(",")))
            {
                  locna = locn.BeforeFirst(',');
                  locnb = locn.AfterFirst(',');
            }
            else
            {
                  locna = locn;
                  locnb.Empty();
            }



            int y_first_line = y * 2 / 100;
            dc.SetFont(*pLFont);
            dc.GetTextExtent(locna, &w, &h);
            if(w > x)
            {
                  dc.SetFont(*pMFont);
                  dc.GetTextExtent(locna, &w, &h);
            }

            dc.DrawText(locna, ((x/4 - w/2) < 0) ? 12:(x/4 - w/2) , y_first_line);

            int y_second_line = y_first_line + h + 2;
            dc.SetFont(*pMFont);
            dc.GetTextExtent(locnb, &w, &h);
            dc.DrawText(locnb, x/2 - w/2, y_second_line);

//    Reference to the master station
            if('t' == pIDX->IDX_type)
            {

                  dc.SetFont(*pMFont);
                  wxString mref(pIDX->IDX_reference_name);
                  dc.GetTextExtent(mref, &w, &h);
                  int y_master_reference = y_graph - h - 2;
                  dc.DrawText(mref, x - w, y_master_reference);

                  wxString ref_legend("Reference Station:");
                  int wl;
                  dc.GetTextExtent(ref_legend, &wl, &h);
                  dc.DrawText(ref_legend, x - w/2 - wl/2, y_master_reference - h + 4);
            }



      //    Draw the Value curve

            dc.SetPen(*pblack_2);
            dc.DrawSpline((wxList *)psList);

      //  More Info

            dc.SetFont(*pSFont);
            dc.GetTextExtent(wxString(stz), &w, &h);
            dc.DrawText(wxString(stz), x/2 - w/2, y * 88/100);


            sprintf(sbuf, "%s", graphday.Format("%#x").c_str());
            dc.SetFont(*pMFont);
            dc.GetTextExtent(wxString(sbuf), &w, &h);
            dc.DrawText(wxString(sbuf), x/2 - w/2, y * 92/100);

            Station_Data *pmsd = pIDX->pref_sta_data;
            if(pmsd)
            {
                  dc.GetTextExtent(wxString(pmsd->units_conv), &w, &h);
                  dc.DrawRotatedText(wxString(pmsd->units_conv),
                        5, y_graph + y_graph_h/2 + w/2, 90.);
            }

//    Today or tomorrow
            wxString sday;
            wxDateTime this_now = wxDateTime::Now();
            int day = graphday.GetWeekDay();
            if(day ==  this_now.GetWeekDay())
                  sday.Append("Today");
            else if(day == (this_now.GetWeekDay() + 1) % 7)
                  sday.Append("Tomorrow");
            dc.SetFont(*pSFont);
            dc.GetTextExtent(wxString(sday), &w, &h);
            dc.DrawText(wxString(sday), 55 - w/2, y * 88/100);

            bForceRedraw = false;
      }
}


void TCWin::OnSize(wxSizeEvent& event)
{
      int width,height;
      GetClientSize(&width, &height);
      int x,y;
      GetPosition(&x, &y);
}


void TCWin::MouseEvent(wxMouseEvent& event)
{

      int x,y;
      event.GetPosition(&x, &y);

}


#ifdef __WXX11__
//----------------------------------------------------------------------------------------------------------
//    ocpCursor Implementation
//----------------------------------------------------------------------------------------------------------
#include "wx/x11/private.h"


//----------------------------------------------------------------------------------------------
//      ocpCursorRefData Definition/Implementation
//----------------------------------------------------------------------------------------------

class ocpCursorRefData: public wxObjectRefData
{
      public:

            ocpCursorRefData();
            ~ocpCursorRefData();

            WXCursor     m_cursor;
            WXDisplay   *m_display;
};


ocpCursorRefData::ocpCursorRefData()
{
      m_cursor = NULL;
      m_display = NULL;
}


ocpCursorRefData::~ocpCursorRefData()
{
      if (m_cursor)
            XFreeCursor( (Display*) m_display, (Cursor) m_cursor );
}


//-----------------------------------------------------------------------------

#define M_CURSORDATA ((ocpCursorRefData *)m_refData)


/*
IMPLEMENT_DYNAMIC_CLASS(ocpCursor,wxObject)

ocpCursor::ocpCursor()
{

}
*/

//----------------------------------------------------------------------------------------------
//      ocpCursor Implementation
//
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
//      A new constructor taking a file name to load and assign as a cursor
//----------------------------------------------------------------------------------------------
ocpCursor::ocpCursor(const wxString& cursorName, long type,
                     int hotSpotX, int hotSpotY): wxCursor(wxCURSOR_CROSS)
{
      wxImage cImage;

      if(!cImage.CanRead(cursorName))
            ::wxInitAllImageHandlers();

      cImage.LoadFile(cursorName);

      int width = cImage.GetWidth();
      int height = cImage.GetHeight();

//    m_refData = new wxCursorRefData();

      //    Get some X parameters
      int xscreen = DefaultScreen( (Display*) wxGlobalDisplay() );
      Window xroot = RootWindow( (Display*) wxGlobalDisplay(), xscreen );
      Visual* xvisual = DefaultVisual( (Display*) wxGlobalDisplay(), xscreen );

      M_CURSORDATA->m_display = wxGlobalDisplay();
      wxASSERT_MSG( M_CURSORDATA->m_display, wxT("No display") );

      //    Make a pixmap
      Pixmap cpixmap = XCreatePixmap( (Display*) wxGlobalDisplay(),
            xroot, width, height, 1 );

      //    Make an Ximage
      XImage *data_image = XCreateImage( (Display*) wxGlobalDisplay(), xvisual,
                                          1, ZPixmap, 0, 0, width, height, 32, 0 );
      data_image->data = (char*) malloc( data_image->bytes_per_line * data_image->height );


      int index = 0;
      int pixel = 0;
      unsigned char* data = cImage.GetData();

        // Create mask

      Pixmap cmask;
      unsigned char mr, mg, mb;

      if (cImage.HasMask())
      {
            XImage *mask_image = XCreateImage( (Display*) wxGlobalDisplay(), xvisual,
                        1, ZPixmap, 0, 0, width, height, 32, 0 );
            mask_image->data = (char*) malloc( mask_image->bytes_per_line * mask_image->height );

            cImage.GetOrFindMaskColour(&mr, &mg, &mb);

            int rit = (mr << 16) + (mg << 8) + mb;
            for (int y = 0; y < height; y++)
            {
                  for (int x = 0; x < width; x++)
                  {
                        int ri = (int)data[index++];
                        ri += data[index++] << 8;
                        ri += data[index++] << 16;

/*
                        int ri = *(int *)(&data[index]);
                        ri &= 0x00ffffff;
                        index++;
                        index++;
                        index++;
*/
                        pixel = 1;
                        if(ri == rit)           // if data is mask value, mask pixel gets 0
                              pixel = 0;


                        XPutPixel( mask_image, x, y, pixel );


                  }
            }

            cmask = XCreatePixmap( (Display*) wxGlobalDisplay(), xroot, width, height, 1  );

            GC gc = XCreateGC( (Display*) wxGlobalDisplay(), cmask, 0, NULL );
            XPutImage( (Display*) wxGlobalDisplay(), cmask, gc, mask_image,
                        0, 0, 0, 0, width, height );

            XDestroyImage( mask_image );
            XFreeGC( (Display*) wxGlobalDisplay(), gc );

      }

      //    Render the wxImage cImage onto the Ximage
      //    Simple black/white cursors only, please

      index = 0;

      for (int y = 0; y < height; y++)
      {
            for (int x = 0; x < width; x++)
            {
                  int ri = (int)data[index++];
                  ri += data[index++] << 8;
                  ri += data[index++] << 16;

                  /*
                  int ri = *(int *)(&data[index]);
                  ri &= 0x00ffffff;
                  index++;
                  index++;
                  index++;
                  */

                  pixel = 0;
                  if(ri)
                        pixel = 1;


                  XPutPixel( data_image, x, y, pixel );


            }
      }



      //    Put the Ximage into the pixmap

      GC gc = XCreateGC( (Display*) wxGlobalDisplay(), cpixmap, 0, NULL );
      XPutImage( (Display*) wxGlobalDisplay(), cpixmap, gc, data_image,
                  0, 0, 0, 0, width, height );

      //    Free the Ximage stuff
      XDestroyImage( data_image );
      XFreeGC( (Display*) wxGlobalDisplay(), gc );

      //    Make a X cursor from the pixmap



      XColor fg, bg;
      fg.red = fg.blue = fg.green = 0xffff;
      bg.red = bg.blue = bg.green = 0;


      M_CURSORDATA->m_cursor = (WXCursor) XCreatePixmapCursor((Display*) wxGlobalDisplay(), cpixmap, cmask,
                                             &fg, &bg, hotSpotX, hotSpotY);




}

//----------------------------------------------------------------------------------------------
//      A new constructor taking a static char ** of XPM data and assign as a cursor
//----------------------------------------------------------------------------------------------

ocpCursor::ocpCursor(char **xpm_data, long type,
                     int hotSpotX, int hotSpotY): wxCursor(wxCURSOR_CROSS)
{
    wxImage cImage(xpm_data);

    int width = cImage.GetWidth();
    int height = cImage.GetHeight();

//    m_refData = new wxCursorRefData();

      //    Get some X parameters
    int xscreen = DefaultScreen( (Display*) wxGlobalDisplay() );
    Window xroot = RootWindow( (Display*) wxGlobalDisplay(), xscreen );
    Visual* xvisual = DefaultVisual( (Display*) wxGlobalDisplay(), xscreen );

    M_CURSORDATA->m_display = wxGlobalDisplay();
    wxASSERT_MSG( M_CURSORDATA->m_display, wxT("No display") );

      //    Make a pixmap
    Pixmap cpixmap = XCreatePixmap( (Display*) wxGlobalDisplay(),
                                     xroot, width, height, 1 );

      //    Make an Ximage
    XImage *data_image = XCreateImage( (Display*) wxGlobalDisplay(), xvisual,
                                        1, ZPixmap, 0, 0, width, height, 32, 0 );
    data_image->data = (char*) malloc( data_image->bytes_per_line * data_image->height );


    int index = 0;
    int pixel = 0;
    unsigned char* data = cImage.GetData();

        // Create mask

    Pixmap cmask;
    unsigned char mr, mg, mb;

    if (cImage.HasMask())
    {
        XImage *mask_image = XCreateImage( (Display*) wxGlobalDisplay(), xvisual,
                                            1, ZPixmap, 0, 0, width, height, 32, 0 );
        mask_image->data = (char*) malloc( mask_image->bytes_per_line * mask_image->height );

        cImage.GetOrFindMaskColour(&mr, &mg, &mb);

        int rit = (mr << 16) + (mg << 8) + mb;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int ri = (int)data[index++];
                ri += data[index++] << 8;
                ri += data[index++] << 16;

/*
                int ri = *(int *)(&data[index]);
                ri &= 0x00ffffff;
                index++;
                index++;
                index++;
*/
                pixel = 1;
                if(ri == rit)           // if data is mask value, mask pixel gets 0
                    pixel = 0;


                XPutPixel( mask_image, x, y, pixel );


            }
        }

        cmask = XCreatePixmap( (Display*) wxGlobalDisplay(), xroot, width, height, 1  );

        GC gc = XCreateGC( (Display*) wxGlobalDisplay(), cmask, 0, NULL );
        XPutImage( (Display*) wxGlobalDisplay(), cmask, gc, mask_image,
                    0, 0, 0, 0, width, height );

        XDestroyImage( mask_image );
        XFreeGC( (Display*) wxGlobalDisplay(), gc );

    }

      //    Render the wxImage cImage onto the Ximage
      //    Simple black/white cursors only, please

    index = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int ri = (int)data[index++];
            ri += data[index++] << 8;
            ri += data[index++] << 16;

                  /*
            int ri = *(int *)(&data[index]);
            ri &= 0x00ffffff;
            index++;
            index++;
            index++;
                  */

            pixel = 0;
            if(ri)
                pixel = 1;


            XPutPixel( data_image, x, y, pixel );


        }
    }



      //    Put the Ximage into the pixmap

    GC gc = XCreateGC( (Display*) wxGlobalDisplay(), cpixmap, 0, NULL );
    XPutImage( (Display*) wxGlobalDisplay(), cpixmap, gc, data_image,
                0, 0, 0, 0, width, height );

      //    Free the Ximage stuff
    XDestroyImage( data_image );
    XFreeGC( (Display*) wxGlobalDisplay(), gc );

      //    Make a X cursor from the pixmap



    XColor fg, bg;
    fg.red = fg.blue = fg.green = 0xffff;
    bg.red = bg.blue = bg.green = 0;


    M_CURSORDATA->m_cursor = (WXCursor) XCreatePixmapCursor((Display*) wxGlobalDisplay(), cpixmap, cmask,
    &fg, &bg, hotSpotX, hotSpotY);




}

#endif      // __WXX11__



//      We derive a class from wxCursor to create ocpCursor
//      Specifically to fix a bug in wxImage-wxBitmap conversions

#ifdef __WXMSW__

 /*
//----------------------------------------------------------------------------------------------
//      ocpCursorRefData Definition/Implementation
//----------------------------------------------------------------------------------------------
class ocpCursorRefData: public wxObjectRefData
{
      public:
            ocpCursorRefData();
            ocpCursorRefData(HCURSOR);
            ~ocpCursorRefData();
};

ocpCursorRefData::ocpCursorRefData()
{

}

ocpCursorRefData::ocpCursorRefData(HCURSOR hcursor)
{
}


ocpCursorRefData::~ocpCursorRefData()
{
}
*/

//----------------------------------------------------------------------------------------------
//      ocpCursor Implementation
//
//----------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------
//      A new constructor taking a file name to load and assign as a cursor
//----------------------------------------------------------------------------------------------


ocpCursor::ocpCursor(const wxString& cursorName, long type,
                     int hotSpotX, int hotSpotY): wxCursor(wxCURSOR_ARROW )

{
    wxImage cImage;

    if(!cImage.CanRead(cursorName))
            ::wxInitAllImageHandlers();

    cImage.LoadFile(cursorName);



//      wxMSW Bug???
//      On Windows XP, conversion from wxImage to wxBitmap fails at the ::CreateDIBitmap() call
//      unless a "compatible" dc is provided.  Why??
//      As a workaround, just make a simple wxDC for temporary use

    wxBitmap tbmp(cImage.GetWidth(),cImage.GetHeight(),-1);
    wxMemoryDC dwxdc;
    dwxdc.SelectObject(tbmp);

    HCURSOR hcursor = wxBitmapToHCURSOR( wxBitmap(cImage, (wxDC &)dwxdc),
                                         hotSpotX, hotSpotY );
    if ( !hcursor )
    {
        wxLogWarning(_("Failed to create ocpCursor."));
        return;
    }


//  Replace the HANDLE created in the base class constructor
//  Probably leaks....
    GetGDIImageData()->m_handle = hcursor;
}


//----------------------------------------------------------------------------------------------
//      A new constructor taking a static char ** of XPM data and assign as a cursor
//----------------------------------------------------------------------------------------------


ocpCursor::ocpCursor(char **xpm_data, long type,
                     int hotSpotX, int hotSpotY): wxCursor(wxCURSOR_ARROW )

{
    wxImage cImage(xpm_data);

//      wxMSW Bug???
//      On Windows XP, conversion from wxImage to wxBitmap fails at the ::CreateDIBitmap() call
//      unless a "compatible" dc is provided.  Why??
//      As a workaround, just make a simple wxDC for temporary use

    wxBitmap tbmp(cImage.GetWidth(),cImage.GetHeight(),-1);
    wxMemoryDC dwxdc;
    dwxdc.SelectObject(tbmp);

    HCURSOR hcursor = wxBitmapToHCURSOR( wxBitmap(cImage, (wxDC &)dwxdc),
                                         hotSpotX, hotSpotY );
    if ( !hcursor )
    {
        wxLogWarning(_("Failed to create ocpCursor."));
        return;
    }


//  Replace the HANDLE created in the base class constructor
//  Probably leaks....
    GetGDIImageData()->m_handle = hcursor;
}




#endif   // __MSW







//---------------------------------------------------------------------------------------
//          S57QueryDialog
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS( S57QueryDialog, wxDialog )


// S57QueryDialog event table definition

BEGIN_EVENT_TABLE( S57QueryDialog, wxDialog )
END_EVENT_TABLE()


S57QueryDialog::S57QueryDialog( )
{
      Init();
}

S57QueryDialog::S57QueryDialog( wxWindow* parent,
      wxWindowID id, const wxString& caption,
      const wxPoint& pos, const wxSize& size, long style )
{
      Init();
      Create(parent, id, caption, pos, size, style);
}

S57QueryDialog::~S57QueryDialog( )
{
      delete pQueryResult;
}


/// Initialisation
void S57QueryDialog::Init( )
{
      pQueryResult = NULL;
}

void S57QueryDialog::SetText(wxString &text_string)
{
      pQueryResult = new wxString(text_string);
}

/*
* S57QueryDialog creator
*/
bool S57QueryDialog::Create( wxWindow* parent,
            wxWindowID id, const wxString& caption,
            const wxPoint& pos, const wxSize& size, long style )
{
// We have to set extra styles before creating the
// dialog
//      SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    if (!wxDialog::Create( parent, id, caption, pos, size, style ))
            return false;

    wxFont *dFont = wxTheFontList->FindOrCreateFont(10, wxFONTFAMILY_TELETYPE,
          wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );

    SetFont(*dFont);
    CreateControls();

//      SetDialogHelp();
//      SetDialogValidators();

// This fits the dialog to the minimum size dictated by
// the sizers
    GetSizer()->Fit(this);

// This ensures that the dialog cannot be sized smaller
// than the minimum size
    GetSizer()->SetSizeHints(this);

// Centre the dialog on the parent or (if none) screen
    Centre();
    return true;
}




void S57QueryDialog::CreateControls()
{

// A top-level sizer
      wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
      this->SetSizer(topSizer);

// A second box sizer to give more space around the controls
      wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
      topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

// Here is the query result

      wxTextCtrl *pQueryTextCtl = new wxTextCtrl( this, -1, _T(""),
            wxDefaultPosition, wxSize(500, 500), wxTE_MULTILINE | wxTE_DONTWRAP | wxTE_READONLY);
      boxSizer->Add(pQueryTextCtl, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

      wxFont *qFont = wxTheFontList->FindOrCreateFont(14, wxFONTFAMILY_TELETYPE,
              wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
      pQueryTextCtl->SetFont(*qFont);

      if(pQueryResult)
            pQueryTextCtl->AppendText(*pQueryResult);

      pQueryTextCtl->SetSelection(0,0);
      pQueryTextCtl->SetInsertionPoint(0);


// A horizontal box sizer to contain Reset, OK, Cancel and Help
      wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
            boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL,
            5);
// The OK button
      wxButton* ok = new wxButton ( this, wxID_OK, wxT("&OK"),
            wxDefaultPosition, wxDefaultSize, 0 );
      okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

// The Cancel button
      wxButton* cancel = new wxButton ( this, wxID_CANCEL,
            wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
      okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

// The Help button
      wxButton* help = new wxButton( this, wxID_HELP, _T("&Help"),
            wxDefaultPosition, wxDefaultSize, 0 );
      okCancelBox->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}




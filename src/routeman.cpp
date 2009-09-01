/******************************************************************************
 * $Id: routeman.cpp,v 1.18 2009/09/01 22:19:46 bdbcat Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Route Manager
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
 * $Log: routeman.cpp,v $
 * Revision 1.18  2009/09/01 22:19:46  bdbcat
 * Correct DeleteRoute()
 *
 * Revision 1.17  2009/08/25 21:28:04  bdbcat
 * Correct delete duplicate waypoints in route
 *
 * Revision 1.16  2009/08/22 01:21:17  bdbcat
 * Tracks
 *
 * Revision 1.15  2009/08/03 03:15:57  bdbcat
 * Improve Waypoint logic
 *
 * Revision 1.14  2009/07/29 00:52:37  bdbcat
 * Update for gcc 4.2.4
 *
 * Revision 1.13  2009/07/16 02:47:01  bdbcat
 * Various
 *
 * Revision 1.12  2009/03/26 22:30:27  bdbcat
 * Opencpn 1.3.0 Update
 *
 * Revision 1.11  2008/11/12 04:14:20  bdbcat
 * Add member NMEA0183 object
 *
 * Revision 1.10  2008/08/29 02:24:40  bdbcat
 * Redefine IconImageList
 *
 * Revision 1.9  2008/08/28 02:28:04  bdbcat
 * Fix Compile bug
 *
 * Revision 1.8  2008/08/27 22:52:16  bdbcat
 * Fix wxImageList bug for  variable icon size
 *
 * Revision 1.7  2008/08/26 13:46:25  bdbcat
 * Better color scheme support
 *
 * Revision 1.6  2008/03/30 22:11:41  bdbcat
 * Add RoutePoint manager
 *
 * $Log: routeman.cpp,v $
 * Revision 1.18  2009/09/01 22:19:46  bdbcat
 * Correct DeleteRoute()
 *
 * Revision 1.17  2009/08/25 21:28:04  bdbcat
 * Correct delete duplicate waypoints in route
 *
 * Revision 1.16  2009/08/22 01:21:17  bdbcat
 * Tracks
 *
 * Revision 1.15  2009/08/03 03:15:57  bdbcat
 * Improve Waypoint logic
 *
 * Revision 1.14  2009/07/29 00:52:37  bdbcat
 * Update for gcc 4.2.4
 *
 * Revision 1.13  2009/07/16 02:47:01  bdbcat
 * Various
 *
 * Revision 1.12  2009/03/26 22:30:27  bdbcat
 * Opencpn 1.3.0 Update
 *
 * Revision 1.11  2008/11/12 04:14:20  bdbcat
 * Add member NMEA0183 object
 *
 * Revision 1.10  2008/08/29 02:24:40  bdbcat
 * Redefine IconImageList
 *
 * Revision 1.9  2008/08/28 02:28:04  bdbcat
 * Fix Compile bug
 *
 * Revision 1.8  2008/08/27 22:52:16  bdbcat
 * Fix wxImageList bug for  variable icon size
 *
 * Revision 1.7  2008/08/26 13:46:25  bdbcat
 * Better color scheme support
 *
 * Revision 1.6  2008/03/30 22:11:41  bdbcat
 * Add RoutePoint manager
 *
 * Revision 1.5  2008/01/12 06:20:56  bdbcat
 * Update for Mac OSX/Unicode
 *
 * Revision 1.4  2007/06/10 02:32:30  bdbcat
 * Improve mercator bearing calculation
 *
 * Revision 1.3  2007/05/03 13:23:56  dsr
 * Major refactor for 1.2.0
 *
 * Revision 1.2  2006/09/21 01:37:36  dsr
 * Major refactor/cleanup
 *
 * Revision 1.1.1.1  2006/08/21 05:52:19  dsr
 * Initial import as opencpn, GNU Automake compliant.
 *
 * Revision 1.3  2006/08/04 11:42:02  dsr
 * no message
 *
 * Revision 1.2  2006/07/28 20:38:19  dsr
 * New AP interface
 *
 * Revision 1.1.1.1  2006/04/19 03:23:28  dsr
 * Rename/Import to OpenCPN
 *
 * Revision 1.5  2006/04/19 02:01:56  dsr
 * Compute Normal arrival distance
 *
 * Revision 1.4  2006/03/16 03:08:24  dsr
 * Cleanup tabs
 *
 * Revision 1.3  2006/02/23 01:46:00  dsr
 * Cleanup
 *
 *
 */

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/image.h"

#include "dychart.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <wx/listimpl.cpp>

#include "routeman.h"
#include "concanv.h"
#include "nmea.h"                   // for Autopilot
#include "navutil.h"
#include "georef.h"


//    Include a (large) set of XPM images for mark/waypoint icons
#include "bitmaps/empty.xpm"
#include "bitmaps/airplane.xpm"
#include "bitmaps/anchorage.xpm"
#include "bitmaps/anchor.xpm"
#include "bitmaps/boarding.xpm"
#include "bitmaps/boundary.xpm"
#include "bitmaps/bouy1.xpm"
#include "bitmaps/bouy2.xpm"
#include "bitmaps/campfire.xpm"
#include "bitmaps/camping.xpm"
#include "bitmaps/circle.xpm"
#include "bitmaps/coral.xpm"
#include "bitmaps/fishhaven.xpm"
#include "bitmaps/fishing.xpm"
#include "bitmaps/fish.xpm"
#include "bitmaps/float.xpm"
#include "bitmaps/food.xpm"
#include "bitmaps/fuel.xpm"
#include "bitmaps/greenlite.xpm"
#include "bitmaps/kelp.xpm"
#include "bitmaps/light1.xpm"
#include "bitmaps/light.xpm"
#include "bitmaps/litevessel.xpm"
#include "bitmaps/mob.xpm"
#include "bitmaps/mooring.xpm"
#include "bitmaps/oilbouy.xpm"
#include "bitmaps/platform.xpm"
#include "bitmaps/redgreenlite.xpm"
#include "bitmaps/redlite.xpm"
#include "bitmaps/rock1.xpm"
#include "bitmaps/rock2.xpm"
#include "bitmaps/sand.xpm"
#include "bitmaps/scuba.xpm"
#include "bitmaps/shoal.xpm"
#include "bitmaps/snag.xpm"
#include "bitmaps/square.xpm"
#include "bitmaps/triangle.xpm"
#include "bitmaps/wreck1.xpm"
#include "bitmaps/wreck2.xpm"
#include "bitmaps/xmblue.xpm"
#include "bitmaps/xmgreen.xpm"
#include "bitmaps/xmred.xpm"
#include "bitmaps/diamond.xpm"
#include "bitmaps/activepoint.xpm"

extern "C" float DistGreatCircle(double slat, double slon, double dlat, double dlon);


extern ConsoleCanvas    *console;

extern RouteList        *pRouteList;
extern Select           *pSelect;
extern MyConfig         *pConfig;
extern NMEA0183         *pNMEA0183;
extern AutoPilotWindow  *pAPilot;
extern WayPointman      *pWayPointMan;
extern wxRect           g_blink_rect;

extern double           gLat, gLon, gSog, gCog;

//    List definitions for Waypoint Manager Icons
WX_DECLARE_LIST(wxBitmap, markicon_bitmap_list_type);
WX_DECLARE_LIST(wxString, markicon_key_list_type);
WX_DECLARE_LIST(wxString, markicon_description_list_type);


//    List implementation for Waypoint Manager Icons
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(markicon_bitmap_list_type);
WX_DEFINE_LIST(markicon_key_list_type);
WX_DEFINE_LIST(markicon_description_list_type);


CPL_CVSID("$Id: routeman.cpp,v 1.18 2009/09/01 22:19:46 bdbcat Exp $");

//--------------------------------------------------------------------------------
//      Routeman   "Route Manager"
//--------------------------------------------------------------------------------

Routeman::Routeman()
{
        pActiveRoute = NULL;
        pActivePoint = NULL;
        pRouteActivatePoint = NULL;
}

Routeman::~Routeman()
{
        if( pRouteActivatePoint)
                delete  pRouteActivatePoint;
}

//    Make a 2-D search to find the route containing a given waypoint
Route *Routeman::FindRouteContainingWaypoint(RoutePoint *pWP)
{
      wxRouteListNode *node = pRouteList->GetFirst();
      while(node)
      {
            Route *proute = node->GetData();

            wxRoutePointListNode *pnode = (proute->pRoutePointList)->GetFirst();
            while(pnode)
            {
                  RoutePoint *prp = pnode->GetData();
                  if(prp == pWP)                // success
                        return proute;

                  pnode = pnode->GetNext();
            }

            node = node->GetNext();
      }

      return NULL;                              // not found
}

wxArrayPtrVoid *Routeman::GetRouteArrayContaining(RoutePoint *pWP)
{
      wxArrayPtrVoid *pArray = new wxArrayPtrVoid;

      wxRouteListNode *route_node = pRouteList->GetFirst();
      while(route_node)
      {
            Route *proute = route_node->GetData();

            wxRoutePointListNode *waypoint_node = (proute->pRoutePointList)->GetFirst();
            while(waypoint_node)
            {
                  RoutePoint *prp = waypoint_node->GetData();
                  if(prp == pWP)                // success
                        pArray->Add((void *)proute);

                  waypoint_node = waypoint_node->GetNext();           // next waypoint
            }

            route_node = route_node->GetNext();                         // next route
      }

      if(pArray->GetCount())
            return pArray;

      else
      {
            delete pArray;
            return NULL;
      }
}


bool Routeman::ActivateRoute(Route *pActivate)
{
        pActiveRoute = pActivate;

        wxRoutePointListNode *node = (pActiveRoute->pRoutePointList)->GetFirst();

        pActivePoint = node->GetData();               // start at beginning

        ActivateRoutePoint(pActivate, pActivePoint);
        /*
        pActiveRoute->m_pRouteActivePoint = pActivePoint;
        pActivePoint->m_bBlink = true;

//      Calculate initial course to first route point

        pRouteActivatePoint = new RoutePoint(gLat, gLon, wxString(_T("")), wxString(_T("")), NULL);       // Current location
        pRouteActivatePoint->m_bShowName = false;

        pActiveRouteSegmentBeginPoint = pRouteActivatePoint;
        */

        m_bArrival = false;

        pActivate->m_bRtIsActive = true;

        m_bDataValid = false;

        console->ShowWithFreshFonts();
        return true;
}

bool Routeman::ActivateRoutePoint(Route *pA, RoutePoint *pRP_target)
{
        pActiveRoute = pA;
        pActivePoint = pRP_target;
        pActiveRoute->m_pRouteActivePoint = pRP_target;

        wxRoutePointListNode *node = (pActiveRoute->pRoutePointList)->GetFirst();
        while(node)
        {
              RoutePoint *pn = node->GetData();
              pn->m_bBlink = false;                     // turn off all blinking points
              pn->m_bIsActive = false;

              node = node->GetNext();
        }

        node = (pActiveRoute->pRoutePointList)->GetFirst();
        RoutePoint *prp_first = node->GetData();

        //  If activating first point in route, create a "virtual" waypoint at present position
        if(pRP_target == prp_first)
        {
                if(pRouteActivatePoint)
                        delete pRouteActivatePoint;

                pRouteActivatePoint = new RoutePoint(gLat, gLon, wxString(_T("")), wxString(_T("")), NULL); // Current location
                pRouteActivatePoint->m_bShowName = false;

                pActiveRouteSegmentBeginPoint = pRouteActivatePoint;
        }

        else
        {
              prp_first->m_bBlink = false;
              node = node->GetNext();
              RoutePoint *np_prev = prp_first;
              while(node)
              {
                  RoutePoint *pnext = node->GetData();
                  if(pnext == pRP_target)
                  {
                          pActiveRouteSegmentBeginPoint = np_prev;
                          break;
                  }

                  np_prev = pnext;
                  node = node->GetNext();
            }
        }

        pRP_target->m_bBlink = true;                               // blink the active point
        pRP_target->m_bIsActive = true;                            // and active

        g_blink_rect = pRP_target->CurrentRect_in_DC;               // set up global blinker

        m_bArrival = false;
        return true;
}

bool Routeman::ActivateNextPoint(Route *pr)
{
      if(pActivePoint)
      {
            pActivePoint->m_bBlink = false;
            pActivePoint->m_bIsActive = false;
      }

      int n_index_active = pActiveRoute->GetIndexOf(pActivePoint);
      if((n_index_active + 1) <= pActiveRoute->GetnPoints())
      {
          pActiveRouteSegmentBeginPoint = pActivePoint;

          pActiveRoute->m_pRouteActivePoint = pActiveRoute->GetPoint(n_index_active + 1);

          pActivePoint = pActiveRoute->GetPoint(n_index_active + 1);

          pActivePoint->m_bBlink = true;
          pActivePoint->m_bIsActive = true;
          g_blink_rect = pActivePoint->CurrentRect_in_DC;               // set up global blinker

          m_bArrival = false;

          return true;
      }

      return false;
}




bool Routeman::UpdateProgress()
{
    bool bret_val = false;

        if(pActiveRoute)
        {
//      Update bearing, range, and crosstrack error
                double north, east;
                toSM(pActivePoint->m_lat, pActivePoint->m_lon, gLat, gLon, &east, &north);
                double a = atan(north / east);
                if(pActivePoint->m_lon > gLon)
                    CurrentBrgToActivePoint = 90. - (a * 180/PI);
                else
                    CurrentBrgToActivePoint = 270. - (a * 180/PI);


//      Calculate range using Great Circle Formula

                float d5 = DistGreatCircle(gLat, gLon, pActivePoint->m_lat, pActivePoint->m_lon );
                CurrentRngToActivePoint = d5;

//      Get the XTE vector, normal to current segment
                VECTOR2D va, vb, vn;

                vb.x = pActivePoint->m_lon - pActiveRouteSegmentBeginPoint->m_lon;
                vb.y = pActivePoint->m_lat - pActiveRouteSegmentBeginPoint->m_lat;
                va.x = pActivePoint->m_lon - gLon;
                va.y = pActivePoint->m_lat - gLat;

                double delta = vGetLengthOfNormal(&va, &vb, &vn);
                CurrentXTEToActivePoint = delta * 60.;

//    Calculate the distance to the arrival line, which is perpendicular to the current route segment
//    Taking advantage of the calculated normal from current position to route segment vn
                VECTOR2D vToArriveNormal;
                vSubtractVectors(&va, &vn, &vToArriveNormal);


//                CurrentRangeToActiveNormalCrossing = vVectorMagnitude(&vToArriveNormal) * 60;

                float d6 = DistGreatCircle(gLat, gLon,
                                           gLat + vToArriveNormal.y, gLon + vToArriveNormal.x );
                CurrentRangeToActiveNormalCrossing = d6;


//          Compute current segment course
//          Using simple Mercater projection
                double x1, y1, x2, y2;
                toSM(pActiveRouteSegmentBeginPoint->m_lat,  pActiveRouteSegmentBeginPoint->m_lon,
                     pActiveRouteSegmentBeginPoint->m_lat,  pActiveRouteSegmentBeginPoint->m_lon, &x1, &y1);

                toSM(pActivePoint->m_lat,  pActivePoint->m_lon,
                     pActiveRouteSegmentBeginPoint->m_lat,  pActiveRouteSegmentBeginPoint->m_lon, &x2, &y2);

                double e1 = atan2((x2 - x1), (y2-y1));
                CurrentSegmentCourse = e1 * 180/PI;
                if(CurrentSegmentCourse < 0)
                      CurrentSegmentCourse += 360;


 //      Compute XTE direction
                double h = atan(vn.y / vn.x);
                if(vn.x > 0)
                        CourseToRouteSegment = 90. - (h * 180/PI);
                else
                        CourseToRouteSegment = 270. - (h * 180/PI);

                h= CurrentBrgToActivePoint - CourseToRouteSegment;
                if(h < 0 )
                        h = h + 360;

                if(h > 180)
                        XTEDir = 1;
                else
                        XTEDir = -1;



//      Determine Arrival
                double ArrivalRadius = .05;

                bool bDidArrival = false;

//                if(CurrentRngToActivePoint <= ArrivalRadius)
                if(CurrentRangeToActiveNormalCrossing <= ArrivalRadius)
                {
                  m_bArrival = true;
                  UpdateAutopilot();

                  bDidArrival = true;

                  if(!ActivateNextPoint(pActiveRoute))            // at the end?
                          DeactivateRoute();
                }

                if(!bDidArrival)                                        // Only once on arrival
                        UpdateAutopilot();

                bret_val = true;                                        // a route is active
        }

        m_bDataValid = true;

        return bret_val;
}

bool Routeman::DeactivateRoute()
{
      if(pActivePoint)
      {
            pActivePoint->m_bBlink = false;
            pActivePoint->m_bIsActive = false;
      }


      if(pActiveRoute)
      {
          pActiveRoute->m_bRtIsActive = false;
          pActiveRoute->m_pRouteActivePoint = NULL;
      }
      pActiveRoute = NULL;

      if(pRouteActivatePoint)
          delete  pRouteActivatePoint;
      pRouteActivatePoint = NULL;

      console->pCDI->ClearBackground();

      console->Show(false);

      m_bDataValid = false;

      return true;
}

bool Routeman::UpdateAutopilot()
{
        wxString str_buf;

        if(pAPilot->IsOK())
        {
                m_NMEA0183.TalkerID = _T("EC");

                SENTENCE snt;
                m_NMEA0183.Rmb.IsDataValid = NTrue;
                m_NMEA0183.Rmb.CrossTrackError = CurrentXTEToActivePoint;

                if(XTEDir < 0)
                      m_NMEA0183.Rmb.DirectionToSteer = Left;
                else
                      m_NMEA0183.Rmb.DirectionToSteer = Right;


                m_NMEA0183.Rmb.To = pActivePoint->m_MarkName.Truncate(6);
                m_NMEA0183.Rmb.From = pActiveRouteSegmentBeginPoint->m_MarkName.Truncate(6);

//                str_buf.Printf(_T("%03d"), pActiveRoute->GetIndexOf(pActiveRouteSegmentBeginPoint));
//                wxString from = str_buf;
//                m_NMEA0183.Rmb.From = from;

//                str_buf.Printf(_T("%03d"), pActiveRoute->GetIndexOf(pActivePoint));
//                wxString to = str_buf;
//                m_NMEA0183.Rmb.To = to;

                if(pActivePoint->m_lat < 0.)
                      m_NMEA0183.Rmb.DestinationPosition.Latitude.Set(-pActivePoint->m_lat, _T("S"));
                else
                      m_NMEA0183.Rmb.DestinationPosition.Latitude.Set(pActivePoint->m_lat, _T("N"));

                if(pActivePoint->m_lon < 0.)
                      m_NMEA0183.Rmb.DestinationPosition.Longitude.Set(-pActivePoint->m_lon, _T("W"));
                else
                      m_NMEA0183.Rmb.DestinationPosition.Longitude.Set(pActivePoint->m_lon, _T("E"));

 //               m_NMEA0183.Rmb.DestinationPosition.Latitude.Latitude = pActivePoint->m_lat;
 //               m_NMEA0183.Rmb.DestinationPosition.Latitude.Northing = North;

 //               m_NMEA0183.Rmb.DestinationPosition.Longitude.Longitude = fabs(pActivePoint->m_lon);
 //               m_NMEA0183.Rmb.DestinationPosition.Longitude.Easting = West;


                m_NMEA0183.Rmb.RangeToDestinationNauticalMiles = CurrentRngToActivePoint;
                m_NMEA0183.Rmb.BearingToDestinationDegreesTrue = CurrentBrgToActivePoint;
                m_NMEA0183.Rmb.DestinationClosingVelocityKnots = gSog;

                if(m_bArrival)
                      m_NMEA0183.Rmb.IsArrivalCircleEntered = NTrue;
                else
                      m_NMEA0183.Rmb.IsArrivalCircleEntered = NFalse;

                m_NMEA0183.Rmb.Write(snt);

        //      stats->pTStat2->TextDraw(( const char *)snt.Sentence);

                pAPilot->AutopilotOut(snt.Sentence);
                }

        return true;
}

void Routeman::DeleteRoute(Route *pRoute)
{
      if(pRoute)
      {
            //    Remove the route from associated lists
            pSelect->DeleteAllSelectableRouteSegments(pRoute);
            pRouteList->DeleteObject(pRoute);

            // walk the route, tentatively deleting/marking points used only by this route
            wxRoutePointListNode *pnode = (pRoute->pRoutePointList)->GetFirst();
            while(pnode)
            {
                  RoutePoint *prp = pnode->GetData();

                  // check all other routes to see if this point appears in any other route
                  Route *pcontainer_route = FindRouteContainingWaypoint(prp);

                  if(pcontainer_route == NULL)
                  {
                        prp->m_bIsInRoute = false;          // Take this point out of this (and only) route
                        if(!prp->m_bKeepXRoute)
                        {
                              pConfig->DeleteWayPoint(prp);
                              pSelect->DeleteSelectablePoint(prp, SELTYPE_ROUTEPOINT);
                              pRoute->pRoutePointList->DeleteNode(pnode);
                              pnode = NULL;
                              delete prp;

                              //    Remove any duplicates in the list
                              bool done = false;
                              while(!done)
                              {
                                    wxRoutePointListNode *pdnode = pRoute->pRoutePointList->Find(prp);
                                    if(pdnode)
                                          pRoute->pRoutePointList->DeleteNode(pdnode);
                                    else
                                          done = true;
                              }

                        }

                  }
                  if(pnode)
                        pnode = pnode->GetNext();
                  else
                        pnode = pRoute->pRoutePointList->GetFirst();                // restart the list
            }


            delete pRoute;
      }
}

void Routeman::DeleteAllRoutes(void)
{
            //    Iterate on the RouteList
      wxRouteListNode *node = pRouteList->GetFirst();
      while(node)
      {
            Route *proute = node->GetData();

            DeleteRoute(proute);

            node = pRouteList->GetFirst();                   // Route
      }

}


void Routeman::AssembleAllRoutes(void)
{
      //    Iterate on the RouteList
      wxRouteListNode *node = pRouteList->GetFirst();
      while(node)
      {
            Route *proute = node->GetData();

            proute->AssembleRoute();
            if(proute->GetnPoints())
            {
                  pSelect->AddAllSelectableRouteSegments(proute);
            }
            else                                // this route has no points
            {
                  pConfig->DeleteConfigRoute(proute);
                  DeleteRoute(proute);
            }

            node = node->GetNext();                   // Route
      }
}

void Routeman::SetColorScheme(ColorScheme cs)
{
      // Re-Create the pens and colors

//      m_pRoutePen =             wxThePenList->FindOrCreatePen(wxColour(0,0,255), 2, wxSOLID);
//      m_pSelectedRoutePen =     wxThePenList->FindOrCreatePen(wxColour(255,0,0), 2, wxSOLID);
//      m_pActiveRoutePen =       wxThePenList->FindOrCreatePen(wxColour(255,0,255), 2, wxSOLID);
      m_pActiveRoutePointPen =  wxThePenList->FindOrCreatePen(wxColour(0,0,255), 2, wxSOLID);
      m_pRoutePointPen =        wxThePenList->FindOrCreatePen(wxColour(0,0,255), 2, wxSOLID);


//    Or in something like S-52 compliance

      m_pRoutePen =             wxThePenList->FindOrCreatePen(GetGlobalColor(_T("UINFB")), 2, wxSOLID);
      m_pSelectedRoutePen =     wxThePenList->FindOrCreatePen(GetGlobalColor(_T("CHRED")), 2, wxSOLID);
      m_pActiveRoutePen =       wxThePenList->FindOrCreatePen(GetGlobalColor(_T("PLRTE")), 2, wxSOLID);
//      m_pActiveRoutePointPen =  wxThePenList->FindOrCreatePen(GetGlobalColor(_T("PLRTE")), 2, wxSOLID);
//      m_pRoutePointPen =        wxThePenList->FindOrCreatePen(GetGlobalColor(_T("CHBLK")), 2, wxSOLID);



      m_pRouteBrush =             wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("UINFB")), wxSOLID);
      m_pSelectedRouteBrush =     wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("CHRED")), wxSOLID);
      m_pActiveRouteBrush =       wxTheBrushList->FindOrCreateBrush(GetGlobalColor(_T("PLRTE")), wxSOLID);
//      m_pActiveRoutePointBrush =  wxTheBrushList->FindOrCreatePen(GetGlobalColor(_T("PLRTE")), wxSOLID);
//      m_pRoutePointBrush =        wxTheBrushList->FindOrCreatePen(GetGlobalColor(_T("CHBLK")), wxSOLID);

 }



//-------------------------------------------------------------------------------
//
//   Route "Send to GPS..." Dialog Implementation
//
//-------------------------------------------------------------------------------


IMPLEMENT_DYNAMIC_CLASS( SendToGpsDlg, wxDialog )

BEGIN_EVENT_TABLE( SendToGpsDlg, wxDialog )
      EVT_BUTTON( ID_STG_CANCEL, SendToGpsDlg::OnCancelClick )
      EVT_BUTTON( ID_STG_OK, SendToGpsDlg::OnSendClick )
END_EVENT_TABLE()



SendToGpsDlg::SendToGpsDlg( )
 {
       m_itemCommListBox = NULL;
       m_pgauge = NULL;
       m_SendButton = NULL;
       m_CancelButton = NULL;
       m_pRoute = NULL;
 }

 SendToGpsDlg::SendToGpsDlg(  wxWindow* parent, wxWindowID id,
                      const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
      Create(parent, id, caption, pos, size, style);
}

SendToGpsDlg::~SendToGpsDlg( )
{
      delete m_itemCommListBox;
      delete m_pgauge;
      delete m_SendButton;
      delete m_CancelButton;
}




bool SendToGpsDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
      SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
      wxDialog::Create( parent, id, caption, pos, size, style );

      CreateControls();
      GetSizer()->Fit(this);
      GetSizer()->SetSizeHints(this);
      Centre();

      return TRUE;
}


void SendToGpsDlg::CreateControls()
{
      SendToGpsDlg* itemDialog1 = this;

      wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
      itemDialog1->SetSizer(itemBoxSizer2);

//      Create the ScrollBox list of available com ports in a labeled staic box

      wxStaticBox* comm_box = new wxStaticBox(this, wxID_ANY, _T("GPS/Plotter Port"));

      wxStaticBoxSizer* comm_box_sizer = new wxStaticBoxSizer(comm_box, wxVERTICAL);
      itemBoxSizer2->Add(comm_box_sizer, 0, wxEXPAND|wxALL, 5);


      wxArrayString *pSerialArray = EnumerateSerialPorts();

      m_itemCommListBox = new wxComboBox(this, ID_STG_CHOICE_COMM);

      //    Fill in the listbox with all detected serial ports
      for (unsigned int iPortIndex=0 ; iPortIndex < pSerialArray->GetCount() ; iPortIndex++)
            m_itemCommListBox->Append( pSerialArray->Item(iPortIndex) );

      //    Make the proper inital selection
      int sidx = 0;
      m_itemCommListBox->SetSelection(sidx);

      comm_box_sizer->Add(m_itemCommListBox, 0, wxEXPAND|wxALL, 5);

      //    Add a reminder text box
      itemBoxSizer2->AddSpacer(20);

      wxStaticText *premtext = new wxStaticText(this, -1, _T("Prepare GPS for Route/Waypoint upload and press Send..."));
      itemBoxSizer2->Add(premtext, 0, wxEXPAND|wxALL, 10);

      //    Create a progress gauge
      wxStaticBox* prog_box = new wxStaticBox(this, wxID_ANY, _T("Progress..."));

      wxStaticBoxSizer* prog_box_sizer = new wxStaticBoxSizer(prog_box, wxVERTICAL);
      itemBoxSizer2->Add(prog_box_sizer, 0, wxEXPAND|wxALL, 5);

      m_pgauge = new wxGauge(this, -1, 100);
      prog_box_sizer->Add(m_pgauge, 0, wxEXPAND|wxALL, 5);


      //    OK/Cancel/etc.
      wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
      itemBoxSizer2->Add(itemBoxSizer16, 0, wxALIGN_RIGHT|wxALL, 5);

      m_CancelButton = new wxButton( itemDialog1, ID_STG_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
      itemBoxSizer16->Add(m_CancelButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

      m_SendButton = new wxButton( itemDialog1, ID_STG_OK, _("Send"), wxDefaultPosition, wxDefaultSize, 0 );
      itemBoxSizer16->Add(m_SendButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
      m_SendButton->SetDefault();

/*
                            MarkProp* itemDialog1 = this;

                            wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
                            itemDialog1->SetSizer(itemBoxSizer2);

                            wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Properties"));

                            wxStaticBoxSizer* itemStaticBoxSizer3 = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxVERTICAL);
                            itemBoxSizer2->Add(itemStaticBoxSizer3, 1, wxEXPAND|wxALL, 5);

                            wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, _("Mark Name"), wxDefaultPosition, wxDefaultSize, 0 );
                            itemStaticBoxSizer3->Add(itemStaticText4, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

                            m_MarkNameCtl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(-1, -1), 0 );
                            itemStaticBoxSizer3->Add(m_MarkNameCtl, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);

                            m_ShowNameCheckbox = new wxCheckBox( itemDialog1, ID_SHOWNAMECHECKBOX1, _("Show Name"), wxDefaultPosition, wxSize(-1, -1), 0 );
                            itemStaticBoxSizer3->Add(m_ShowNameCheckbox, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);

                            wxStaticText* itemStaticText4a= new wxStaticText( itemDialog1, wxID_STATIC, _("Mark Icon"), wxDefaultPosition, wxDefaultSize, 0 );
                            itemStaticBoxSizer3->Add(itemStaticText4a, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

                            m_IconList = new wxListCtrl( itemDialog1, ID_ICONCTRL, wxDefaultPosition, wxSize(300, 100),
                                        wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VRULES );
                            itemStaticBoxSizer3->Add(m_IconList, 2, wxEXPAND|wxALL, 5);


                            wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Position"));

                            wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
                            itemBoxSizer2->Add(itemStaticBoxSizer4, 0, wxEXPAND|wxALL, 5);

                            wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("Latitude"), wxDefaultPosition, wxDefaultSize, 0 );
                            itemStaticBoxSizer4->Add(itemStaticText5, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

                            m_MarkLatCtl = new LatLonTextCtrl( itemDialog1, ID_LATCTRL, _T(""), wxDefaultPosition, wxSize(180, -1), 0 );
                            itemStaticBoxSizer4->Add(m_MarkLatCtl, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);


                            wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, _("Longitude"), wxDefaultPosition, wxDefaultSize, 0 );
                            itemStaticBoxSizer4->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

                            m_MarkLonCtl = new LatLonTextCtrl( itemDialog1, ID_LONCTRL, _T(""), wxDefaultPosition, wxSize(180, -1), 0 );
                            itemStaticBoxSizer4->Add(m_MarkLonCtl, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);

                            wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
                            itemBoxSizer2->Add(itemBoxSizer16, 0, wxALIGN_RIGHT|wxALL, 5);

                            m_CancelButton = new wxButton( itemDialog1, ID_MARKPROP_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
                            itemBoxSizer16->Add(m_CancelButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

                            m_OKButton = new wxButton( itemDialog1, ID_MARKPROP_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
                            itemBoxSizer16->Add(m_OKButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
                            m_OKButton->SetDefault();

    //  Fill in list control

                            m_IconList->Hide();

                            int client_x, client_y;
                            m_IconList->GetClientSize(&client_x, &client_y);

                            m_IconList->SetImageList(pWayPointMan->Getpmarkicon_image_list(), wxIMAGE_LIST_SMALL);

                            wxListItem itemCol0;
                            itemCol0.SetImage(-1);
                            itemCol0.SetText(_T("Icon"));

                            m_IconList->InsertColumn(0, itemCol0);
                            m_IconList->SetColumnWidth( 0, 40 );

                            wxListItem itemCol;
                            itemCol.SetText(_T("Description"));
                            itemCol.SetImage(-1);
                            itemCol.SetAlign(wxLIST_FORMAT_LEFT);
                            m_IconList->InsertColumn(1, itemCol);
                            m_IconList->SetColumnWidth( 1, client_x - 56 );


    //      Iterate on the Icon Descriptions, filling in the control

                            for(int i = 0 ; i < pWayPointMan->GetNumIcons() ; i++)
                            {
                                  wxString *ps = pWayPointMan->GetIconDescription(i);

                                  long item_index = m_IconList->InsertItem(i, _T(""), 0);
                                  m_IconList->SetItem(item_index, 1, *ps);

                                  m_IconList->SetItemImage(item_index,i);
                            }

                            m_IconList->Show();

                            SetColorScheme((ColorScheme)0);
*/
}


void SendToGpsDlg::OnSendClick( wxCommandEvent& event )
{
      //    Get the selected comm port
      int i = m_itemCommListBox->GetSelection();
      wxString src(m_itemCommListBox->GetString(i));

      //    And send it out
      if(m_pRoute)
            m_pRoute->SendToGPS(src, true, m_pgauge);

      Show(false);
      event.Skip();
}

void SendToGpsDlg::OnCancelClick( wxCommandEvent& event )
{
      Show(false);
      event.Skip();
}

















//--------------------------------------------------------------------------------
//      WayPointman   Implementation
//--------------------------------------------------------------------------------

/*
#define MAKEICONARRAYS(key, xpm_ptr, description)\
       pmarkiconImage = new wxImage((char **)xpm_ptr);\
       pmarkiconBitmap = new wxBitmap(*pmarkiconImage, (wxDC)dwxdc);\
       delete pmarkiconImage;\
       pmi = new MarkIcon;\
       pmi->picon_bitmap = pmarkiconBitmap;\
       pmi->icon_name = _T(key);\
       pmi->icon_description = _T(description);\
       DayIconArray.Add((void *)pmi);\
       pmarkiconImage = new wxImage((char **)xpm_ptr);\
       pmarkiconBitmap = new wxBitmap(*pmarkiconImage, dwxdc);\
       delete pmarkiconImage;\
       pmarkiconBitmapDim = CreateDimBitmap(pmarkiconBitmap, .50, dwxdc);\
       delete pmarkiconBitmap;\
       pmi = new MarkIcon;\
       pmi->picon_bitmap = pmarkiconBitmapDim;\
       pmi->icon_name = _T(key);\
       pmi->icon_description = _T(description);\
       DuskIconArray.Add((void *)pmi);\
       pmarkiconImage = new wxImage((char **)xpm_ptr);\
       pmarkiconBitmap = new wxBitmap(*pmarkiconImage, dwxdc);\
       delete pmarkiconImage;\
       pmarkiconBitmapDim = CreateDimBitmap(pmarkiconBitmap, .25, dwxdc);\
       delete pmarkiconBitmap;\
       pmi = new MarkIcon;\
       pmi->picon_bitmap = pmarkiconBitmapDim;\
       pmi->icon_name = _T(key);\
       pmi->icon_description = _T(description);\
       NightIconArray.Add((void *)pmi);\
*/
#define MAKEICONARRAYS(key, xpm_ptr, description)\
 pmarkiconImage = new wxImage(( const char **)xpm_ptr);\
 ProcessIcon(pmarkiconImage, _T(key), _T(description));\
 delete pmarkiconImage;


WayPointman::WayPointman()
{

      m_pWayPointList = new RoutePointList;

      wxImage *pmarkiconImage;

      pmarkicon_image_list = NULL;

      MAKEICONARRAYS("empty", empty, "Empty")
      MAKEICONARRAYS("airplane", airplane, "Airplane")
      MAKEICONARRAYS("anchorage", anchorage, "Anchorage")
      MAKEICONARRAYS("anchor", anchor, "Anchor")
      MAKEICONARRAYS("boarding", boarding, "Boarding Location")
      MAKEICONARRAYS("boundary", boundary, "Boundary Mark")
      MAKEICONARRAYS("bouy1", bouy1, "Bouy Type A")
      MAKEICONARRAYS("bouy2", bouy2, "Bouy Type B")
      MAKEICONARRAYS("campfire", campfire, "Campfire")
      MAKEICONARRAYS("camping", camping, "Camping Spot")
      MAKEICONARRAYS("coral", coral, "Coral")
      MAKEICONARRAYS("fishhaven", fishhaven, "Fish Haven")
      MAKEICONARRAYS("fishing", fishing, "Fishing Spot")
      MAKEICONARRAYS("fish", fish, "Fish")
      MAKEICONARRAYS("floating", floating, "Float")
      MAKEICONARRAYS("food", food, "Food")
      MAKEICONARRAYS("fuel", fuel, "Fuel")
      MAKEICONARRAYS("greenlite", greenlite, "Green Light")
      MAKEICONARRAYS("kelp", kelp, "Kelp")
      MAKEICONARRAYS("light1", light1, "Light Type A")
      MAKEICONARRAYS("light", light, "Light Type B")
      MAKEICONARRAYS("litevessel", litevessel, "Light Vessel")
      MAKEICONARRAYS("mob", mob, "MOB")
      MAKEICONARRAYS("mooring", mooring, "Mooring Bouy")
      MAKEICONARRAYS("oilbouy", oilbouy, "Oil Bouy")
      MAKEICONARRAYS("platform", platform, "Platform")
      MAKEICONARRAYS("redgreenlite", redgreenlite, "Red/Green Light")
      MAKEICONARRAYS("redlite", redlite, "Red Light")
      MAKEICONARRAYS("rock1", rock1, "Rock (exposed)")
      MAKEICONARRAYS("rock2", rock2, "Rock, (awash)")
      MAKEICONARRAYS("sand", sand, "Sand")
      MAKEICONARRAYS("scuba", scuba, "Scuba")
      MAKEICONARRAYS("shoal", shoal, "Shoal")
      MAKEICONARRAYS("snag", snag, "Snag")
      MAKEICONARRAYS("square", square, "Square")
      MAKEICONARRAYS("triangle", triangle, "Triangle")
      MAKEICONARRAYS("diamond", diamond, "Diamond")
      MAKEICONARRAYS("circle", circle, "Circle")
      MAKEICONARRAYS("wreck1", wreck1, "Wreck A")
      MAKEICONARRAYS("wreck2", wreck2, "Wreck B")
      MAKEICONARRAYS("xmblue", xmblue, "Blue X")
      MAKEICONARRAYS("xmgreen", xmgreen, "Green X")
      MAKEICONARRAYS("xmred", xmred, "Red X")
      MAKEICONARRAYS("activepoint", activepoint, "Active WP")

      m_nIcons = DayIconArray.GetCount();
      m_pcurrent_icon_array = &DayIconArray;
}

WayPointman::~WayPointman()
{

      //    Two step here, since the RoutePoint dtor also touches the
      //    RoutePoint list.
      //    Copy the master RoutePoint list to a temporary list,
      //    then clear and delete objects from the temp list

      RoutePointList    temp_list;

      wxRoutePointListNode *node = m_pWayPointList->GetFirst();
      while(node)
      {
            RoutePoint *pr = node->GetData();

            temp_list.Append(pr);
            node = node->GetNext();
      }

      temp_list.DeleteContents(true);
      temp_list.Clear();


      m_pWayPointList->Clear();
      delete m_pWayPointList;

      for( unsigned int i = 0 ; i< DayIconArray.GetCount() ; i++)
      {
            MarkIcon *pmi = (MarkIcon *)NightIconArray.Item(i);
            delete pmi->picon_bitmap;
            delete pmi;

            pmi = (MarkIcon *)DuskIconArray.Item(i);
            delete pmi->picon_bitmap;
            delete pmi;

            pmi = (MarkIcon *)DayIconArray.Item(i);
            delete pmi->picon_bitmap;
            delete pmi;
      }


      NightIconArray.Empty();
      DuskIconArray.Empty();
      DayIconArray.Empty();

      if(pmarkicon_image_list)
            pmarkicon_image_list->RemoveAll();
      delete pmarkicon_image_list;
}

void WayPointman::ProcessIcon(wxImage *pimage, wxString key, wxString description)
{

      wxBitmap *pmarkiconBitmap;
      wxBitmap *pmarkiconBitmapDim;
      MarkIcon *pmi;

//    Day Icon
#ifdef __WXMSW__
//      wxMSW Bug???
//      On Windows XP, conversion from wxImage to wxBitmap fails at the ::CreateDIBitmap() call
//      unless a "compatible" dc is provided.  Why??
//      As a workaround, just make a simple wxDC for temporary use
      wxBitmap tbmp ( 1, 1,-1 );
      wxMemoryDC dwxdc;
      dwxdc.SelectObject ( tbmp );

      pmarkiconBitmap = new wxBitmap(*pimage, dwxdc);
#else
      pmarkiconBitmap = new wxBitmap(*pimage);
#endif

      pmi = new MarkIcon;
      pmi->picon_bitmap = pmarkiconBitmap;
      pmi->icon_name = key;
      pmi->icon_description = description;
      DayIconArray.Add((void *)pmi);

//    Dusk
      pmarkiconBitmapDim = CreateDimBitmap(pmarkiconBitmap, .50);
      pmi = new MarkIcon;
      pmi->picon_bitmap = pmarkiconBitmapDim;
      pmi->icon_name = key;
      pmi->icon_description = description;
      DuskIconArray.Add((void *)pmi);

//    Night
      pmarkiconBitmapDim = CreateDimBitmap(pmarkiconBitmap, .50);
      pmi = new MarkIcon;
      pmi->picon_bitmap = pmarkiconBitmapDim;
      pmi->icon_name = key;
      pmi->icon_description = description;
      NightIconArray.Add((void *)pmi);
}



wxImageList *WayPointman::Getpmarkicon_image_list(void)
{
      // First find the largest bitmap size
      int w=0;
      int h=0;

      MarkIcon *pmi;

      for( unsigned int i = 0 ; i< m_pcurrent_icon_array->GetCount() ; i++)
      {
            pmi = (MarkIcon *)m_pcurrent_icon_array->Item(i);
            w = wxMax(w, pmi->picon_bitmap->GetWidth());
            h = wxMax(h, pmi->picon_bitmap->GetHeight());
      }

      //Build an image list large enough

      if(NULL != pmarkicon_image_list)
      {
            pmarkicon_image_list->RemoveAll();
            delete pmarkicon_image_list;
      }
      pmarkicon_image_list = new wxImageList(w, h);

      //  Add the icons
      for( unsigned int ii = 0 ; ii< m_pcurrent_icon_array->GetCount() ; ii++)
      {
            pmi = (MarkIcon *)m_pcurrent_icon_array->Item(ii);
            wxImage icon_image = pmi->picon_bitmap->ConvertToImage();
            wxImage icon_larger = icon_image.Resize(wxSize(h,w), wxPoint(0,0));
            pmarkicon_image_list->Add(icon_larger);
       }

      return pmarkicon_image_list;
}

wxBitmap *WayPointman::CreateDimBitmap(wxBitmap *pBitmap, double factor)
{
      wxImage img = pBitmap->ConvertToImage();
      int sx = img.GetWidth();
      int sy = img.GetHeight();

      wxImage new_img(img);

      for(int i = 0 ; i < sx ; i++)
      {
            for(int j = 0 ; j < sy ; j++)
            {
                  if(!img.IsTransparent(i,j))
                  {
                        new_img.SetRGB(i, j, (unsigned char)(img.GetRed(i, j) * factor),
                                             (unsigned char)(img.GetGreen(i, j) * factor),
                                             (unsigned char)(img.GetBlue(i, j) * factor));
                  }
            }
      }

      wxBitmap *pret = new wxBitmap(new_img);

      return pret;

}

void WayPointman::SetColorScheme(ColorScheme cs)
{
      switch(cs)
      {
            case GLOBAL_COLOR_SCHEME_DAY:
                  m_pcurrent_icon_array = &DayIconArray;
                  break;
            case GLOBAL_COLOR_SCHEME_DUSK:
                  m_pcurrent_icon_array = &DuskIconArray;
                  break;
            case GLOBAL_COLOR_SCHEME_NIGHT:
                  m_pcurrent_icon_array = &NightIconArray;
                  break;
            default:
                  m_pcurrent_icon_array = &DayIconArray;
                  break;
      }

      //    Iterate on the RoutePoint list, requiring each to reload icon

      wxRoutePointListNode *node = m_pWayPointList->GetFirst();
      while(node)
      {
            RoutePoint *pr = node->GetData();
            pr->ReLoadIcon();
            node = node->GetNext();
      }
}


wxBitmap *WayPointman::GetIconBitmap(const wxString& icon_key)
{
      wxBitmap *pret = NULL;
      MarkIcon *pmi;
      unsigned int i;

      for( i = 0 ; i< m_pcurrent_icon_array->GetCount() ; i++)
      {
            pmi = (MarkIcon *)m_pcurrent_icon_array->Item(i);
            if(pmi->icon_name.IsSameAs(icon_key))
                  break;
      }

      if(i == m_pcurrent_icon_array->GetCount())              // not found
            pmi = (MarkIcon *)m_pcurrent_icon_array->Item(0);       // use item 0



      pret = pmi->picon_bitmap;


      return pret;
}

wxBitmap *WayPointman::GetIconBitmap(int index)
{
      wxBitmap *pret = NULL;

      if(index >= 0)
      {
            MarkIcon *pmi = (MarkIcon *)m_pcurrent_icon_array->Item(index);
            pret = pmi->picon_bitmap;
      }
      return pret;
}


wxString *WayPointman::GetIconDescription(int index)
{
      wxString *pret = NULL;

      if(index >= 0)
      {
            MarkIcon *pmi = (MarkIcon *)m_pcurrent_icon_array->Item(index);
            pret = &pmi->icon_description;
      }
      return pret;
}

wxString *WayPointman::GetIconKey(int index)
{
      wxString *pret = NULL;

      if(index >= 0)
      {
            MarkIcon *pmi = (MarkIcon *)m_pcurrent_icon_array->Item(index);
            pret = &pmi->icon_name;
      }
      return pret;
}

int WayPointman::GetIconIndex(const wxBitmap *pbm)
{
      unsigned int i;

      for( i = 0 ; i< m_pcurrent_icon_array->GetCount() ; i++)
      {
            MarkIcon *pmi = (MarkIcon *)m_pcurrent_icon_array->Item(i);
            if(pmi->picon_bitmap == pbm)
                  break;
      }

      return i;

}

     //  Create the unique identifier

wxString WayPointman::CreateGUID(RoutePoint *pRP)
{
      wxDateTime now = wxDateTime::Now();
      time_t ticks = now.GetTicks();
      wxString GUID;
      GUID.Printf(_T("%d-%d-%d"), ((int)fabs(pRP->m_lat * 1e4)), ((int)fabs(pRP->m_lon * 1e4)), (int)ticks);
      return GUID;
}

RoutePoint *WayPointman::GetNearbyWaypoint(double lat, double lon, double radius_meters)
{
      //    Iterate on the RoutePoint list, checking distance

      wxRoutePointListNode *node = m_pWayPointList->GetFirst();
      while(node)
      {
            RoutePoint *pr = node->GetData();

            double a = lat - pr->m_lat;
            double b = lon - pr->m_lon;
            double l = sqrt((a*a) + (b*b));

            if((l * 60. * 1852.) < radius_meters)
                  return pr;

            node = node->GetNext();
      }
      return NULL;

}

void WayPointman::DeleteAllWaypoints(bool b_delete_used)
{
      //    Iterate on the RoutePoint list, deleting all

      wxRoutePointListNode *node = m_pWayPointList->GetFirst();
      while(node)
      {
            RoutePoint *prp = node->GetData();

            if(b_delete_used || !prp->m_bIsInRoute)          // if argument is false, then only delete non-route waypoints
            {
                  pConfig->DeleteWayPoint ( prp );
                  pSelect->DeleteSelectablePoint ( prp, SELTYPE_ROUTEPOINT );
                  delete prp;
                  node = m_pWayPointList->GetFirst();

            }
            else
                  node = node->GetNext();
      }
      return;

}



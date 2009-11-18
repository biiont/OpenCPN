/******************************************************************************
 * $Id: ais.cpp,v 1.34 2009/11/18 01:22:44 bdbcat Exp $
 *
 * Project:  OpenCPN
 * Purpose:  AIS Decoder Object
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
 * $Log: ais.cpp,v $
 * Revision 1.34  2009/11/18 01:22:44  bdbcat
 * Move all buffers to heap
 *
 * Revision 1.33  2009/09/29 18:13:50  bdbcat
 * Add color to managed fonts
 *
 * Revision 1.32  2009/09/18 02:31:04  bdbcat
 * Rebuild AIS Alert dialog
 *
 * Revision 1.31  2009/09/11 19:49:40  bdbcat
 * Improve message handling, format dialogs
 *
 * Revision 1.30  2009/09/04 02:05:35  bdbcat
 * Improve NMEA message handling
 *
 * Revision 1.29  2009/08/29 23:24:06  bdbcat
 * Various, including alert suppression logic
 *
 * Revision 1.28  2009/08/25 21:26:23  bdbcat
 * GMT/DST Fix
 *
 * Revision 1.27  2009/08/22 01:16:22  bdbcat
 * Expand Query
 *
 * Revision 1.26  2009/08/03 03:10:27  bdbcat
 * Cleanup
 *
 * Revision 1.25  2009/07/29 00:48:14  bdbcat
 * Add Debug messages
 *
 * Revision 1.24  2009/07/17 03:56:02  bdbcat
 * Implement Class B Messages.
 *
 * Revision 1.23  2009/07/16 02:47:54  bdbcat
 * Various
 *
 * Revision 1.22  2009/07/11 00:59:59  bdbcat
 * Correct buffer overrun on multi-part messages
 *
 * Revision 1.21  2009/07/10 03:49:42  bdbcat
 * Improve Lost Target logic.
 *
 * Revision 1.20  2009/07/08 03:39:18  bdbcat
 * Improve Alert dialog.
 *
 * Revision 1.19  2009/07/08 01:52:43  bdbcat
 * Convert AISDecoder to wxEvtHandler.
 *
 * Revision 1.18  2009/07/03 13:07:35  bdbcat
 * Typo.
 *
 * Revision 1.17  2009/07/03 02:59:41  bdbcat
 * Improve AIS Dialogs.
 *
 * Revision 1.16  2009/06/17 02:44:38  bdbcat
 * Alarms/Alerts
 *
 * Revision 1.15  2009/06/14 01:50:20  bdbcat
 * AIS Alert Dialog
 *
 * Revision 1.14  2009/06/03 03:12:17  bdbcat
 * Implement AIS/GPS Port sharing
 *
 * Revision 1.13  2009/04/30 00:39:08  bdbcat
 * Correct Logic
 *
 * Revision 1.12  2009/04/07 16:48:50  bdbcat
 * Support AIS Class B
 *
 * Revision 1.11  2009/03/26 22:26:06  bdbcat
 * *** empty log message ***
 *
 * Revision 1.10  2008/04/10 01:02:59  bdbcat
 * Cleanup
 *
 * Revision 1.9  2008/03/30 21:36:27  bdbcat
 * Correct Target Age calculation
 *
 * Revision 1.8  2008/01/12 06:23:11  bdbcat
 * Update for Mac OSX/Unicode
 *
 * Revision 1.7  2008/01/10 03:35:31  bdbcat
 * Update for Mac OSX
 *
 * Revision 1.5  2007/06/15 02:47:41  bdbcat
 * Remove (some) debug printf
 *
 * Revision 1.4  2007/06/10 02:23:14  bdbcat
 * Improve message handling
 *
 * Revision 1.3  2007/05/03 13:23:55  dsr
 * Major refactor for 1.2.0
 *
 * Revision 1.2  2007/02/06 02:04:51  dsr
 * Correct text decode algorithm
 *
 * Revision 1.1  2006/11/01 02:17:37  dsr
 * AIS Support
 *
 */

#include "wx/wx.h"
#include "wx/tokenzr.h"
#include "wx/datetime.h"
#include "wx/sound.h"
#include <wx/wfstream.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "dychart.h"

#include "ais.h"
#include "chart1.h"
#include "nmea.h"           // for DNSTestThread
#include "navutil.h"        // for Select
#include "georef.h"

extern  OCP_AIS_Thread  *pAIS_Thread;
extern  wxString        *pAISDataSource;
extern  int             s_dns_test_flag;
extern  Select          *pSelectAIS;
extern  double          gLat, gLon, gSog, gCog;
extern  bool            g_bGPSAISMux;
extern FontMgr          *pFontMgr;
extern ChartCanvas      *cc1;

//    AIS Global configuration
extern bool             g_bCPAMax;
extern double           g_CPAMax_NM;
extern bool             g_bCPAWarn;
extern double           g_CPAWarn_NM;
extern bool             g_bTCPA_Max;
extern double           g_TCPA_Max;
extern bool             g_bMarkLost;
extern double           g_MarkLost_Mins;
extern bool             g_bRemoveLost;
extern double           g_RemoveLost_Mins;
extern bool             g_bShowCOG;
extern double           g_ShowCOG_Mins;
extern bool             g_bShowTracks;
extern double           g_ShowTracks_Mins;
extern bool             g_bShowMoored;
extern double           g_ShowMoored_Kts;

extern bool             g_bGPSAISMux;
extern ColorScheme      global_color_scheme;

extern bool             g_bAIS_CPA_Alert;
extern bool             g_bAIS_CPA_Alert_Audio;
extern bool             g_bAIS_CPA_Alert_Suppress_Moored;

extern AISTargetAlertDialog    *g_pais_alert_dialog_active;
extern int              g_ais_alert_dialog_x, g_ais_alert_dialog_y;
extern int              g_ais_alert_dialog_sx, g_ais_alert_dialog_sy;
extern wxString         g_sAIS_Alert_Sound_File;

extern int              g_nNMEADebug;
extern int              g_total_NMEAerror_messages;

//    A static structure storing generic position data
//    Used to communicate from NMEA threads to main application thread
//static      GenericPosDat     AISPositionMuxData;

//-------------------------------------------------------------------------------------------------------------
//    OCP_AIS_Thread Static data store
//-------------------------------------------------------------------------------------------------------------

//extern char                         rx_share_buffer[];
//extern unsigned int                 rx_share_buffer_length;
//extern ENUM_BUFFER_STATE            rx_share_buffer_state;



CPL_CVSID("$Id: ais.cpp,v 1.34 2009/11/18 01:22:44 bdbcat Exp $");

// the first string in this list produces a 6 digit MMSI... BUGBUG

char test_str[24][79] = {
"!AIVDM,1,1,,A,100Rsb@P1PJREovFCnS7n?vt08Ag,0*13**",
"!AIVDM,1,1,,A,15Mnh`PP0jIcl78Csm`hCgvB2D00,0*29**",
"!AIVDM,2,1,1,A,55Mnh`P00001L@7S3GP=Dl8E8h4pB2222222220P0`A6357d07PT851F,0*75**",
"!AIVDM,2,2,1,A,0Dp0jE6H8888880,2*40**",
"!AIVDM,1,1,,A,15Mnh`PP0rIce3VCr8nPWgvt24B0,0*10**",
"!AIVDM,1,1,,A,15Mnh`PP0pIcfJ<Cs0lPFwwR24Bt,0*6B**",
"!AIVDM,1,1,,A,15Mnh`PP0pIcfKBCs23P6gvB2D00,0*7E**",
"!AIVDM,1,1,,A,15Mnh`PP0nIcfL8Cs3DPLwvt28AV,0*5C**",
"!AIVDM,1,1,,A,15Mnh`PP0mIcfSlCs7w@KOwT2L00,0*4E**",
"!AIVDM,1,1,,A,15Mnh`PP0lIcjQRCsOK1LwvB2D00,0*6A**",
"!AIVDM,1,1,,A,15Mnh`PP0mIcj`NCsPNADOvt2@AQ,0*3E**",
"!AIVDM,1,1,,A,15Mnh`PP0mIcjfhCsQLAHOwT24H0,0*2C**",
"!AIVDM,1,1,,A,15Mnh`PP0mIcjlNCsRCiBwvB2@4s,0*4A**",
"!AIVDM,1,1,,A,15Mnh`PP0nIck5@CsUBi2gvB2<00,0*42**",
"!AIVDM,1,1,,A,15Mnh`PP0nIck:PCsVO0uOvt28AQ,0*47**",
"!AIVDM,1,1,,A,15Mnh`PP0oIck?:CsWQi0?wR2L00,0*19**",
"!AIVDM,1,1,,A,15Mnh`PP0oIckRVCscni3gvB24H@,0*29**",
"!AIVDM,1,1,,A,15Mnh`PP0pIck`VCse0i8wvt20Rg,0*48**",
"!AIVDM,1,1,,A,15Mnh`PP0nIckelCsf0hrgwR2D00,0*57**",
"!AIVDM,1,1,,A,15Mnh`PP0mIckiHCsfn@kOvB20S4,0*39**",
"!AIVDM,1,1,,A,15Mnh`PP0nIckmtCsh<0h?vr2HA6,0*05**",
"!AIVDM,1,1,,A,15Mnh`PP0nIckqdCsiC@iOwR2@LP,0*34**",
"!AIVDM,1,1,,A,15Mnh`PP0nIckttCsjEhdgvB2H4m,0*75**",
"!AIVDM,1,1,,A,15Mnh`PP0mIcl0jCskPhiwvr2D00,0*47**",


};


char ais_status[][40] = {

      "Underway",
      "At Anchor",
      "Not Under Command",
      "Restricted Manoeuvrability",
      "Constrained by draught",
      "Moored",
      "Aground",
      "Engaged in Fishing",
      "Under way sailing"
};

char ais_type[][80] = {
"Vessel Fishing",             //30        0
"Vessel Towing",              //31        1
"Vessel Towing, Long",        //32        2
"Vessel Dredging",            //33        3
"Vessel Diving",              //34        4
"Military Vessel",            //35        5
"Vessel Sailing",             //36        6
"High Speed Craft",           //4x        7
"Pilot Vessel",               //50        8
"Search and Rescue Vessel",   //51        9
"Tug",                        //52        10
"Port Tender",                //53        11
"Pollution Control Vessel",   //54        12
"Law Enforcement Vessel",     //55        13
"Medical Transport",          //58        14
"Passenger Ship",             //6x        15
"Cargo Ship",                 //7x        16
"Tanker",                     //8x        17
"Unknown"                     //          18
};

char short_ais_type[][80] = {
      "F/V",                  //30        0
      "Tow",                  //31        1
      "Long Tow",             //32        2
      "Dredge",               //33        3
      "D/V",                  //34        4
      "Mil/V",                //35        5
      "S/V",                  //36        6
      "HSC",                  //4x        7
      "P/V",                  //50        8
      "SAR/V",                //51        9
      "Tug",                  //52        10
      "Tender",               //53        11
      "PC/V",                 //54        12
      "LE/V",                 //55        13
      "Med/V",                //58        14
      "Pass/V",               //6x        15
      "M/V",                  //7x        16
      "M/T",                  //8x        17
      ""                      //          18
};



//#define AIS_DEBUG  1

//---------------------------------------------------------------------------------
//
//  AIS_Target_Data Implementation
//
//---------------------------------------------------------------------------------
AIS_Target_Data::AIS_Target_Data()
{
    strncpy(ShipName, "Unknown             ", 21);
    strncpy(CallSign, "       ", 8);
    strncpy(Destination, "Unknown             ", 21);

    SOG = 555.;
    COG = 666.;
    HDG = 511.;

    wxDateTime now = wxDateTime::Now();
    now.MakeGMT();
    ReportTicks = now.GetTicks();       // Default is my idea of NOW

    MID = 555;
    MMSI = 666;
    NavStatus = 777;
    SyncState = 888;
    SlotTO = 999;

    CPA = 100;                // Large values avoid false alarms
    TCPA = 100;

    Range_NM = 1.;
    Brg = 0;

    DimA = DimB = DimC = DimD = 0;;

    ETA_Mo = 0;
    ETA_Day = 0;
    ETA_Hr = 24;
    ETA_Min = 60;

    Draft = 0.;

    RecentPeriod = 0;

    m_utc_hour = 0;
    m_utc_min = 0;
    m_utc_sec = 0;


    Class = AIS_CLASS_A;      // default
    n_alarm_state = AIS_NO_ALARM;
    b_suppress_audio = false;
    b_positionValid = false;
    b_nameValid = false;


}

wxString AIS_Target_Data::BuildQueryResult( void )
{

      wxString line;
      wxString result;

    //  Clip any unused characters (@) from the name
      wxString ts;
      char *tp = ShipName;
      while((*tp) && (*tp != '@'))
            ts.Append(*tp++);

      line.Printf(_T("ShipName:  "));
      line.Append( ts );
      line.Append(_T("\n\n"));
      result.Append(line);

      line.Printf(_T("MMSI:      %d\n"), MMSI);
      result.Append(line);

       //  Clip any unused characters (@) from the callsign
      ts.Clear();
      tp = CallSign;
      while((*tp) && (*tp != '@'))
            ts.Append(*tp++);

      line.Printf(_T("CallSign:  "));
      line.Append( ts );
      line.Append(_T("\n"));
      result.Append(line);

      if(IMO > 0)
            line.Printf(_T("IMO:        %8d\n"), IMO);
      else
            line.Printf(_T("IMO:\n"));
      result.Append(line);

      if(AIS_CLASS_A == Class)
            line.Printf(_T("Class:     A\n\n"));
      else
            line.Printf(_T("Class:     B\n\n"));
      result.Append(line);

    //      Nav Status
      ts.Clear();
      if((NavStatus <= 8) && (NavStatus >= 0))
      {
            tp = &ais_status[NavStatus][0];
            while(*tp)
                  ts.Append(*tp++);
      }

      line.Printf(_T("Navigational Status:  "));
      line.Append( ts );
      line.Append(_T("\n"));
      result.Append(line);


    //      Ship type
      tp = Get_vessel_type_string();
      ts.Clear();
      while(*tp)
            ts.Append(*tp++);

      line.Printf(_T("Ship Type:            "));
      line.Append( ts );
      line.Append(_T("\n"));
      result.Append(line);

    //  Destination
      ts.Clear();
      tp = Destination;
      while((*tp) && (*tp != '@'))
            ts.Append(*tp++);

      line.Printf(_T("Destination:          "));
      line.Append( ts );
      line.Append(_T("\n"));
      result.Append(line);

      wxDateTime now = wxDateTime::Now();

    //  ETA
      if((ETA_Mo) && (ETA_Hr < 24))
      {
            wxDateTime eta(ETA_Day, wxDateTime::Month(ETA_Mo), now.GetYear(), ETA_Hr, ETA_Min);
            line.Printf(_T("ETA:                  "));
            line.Append( eta.FormatISODate());
            line.Append(_T("  "));
            line.Append( eta.FormatISOTime());
            line.Append(_T("\n"));
      }
      else
      {
            line.Printf(_T("ETA:                  Unknown"));
            line.Append(_T("\n"));
      }

      result.Append(line);

    //  Dimensions
      if((DimA + DimB + DimC + DimD) == 0)
      {
            if(Draft > 0.01)
                  line.Printf(_T("Size:                ---m x ---m x %4.1fm\n\n"),  Draft);
            else
                  line.Printf(_T("Size:                ---m x ---m x ---m\n\n"));
      }
      else if(Draft < 0.01)
            line.Printf(_T("Size:                %5dm x %dm x ---m\n\n"), (DimA + DimB), (DimC + DimD));
      else
            line.Printf(_T("Size:                %5dm x %dm x %4.1fm\n\n"), (DimA + DimB), (DimC + DimD), Draft);


      result.Append(line);

      line.Printf(_T("Course:               %5.0f Deg.\n"), COG);
      result.Append(line);

      line.Printf(_T("Speed:                %5.2f Kts.\n"), SOG);
      result.Append(line);

      line.Printf(_T("Range:                %5.1f NM\n"), Range_NM);
      result.Append(line);

      line.Printf(_T("Bearing:              %5.0f Deg.\n"), Brg);
      result.Append(line);

      line.Printf(_T("Position:              "));

      wxString pos_st;
      pos_st += toSDMM(1, Lat);
      pos_st <<_T("\n                       ");
      pos_st += toSDMM(2, Lon);
      pos_st << _T("\n\n");
      line << pos_st;
      result.Append(line);


      wxDateTime rt(ReportTicks);
      line.Printf(_T("Latest Report Time:       "));
      line << rt.FormatISOTime();
      line << _(" UTC\n");
      result.Append(line);


      now.MakeGMT();
      int target_age = now.GetTicks() - ReportTicks;


      line.Printf(_T("Report Age:               %3d Sec.\n"), target_age);
      result.Append(line);

      line.Printf(_T("Recent Report Period:     %3d Sec.\n\n"), RecentPeriod);
      result.Append(line);

      double hours = floor(TCPA / 60.);
      double mins = TCPA - (hours * 60);

      if(bCPA_Valid)
            line.Printf(_T("TCPA:                 %02d:%02d Hr:Min\n"), (int)hours, (int)mins);
      else
            line.Printf(_T("TCPA:  \n"));
      result.Append(line);

      if(bCPA_Valid)
            line.Printf(_T("CPA:                 %6.1f NM\n"), CPA);
      else
            line.Printf(_T("CPA:       \n"));
      result.Append(line);
/*
      //    Count lines and characters
      wxString max_line;
      int nl = 0;
      unsigned int max_len = 0;
      int max_pix = 0;

      if(pn_nl)
      {
            unsigned int i = 0;
            wxString rline;

            while(i < result.Len())
            {
                  while(result.GetChar(i) != '\n')
                        rline.Append(result.GetChar(i++));

                  if(rline.Len() > max_len)
                  {
                        max_line = rline;
                        max_len = rline.Len();
                  }

                  if(pFont && ppix_size)              // measure this line exactly
                  {
                        int w, h;
                        pdc->GetTextExtent(rline, &w, &h, NULL, NULL, pFont);
                        if(w > max_pix)
                              max_pix = w;
                  }


                  i++;              // skip nl
                  nl++;
                  rline.Clear();
            }

            *pn_nl = nl;
            if(pn_cmax)
                  *pn_cmax = max_len;
      }


      //    Measurement requested?
      if(pFont && ppix_size)
      {
            int w, h;
            pdc->GetTextExtent(max_line, &w, &h, NULL, NULL, pFont);

            ppix_size->x = max_pix;       // x comes from above
            ppix_size->y = h * nl;        // y is the same for all
      }

*/
      return result;
}


char *AIS_Target_Data::Get_vessel_type_string(bool b_short)
{
      int i=18;
      switch(ShipType)
      {
            case 30:
                  i=0; break;
            case 31:
                  i=1; break;
            case 32:
                  i=2; break;
            case 33:
                  i=3; break;
            case 34:
                  i=4; break;
            case 35:
                  i=5; break;
            case 36:
                  i=6; break;
            case 50:
                  i=8; break;
            case 51:
                  i=9; break;
            case 52:
                  i=10; break;
            case 53:
                  i=11; break;
            case 54:
                  i=12; break;
            case 55:
                  i=13; break;
            case 58:
                  i=14; break;
            default:
                  i=18; break;
      }

      if((ShipType >= 40) && (ShipType < 50))
            i=7;

      if((ShipType >= 60) && (ShipType < 70))
            i=15;

      if((ShipType >= 70) && (ShipType < 80))
            i=16;

      if((ShipType >= 80) && (ShipType < 90))
            i=17;

      if(!b_short)
            return &ais_type[i][0];
      else
            return &short_ais_type[i][0];
}





//---------------------------------------------------------------------------------
//
//  AIS_Decoder Helpers
//
//---------------------------------------------------------------------------------
AIS_Bitstring::AIS_Bitstring(const char *str)
{
    byte_length = strlen(str);

    for(int i=0 ; i<byte_length ; i++)
    {
        bitbytes[i] = to_6bit(str[i]);
    }
}


//  Convert printable characters to IEC 6 bit representation
//  according to rules in IEC AIS Specification
unsigned char AIS_Bitstring::to_6bit(const char c)
{
    if(c < 0x30)
        return (unsigned char)-1;
    if(c > 0x77)
        return (unsigned char)-1;
    if((0x57 < c) && (c < 0x60))
        return (unsigned char)-1;

    unsigned char cp = c;
    cp += 0x28;

    if(cp > 0x80)
        cp += 0x20;
    else
        cp += 0x28;

    return (unsigned char)(cp & 0x3f);
}


int AIS_Bitstring::GetInt(int sp, int len)
{
    int acc = 0;
    int s0p = sp-1;                          // to zero base

    int cp, cx, c0, cs;


    for(int i=0 ; i<len ; i++)
    {
        acc  = acc << 1;
        cp = (s0p + i) / 6;
        cx = bitbytes[cp];
        cs = 5 - ((s0p + i) % 6);
        c0 = (cx >> (5 - ((s0p + i) % 6))) & 1;
        acc |= c0;
    }

    return acc;

}

bool AIS_Bitstring::GetStr(int sp, int len, char *dest, int max_len)
{
    char temp_str[85];

    char acc = 0;
    int s0p = sp-1;                          // to zero base

    int k=0;
    int cp, cx, c0, cs;

    int i = 0;
    while(i < len)
    {
         acc=0;
         for(int j=0 ; j<6 ; j++)
         {
            acc  = acc << 1;
            cp = (s0p + i) / 6;
            cx = bitbytes[cp];
            cs = 5 - ((s0p + i) % 6);
            c0 = (cx >> (5 - ((s0p + i) % 6))) & 1;
            acc |= c0;

            i++;
         }
         temp_str[k] = (char)(acc & 0x3f);

         if(acc < 32)
             temp_str[k] += 0x40;
         k++;

    }

    temp_str[k] = 0;

    int copy_len = wxMin((int)strlen(temp_str), max_len);
    strncpy(dest, temp_str, copy_len);

    return true;
}




//---------------------------------------------------------------------------------
//
//  AIS_Decoder Implementation
//
//---------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AIS_Decoder, wxEvtHandler)

  EVT_SOCKET(AIS_SOCKET_ID, AIS_Decoder::OnSocketEvent)
  EVT_TIMER(TIMER_AIS1, AIS_Decoder::OnTimerAIS)
  EVT_TIMER(TIMER_AISAUDIO, AIS_Decoder::OnTimerAISAudio)
  EVT_COMMAND(ID_AIS_WINDOW, EVT_AIS, AIS_Decoder::OnEvtAIS)

  END_EVENT_TABLE()

static int n_msgs;
static int n_msg1;
static int n_msg5;
static int n_msg24;
static int n_newname;
static bool b_firstrx;
static int first_rx_ticks;
static int rx_ticks;



AIS_Decoder::AIS_Decoder(int window_id, wxFrame *pParent, const wxString& AISDataSource, wxMutex *pGPSMutex)

{
      AISTargetList = new AIS_Target_Hash;

      m_pShareGPSMutex = pGPSMutex;

      m_parent_frame = pParent;

      m_pMainEventHandler = pParent->GetEventHandler();

      g_pais_alert_dialog_active = NULL;
      m_bAIS_Audio_Alert_On = false;

      m_n_targets = 0;

      OpenDataSource(pParent, AISDataSource);
}

AIS_Decoder::~AIS_Decoder(void)
{

 //    Kill off the  RX Thread if alive
//      Todo Need to fix this by re-building the comm thread to not use blocking event structure
//      Also affects the NMEA thread in nmea.cpp

      if(pAIS_Thread)
      {
            pAIS_Thread->Delete();         //was kill();
#ifdef __WXMSW__
//          wxSleep(1);
#else
            sleep(2);
#endif
      }


    AIS_Target_Hash::iterator it;
    AIS_Target_Hash *current_targets = GetTargetList();

    for( it = (*current_targets).begin(); it != (*current_targets).end(); ++it )
    {
          AIS_Target_Data *td = it->second;

          delete td;
    }

    delete current_targets;

    //    Kill off the TCP/IP Socket if alive
    if(m_sock)
    {
          m_sock->Notify(FALSE);
          m_sock->Destroy();
          TimerAIS.Stop();
    }

    wxDateTime now = wxDateTime::Now();
    now.MakeGMT();

#ifdef AIS_DEBUG
    printf("First message[1, 2] ticks: %d  Last Message [1,2]ticks %d  Difference:  %d\n", first_rx_ticks, rx_ticks, rx_ticks - first_rx_ticks);
#endif


}

//----------------------------------------------------------------------------------
//     Handle events from AIS Serial Port RX thread
//----------------------------------------------------------------------------------
void AIS_Decoder::OnEvtAIS(wxCommandEvent& event)
{
    switch(event.GetExtraLong())
    {
        case EVT_AIS_PARSE_RX:
        {
//              wxDateTime now = wxDateTime::Now();
//              printf("AIS Event at %ld\n", now.GetTicks());

            wxString message = event.GetString();

            int nr = 0;
            if(!message.IsEmpty())
                nr = Decode(message);


            if(g_bGPSAISMux)
                  Parse_And_Send_Posn(message);

            break;
        }       //case
    }           // switch
}

void AIS_Decoder::ThreadMessage(const wxString &msg)
{

    //    Signal the main program thread
      wxCommandEvent event( EVT_THREADMSG);
      event.SetEventObject( (wxObject *)this );
      event.SetString(msg);
      m_pMainEventHandler->AddPendingEvent(event);

}


void AIS_Decoder::Parse_And_Send_Posn(wxString &message)
{
      if( g_nNMEADebug && (g_total_NMEAerror_messages < g_nNMEADebug) )
      {
            g_total_NMEAerror_messages++;
            wxString msg(_T("AIS.NMEA Sentence received..."));
            msg.Append(message);
            ThreadMessage(msg);
      }

      OCPN_NMEAEvent Nevent(wxEVT_OCPN_NMEA, 0);
      Nevent.SetNMEAString(message);
      m_pMainEventHandler->AddPendingEvent(Nevent);

      return;

#if 0
   // Send the NMEA string to the decoder
      m_NMEA0183 << str_temp_buf;

      if (true == m_NMEA0183.PreParse())
      {
            {
//    Signal the main program thread with raw sentence
                  wxCommandEvent event( EVT_NMEA);
                  event.SetEventObject( (wxObject *)this );
                  event.SetExtraLong(EVT_NMEA_PARSE_RX);

                  wxMutexLocker stateLocker(*m_pShareGPSMutex);          // scope is right here
                  if(stateLocker.IsOk() )
                  {
                        if(RX_BUFFER_EMPTY == rx_share_buffer_state)
                        {
                              strcpy(rx_share_buffer, str_temp_buf.mb_str());
                              rx_share_buffer_state = RX_BUFFER_FULL;
                              rx_share_buffer_length = str_temp_buf.Len();

                              m_pMainEventHandler->AddPendingEvent(event);
                        }
                  }
                  else
                  {
                        // Cant get mutex, so punt
                  }
            }
      }
      else
      {
            if( g_nNMEADebug && (g_total_NMEAerror_messages < g_nNMEADebug) )
            {
                  g_total_NMEAerror_messages++;
                  wxString msg(_T("   AIS.Unrecognized NMEA Sentence..."));
                  msg.Append(str_temp_buf);
                  ThreadMessage(msg);
            }
      }

#endif
}






#if 0
void AIS_Decoder::Parse_And_Send_Posn(wxString &str_temp_buf)
{

   // Send the NMEA string to the decoder
      m_NMEA0183 << str_temp_buf;

   // we must check the return from parse, as some usb to serial adaptors on the MAC spew
   // junk if there is not a serial data cable connected.
      if (true == m_NMEA0183.Parse())
      {
            if(m_NMEA0183.LastSentenceIDReceived == wxString(_T("RMC")))
            {

                  if(m_NMEA0183.Rmc.IsDataValid == NTrue)
                  {

                        if(m_pShareGPSMutex)
                              m_pShareGPSMutex->Lock();

                        float llt = m_NMEA0183.Rmc.Position.Latitude.Latitude;
                        int lat_deg_int = (int)(llt / 100);
                        float lat_deg = lat_deg_int;
                        float lat_min = llt - (lat_deg * 100);
                        AISPositionMuxData.kLat = lat_deg + (lat_min/60.);
                        if(m_NMEA0183.Rmc.Position.Latitude.Northing == South)
                              AISPositionMuxData.kLat = -AISPositionMuxData.kLat;

                        float lln = m_NMEA0183.Rmc.Position.Longitude.Longitude;
                        int lon_deg_int = (int)(lln / 100);
                        float lon_deg = lon_deg_int;
                        float lon_min = lln - (lon_deg * 100);
                        AISPositionMuxData.kLon = lon_deg + (lon_min/60.);
                        if(m_NMEA0183.Rmc.Position.Longitude.Easting == West)
                              AISPositionMuxData.kLon = -AISPositionMuxData.kLon;

                        AISPositionMuxData.kSog = m_NMEA0183.Rmc.SpeedOverGroundKnots;
                        AISPositionMuxData.kCog = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;
                        AISPositionMuxData.FixTime = 0;

                        if(m_pShareGPSMutex)
                              m_pShareGPSMutex->Unlock();


 //    Signal the main program thread
                        wxCommandEvent event( EVT_NMEA );
                        event.SetEventObject( (wxObject *)this );
                        event.SetExtraLong(EVT_NMEA_DIRECT);
                        event.SetClientData(&AISPositionMuxData);
                        m_pMainEventHandler->AddPendingEvent(event);
                  }
            }
      }
}

#endif


//----------------------------------------------------------------------------------
//      Decode NMEA VDM sentence to AIS Target(s)
//----------------------------------------------------------------------------------
AIS_Error AIS_Decoder::Decode(const wxString& str)
{
    AIS_Error ret;
    wxString string_to_parse;

    //  Make some simple tests for validity

//    int nlen = str.Len();

    if(str.Len() > 100)
        return AIS_NMEAVDM_TOO_LONG;

    if(!NMEACheckSumOK(str))
    {
//          printf("Checksum error at n_msgs:%d\n", n_msgs);
          return AIS_NMEAVDM_CHECKSUM_BAD;
    }

    if(!str.Mid(3,3).IsSameAs(_T("VDM")))
    {
//          printf("Not VDM at n_msgs:%d\n", n_msgs);
          return AIS_NMEAVDM_BAD;
    }

    //  OK, looks like the sentence is OK

    //  Use a tokenizer to pull out the first 4 fields
    wxString string(str);
    wxStringTokenizer tkz(string, _T(","));

    wxString token;
    token = tkz.GetNextToken();         // !xxVDM

    token = tkz.GetNextToken();
    nsentences = atoi(token.mb_str());

    token = tkz.GetNextToken();
    isentence = atoi(token.mb_str());

    token = tkz.GetNextToken();
    long lsequence_id = 0;
    token.ToLong(&lsequence_id);

    token = tkz.GetNextToken();
    long lchannel;
    token.ToLong(&lchannel);

    //  Now, some decisions

    string_to_parse.Clear();

    //  Simple case first
    //  First and only part of a one-part sentence
    if((1 == nsentences) && (1 == isentence))
    {
        string_to_parse = tkz.GetNextToken();         // the encapsulated data
    }

    else if(nsentences > 1)
    {
        if(1 == isentence)
        {
            sentence_accumulator = tkz.GetNextToken();         // the encapsulated data
        }

        else
        {
            sentence_accumulator += tkz.GetNextToken();
        }

        if(isentence == nsentences)
        {
            string_to_parse = sentence_accumulator;
        }
     }


     if(!string_to_parse.IsEmpty() && (string_to_parse.Len() < AIS_MAX_MESSAGE_LEN))
     {

        //  Create the bit accessible string
        AIS_Bitstring strbit(string_to_parse.mb_str());

        //  Extract the MMSI
        int mmsi = strbit.GetInt(9, 30);

//        int message_ID = strbit.GetInt(1, 6);

        AIS_Target_Data *pTargetData;
        AIS_Target_Data *pStaleTarget = NULL;
        bool bnewtarget = false;

        //  Search the current AISTargetList for an MMSI match
        AIS_Target_Hash::iterator it = AISTargetList->find( mmsi );
        if(it == AISTargetList->end())                  // not found
        {
              pTargetData = new AIS_Target_Data;
              bnewtarget = true;
              m_n_targets++;
        }
        else
        {
              pTargetData = (*AISTargetList)[mmsi];          // find current entry
              pStaleTarget = pTargetData;                   // save a pointer to stale data
        }

        //  Grab the stale targets's last report time
        int last_report_ticks;
        if(pStaleTarget)
              last_report_ticks = pStaleTarget->ReportTicks;
        else
        {
              wxDateTime now = wxDateTime::Now();
              now.MakeGMT();
              last_report_ticks = now.GetTicks();
        }



        // Delete the stale AIS Target selectable point
        if(pStaleTarget)
            pSelectAIS->DeleteSelectablePoint((void *)mmsi, SELTYPE_AISTARGET);

      bool bhad_name = false;
      if(pStaleTarget)
            bhad_name =  pStaleTarget->b_nameValid;


        bool bdecode_result = Parse_VDMBitstring(&strbit, pTargetData);            // Parse the new data

        if((bdecode_result) && (pTargetData->b_nameValid) && (pStaleTarget))
           if(!bhad_name)
                    n_newname++;

        //  If the message was decoded correctly, Update the AIS Target in the Selectable list, and update the TargetData
        if(bdecode_result)
        {
              (*AISTargetList)[mmsi] = pTargetData;            // update the entry

              if(pTargetData->b_positionValid)
              {
                    SelectItem *pSel = pSelectAIS->AddSelectablePoint(pTargetData->Lat, pTargetData->Lon, (void *)mmsi, SELTYPE_AISTARGET);
                    pSel->SetUserData(mmsi);
              }

        //     Update the most recent report period
              pTargetData->RecentPeriod = pTargetData->ReportTicks - last_report_ticks;

            //    Calculate CPA info for this target immediately
              UpdateOneCPA(pTargetData);
        }
        else
        {
//             printf("Unrecognised AIS message ID: %d\n", pTargetData->MID);
             if(bnewtarget)
             {
                    delete pTargetData;                                       // this target is not going to be used
                    m_n_targets--;
             }
        }

        ret = AIS_NoError;

    }
    else
        ret = AIS_Partial;                // accumulating parts of a multi-sentence message

    n_msgs++;

#ifdef AIS_DEBUG
    if((n_msgs % 10000) == 0)
          printf("n_msgs %10d m_n_targets: %6d  n_msg1: %10d  n_msg5+24: %10d  n_new5: %10d \n", n_msgs, n_targets, n_msg1, n_msg5 + n_msg24, n_newname);
#endif

    return ret;
}



//----------------------------------------------------------------------------
//      Parse a NMEA VDM Bitstring
//----------------------------------------------------------------------------
bool AIS_Decoder::Parse_VDMBitstring(AIS_Bitstring *bstr, AIS_Target_Data *ptd)
{
    bool parse_result = false;

    wxDateTime now = wxDateTime::Now();
    now.MakeGMT(true);                    // no DST
    if(now.IsDST())
          now.Subtract(wxTimeSpan(1,0,0,0));



//    int utc_hour = now.GetHour();
//    int utc_min = now.GetMinute();
//    int utc_sec = now.GetSecond();
 //   int utc_day = now.GetDay();
 //   wxDateTime::Month utc_month = now.GetMonth();
 //   int utc_year = now.GetYear();
    ptd->ReportTicks = now.GetTicks();       // Default is my idea of NOW
                                            // which may disagee with target...

    int message_ID = bstr->GetInt(1, 6);        // Parse on message ID

    ptd->MID = message_ID;

    ptd->MMSI = bstr->GetInt(9, 30);            // MMSI is always in the same spot in the bitstream


    switch (message_ID)
    {
    case 1:                                 // Position Report
    case 2:
    case 3:
        {
            n_msg1++;

            ptd->NavStatus = bstr->GetInt(39, 4);
            ptd->SOG = 0.1 * (bstr->GetInt(51, 10));

            int lon = bstr->GetInt(62, 28);
            if(lon & 0x08000000)                    // negative?
                lon |= 0xf0000000;
            ptd->Lon = lon / 600000.;

            int lat = bstr->GetInt(90, 27);
            if(lat & 0x04000000)                    // negative?
                lat |= 0xf8000000;
            ptd->Lat = lat / 600000.;

            ptd->COG = 0.1 * (bstr->GetInt(117, 12));
            ptd->HDG = 1.0 * (bstr->GetInt(129, 9));

            ptd->b_positionValid = true;

            ptd->ROTAIS = bstr->GetInt(43, 8);
            if(ptd->ROTAIS == 128)
                  ptd->ROTAIS = 0;                      // not available codes as zero
            if((ptd->ROTAIS & 0x80) == 0x80)
                  ptd->ROTAIS = ptd->ROTAIS - 256;       // convert to twos complement

            ptd->m_utc_sec = bstr->GetInt(138, 6);

            if((1 == message_ID) || (2 == message_ID))      // decode SOTDMA per 7.6.7.2.2
            {
                  ptd->SyncState = bstr->GetInt(151,2);
                  ptd->SlotTO = bstr->GetInt(153,2);
                  if((ptd->SlotTO == 1) && (ptd->SyncState == 0)) // UTCDirect follows
                  {
                        ptd->m_utc_hour = bstr->GetInt(155, 5);

                        ptd->m_utc_min = bstr->GetInt(160,7);

                        if((ptd->m_utc_hour < 24) && (ptd->m_utc_min <60) && (ptd->m_utc_sec < 60))
                        {
                              wxDateTime rx_time(ptd->m_utc_hour, ptd->m_utc_min, ptd->m_utc_sec);
                              rx_ticks = rx_time.GetTicks();
                              if(!b_firstrx)
                              {
                                    first_rx_ticks = rx_ticks;
                                    b_firstrx = true;
                                    }
                        }
//                    else
//                          printf("hr:min:sec:   %d:%d:%d\n", ptd->m_utc_hour, ptd->m_utc_min, ptd->m_utc_sec);

                  }
            }
            parse_result = true;                // so far so good


            ptd->Class = AIS_CLASS_A;

            break;
        }

          case 18:
          {
                ptd->SOG = 0.1 * (bstr->GetInt(47, 10));

                int lon = bstr->GetInt(58, 28);
                if(lon & 0x08000000)                    // negative?
                      lon |= 0xf0000000;
                ptd->Lon = lon / 600000.;

                int lat = bstr->GetInt(86, 27);
                if(lat & 0x04000000)                    // negative?
                      lat |= 0xf8000000;
                ptd->Lat = lat / 600000.;

                ptd->COG = 0.1 * (bstr->GetInt(113, 12));
                ptd->HDG = 1.0 * (bstr->GetInt(125, 9));

                ptd->b_positionValid = true;

                ptd->m_utc_sec = bstr->GetInt(134, 6);

                parse_result = true;                // so far so good

                ptd->Class = AIS_CLASS_B;

                break;
          }

    case 5:
        {
              n_msg5++;
              ptd->Class = AIS_CLASS_A;


            int DSI = bstr->GetInt(39, 2);
            if(0 == DSI)
            {
                  ptd->IMO = bstr->GetInt(41, 30);

                  bstr->GetStr(71,42, &ptd->CallSign[0], 7);
                  bstr->GetStr(113,120, &ptd->ShipName[0], 20);
                  ptd->b_nameValid = true;
//                  printf("%d %d %s\n", n_msgs, ptd->MMSI, ptd->ShipName);

                  ptd->ShipType = (unsigned char)bstr->GetInt(233,8);

                  ptd->DimA = bstr->GetInt(241, 9);
                  ptd->DimB = bstr->GetInt(250, 9);
                  ptd->DimC = bstr->GetInt(259, 6);
                  ptd->DimD = bstr->GetInt(265, 6);

//                int epfd = bstr->GetInt(271, 4);

                  ptd->ETA_Mo =  bstr->GetInt(275, 4);
                  ptd->ETA_Day = bstr->GetInt(279, 5);
                  ptd->ETA_Hr =  bstr->GetInt(284, 5);
                  ptd->ETA_Min = bstr->GetInt(289, 6);

                  ptd->Draft = (double)(bstr->GetInt(295, 8)) / 10.0;

                  bstr->GetStr(303,120, &ptd->Destination[0], 20);

                  parse_result = true;
            }

            break;
        }

     case 24:
         {
               ptd->Class = AIS_CLASS_B;

               int part_number = bstr->GetInt(39, 2);
               if(0 == part_number)
               {
                     bstr->GetStr(41,120, &ptd->ShipName[0], 20);
//                     printf("%d %d %s\n", n_msgs, ptd->MMSI, ptd->ShipName);
                     ptd->b_nameValid = true;
                     parse_result = true;
                     n_msg24++;
               }
               else if(1 == part_number)
               {
                     ptd->ShipType = (unsigned char)bstr->GetInt(41,8);
                     bstr->GetStr(91,42, &ptd->CallSign[0], 7);

                     ptd->DimA = bstr->GetInt(133, 9);
                     ptd->DimB = bstr->GetInt(142, 9);
                     ptd->DimC = bstr->GetInt(151, 6);
                     ptd->DimD = bstr->GetInt(157, 6);
                     parse_result = true;
               }


               break;
         }
     case 4:                                    // base station
          {
                break;
          }
     case 9:                                    // Special Position Report
          {
                break;
          }
     case 21:                                    // Test Message
          {
                break;
          }
     case 8:                                    // Binary Broadcast
          {
                break;
          }
     case 6:                                    // Addressed Binary Message
          {
                break;
          }
     case 7:                                    // Binary Ack
          {
                break;
          }
     default:
          {
                break;
          }

    }

    if(true == parse_result)
        ptd->b_active = true;

    return parse_result;
}



bool AIS_Decoder::NMEACheckSumOK(const wxString& str)
{

   unsigned char checksum_value = 0;
   int sentence_hex_sum;

   int string_length = str.Len();
   int index = 1; // Skip over the $ at the begining of the sentence

   while( index < string_length    &&
          str[ index ] != '*' &&
          str[ index ] != 0x0d &&
          str[ index ] != 0x0a )
   {
      checksum_value ^= str[ index ];
      index++;
   }

   if(string_length > 4)
   {
        char scanstr[3];
        scanstr[0] = str[string_length-4];
        scanstr[1] = str[string_length - 3];
        scanstr[2] = 0;
        sscanf(scanstr, "%2x", &sentence_hex_sum);
        if(sentence_hex_sum == checksum_value)
            return true;
   }

   return false;
}

void AIS_Decoder::UpdateAllCPA(void)
{
      //    Iterate thru all the targets
      AIS_Target_Hash::iterator it;
      AIS_Target_Hash *current_targets = GetTargetList();

      for( it = (*current_targets).begin(); it != (*current_targets).end(); ++it )
      {
            AIS_Target_Data *td = it->second;

            if(NULL != td)
                  UpdateOneCPA(td);
      }
}
void AIS_Decoder::UpdateAllAlarms(void)
{
           //    Iterate thru all the targets
      AIS_Target_Hash::iterator it;
      AIS_Target_Hash *current_targets = GetTargetList();

      for( it = (*current_targets).begin(); it != (*current_targets).end(); ++it )
      {
            AIS_Target_Data *td = it->second;

            if(NULL != td)
            {
                  ais_alarm_type this_alarm = AIS_NO_ALARM;
                  if(g_bCPAWarn && td->b_active && td->b_positionValid)
                  {
                        //      Skip anchored/moored targets if requested
                        if((!g_bShowMoored) && (td->SOG <= g_ShowMoored_Kts))
                        {
                              td->n_alarm_state = AIS_NO_ALARM;
                              continue;
                        }

                        //    No Alert on moored targets if so requested
                        if(g_bAIS_CPA_Alert_Suppress_Moored &&
                           ((td->NavStatus == MOORED) || (td->NavStatus == AT_ANCHOR) || (td->SOG <= g_ShowMoored_Kts)))
                        {
                              td->n_alarm_state = AIS_NO_ALARM;
                              continue;
                        }


                        //    Skip distant targets if requested
                        if(g_bCPAMax)
                        {
                              if( td->Range_NM > g_CPAMax_NM)
                              {
                                    td->n_alarm_state = AIS_NO_ALARM;
                                    continue;
                              }
                        }

                        if((td->CPA < g_CPAWarn_NM) && (td->TCPA > 0))
                        {
                              if(g_bTCPA_Max)
                              {
                                    if(td->TCPA < g_TCPA_Max)
                                          this_alarm = AIS_ALARM_SET;
                              }
                        else
                              this_alarm = AIS_ALARM_SET;
                        }
                  }

                  //    If the alarm has been acknowledged, we can only turn it off
                  if(td->n_alarm_state == AIS_ALARM_ACKNOWLEDGED)
                  {
                        if(AIS_NO_ALARM == this_alarm)
                              td->n_alarm_state = AIS_NO_ALARM;
                  }
                  else
                        td->n_alarm_state = this_alarm;

            }
      }
}


void AIS_Decoder::UpdateOneCPA(AIS_Target_Data *ptarget)
{
      if(!ptarget->b_positionValid)
            return;

            //    Express the SOGs as meters per hour
      double v0 = gSog         * 1852.;
      double v1 = ptarget->SOG * 1852.;

      if((v0 < 1e-6) && (v1 < 1e-6))
      {
            ptarget->TCPA = 0.;
            ptarget->CPA = 0.;

            ptarget->bCPA_Valid = false;
      }
      else
      {
            //    Calculate the TCPA first

            //    Working on a Reduced Lat/Lon orthogonal plotting sheet....
            //    Get easting/northing to target,  in meters

            double east1 = (ptarget->Lon - gLon) * 60 * 1852;
            double north1 = (ptarget->Lat - gLat) * 60 * 1852;

            double east = east1 * (cos(gLat * PI / 180));;
            double north = north1;

            //    Convert COGs trigonometry to standard unit circle
            double cosa = cos((90. - gCog) * PI / 180.);
            double sina = sin((90. - gCog) * PI / 180.);
            double cosb = cos((90. - ptarget->COG) * PI / 180.);
            double sinb = sin((90. - ptarget->COG) * PI / 180.);


            //    These will be useful
            double fc = (v0 * cosa) - (v1 * cosb);
            double fs = (v0 * sina) - (v1 * sinb);

            //    Here is the equation for t, which will be in hours
            double tcpa = ((fc * east) + (fs * north)) / ((fc * fc) + (fs * fs));

            //    Convert to minutes
            ptarget->TCPA = tcpa * 60.;

            //    Calculate CPA
            //    Using TCPA, predict ownship and target positions

            double OwnshipLatCPA, OwnshipLonCPA, TargetLatCPA, TargetLonCPA;

            ll_gc_ll(gLat,         gLon,         gCog,         gSog * tcpa,         &OwnshipLatCPA, &OwnshipLonCPA);
            ll_gc_ll(ptarget->Lat, ptarget->Lon, ptarget->COG, ptarget->SOG * tcpa, &TargetLatCPA,  &TargetLonCPA);

            //   And compute the distance
            ptarget->CPA = DistGreatCircle(OwnshipLatCPA, OwnshipLonCPA, TargetLatCPA, TargetLonCPA);

            ptarget->bCPA_Valid = true;

            if(ptarget->TCPA  < 0)
                  ptarget->bCPA_Valid = false;
      }

      //    Compute the current Range/Brg to the target
      double brg, dist;
      DistanceBearing(ptarget->Lat, ptarget->Lon, gLat, gLon, &brg, &dist);
      ptarget->Range_NM = dist;
      ptarget->Brg = brg;
}




//------------------------------------------------------------------------------------
//
//  AIS Target Query Support
//
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
//
//      AIS Data Source Support
//
//------------------------------------------------------------------------------------


AIS_Error AIS_Decoder::OpenDataSource(wxFrame *pParent, const wxString& AISDataSource)
{
      m_pParentEventHandler = pParent->GetEventHandler();

      pAIS_Thread = NULL;
      m_sock = NULL;
      m_OK = false;

      TimerAIS.SetOwner(this, TIMER_AIS1);
      TimerAIS.Stop();

      m_data_source_string = AISDataSource;

//      Create and manage AIS data stream source

//    Decide upon source
      wxString msg(_T("AIS Data Source is...."));
      msg.Append(m_data_source_string);
      wxLogMessage(msg);

//      Data Source is private TCP/IP Server
      if(m_data_source_string.Contains(_T("TCP/IP")))
      {
            wxString AIS_data_ip;
            AIS_data_ip = m_data_source_string.Mid(7);         // extract the IP

// Create the socket
            m_sock = new wxSocketClient();

// Setup the event handler and subscribe to most events
            m_sock->SetEventHandler(*this, AIS_SOCKET_ID);

            m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
            m_sock->Notify(TRUE);

            m_busy = FALSE;


//    Build the target address

//    n.b. Win98
//    wxIPV4address::Hostname() uses sockets function gethostbyname() for address resolution
//    Implications...Either name target must exist in c:\windows\hosts, or
//                            a DNS server must be active on the network.
//    If neither true, then wxIPV4address::Hostname() will block (forever?)....
//
//    Workaround....
//    Use a thread to try the name lookup, in case it hangs

            DNSTestThread *ptest_thread = NULL;
            ptest_thread = new DNSTestThread(AIS_data_ip);

            ptest_thread->Run();                      // Run the thread from ::Entry()


//    Sleep and loop for N seconds
#define SLEEP_TEST_SEC  2

            for(int is=0 ; is<SLEEP_TEST_SEC * 10 ; is++)
            {
                  wxMilliSleep(100);
                  if(s_dns_test_flag)
                        break;
            }

            if(!s_dns_test_flag)
            {

                  wxString msg(AIS_data_ip);
                  msg.Prepend(_T("Could not resolve TCP/IP host '"));
                  msg.Append(_T("'\n Suggestion: Try 'xxx.xxx.xxx.xxx' notation"));
                  wxMessageDialog md(pParent, msg, _T("OpenCPN Message"), wxICON_ERROR );
                  md.ShowModal();

                  m_sock->Notify(FALSE);
                  m_sock->Destroy();
                  m_sock = NULL;

                  return AIS_NO_TCP;
            }


            //      Resolved the name, somehow, so Connect() the socket
            addr.Hostname(AIS_data_ip);
            addr.Service(3047/*GPSD_PORT_NUMBER*/);
            m_sock->Connect(addr, FALSE);       // Non-blocking connect
            m_OK = true;
      }


//    AIS Data Source is specified serial port

      else if(m_data_source_string.Contains(_T("Serial")))
      {
          wxString comx;
//          comx =  m_pdata_source_string->Mid(7);        // either "COM1" style or "/dev/ttyS0" style
          comx =  m_data_source_string.AfterFirst(':');      // strip "Serial:"

#ifdef __WXMSW__
          comx.Prepend(_T("\\\\.\\"));                  // Required for access to Serial Ports greater than COM9

//  As a quick check, verify that the specified port is available
            HANDLE m_hSerialComm = CreateFile(comx.mb_str(),       // Port Name
                                             GENERIC_READ,
                                             0,
                                             NULL,
                                             OPEN_EXISTING,
                                             0,
                                             NULL);

            if(m_hSerialComm == INVALID_HANDLE_VALUE)
            {
                  wxString msg(comx);
                  msg.Prepend(_T("  Could not open AIS serial port '"));
                  msg.Append(_T("'\nSuggestion: Try closing other applications."));
                  wxMessageDialog md(pParent, msg, _T("OpenCPN Message"), wxICON_ERROR );
                  md.ShowModal();

                  return AIS_NO_SERIAL;
            }

            else
                  CloseHandle(m_hSerialComm);

//    Kick off the AIS RX thread
            pAIS_Thread = new OCP_AIS_Thread(this, comx);
            pAIS_Thread->Run();
#endif


#ifdef __POSIX__
//    Kick off the NMEA RX thread
            pAIS_Thread = new OCP_AIS_Thread(this, comx);
            pAIS_Thread->Run();
#endif

            m_OK = true;
      }

      if(m_OK)
          TimerAIS.Start(TIMER_AIS_MSEC,wxTIMER_CONTINUOUS);

      return AIS_NoError;

}




void AIS_Decoder::GetSource(wxString& source)
{
      source = m_data_source_string;
}




void AIS_Decoder::Pause(void)
{
      TimerAIS.Stop();

      if(m_sock)
            m_sock->Notify(FALSE);
}

void AIS_Decoder::UnPause(void)
{
    TimerAIS.Start(TIMER_AIS_MSEC,wxTIMER_CONTINUOUS);

      if(m_sock)
            m_sock->Notify(TRUE);
}



void AIS_Decoder::OnSocketEvent(wxSocketEvent& event)
{

#define RD_BUF_SIZE    200

    int nBytes;
    char *bp;
    char buf[RD_BUF_SIZE + 1];
    int char_count;


  switch(event.GetSocketEvent())
  {
      case wxSOCKET_INPUT :                     // from  Daemon
            m_sock->SetFlags(wxSOCKET_NOWAIT);


//          Read the reply, one character at a time, looking for 0x0a (lf)
            bp = buf;
            char_count = 0;

            while (char_count < RD_BUF_SIZE)
            {
                m_sock->Read(bp, 1);
                nBytes = m_sock->LastCount();
                if(*bp == 0x0a)
                {
                    bp++;
                    break;
                }

                bp++;
                char_count++;
            }

            *bp = 0;                        // end string

//          Validate the string

            if(!strncmp((const char *)buf, "!AIVDM", 6))
            {
//                  Decode(buf);

//    Signal the main program thread

//                    wxCommandEvent event( EVT_AIS, ID_AIS_WINDOW );
//                    event.SetEventObject( (wxObject *)this );
//                    event.SetExtraLong(EVT_AIS_DIRECT);
//                    m_pParentEventHandler->AddPendingEvent(event);
            }



    case wxSOCKET_LOST       :
    case wxSOCKET_CONNECTION :
    default                  :
          break;
  }

}

void AIS_Decoder::OnTimerAISAudio(wxTimerEvent& event)
{
      if(g_bAIS_CPA_Alert_Audio && m_bAIS_Audio_Alert_On)
      {
            m_AIS_Sound.Create(g_sAIS_Alert_Sound_File);
            if(m_AIS_Sound.IsOk())
                  m_AIS_Sound.Play();
      }
      m_AIS_Audio_Alert_Timer.Start(TIMER_AIS_AUDIO_MSEC,wxTIMER_CONTINUOUS);
}


void AIS_Decoder::OnTimerAIS(wxTimerEvent& event)
{
      TimerAIS.Stop();

#if 0
      /// testing for orphans in the Select list
      SelectItem *pFindSel;

//    Iterate on the list
      wxSelectableItemListNode *node = pSelectAIS->GetSelectList()->GetFirst();

      int n_sel = 0;
      while(node)
      {
            pFindSel = node->GetData();
            if(pFindSel->m_seltype == SELTYPE_AISTARGET)
            {
                  int mmsi = (int) pFindSel->m_Data4;

                  AIS_Target_Data *tdp = Get_Target_Data_From_MMSI(mmsi);
                  if(tdp == NULL)
                        int yyp = 6;

                  if(tdp->SOG > 200.)
                        int yyu = 4;
            }

            n_sel++;

            node = node->GetNext();
      }

      if(n_sel > m_n_targets)
            int yyr = 3;

      if(n_sel > GetTargetList()->size())
            int yyl = 4;
#endif


      ///testing


      //    Scrub the target hash list
      //    removing any targets older than stipulated age


      wxDateTime now = wxDateTime::Now();
      now.MakeGMT();

      AIS_Target_Hash::iterator it;
      AIS_Target_Hash *current_targets = GetTargetList();

      for( it = (*current_targets).begin(); it != (*current_targets).end(); ++it )
      {

          AIS_Target_Data *td = it->second;

          if(NULL == td)                        // This should never happen, but I saw it once....
          {
                current_targets->erase(it);
                break;                          // leave the loop
          }

          int target_age = now.GetTicks() - td->ReportTicks;
//          printf("Current Target: MMSI: %d    target_age:%d\n", td->MMSI, target_age);

          //      Mark lost targets if specified
          if(g_bMarkLost)
          {
                  if(target_age > g_MarkLost_Mins * 60)
                        td->b_active = false;
          }

          //      Remove lost targets if specified
          double removelost_Mins = fmax(g_RemoveLost_Mins,g_MarkLost_Mins);
          if(g_bRemoveLost)
          {
                if(target_age > removelost_Mins * 60)
                {
                      pSelectAIS->DeleteSelectablePoint((void *)td->MMSI, SELTYPE_AISTARGET);
                      current_targets->erase(it);
                      delete td;
                      break;        // kill only one per tick, since iterator becomes invalid...
                }
          }
      }


//--------------TEST DATA strings
#if 0
      char str[82];
      if(1)
      {
          if(itime++ > 2)
          {
              itime = 0;
              strcpy(str, test_str[istr]);
              istr++;
              if(istr > 23)
                  istr = 0;
              Decode(str);
          }
      }
#endif

      UpdateAllCPA();

      UpdateAllAlarms();

      if(g_bAIS_CPA_Alert)
      {

            m_bAIS_Audio_Alert_On = false;            // default, may be set on

            //    Process any Alarms

            //    If the AIS Alert Dialog is not currently shown....

            //    Show the Alert dialog
            //    Which of multiple targets?
            //    search the list for any targets with alarms, selecting the target with shortest TCPA

            if(NULL == g_pais_alert_dialog_active)
            {
                  double tcpa_min = 1e6;             // really long
                  AIS_Target_Data *palarm_target = NULL;

                  for( it = (*current_targets).begin(); it != (*current_targets).end(); ++it )
                  {
                        AIS_Target_Data *td = it->second;
                        if(td)
                        {
                              if(td->b_active)
                              {
                                    if(AIS_ALARM_SET == td->n_alarm_state)
                                    {
                                          if(td->TCPA < tcpa_min)
                                          {
                                                tcpa_min = td->TCPA;
                                                palarm_target = td;
                                          }
                                    }
                              }
                        }
                  }

                  if(palarm_target)
                  {
                  //    Show the alert

                        AISTargetAlertDialog *pAISAlertDialog = new AISTargetAlertDialog();
                        pAISAlertDialog->Create ( palarm_target->MMSI, m_parent_frame, this, -1, _T("AIS Alert"),
                                                wxPoint( g_ais_alert_dialog_x, g_ais_alert_dialog_y),
                                                wxSize( g_ais_alert_dialog_sx, g_ais_alert_dialog_sy));

                        g_pais_alert_dialog_active = pAISAlertDialog;
                        pAISAlertDialog->Show();                        // Show modeless, so it stays on the screen


                        //    Audio alert if requested
                        m_bAIS_Audio_Alert_On = true;             // always on when alert is first shown
                  }
            }

            //    The AIS Alert dialog is already shown.  If the  dialog MMSI number is still alerted, update the dialog
            //    otherwise, destroy the dialog
            else
            {
                  AIS_Target_Data *palert_target = Get_Target_Data_From_MMSI(g_pais_alert_dialog_active->Get_Dialog_MMSI());

                  if(palert_target)
                  {
                        if(AIS_ALARM_SET == palert_target->n_alarm_state)
                        {
                              g_pais_alert_dialog_active->UpdateText();
                        }
                        else
                        {
                              g_pais_alert_dialog_active->Close();
                              m_bAIS_Audio_Alert_On = false;
                        }

                        if(true == palert_target->b_suppress_audio)
                              m_bAIS_Audio_Alert_On = false;
                        else
                              m_bAIS_Audio_Alert_On = true;
                  }
                  else
                  {                                                     // this should not happen, however...
                        g_pais_alert_dialog_active->Close();
                        m_bAIS_Audio_Alert_On = false;
                  }

            }
      }

      //    At this point, the audio flag is set

      //    Honor the global flag
      if(!g_bAIS_CPA_Alert_Audio)
            m_bAIS_Audio_Alert_On = false;


      if(m_bAIS_Audio_Alert_On)
      {
            if(!m_AIS_Audio_Alert_Timer.IsRunning())
            {
                  m_AIS_Audio_Alert_Timer.SetOwner(this, TIMER_AISAUDIO);
                  m_AIS_Audio_Alert_Timer.Start(TIMER_AIS_AUDIO_MSEC);

                  m_AIS_Sound.Create(g_sAIS_Alert_Sound_File);
                  if(m_AIS_Sound.IsOk())
                        m_AIS_Sound.Play();
            }
      }
      else
            m_AIS_Audio_Alert_Timer.Stop();


      TimerAIS.Start(TIMER_AIS_MSEC,wxTIMER_CONTINUOUS);
}


AIS_Target_Data *AIS_Decoder::Get_Target_Data_From_MMSI(int mmsi)
{
      if(AISTargetList->find( mmsi ) == AISTargetList->end())     // if entry does not exist....
            return NULL;
      else
            return (*AISTargetList)[mmsi];          // find current entry

/*
      //    Search the active target list for a matching MMSI
      AIS_Target_Hash::iterator it;
      AIS_Target_Hash *current_targets =GetTargetList();

      AIS_Target_Data *td_found = NULL;
      for( it = (*current_targets).begin(); it != (*current_targets).end(); ++it )
      {
            AIS_Target_Data *td = it->second;
            if(mmsi == td->MMSI)
            {
                  td_found = td;
                  break;
            }
      }
      return td_found;
*/
}




//-------------------------------------------------------------------------------------------------------------
//
//    AIS Serial Input Thread
//
//    This thread manages reading the AIS data stream from the declared serial port
//
//-------------------------------------------------------------------------------------------------------------

//          Inter-thread communication event implementation
DEFINE_EVENT_TYPE(EVT_AIS)



//-------------------------------------------------------------------------------------------------------------
//    OCP_AIS_Thread Implementation
//-------------------------------------------------------------------------------------------------------------

//    ctor

OCP_AIS_Thread::OCP_AIS_Thread(wxEvtHandler *pParent, const wxString& PortName)
{

      m_pMainEventHandler = pParent;

      m_pPortName = new wxString(PortName);

      rx_buffer = new char[RX_BUFFER_SIZE + 1];
      temp_buf = new char[RX_BUFFER_SIZE + 1];

      put_ptr = rx_buffer;
      tak_ptr = rx_buffer;
      nl_count = 0;

      Create();
}

OCP_AIS_Thread::~OCP_AIS_Thread(void)
{
      delete m_pPortName;
      delete rx_buffer;
      delete temp_buf;

}

void OCP_AIS_Thread::OnExit(void)
{
    //  Mark the global status as dead, gone
    pAIS_Thread = NULL;
}

bool OCP_AIS_Thread::HandleRead(char *buf, int character_count)
{
    // Copy the characters into circular buffer

    char *bp = buf;
    int ichar = character_count;
    while(ichar)
    {
     // keep track of how many nl in the buffer
        if(0x0a == *bp)
            nl_count++;

        *put_ptr++ = *bp++;
        if((put_ptr - rx_buffer) > RX_BUFFER_SIZE)
            put_ptr = rx_buffer;

        ichar--;
    }

///    wxString msg;
///    msg.Printf(_T("(put_ptr-tak_ptr): %d      character_count: %d        nl_count: %d"), (int)(put_ptr-tak_ptr), character_count, nl_count);
///    wxLogMessage(msg);

    if(nl_count < 0)                // "This will never happen..."
        nl_count = 0;

    // If there are any nl's in the buffer
    // There is at least one sentences in the buffer,
    // plus maybe a partial
    // Try hard to send multiple sentences

    if(nl_count)
    {
        bool bmore_data = true;

        while((bmore_data) && (nl_count))
        {
            if(put_ptr != tak_ptr)
            {
                //copy the buffer, looking for nl
                char *tptr;
                char *ptmpbuf;

                tptr = tak_ptr;
                ptmpbuf = temp_buf;

                while((*tptr != 0x0a) && (tptr != put_ptr))
                {
                    *ptmpbuf++ = *tptr++;

                    if((tptr - rx_buffer) > RX_BUFFER_SIZE)
                        tptr = rx_buffer;
                }

                if((*tptr == 0x0a) && (tptr != put_ptr))                // well formed sentence
                {
                    *ptmpbuf++ = *tptr++;
                    if((tptr - rx_buffer) > RX_BUFFER_SIZE)
                        tptr = rx_buffer;

                    *ptmpbuf = 0;

                    tak_ptr = tptr;

    //    Signal the main program thread

                    wxCommandEvent event( EVT_AIS, ID_AIS_WINDOW );
                    event.SetEventObject( (wxObject *)this );
                    event.SetExtraLong(EVT_AIS_PARSE_RX);
                    event.SetString(wxString(temp_buf,  wxConvUTF8));
                    m_pMainEventHandler->AddPendingEvent(event);

///     msg.Printf(_T("         removing: %d,   (put_ptr-tak_ptr): %d,   nl_count:%d"), strlen(temp_buf), (int)(put_ptr-tak_ptr), nl_count);
///     wxLogMessage(msg);

                    nl_count--;
                }   // if good sentence
                else                                 // partial sentence
                {
                    bmore_data = false;
///                    msg.Printf(_T("         partial...   nl_count:%d"), nl_count);
///                    wxLogMessage(msg);
                }

            }   // if data
            else
            {
                bmore_data = false;                  // no more data in buffer
///                msg.Printf(_T("         no data...   nl_count:%d"), nl_count);
///                wxLogMessage(msg);
            }

        }       // while
    }       // if nl

    return true;
}


//      Sadly, the thread itself must implement the underlying OS serial port
//      in a very machine specific way....

#ifdef __POSIX__
//    Entry Point
void *OCP_AIS_Thread::Entry()
{
#if 0
                                     // testing direct file read of AIS data stream
      char buf[100];
      wxFile f("/home/dsr/Desktop/Downloads/SIITECHBIG.TXT");
                           if(f.IsOpened())
                           int yyo = 5;

                           wxFileInputStream stream(f);

                           if(stream.IsOk())
                           int yyp = 5;

                           while(!stream.Eof())
               {
                           stream.Read(buf, 40);
                           HandleRead(buf, 40);                   // Read completed successfully.
}

                           return 0;
#endif
          // Allocate the termios data structures

      pttyset = (termios *)calloc(sizeof (termios), 1);
      pttyset_old = (termios *)malloc(sizeof (termios));

    // Open the requested port.
    //   using O_NDELAY to force ignore of DCD (carrier detect) MODEM line
    if ((m_ais_fd = open(m_pPortName->mb_str(), O_RDWR|O_NDELAY|O_NOCTTY)) < 0)
    {
        wxString msg(_T("AIS input device open failed: "));
        msg.Append(*m_pPortName);
        wxLogMessage(msg);
        return 0;
    }

    /*
     A special test for a user defined FIFO
     To use this method, do the following:
     a.  Create a fifo            $mkfifo /tmp/aisfifo
     b.  netcat into the fifo     $nc {ip} {port} > /tmp/aisfifo                   sample {ip} {port} could be  nc 82.182.117.51 6401 > /tmp/aisfifo
     c.  hand edit opencpn.conf and make AIS data source like this:
          [Settings/AISPort]
          Port=Serial:/tmp/aisfifo
    */
    if(m_pPortName->Contains(_T("fifo")))
          goto port_ready;


      tcsetattr(m_ais_fd, TCSANOW, pttyset);

    if (isatty(m_ais_fd) == 0)
    {
        wxString msg(_T("AIS tty input device isatty() failed, retrying open()...  "));
        msg.Append(*m_pPortName);
        wxLogMessage(msg);

           close(m_ais_fd);
         if ((m_ais_fd = open(m_pPortName->mb_str(), O_RDWR|O_NDELAY|O_NOCTTY)) < 0)
           {
               wxString msg(_T("AIS tty input device open failed on retry, aborting.  "));
               msg.Append(*m_pPortName);
               wxLogMessage(msg);
               close(m_ais_fd);
               return(0);
           }

           if (isatty(m_ais_fd) == 0)
           {
               wxString msg(_T("AIS tty input device isatty() failed on retry, aborting.  "));
               msg.Append(*m_pPortName);
               wxLogMessage(msg);
               close(m_ais_fd);
               return(0);
           }
    }

    if (1/*isatty(m_ais_fd) != 0*/)
    {

      /* Save original terminal parameters */
      if (tcgetattr(m_ais_fd,pttyset_old) != 0)
      {
          wxString msg(_T("AIS tty input device getattr() failed, retrying...  "));
          msg.Append(*m_pPortName);
          wxLogMessage(msg);

        close(m_ais_fd);
        if ((m_ais_fd = open(m_pPortName->mb_str(), O_RDWR|O_NDELAY|O_NOCTTY)) < 0)
          {
              wxString msg(_T("AIS tty input device open failed on retry, aborting.  "));
              msg.Append(*m_pPortName);
              wxLogMessage(msg);
              return 0;
          }

        if (tcgetattr(m_ais_fd,pttyset_old) != 0)
          {
              wxString msg(_T("AIS tty input device getattr failed on retry, aborting.  "));
              msg.Append(*m_pPortName);
              wxLogMessage(msg);
              return 0;
          }
      }

      memcpy(pttyset, pttyset_old, sizeof(termios));

      //  Build the new parms off the old

      // Set blocking/timeout behaviour
      memset(pttyset->c_cc,0,sizeof(pttyset->c_cc));

      pttyset->c_cc[VTIME] = 11;                        // 1.1 sec timeout
      pttyset->c_cc[VEOF]  = 4;                         // EOF Character

      /*
      * No Flow Control
      */
      pttyset->c_cflag &= ~(PARENB | PARODD | CRTSCTS);
      pttyset->c_cflag |= CREAD | CLOCAL;
      pttyset->c_iflag = pttyset->c_oflag = pttyset->c_lflag = (tcflag_t) 0;

      int stopbits = 1;
      char parity = 'N';
      pttyset->c_iflag &=~ (PARMRK | INPCK);
      pttyset->c_cflag &=~ (CSIZE | CSTOPB | PARENB | PARODD);
      pttyset->c_cflag |= (stopbits==2 ? CS7|CSTOPB : CS8);
      switch (parity)
      {
          case 'E':
              pttyset->c_iflag |= INPCK;
              pttyset->c_cflag |= PARENB;
              break;
          case 'O':
              pttyset->c_iflag |= INPCK;
              pttyset->c_cflag |= PARENB | PARODD;
              break;
      }
      pttyset->c_cflag &=~ CSIZE;
      pttyset->c_cflag |= (CSIZE & (stopbits==2 ? CS7 : CS8));

      cfsetispeed(pttyset, B38400);
      cfsetospeed(pttyset, B38400);

      if (tcsetattr(m_ais_fd, TCSANOW, pttyset) != 0)
      {
          wxString msg(_T("AIS tty input device setattr() failed  "));
          msg.Append(*m_pPortName);
          wxLogMessage(msg);
          return 0;
      }

      (void)tcflush(m_ais_fd, TCIOFLUSH);
    }


port_ready:

    bool not_done = true;
    char next_byte = 0;
    ssize_t newdata = 0;

//    The main loop
//    printf("starting\n");


    while(not_done)
    {
        if(TestDestroy())
        {
            not_done = false;                               // smooth exit
///            printf("smooth exit\n");
        }

//#define oldway 1
#ifdef oldway
//    Blocking, timeout protected read of one character at a time
//    Timeout value is set by c_cc[VTIME]
//    Storing incoming characters in circular buffer
//    And watching for new line character
//     On new line character, send notification to parent


        newdata = read(m_ais_fd, &next_byte, 1);            // blocking read of one char
                                                            // return (-1) if no data available, timeout
#else
//      Kernel I/O multiplexing provides a cheap way to wait for chars

        fd_set rfds;
        struct timeval tv;
        int retval;

        //      m_ais_fd to see when it has input.
        FD_ZERO(&rfds);
        FD_SET(m_ais_fd, &rfds);
        // Wait up to 1 second
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        newdata = 0;
        next_byte = 0;

//      wait for a read available on m_ais_fd, we don't care about write or exceptions
        retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);

        if(FD_ISSET(m_ais_fd, &rfds) && (retval != -1) && retval)
            newdata = read(m_ais_fd, &next_byte, 1);            // blocking read of one char
                                                                // bound to succeed
#endif

        if(newdata > 0)
            HandleRead(&next_byte, 1);

    }           // the big while


//          Close the port cleanly
// this is the clean way to do it
//    pttyset_old->c_cflag |= HUPCL;
//    (void)tcsetattr(m_ais_fd,TCSANOW,pttyset_old);
    close(m_ais_fd);

    free (pttyset);
    free (pttyset_old);

    return 0;

}


#endif          //__POSIX__


#ifdef __WXMSW__
//    Entry Point
void *OCP_AIS_Thread::Entry()
{


      bool not_done;

      DWORD dwRead;
      BOOL fWaitingOnRead = FALSE;
      OVERLAPPED osReader = {0};
#define READ_BUF_SIZE 1000
#define READ_LEN_REQUEST 40
      char buf[READ_BUF_SIZE];

#define READ_TIMEOUT      500      // milliseconds

      DWORD dwRes;
      DWORD dwError;

#if 0                                     // testing direct file read of AIS data stream
      wxFile f("C:\\SIITECH.TXT");

      wxFileInputStream stream(f);

      if(stream.IsOk())
            int yyp = 5;

      while(!stream.Eof())
      {
            stream.Read(buf, 40);
            HandleRead(buf, 40);                   // Read completed successfully.
      }

      return 0;
#endif

//    Set up the serial port

      m_hSerialComm = CreateFile(m_pPortName->mb_str(),      // Port Name
                                             GENERIC_READ,              // Desired Access
                                             0,                         // Shared Mode
                                             NULL,                      // Security
                                             OPEN_EXISTING,             // Creation Disposition
                                             FILE_FLAG_OVERLAPPED,
                                             NULL);                     //  Overlapped

      if(m_hSerialComm == INVALID_HANDLE_VALUE)
      {
            error = ::GetLastError();
            goto fail_point;
      }


      if(!SetupComm(m_hSerialComm, 1024, 1024))
            goto fail_point;

      DCB dcbConfig;

      if(GetCommState(m_hSerialComm, &dcbConfig))           // Configuring Serial Port Settings
      {
            dcbConfig.BaudRate = 38400;
            dcbConfig.ByteSize = 8;
            dcbConfig.Parity = NOPARITY;
            dcbConfig.StopBits = ONESTOPBIT;
            dcbConfig.fBinary = TRUE;
            dcbConfig.fParity = TRUE;
      }

      else
            goto fail_point;

      if(!SetCommState(m_hSerialComm, &dcbConfig))
            goto fail_point;

      COMMTIMEOUTS commTimeout;

      TimeOutInSec = 2;

      if(GetCommTimeouts(m_hSerialComm, &commTimeout)) // Configuring Read & Write Time Outs
      {
            commTimeout.ReadIntervalTimeout = 1000*TimeOutInSec;
            commTimeout.ReadTotalTimeoutConstant = 1000*TimeOutInSec;
            commTimeout.ReadTotalTimeoutMultiplier = 0;
            commTimeout.WriteTotalTimeoutConstant = 1000*TimeOutInSec;
            commTimeout.WriteTotalTimeoutMultiplier = 0;
      }

      else
            goto fail_point;

      if(!SetCommTimeouts(m_hSerialComm, &commTimeout))
            goto fail_point;


//    Set up event specification

      if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) // Setting Event Type
            goto fail_point;


// Create the overlapped event. Must be closed before exiting
// to avoid a handle leak.
      osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
      if (osReader.hEvent == NULL)
            goto fail_point;                                     // Error creating overlapped event; abort.


//    The main loop
      not_done = true;

      while(not_done)
      {
            if(TestDestroy())
                  not_done = false;                                     // smooth exit

            if (!fWaitingOnRead)
            {
                        // Issue read operation.
                if (!ReadFile(m_hSerialComm, buf, READ_LEN_REQUEST, &dwRead, &osReader))
                {
                    dwError = GetLastError();

                    if (dwError == ERROR_IO_PENDING)                    // read not delayed?
                          fWaitingOnRead = TRUE;

                    else if(dwError == ERROR_OPERATION_ABORTED)
                          fWaitingOnRead = TRUE;
                    else
                          ClearCommError(m_hSerialComm, NULL, NULL); //goto fail_point;  // Error in communications.
                }
                else
                      HandleRead(buf, dwRead);                         // read completed immediately
            }



            if (fWaitingOnRead)
            {
                dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
                switch(dwRes)
                {
                  // Read completed.
                    case WAIT_OBJECT_0:
                        if (!GetOverlappedResult(m_hSerialComm, &osReader, &dwRead, FALSE))
                              ClearCommError(m_hSerialComm, NULL, NULL); //goto fail_point;  // Error in communications
                        else
                        {
                              if(dwRead)
                                    HandleRead(buf, dwRead);                   // Read completed successfully.
                        }

          //  Reset flag so that another opertion can be issued.
                        fWaitingOnRead = FALSE;
                        break;

                    case WAIT_TIMEOUT:
          // Operation isn't complete yet. fWaitingOnRead flag isn't
          // changed since I'll loop back around, and I don't want
          // to issue another read until the first one finishes.

                          break;

                    default:
          // Error in the WaitForSingleObject; abort.
          // This indicates a problem with the OVERLAPPED structure's
          // event handle.
                          break;
                }                   // switch
            }                       // if
      }                 // big while


fail_point:

      CloseHandle(m_hSerialComm);

      if (osReader.hEvent)
            CloseHandle(osReader.hEvent);

      return 0;
}

#endif            //__WXMSW__


//---------------------------------------------------------------------------------------
//          AISTargetAlertDialog Implementation
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS ( AISTargetAlertDialog, wxDialog )


// AISTargetAlertDialog event table definition

            BEGIN_EVENT_TABLE ( AISTargetAlertDialog, wxDialog )

            EVT_CLOSE(AISTargetAlertDialog::OnClose)
            EVT_BUTTON( ID_ACKNOWLEDGE, AISTargetAlertDialog::OnIdAckClick )
            EVT_BUTTON( ID_SILENCE, AISTargetAlertDialog::OnIdSilenceClick )
            EVT_MOVE( AISTargetAlertDialog::OnMove )
            EVT_SIZE( AISTargetAlertDialog::OnSize )

            END_EVENT_TABLE()


AISTargetAlertDialog::AISTargetAlertDialog( )
{
      Init();
}

AISTargetAlertDialog::~AISTargetAlertDialog( )
{
}


void AISTargetAlertDialog::Init( )
{
      m_target_mmsi = 0;
      m_pparent = NULL;

}


bool AISTargetAlertDialog::Create ( int target_mmsi,
                                    wxWindow *parent, AIS_Decoder *pdecoder,
                                    wxWindowID id, const wxString& caption,
                                    const wxPoint& pos, const wxSize& size, long style )
{

        //    As a display optimization....
        //    if current color scheme is other than DAY,
        //    Then create the dialog ..WITHOUT.. borders and title bar.
        //    This way, any window decorations set by external themes, etc
        //    will not detract from night-vision

      long wstyle = wxDEFAULT_FRAME_STYLE;
      if (( global_color_scheme != GLOBAL_COLOR_SCHEME_DAY ) && ( global_color_scheme != GLOBAL_COLOR_SCHEME_RGB ))
            wstyle |= ( wxNO_BORDER );

      wxSize size_min = size;
      size_min.IncTo(wxSize(500,600));
      if ( !wxDialog::Create ( parent, id, caption, pos, size_min, wstyle ) )
            return false;

      m_target_mmsi = target_mmsi;
      m_pparent = parent;
      m_pdecoder = pdecoder;

      wxFont *dFont = pFontMgr->GetFont(_T("AISTargetAlert"), 12);
      SetFont ( *dFont );
      m_pFont = dFont;

      wxColour back_color = GetGlobalColor ( _T ( "UIBDR" ) );
      SetBackgroundColour ( back_color );

      wxColour text_color = GetGlobalColor ( _T ( "UINFF" ) );          // or UINFD
      SetForegroundColour ( text_color );
//      SetForegroundColour(pFontMgr->GetFontColor(_T("AISTargetAlert")));

      CreateControls();

      if(CanSetTransparent())
            SetTransparent(192);

      // This fits the dialog to the minimum size dictated by
// the sizers
      GetSizer()->Fit ( this );

// This ensures that the dialog cannot be sized smaller
// than the minimum size
      GetSizer()->SetSizeHints ( this );

      return true;
}




void AISTargetAlertDialog::CreateControls()
{

// A top-level sizer
      wxBoxSizer* topSizer = new wxBoxSizer ( wxVERTICAL );
      SetSizer ( topSizer );

// A second box sizer to give more space around the controls
      wxBoxSizer* boxSizer = new wxBoxSizer ( wxVERTICAL );
      topSizer->Add ( boxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 2 );

// Here is the query result

      m_pAlertTextCtl = new AISInfoWin ( this );
      m_pAlertTextCtl->SetHPad(8);
      m_pAlertTextCtl->SetVPad(2);


      wxColour back_color =GetGlobalColor ( _T ( "UIBCK" ) );
      m_pAlertTextCtl->SetBackgroundColour ( back_color );

      wxColour text_color = GetGlobalColor ( _T ( "UINFF" ) );          // or UINFD
      m_pAlertTextCtl->SetForegroundColour ( text_color );
//      m_pAlertTextCtl->SetForegroundColour(pFontMgr->GetFontColor(_T("AISTargetAlert")));


//      wxString alert_text;
//      int n_nl;
//      wxSize pix_size;

      if(GetAlertText())
      {
            m_pAlertTextCtl->AppendText ( m_alert_text );

            wxSize osize = m_pAlertTextCtl->GetOptimumSize();

            m_pAlertTextCtl->SetSize(osize);
            boxSizer->SetMinSize(osize);
            boxSizer->FitInside(m_pAlertTextCtl);
      }
      boxSizer->Add ( m_pAlertTextCtl, 1, wxALIGN_LEFT|wxALL|wxEXPAND, 1 );



// A horizontal box sizer to contain Ack
      wxBoxSizer* AckBox = new wxBoxSizer ( wxHORIZONTAL );
      boxSizer->Add ( AckBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

//    Button color
      wxColour button_color = GetGlobalColor ( _T ( "UIBCK" ) );;

// The Silence button
      wxButton* silence = new wxButton ( this, ID_SILENCE, wxT ( "&Silence Alert" ),
                                    wxDefaultPosition, wxDefaultSize, 0 );
      AckBox->Add ( silence, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
      silence->SetBackgroundColour ( button_color );

// The Ack button
      wxButton* ack = new wxButton ( this, ID_ACKNOWLEDGE, wxT ( "&Acknowledge" ),
                                     wxDefaultPosition, wxDefaultSize, 0 );
      AckBox->Add ( ack, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
      ack->SetBackgroundColour ( button_color );

}

bool AISTargetAlertDialog::GetAlertText()
{
      //    Search the parent AIS_Decoder's target list for specified mmsi

      if(m_pdecoder)
      {
            AIS_Target_Data *td_found = m_pdecoder->Get_Target_Data_From_MMSI(Get_Dialog_MMSI());

            if(td_found)
            {
                  m_alert_text = td_found->BuildQueryResult();
                  return true;
            }
            else
                  return false;
      }
      else
            return false;
}

void AISTargetAlertDialog::UpdateText()
{
      if(GetAlertText())
      {
            m_pAlertTextCtl->Clear();
            m_pAlertTextCtl->AppendText ( m_alert_text );
      }
      if(CanSetTransparent())
            SetTransparent(192);

      m_pAlertTextCtl->SetInsertionPoint(0);
}


void AISTargetAlertDialog::OnClose(wxCloseEvent& event)
{
      Destroy();
      g_pais_alert_dialog_active = NULL;
}


void AISTargetAlertDialog::OnIdAckClick( wxCommandEvent& event )
{
      //    Acknowledge the Alert, and dismiss the dialog
      if(m_pdecoder)
      {
            AIS_Target_Data *td = m_pdecoder->Get_Target_Data_From_MMSI(Get_Dialog_MMSI());
            if(td)
            {
                  if(AIS_ALARM_SET == td->n_alarm_state)
                        td->n_alarm_state = AIS_ALARM_ACKNOWLEDGED;
            }
      }
      Destroy();
      g_pais_alert_dialog_active = NULL;
}

void AISTargetAlertDialog::OnIdSilenceClick( wxCommandEvent& event )
{
      //    Set the suppress audio flag
      if(m_pdecoder)
      {
            AIS_Target_Data *td = m_pdecoder->Get_Target_Data_From_MMSI(Get_Dialog_MMSI());
            if(td)
                  td->b_suppress_audio = true;
      }
}

void AISTargetAlertDialog::OnMove( wxMoveEvent& event )
{
      //    Record the dialog position
      wxPoint p = event.GetPosition();
      g_ais_alert_dialog_x = p.x;
      g_ais_alert_dialog_y = p.y;

      event.Skip();
}

void AISTargetAlertDialog::OnSize( wxSizeEvent& event )
{
      //    Record the dialog size
      wxSize p = event.GetSize();
      g_ais_alert_dialog_sx = p.x;
      g_ais_alert_dialog_sy = p.y;

      event.Skip();
}





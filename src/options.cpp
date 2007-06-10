/******************************************************************************
 * $Id: options.cpp,v 1.2 2007/06/10 02:31:21 bdbcat Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Options Dialog
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
 * $Log: options.cpp,v $
 * Revision 1.2  2007/06/10 02:31:21  bdbcat
 * Cleanup
 *
 * Revision 1.8  2006/12/03 21:29:03  dsr
 * Cleanup AIS options.
 *
 * Revision 1.7  2006/11/01 02:19:08  dsr
 * AIS Support
 *
 * Revision 1.6  2006/10/08 00:37:16  dsr
 * no message
 *
 *
 */

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/generic/progdlgg.h"


#include "dychart.h"

#include "options.h"

#include "navutil.h"
#ifdef USE_S57
#include "s52plib.h"
#endif

extern bool             g_bShowPrintIcon;
extern bool             g_bShowOutlines;
extern wxString         *pNMEADataSource;
extern wxString         *pNMEA_AP_Port;
extern FontMgr          *pFontMgr;
extern wxString         *pAIS_Port;

#ifdef USE_WIFI_CLIENT
extern wxString         *pWIFIServerName;
#endif

#ifdef USE_S57
extern s52plib          *ps52plib;
#endif

//    Some constants
#define ID_CHOICE_NMEA  wxID_HIGHEST + 1


IMPLEMENT_DYNAMIC_CLASS( options, wxDialog )

BEGIN_EVENT_TABLE( options, wxDialog )
//    BUGBUG DSR Must use wxID_TREECTRL to capture tree events.

    EVT_TREE_SEL_CHANGED( wxID_TREECTRL, options::OnDirctrlSelChanged )
    EVT_CHECKBOX( ID_DEBUGCHECKBOX1, options::OnDebugcheckbox1Click )
    EVT_TREE_SEL_CHANGED( ID_DIRCTRL, options::OnDirctrlSelChanged )
    EVT_BUTTON( ID_BUTTONADD, options::OnButtonaddClick )
    EVT_BUTTON( ID_BUTTONDELETE, options::OnButtondeleteClick )
    EVT_BUTTON( ID_BUTTONREBUILD, options::OnButtonrebuildClick )
    EVT_RADIOBOX( ID_RADIOBOX, options::OnRadioboxSelected )
    EVT_BUTTON( xID_OK, options::OnXidOkClick )
    EVT_BUTTON( wxID_CANCEL, options::OnCancelClick )
    EVT_BUTTON( ID_BUTTONFONTCHOOSE, options::OnChooseFont )
    EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, options::OnPageChange)
    EVT_CHOICE( ID_CHOICE_NMEA, options::OnNMEASourceChoice )
    EVT_COMBOBOX( ID_CHOICE_NMEA, options::OnNMEASourceChoice )
    EVT_RADIOBOX(ID_RADIOBOX, options::OnDisplayCategoryRadioButton )
    EVT_BUTTON( ID_CLEARLIST, options::OnButtonClearClick )
    EVT_BUTTON( ID_SELECTLIST, options::OnButtonSelectClick )

END_EVENT_TABLE()


options::options( )
{
}

options::options( wxWindow* parent, wxWindowID id, const wxString& caption, const wxString& Initial_Chart_Dir,
              const wxPoint& pos, const wxSize& size, long style)
{
      pDirCtl = NULL;
      m_pCurrentDirList = NULL;
      m_pWorkDirList = NULL;

      pParent = parent;

    Create(parent, id, caption, pos, size, style, Initial_Chart_Dir);
}


bool options::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos,
                     const wxSize& size, long style, const wxString& Initial_Chart_Dir)
{
    pDebugShowStat = NULL;
    pDirCtl = NULL;
    pSelCtl = NULL;
    pTextCtl = NULL;
    ps57Ctl = NULL;
    ps57CtlListBox = NULL;
    pDispCat = NULL;

    itemStaticBoxSizer11 = NULL;
    pDirCtl = NULL;;
    itemActiveChartStaticBox = NULL;

    m_pinit_chart_dir = (wxString *)&Initial_Chart_Dir;

    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();


    return TRUE;
}


void options::CreateControls()
{

    options* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);


    wxNotebook* itemNotebook4 = new wxNotebook( itemDialog1, ID_NOTEBOOK, wxDefaultPosition,
            wxSize(-1, -1), wxNB_TOP );
    itemBoxSizer2->Add(itemNotebook4, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxGROW, 10);

   //      Add Invariant Notebook buttons
    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer28, 0, wxALIGN_RIGHT|wxALL, 5);

    wxButton* itemButton29 = new wxButton( itemDialog1, xID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton29->SetDefault();
    itemBoxSizer28->Add(itemButton29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton30 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);


    //  Create "Settings" panel

    wxPanel* itemPanel5 = new wxPanel( itemNotebook4, ID_PANEL2, wxDefaultPosition, wxDefaultSize,
                                       wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    itemPanel5->SetSizer(itemBoxSizer6);

    itemNotebook4->AddPage(itemPanel5, _("Settings"));



    //  Debug checkbox
    wxStaticBox* itemStaticBoxSizerDebugStatic = new wxStaticBox(itemPanel5, wxID_ANY, _("Debug"));
    wxStaticBoxSizer* itemStaticBoxSizerDebug = new wxStaticBoxSizer(itemStaticBoxSizerDebugStatic, wxVERTICAL);
    itemBoxSizer6->Add(itemStaticBoxSizerDebug, 0, wxGROW|wxALL, 5);
    pDebugShowStat = new wxCheckBox( itemPanel5, ID_DEBUGCHECKBOX1, _("Show Status and Debug Windows"), wxDefaultPosition,
                             wxSize(-1, -1), 0 );
    pDebugShowStat->SetValue(FALSE);
    itemStaticBoxSizerDebug->Add(pDebugShowStat, 1, wxALIGN_LEFT|wxALL, 5);

    //  Printing checkbox
    wxStaticBox* itemStaticBoxSizerPrintStatic = new wxStaticBox(itemPanel5, wxID_ANY, _("Printing"));
    wxStaticBoxSizer* itemStaticBoxSizerPrint = new wxStaticBoxSizer(itemStaticBoxSizerPrintStatic, wxVERTICAL);
    itemBoxSizer6->Add(itemStaticBoxSizerPrint, 0, wxGROW|wxALL, 5);
    pPrintShowIcon = new wxCheckBox( itemPanel5, ID_DEBUGCHECKBOX1, _("Show Printing Icon"), wxDefaultPosition,
                             wxSize(-1, -1), 0 );
    pPrintShowIcon->SetValue(FALSE);
    itemStaticBoxSizerPrint->Add(pPrintShowIcon, 1, wxALIGN_LEFT|wxALL, 5);

    //  Chart Outlines checkbox
    wxStaticBox* itemStaticBoxSizerCDOStatic = new wxStaticBox(itemPanel5, wxID_ANY, _("Chart Display Options"));
    wxStaticBoxSizer* itemStaticBoxSizerCDO = new wxStaticBoxSizer(itemStaticBoxSizerCDOStatic, wxVERTICAL);
    itemBoxSizer6->Add(itemStaticBoxSizerCDO, 0, wxGROW|wxALL, 5);
    pCDOOutlines = new wxCheckBox( itemPanel5, ID_DEBUGCHECKBOX1, _("Show Chart Outlines"), wxDefaultPosition,
                             wxSize(-1, -1), 0 );
    pCDOOutlines->SetValue(FALSE);
    itemStaticBoxSizerCDO->Add(pCDOOutlines, 1, wxALIGN_LEFT|wxALL, 5);




//    Add NMEA Options Box
    wxStaticBox* itemNMEAStaticBox = new wxStaticBox(itemPanel5, wxID_ANY, _("NMEA Options"));
    wxStaticBoxSizer* itemNMEAStaticBoxSizer = new wxStaticBoxSizer(itemNMEAStaticBox, wxVERTICAL);
    itemBoxSizer6->Add(itemNMEAStaticBoxSizer, 0, wxGROW|wxALL, 5);


//    Add NMEA data source controls
    wxStaticBox* itemNMEASourceStaticBox = new wxStaticBox(itemPanel5, wxID_ANY, _("NMEA Data Source"));
    wxStaticBoxSizer* itemNMEASourceStaticBoxSizer = new wxStaticBoxSizer(itemNMEASourceStaticBox, wxVERTICAL);
    itemNMEAStaticBoxSizer->Add(itemNMEASourceStaticBoxSizer, 0, wxGROW|wxALL, 5);

      m_itemNMEAListBox = new wxComboBox(itemPanel5, ID_CHOICE_NMEA);
      m_itemNMEAListBox->Append( _T("None"));

#ifdef __WXMSW__
      m_itemNMEAListBox->Append( _T("COM1"));
      m_itemNMEAListBox->Append( _T("COM2"));
      m_itemNMEAListBox->Append( _T("COM3"));
      m_itemNMEAListBox->Append( _T("COM4"));
#else
      m_itemNMEAListBox->Append( _T("/dev/ttyS0"));
      m_itemNMEAListBox->Append( _T("/dev/ttyS1"));
#endif

#ifndef OCPN_DISABLE_SOCKETS
      m_itemNMEAListBox->Append( _T("Network GPSD"));
#endif

//    Activate the proper selections
//    n.b. Hard coded indices
      int sidx;
      bool tcp_en = false;
      wxString source;
      source = (*pNMEADataSource);
      if(source.Upper().Contains("SERIAL"))
      {
          wxString sourcex = source.Mid(7);
          sidx = m_itemNMEAListBox->FindString(sourcex);
      }
      else if(source.Upper().Contains("NONE"))
            sidx = 0;
#ifndef OCPN_DISABLE_SOCKETS
      else if(source.Upper().Contains("GPSD"))
      {
          sidx = m_itemNMEAListBox->FindString(_T("Network GPSD"));
          tcp_en = true;
      }
#endif
      else
          sidx = 0;                                 // malformed selection

      if(sidx ==  wxNOT_FOUND)                  // user specified in ComboBox
      {
          wxString nsource = source.AfterFirst(':');
          m_itemNMEAListBox->Append( nsource );
          sidx = m_itemNMEAListBox->FindString(nsource);
      }


      m_itemNMEAListBox->SetSelection(sidx);
      itemNMEASourceStaticBoxSizer->Add(m_itemNMEAListBox, 0, wxGROW|wxALL, 5);

#ifndef OCPN_DISABLE_SOCKETS

//    Add NMEA TCP/IP Server address
      m_itemNMEA_TCPIP_StaticBox = new wxStaticBox(itemPanel5, wxID_ANY, _("GPSD Data Server"));
      m_itemNMEA_TCPIP_StaticBoxSizer = new wxStaticBoxSizer(m_itemNMEA_TCPIP_StaticBox, wxVERTICAL);
      itemNMEAStaticBoxSizer->Add(m_itemNMEA_TCPIP_StaticBoxSizer, 0, wxGROW|wxALL, 5);

      m_itemNMEA_TCPIP_Source = new wxTextCtrl(itemPanel5, wxID_ANY);
      m_itemNMEA_TCPIP_StaticBoxSizer->Add(m_itemNMEA_TCPIP_Source, 0, wxGROW|wxALL, 5);

      m_itemNMEA_TCPIP_StaticBox->Enable(tcp_en);
      m_itemNMEA_TCPIP_Source->Enable(tcp_en);

      if(tcp_en)
      {
            wxString ip;
            ip = source.Mid(5);
            m_itemNMEA_TCPIP_Source->WriteText(ip);
      }
#endif



//    Add Autopilot serial output port controls
      wxStaticBox* itemNMEAAutoStaticBox = new wxStaticBox(itemPanel5, wxID_ANY, _("Autopilot Output Port"));
      wxStaticBoxSizer* itemNMEAAutoStaticBoxSizer = new wxStaticBoxSizer(itemNMEAAutoStaticBox, wxVERTICAL);
      itemNMEAStaticBoxSizer->Add(itemNMEAAutoStaticBoxSizer, 0, wxGROW|wxALL, 5);

      m_itemNMEAAutoListBox = new wxComboBox(itemPanel5, ID_CHOICE_AP);
      m_itemNMEAAutoListBox->Append( _T("None"));


#ifdef __WXMSW__
      m_itemNMEAAutoListBox->Append( _T("COM1"));
      m_itemNMEAAutoListBox->Append( _T("COM2"));
      m_itemNMEAAutoListBox->Append( _T("COM3"));
      m_itemNMEAAutoListBox->Append( _T("COM4"));

#else
      m_itemNMEAAutoListBox->Append( _T("/dev/ttyS0"));
      m_itemNMEAAutoListBox->Append( _T("/dev/ttyS1"));
#endif

      wxString ap_com;
      if(pNMEA_AP_Port->Contains("Serial"))
          ap_com = pNMEA_AP_Port->Mid(7);
      else
          ap_com = "None";

      sidx = m_itemNMEAAutoListBox->FindString(ap_com);
      m_itemNMEAAutoListBox->SetSelection(sidx);

      itemNMEAAutoStaticBoxSizer->Add(m_itemNMEAAutoListBox, 0, wxGROW|wxALL, 5);

//    Add AIS Data Input controls
      wxStaticBox* itemAISStaticBox = new wxStaticBox(itemPanel5, wxID_ANY, _("AIS Data Port"));
      wxStaticBoxSizer* itemAISStaticBoxSizer = new wxStaticBoxSizer(itemAISStaticBox, wxVERTICAL);
      itemNMEAStaticBoxSizer->Add(itemAISStaticBoxSizer, 0, wxGROW|wxALL, 5);

      m_itemAISListBox = new wxComboBox(itemPanel5, ID_CHOICE_AIS);
      m_itemAISListBox->Append( _T("None"));

#ifdef __WXMSW__
      m_itemAISListBox->Append( _T("COM1"));
      m_itemAISListBox->Append( _T("COM2"));
      m_itemAISListBox->Append( _T("COM3"));
      m_itemAISListBox->Append( _T("COM4"));

#else
      m_itemAISListBox->Append( _T("/dev/ttyS0"));
      m_itemAISListBox->Append( _T("/dev/ttyS1"));
#endif

      wxString ais_com;
      if(pAIS_Port->Contains("Serial"))
          ais_com = pAIS_Port->Mid(7);
      else
          ais_com = "None";

      m_itemAISListBox->SetStringSelection(ais_com);

      itemAISStaticBoxSizer->Add(m_itemAISListBox, 0, wxGROW|wxALL, 5);



#ifdef USE_WIFI_CLIENT
//    Add WiFi Options Box
      wxStaticBox* itemWIFIStaticBox = new wxStaticBox(itemPanel5, wxID_ANY, _("WiFi Options"));
      wxStaticBoxSizer* itemWIFIStaticBoxSizer = new wxStaticBoxSizer(itemWIFIStaticBox, wxVERTICAL);
      itemBoxSizer6->Add(itemWIFIStaticBoxSizer, 0, wxGROW|wxALL, 5);

//    Add WiFi TCP/IP Server address
      m_itemWIFI_TCPIP_StaticBox = new wxStaticBox(itemPanel5, wxID_ANY, _("TCP/IP WiFi Data Server"));
      m_itemWIFI_TCPIP_StaticBoxSizer = new wxStaticBoxSizer(m_itemWIFI_TCPIP_StaticBox, wxVERTICAL);
      itemWIFIStaticBoxSizer->Add(m_itemWIFI_TCPIP_StaticBoxSizer, 0, wxGROW|wxALL, 5);

      m_itemWIFI_TCPIP_Source = new wxTextCtrl(itemPanel5, wxID_ANY);
      m_itemWIFI_TCPIP_StaticBoxSizer->Add(m_itemWIFI_TCPIP_Source, 0, wxGROW|wxALL, 5);

      m_itemWIFI_TCPIP_StaticBox->Enable(1);
      m_itemWIFI_TCPIP_Source->Enable(1);

      wxString ip;
      ip = pWIFIServerName->Mid(7);
      m_itemWIFI_TCPIP_Source->WriteText(ip);
#endif




//    Build "Charts" page

    itemPanel9 = new wxPanel( itemNotebook4, ID_PANEL, wxDefaultPosition, wxDefaultSize,
                              wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer10 = new wxBoxSizer(wxVERTICAL);
    itemPanel9->SetSizer(itemBoxSizer10);

    //      Linux wxUNIVERSAL wxGenericDirCtrl is --very-- slow.
    //      Slowness is in ArtProvider, generating the proper icons
    //      So....
    //      Move the dir tree control out of the ctor, and only build it if "CHART" panel is selected.
    //      See this::OnPageChange event handler


    itemNotebook4->AddPage(itemPanel9, _("Charts"));


    //      Build S57 Options page

    ps57Ctl = new wxPanel( itemNotebook4, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxVERTICAL);
    ps57Ctl->SetSizer(itemBoxSizer25);

//    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
//    itemBoxSizer22->Add(itemBoxSizer25, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxGROW, 5);

    wxStaticBox* itemStaticBoxSizer26Static = new wxStaticBox(ps57Ctl, wxID_ANY, _("Chart Display Filters"));
    wxStaticBoxSizer* itemStaticBoxSizer26 = new wxStaticBoxSizer(itemStaticBoxSizer26Static, wxHORIZONTAL);
    itemBoxSizer25->Add(itemStaticBoxSizer26, 0, wxTOP|wxALL|wxGROW, 5);


    wxStaticBox* itemStaticBoxSizer57Static = new wxStaticBox(ps57Ctl, wxID_ANY, _("Mariner's Standard"));
    wxStaticBoxSizer* itemStaticBoxSizer57 = new wxStaticBoxSizer(itemStaticBoxSizer57Static, wxVERTICAL);
    itemStaticBoxSizer26->Add(itemStaticBoxSizer57, 0, wxTOP|wxALL|wxGROW, 5);

    wxString* ps57CtlListBoxStrings = NULL;
    ps57CtlListBox = new wxCheckListBox( ps57Ctl, ID_CHECKLISTBOX, wxDefaultPosition, wxSize(-1, 200), 0,
                                         ps57CtlListBoxStrings, wxLB_SINGLE );
    itemStaticBoxSizer57->Add(ps57CtlListBox, 0, wxALIGN_LEFT|wxALL, 5);

    itemButtonClearList = new wxButton( ps57Ctl, ID_CLEARLIST, _("Clear All"),
            wxDefaultPosition, wxDefaultSize, 0 );
    itemButtonClearList->SetDefault();
    itemStaticBoxSizer57->Add(itemButtonClearList, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemButtonSelectList = new wxButton( ps57Ctl, ID_SELECTLIST, _("Select All"),
            wxDefaultPosition, wxDefaultSize, 0 );
    itemButtonSelectList->SetDefault();
    itemStaticBoxSizer57->Add(itemButtonSelectList, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);



    wxBoxSizer* itemBoxSizer75 = new wxBoxSizer(wxVERTICAL);
    itemStaticBoxSizer26->Add(itemBoxSizer75, 0, wxTOP|wxALL, 5);

    wxString pDispCatStrings[] = {
        _("&Base"),
        _("&Standard"),
        _("&Other"),
        _("&MarinersStandard")
    };
    pDispCat = new wxRadioBox( ps57Ctl, ID_RADIOBOX, _("Display Category"), wxDefaultPosition, wxDefaultSize,
                               4, pDispCatStrings, 1, wxRA_SPECIFY_COLS );
    itemBoxSizer75->Add(pDispCat, 0, wxTOP|wxALL|wxGROW, 5);


    pCheck_SOUNDG = new wxCheckBox( ps57Ctl, ID_SOUNDGCHECKBOX, _("ShowSoundings"), wxDefaultPosition,
                                     wxSize(-1, -1), 0 );
    pCheck_SOUNDG->SetValue(FALSE);
    itemBoxSizer75->Add(pCheck_SOUNDG, 1, wxALIGN_LEFT|wxALL|wxGROW, 5);

    pCheck_META = new wxCheckBox( ps57Ctl, ID_METACHECKBOX, _("META Objects"), wxDefaultPosition,
            wxSize(-1, -1), 0 );
    pCheck_META->SetValue(FALSE);
    itemBoxSizer75->Add(pCheck_META, 1, wxALIGN_LEFT|wxALL|wxGROW, 5);

    pCheck_SHOWTEXT = new wxCheckBox( ps57Ctl, ID_TEXTCHECKBOX, _("ShowText"), wxDefaultPosition,
            wxSize(-1, -1), 0 );
    pCheck_SHOWTEXT->SetValue(FALSE);
    itemBoxSizer75->Add(pCheck_SHOWTEXT, 1, wxALIGN_LEFT|wxALL|wxGROW, 5);

    pCheck_SCAMIN = new wxCheckBox( ps57Ctl, ID_SCAMINCHECKBOX, _("SCAMIN"), wxDefaultPosition,
            wxSize(-1, -1), 0 );
    pCheck_SCAMIN->SetValue(FALSE);
    itemBoxSizer75->Add(pCheck_SCAMIN, 1, wxALIGN_LEFT|wxALL|wxGROW, 5);


    wxStaticBox* itemStaticBoxSizer83Static = new wxStaticBox(ps57Ctl, wxID_ANY, _("Chart Display Style"));
    wxStaticBoxSizer* itemStaticBoxSizer83 = new wxStaticBoxSizer(itemStaticBoxSizer83Static, wxHORIZONTAL);
    itemBoxSizer25->Add(itemStaticBoxSizer83, 0, wxTOP|wxALL|wxGROW, 5);

    wxString pPointStyleStrings[] = {
        _("&Paper Chart"),
        _("&Simplified"),
    };
    pPointStyle = new wxRadioBox( ps57Ctl, ID_RADIOBOX, _("Points"), wxDefaultPosition, wxDefaultSize,
                                  2, pPointStyleStrings, 1, wxRA_SPECIFY_COLS );
    itemStaticBoxSizer83->Add(pPointStyle, 0, wxTOP|wxALL| 5);

    wxString pBoundStyleStrings[] = {
        _("&Plain"),
        _("&Symbolized"),
    };
    pBoundStyle = new wxRadioBox( ps57Ctl, ID_RADIOBOX, _("Boundaries"), wxDefaultPosition, wxDefaultSize,
                                              2, pBoundStyleStrings, 1, wxRA_SPECIFY_COLS );
    itemStaticBoxSizer83->Add(pBoundStyle, 0, wxTOP|wxALL| 5);

    itemNotebook4->AddPage(ps57Ctl, _("S52 Options"));


    //      Build Fonts panel
    wxPanel* itemPanelFont = new wxPanel( itemNotebook4, ID_PANELFONT, wxDefaultPosition, wxDefaultSize,
                                          wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizerFontPanel = new wxBoxSizer(wxVERTICAL);
    itemPanelFont->SetSizer(itemBoxSizerFontPanel);

    wxStaticBox* itemFontStaticBox = new wxStaticBox(itemPanelFont, wxID_ANY, _("Font Options"));
    wxStaticBoxSizer* itemFontStaticBoxSizer = new wxStaticBoxSizer(itemFontStaticBox, wxVERTICAL);
    itemBoxSizerFontPanel->Add(itemFontStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxStaticBox* itemFontElementStaticBox = new wxStaticBox(itemPanelFont, wxID_ANY, _("Text Element"));
    wxStaticBoxSizer* itemFontElementStaticBoxSizer = new wxStaticBoxSizer(itemFontElementStaticBox, wxVERTICAL);
    itemFontStaticBoxSizer->Add(itemFontElementStaticBoxSizer, 0, wxGROW|wxALL, 5);

    m_itemFontElementListBox = new wxComboBox(itemPanelFont, ID_CHOICE_FONTELEMENT);

    int nFonts = pFontMgr->GetNumFonts();
    for( int it = 0 ; it < nFonts ; it++)
    {
          wxString *t = pFontMgr->GetDialogString(it);
          m_itemFontElementListBox->Append( *t );
    }

    if(nFonts)
          m_itemFontElementListBox->SetSelection(0);

    itemFontElementStaticBoxSizer->Add(m_itemFontElementListBox, 0, wxGROW|wxALL, 5);

    wxButton* itemFontChooseButton = new wxButton( itemPanelFont, ID_BUTTONFONTCHOOSE, _("Choose Font..."),
                wxDefaultPosition, wxDefaultSize, 0 );
    itemFontElementStaticBoxSizer->Add(itemFontChooseButton, 0, wxGROW|wxALL, 5);


    itemNotebook4->AddPage(itemPanelFont, _("Fonts"));


    pSettingsCB1 = pDebugShowStat;
}



void options::SetInitialSettings()
{
      wxString dirname;

      if(m_pCurrentDirList)
      {
            int nDir = m_pCurrentDirList->GetCount();

            for(int i=0 ; i<nDir ; i++)
            {
                  dirname = m_pCurrentDirList->Item(i);
                  if(!dirname.IsEmpty())
                  {
                      if (pTextCtl)
                      {
                          pTextCtl->AppendText(dirname);
                          pTextCtl->AppendText(_T("\n"));
                      }
                  }
            }
      }

//    Settings too

      pSettingsCB1->SetValue(m_pConfig->m_bShowDebugWindows);
      pPrintShowIcon->SetValue(g_bShowPrintIcon);
      pCDOOutlines->SetValue(g_bShowOutlines);


#ifdef USE_S57
//    S52 Primary Filters
      ps57CtlListBox->Clear();

      for(unsigned int iPtr = 0 ; iPtr < ps52plib->pOBJLArray->GetCount() ; iPtr++)
      {
            OBJLElement *pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));

            ps57CtlListBox->Append(wxString(pOLE->OBJLName));
            ps57CtlListBox->Check(ps57CtlListBox->GetCount()-1, pOLE->nViz);
      }

//    Diplay Category
      if(ps52plib)
      {
            int nset = 2;                             // default OTHER

            switch(ps52plib->m_nDisplayCategory)
            {
            case (DISPLAYBASE):
                  nset = 0;
                  break;
            case (STANDARD):
                  nset = 1;
                  break;
            case (OTHER):
                  nset = 2;
                  break;
            case (MARINERS_STANDARD):
                  nset = 3;
                  break;
            default:
                    nset = 3;
                    break;
            }

            pDispCat->SetSelection(nset);


      ps57CtlListBox->Enable(MARINERS_STANDARD == ps52plib->m_nDisplayCategory);
      itemButtonClearList->Enable(MARINERS_STANDARD == ps52plib->m_nDisplayCategory);
      itemButtonSelectList->Enable(MARINERS_STANDARD == ps52plib->m_nDisplayCategory);

      //  Other Display Filters
      pCheck_SOUNDG->SetValue(ps52plib->m_bShowSoundg);
      pCheck_META->SetValue(ps52plib->m_bShowMeta);
      pCheck_SHOWTEXT->SetValue(ps52plib->m_bShowS57Text);
      pCheck_SCAMIN->SetValue(ps52plib->m_bUseSCAMIN);

      // Chart Display Style
      if(ps52plib->m_nSymbolStyle == PAPER_CHART)
          pPointStyle->SetSelection(0);
      else
          pPointStyle->SetSelection(1);

      if(ps52plib->m_nBoundaryStyle == PLAIN_BOUNDARIES)
          pBoundStyle->SetSelection(0);
      else
          pBoundStyle->SetSelection(1);
    }
#endif
}


void options::OnDisplayCategoryRadioButton( wxCommandEvent& event)
{
   int select = pDispCat->GetSelection();

    if(3 == select)
    {
        ps57CtlListBox->Enable();
        itemButtonClearList->Enable();
        itemButtonSelectList->Enable();
    }

    else
    {
        ps57CtlListBox->Disable();
        itemButtonClearList->Disable();
        itemButtonSelectList->Disable();
    }

    event.Skip();
}

void options::OnButtonClearClick( wxCommandEvent& event )
{
    int nOBJL = ps57CtlListBox->GetCount();
    for( int iPtr = 0 ; iPtr < nOBJL ; iPtr++)
        ps57CtlListBox->Check(iPtr, false);

   event.Skip();
}


void options::OnButtonSelectClick( wxCommandEvent& event )
{
    int nOBJL = ps57CtlListBox->GetCount();
    for( int iPtr = 0 ; iPtr < nOBJL ; iPtr++)
        ps57CtlListBox->Check(iPtr, true);

    event.Skip();
}


void options::OnDirctrlSelChanged( wxTreeEvent& event )
{
    if(pDirCtl)
      {
            wxString SelDir;
            SelDir = pDirCtl->GetPath();
            if(pSelCtl)
            {
                pSelCtl->Clear();
                pSelCtl->AppendText(SelDir);
            }
      }

      event.Skip();
}


bool options::ShowToolTips()
{
    return TRUE;
}

void options::OnButtonaddClick( wxCommandEvent& event )
{
      wxString SelDir;
      SelDir = pDirCtl->GetPath();

      pTextCtl->AppendText(SelDir);
      pTextCtl->AppendText(_T("\n"));

      event.Skip();
}



void options::OnXidOkClick( wxCommandEvent& event )
{
//    Handle Chart Tab
      wxString dirname;

      if(pTextCtl)
      {
            int n = pTextCtl->GetNumberOfLines();
            if(m_pWorkDirList)
            {
                    m_pWorkDirList->Clear();
                    for(int i=0 ; i<n ; i++)
                    {
                        dirname = pTextCtl->GetLineText(i);
                        if(!dirname.IsEmpty())
                                m_pWorkDirList->Add(dirname);
                    }
            }
      }
      else
      {
          if(m_pCurrentDirList)
          {
              m_pWorkDirList->Clear();
              int nDir = m_pCurrentDirList->GetCount();

              for(int i=0 ; i<nDir ; i++)
              {
                  dirname = m_pCurrentDirList->Item(i);
                  if(!dirname.IsEmpty())
                      m_pWorkDirList->Add(dirname);
              }
          }
      }


//    Handle Settings Tab

      if(m_pConfig)
      {
            m_pConfig->m_bShowDebugWindows = pSettingsCB1->GetValue();
      }

    g_bShowPrintIcon = pPrintShowIcon->GetValue();
    g_bShowOutlines = pCDOOutlines->GetValue();


//    NMEA Options

// Source
      wxString sel(m_itemNMEAListBox->GetStringSelection());
      if(sel.Contains("COM"))
            sel.Prepend("Serial:");
      else if(sel.Contains("/dev"))
          sel.Prepend("Serial:");
      else if(sel.Contains("GPSD"))
      {
            sel.Empty();
            sel.Append("GPSD:");
            sel.Append(m_itemNMEA_TCPIP_Source->GetLineText(0));
      }
    *pNMEADataSource = sel;

// AP Output
    wxString selp(m_itemNMEAAutoListBox->GetStringSelection());
    if(selp.Contains("COM"))
        selp.Prepend("Serial:");
    else if(selp.Contains("/dev"))
        selp.Prepend("Serial:");
    *pNMEA_AP_Port = selp;

// AIS Input
    wxString selais(m_itemAISListBox->GetStringSelection());
    if(selais.Contains("COM"))
        selais.Prepend("Serial:");
    else if(selais.Contains("/dev"))
        selais.Prepend("Serial:");
    *pAIS_Port = selais;

#ifdef USE_WIFI_CLIENT
// WiFi
    wxString WiFiSource;
    WiFiSource.Empty();
    WiFiSource.Append("TCP/IP:");
    WiFiSource.Append(m_itemWIFI_TCPIP_Source->GetLineText(0));
    *pWIFIServerName = WiFiSource;
#endif

#ifdef USE_S57
    //    Handle s57 Tab
      int nOBJL = ps57CtlListBox->GetCount();

      for( int iPtr = 0 ; iPtr < nOBJL ; iPtr++)
      {
            OBJLElement *pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));

            pOLE->nViz = ps57CtlListBox->IsChecked(iPtr);
    }


      if(ps52plib)
      {
            enum _DisCat nset = OTHER;
            switch(pDispCat->GetSelection())
            {
            case 0:
                  nset = DISPLAYBASE;
                  break;
            case 1:
                  nset = STANDARD;
                  break;
            case 2:
                  nset = OTHER;
                  break;
            case 3:
                  nset = MARINERS_STANDARD;
                  break;
            }
            ps52plib->m_nDisplayCategory = nset;

            ps52plib->m_bShowSoundg =  pCheck_SOUNDG->GetValue();
            ps52plib->m_bShowMeta =    pCheck_META->GetValue();
            ps52plib->m_bShowS57Text = pCheck_SHOWTEXT->GetValue();
            ps52plib->m_bUseSCAMIN =   pCheck_SCAMIN->GetValue();

            if(0 == pPointStyle->GetSelection())
                ps52plib->m_nSymbolStyle = PAPER_CHART;
            else
                ps52plib->m_nSymbolStyle = SIMPLIFIED;

            if(0 == pBoundStyle->GetSelection())
                ps52plib->m_nBoundaryStyle = PLAIN_BOUNDARIES;
            else
                ps52plib->m_nBoundaryStyle = SYMBOLIZED_BOUNDARIES;

            ps52plib->UpdateMarinerParams();

      }
#endif

      EndModal(1);

}


void options::OnButtondeleteClick( wxCommandEvent& event )
{

      wxString dirname;

      pTextCtl->Cut();

      int n = pTextCtl->GetNumberOfLines();
      if(m_pWorkDirList)
      {
            m_pWorkDirList->Clear();


            for(int i=0 ; i<n ; i++)
            {
                  dirname = pTextCtl->GetLineText(i);
                  if(!dirname.IsEmpty())
                        m_pWorkDirList->Add(dirname);
            }

            pTextCtl->Clear();

            int nDir = m_pWorkDirList->GetCount();
            for(int id=0 ; id<nDir ; id++)
            {
                  dirname = m_pWorkDirList->Item(id);
                  if(!dirname.IsEmpty())
                  {
                        pTextCtl->AppendText(dirname);
                        pTextCtl->AppendText(_T("\n"));
                  }
            }
      }

      event.Skip();
}



void options::OnButtonrebuildClick( wxCommandEvent& event )
{
    event.Skip();
}



void options::OnDebugcheckbox1Click( wxCommandEvent& event )
{
    event.Skip();
}



void options::OnCancelClick( wxCommandEvent& event )
{
      EndModal(0);
}

void options::OnRadioboxSelected( wxCommandEvent& event )
{
    event.Skip();
}

void options::OnChooseFont( wxCommandEvent& event )
{
      wxString sel_text_element = m_itemFontElementListBox->GetStringSelection();

      wxFont *psfont;
      wxFontData font_data;

      wxFont *pif = pFontMgr->GetFont(sel_text_element);
      wxFontData init_font_data;
      if(pif)
            init_font_data.SetInitialFont(*pif);

#ifdef __WXX11__
      X11FontPicker dg(pParent, init_font_data);
#else
      wxFontDialog dg(pParent, init_font_data);
#endif
      int retval = dg.ShowModal();
      if(wxID_CANCEL != retval)
      {
            font_data = dg.GetFontData();
            wxFont font = font_data.GetChosenFont();
            psfont = new wxFont(font);
            pFontMgr->SetFont(sel_text_element, psfont);
      }

      event.Skip();
}


void options::OnPageChange(wxNotebookEvent& event)
{
      int i = event.GetSelection();

      //    User selected Chart Page?
      //    If so, build the "Charts" page variants
      //    Also, show a progress dialog, since getting the GenericTreeCtrl may be slow
      //    and the user needs feedback
      if(1 == i)                        // 1 is the index of "Charts" page
      {


          itemBoxSizer10->Clear(true);
          pSelCtl = NULL;
          pDirCtl = NULL;

          //    "Available" tree control and selection
          wxStaticBox* itemStaticBoxSizer11Static = new wxStaticBox(itemPanel9, wxID_ANY,
                  _("Available Chart Directories"));
          itemStaticBoxSizer11 = new wxStaticBoxSizer(itemStaticBoxSizer11Static, wxVERTICAL);
          itemBoxSizer10->Add(itemStaticBoxSizer11, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxGROW, 5);
          pDirCtl = new wxGenericDirCtrl( itemPanel9, ID_DIRCTRL, *m_pinit_chart_dir, wxDefaultPosition,
                                          wxSize(-1, -1), 0, _T("All files (*.*)|*.*"), 0 );
          pDirCtl->SetMinSize(wxSize(-1, 160));
          itemStaticBoxSizer11->Add(pDirCtl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxGROW, 5);


          pSelCtl = new wxTextCtrl( itemPanel9, ID_TEXTCTRL1, _T(""), wxDefaultPosition, wxSize(-1, -1), 0 );
          itemStaticBoxSizer11->Add(pSelCtl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxGROW, 5);

          wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
          itemStaticBoxSizer11->Add(itemBoxSizer14, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
          wxButton* itemButton15 = new wxButton( itemPanel9, ID_BUTTONADD, _("Add Selection"), wxDefaultPosition, wxDefaultSize, 0 );
          itemBoxSizer14->Add(itemButton15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);


          //    "Active" list
          itemActiveChartStaticBox = new wxStaticBox(itemPanel9, wxID_ANY, _("Active Chart Directories"));
          wxStaticBoxSizer* itemStaticBoxSizer16 = new wxStaticBoxSizer(itemActiveChartStaticBox, wxVERTICAL);
          itemBoxSizer10->Add(itemStaticBoxSizer16, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxGROW, 5);


          int tc_flags = wxTE_MULTILINE;
          //    wxX11 has some trouble with wxTextCtrl....
          //    In this case, wxTE_DONTWRAP causes corruption of the control's parent data structures....
          //    So, dont do that...
#ifndef __WXX11__
          tc_flags |= wxTE_DONTWRAP;
#endif

          pTextCtl = new wxTextCtrl( itemPanel9, ID_TEXTCTRL, _T(""),
                                     wxDefaultPosition, wxSize(-1, -1), tc_flags  );
          pTextCtl->SetMinSize(wxSize(-1, 100));
          itemStaticBoxSizer16->Add(pTextCtl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxGROW, 5);

          wxButton* itemButton18 = new wxButton( itemPanel9, ID_BUTTONDELETE, _("Delete Selection"), wxDefaultPosition, wxDefaultSize, 0 );
          itemStaticBoxSizer16->Add(itemButton18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

          //        Fill in the control variable data

          //        Currently selected chart dirs
          wxString dirname;
          if(m_pCurrentDirList)
          {
              int nDir = m_pCurrentDirList->GetCount();
              for(int i=0 ; i<nDir ; i++)
              {
                  dirname = m_pCurrentDirList->Item(i);
                  if(!dirname.IsEmpty())
                  {
                      if (pTextCtl)
                      {
                          pTextCtl->AppendText(dirname);
                          pTextCtl->AppendText(_T("\n"));
                      }
                  }
              }
          }

          //        Selected Directory
          wxString SelDir;
          SelDir = pDirCtl->GetPath();
          pSelCtl->Clear();
          pSelCtl->AppendText(SelDir);

          itemBoxSizer10->Layout();
      }
//#endif
}


void options::OnNMEASourceChoice(wxCommandEvent& event)
{
    int i = event.GetSelection();
    wxString src(m_itemNMEAListBox->GetString(i));
    if(src.Contains("GPSD"))
    {
        m_itemNMEA_TCPIP_StaticBox->Enable();
        m_itemNMEA_TCPIP_Source->Enable();

        m_itemNMEA_TCPIP_Source->Clear();
        m_itemNMEA_TCPIP_Source->WriteText("localhost"); // default

        wxString source;
        source = *pNMEADataSource;
        if(source.Contains("GPSD"))
        {
            wxString ip;
            ip = source.Mid(5);
            if(ip.Len())
            {
                m_itemNMEA_TCPIP_Source->Clear();
                m_itemNMEA_TCPIP_Source->WriteText(ip);
            }
        }
    }
    else
    {
        m_itemNMEA_TCPIP_StaticBox->Disable();
        m_itemNMEA_TCPIP_Source->Disable();
    }
}

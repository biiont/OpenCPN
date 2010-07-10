/*
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301, USA.

    ---
    Copyright (C) 2010, Anders Lund <anders@alweb.dk>
 */

#ifndef _RouteManagerDialog_h_
#define _RouteManagerDialog_h_

#include <wx/dialog.h>
#include <wx/timer.h>
#include <wx/listctrl.h>

class wxButton;
class Route;

class RouteManagerDialog : public wxDialog {
      DECLARE_EVENT_TABLE()

      public:
            RouteManagerDialog(wxWindow *parent);
            ~RouteManagerDialog();
            void UpdateRouteListCtrl();     // Rebuild route list
            void UpdateTrkListCtrl();
            void UpdateWptListCtrl();

      private:
            void Create();
            void UpdateRteButtons();           // Correct button state
            void MakeAllRoutesInvisible();  // Mark all routes as invisible. Does not flush settings.
            void ZoomtoRoute(Route *route); // Attempt to zoom route into the view
            void UpdateTrkButtons();           // Correct button state
            void UpdateWptButtons();           // Correct button state

            // event handlers
            void OnRteDeleteClick(wxCommandEvent &event);
            void OnRtePropertiesClick(wxCommandEvent &event);
            void OnRteZoomtoClick(wxCommandEvent &event);
            void OnRteActivateClick(wxCommandEvent &event);
            void OnRteReverseClick(wxCommandEvent &event);
            void OnRteExportClick(wxCommandEvent &event);
            void OnRteToggleVisibility(wxMouseEvent &event);
            void OnRteBtnLeftDown(wxMouseEvent &event); // record control key state for some action buttons
            void OnRteDeleteAllClick(wxCommandEvent &event);
            void OnRteSelected(wxListEvent &event);
            void OnRteSendToGPSClick(wxCommandEvent &event);
            void OnRteDefaultAction(wxListEvent &event);
            void OnTrkDefaultAction(wxListEvent &event);
            void OnTrkNewClick(wxCommandEvent &event);
            void OnTrkPropertiesClick(wxCommandEvent &event);
            void OnTrkDeleteClick(wxCommandEvent &event);
            void OnTrkExportClick(wxCommandEvent &event);
            void OnTrkRouteFromTrackClick(wxCommandEvent &event);
            void OnTrkDeleteAllClick(wxCommandEvent &event);
            void OnTrkSelected(wxListEvent &event);
            void OnTrkToggleVisibility(wxMouseEvent &event);
            void OnWptDefaultAction(wxListEvent &event);
            void OnWptNewClick(wxCommandEvent &event);
            void OnWptPropertiesClick(wxCommandEvent &event);
            void OnWptZoomtoClick(wxCommandEvent &event);
            void OnWptDeleteClick(wxCommandEvent &event);
            void OnWptGoToClick(wxCommandEvent &event);
            void OnWptExportClick(wxCommandEvent &event);
            void OnWptSendToGPSClick(wxCommandEvent &event);
            void OnWptDeleteAllClick(wxCommandEvent &event);
            void OnWptSelected(wxListEvent &event);
            void OnWptToggleVisibility(wxMouseEvent &event);
            void OnImportClick(wxCommandEvent &event);
            void OnExportClick(wxCommandEvent &event);

            // properties
            wxListCtrl *m_pRouteListCtrl;
            wxListCtrl *m_pTrkListCtrl;
            wxListCtrl *m_pWptListCtrl;

            wxButton *btnRteProperties;
            wxButton *btnRteActivate;
            wxButton *btnRteZoomto;
            wxButton *btnRteReverse;
            wxButton *btnRteDelete;
            wxButton *btnRteExport;
            wxButton *btnRteSendToGPS;
            wxButton *btnTrkProperties;
            wxButton *btnTrkDelete;
            wxButton *btnTrkExport;
            wxButton *btnTrkRouteFromTrack;
            wxButton *btnWptProperties;
            wxButton *btnWptZoomto;
            wxButton *btnWptDelete;
            wxButton *btnWptGoTo;
            wxButton *btnWptExport;
            wxButton *btnWptSendToGPS;
            wxButton *btnImport;
            wxButton *btnExport;

            bool m_bPossibleClick;    // do
            bool m_bCtrlDown;         // record control key state for some action buttons
            bool m_bNeedConfigFlush;  // if true, update config in destructor

};

#endif // _RouteManagerDialog_h_
// kate: indent-width 6; indent-mode cstyle; space-indent on;
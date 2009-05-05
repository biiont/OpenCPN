/******************************************************************************
 * $Id: s52plib.h,v 1.15 2009/05/05 04:02:49 bdbcat Exp $
 *
 * Project:  OpenCP
 * Purpose:  S52 Presentation Library
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
 * $Log: s52plib.h,v $
 * Revision 1.15  2009/05/05 04:02:49  bdbcat
 * *** empty log message ***
 *
 * Revision 1.14  2009/04/19 02:23:52  bdbcat
 * *** empty log message ***
 *
 * Revision 1.13  2009/04/18 03:33:21  bdbcat
 * *** empty log message ***
 *
 * Revision 1.12  2009/04/13 02:34:17  bdbcat
 * Add "show ATON labels" option
 *
 * Revision 1.11  2009/03/26 22:35:35  bdbcat
 * Opencpn 1.3.0 Update
 *
 * Revision 1.10  2008/12/19 01:46:39  bdbcat
 * Add selectable depth unit conversion for S57 charts
 *
 * Revision 1.9  2008/08/26 13:49:53  bdbcat
 * Better color scheme support
 *
 * Revision 1.8  2008/08/09 23:36:46  bdbcat
 * *** empty log message ***
 *
 * Revision 1.7  2008/03/30 23:23:08  bdbcat
 * *** empty log message ***
 *
 * $Log: s52plib.h,v $
 * Revision 1.15  2009/05/05 04:02:49  bdbcat
 * *** empty log message ***
 *
 * Revision 1.14  2009/04/19 02:23:52  bdbcat
 * *** empty log message ***
 *
 * Revision 1.13  2009/04/18 03:33:21  bdbcat
 * *** empty log message ***
 *
 * Revision 1.12  2009/04/13 02:34:17  bdbcat
 * Add "show ATON labels" option
 *
 * Revision 1.11  2009/03/26 22:35:35  bdbcat
 * Opencpn 1.3.0 Update
 *
 * Revision 1.10  2008/12/19 01:46:39  bdbcat
 * Add selectable depth unit conversion for S57 charts
 *
 * Revision 1.9  2008/08/26 13:49:53  bdbcat
 * Better color scheme support
 *
 * Revision 1.8  2008/08/09 23:36:46  bdbcat
 * *** empty log message ***
 *
 * Revision 1.7  2008/03/30 23:23:08  bdbcat
 * *** empty log message ***
 *
 * Revision 1.6  2008/01/12 06:18:50  bdbcat
 * Update for Mac OSX/Unicode
 *
 * Revision 1.5  2007/05/03 13:31:19  dsr
 * Major refactor for 1.2.0
 *
 * Revision 1.4  2007/03/02 02:06:32  dsr
 * Convert to UTM Projection
 *
 * Revision 1.3  2006/10/07 03:50:54  dsr
 * *** empty log message ***
 *
 * Revision 1.2  2006/09/21 01:38:23  dsr
 * Major refactor/cleanup
 *
 *
 */


#ifndef _S52PLIB_H_
#define _S52PLIB_H_


#include "s52s57.h"                 //types

//    Dynamic arrays of pointers need explicit macros in wx261
#ifdef __WX261
WX_DEFINE_ARRAY_PTR(S57attVal *, wxArrayOfS57attVal);
#else
WX_DEFINE_ARRAY(S57attVal *, wxArrayOfS57attVal);
#endif



//    wxWindows Hash Map Declarations
#include <wx/hashmap.h>
//class LUPHash;
class RuleHash;

//WX_DECLARE_HASH_MAP( wxString, LUPrec*, wxStringHash , wxStringEqual, LUPHash );
WX_DECLARE_HASH_MAP( wxString, Rule*, wxStringHash , wxStringEqual, RuleHash );

WX_DEFINE_SORTED_ARRAY(LUPrec *, wxArrayOfLUPrec);

WX_DECLARE_STRING_HASH_MAP( wxColour, ColourHash );

WX_DECLARE_LIST(S57Obj, ObjList);



class ViewPort;
class PixelCache;

//-----------------------------------------------------------------------------
//    s52plib definition
//-----------------------------------------------------------------------------

class s52plib
{
public:
      s52plib(const wxString& PLib);
      ~s52plib();

      void  SetPPMM(float ppmm){ canvas_pix_per_mm = ppmm;}
      LUPrec  *S52_LUPLookup(LUPname LUP_name, const char * objectName, S57Obj *pObj, bool bStrict = 0);
      int   _LUP2rules(LUPrec *LUP, S57Obj *pObj);
      color *S52_getColor(char *colorName);
      wxColour S52_getwxColour(const wxString &colorName);

      void UpdateMarinerParams(void);
      void ClearCNSYLUPArray(void);

      void SetPLIBColorScheme(wxString scheme);
      bool ObjectRenderCheck(ObjRazRules *rzRules, ViewPort *vp);

//    Temporarily save/restore the current colortable index
//    Useful for Thumbnail rendering
      void SaveColorScheme(void){m_colortable_index_save = m_colortable_index;}
      void RestoreColorScheme(void){m_colortable_index_save = m_colortable_index_save;}

//    Rendering stuff
      void PrepareForRender(void);
      void AdjustTextList(int dx, int dy,  int screenw, int screenh);
      void ClearTextList(void);
      int _draw(wxDC *pdc, ObjRazRules *rzRules, ViewPort *vp);
      int RenderArea(wxDC *pdc, ObjRazRules *rzRules, ViewPort *vp, render_canvas_parms *pb_spec);
      int SetLineFeaturePriority( ObjRazRules *rzRules, int npriority );

 // Accessors
      bool GetShowS57Text(){return m_bShowS57Text;}
      void SetShowS57Text(bool f){m_bShowS57Text = f;}

      bool GetShowS57ImportantTextOnly(){return m_bShowS57ImportantTextOnly;}
      void SetShowS57ImportantTextOnly(bool f){m_bShowS57ImportantTextOnly = f;}

      int GetMajorVersion(void){return m_VersionMajor;}
      int GetMinorVersion(void){return m_VersionMinor;}

      void SetTextOverlapAvoid(bool f){m_bDeClutterText = f;}
      void SetShowAtonText(bool f){m_bShowAtonText = f;}
      void SetShowLdisText(bool f){m_bShowLdisText = f;}

 //Todo accessors
      DisCat      m_nDisplayCategory;
      LUPname     m_nSymbolStyle;
      LUPname     m_nBoundaryStyle;
      bool        m_bOK;

      bool        m_bShowSoundg;
      bool        m_bShowMeta;
      bool        m_bShowS57Text;
      bool        m_bUseSCAMIN;
      bool        m_bShowAtonText;
      bool        m_bShowLdisText;
      bool        m_bShowS57ImportantTextOnly;
      bool        m_bDeClutterText;

      int         m_nDepthUnitDisplay;

//  Todo Make this type safe, it is always an array of (OBJLElement *)
      wxArrayPtrVoid    *pOBJLArray;    // Used for Display Filtering
      RuleHash          *_symb_sym;     // symbol symbolisation rules

  private:
      int   S52_load_Plib(const wxString& PLib);
      bool  S52_flush_Plib();

      int RenderTX(ObjRazRules *rzRules, Rules *rules, ViewPort *vp);
      int RenderTE(ObjRazRules *rzRules, Rules *rules, ViewPort *vp);
      int RenderSY(ObjRazRules *rzRules, Rules *rules, ViewPort *vp);
      int RenderLS(ObjRazRules *rzRules, Rules *rules, ViewPort *vp);
      int RenderLC(ObjRazRules *rzRules, Rules *rules, ViewPort *vp);
      int RenderMPS(ObjRazRules *rzRules, Rules *rules, ViewPort *vp);
      int RenderCARC(ObjRazRules *rzRules, Rules *rules, ViewPort *vp);
      char *RenderCS(ObjRazRules *rzRules, Rules *rules);

      int RenderToBufferAC(ObjRazRules *rzRules, Rules *rules, ViewPort *vp, render_canvas_parms *pb_spec);
      int RenderToBufferAP(ObjRazRules *rzRules, Rules *rules, ViewPort *vp, render_canvas_parms *pb_spec);

      void RenderToBufferFilledPolygon(ObjRazRules *rzRules, S57Obj *obj, color *c, wxBoundingBox &BBView,
               render_canvas_parms *pb_spec, render_canvas_parms *patt_spec);

      void draw_lc_poly(wxDC *pdc, wxPoint *ptp, int npt,
                        float sym_len, float sym_factor, Rule *draw_rule, ViewPort *vp);

      bool RenderHPGLtoDC(char *str, char *col, wxDC *pdc, wxPoint &r, wxPoint &pivot, double rot_angle = 0);
      bool RenderHPGL(ObjRazRules *rzRules, Rule * rule_in, wxDC *pdc, wxPoint &r,  ViewPort *vp, float rot_angle = 0.);
      bool RenderRasterSymbol(ObjRazRules *rzRules, Rule *prule, wxDC *pdc, wxPoint &r,  ViewPort *vp, float rot_angle = 0.);
      wxImage RuleXBMToImage(Rule *prule);

//      bool RenderText ( wxDC *pdc, wxFont *pFont, const wxString& str,
//            int x, int y, int xoff_unit, int yoff_unit, color *pcol, wxRect *pRectDrawn,
//            S57Obj *pobj, bool bCheckOverlap );
      bool RenderText ( wxDC *pdc, S52_Text *ptext, int x, int y, wxRect *pRectDrawn, S57Obj *pobj, bool bCheckOverlap );

      bool CheckTextRectList( const wxRect &test_rect,  S57Obj *pobj);
      int  RenderT_All ( ObjRazRules *rzRules, Rules *rules, ViewPort *vp, bool bTX );

      int PrioritizeLineFeature ( ObjRazRules *rzRules, int npriority);

      int dda_tri(wxPoint *ptp, color *c, render_canvas_parms *pb_spec, render_canvas_parms *pPatt_spec);
      int dda_trap(wxPoint *segs, int lseg, int rseg, int ytop, int ybot, color *c, render_canvas_parms *pb_spec, render_canvas_parms *pPatt_spec );

      wxArrayOfLUPrec *SelectLUPARRAY(LUPname TNAM);

      LUPrec *FindBestLUP(wxArrayPtrVoid *nameMatch,char *objAtt, wxArrayOfS57attVal *objAttVal, bool bStrict);
      Rules *StringToRules(const wxString& str_in);
      void GetAndAddCSRules(ObjRazRules *rzRules, Rules *rules);

      int ReadS52Line( char *pBuffer, char *delim, int nCount, FILE *fp );
      int ChopS52Line(char *pBuffer, char c);
      int ParsePos(position *pos, char *buf, bool patt);
      int ParseLBID(FILE *fp);
      int ParseCOLS(FILE *fp);
      int ParseLUPT(FILE *fp);
      int ParseLNST(FILE *fp);
      int ParsePATT(FILE *fp);
      int ParseSYMB(FILE *fp, RuleHash *pHash);
      int _CIE2RGB(void);
      bool FindUnusedColor(void);
      void CreateColourHash(void);
//      int LoadColors(const wxString& ColorFile);

      void DestroyPattRules(RuleHash *rh);
      void DestroyPatternRuleNode(Rule *pR);

      void DestroyRules(RuleHash *rh);
      void DestroyRuleNode(Rule *pR);

      void DestroyLUPArray(wxArrayOfLUPrec *pLUPArray);
      void DestroyLUP(LUPrec *pLUP);

      bool TextRenderCheck(ObjRazRules *rzRules);

//    Library data

      wxArrayPtrVoid          *pAlloc;

// working buffer
#define  MAX_BUF  1024
      char buffer[MAX_BUF];
      char *pBuf;


// Look-Up --

// Symbolisation Rules --
      RuleHash *_line_sym;                // line symbolisation rules
      RuleHash *_patt_sym;                // pattern symbolisation rules
      RuleHash *_cond_sym;                // conditional symbolisation rules
      RuleHash *_symb_symR;               // symbol symbolisation rules, Raster


//    Sorted Arrays of LUPrecs
      wxArrayOfLUPrec *lineLUPArray;            // lines
      wxArrayOfLUPrec *areaPlaineLUPArray;      // areas: PLAIN_BOUNDARIES
      wxArrayOfLUPrec *areaSymbolLUPArray;      // areas: SYMBOLIZED_BOUNDARIE
      wxArrayOfLUPrec *pointSimplLUPArray;      // points: SIMPLIFIED
      wxArrayOfLUPrec *pointPaperLUPArray;      // points: PAPER_CHART
      wxArrayOfLUPrec *condSymbolLUPArray;      // Dynamic Conditional Symbology

      int         m_LUPSequenceNumber;

      wxArrayPtrVoid *ColorTableArray;
      wxArrayPtrVoid *ColourHashTableArray;

      float       canvas_pix_per_mm;            // Set by parent, used to scale symbols/lines/patterns

      color       unused_color;
      bool        bUseRasterSym;

      wxDC       *pdc;                       // The current DC

      int         *ledge;
      int         *redge;

      int         m_colortable_index;
      int         m_colortable_index_save;

      ObjList     m_textObjList;

      int         m_VersionMajor;
      int         m_VersionMinor;

      double      m_display_pix_per_mm;


};

#endif //_S52PLIB_H_

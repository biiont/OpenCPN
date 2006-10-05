/******************************************************************************
 * $Id: s52cnsy.cpp,v 1.2 2006/09/21 01:37:36 dsr Exp $
 *
 * Project:  OpenCPN
 * Purpose:  S52 Conditional Symbology Library
 * Author:   David Register, Sylvain Duclos
 *
 ***************************************************************************
 *   Copyright (C) $YEAR$ by $AUTHOR$   *
 *   $EMAIL$   *
 *
 *   Copyright (C) 2000-2001  Sylvain Duclos
 *   sylvain_duclos@yahoo.com
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
 * $Log: s52cnsy.cpp,v $
 * Revision 1.2  2006/09/21 01:37:36  dsr
 * Major refactor/cleanup
 *
 * Revision 1.1.1.1  2006/08/21 05:52:19  dsr
 * Initial import as opencpn, GNU Automake compliant.
 *
 * Revision 1.5  2006/08/04 11:42:02  dsr
 * no message
 *
 * Revision 1.4  2006/06/15 02:44:15  dsr
 * Implement more CS
 *
 * Revision 1.3  2006/06/02 02:12:32  dsr
 * More CS
 *
 * Revision 1.2  2006/05/19 19:11:06  dsr
 * Implement some additional rules
 *
 * Revision 1.1.1.1  2006/04/19 03:23:28  dsr
 * Rename/Import to OpenCPN
 *
 *
 */

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "dychart.h"

#include "s57chart.h"
#include "s52plib.h"
#include "s52utils.h"

extern bool GetFloatAttr(S57Obj *obj, char *AttrName, float &val);


// size of attributes value list buffer
#define LISTSIZE   16   // list size


extern s52plib  *ps52plib;

CPL_CVSID("$Id: s52cnsy.cpp,v 1.2 2006/09/21 01:37:36 dsr Exp $");

static void *CLRLIN01(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        printf("s52csny : CLRLIN01 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *DATCVR01(void *param)
{

// Remarks: This conditional symbology procedure describes procedures for:
// - symbolizing the limit of ENC coverage;
// - symbolizing navigational purpose boundaries ("scale boundarie"); and
// - indicating overscale display.
           //
// Note that the mandatory meta object CATQUA is symbolized by the look-up table.
           //
// Because the methods adopted by an ECDIS to meet the IMO and IHO requirements
// listed on the next page will depend on the manufacturer's software, and cannot be
// described in terms of a flow chart in the same way as other conditional procedures,
// this procedure is in the form of written notes.

//    ObjRazRules *rzRules = (ObjRazRules *)param;
//    S57Obj *obj = rzRules->obj;

    wxString rule_str;
       ///////////////////////
    // 1- REQUIREMENT
    // (IMO/IHO specs. explenation)

       ///////////////////////
    // 2- ENC COVERAGE
       //
    // 2.1- Limit of ENC coverage
    //datcvr01 = g_string_new(";OP(3OD11060);LC(HODATA01)");
    rule_str.Append("LC(HODATA01)");
    // FIXME: get cell extend

    // 2.2- No data areas
    // This can be done outside of CS (ie when clearing the screen in Mesa)
    // FIXME: ";OP(0---);AC(NODATA)"
    // FIXME: set geo to cover earth (!)

       //////////////////////
    // 3- SCALE BOUNDARIES
       //
    // 3.1- Chart scale boundaties
    // FIXME;
    //g_string_append(datcvr01, ";LS(SOLD,1,CHGRD)");
    // -OR- LC(SCLBDYnn) (?)
       //
    // ;OP(3OS21030)

    // 3.2- Graphical index of navigational purpose
    // FIXME: draw extent of available SENC in DB

       //////////////////////
    // 4- OVERSCALE
       //
    // FIXME: get meta date CSCL of DSPM field
    // FIXME: get object M_CSCL or CSCALE
       //
    // 4.1- Overscale indication
       // FIXME: compute, scale = [denominator of the compilation scale] /
    //                         [denominator of the display scale]
    // FIXME: draw overscale indication (ie TX("X%3.1f",scale))
       //
    // 4.2- Ovescale area at a chart scale boundary
    // FIXME: test if next chart is over scale (ie going from large scale chart
    //        to a small scale chart)
    // FIXME: draw AP(OVERSC01) on overscale part of display
    //g_string(";OP(3OS21030)");

       //
    // 4.3- Larger scale data available
    // FIXME: display indication of better scale available (?)




   wxString datcvr01;
   datcvr01.Append(rule_str);
   datcvr01.Append('\037');

   char *r = (char *)malloc(datcvr01.Len() + 1);
   strcpy(r, datcvr01.c_str());

   return r;

}

bool GetIntAttr(S57Obj *obj, char *AttrName, int &val)
{
        char *attList = (char *)(obj->attList->c_str());        //attList is wxString

        char *patl = attList;
        char *patr;
        int idx = 0;
        while(*patl)
        {
                patr = patl;
                while(*patr != '\037')
                        patr++;

                if(!strncmp(patl, AttrName, 6))
                        break;

                patl = patr + 1;
                idx++;
        }

        if(!*patl)                                                      // Requested Attribute not found
        {
                return false;                                           // so don't return a value
        }

//      using idx to get the attribute value
        wxArrayOfS57attVal      *pattrVal = obj->attVal;

        S57attVal *v = pattrVal->Item(idx);
        val = *(int*)(v->value);

        return true;
}

/*
bool GetFloatAttr(S57Obj *obj, char *AttrName, float &val)
{
        char *attList = (char *)(obj->attList->c_str());        //attList is wxString

        char *patl = attList;
        char *patr;
        int idx = 0;
        while(*patl)
        {
                patr = patl;
                while(*patr != '\037')
                        patr++;

                if(!strncmp(patl, AttrName, 6))
                        break;

                patl = patr + 1;
                idx++;
        }

        if(!*patl)                                                      // Requested Attribute not found
        {
                return false;                                           // so don't return a value
        }

//      using idx to get the attribute value
        wxArrayOfS57attVal      *pattrVal = obj->attVal;

        S57attVal *v = pattrVal->Item(idx);
        val = *(float*)(v->value);

        return true;
}

*/
bool GetStringAttr(S57Obj *obj, char *AttrName, char *pval, int nc)
{
        char *attList = (char *)(obj->attList->c_str());        //attList is wxString

        char *patl = attList;
        char *patr;
        int idx = 0;
        while(*patl)
        {
                patr = patl;
                while(*patr != '\037')
                        patr++;

                if(!strncmp(patl, AttrName, 6))
                        break;

                patl = patr + 1;
                idx++;
        }

        if(!*patl)
                return false;

//      using idx to get the attribute value
        wxArrayOfS57attVal      *pattrVal = obj->attVal;

        S57attVal *v = pattrVal->Item(idx);

        char *val = (char *)(v->value);

        strncpy(pval, val, nc);

        return true;
}

wxString *GetStringAttrWXS(S57Obj *obj, char *AttrName)
{

        char *attList = (char *)(obj->attList->c_str());        //attList is wxString

        char *patl = attList;
        char *patr;
        int idx = 0;
        while(*patl)
        {
                patr = patl;
                while(*patr != '\037')
                        patr++;

                if(!strncmp(patl, AttrName, 6))
                        break;

                patl = patr + 1;
                idx++;
        }

        if(!*patl)
                return NULL;

//      using idx to get the attribute value
        wxArrayOfS57attVal      *pattrVal = obj->attVal;

        S57attVal *v = pattrVal->Item(idx);

        char *val = (char *)(v->value);

        wxString *ret = new wxString(val);

        return ret;
}

static int      _parseList(const char *str, char *buf)
// Put a string of comma delimited number in an array (buf).
// Return: the number of value in buf.
// Assume: - number < 256,
//         - list size less then LISTSIZE-1 .
// Note: buf is \0 terminated for strpbrk().
{
    int i = 0;

    if (NULL != str && *str != '\0') {
        do {
            if ( i>= LISTSIZE-1) {
                printf("OVERFLOW --value in list lost!!\n");
                break;
            }

            /*
            if (255 <  (unsigned char) atoi(str)) {
                PRINTF("value overflow (>255)\n");
                exit(0);
            }
            */

            buf[i++] = (unsigned char) atoi(str);

            while(isdigit(*str++));   // next
            //while( g_ascii_isdigit(c));   // next

        } while(*str++ != '\0');      // skip ',' or exit
    }

    buf[i] = '\0';

    return i;
}


static int      _atPtPos(S57Obj *objNew, wxArrayPtrVoid *curntList, int bSectorCheck)
// return TRUE if there is a light at this position
// or if its an extended arc radius else FALSE
{
    unsigned int i;

    for (i=0; i<curntList->GetCount(); i++) {
        S57Obj *objOld = (S57Obj *)curntList->Item(i);

        if ((objOld->x == objNew->x) && (objOld->y == objNew->y)) {

            if (!bSectorCheck)
                return TRUE;
/*
            else {
                // check for extend arc radius
                GString *Asectr1str = S57_getAttVal(geoOld, "SECTR1");
                GString *Asectr2str = S57_getAttVal(geoOld, "SECTR2");
                GString *Bsectr1str = S57_getAttVal(geoNew, "SECTR1");
                GString *Bsectr2str = S57_getAttVal(geoNew, "SECTR2");

                // check  present
                if (NULL == Asectr1str ||
                    NULL == Asectr1str ||
                    NULL == Asectr1str ||
                    NULL == Asectr1str)
                    return FALSE;

                {
                    double Asectr1 = atof(Asectr1str->str);
                    double Asectr2 = atof(Asectr2str->str);
                    double Bsectr1 = atof(Bsectr1str->str);
                    double Bsectr2 = atof(Bsectr2str->str);
                    double Asweep = (Asectr1 > Asectr2) ?
                        Asectr2-Asectr1+360 : Asectr2-Asectr1;
                    double Bsweep = (Bsectr1 > Bsectr2) ?
                        Bsectr2-Bsectr1+360 : Bsectr2-Bsectr1;

                    // check sector overlap
                    if (Asectr2<=Bsectr1 || Asectr1>=Bsectr2) {
                        if (Asweep == Bsweep) {
                            g_string_truncate(Bsectr2str, 0);
                            g_string_sprintf(Bsectr2str, "%f",Bsectr2-1);
                            S57_setAtt(geoNew, "SECTR2", Bsectr2str->str);
                        }

                        return FALSE;
                    }

                    // check if other sector larger
                    if (Asweep >= Bsweep)
                        return TRUE;
                }
            }
*/
        }
    }

    return FALSE;
}






// Remarks: An object of the class "depth area" is coloured and covered with fill patterns
// according to the mariners selections of shallow contour, safety contour and
// deep contour. This requires a decision making process based  on DRVAL1 and DRVAL2.
// Objects of the class "dredged area" are handled by this routine as well to
// ensure a consistent symbolization of areas that represent the surface of the
// seabed.

static void *DEPARE01(void *param)
{

   ObjRazRules *rzRules = (ObjRazRules *)param;
   S57Obj *obj = rzRules->obj;


   float drval1, drval2;

//      Determine the color based on mariner selections


   drval1 = -1.0;                                          // default values
   GetFloatAttr(obj, "DRVAL1", drval1);
   drval2 = drval1 + 0.01;
   GetFloatAttr(obj, "DRVAL2", drval2);




   //   Create a string of the proper color reference

    bool shallow  = TRUE;
    wxString rule_str ="AC(DEPIT)";


    if (drval1 >= 0.0 && drval2 > 0.0)
        rule_str  = "AC(DEPVS)";

    if (TRUE == S52_getMarinerParam(S52_MAR_TWO_SHADES))
    {
        if (drval1 >= S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)  &&
            drval2 >  S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR))
        {
            rule_str  = "AC(DEPDW)";
            shallow = FALSE;
        }
    }
    else
    {
        if (drval1 >= S52_getMarinerParam(S52_MAR_SHALLOW_CONTOUR) &&
            drval2 >  S52_getMarinerParam(S52_MAR_SHALLOW_CONTOUR))
            rule_str  = "AC(DEPMS)";

        if (drval1 >= S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)  &&
                drval2 >  S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR))
        {
            rule_str  = "AC(DEPMD)";
            shallow = FALSE;
        }

        if (drval1 >= S52_getMarinerParam(S52_MAR_DEEP_CONTOUR)  &&
                drval2 >  S52_getMarinerParam(S52_MAR_DEEP_CONTOUR))
        {
            rule_str  = "AC(DEPDW)";
            shallow = FALSE;
        }

    }


//  If object is DRGARE....

    if(!strncmp(rzRules->LUP->OBCL, "DRGARE", 6))
    {
        rule_str.Append(";AP(DRGARE01)");
        rule_str.Append(";LS(DASH,1,CHGRF)");

// Todo Restrictions
/*
        char pval[30];
        if(true == GetStringAttr(obj, "RESTRN", pval, 20))
        {
            GString *rescsp01 = _RESCSP01(geo);
            if (NULL != rescsp01)
            {
                g_string_append(depare01, rescsp01->str);
                g_string_free(rescsp01, TRUE);
            }
        }
*/
    }


    rule_str.Append('\037');

    char *r = (char *)malloc(rule_str.Len() + 1);
    strcpy(r, rule_str.c_str());
    return r;

}

static void *DEPCNT02(void *param)
{
 //       ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        //      Add another rule onto the ruleList
/*
    Rules *r = NULL;

    r = (Rules*)calloc(1, sizeof(Rules));
    r->ruleType = RUL_SIM_LN;
    r->INSTstr = "SOLD,1,DEPCN";                                // points into the plib data space
*/
        return NULL;

}

static void *DEPVAL01(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        printf("s52csny : DEPVAL01 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *LEGLIN02(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        printf("s52csny : LEGLIN02 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *LIGHTS04(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
        S57Obj *obj = rzRules->obj;

        wxString rule_str;


        char col_str[2];
        GetStringAttr(obj, "COLOUR", col_str, 1);

        float height_val = 0;
        GetFloatAttr(obj, "HEIGHT", height_val);

//      if(obj->attList->Contains(wxString("HEIGHT")))
//              int uupr = 5;

//      Different symbology depending upon Paper or Simplified Mariner Selection


    wxString key;
    if(ps52plib->m_nSymbolStyle == PAPER_CHART)
    {
            if(col_str[0] == '3')
            {                                                                       // red
                    if(height_val)
                    {
                            key = "LIGHTS93";                       // all round
                            rule_str = "SY(LIGHTS93)";
                    }
                    else
                    {
                            key = "LIGHTS01";                       // flare
                            rule_str = "SY(LIGHTS01)";
                    }
            }

            else if(col_str[0] == '4')                      // green
            {
                    if(height_val)
                    {
                            key = "LIGHTS92";                       // all round
                            rule_str = "SY(LIGHTS92)";
                    }
                    else
                    {
                            key = "LIGHTS02";                       // flare
                            rule_str = "SY(LIGHTS02)";
                    }
            }

            else                                                            // Generic, shows as yellow
            {
                    if(height_val)
                    {
                            key = "LIGHTS91";                       // all round
                            rule_str = "SY(LIGHTS91)";
                    }
                    else
                    {
                            key = "LIGHTS03";                       // flare
                            rule_str = "SY(LIGHTS03)";
                    }
            }
    }

    else                                                                    // must be Simplified, so use flares
    {
            if(col_str[0] == '3')
            {                                                                       // red
                    key = "LIGHTS01";
                    rule_str = "SY(LIGHTS01)";
            }

            else if(col_str[0] == '4')                      // green
            {
                    key = "LIGHTS02";
                    rule_str = "SY(LIGHTS02)";
            }

            else                                                            // generic, shows as yellow
            {
                    key = "LIGHTS03";
                    rule_str = "SY(LIGHTS03)";
            }
    }



    rule_str.Append('\037');

    char *r = (char *)malloc(rule_str.Len() + 1);
    strcpy(r, rule_str.c_str());
    return r;
}

static void *LITDSN01(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        printf("s52csny : LITDSN01 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *OBSTRN04(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        static int f03;
        if(!f03)
            printf("s52csny : OBSTRN04 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
        f03++;
   return NULL;
}

static void *OWNSHP02(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        printf("s52csny : OWNSHP02 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *PASTRK01(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        printf("s52csny : PASTRK01 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *QUALIN01(void *param);
static void *QUAPNT01(void *param);

static void *QUAPOS01(void *param)
// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// to the spatial object, not the feature object.
//
// This procedure passes the object to procedure QUALIN01 or QUAPNT01,
// which traces back to the spatial object, retrieves any QUAPOS attributes,
// and returns the appropriate symbolization to QUAPOS01.
{
    ObjRazRules *rzRules = (ObjRazRules *)param;
    S57Obj *obj = rzRules->obj;

    char *quapos01 = NULL;

    if (GEO_LINE == obj->Primitive_type)
        quapos01 = (char *)QUALIN01(param);
    else
        quapos01 = (char *)QUAPNT01(param);

    return quapos01;
}

static void *QUALIN01(void *param)
// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// A line object may be composed of more than one spatial object.
//
// This procedure looks at each of the spatial
// objects, and symbolizes the line according to the positional accuracy.
{
    ObjRazRules *rzRules = (ObjRazRules *)param;
    S57Obj *obj = rzRules->obj;

    wxString qualino1;
    int quapos = 0;
    bool bquapos = GetIntAttr(obj, "QUAPOS", quapos);
    char    *line      = NULL;

    if (bquapos) {
        if ( 2 <= quapos && quapos < 10)
            line = "LC(LOWACC21)";
    } else {
        if (!strncmp("COALNE", rzRules->LUP->OBCL, 6)) {
            int conrad;
            bool bconrad = GetIntAttr(obj, "CONRAD", conrad);

            if (bconrad) {
                if (1 == conrad)
                    line = "LS(SOLD,3,CHMGF);LS(SOLD,1,CSTLN)";
                else
                    line = "LS(SOLD,1,CSTLN)";
            } else
                line = "LS(SOLD,1,CSTLN)";

        } else  //LNDARE
            line = "LS(SOLD,1,CSTLN)";
    }

    if (NULL != line)
        qualino1.Append(line);

    qualino1.Append('\037');

    char *r = (char *)malloc(qualino1.Len() + 1);
    strcpy(r, qualino1.c_str());


    return r;
}

static void *QUAPNT01(void *param)
// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// This procedure retrieves any QUAPOS attributes, and returns the
// appropriate symbols to the calling procedure.
{
    ObjRazRules *rzRules = (ObjRazRules *)param;
    S57Obj *obj = rzRules->obj;

    wxString quapnt01;
    int accurate  = TRUE;
    int quapos;
    bool bquapos = GetIntAttr(obj, "QUAPOS", quapos);

    if (bquapos) {
        if ( 2 <= quapos && quapos < 10)
            accurate = FALSE;
    }

    if (!accurate)
        quapnt01.Append("SY(LOWACC01)");


    quapnt01.Append('\037');

    char *r = (char *)malloc(quapnt01.Len() + 1);
    strcpy(r, quapnt01.c_str());


    return r;

}



static void *SLCONS03(void *param)

    // Remarks: Shoreline construction objects which have a QUAPOS attribute on their
// spatial component indicating that their position is unreliable are symbolized
// by a special linestyle in the place of the varied linestyles normally used.
// Otherwise this procedure applies the normal symbolization.
{
    ObjRazRules *rzRules = (ObjRazRules *)param;
    S57Obj *obj = rzRules->obj;


    wxString slcons03;

    bool bvalstr;
    int ival;

    char    *cmdw      = NULL;   // command word

    int quapos;
    bool bquapos = GetIntAttr(obj, "QUAPOS", quapos);


    if (GEO_POINT == obj->Primitive_type) {
        if (bquapos) {
            if (2 <= quapos && quapos < 10)
                cmdw ="SY(LOWACC01)";
        }
    } else {
        // GEO_LINE and GEO_AREA are the same
        if (bquapos) {
            if (2 <= quapos && quapos < 10)
                cmdw ="LC(LOWACC01)";
        } else {
            bvalstr = GetIntAttr(obj, "CONDTN", ival);

            if (bvalstr && ( 1 == ival || 2 == ival))
                    cmdw = "LS(DASH,1,CSTLN)";
            else {
                ival = 0;
                bvalstr  = GetIntAttr(obj, "CATSLC", ival);

                if (bvalstr && ( 6  == ival || 15 == ival || 16 == ival ))
                        cmdw = "LS(SOLD,4,CSTLN)";
                else {
                    bvalstr = GetIntAttr(obj, "WATLEV", ival);

                    if (bvalstr && 2 == ival)
                            cmdw = "LS(SOLD,2,CSTLN)";
                    else
                        if (bvalstr && (3 == ival || 4 == ival))
                            cmdw = "LS(DASH,2,CSTLN)";
                        else
                            cmdw = "LS(SOLD,2,CSTLN)";  // default

                }
            }
        }
    }



    // WARNING: not explicitly specified in S-52 !!
    // FIXME:this is to put AC(DEPIT) --intertidal area
    // Could this be bug in OGR ?
    /*
    if (AREAS_T == S57_getObjtype(geo)) {
        GString    *seabed01  = NULL;
        GString    *drval1str = S57_getAttVal(geo, "DRVAL1");
        double      drval1    = (NULL == drval1str)? -UNKNOWN : atof(drval1str->str);
        GString    *drval2str = S57_getAttVal(geo, "DRVAL2");
        double      drval2    = (NULL == drval2str)? -UNKNOWN : atof(drval2str->str);
        // NOTE: change sign of infinity (minus) to get out of bound in seabed01


        PRINTF("***********drval1=%f drval2=%f \n", drval1, drval2);
        seabed01 = _SEABED01(drval1, drval2);
        slcons03 = g_string_new(seabed01->str);
        g_string_free(seabed01, TRUE);

    }
    */

    if (NULL != cmdw)
        slcons03.Append(cmdw);

    slcons03.Append('\037');

    char *r = (char *)malloc(slcons03.Len() + 1);
    strcpy(r, slcons03.c_str());


    return r;


}

static void *RESARE02(void *param)
// Remarks: A list-type attribute is used because an area of the object class RESARE may
// have more than one category (CATREA). For example an inshore traffic
// zone might also have fishing and anchoring prohibition and a prohibited
// area might also be a bird sanctuary or a mine field.
//
// This conditional procedure is set up to ensure that the categories of most
// importance to safe navigation are prominently symbolized, and to pass on
// all given information with minimum clutter. Only the most significant
// restriction is symbolized, and an indication of further limitations is given by
// a subscript "!" or "I". Further details are given under conditional
// symbology procedure RESTRN01
//
// Other object classes affected by attribute RESTRN are handled by
// conditional symbology procedure RESTRN01.
{
    ObjRazRules *rzRules = (ObjRazRules *)param;
    S57Obj *obj = rzRules->obj;


    wxString resare02;

    wxString *restrnstr = GetStringAttrWXS(obj, "RESTRN");
//    GString *restrnstr        = S57_getAttVal(geo, "RESTRN");


    char     restrn[LISTSIZE] = {'\0'};
//    GString *catreastr        = S57_getAttVal(geo, "CATREA");
    wxString *catreastr = GetStringAttrWXS(obj, "CATREA");

    char     catrea[LISTSIZE] = {'\0'};
    char    *symb             = NULL;
    char    *line             = NULL;
    char    *prio             = NULL;

    if ( NULL != restrnstr) {
        _parseList(restrnstr->c_str(), restrn);

        if (NULL != catreastr) _parseList(catreastr->c_str(), catrea);

        if (strpbrk(restrn, "\007\010\016")) {                          // entry restrictions
            // Continuation A
            if (strpbrk(restrn, "\001\002\003\004\005\006"))            // anchoring, fishing, trawling
                symb = ";SY(ENTRES61)";
            else {
                if (NULL != catreastr && strpbrk(catrea, "\001\010\011\014\016\023\025\031"))
                        symb = ";SY(ENTRES61)";
                else {
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(ENTRES71)";
                    else {
                        if (NULL != catreastr && strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                            symb = ";SY(ENTRES71)";
                        else
                            symb = ";SY(ENTRES51)";
                    }
                }
            }

            if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
                line = ";LC(RESARE51)";
            else
                line = ";LS(DASH,2,CHMGD)";

            prio = ";OP(6---)";  // display prio set to 6

        } else {
            if (strpbrk(restrn, "\001\002")) {                          // anchoring
                // Continuation B
                if (strpbrk(restrn, "\003\004\005\006"))
                    symb = ";SY(ACHRES61)";
                else {
                    if (NULL != catreastr && strpbrk(catrea, "\001\010\011\014\016\023\025\031"))
                            symb = ";SY(ACHRES61)";
                    else {
                        if (strpbrk(restrn, "\011\012\013\014\015"))
                            symb = ";SY(ACHRES71)";
                        else {
                            if (NULL != catreastr && strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                                symb = ";SY(ACHRES71)";
                            else
                                symb = ";SY(RESTRN51)";
                        }
                    }
                }

                if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
                    line = ";LC(RESARE51)";                // could be ACHRES51 when _drawLC is implemented fully
                else
                    line = ";LS(DASH,2,CHMGD)";

                prio = ";OP(6---)";  // display prio set to 6

            } else {
                if (strpbrk(restrn, "\003\004\005\006")) {              // fishing/trawling
                    // Continuation C
                    if (NULL != catreastr && strpbrk(catrea, "\001\010\011\014\016\023\025\031"))
                            symb = ";SY(FSHRES51)";
                    else {
                        if (strpbrk(restrn, "\011\012\013\014\015"))
                            symb = ";SY(FSHRES71)";
                        else{
                            if (NULL != catreastr && strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                                symb = ";SY(FSHRES71)";
                            else
                                symb = ";SY(FSHRES51)";
                        }
                    }

                    if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
                        line = ";LC(FSHRES51)";
                    else
                        line = ";LS(DASH,2,CHMGD)";

                    prio = ";OP(6---)";  // display prio set to 6

                } else {
                    if (strpbrk(restrn, "\011\012\013\014\015"))        // diving, dredging, waking...
                        symb = ";SY(INFARE51)";
                    else
                        symb = ";SY(RSRDEF51)";

                    if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
                        line = ";LC(CTYARE51)";
                    else
                        line = ";LS(DASH,2,CHMGD)";

                }
                //  Todo more for s57 3.1  Look at caris catalog ATTR::RESARE

            }
        }

    } else {
        // Continuation D
        if (NULL != catreastr) {
            if (strpbrk(catrea, "\001\010\011\014\016\023\025\031")) {
                if (strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                    symb = ";SY(CTYARE71)";
                else
                    symb = ";SY(CTYARE51)";
            } else {
                if (strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                    symb = ";SY(INFARE71)";
                else
                    symb = ";SY(RSRDEF51)";
            }
        } else
            symb = ";SY(RSRDEF51)";

        if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
            line = ";LC(CTYARE51)";
        else
            line = ";LS(DASH,2,CHMGD)";
    }

    // create command word
    if (NULL != prio)
        resare02.Append(prio);          //g_string_append(resare02, prio);
    resare02.Append(line);              //g_string_append(resare02, line);
    resare02.Append(symb);              //g_string_append(resare02, symb);

    resare02.Append('\037');

    char *r = (char *)malloc(resare02.Len() + 1);
    strcpy(r, resare02.c_str());

    delete restrnstr;
    delete catreastr;

    return r;
}








static void *_RESCSP01(void *param);
static void *RESTRN01 (void *param)
// Remarks: Objects subject to RESTRN01 are actually symbolised in sub-process
// RESCSP01, since the latter can also be accessed from other conditional
// symbology procedures. RESTRN01 merely acts as a "signpost" for
// RESCSP01.
//
// Object class RESARE is symbolised for the effect of attribute RESTRN in a separate
// conditional symbology procedure called RESARE02.
//
// Since many of the areas concerned cover shipping channels, the number of symbols used
// is minimised to reduce clutter. To do this, values of RESTRN are ranked for significance
// as follows:
// "Traffic Restriction" values of RESTRN:
// (1) RESTRN 7,8: entry prohibited or restricted
//     RESTRN 14: IMO designated "area to be avoided" part of a TSS
// (2) RESTRN 1,2: anchoring prohibited or restricted
// (3) RESTRN 3,4,5,6: fishing or trawling prohibited or restricted
// (4) "Other Restriction" values of RESTRN are:
//     RESTRN 9, 10: dredging prohibited or restricted,
//     RESTRN 11,12: diving prohibited or restricted,
//     RESTRN 13   : no wake area.
{
    ObjRazRules *rzRules = (ObjRazRules *)param;
    S57Obj *obj = rzRules->obj;

    wxString *restrnstr = GetStringAttrWXS(obj, "RESTRN");

//    GString *restrn01str = S57_getAttVal(geo, "RESTRN");
    char *restrn01    = NULL;

    if (NULL != restrnstr)
        restrn01 = (char *)_RESCSP01(param);
    else
        restrn01 = NULL;

    delete restrnstr;
    return restrn01;
}

static void *_RESCSP01(void *param)
// Remarks: See procedure RESTRN01
{
    ObjRazRules *rzRules = (ObjRazRules *)param;
    S57Obj *obj = rzRules->obj;

    wxString rescsp01;
//    char *rescsp01         = NULL;
    wxString *restrnstr = GetStringAttrWXS(obj, "RESTRN");
//    GString *restrnstr        = S57_getAttVal(geo, "RESTRN");
    char     restrn[LISTSIZE] = {'\0'};   // restriction list
    char    *symb             = NULL;
    char    *r;

    if ( restrnstr->Len()) {
        _parseList(restrnstr->c_str(), restrn);

        if (strpbrk(restrn, "\007\010\016")) {
            // continuation A
            if (strpbrk(restrn, "\001\002\003\004\005\006"))
                symb = ";SY(ENTRES61)";
            else {
                if (strpbrk(restrn, "\011\012\013\014\015"))
                    symb = ";SY(ENTRES71)";
                else
                    symb = ";SY(ENTRES51)";

            }
        } else {
            if (strpbrk(restrn, "\001\002")) {
                // continuation B
                if (strpbrk(restrn, "\003\004\005\006"))
                    symb = ";SY(ACHRES61)";
                else {
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(ACHRES71)";
                    else
                        symb = ";SY(ACHRES51)";
                }


            } else {
                if (strpbrk(restrn, "\003\004\005\006")) {
                    // continuation C
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(FSHRES71)";
                    else
                        symb = ";SY(FSHRES51)";


                } else {
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(INFARE51)";
                    else
                        symb = ";SY(RSRDEF51)";

                }
            }
        }

        rescsp01.Append(symb);
        rescsp01.Append('\037');

        r = (char *)malloc(rescsp01.Len() + 1);
        strcpy(r, rescsp01.c_str());

        delete restrnstr;
    }

    return r;
}

static void *SEABED01(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        CPLError((CPLErr)0, 0,"s52csny : SEABED01 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *SNDFRM02(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        CPLError((CPLErr)0, 0,"s52csny : SNDFRM02 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *SOUNDG02(void *param)
{
//      S57Obj *obj = rzRules->obj;
//      ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

/*
      //    Add another rule onto the ruleList
      Rules *r = NULL;

      r = (Rules*)calloc(1, sizeof(Rules));
      r->ruleType = RUL_MUL_SG;
*/
//      r->INSTstr = "SY(LIGHTS03)";
//      r->razRule = (*ps52plib->_symb_sym)["QUESMRK1"];

//      CPLError((CPLErr)0, 0,"s52csny : SOUNDG02 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
      return NULL; //r;
}


static void *TOPMAR01 (void *param)
// Remarks: Topmark objects are to be symbolized through consideration of their
// platforms e.g. a buoy. Therefore this conditional symbology procedure
// searches for platforms by looking for other objects that are located at the
// same position.. Based on the finding whether the platform is rigid or
// floating, the respective upright or sloping symbol is selected and presented
// at the objects location. Buoy symbols and topmark symbols have been
// carefully designed to fit to each other when combined at the same position.
// The result is a composed symbol that looks like the traditional symbols the
// mariner is used to.
{
    ObjRazRules *rzRules = (ObjRazRules *)param;
    S57Obj *obj = rzRules->obj;

    int top_int = 0;
    bool battr = GetIntAttr(obj, "TOPSHP", top_int);

    char    *sy        = NULL;

    if (!battr)
        sy = ";SY(QUESMRK1)";
    else {
        int floating    = FALSE; // not a floating platform
        int topshp      = (!battr) ? 0 : top_int;


        if (TRUE == _atPtPos(obj, rzRules->chart->pFloatingATONArray, false))
            floating = TRUE;
        else
            // FIXME: this test is wierd since it doesn't affect 'floating'
            if (TRUE == _atPtPos(obj, rzRules->chart->pRigidATONArray, false))
                floating = FALSE;


        if (floating) {
            // floating platform
            switch (topshp) {
                case 1 : sy = ";SY(TOPMAR02)"; break;
                case 2 : sy = ";SY(TOPMAR04)"; break;
                case 3 : sy = ";SY(TOPMAR10)"; break;
                case 4 : sy = ";SY(TOPMAR12)"; break;

                case 5 : sy = ";SY(TOPMAR13)"; break;
                case 6 : sy = ";SY(TOPMAR14)"; break;
                case 7 : sy = ";SY(TOPMAR65)"; break;
                case 8 : sy = ";SY(TOPMAR17)"; break;

                case 9 : sy = ";SY(TOPMAR16)"; break;
                case 10: sy = ";SY(TOPMAR08)"; break;
                case 11: sy = ";SY(TOPMAR07)"; break;
                case 12: sy = ";SY(TOPMAR14)"; break;

                case 13: sy = ";SY(TOPMAR05)"; break;
                case 14: sy = ";SY(TOPMAR06)"; break;
                case 17: sy = ";SY(TMARDEF2)"; break;
                case 18: sy = ";SY(TOPMAR10)"; break;

                case 19: sy = ";SY(TOPMAR13)"; break;
                case 20: sy = ";SY(TOPMAR14)"; break;
                case 21: sy = ";SY(TOPMAR13)"; break;
                case 22: sy = ";SY(TOPMAR14)"; break;

                case 23: sy = ";SY(TOPMAR14)"; break;
                case 24: sy = ";SY(TOPMAR02)"; break;
                case 25: sy = ";SY(TOPMAR04)"; break;
                case 26: sy = ";SY(TOPMAR10)"; break;

                case 27: sy = ";SY(TOPMAR17)"; break;
                case 28: sy = ";SY(TOPMAR18)"; break;
                case 29: sy = ";SY(TOPMAR02)"; break;
                case 30: sy = ";SY(TOPMAR17)"; break;

                case 31: sy = ";SY(TOPMAR14)"; break;
                case 32: sy = ";SY(TOPMAR10)"; break;
                case 33: sy = ";SY(TMARDEF2)"; break;
                default: sy = ";SY(TMARDEF2)"; break;
            }
        } else {
            // not a floating platform
            switch (topshp) {
                case 1 : sy = ";SY(TOPMAR22)"; break;
                case 2 : sy = ";SY(TOPMAR24)"; break;
                case 3 : sy = ";SY(TOPMAR30)"; break;
                case 4 : sy = ";SY(TOPMAR32)"; break;

                case 5 : sy = ";SY(TOPMAR33)"; break;
                case 6 : sy = ";SY(TOPMAR34)"; break;
                case 7 : sy = ";SY(TOPMAR85)"; break;
                case 8 : sy = ";SY(TOPMAR86)"; break;

                case 9 : sy = ";SY(TOPMAR36)"; break;
                case 10: sy = ";SY(TOPMAR28)"; break;
                case 11: sy = ";SY(TOPMAR27)"; break;
                case 12: sy = ";SY(TOPMAR14)"; break;

                case 13: sy = ";SY(TOPMAR25)"; break;
                case 14: sy = ";SY(TOPMAR26)"; break;
                case 15: sy = ";SY(TOPMAR88)"; break;
                case 16: sy = ";SY(TOPMAR87)"; break;

                case 17: sy = ";SY(TMARDEF1)"; break;
                case 18: sy = ";SY(TOPMAR30)"; break;
                case 19: sy = ";SY(TOPMAR33)"; break;
                case 20: sy = ";SY(TOPMAR34)"; break;

                case 21: sy = ";SY(TOPMAR33)"; break;
                case 22: sy = ";SY(TOPMAR34)"; break;
                case 23: sy = ";SY(TOPMAR34)"; break;
                case 24: sy = ";SY(TOPMAR22)"; break;

                case 25: sy = ";SY(TOPMAR24)"; break;
                case 26: sy = ";SY(TOPMAR30)"; break;
                case 27: sy = ";SY(TOPMAR86)"; break;
                case 28: sy = ";SY(TOPMAR89)"; break;

                case 29: sy = ";SY(TOPMAR22)"; break;
                case 30: sy = ";SY(TOPMAR86)"; break;
                case 31: sy = ";SY(TOPMAR14)"; break;
                case 32: sy = ";SY(TOPMAR30)"; break;
                case 33: sy = ";SY(TMARDEF1)"; break;
                default: sy = ";SY(TMARDEF1)"; break;
            }
        }

    }

    wxString topmar;
    topmar.Append(sy);
    topmar.Append('\037');

    char *r = (char *)malloc(topmar.Len() + 1);
    strcpy(r, topmar.c_str());

    return r;
}




















static void *UDWHAZ03(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        CPLError((CPLErr)0, 0,"s52csny : UDWHAZ03 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *VESSEL01(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        CPLError((CPLErr)0, 0,"s52csny : VESSEL01 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *VRMEBL01(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        CPLError((CPLErr)0, 0,"s52csny : VRMEBL01 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
   return NULL;
}

static void *WRECKS02(void *param)
{
        ObjRazRules *rzRules = (ObjRazRules *)param;
//      S57Obj *obj = rzRules->obj;

        static int f07;
        if(!f07)
                CPLError((CPLErr)0, 0,"s52csny : WRECKS02 ERROR no conditional symbology for: %s\n",rzRules->LUP->OBCL);
        f07++;
   return NULL;
}


//--------------------------------
//
// JUMP TABLE SECTION
//
//--------------------------------
Cond condTable[] = {
   {"CLRLIN01",CLRLIN01},
   {"DATCVR01",DATCVR01},
   {"DATCVR01",DATCVR01},
   {"DEPARE01",DEPARE01},
   {"DEPCNT02",DEPCNT02},
   {"DEPVAL01",DEPVAL01},
   {"LEGLIN02",LEGLIN02},
   {"LIGHTS04",LIGHTS04},
   {"LITDSN01",LITDSN01},
   {"OBSTRN04",OBSTRN04},
   {"OWNSHP02",OWNSHP02},
   {"PASTRK01",PASTRK01},
   {"QUAPOS01",QUAPOS01},
   {"QUALIN01",QUALIN01},
   {"QUAPNT01",QUAPNT01},
   {"SLCONS03",SLCONS03},
   {"RESARE02",RESARE02},
   {"RESTRN01",RESTRN01},
//   {"RESCSP01",RESCSP01},
   {"SEABED01",SEABED01},
   {"SNDFRM02",SNDFRM02},
   {"SOUNDG02",SOUNDG02},
   {"TOPMAR01",TOPMAR01},
   {"UDWHAZ03",UDWHAZ03},
   {"VESSEL01",VESSEL01},
   {"VRMEBL01",VRMEBL01},
   {"WRECKS02",WRECKS02},
   {"########",NULL}
};



#if 0
// S52CS.c : Conditional Symbologie procedure 3.2 (CS)
//
// Project:  OpENCview

/*
    This file is part of the OpENCview project, a viewer of ENC
    Copyright (C) 2000-2004  Sylvain Duclos sduclos@users.sourceforgue.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// NOTE: remarks commenting each CS are extracted from pslb03_2.pdf (sec. 12)

// FIXME: DEPCNT02: call DB for area DEPARE & DRGARE that intersect this line
// FIXME:_DEPVAL01: call DB for area DEPARE & UNSARE that intersect this area
// FIXME:_UDWHAZ03: call DB for area DRGARE & DEPARE that intersect this point/area

#include "S52CS.h"

#include "S52utils.h"   // PRINTF()

#include <stdlib.h>     // atof()
#include <math.h>       // fabsf(), HUGE_VAL
#include <ctype.h>      // isdigit()

#define UNKNOWN HUGE_VAL   // INFINITY/NAN

#define COALNE   30   // Coastline
#define DEPARE   42   // Depth area
#define DEPCNT   43   // Depth contour
#define DRGARE   46   // Dredged area
#define UWTROC  153   // Underwater rock / awash rock
#define WRECKS  159   // Wreck

// point list name
#define LIGHTLIST 0
#define SECTRLIST 1
#define FLOATLIST 2   // floating platform
#define RIGIDLIST 3   // rigid platform
//static GPtrArray   *_lightList = NULL;
//static GPtrArray   *_sectrList = NULL;
//static GPtrArray   *_flaotList = NULL;
static GPtrArray *_ptList[] = {NULL, NULL, NULL, NULL};

int S52_state = 1;

// size of attributes value list buffer
#define LISTSIZE   16   // list size

#define version "3.2.0"
char     *S52_CS_version()
{
    return version;
}

int       S52_CS_init()
{
    _ptList[LIGHTLIST] = g_ptr_array_new();
    _ptList[SECTRLIST] = g_ptr_array_new();
    _ptList[FLOATLIST] = g_ptr_array_new();
    _ptList[RIGIDLIST] = g_ptr_array_new();

    return 1;
}

int       S52_CS_done()
{
    g_ptr_array_free(_ptList[LIGHTLIST], TRUE);
    g_ptr_array_free(_ptList[SECTRLIST], TRUE);
    g_ptr_array_free(_ptList[FLOATLIST], TRUE);
    g_ptr_array_free(_ptList[RIGIDLIST], TRUE);

    return 1;
}

int       S52_CS_setPtPos(S57_geo *geoData, char *name)
{
    printf("name = %s\n", name);

    if (POINT_T == S57_getObjtype(geoData)) {

        // set floating platform
        if ((0==strncmp(name, "LITFLT", 6)) ||
            (0==strncmp(name, "LITVES", 6)) ||
            (0==strncmp(name, "BOY",    3)))
            g_ptr_array_add(_ptList[FLOATLIST], (gpointer) geoData);

        // set rigid platform
        if (0==strncmp(name, "BCN",    3))
            g_ptr_array_add(_ptList[RIGIDLIST], (gpointer) geoData);
    }

    return 1;
}

static int      _atPtPos(S57_geo *geoNew, int listNm)
// return TRUE if there is a light at this position
// or if its an extended arc radius else FALSE
{
    int i;
    GPtrArray *curntList = _ptList[listNm];

    for (i=0; i<curntList->len; i++) {
        S57_geo *geoOld = g_ptr_array_index(curntList, i);

        if (S57_samePtPos(geoNew, geoOld)) {

            if (SECTRLIST != listNm)
                return TRUE;
            else {
                // check for extend arc radius
                GString *Asectr1str = S57_getAttVal(geoOld, "SECTR1");
                GString *Asectr2str = S57_getAttVal(geoOld, "SECTR2");
                GString *Bsectr1str = S57_getAttVal(geoNew, "SECTR1");
                GString *Bsectr2str = S57_getAttVal(geoNew, "SECTR2");

                // check  present
                if (NULL == Asectr1str ||
                    NULL == Asectr1str ||
                    NULL == Asectr1str ||
                    NULL == Asectr1str)
                    return FALSE;

                {
                    double Asectr1 = atof(Asectr1str->str);
                    double Asectr2 = atof(Asectr2str->str);
                    double Bsectr1 = atof(Bsectr1str->str);
                    double Bsectr2 = atof(Bsectr2str->str);
                    double Asweep = (Asectr1 > Asectr2) ?
                        Asectr2-Asectr1+360 : Asectr2-Asectr1;
                    double Bsweep = (Bsectr1 > Bsectr2) ?
                        Bsectr2-Bsectr1+360 : Bsectr2-Bsectr1;

                    // check sector overlap
                    if (Asectr2<=Bsectr1 || Asectr1>=Bsectr2) {
                        if (Asweep == Bsweep) {
                            g_string_truncate(Bsectr2str, 0);
                            g_string_sprintf(Bsectr2str, "%f",Bsectr2-1);
                            S57_setAtt(geoNew, "SECTR2", Bsectr2str->str);
                        }

                        return FALSE;
                    }

                    // check if other sector larger
                    if (Asweep >= Bsweep)
                        return TRUE;
                }
            }
        }
    }

    return FALSE;
}

static int      _setPtPos(S57_geo *geo, int listNm)
// TRUE if set new position of a light
// else FALSE (ie there is a light at this position)
{
    GPtrArray *curntList = _ptList[listNm];

    if (_atPtPos(geo, listNm))
        return 1;
    else
        g_ptr_array_add(curntList, (gpointer) geo);

    return 0;
}

static int      _parseList(const char *str, char *buf)
// Put a string of comma delimited number in an array (buf).
// Return: the number of value in buf.
// Assume: - number < 256,
//         - list size less then LISTSIZE-1 .
// Note: buf is \0 terminated for strpbrk().
{
    int i = 0;

    if (NULL != str && *str != '\0') {
        do {
            if ( i>= LISTSIZE-1) {
                PRINTF("OVERFLOW --value in list lost!!\n");
                break;
            }

            /*
            if (255 <  (unsigned char) atoi(str)) {
                PRINTF("value overflow (>255)\n");
                exit(0);
            }
            */

            buf[i++] = (unsigned char) atoi(str);

            while(isdigit(*str++));   // next
            //while( g_ascii_isdigit(c));   // next

        } while(*str++ != '\0');      // skip ',' or exit
    }

    buf[i] = '\0';

    return i;
}

static char    *_selSYcol(char *buf)
{
    // FIXME: C1 3.1 use LIGHTS0x          and specs 3.2 use LIGHTS1x

    char *sym = ";SY(LIGHTDEF";            //sym = ";SY(LITDEF11";

    // max 1 color
    if ('\0' == buf[1]) {
        if (strpbrk(buf, "\003"))
            sym = ";SY(LIGHTS01";          //sym = ";SY(LIGHTS11";
        else if (strpbrk(buf, "\004"))
            sym = ";SY(LIGHTS02";          //sym = ";SY(LIGHTS12";
        else if (strpbrk(buf, "\001\006\013"))
            sym = ";SY(LIGHTS03";          //sym = ";SY(LIGHTS13";
    } else {
        // max 2 color
        if ('\0' == buf[2]) {
            if (strpbrk(buf, "\001") && strpbrk(buf, "\003"))
                sym = ";SY(LIGHTS01";          //sym = ";SY(LIGHTS11";
            else if (strpbrk(buf, "\001") && strpbrk(buf, "\004"))
                sym = ";SY(LIGHTS02";          //sym = ";SY(LIGHTS12";
        }
    }

    return sym;
}

static GString *CLRLIN01 (S57_geo *geo)
// Remarks: A clearing line shows a single arrow head at one of its ends. The direction
// of the clearing line must be calculated from its line object in order to rotate
// the arrow head symbol and place it at the correct end. This cannot be
// achieved with a complex linestyle since linestyle symbols cannot be sized
// to the length of the clearing line. Instead a linestyle with a repeating pattern
// of arrow symbols had to be used which does not comply with the required
// symbolization.
{

    PRINTF("Mariner's object not drawn\n");

    return NULL;
}

static GString *DATCVR01 (S57_geo *geo)
// Remarks: This conditional symbology procedure describes procedures for:
// - symbolizing the limit of ENC coverage;
// - symbolizing navigational purpose boundaries ("scale boundarie"); and
// - indicating overscale display.
//
// Note that the mandatory meta object CATQUA is symbolized by the look-up table.
//
// Because the methods adopted by an ECDIS to meet the IMO and IHO requirements
// listed on the next page will depend on the manufacturer's software, and cannot be
// described in terms of a flow chart in the same way as other conditional procedures,
// this procedure is in the form of written notes.
{
    GString *datcvr01 = NULL;

    ///////////////////////
    // 1- REQUIREMENT
    // (IMO/IHO specs. explenation)

    ///////////////////////
    // 2- ENC COVERAGE
    //
    // 2.1- Limit of ENC coverage
    //datcvr01 = g_string_new(";OP(3OD11060);LC(HODATA01)");
    // FIXME: get cell extend

    // 2.2- No data areas
    // This can be done outside of CS (ie when clearing the screen in Mesa)
    // FIXME: ";OP(0---);AC(NODATA)"
    // FIXME: set geo to cover earth (!)

    //////////////////////
    // 3- SCALE BOUNDARIES
    //
    // 3.1- Chart scale boundaties
    // FIXME;
    //g_string_append(datcvr01, ";LS(SOLD,1,CHGRD)");
    // -OR- LC(SCLBDYnn) (?)
    //
    // ;OP(3OS21030)

    // 3.2- Graphical index of navigational purpose
    // FIXME: draw extent of available SENC in DB

    //////////////////////
    // 4- OVERSCALE
    //
    // FIXME: get meta date CSCL of DSPM field
    // FIXME: get object M_CSCL or CSCALE
    //
    // 4.1- Overscale indication
    // FIXME: compute, scale = [denominator of the compilation scale] /
    //                         [denominator of the display scale]
    // FIXME: draw overscale indication (ie TX("X%3.1f",scale))
    //
    // 4.2- Ovescale area at a chart scale boundary
    // FIXME: test if next chart is over scale (ie going from large scale chart
    //        to a small scale chart)
    // FIXME: draw AP(OVERSC01) on overscale part of display
    //g_string(";OP(3OS21030)");

    //
    // 4.3- Larger scale data available
    // FIXME: display indication of better scale available (?)


    PRINTF("not computed\n");

    return datcvr01;
}

static GString *_SEABED01(double drval1, double drval2);
static GString *_RESCSP01(S57_geo *geo);
static GString *DEPARE01 (S57_geo *geo)
// Remarks: An object of the class "depth area" is coloured and covered with fill patterns
// according to the mariners selections of shallow contour, safety contour and
// deep contour. This requires a decision making process provided by the sub-procedure
// "SEABED01" which is called by this symbology procedure.
// Objects of the class "dredged area" are handled by this routine as well to
// ensure a consistent symbolization of areas that represent the surface of the
// seabed.
{
    GString *depare01  = NULL;
    int      objl      = 0;
    GString *objlstr   = NULL;
    GString *drval1str = S57_getAttVal(geo, "DRVAL1");
    double   drval1    = UNKNOWN;
    GString *drval2str = S57_getAttVal(geo, "DRVAL2");
    double   drval2    = UNKNOWN;

    if (NULL == drval1str || NULL == drval2str) {
        PRINTF("ERROR: drval1 or drval2 should have the value UNKNOWN\n");
        return NULL;
    }

    drval1 = (NULL == drval1str) ? -1.0 : atof(drval1str->str);
    drval2 = (NULL == drval2str) ? drval1+0.01 : atof(drval2str->str);

    depare01 = _SEABED01(drval1, drval2);

    objlstr = S57_getAttVal(geo, "OBJL");
    objl    = (NULL == objlstr) ? 0 : atoi(objlstr->str);

    if (DRGARE == objl) {
        g_string_append(depare01, ";AP(DRGARE01)");
        g_string_append(depare01, ";LS(DASH,1,CHGRF)");

        if (NULL != S57_getAttVal(geo, "RESTRN")) {
            GString *rescsp01 = _RESCSP01(geo);
            if (NULL != rescsp01) {
                g_string_append(depare01, rescsp01->str);
                g_string_free(rescsp01, TRUE);
            }
        }

    }

    return depare01;
}

static GString *_SNDFRM02(S57_geo *geo, double depth_value);
static GString *DEPCNT02 (S57_geo *geo)
// Remarks: An object of the class "depth contour" or "line depth area" is highlighted and must
// be shown under all circumstances if it matches the safety contour depth value
// entered by the mariner (see IMO PS 3.6). But, while the mariner is free to enter any
// safety contour depth value that he thinks is suitable for the safety of his ship, the
// SENC only contains a limited choice of depth contours. This symbology procedure
// determines whether a contour matches the selected safety contour. If the selected
// safety contour does not exist in the data, the procedure will default to the next deeper
// contour. The contour selected is highlighted as the safety contour and put in
// DISPLAYBASE. The procedure also identifies any line segment of the spatial
// component of the object that has a "QUAPOS" value indicating unreliable
// positioning, and symbolizes it with a double dashed line.
//
// Note: Depth contours are not normally labeled. The ECDIS may provide labels, on demand
// only as with other text, or provide the depth value on cursor picking
{
    GString *depcnt02  = NULL;
    int      safe      = FALSE;     // initialy not a safety contour
    GString *objlstr   = NULL;
    int      objl      = 0;
    GString *quaposstr = NULL;
    int      quapos    = 0;
    double   depth_value;

    objlstr = S57_getAttVal(geo, "OBJL");
    objl    = (NULL == objlstr) ? 0 : atoi(objlstr->str);

    if (DEPARE==objl && LINES_T==S57_getObjtype(geo)) {
        GString *drval1str = S57_getAttVal(geo, "DRVAL1");
        double   drval1    = (NULL == drval1str) ? 0.0    : atof(drval1str->str);
        GString *drval2str = S57_getAttVal(geo, "DRVAL2");
        double   drval2    = (NULL == drval2str) ? drval1 : atof(drval2str->str);

        if (drval1 <= S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)) {
            if (drval2 >= S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR))
                safe = TRUE;
        } else {
            if (1 == S52_state)
                return NULL;
            else {
                S57_geo *geoTmp = geo;

                // get area DEPARE & DRGARE that intersect this line
                while (NULL != (geoTmp = S57_nextObj(geoTmp))) {
                    drval1str = S57_getAttVal(geoTmp, "DRVAL1");
                    drval1    = (NULL == drval1str) ? 0.0 : atof(drval1str->str);

                    if (NULL == drval1str) {
                        safe = TRUE;
                        break;
                    }

                    if (drval1 < S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)) {
                        safe = TRUE;
                        break;
                    }
                }
                // debug trace
                //if (safe) PRINTF("** DEPARE: SAFE FOUND**\n");
            }
        }

        depth_value = drval1;

    } else {
        // continuation A (DEPCNT)
        GString *valdcostr = S57_getAttVal(geo, "VALDCO");
        double   valdco    = (NULL == valdcostr) ? 0.0 : atof(valdcostr->str);

        depth_value = valdco;

        if (valdco == S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR))
            safe = TRUE;   // this is useless !?!?
        else {
            if (valdco > S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)) {
                if (1 == S52_state)
                    return NULL;
                else {
                    S57_geo *geoTmp = geo;

                    // get area DEPARE & DRGARE that intersect this line
                    while (NULL != (geoTmp = S57_nextObj(geoTmp))){
                        GString *drval1str = S57_getAttVal(geoTmp, "DRVAL1");
                        double   drval1    = (NULL == drval1str) ? 0.0 : atof(drval1str->str);

                        if (NULL == drval1str) {
                            safe = TRUE;
                            break;
                        }

                        if (drval1 < S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)) {
                            safe = TRUE;
                            break;
                        }
                    }
                    // debug trace
                    //if (safe) PRINTF("** DEPCN: SAFE FOUND**\n");
                }
            }
        }
    }

    // Continuation B
    quaposstr = S57_getAttVal(geo, "QUAPOS");
    if (NULL != quaposstr) {
        quapos = atoi(quaposstr->str);
        if ( 2 <= quapos && quapos < 10) {
            if (safe)
                depcnt02 = g_string_new(";LS(DASH,2,DEPSC)");
            else
                depcnt02 = g_string_new(";LS(DASH,1,DEPCN)");
        }
    } else {
        if (safe)
            depcnt02 = g_string_new(";LS(SOLD,2,DEPSC)");
        else
            depcnt02 = g_string_new(";LS(SOLD,1,DEPCN)");
    }

    if (safe) {
        S57_setAtt(geo, "SCAMIN", "INFINITE");
        depcnt02 = g_string_prepend(depcnt02, ";OP(8OD13010)");
    } else
        depcnt02 = g_string_prepend(depcnt02, ";OP(---33020)");

    // facultative in S-52
    //if (TRUE == S52_getMarinerParam(S52_MAR_SHOW_TEXT)) {
    //    GString *sndfrm02 = _SNDFRM02(geo, depth_value);
    //    depcnt02 = g_string_append(depcnt02, sndfrm02->str);
    //    g_string_free(sndfrm02, TRUE);
    //}

    // debug
    //PRINTF("depth= %f\n", depth_value);

    S57_unlinkObj(geo);

    return depcnt02;
}

static double   _DEPVAL01(S57_geo *geo, double least_depth)
// Remarks: S-57 Appendix B1 Annex A requires in Section 6 that areas of rocks be
// encoded as area obstruction, and that area OBSTRNs and area WRECKS
// be covered by either group 1 object DEPARE or group 1 object UNSARE.
// If the value of the attribute VALSOU for an area OBSTRN or WRECKS
// is missing, the DRVAL1 of an underlying DEPARE is the preferred default
// for establishing a depth vale. This procedure either finds the shallowest
// DRVAL1 of the one or more underlying DEPAREs, or returns an
// "unknown"" depth value to the main procedure for the next default
// procedure.

// NOTE: UNSARE test is useless since least_depth is already UNKNOWN
{
    least_depth = UNKNOWN;


    S57_geo *geoTmp = geo;

    // NOTE: the geo list is unchange (_UDWHAZ03 will unlink geo)
    while (NULL != (geoTmp = S57_nextObj(geoTmp))) {
        GString *objlstr = S57_getAttVal(geoTmp, "OBJL");
        int      objl    = (NULL == objlstr) ? 0 : atoi(objlstr->str);

        // get area DEPARE  that intersect this area
        if (DEPARE==objl && LINES_T==S57_getObjtype(geo)) {
            GString *drval1str = S57_getAttVal(geoTmp, "DRVAL1");
            double   drval1    = (NULL == drval1str) ? 9.0 : atof(drval1str->str);

            if (NULL != drval1str) {
                if (UNKNOWN==least_depth || least_depth<drval1)
                    least_depth = drval1;
            }

        }
    }

    return least_depth;
}

static GString *LEGLIN02 (S57_geo *geo)

// Remarks: The course of a leg is given by its start and end point. Therefore this
// conditional symbology procedure calculates the course and shows it
// alongside the leg. It also places the "distance to run" labels and cares for the
// different presentation of planned & alternate legs.
{
    PRINTF("Mariner's object not drawn\n");
    return NULL;
}

static GString *_LITDSN01(S57_geo *geo);
static GString *LIGHTS05 (S57_geo *geo)
// Remarks: A light is one of the most complex S-57 objects. Its presentation depends on
// whether it is a light on a floating or fixed platform, its range, it's colour and
// so on. This conditional symbology procedure derives the correct
// presentation from these parameters and also generates an area that shows the
// coverage of the light.
//
// Notes on light sectors:
// 1.) The radial leg-lines defining the light sectors are normally drawn to only 25mm
// from the light to avoid clutter (see Part C). However, the mariner should be able to
// select "full light-sector lines" and have the leg-lines extended to the nominal range
// of the light (VALMAR).
//
// 2.) Part C of this procedure symbolizes the sectors at the light itself. In addition,
// it should be possible, upon request, for the mariner to be capable of identifying
// the colour and sector limit lines of the sectors affecting the ship even if the light
// itself is off the display.
// [ed. last sentence in bold]

// NOTE: why is this relationship not already encoded in S57 (ei. C_AGGR or C_STAC) ?

{
    GString *lights05          = NULL;
    GString *valnmrstr         = S57_getAttVal(geo, "VALNMR");
    double   valnmr            = 0.0;
    GString *catlitstr         = S57_getAttVal(geo, "CATLIT");
    char     catlit[LISTSIZE]  = {'\0'};
    int      flare_at_45       = FALSE;
    int      extend_arc_radius = TRUE;
    GString *sectr1str         = NULL;
    GString *sectr2str         = NULL;
    double   sectr1            = 0.0;
    double   sectr2            = 0.0;
    GString *colourstr         = NULL;
    char     colist[LISTSIZE]  = {'\0'};   // colour list
    GString *orientstr         = NULL;
    double   sweep             = 0.0;


    lights05 = g_string_new("");

    valnmr = (NULL == valnmrstr) ? 9.0 : atof(valnmrstr->str);

    if ( NULL != catlitstr) {
        _parseList(catlitstr->str, catlit);

        // FIXME: OR vs AND/OR
        if (strpbrk(catlit, "\010\013")) {
            g_string_append(lights05, ";SY(LIGHTS82)");
            return lights05;
        }

        if (strpbrk(catlit, "\011")) {
            g_string_append(lights05, ";SY(LIGHTS81)");
            return lights05;
        }

        if (strpbrk(catlit, "\001\020")) {
            orientstr = S57_getAttVal(geo, "ORIENT");
            if (NULL != orientstr) {
                // FIXME: create a geo object (!?) LINE of lenght VALNMR
                // using ORIENT (from seaward) & POINT_T position
                g_string_append(lights05, ";LS(DASH,1,CHBLK)");
            }
        }
    }

    // Continuation A
    colourstr = S57_getAttVal(geo, "COLOUR");
    if (NULL != colourstr)
        _parseList(colourstr->str, colist);
    else {
        colist[0] = '\014';  // maganta (12)
        colist[1] = '\000';
    }

    sectr1str = S57_getAttVal(geo, "SECTR1");
    sectr1    = (NULL == sectr1str) ? 0.0 : atof(sectr1str->str);
    sectr2str = S57_getAttVal(geo, "SECTR2");
    sectr2    = (NULL == sectr2str) ? 0.0 : atof(sectr2str->str);

    if (NULL==sectr1str || NULL==sectr2str) {
        // not a sector light
        char *sym;

        if (1==S52_state) {
            _setPtPos(geo, LIGHTLIST);
            g_string_free(lights05, TRUE);
            return NULL;
        } else
            flare_at_45 = _atPtPos(geo, LIGHTLIST);

        sym = _selSYcol(colist);

        if (strpbrk(catlit, "\001\020")) {
            if (NULL != orientstr){
                g_string_append(lights05, sym);
                g_string_sprintfa(lights05, ",%s)", orientstr->str);
                g_string_append(lights05, ";TE('%03.0lf deg','ORIENT',3,3,3,'15110',3,1,CHBLK,23)" );
            } else
                g_string_append(lights05, ";SY(QUSMRK1)");
        } else {
            g_string_append(lights05, sym);
            if (flare_at_45)
                g_string_append(lights05, ",145)");
            else
                g_string_append(lights05, ",135)");
        }

        if (TRUE == S52_getMarinerParam(S52_MAR_SHOW_TEXT)) {
            GString *litdsn01 = _LITDSN01(geo);
            if (NULL != litdsn01){
                g_string_append(lights05, ";TX('");
                g_string_append(lights05, litdsn01->str);
                g_string_free(litdsn01, TRUE);

                if (flare_at_45)
                    g_string_append(lights05, "',3,3,3,'15110',2,-1,CHBLK,23)" );
                else
                    g_string_append(lights05, "',3,2,3,'15110',2,0,CHBLK,23)" );
            }
        }

        return lights05;
    }

    // Continuation B --sector light
    if (NULL == sectr1str) {
        sectr1 = 0.0;
        sectr2 = 0.0;
    } else
        sweep = (sectr1 > sectr2) ? sectr2-sectr1+360 : sectr2-sectr1;


    if (sweep<1.0 || sweep==360.0) {
        // handle all round light
        char *sym = _selSYcol(colist);;

        g_string_append(lights05, sym);
        g_string_append(lights05, ",135)");

        if (TRUE == S52_getMarinerParam(S52_MAR_SHOW_TEXT)) {
            GString *litdsn01 = _LITDSN01(geo);
            if (NULL != litdsn01) {
                g_string_append(lights05, ";TX('");
                g_string_append(lights05, litdsn01->str);
                g_string_append(lights05, "',3,2,3,'15110',2,0,CHBLK,23)" );
                g_string_free(litdsn01, TRUE);
            }

        }

        return lights05;
    }

    // scan for other lights with sector overlap at this position
    // compute light sector radius acording to other sector
    if (1 == S52_state) {
        _setPtPos(geo, SECTRLIST);
        g_string_free(lights05, TRUE);
        return NULL;
    } else {
        extend_arc_radius = _atPtPos(geo, SECTRLIST);

        // passe value via attribs to _renderAC
        if (extend_arc_radius)
            // FIXME: draw radius 25 mm
            S57_setAtt(geo, "extend_arc_radius", "Y");
        else
            // FIXME: draw radius 20 mm
            S57_setAtt(geo, "extend_arc_radius", "N");
    }

    // setup sector
    {
        char litvis[LISTSIZE] = {'\0'};  // list visibility
        GString *litvisstr = S57_getAttVal(geo, "LITVIS");

        // sector leg --logic is _renderLS()
        g_string_append(lights05, ";LS(DASH,1,CHBLK)");

        // get light vis.
        if (NULL != litvisstr) _parseList(litvisstr->str, litvis);

        // faint light
        // FIXME: spec say OR (ie 1 number) the code is AND/OR
        if (strpbrk(litvis, "\003\007\010")) {
            // NOTE: LS(DASH,1,CHBLK)
            // pass flag to _renderAC()
            g_string_append(lights05, ";AC(CHBLK)");
            S57_setAtt(geo, "faint_light", "Y");

        } else {
            // set arc colour
            char *sym = ";AC(CHMGD)";  // other

            // max 1 color
            if ('\0' == colist[1]) {
                if (strpbrk(colist, "\003"))
                    sym = ";AC(LITRD)";
                else if (strpbrk(colist, "\004"))
                    sym = ";AC(LITGN)";
                else if (strpbrk(colist, "\001\006\013"))
                    sym = ";AC(LITYW)";
            } else {
                // max 2 color
                if ('\0' == colist[2]) {
                    if (strpbrk(colist, "\001") && strpbrk(colist, "\003"))
                        sym = ";AC(LITRD)";
                    else if (strpbrk(colist, "\001") && strpbrk(colist, "\004"))
                        sym = ";AC(LITGN)";
                }
            }

            g_string_append(lights05, sym);
        }
    }

    return lights05;
}

static GString *_LITDSN01(S57_geo *geo)
// Remarks: In S-57 the light characteristics are held as a series of attributes values. The
// mariner may wish to see a light description text string displayed on the
// screen similar to the string commonly found on a paper chart. This
// conditional procedure, reads the attribute values from the above list of
// attributes and composes a light description string which can be displayed.
// This procedure is provided as a C function which has as input, the above
// listed attribute values and as output, the light description.
{
    GString *litdsn01 = g_string_new("");
    GString *gstr     = NULL;  // tmp

    // FIXME: need grammar to create light's text

    // CATLIT, LITCHR, COLOUR, HEIGHT, LITCHR, SIGGRP, SIGPER, STATUS, VALNMR

    // CATLIT
    gstr = S57_getAttVal(geo, "CATLIT");
    if (NULL != gstr)
        g_string_append(litdsn01, gstr->str);

    /*
     1: directional function  IP 30.1-3;  475.7;
     2: rear/upper light
     3: front/lower light
     4: leading light           IP 20.1-3;      475.6;
     5: aero light                  IP 60;      476.1;
     6: air obstruction light IP 61;      476.2;
     7: fog detector light        IP 62;  477;
     8: flood light                 IP 63;      478.2;
     9: strip light                 IP 64;      478.5;
    10: subsidiary light          IP 42;  471.8;
    11: spotlight
    12: front
    13: rear
    14: lower
    15: upper
    16: moire' effect           IP 31;    475.8;
    17: emergency
    18: bearing light                   478.1;
    19: horizontally disposed
    20: vertically disposed
     */

    // LITCHR
    gstr = S57_getAttVal(geo, "LITCHR");
    if (NULL != gstr)
        g_string_append(litdsn01, gstr->str);

    /*
     1: fixed     IP 10.1;
     2: flashing  IP 10.4;
     3: long-flashing   IP 10.5;
     4: quick-flashing  IP 10.6;
     5: very quick-flashing   IP 10.7;
     6: ultra quick-flashing  IP 10.8;
     7: isophased IP 10.3;
     8: occulting IP 10.2;
     9: interrupted quick-flashing  IP 10.6;
    10: interrupted very quick-flashing   IP 10.7;
    11: interrupted ultra quick-flashing  IP 10.8;
    12: morse     IP 10.9;
    13: fixed/flash     IP 10.10;
    14: flash/long-flash
    15: occulting/flash
    16: fixed/long-flash
    17: occulting alternating
    18: long-flash alternating
    19: flash alternating
    20: group alternating
    21: 2 fixed (vertical)
    22: 2 fixed (horizontal)
    23: 3 fixed (vertical)
    24: 3 fixed (horizontal)
    25: quick-flash plus long-flash
    26: very quick-flash plus long-flash
    27: ultra quick-flash plus long-flash
    28: alternating
    29: fixed and alternating flashing
    */

    // COLOUR,
    gstr = S57_getAttVal(geo, "COLOUR");
    if (NULL != gstr)
        g_string_append(litdsn01, gstr->str);

    /*
     1: white     IP 11.1;    450.2-3;
     2: black
     3: red IP 11.2;    450.2-3;
     4: green     IP 11.3;    450.2-3;
     5: blue      IP 11.4;    450.2-3;
     6: yellow    IP 11.6;    450.2-3;
     7: grey
     8: brown
     9: amber     IP 11.8;    450.2-3;
    10: violet    IP 11.5;    450.2-3;
    11: orange    IP 11.7;    450.2-3;
    12: magenta
    13: pink
    */

    // HEIGHT, xxx.x
    gstr = S57_getAttVal(geo, "HEIGHT");
    if (NULL != gstr)
        g_string_append(litdsn01, gstr->str);


    // SIGGRP, (c)(c) ...
    gstr = S57_getAttVal(geo, "SIGGRP");
    if (NULL != gstr)
        g_string_append(litdsn01, gstr->str);


    // SIGPER, xx.xx
    gstr = S57_getAttVal(geo, "SIGPER");
    if (NULL != gstr)
        g_string_append(litdsn01, gstr->str);


    // STATUS,
    gstr = S57_getAttVal(geo, "STATUS");
    if (NULL != gstr)
        g_string_append(litdsn01, gstr->str);

    /*
     1: permanent
     2: occasional      IP 50;      473.2;
     3: recommended     IN 10;      431.1;
     4: not in use      IL 14, 44;  444.7;
     5: periodic/intermittent IC 21; IQ 71;     353.3; 460.5;
     6: reserved  IN 12.9;
     7: temporary IP 54;
     8: private   IQ 70;
     9: mandatory
    10: destroyed/ruined
    11: extinguished
    12: illuminated
    13: historic
    14: public
    15: synchronized
    16: watched
    17: un-watched
    18: existence doubtful
    */

    // VALNMR, xx.x
    gstr = S57_getAttVal(geo, "VALNMR");
    if (NULL != gstr)
        g_string_append(litdsn01, gstr->str);


    PRINTF("FIXME: lights description not translated into text\n");

    return litdsn01;
}

static GString *_UDWHAZ03(S57_geo *geo, double depth_value);
static GString *_QUAPNT01(S57_geo *geo);

static GString *OBSTRN04 (S57_geo *geo)
// Remarks: Obstructions or isolated underwater dangers of depths less than the safety
// contour which lie within the safe waters defined by the safety contour are
// to be presented by a specific isolated danger symbol and put in IMO
// category DISPLAYBASE (see (3), App.2, 1.3). This task is performed
// by the sub-procedure "UDWHAZ03" which is called by this symbology
// procedure. Objects of the class "under water rock" are handled by this
// routine as well to ensure a consistent symbolization of isolated dangers on
// the seabed.
{
    GString *obstrn04str = g_string_new("");
    GString *sndfrm02str = NULL;
    GString *udwhaz03str = NULL;
    GString *valsoustr   = S57_getAttVal(geo, "VALSOU");
    double   valsou      = UNKNOWN;
    double   depth_value = UNKNOWN;
    double   least_depth = UNKNOWN;

    // exit if not in drawing state
    if (1 == S52_state)
        return NULL;

    if (NULL != valsoustr) {
        valsou      = atof(valsoustr->str);
        depth_value = valsou;
        sndfrm02str = _SNDFRM02(geo, depth_value);
    } else {
        if (AREAS_T == S57_getObjtype(geo))
            least_depth = _DEPVAL01(geo, least_depth);

        if (UNKNOWN != least_depth) {
            GString *catobsstr = S57_getAttVal(geo, "CATOBS");
            GString *watlevstr = S57_getAttVal(geo, "WATLEV");

            if (NULL != catobsstr && '6' == *catobsstr->str)
                depth_value = 0.01;
            else
                if (NULL == watlevstr) // default
                    depth_value = -15.0;
                else {
                    switch (*watlevstr->str){
                        case '5': depth_value =   0.0 ; break;
                        case '3': depth_value =   0.01; break;
                        case '4':
                        case '1':
                        case '2':
                        default : depth_value = -15.0 ; break;
                    }
                }
        } else
            depth_value = least_depth;
    }

    udwhaz03str = _UDWHAZ03(geo, depth_value);

    if (POINT_T == S57_getObjtype(geo)) {
        // Continuation A
        int      sounding    = FALSE;
        GString *quapnt01str = _QUAPNT01(geo);

        if (NULL != udwhaz03str){
            g_string_append(obstrn04str, udwhaz03str->str);
            if (NULL != quapnt01str)
                g_string_append(obstrn04str, quapnt01str->str);

            if (NULL != udwhaz03str) g_string_free(udwhaz03str, TRUE);
            if (NULL != sndfrm02str) g_string_free(sndfrm02str, TRUE);
            if (NULL != quapnt01str) g_string_free(quapnt01str, TRUE);

            return obstrn04str;
        }

        if (UNKNOWN != valsou) {
            if (valsou <= 20.0) {
                GString *objlstr   = S57_getAttVal(geo, "OBJL");
                int      objl      = (NULL == objlstr)? 0 : atoi(objlstr->str);
                GString *watlevstr = S57_getAttVal(geo, "WATLEV");

                if (UWTROC == objl) {
                    if (NULL == watlevstr) {  // default
                        g_string_append(obstrn04str, ";SY(DANGER01)");
                        sounding = TRUE;
                    } else {
                        switch (*watlevstr->str){
                            case '3': g_string_append(obstrn04str, ";SY(DANGER01)"); sounding = TRUE ; break;
                            case '4':
                            case '5': g_string_append(obstrn04str, ";SY(UWTROC04)"); sounding = FALSE; break;
                            default : g_string_append(obstrn04str, ";SY(DANGER01)"); sounding = TRUE ; break;
                        }
                    }
                } else { // OBSTRN
                    if (NULL == watlevstr) { // default
                        g_string_append(obstrn04str, ";SY(DANGER01)");
                        sounding = TRUE;
                    } else {
                        switch (*watlevstr->str) {
                            case '1':
                            case '2': g_string_append(obstrn04str, ";SY(OBSTRN11)"); sounding = FALSE; break;
                            case '3': g_string_append(obstrn04str, ";SY(DANGER01)"); sounding = TRUE;  break;
                            case '4':
                            case '5': g_string_append(obstrn04str, ";SY(DANGER03)"); sounding = TRUE; break;
                            default : g_string_append(obstrn04str, ";SY(DANGER01)"); sounding = TRUE; break;
                        }
                    }
                }
            } else {  // valsou > 20.0
                g_string_append(obstrn04str, ";SY(DANGER02)");
                sounding = FALSE;
            }

        } else {  // NO valsou
                GString *objlstr   = S57_getAttVal(geo, "OBJL");
                int     objl       = (NULL == objlstr)? 0 : atoi(objlstr->str);
                GString *watlevstr = S57_getAttVal(geo, "WATLEV");

                if (UWTROC == objl) {
                    if (NULL == watlevstr)  // default
                       g_string_append(obstrn04str, ";SY(UWTROC04)");
                    else {
                        if ('3' == *watlevstr->str)
                            g_string_append(obstrn04str, ";SY(UWTROC03)");
                        else
                            g_string_append(obstrn04str, ";SY(UWTROC04)");
                    }

                } else { // OBSTRN
                    if ( NULL == watlevstr) // default
                        g_string_append(obstrn04str, ";SY(OBSTRN01)");
                    else {
                        switch (*watlevstr->str) {
                            case '1':
                            case '2': g_string_append(obstrn04str, ";SY(OBSTRN11)"); break;
                            case '3': g_string_append(obstrn04str, ";SY(OBSTRN01)"); break;
                            case '4':
                            case '5':
                            default : g_string_append(obstrn04str, ";SY(OBSTRN01)"); break;
                        }
                    }
                }

        }

        if (sounding && NULL != sndfrm02str)
            g_string_append(obstrn04str, sndfrm02str->str);

        if (NULL != quapnt01str)
            g_string_append(obstrn04str, quapnt01str->str);

        if (NULL != udwhaz03str) g_string_free(udwhaz03str, TRUE);
        if (NULL != sndfrm02str) g_string_free(sndfrm02str, TRUE);
        if (NULL != quapnt01str) g_string_free(quapnt01str, TRUE);

        return obstrn04str;

    } else {
        if (LINES_T == S57_getObjtype(geo)) {
            // Continuation B
            GString *quaposstr = S57_getAttVal(geo, "QUAPOS");
            int      quapos    = 0;

            if (NULL != quaposstr) {
                quapos = atoi(quaposstr->str);
                if ( 2 <= quapos && quapos < 10){
                    if (NULL != udwhaz03str)
                        g_string_append(obstrn04str, ";LC(LOWACC41)");
                    else
                        g_string_append(obstrn04str, ";LC(LOWACC31)");
                }
            }

            if (NULL != udwhaz03str)
                g_string_append(obstrn04str, ";LS(DOTT,2,CHBLK)");

            if (UNKNOWN != valsou)
                if (valsou <= 20.0)
                    g_string_append(obstrn04str, ";LS(DOTT,2,CHBLK)");
                else
                    g_string_append(obstrn04str, ";LS(DASH,2,CHBLK)");
            else
                g_string_append(obstrn04str, ";LS(DOTT,2,CHBLK)");


            if (NULL != udwhaz03str)
                g_string_append(obstrn04str, udwhaz03str->str);
            else {
                if (UNKNOWN != valsou)
                    if (valsou <= 20.0)
                        g_string_append(obstrn04str, sndfrm02str->str);
            }

            if (NULL != udwhaz03str) g_string_free(udwhaz03str, TRUE);
            if (NULL != sndfrm02str) g_string_free(sndfrm02str, TRUE);

            return obstrn04str;

        } else {
            // Continuation C (AREAS_T)
            GString *quapnt01str = _QUAPNT01(geo);
            if (NULL != udwhaz03str) {
                g_string_append(obstrn04str, ";AC(DEPVS);AP(FOULAR01)");
                g_string_append(obstrn04str, ";LS(DOTT,2,CHBLK)");
                g_string_append(obstrn04str, udwhaz03str->str);
                if (NULL != quapnt01str)
                    g_string_append(obstrn04str, quapnt01str->str);

                if (NULL != udwhaz03str) g_string_free(udwhaz03str, TRUE);
                if (NULL != sndfrm02str) g_string_free(sndfrm02str, TRUE);
                if (NULL != quapnt01str) g_string_free(quapnt01str, TRUE);

                return obstrn04str;
            }

            if (UNKNOWN != valsou) {
                // BUG in CA49995B.000 if we get here because there is no color
                // beside NODATA (ie there is a hole in group 1 area!)
                //g_string_append(obstrn04, ";AC(UINFR)");

                if (valsou <= 20.0)
                    g_string_append(obstrn04str, ";LS(DOTT,2,CHBLK)");
                else
                    g_string_append(obstrn04str, ";LS(DASH,2,CHBLK)");

                g_string_append(obstrn04str, sndfrm02str->str);

            } else {
                GString *watlevstr = S57_getAttVal(geo, "WATLEV");

                if (NULL == watlevstr)   // default
                    g_string_append(obstrn04str, ";AC(DEPVS);LS(DOTT,2,CHBLK)");
                else {
                    if ('3' == *watlevstr->str) {
                        GString *catobsstr = S57_getAttVal(geo, "CATOBS");
                        if (NULL != catobsstr && '6' == *catobsstr->str)
                            g_string_append(obstrn04str, ";AC(DEPVS);AP(FOULAR01);LS(DOTT,2,CHBLK)");
                    } else {
                        switch (*watlevstr->str) {
                            case '1':
                            case '2': g_string_append(obstrn04str, ";AC(CHBRN);LS(SOLD,2,CSTLN)"); break;
                            case '4': g_string_append(obstrn04str, ";AC(DEPIT);LS(DASH,2,CSTLN)"); break;
                            case '5':
                            case '3':
                            default : g_string_append(obstrn04str, ";AC(DEPVS);LS(DOTT,2,CHBLK)");  break;
                        }
                    }
                }
            }

            g_string_append(obstrn04str, quapnt01str->str);

            if (NULL != udwhaz03str) g_string_free(udwhaz03str, TRUE);
            if (NULL != sndfrm02str) g_string_free(sndfrm02str, TRUE);
            if (NULL != quapnt01str) g_string_free(quapnt01str, TRUE);


            return obstrn04str;
        }

    }


    // check if one exit point could do!!!
    return NULL;
}

static GString *OWNSHP02 (S57_geo *geo)
// Remarks:
// 1. CONNING POSITION
//    1.1 When own-ship is drawn to scale, the conning position must be correctly located in
//        relation to the ship's outline. The conning position then serves as the pivot point for
//        the own-ship symbol, to be located by the ECDIS at the correct latitude, longitude
//        for the conning point, as computed from the positioning system, correcting for
//        antenna offset.
//    1.2 In this procedure it is assumed that the heading line, beam bearing line and course
//        and speed vector originate at the conning point. If another point of origin is used,
//        for example to account for the varying position of the ships turning centre, this must
//        be made clear to the mariner.
//
// 2. DISPLAY OPTIONS
//    2.1 Only the ship symbol is mandatory for an ECDIS. The mariner should be prompted
//        to select from the following additional optional features:
//    - display own-ship as:
//        1. symbol, or
//        2. scaled outline
//    - select time period determining vector length for own-ship and other vessel course and speed
//      vectors, (all vectors must be for the same time period),
//    - display own-ship vector,
//    - select ground or water stabilization for all vectors, and select whether to display the type of
//      stabilization, (by arrowhead),
//    - select one-minute or six-minute vector time marks,
//    - select whether to show a heading line, to the edge of the display window,
//    - select whether to show a beam bearing line, and if so what length (default: 10mm total
//      length).
{
    PRINTF("Mariner's object not drawn\n");
    return NULL;
}

static GString *PASTRK01 (S57_geo *geo)
// Remarks: This conditional symbology procedure was designed to allow the mariner
// to select time labels at the pasttrack (see (3) 10.5.11.1). The procedure also
// cares for the presentation of primary and secondary pasttrack.
//
// The manufacturer should define his own data class (spatial primitive) in xyt
// (position and time) in order to represent Pastrk.
{

    PRINTF("Mariner's object not drawn\n");
    return NULL;
}

static GString *_QUALIN01(S57_geo *geo);
static GString *QUAPOS01 (S57_geo *geo)
// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// to the spatial object, not the feature object.
//
// This procedure passes the object to procedure QUALIN01 or QUAPNT01,
// which traces back to the spatial object, retrieves any QUAPOS attributes,
// and returns the appropriate symbolization to QUAPOS01.
{
    GString *quapos01 = NULL;

    if (LINES_T == S57_getObjtype(geo))
        quapos01 = _QUALIN01(geo);
    else
        quapos01 = _QUAPNT01(geo);

    return quapos01;
}

static GString *_QUALIN01(S57_geo *geo)
// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// A line object may be composed of more than one spatial object.
//
// This procedure looks at each of the spatial
// objects, and symbolizes the line according to the positional accuracy.
{
    GString *qualino1  = NULL;
    GString *quaposstr = S57_getAttVal(geo, "QUAPOS");
    int      quapos    = 0;
    char    *line      = NULL;

    if (NULL != quaposstr) {
        quapos = atoi(quaposstr->str);
        if ( 2 <= quapos && quapos < 10)
            line = ";LC(LOWACC21)";
    } else {
        GString *objlstr = S57_getAttVal(geo, "OBJL");
        int      objl    = (NULL == objlstr)? 0 : atoi(objlstr->str);

        if (COALNE == objl) {
            GString *conradstr = S57_getAttVal(geo, "CONRAD");

            if (NULL != conradstr) {
                if ('1' == *conradstr->str)
                    line = ";LS(SOLD,3,CHMGF);LS(SOLD,1,CSTLN)";
                else
                    line = ";LS(SOLD,1,CSTLN)";
            } else
                line = ";LS(SOLD,1,CSTLN)";

        } else  //LNDARE
            line = ";LS(SOLD,1,CSTLN)";
    }

    if (NULL != line)
        qualino1 = g_string_new(line);

    return qualino1;
}

static GString *_QUAPNT01(S57_geo *geo)
// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// This procedure retrieves any QUAPOS attributes, and returns the
// appropriate symbols to the calling procedure.
{
    GString *quapnt01  = NULL;
    int      accurate  = TRUE;
    GString *quaposstr = S57_getAttVal(geo, "QUAPOS");
    int      quapos    = (NULL == quaposstr)? 0 : atoi(quaposstr->str);

    if (NULL != quaposstr) {
        if ( 2 <= quapos && quapos < 10)
            accurate = FALSE;
    }

    if (accurate)
        quapnt01 = g_string_new(";SY(LOWACC01)");

    return quapnt01;
}

static GString *SLCONS03 (S57_geo *geo)
// Remarks: Shoreline construction objects which have a QUAPOS attribute on their
// spatial component indicating that their position is unreliable are symbolized
// by a special linestyle in the place of the varied linestyles normally used.
// Otherwise this procedure applies the normal symbolization.
{
    GString *slcons03  = NULL;
    GString *valstr    = NULL;
    char    *cmdw      = NULL;   // command word
    GString *quaposstr = S57_getAttVal(geo, "QUAPOS");
    int      quapos    = (NULL == quaposstr)? 0 : atoi(quaposstr->str);

    if (POINT_T == S57_getObjtype(geo)) {
        if (NULL != quaposstr) {
            if (2 <= quapos && quapos < 10)
                cmdw =";SY(LOWACC01)";
        }
    } else {
        // LINE_T and AREA_T are the same
        if (NULL != quaposstr) {
            if (2 <= quapos && quapos < 10)
                cmdw =";LC(LOWACC01)";
        } else {
            valstr = S57_getAttVal(geo, "CONDTN");

            if (NULL != valstr && ( '1' == *valstr->str || '2' == *valstr->str))
                    cmdw = ";LS(DASH,1,CSTLN)";
            else {
                int val = 0;
                valstr  = S57_getAttVal(geo, "CATSLC");
                val     = (NULL == valstr)? 0 : atoi(valstr->str);

                if (NULL != valstr && ( 6  == val || 15 == val || 16 == val ))
                        cmdw = ";LS(SOLD,4,CSTLN)";
                else {
                    valstr = S57_getAttVal(geo, "WATLEV");

                    if (NULL != valstr && '2' == *valstr->str)
                            cmdw = ";LS(SOLD,2,CSTLN)";
                    else
                        if (NULL != valstr && ('3' == *valstr->str || '4' == *valstr->str))
                            cmdw = ";LS(DASH,2,CSTLN)";
                        else
                            cmdw = ";LS(SOLD,2,CSTLN)";  // default

                }
            }
        }
    }

    // WARNING: not explicitly specified in S-52 !!
    // FIXME:this is to put AC(DEPIT) --intertidal area
    // Could this be bug in OGR ?
    /*
    if (AREAS_T == S57_getObjtype(geo)) {
        GString    *seabed01  = NULL;
        GString    *drval1str = S57_getAttVal(geo, "DRVAL1");
        double      drval1    = (NULL == drval1str)? -UNKNOWN : atof(drval1str->str);
        GString    *drval2str = S57_getAttVal(geo, "DRVAL2");
        double      drval2    = (NULL == drval2str)? -UNKNOWN : atof(drval2str->str);
        // NOTE: change sign of infinity (minus) to get out of bound in seabed01


        PRINTF("***********drval1=%f drval2=%f \n", drval1, drval2);
        seabed01 = _SEABED01(drval1, drval2);
        slcons03 = g_string_new(seabed01->str);
        g_string_free(seabed01, TRUE);

    }
    */

    if (NULL != cmdw) {
        if (NULL == slcons03)
            slcons03 = g_string_new(cmdw);
        else
            g_string_append(slcons03, cmdw);
    }

    return slcons03;
}

static GString *RESARE02 (S57_geo *geo)
// Remarks: A list-type attribute is used because an area of the object class RESARE may
// have more than one category (CATREA). For example an inshore traffic
// zone might also have fishing and anchoring prohibition and a prohibited
// area might also be a bird sanctuary or a mine field.
//
// This conditional procedure is set up to ensure that the categories of most
// importance to safe navigation are prominently symbolized, and to pass on
// all given information with minimum clutter. Only the most significant
// restriction is symbolized, and an indication of further limitations is given by
// a subscript "!" or "I". Further details are given under conditional
// symbology procedure RESTRN01
//
// Other object classes affected by attribute RESTRN are handled by
// conditional symbology procedure RESTRN01.
{
    GString *resare02         = g_string_new("");
    GString *restrnstr        = S57_getAttVal(geo, "RESTRN");
    char     restrn[LISTSIZE] = {'\0'};
    GString *catreastr        = S57_getAttVal(geo, "CATREA");
    char     catrea[LISTSIZE] = {'\0'};
    char    *symb             = NULL;
    char    *line             = NULL;
    char    *prio             = NULL;

    if ( NULL != restrnstr) {
        _parseList(restrnstr->str, restrn);

        if (NULL != catreastr) _parseList(catreastr->str, catrea);

        if (strpbrk(restrn, "\007\010\016")) {
            // Continuation A
            if (strpbrk(restrn, "\001\002\003\004\005\006"))
                symb = ";SY(ENTRES61)";
            else {
                if (NULL != catreastr && strpbrk(catrea, "\001\010\011\014\016\023\025\031"))
                        symb = ";SY(ENTRES61)";
                else {
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(ENTRES71)";
                    else {
                        if (NULL != catreastr && strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                            symb = ";SY(ENTRES71)";
                        else
                            symb = ";SY(ENTRES51)";
                    }
                }
            }

            if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
                line = ";LC(CTYARE51)";
            else
                line = ";LS(DASH,2,CHMGD)";

            prio = ";OP(6---)";  // display prio set to 6

        } else {
            if (strpbrk(restrn, "\001\002")) {
                // Continuation B
                if (strpbrk(restrn, "\003\004\005\006"))
                    symb = ";SY(ACHRES61)";
                else {
                    if (NULL != catreastr && strpbrk(catrea, "\001\010\011\014\016\023\025\031"))
                            symb = ";SY(ACHRES61)";
                    else {
                        if (strpbrk(restrn, "\011\012\013\014\015"))
                            symb = ";SY(ACHRES71)";
                        else {
                            if (NULL != catreastr && strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                                symb = ";SY(ACHRES71)";
                            else
                                symb = ";SY(ACHRES51)";
                        }
                    }
                }

                if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
                    line = ";LC(ACHRE51)";
                else
                    line = ";LS(DASH,2,CHMGD)";

                prio = ";OP(6---)";  // display prio set to 6

            } else {
                if (strpbrk(restrn, "\003\004\005\006")) {
                    // Continuation C
                    if (NULL != catreastr && strpbrk(catrea, "\001\010\011\014\016\023\025\031"))
                            symb = ";SY(FSHRES51)";
                    else {
                        if (strpbrk(restrn, "\011\012\013\014\015"))
                            symb = ";SY(FSHRES71)";
                        else{
                            if (NULL != catreastr && strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                                symb = ";SY(FSHRES71)";
                            else
                                symb = ";SY(FSHRES51)";
                        }
                    }

                    if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
                        line = ";LC(FSHRES51)";
                    else
                        line = ";LS(DASH,2,CHMGD)";

                    prio = ";OP(6---)";  // display prio set to 6

                } else {
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(INFARE51)";
                    else
                        symb = ";SY(RSRDEF51)";

                    if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
                        line = ";LC(CTYARE51)";
                    else
                        line = ";LS(DASH,2,CHMGD)";

                }
            }
        }

    } else {
        // Continuation D
        if (NULL != catreastr) {
            if (strpbrk(catrea, "\001\010\011\014\016\023\025\031")) {
                if (strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                    symb = ";SY(CTYARE71)";
                else
                    symb = ";SY(CTYARE51)";
            } else {
                if (strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030"))
                    symb = ";SY(INFARE71)";
                else
                    symb = ";SY(RSRDEF51)";
            }
        } else
            symb = ";SY(RSRDEF51)";

        if (TRUE == S52_getMarinerParam(S52_MAR_SYMBOLIZED_BND))
            line = ";LC(CTYARE51)";
        else
            line = ";LS(DASH,2,CHMGD)";
    }

    // create command word
    if (NULL != prio)
        g_string_append(resare02, prio);
    g_string_append(resare02, line);
    g_string_append(resare02, symb);

    return resare02;
}

static GString *RESTRN01 (S57_geo *geo)
// Remarks: Objects subject to RESTRN01 are actually symbolised in sub-process
// RESCSP01, since the latter can also be accessed from other conditional
// symbology procedures. RESTRN01 merely acts as a "signpost" for
// RESCSP01.
//
// Object class RESARE is symbolised for the effect of attribute RESTRN in a separate
// conditional symbology procedure called RESARE02.
//
// Since many of the areas concerned cover shipping channels, the number of symbols used
// is minimised to reduce clutter. To do this, values of RESTRN are ranked for significance
// as follows:
// "Traffic Restriction" values of RESTRN:
// (1) RESTRN 7,8: entry prohibited or restricted
//     RESTRN 14: IMO designated "area to be avoided" part of a TSS
// (2) RESTRN 1,2: anchoring prohibited or restricted
// (3) RESTRN 3,4,5,6: fishing or trawling prohibited or restricted
// (4) "Other Restriction" values of RESTRN are:
//     RESTRN 9, 10: dredging prohibited or restricted,
//     RESTRN 11,12: diving prohibited or restricted,
//     RESTRN 13   : no wake area.
{
    GString *restrn01str = S57_getAttVal(geo, "RESTRN");
    GString *restrn01    = NULL;

    if (NULL != restrn01str)
        restrn01 = _RESCSP01(geo);
    else
        restrn01 = g_string_new(";OP(----)");  // return NOOP to silence error msg

    return restrn01;
}

static GString *_RESCSP01(S57_geo *geo)
// Remarks: See procedure RESTRN01
{
    GString *rescsp01         = NULL;
    GString *restrnstr        = S57_getAttVal(geo, "RESTRN");
    char     restrn[LISTSIZE] = {'\0'};   // restriction list
    char    *symb             = NULL;

    if ( NULL != restrnstr) {
        _parseList(restrnstr->str, restrn);

        if (strpbrk(restrn, "\007\010\016")) {
            // continuation A
            if (strpbrk(restrn, "\001\002\003\004\005\006"))
                symb = ";SY(ENTRES61)";
            else {
                if (strpbrk(restrn, "\011\012\013\014\015"))
                    symb = ";SY(ENTRES71)";
                else
                    symb = ";SY(ENTRES51)";

            }
        } else {
            if (strpbrk(restrn, "\001\002")) {
                // continuation B
                if (strpbrk(restrn, "\003\004\005\006"))
                    symb = ";SY(ACHRES61)";
                else {
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(ACHRES71)";
                    else
                        symb = ";SY(ACHRES51)";
                }


            } else {
                if (strpbrk(restrn, "\003\004\005\006")) {
                    // continuation C
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(FSHRES71)";
                    else
                        symb = ";SY(FSHRES51)";


                } else {
                    if (strpbrk(restrn, "\011\012\013\014\015"))
                        symb = ";SY(INFARE51)";
                    else
                        symb = ";SY(RSRDEF51)";

                }
            }
        }

        rescsp01 = g_string_new(symb);
    }

    return rescsp01;
}

static GString *_SEABED01(double drval1, double drval2)
// Remarks: An area object that is part of the seabed is coloured as necessary according
// to the mariners selection of two shades, (shallow contour, safety contour,
// deep contour), or four shades (safety contour only). This requires a decision
// making process provided by this conditional symbology procedure. Note
// that this procedure is called as a sub-procedure by other conditional
// symbology procedures.
//
// Note: The requirement to show four depth shades is not mandatory. Also,
// the requirement to show the shallow pattern is not mandatory. However,
// both these features are strongly recommended.

// return: is never NULL

{
    GString *seabed01 = NULL;
    gboolean shallow  = TRUE;
    char    *arecol   = ";AC(DEPIT)";

    if (drval1 >= 0.0 && drval2 > 0.0)
        arecol  = ";AC(DEPVS)";

    if (TRUE == S52_getMarinerParam(S52_MAR_TWO_SHADES)){
        if (drval1 >= S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)  &&
            drval2 >  S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)) {
            arecol  = ";AC(DEPDW)";
            shallow = FALSE;
        }
    } else {
        if (drval1 >= S52_getMarinerParam(S52_MAR_SHALLOW_CONTOUR) &&
            drval2 >  S52_getMarinerParam(S52_MAR_SHALLOW_CONTOUR))
            arecol  = ";AC(DEPMS)";

            if (drval1 >= S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)  &&
                drval2 >  S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR)) {
            arecol  = ";AC(DEPMD)";
            shallow = FALSE;
        }

            if (drval1 >= S52_getMarinerParam(S52_MAR_DEEP_CONTOUR)  &&
                drval2 >  S52_getMarinerParam(S52_MAR_DEEP_CONTOUR)) {
            arecol  = ";AC(DEPDW)";
            shallow = FALSE;
        }

    }

    seabed01 = g_string_new(arecol);

    if (TRUE==S52_getMarinerParam(S52_MAR_SHALLOW_PATTERN) && TRUE==shallow)
        g_string_append(seabed01, ";AP(DIAMOND1)");

    return seabed01;
}

static GString *SOUNDG02 (S57_geo *geo)
// Remarks: In S-57 soundings are elements of sounding arrays rather than individual
// objects. Thus this conditional symbology procedure examines each
// sounding of a sounding array one by one. To symbolize the depth values it
// calls the procedure SNDFRM02 which in turn translates the depth values
// into a set of symbols to be shown at the soundings position.
{
    guint   npt = 0;
    double *ppt = NULL;

    if (POINT_T != S57_getObjtype(geo)) {
        PRINTF("invalid object type (not M_PNT_T)\n");
        //return NULL;
        exit(0);
    }

    S57_getGeoData(geo, 0, &npt, &ppt);

    return _SNDFRM02(geo, ppt[2]);
}

static GString *_SNDFRM02(S57_geo *geo, double depth_value)
// Remarks: Soundings differ from plain text because they have to be readable under all
// circumstances and their digits are placed according to special rules. This
// conditional symbology procedure accesses a set of carefully designed
// sounding symbols provided by the symbol library and composes them to
// sounding labels. It symbolizes swept depth and it also symbolizes for low
// reliability as indicated by attributes QUASOU and QUAPOS.
{
    GString *sndfrm02         = g_string_new("");
    char    *symbol_prefix    = NULL;
    GString *tecsoustr        = S57_getAttVal(geo, "TECSOU");
    char     tecsou[LISTSIZE] = {'\0'};
    GString *quasoustr        = S57_getAttVal(geo, "QUASOU");
    char     quasou[LISTSIZE] = {'\0'};
    GString *statusstr        = S57_getAttVal(geo, "STATUS");
    char     status[LISTSIZE] = {'\0'};
    double   leading_digit    = 0.0;

    // FIXME: test to fix the rounding error (!?)
    depth_value  += (depth_value > 0.0)? 0.01: -0.01;
    leading_digit = (int) depth_value;

    if (depth_value <= S52_getMarinerParam(S52_MAR_SAFETY_DEPTH))
        symbol_prefix = "SOUNDS";
    else
        symbol_prefix = "SOUNDG";

    if (NULL != tecsoustr) {
        _parseList(tecsoustr->str, tecsou);
        if (strpbrk(tecsou, "\006"))
            g_string_sprintfa(sndfrm02, ";SY(%sB1)", symbol_prefix);
    }

    if (NULL != quasoustr) _parseList(quasoustr->str, quasou);
    if (NULL != statusstr) _parseList(statusstr->str, status);

    if (strpbrk(quasou, "\003\004\005\010\011") || strpbrk(status, "\022"))
            g_string_sprintfa(sndfrm02, ";SY(%sC2)", symbol_prefix);
    else {
        GString *quaposstr = S57_getAttVal(geo, "QUAPOS");
        int      quapos    = (NULL == quaposstr)? 0 : atoi(quaposstr->str);

        if (NULL != quaposstr) {
            if (2 <= quapos && quapos < 10)
                g_string_sprintfa(sndfrm02, ";SY(%sC2)", symbol_prefix);
        }
    }

    // Continuation A
    if (depth_value < 10.0) {
        // can be above water (negative)
        int fraction = (int)ABS((depth_value - leading_digit)*10);

        g_string_sprintfa(sndfrm02, ";SY(%s1%1i)", symbol_prefix, (int)ABS(leading_digit));
        g_string_sprintfa(sndfrm02, ";SY(%s5%1i)", symbol_prefix, fraction);

        // above sea level (negative)
        if (depth_value < 0.0)
            g_string_sprintfa(sndfrm02, ";SY(%sA1)", symbol_prefix);

        return sndfrm02;
    }

    if (depth_value < 31.0) {
        double fraction = depth_value - leading_digit;

        if (fraction != 0.0) {
            fraction = fraction * 10;
            if (leading_digit >= 10.0)
                g_string_sprintfa(sndfrm02, ";SY(%s2%1i)", symbol_prefix, (int)leading_digit/10);

            g_string_sprintfa(sndfrm02, ";SY(%s1%1i)", symbol_prefix, (int)leading_digit);
            g_string_sprintfa(sndfrm02, ";SY(%s5%1i)", symbol_prefix, (int)fraction);

            return sndfrm02;
        }
    }

    // Continuation B
    depth_value = leading_digit;    // truncate to integer
    if (depth_value < 100.0) {
        double first_digit = leading_digit / 10;
        double secnd_digit = leading_digit - (first_digit * 10);

        g_string_sprintfa(sndfrm02, ";SY(%s1%1i)", symbol_prefix, (int)first_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s0%1i)", symbol_prefix, (int)secnd_digit);

        return sndfrm02;
    }

    if (depth_value < 1000.0) {
        double first_digit = leading_digit / 100;
        double secnd_digit = leading_digit - (first_digit * 100);
        double third_digit = leading_digit - (first_digit * 100) - (secnd_digit * 10);

        g_string_sprintfa(sndfrm02, ";SY(%s2%1i)", symbol_prefix, (int)first_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s1%1i)", symbol_prefix, (int)secnd_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s0%1i)", symbol_prefix, (int)third_digit);

        return sndfrm02;
    }

    if (depth_value < 10000.0) {
        double first_digit = leading_digit / 1000;
        double secnd_digit = leading_digit - (first_digit * 1000);
        double third_digit = leading_digit - (first_digit * 1000) - (secnd_digit * 100);
        double last_digit  = leading_digit - (first_digit * 1000) - (secnd_digit * 100) - (third_digit * 100) ;

        g_string_sprintfa(sndfrm02, ";SY(%s2%1i)", symbol_prefix, (int)first_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s1%1i)", symbol_prefix, (int)secnd_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s0%1i)", symbol_prefix, (int)third_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s4%1i)", symbol_prefix, (int)last_digit);

        return sndfrm02;
    }

    // Continuation C
    {
        double first_digit  = leading_digit / 10000;
        double secnd_digit  = leading_digit - (first_digit * 10000);
        double third_digit  = leading_digit - (first_digit * 10000) - (secnd_digit * 1000);
        double fourth_digit = leading_digit - (first_digit * 10000) - (secnd_digit * 1000) - (third_digit * 100) ;
        double last_digit   = leading_digit - (first_digit * 10000) - (secnd_digit * 1000) - (third_digit * 100) - (fourth_digit * 10) ;

        g_string_sprintfa(sndfrm02, ";SY(%s3%1i)", symbol_prefix, (int)first_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s2%1i)", symbol_prefix, (int)secnd_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s1%1i)", symbol_prefix, (int)third_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s0%1i)", symbol_prefix, (int)fourth_digit);
        g_string_sprintfa(sndfrm02, ";SY(%s4%1i)", symbol_prefix, (int)last_digit);

        return sndfrm02;
    }

    return sndfrm02;
}

static GString *TOPMAR01 (S57_geo *geo)
// Remarks: Topmark objects are to be symbolized through consideration of their
// platforms e.g. a buoy. Therefore this conditional symbology procedure
// searches for platforms by looking for other objects that are located at the
// same position.. Based on the finding whether the platform is rigid or
// floating, the respective upright or sloping symbol is selected and presented
// at the objects location. Buoy symbols and topmark symbols have been
// carefully designed to fit to each other when combined at the same position.
// The result is a composed symbol that looks like the traditional symbols the
// mariner is used to.
{
    GString *topshpstr = S57_getAttVal(geo, "TOPSHP");
    GString *topmar    = NULL;
    char    *sy        = NULL;

    if (NULL == topshpstr)
        sy = ";SY(QUESMRK1)";
    else {
        int floating    = FALSE; // not a floating platform
        int topshp      = (NULL==topshpstr) ? 0 : atoi(topshpstr->str);

        if (TRUE == _atPtPos(geo, FLOATLIST))
            floating = TRUE;
        else
            // FIXME: this test is wierd since it doesn't affect 'floating'
            if (TRUE == _atPtPos(geo, RIGIDLIST))
                floating = FALSE;


        if (floating) {
            // floating platform
            switch (topshp) {
                case 1 : sy = ";SY(TOPMAR02)"; break;
                case 2 : sy = ";SY(TOPMAR04)"; break;
                case 3 : sy = ";SY(TOPMAR10)"; break;
                case 4 : sy = ";SY(TOPMAR12)"; break;

                case 5 : sy = ";SY(TOPMAR13)"; break;
                case 6 : sy = ";SY(TOPMAR14)"; break;
                case 7 : sy = ";SY(TOPMAR65)"; break;
                case 8 : sy = ";SY(TOPMAR17)"; break;

                case 9 : sy = ";SY(TOPMAR16)"; break;
                case 10: sy = ";SY(TOPMAR08)"; break;
                case 11: sy = ";SY(TOPMAR07)"; break;
                case 12: sy = ";SY(TOPMAR14)"; break;

                case 13: sy = ";SY(TOPMAR05)"; break;
                case 14: sy = ";SY(TOPMAR06)"; break;
                case 17: sy = ";SY(TMARDEF2)"; break;
                case 18: sy = ";SY(TOPMAR10)"; break;

                case 19: sy = ";SY(TOPMAR13)"; break;
                case 20: sy = ";SY(TOPMAR14)"; break;
                case 21: sy = ";SY(TOPMAR13)"; break;
                case 22: sy = ";SY(TOPMAR14)"; break;

                case 23: sy = ";SY(TOPMAR14)"; break;
                case 24: sy = ";SY(TOPMAR02)"; break;
                case 25: sy = ";SY(TOPMAR04)"; break;
                case 26: sy = ";SY(TOPMAR10)"; break;

                case 27: sy = ";SY(TOPMAR17)"; break;
                case 28: sy = ";SY(TOPMAR18)"; break;
                case 29: sy = ";SY(TOPMAR02)"; break;
                case 30: sy = ";SY(TOPMAR17)"; break;

                case 31: sy = ";SY(TOPMAR14)"; break;
                case 32: sy = ";SY(TOPMAR10)"; break;
                case 33: sy = ";SY(TMARDEF2)"; break;
                default: sy = ";SY(TMARDEF2)"; break;
            }
        } else {
            // not a floating platform
            switch (topshp) {
                case 1 : sy = ";SY(TOPMAR22)"; break;
                case 2 : sy = ";SY(TOPMAR24)"; break;
                case 3 : sy = ";SY(TOPMAR30)"; break;
                case 4 : sy = ";SY(TOPMAR32)"; break;

                case 5 : sy = ";SY(TOPMAR33)"; break;
                case 6 : sy = ";SY(TOPMAR34)"; break;
                case 7 : sy = ";SY(TOPMAR85)"; break;
                case 8 : sy = ";SY(TOPMAR86)"; break;

                case 9 : sy = ";SY(TOPMAR36)"; break;
                case 10: sy = ";SY(TOPMAR28)"; break;
                case 11: sy = ";SY(TOPMAR27)"; break;
                case 12: sy = ";SY(TOPMAR14)"; break;

                case 13: sy = ";SY(TOPMAR25)"; break;
                case 14: sy = ";SY(TOPMAR26)"; break;
                case 15: sy = ";SY(TOPMAR88)"; break;
                case 16: sy = ";SY(TOPMAR87)"; break;

                case 17: sy = ";SY(TMARDEF1)"; break;
                case 18: sy = ";SY(TOPMAR30)"; break;
                case 19: sy = ";SY(TOPMAR33)"; break;
                case 20: sy = ";SY(TOPMAR34)"; break;

                case 21: sy = ";SY(TOPMAR33)"; break;
                case 22: sy = ";SY(TOPMAR34)"; break;
                case 23: sy = ";SY(TOPMAR34)"; break;
                case 24: sy = ";SY(TOPMAR22)"; break;

                case 25: sy = ";SY(TOPMAR24)"; break;
                case 26: sy = ";SY(TOPMAR30)"; break;
                case 27: sy = ";SY(TOPMAR86)"; break;
                case 28: sy = ";SY(TOPMAR89)"; break;

                case 29: sy = ";SY(TOPMAR22)"; break;
                case 30: sy = ";SY(TOPMAR86)"; break;
                case 31: sy = ";SY(TOPMAR14)"; break;
                case 32: sy = ";SY(TOPMAR30)"; break;
                case 33: sy = ";SY(TMARDEF1)"; break;
                default: sy = ";SY(TMARDEF1)"; break;
            }
        }

    }

    topmar = g_string_new(sy);

    return topmar;
}

static GString *_UDWHAZ03(S57_geo *geo, double depth_value)
// Remarks: Obstructions or isolated underwater dangers of depths less than the safety
// contour which lie within the safe waters defined by the safety contour are
// to be presented by a specific isolated danger symbol as hazardous objects
// and put in IMO category DISPLAYBASE (see (3), App.2, 1.3). This task
// is performed by this conditional symbology procedure.
{
    GString *udwhaz03str    = NULL;
    int      danger         = FALSE;
    double   safety_contour = S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR);
    S57_geo *geoTmp         = geo;

    if (depth_value <= safety_contour) {
        // that intersect this point/line/area for OBSTRN04
        // that intersect this point/area      for WRECKS02

        // get area DEPARE & DRGARE that intersect this point/line/area
        while (NULL != (geoTmp = S57_nextObj(geoTmp))) {

            if (LINES_T  == S57_getObjtype(geoTmp)) {
                GString *drval2str = S57_getAttVal(geoTmp, "DRVAL2");
                double   drval2    = (NULL == drval2str) ? 0.0 : atof(drval2str->str);

                if (drval2 < safety_contour) {
                    danger = TRUE;
                    break;
                }

            } else {
                //
                GString *drval1str = S57_getAttVal(geoTmp, "DRVAL1");
                double   drval1    = (NULL == drval1str) ? 0.0 : atof(drval1str->str);

                if (drval1 >= safety_contour) {
                    danger = TRUE;
                    break;
                }
            }

        }


        //danger = TRUE;   // true
        if (TRUE == danger) {
            GString *watlevstr = S57_getAttVal(geo, "WATLEV");
            if (NULL != watlevstr && ('1' == *watlevstr->str || '2' == *watlevstr->str))
                udwhaz03str = g_string_new(";OP(--D14050");
            else {
                udwhaz03str = g_string_new(";OP(8OD14010);SY(ISODGR01)");
                S57_setAtt(geo, "SCAMIN", "INFINITE");
            }
        }
    }

    S57_unlinkObj(geo);

    return udwhaz03str;
}

static GString *VESSEL01 (S57_geo *geo)
// Remarks: The mariner should be prompted to select from the following options:
// - ARPA target or AIS report (overall decision or vessel by vessel) (vesrce)
// - *time-period determining vector-length for all vectors (vecper)
// - whether to show a vector (overall or vessel by vessel) (vestat)
// - *whether to symbolize vector stabilization (vecstb)
// - *whether to show one-minute or six-minute vector time marks (vecmrk)
// - whether to show heading line on AIS vessel reports (headng)
// * Note that the same vector parameters should be used for own-ship and all vessel
// vectors.
{
    PRINTF("Mariner's object not drawn\n");
    return NULL;
}

static GString *VRMEBL01 (S57_geo *geo)
// Remarks: This conditional symbology procedure symbolizes the three cases of range
// circle, bearing line and range/bearing line. VRM's and EBL's can be ship-centred
// or freely movable, and two line-styles are available
{
    PRINTF("Mariner's object not drawn\n");
    return NULL;
}

static GString *WRECKS02 (S57_geo *geo)
// Remarks: Wrecks of depths less than the safety contour which lie within the safe waters
// defined by the safety contour are to be presented by a specific isolated
// danger symbol and put in IMO category DISPLAYBASE (see (3), App.2,
// 1.3). This task is performed by the sub-procedure "UDWHAZ03" which is
// called by this symbology procedure.
{
    GString *wrecks02str = NULL;
    GString *sndfrm02str = NULL;
    GString *udwhaz03str = NULL;
    GString *quapnt01str = NULL;
    double   least_depth = UNKNOWN;
    double   depth_value = UNKNOWN;
    GString *valsoustr   = S57_getAttVal(geo, "VALSOU");
    double   valsou      = UNKNOWN;

    // exit if not in drawing state
    if (1 == S52_state)
        return NULL;

    if (NULL != valsoustr) {
        valsou      = atof(valsoustr->str);
        depth_value = valsou;
        sndfrm02str = _SNDFRM02(geo, depth_value);
    } else {
        if (AREAS_T == S57_getObjtype(geo))
            least_depth = _DEPVAL01(geo, least_depth);

        if (least_depth == UNKNOWN) {
            // WARNING: ambiguity removed in WRECKS03 (see update)
            GString *watlevstr = S57_getAttVal(geo, "WATLEV");
            GString *catwrkstr = S57_getAttVal(geo, "CATWRK");

            if (NULL == watlevstr) // default
                depth_value = -15.0;
            else
                switch (*watlevstr->str) { // ambiguous
                    case '1':
                    case '2': depth_value = -15.0 ; break;
                    case '3': depth_value =   0.01; break;
                    case '4': depth_value = -15.0 ; break;
                    case '5': depth_value =   0.0 ; break;
                    case '6': depth_value = -15.0 ; break;
                    default :{
                        if (NULL != catwrkstr) {
                            switch (*catwrkstr->str) {
                                case '1': depth_value =  20.0; break;
                                case '2': depth_value =   0.0; break;
                                case '4':
                                case '5': depth_value = -15.0; break;
                            }
                        }
                    }
                }
        } else
            depth_value = least_depth;


    }

    udwhaz03str = _UDWHAZ03(geo, depth_value);
    quapnt01str = _QUAPNT01(geo);

    if (POINT_T == S57_getObjtype(geo)) {
        if (NULL != udwhaz03str) {
            wrecks02str = g_string_new(udwhaz03str->str);

            if (NULL != quapnt01str)
                g_string_append(wrecks02str, quapnt01str->str);

        } else {
            // Continuation A (POINT_T)
            if (UNKNOWN != valsou) {

                if (valsou <= 20.0) {
                    wrecks02str = g_string_new(";SY(DANGER01)");
                    if (NULL != sndfrm02str)
                        g_string_append(wrecks02str, sndfrm02str->str);

                } else
                    wrecks02str = g_string_new(";SY(DANGER02)");

                if (NULL != udwhaz03str)
                    g_string_append(wrecks02str, udwhaz03str->str);
                if (NULL != quapnt01str)
                    g_string_append(wrecks02str, quapnt01str->str);

            } else {
                char    *sym       = NULL;
                GString *catwrkstr = S57_getAttVal(geo, "CATWRK");
                GString *watlevstr = S57_getAttVal(geo, "WATLEV");

                if (NULL != catwrkstr && NULL != watlevstr) {
                    if ('1' == *catwrkstr->str && '3' == *watlevstr->str)
                        sym =";SY(WRECKS04)";
                    else {
                        if ('2' == *catwrkstr->str && '3' == *watlevstr->str)
                            sym = ";SY(WRECKS05)";
                        else {
                            if ('4' == *catwrkstr->str || '5' == *catwrkstr->str)
                                sym = ";SY(WRECKS01)";
                            else {
                                if ('1' == *watlevstr->str ||
                                    '2' == *watlevstr->str ||
                                    '5' == *watlevstr->str ||
                                    '4' == *watlevstr->str ){
                                    sym = ";SY(WRECKS01)";
                                } else
                                    sym = ";SY(WRECKS05)"; // default

                            }
                        }
                    }
                }

                wrecks02str = g_string_new(sym);
                if (NULL != quapnt01str)
                    g_string_append(wrecks02str, quapnt01str->str);

            }

        }


    } else {
        // Continuation B (AREAS_T)
        GString *quaposstr = S57_getAttVal(geo, "QUAPOS");
        int      quapos    = (NULL == quaposstr)? 0 : atoi(quaposstr->str);
        char    *line      = NULL;

        if (2 <= quapos && quapos < 10)
            line = ";LC(LOWACC41)";
        else {
            if ( NULL != udwhaz03str)
                line = ";LS(DOTT,2,CHBLK)";
            else {
                 if (UNKNOWN != valsou){
                     if (valsou <= 20)
                         line = ";LS(DOTT,2,CHBLK)";
                     else
                         line = ";LS(DASH,2,CHBLK)";
                 } else {
                     GString  *watlevstr = S57_getAttVal(geo, "WATLEV");

                     if (NULL == watlevstr)
                         line = ";LS(DOTT,2,CSTLN)";
                     else {
                         switch (*watlevstr->str){
                             case '1':
                             case '2': line = ";LS(SOLD,2,CSTLN)"; break;
                             case '4': line = ";LS(DASH,2,CSTLN)"; break;
                             case '3':
                             case '5':

                             default : line = ";LS(DOTT,2,CSTLN)"; break;
                         }
                     }

                 }
            }
        }
        wrecks02str = g_string_new(line);

        if (UNKNOWN != valsou) {
            if (valsou <= 20) {
                if (NULL != udwhaz03str)
                    g_string_append(wrecks02str, udwhaz03str->str);

                if (NULL != quapnt01str)
                    g_string_append(wrecks02str, quapnt01str->str);

                if (NULL != sndfrm02str)
                    g_string_append(wrecks02str, sndfrm02str->str);

            } else {
                // NOTE: ??? same as above ???
                if (NULL != udwhaz03str)
                    g_string_append(wrecks02str, udwhaz03str->str);

                if (NULL != quapnt01str)
                    g_string_append(wrecks02str, quapnt01str->str);
            }
        } else {
            char    *ac        = NULL;
            GString *watlevstr = S57_getAttVal(geo, "WATLEV");

            if (NULL == watlevstr)
                ac = ";AC(DEPVS)";
            else
                switch (*watlevstr->str) {
                    case '1':
                    case '2': ac = ";AC(CHBRN)"; break;
                    case '4': ac = ";AC(DEPIT)"; break;
                    case '5':
                    case '3':
                    default : ac = ";AC(DEPVS)"; break;
                }

            g_string_append(wrecks02str, ac);

            if (NULL != udwhaz03str)
                g_string_append(wrecks02str, udwhaz03str->str);

            if (NULL != quapnt01str)
                g_string_append(wrecks02str, quapnt01str->str);
        }
    }

    if (NULL != sndfrm02str) g_string_free(sndfrm02str, TRUE);
    if (NULL != udwhaz03str) g_string_free(udwhaz03str, TRUE);
    if (NULL != quapnt01str) g_string_free(quapnt01str, TRUE);

    return wrecks02str;
}


//--------------------------------
//
// JUMP TABLE SECTION
//
//--------------------------------


CondSymb condTable[] = {
   // name      call            Sub-Procedure
   {"CLRLIN01", CLRLIN01},   //
   {"DATCVR01", DATCVR01},   //
   {"DEPARE01", DEPARE01},   // _RESCSP01, _SEABED01
   {"DEPCNT02", DEPCNT02},   //
   {"LEGLIN02", LEGLIN02},   //                          str
   {"LIGHTS05", LIGHTS05},   // _LITDSN01
   {"OBSTRN04", OBSTRN04},   // _DEPVAL01, _QUAPNT01, _SNDFRM02, _UDWHAZ03
   {"OWNSHP02", OWNSHP02},   //
   {"PASTRK01", PASTRK01},   //
   {"QUAPOS01", QUAPOS01},   // _QUALIN01, _QUAPNT01
   {"SLCONS03", SLCONS03},   //
   {"RESARE02", RESARE02},   //
   {"RESTRN01", RESTRN01},   // _RESCSP01
   {"SOUNDG02", SOUNDG02},   // _SNDFRM02
   {"TOPMAR01", TOPMAR01},   //
   {"VESSEL01", VESSEL01},   //
   {"VRMEBL01", VRMEBL01},   //
   {"WRECKS02", WRECKS02},   // _DEPVAL01, _QUAPNT01, _SNDFRM02, _UDWHAZ03

   {"########",NULL}
};

#endif
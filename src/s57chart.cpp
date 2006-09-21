/******************************************************************************
 * $Id: s57chart.cpp,v 1.2 2006/09/21 01:37:37 dsr Exp $
 *
 * Project:  OpenCPN
 * Purpose:  S57 Chart Object
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
 * $Log: s57chart.cpp,v $
 * Revision 1.2  2006/09/21 01:37:37  dsr
 * Major refactor/cleanup
 *
 * Revision 1.1.1.1  2006/08/21 05:52:19  dsr
 * Initial import as opencpn, GNU Automake compliant.
 *
 * Revision 1.9  2006/08/04 11:42:02  dsr
 * no message
 *
 * Revision 1.8  2006/07/28 20:46:57  dsr
 * Implement GDAL/OGR library interface
 *
 * Revision 1.7  2006/06/15 02:48:53  dsr
 * Cleanup
 *
 * Revision 1.6  2006/06/02 02:15:05  dsr
 * Capture ATON floating arrays
 *
 * Revision 1.5  2006/05/28 01:46:19  dsr
 * Cleanup
 *
 * Revision 1.4  2006/05/28 00:54:22  dsr
 * Implement PolyGeo
 *
 * Revision 1.3  2006/05/19 19:27:06  dsr
 * Implement  POLYPOLY SENC object definition
 *
 * Revision 1.2  2006/04/23 03:59:50  dsr
 * Implement S57 Query
 *
 * Revision 1.1.1.1  2006/04/19 03:23:28  dsr
 * Rename/Import to OpenCPN
 *
 * Revision 1.12  2006/04/19 00:51:10  dsr
 * Update some georeferencing errors in vector charts
 *
 * Revision 1.11  2006/03/16 03:08:24  dsr
 * Cleanup tabs
 *
 * Revision 1.10  2006/03/13 05:08:35  dsr
 * Implement S57USE_PIXELCACHE
 *
 * Revision 1.9  2006/03/04 21:20:41  dsr
 * Cleanup
 *
 * Revision 1.8  2006/03/01 04:22:32  dsr
 * Correct seteuid logic for SENC and Thumbnail creation
 *
 * Revision 1.7  2006/02/24 18:07:39  dsr
 * Update SENC for MultiPont Soundings, name, dates
 *
 * Revision 1.6  2006/02/24 03:05:41  dsr
 * Explicitely free some S57ClassRegistrar string leaks
 *
 * Revision 1.5  2006/02/23 01:49:47  dsr
 * Cleanup, optimize, increment SENC file format
 *
 *
 *
 */


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/image.h"                           // for some reason, needed for msvc???
#include "wx/tokenzr.h"
#include <wx/textfile.h>

#include "dychart.h"

#include "s52s57.h"
#include "s52plib.h"

#include "s57chart.h"
#include "nmea.h"                                       // for Pause/UnPause

#include "mygeom.h"
#include "cutil.h"

#include "cpl_csv.h"
#include "setjmp.h"

CPL_CVSID("$Id: s57chart.cpp,v 1.2 2006/09/21 01:37:37 dsr Exp $");


void OpenCPN_OGRErrorHandler( CPLErr eErrClass, int nError,
                              const char * pszErrorMsg );               // installed GDAL OGR library error handler

const char *MyCSVGetField( const char * pszFilename,
                         const char * pszKeyFieldName,
                         const char * pszKeyFieldValue,
                         CSVCompareCriteria eCriteria,
                         const char * pszTargetField ) ;



extern s52plib  *ps52plib;

extern int    user_user_id;
extern int    file_user_id;

static jmp_buf env_ogrf;                    // the context saved by setjmp();

#include <wx/arrimpl.cpp>                   // Implement an array of S57 Objects
WX_DEFINE_OBJARRAY(ArrayOfS57Obj);



//----------------------------------------------------------------------------------
//      S57Obj CTOR
//----------------------------------------------------------------------------------

S57Obj::S57Obj()
{
        thisObj = this;
        attList = NULL;
        attVal = NULL;
        ring = NULL;
        pPolyTessGeo = NULL;
        bCS_Added = 0;
        CSrules = NULL;
        FText = NULL;
        bFText_Added = 0;
        OGeo = NULL;
        Scamin = 10000000;                              // ten million enough?
}

//----------------------------------------------------------------------------------
//      S57Obj DTOR
//----------------------------------------------------------------------------------

S57Obj::~S57Obj()
{
}

//----------------------------------------------------------------------------------
//      S57Obj CTOR from SENC file
//----------------------------------------------------------------------------------

S57Obj::S57Obj(char *first_line, wxBufferedInputStream *pfpx)

{
    thisObj = this;
    attList = NULL;
    attVal = NULL;
    ring = NULL;
    pPolyTessGeo = NULL;
    bCS_Added = 0;
    CSrules = NULL;
    FText = NULL;
    bFText_Added = 0;
    OGeo = NULL;
    Scamin = 10000000;                              // ten million enough?

    int FEIndex;


    int MAX_LINE = 499999;
    char *buf = (char *)malloc(MAX_LINE + 1);
    int llmax = 0;


    char szFeatureName[20];

    char *br;
    char szAtt[20];
    char geoMatch[20];

    OGRGeometry     *OGRgeo;
    bool            bMulti;
    OGREnvelope     Envelope;


    char *hdr_buf = (char *)malloc(1);


    if(strlen(first_line) == 0)
        return;
    strcpy(buf, first_line);

//    while(!dun)
    {

        if(!strncmp(buf, "OGRF", 4))
        {
            attList = new wxString();
            attVal =  new wxArrayOfS57attVal();
            geoPt = NULL;
            OGeo = NULL;

            bMulti = false;


            FEIndex = atoi(buf+19);

            strncpy(szFeatureName, buf+11, 6);
            szFeatureName[6] = 0;
            strcpy(FeatureName, szFeatureName);


    //      Build/Maintain a list of found OBJL types for later use
    //      And back-reference the appropriate list index in S57Obj for Display Filtering


            bool bNeedNew = true;
            OBJLElement *pOLE;


            for(unsigned int iPtr = 0 ; iPtr < ps52plib->pOBJLArray->GetCount() ; iPtr++)
            {
                pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));
                if(!strncmp(pOLE->OBJLName, szFeatureName, 6))
                {
                    iOBJL = iPtr;
                    bNeedNew = false;
                    break;
                }
            }

            if(bNeedNew)
            {
                pOLE = (OBJLElement *)malloc(sizeof(OBJLElement));
                strcpy(pOLE->OBJLName, szFeatureName);
                pOLE->nViz = 1;

                ps52plib->pOBJLArray->Add((void *)pOLE);
                iOBJL  = ps52plib->pOBJLArray->GetCount() - 1;
            }


    //      Walk thru the attributes, adding interesting ones
            int hdr_len = 0;
            char *mybuf_ptr;
            char *hdr_end;

            int prim = -1;
            int attdun = 0;

            strcpy(geoMatch, "Dummy");

            while(!attdun)
            {
                if(hdr_len)
                {
                    int nrl = my_bufgetl( mybuf_ptr, hdr_end, buf, MAX_LINE );
                    mybuf_ptr += nrl;
                    if(0 == nrl)
                    {
                        attdun = 1;
                        my_fgets(buf, MAX_LINE, *pfpx);     // this will be PolyGeo
                        break;
                    }
                }

                else
                    my_fgets(buf, MAX_LINE, *pfpx);


                if(!strncmp(buf, "HDRLEN", 6))
                {
                    hdr_len = atoi(buf+7);
                    hdr_buf = (char *)realloc(hdr_buf, hdr_len);
                    pfpx->Read(hdr_buf, hdr_len);
                    mybuf_ptr = hdr_buf;
                    hdr_end = hdr_buf + hdr_len;
                }

                else if(!strncmp(buf, geoMatch, 6))
                {
                    attdun =1;
                    break;
                }

                else if(!strncmp(buf, "  MULT", 6))         // Special multipoint
                {
                    bMulti = true;
                    attdun =1;
                    break;
                }



                else if(!strncmp(buf, "  PRIM", 6))
                {
                    prim = atoi(buf+13);
                    switch(prim)
                    {
                        case 1:
                        {
                            strcpy(geoMatch, "  POIN");
                            break;
                        }

                        case 2:                            // linestring
                        {
                            strcpy(geoMatch, "  LINE");
                            break;
                        }

                        case 3:                            // area as polygon
                        {
                            strcpy(geoMatch, "  POLY");
                            break;
                        }

                        default:                            // unrecognized
                        {
                            break;
                        }

                    }       //switch
                }               // if PRIM


                bool iua = IsUsefulAttribute(buf);

                szAtt[0] = 0;

                if(iua)
                {
                    S57attVal *pattValTmp = new S57attVal;

                    if(buf[10] == 'I')
                    {
                        br = buf+2;
                        int i=0;
                        while(*br != ' ')
                        {
                            szAtt[i++] = *br;
                            br++;
                        }

                        szAtt[i] = 0;

                        while(*br != '=')
                            br++;

                        br += 2;

                        int AValInt = atoi(br);
                        int *pAVI = (int *)malloc(sizeof(int));         //new int;
                        *pAVI = AValInt;
                        pattValTmp->valType = OGR_INT;
                        pattValTmp->value   = pAVI;

    //      Capture SCAMIN on the fly during load
                        if(!strcmp(szAtt, "SCAMIN"))
                            Scamin = AValInt;
                    }


                    else if(buf[10] == 'S')
                    {
                        strncpy(szAtt, &buf[2], 6);
                        szAtt[6] = 0;

                        br = buf + 15;

                        int nlen = strlen(br);
                        br[nlen-1] = 0;                                 // dump the NL char
                        char *pAVS = (char *)malloc(nlen + 1);          ;
                        strcpy(pAVS, br);

                        pattValTmp->valType = OGR_STR;
                        pattValTmp->value   = pAVS;
                    }

    /*
                    else if(strstr(buf, "(S)"))
                    {
                    br = buf+2;
                    int i=0;
                    while(*br != ' ')
                    {
                    szAtt[i++] = *br;
                    br++;
                }

                    szAtt[i] = 0;

                    while(*br != '=')
                    br++;

                    br += 2;

                    int nlen = strlen(br);
                    br[nlen-1] = 0;                                 // dump the NL char
                    char *pAVS = (char *)malloc(nlen + 1);          ;
                    strcpy(pAVS, br);

                    pattValTmp->valType = OGR_STR;
                    pattValTmp->value   = pAVS;
                }
    */

    //                                      else if(strstr(buf, "(R)"))
                    else if(buf[10] == 'R')
                    {
                        br = buf+2;
                        int i=0;
                        while(*br != ' ')
                        {
                            szAtt[i++] = *br;
                            br++;
                        }

                        szAtt[i] = 0;

                        while(*br != '=')
                            br++;

                        br += 2;

                        float AValReal;
                        sscanf(br, "%f", &AValReal);

                        float *pAVR = (float *)malloc(sizeof(float));   //new float;
                        *pAVR = AValReal;
                        pattValTmp->valType = OGR_REAL;
                        pattValTmp->value   = pAVR;
                    }

                    else
                    {
                              // unknown attribute type
    //                        CPLError((CPLErr)0, 0,"Unknown Attribute Type %s", buf);
                    }


                    if(strlen(szAtt))
                    {
                        attList->Append(szAtt);
                        attList->Append('\037');

                        attVal->Add(pattValTmp);
                    }
                    else
                        delete pattValTmp;


                }        //useful

            }               // while attdun



    //              Develop Geometry

            switch(prim)
            {
                case 1:
                {
                    if(!bMulti)
                    {
                        Primitive_type = GEO_POINT;

                        OGRPoint *point = new OGRPoint;

                        my_fgets(buf, MAX_LINE, *pfpx);
                        int wkb_len = atoi(buf+2);
                        pfpx->Read(buf,  wkb_len);

                        point->importFromWkb( (unsigned char *)buf );

                        npt  = 1;
                        x   = point->getX();
                        y   = point->getY();
                        z   = point->getZ();

                        OGRgeo = point;

   //              Setup Envelope
                        OGRgeo->getEnvelope(&Envelope);
                        BBObj.SetMin(Envelope.MinX, Envelope.MinY);
                        BBObj.SetMax(Envelope.MaxX, Envelope.MaxY);


                    }
                    else
                    {
                        Primitive_type = GEO_POINT;

                        OGRMultiPoint *mulpoint = new OGRMultiPoint;

                        my_fgets(buf, MAX_LINE, *pfpx);
                        int wkb_len = atoi(buf+2);
                        pfpx->Read(buf,  wkb_len);


                        mulpoint->importFromWkb( (unsigned char *)buf );

                        OGRgeo = mulpoint;

    //              Setup Envelope
                        OGRgeo->getEnvelope(&Envelope);
                        BBObj.SetMin(Envelope.MinX, Envelope.MinY);
                        BBObj.SetMax(Envelope.MaxX, Envelope.MaxY);
                    }
                    break;
                }

                case 2:                                                                 // linestring
                {
                    Primitive_type = GEO_LINE;


                    my_fgets(buf, MAX_LINE, *pfpx);
                    int sb_len = atoi(buf+2);

                    unsigned char *buft = (unsigned char *)malloc(sb_len);
                    pfpx->Read(buft,  sb_len);

                    npt = *((int *)(buft + 5));

                    geoPt = (pt*)malloc((npt) * sizeof(pt));
                    pt *ppt = geoPt;
                    float *pf = (float *)(buft + 9);

                        // Capture points
                    for(int ip = 0 ; ip < npt ; ip++)
                    {
                        OGRPoint p;
                        ppt->x = *pf++;
                        ppt->y = *pf++;
                        ppt++;
                    }

                        // Capture bbox limits
                    float xmax = *pf++;
                    float xmin = *pf++;
                    float ymax = *pf++;
                    float ymin = *pf;

                    delete buft;

                    BBObj.SetMin(xmin, ymin);
                    BBObj.SetMax(xmax, ymax);

                    OGRgeo = NULL;

                    break;
                }

                case 3:                                                                 // area as polygon
                {
                    Primitive_type = GEO_AREA;

                    int ll = strlen(buf);
                    if(ll > llmax)
                        llmax = ll;

/*
                    if(!strncmp(buf, "  POLYGEO", 9))
                    {
                        int nrecl;
                        sscanf(buf, "  POLYGEO %d", &nrecl);

                        if (nrecl)
                        {
                            unsigned char *polybuf = (unsigned char *)malloc(nrecl + 1);
                            pfpx->Read(polybuf,  nrecl);
                            polybuf[nrecl] = 0;                     // endit
                            PolyGeo *ppg = new PolyGeo(polybuf, nrecl, FEIndex);
                            free(polybuf);

                            pPolyGeo = ppg;
                            ring = NULL;
                            BBObj.SetMin(ppg->Get_xmin(), ppg->Get_ymin());
                            BBObj.SetMax(ppg->Get_xmax(), ppg->Get_ymax());

                        }
                    }
*/
                    if(!strncmp(buf, "  POLYTESSGEO", 13))
                    {
                        int nrecl;
                        sscanf(buf, "  POLYTESSGEO %d", &nrecl);

                        if (nrecl)
                        {
                            unsigned char *polybuf = (unsigned char *)malloc(nrecl + 1);
                            pfpx->Read(polybuf,  nrecl);
                            polybuf[nrecl] = 0;                     // endit
                            PolyTessGeo *ppg = new PolyTessGeo(polybuf, nrecl, FEIndex);
                            free(polybuf);

                            pPolyTessGeo = ppg;
                            ring = NULL;
                            BBObj.SetMin(ppg->Get_xmin(), ppg->Get_ymin());
                            BBObj.SetMax(ppg->Get_xmax(), ppg->Get_ymax());

                        }
                    }

                    OGRgeo = NULL;

                    break;
                }
            }       //switch




            if(prim > 0)
            {

                OGeo = OGRgeo;                                     // store the OGRGeometry in the s57obj

                Index = FEIndex;
            }
        }               //OGRF
    }                       //while(!dun)

    free( buf );
    free(hdr_buf);

}

//-------------------------------------------------------------------------------------------
//      Attributes in SENC file may not be needed, and can be safely ignored when creating S57Obj
//      Look at a buffer, and return true or false according to a (default) definition
//-------------------------------------------------------------------------------------------

bool S57Obj::IsUsefulAttribute(char *buf)
{

    if(!strncmp(buf, "HDRLEN", 6))
        return false;

//      Dump the first 8 standard attributes
    /* -------------------------------------------------------------------- */
    /*      RCID                                                            */
    /* -------------------------------------------------------------------- */
    if(!strncmp(buf+2, "RCID", 4))
        return false;

    /* -------------------------------------------------------------------- */
    /*      LNAM                                                            */
    /* -------------------------------------------------------------------- */
    if(!strncmp(buf+2, "LNAM", 4))
        return false;

    /* -------------------------------------------------------------------- */
    /*      PRIM                                                            */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf+2, "PRIM", 4))
        return false;

    /* -------------------------------------------------------------------- */
    /*      SORDAT                                                          */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf+2, "SORDAT", 6))
        return false;

    /* -------------------------------------------------------------------- */
    /*      SORIND                                                          */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf+2, "SORIND", 6))
        return false;

    //      All others are "Useful"
    else if(1)
        return true;

    /* -------------------------------------------------------------------- */
    /*      GRUP                                                            */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf, "  GRUP", 6))
        return false;

    /* -------------------------------------------------------------------- */
    /*      OBJL                                                            */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf, "  OBJL", 6))
        return false;

    /* -------------------------------------------------------------------- */
    /*      RVER                                                            */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf, "  RVER", 6))
        return false;

    /* -------------------------------------------------------------------- */
    /*      AGEN                                                            */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf, "  AGEN", 6))
        return false;

    /* -------------------------------------------------------------------- */
    /*      FIDN                                                            */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf, "  FIDN", 6))
        return false;

    /* -------------------------------------------------------------------- */
    /*      FIDS                                                            */
    /* -------------------------------------------------------------------- */
    else if(!strncmp(buf, "  FIDS", 6))
        return false;

//      UnPresent data
    else if(strstr(buf, "(null)"))
        return false;

    else
        return true;
}

//------------------------------------------------------------------------------
//      Local version of fgets for Binary Mode (SENC) file
//------------------------------------------------------------------------------
 int S57Obj::my_fgets( char *buf, int buf_len_max, wxBufferedInputStream& ifs )

{
    char        chNext;
    int         nLineLen = 0;
    char            *lbuf;

    lbuf = buf;


    while( !ifs.Eof() && nLineLen < buf_len_max )
    {
        chNext = ifs.GetC();

        /* each CR/LF (or LF/CR) as if just "CR" */
        if( chNext == 10 || chNext == 13 )
        {
            chNext = '\n';
        }

        *lbuf = chNext; lbuf++, nLineLen++;

        if( chNext == '\n' )
        {
            *lbuf = '\0';
            return nLineLen;
        }
    }

    *(lbuf) = '\0';

    return nLineLen;
}

//------------------------------------------------------------------------------
//      Local version of bufgetl for Binary Mode (SENC) file
//------------------------------------------------------------------------------
 int S57Obj::my_bufgetl( char *ib_read, char *ib_end, char *buf, int buf_len_max )
{
    char        chNext;
    int         nLineLen = 0;
    char        *lbuf;
    char        *ibr = ib_read;

    lbuf = buf;


    while( (nLineLen < buf_len_max) && (ibr < ib_end))
    {
        chNext = *ibr++;

        /* each CR/LF (or LF/CR) as if just "CR" */
        if( chNext == 10 || chNext == 13 )
            chNext = '\n';

        *lbuf++ = chNext;
        nLineLen++;

        if( chNext == '\n' )
        {
            *lbuf = '\0';
            return nLineLen;
        }
    }

    *(lbuf) = '\0';
    return nLineLen;
}




//----------------------------------------------------------------------------------
//      s57chart Implementation
//----------------------------------------------------------------------------------

s57chart::s57chart()
{

    ChartType = CHART_TYPE_S57;

    for(int i=0 ; i<PRIO_NUM ; i++)
            for(int j=0 ; j<LUPNAME_NUM ; j++)
                    razRules[i][j] = NULL;

    NativeScale = 1.0;                              // Will be fetched during Init()

    pDIB = NULL;

    pFullPath = NULL;

    pFloatingATONArray = NULL;
    pRigidATONArray = NULL;

    tmpup_array = NULL;
    m_pcsv_locn = NULL;

    wxString csv_dir;
    if(wxGetEnv("S57_CSV", &csv_dir))
        m_pcsv_locn = new wxString(csv_dir);

    wxString *test;
    wxString cat("SCAMIN");
    test = GetAttributeDecode(cat, 35000);

}

s57chart::~s57chart()
{

    S57_done();

    delete pDIB;

    delete pFullPath;

    delete pFloatingATONArray;
    delete pRigidATONArray;

    delete m_pcsv_locn;


}



void s57chart::GetValidCanvasRegion(const ViewPort& VPoint, wxRegion *pValidRegion)
{
        int rxl, rxr;
        rxl = 0;
        rxr = VPoint.pix_width;

        int ryb, ryt;
        ryt = 0;
        ryb = VPoint.pix_height;

        pValidRegion->Clear();
        pValidRegion->Union(rxl, ryt, rxr - rxl, ryb - ryt);

}

void s57chart::SetColorScheme(ColorScheme cs, bool bApplyImmediate)
{
      m_color_scheme = cs;

      if(bApplyImmediate)
            InvalidateCache();
}


int s57chart::S57_done()
{


//      Delete the created S57Objs
        ObjRazRules *top;
        ObjRazRules *nxx;
        for (int i=0; i<PRIO_NUM; ++i)
        {
                for(int j=0 ; j<LUPNAME_NUM ; j++)
                {

                        top = razRules[i][j];
                        while ( top != NULL)
                        {
                                S57Obj *po = top->obj;
                                nxx  = top->next;

                                S57_freeObj(po);
                                free(top);
                                top = nxx;
                        }
                }
        }


        return 1;
}

int s57chart::S57_freeObj(S57Obj *obj)
{
        for(unsigned int iv = 0 ; iv < obj->attVal->GetCount() ; iv++)
        {
                S57attVal *vv =  obj->attVal->Item(iv);
                void *v2 = vv->value;
                free(v2);
                delete vv;
        }

        delete obj->attVal;

        delete obj->attList;


        delete obj->pPolyTessGeo;


       while (obj->ring)
       {
                    S57Obj *tmpObj = obj->ring;
                    obj->ring = tmpObj->ring;


                    free(tmpObj);
       }

           delete obj->OGeo;


   if(obj->FText)
   {
           delete obj->FText->frmtd;    // the formatted text string
           free(obj->FText);
   }

   if(obj->geoPt)
                free(obj->geoPt);

   free(obj);

   return 1;
}


//-----------------------------------------------------------------------
//              Pixel to Lat/Long Conversion helpers
//-----------------------------------------------------------------------

#ifdef __WXMSW__
double round(double x)
{
    return x;
}
#endif

void s57chart::GetPointPix(float rlat, float rlon, wxPoint *r)
{
        GetPointPixEst(rlat, rlon, r);
}

void s57chart::GetPointPixEst(float rlat, float rlon, wxPoint *r)
{
//        r->x = (int)(((rlon - lon_left) * pix_per_deg_lon) + 0.5);
//        r->y = (int)(((lat_top - rlat) * pix_per_deg_lat) + 0.5);
        r->x = (int)round((rlon - lon_left) * pix_per_deg_lon);
        r->y = (int)round((lat_top - rlat) * pix_per_deg_lat);

}

void s57chart::pix_to_latlong(int pixx, int pixy, double *plat, double *plon)
{
        float lat = lat_top      - pixy / pix_per_deg_lat;
        float lon = lon_left + pixx / pix_per_deg_lon;

        *plat = lat;
        *plon = lon;
}

void s57chart::latlong_to_pix(double lat, double lon, int &pixx, int &pixy)
{
        wxPoint r;
        GetPointPixEst(lat, lon, &r);

        pixx = r.x;
        pixy = r.y;

}


void s57chart::latlong_to_pix_vp(double lat, double lon, int &pixx, int &pixy, ViewPort& vp)
{

//      pixx = (int)(((lon - vp.lon_left) * vp.ppd_lon) + 0.5);
//      pixy = (int)(((vp.lat_top - lat)  * vp.ppd_lat) + 0.5);
    pixx = (int)round((lon - vp.lon_left) * vp.ppd_lon);
    pixy = (int)round((vp.lat_top - lat)  * vp.ppd_lat);

}

void s57chart::vp_pix_to_latlong(ViewPort& vp, int pixx, int pixy, double *plat, double *plon)
{
//        float pix_per_deg_lon = vp.pix_width / (vp.lon_right - vp.lon_left);
//        float pix_per_deg_lat = vp.pix_height / (vp.lat_top - vp.lat_bot);
//        printf("57 ppd %f %f\n",pix_per_deg_lon, pix_per_deg_lat);

//        float lat_top  = vp.lat_top;
//        float lon_left = vp.lon_left;

//        double pix_per_deg_lon = vp.ppd_lon;
//        double pix_per_deg_lat = vp.ppd_lat;

        double lat = vp.lat_top  - pixy / vp.ppd_lat;
        double lon = vp.lon_left + pixx / vp.ppd_lon;

        *plat = lat;
        *plon = lon;

}


//-----------------------------------------------------------------------
//              Calculate and Set ViewPoint Constants
//-----------------------------------------------------------------------

void s57chart::SetVPParms(ViewPort *vpt)
{
        //      Todo This is silly... Use the vp directly
        pix_per_deg_lon = vpt->ppd_lon;
        pix_per_deg_lat = vpt->ppd_lat;

        lat_top    = vpt->lat_top;
        lon_left   = vpt->lon_left;


}

/*
int rftoi(float x)
{
        if(x < 0.0)
                return (int)(x - 0.5);
        else
                return (int)(x + 0.5);
}
*/

ThumbData *s57chart::GetThumbData(int tnx, int tny, float lat, float lon)
{

//      Plot the passed lat/lon at the thumbnail bitmap scale
        if( pThumbData->pDIB)
        {

                float lat_top =   FullExtent.NLAT;
                float lat_bot =   FullExtent.SLAT;
                float lon_left =  FullExtent.WLON;
                float lon_right = FullExtent.ELON;

                // Build the scale factors just as the thumbnail was built
                float ext_max = fmax((lat_top - lat_bot), (lon_right - lon_left));

                float pix_per_deg_lat = pThumbData->pDIB->GetHeight()/ ext_max;
                float pix_per_deg_lon = pix_per_deg_lat;


                pThumbData->ShipX = (int)(((lon - lon_left) * pix_per_deg_lon) + 0.5);
                pThumbData->ShipY = (int)(((lat_top - lat)  * pix_per_deg_lat) + 0.5);
        }
        else
        {
                pThumbData->ShipX = 0;
                pThumbData->ShipY = 0;
        }

        return pThumbData;
}

void s57chart::SetFullExtent(Extent& ext)
{
  FullExtent.NLAT = ext.NLAT;
  FullExtent.SLAT = ext.SLAT;
  FullExtent.WLON = ext.WLON;
  FullExtent.ELON = ext.ELON;
}


void s57chart::RenderViewOnDC(wxMemoryDC& dc, ViewPort& VPoint, ScaleTypeEnum scale_type)
{
    ps52plib->SetColorScheme((Col_Scheme_t)m_color_scheme);

    DoRenderViewOnDC(dc, VPoint, DC_RENDER_ONLY, NULL, NULL);
}



void s57chart::DoRenderViewOnDC(wxMemoryDC& dc, ViewPort& VPoint,
                        RenderTypeEnum option,
                        wxBitmap **ppDIB, wxImage **ppImg)
{
        wxPoint rul, rlr;

//        int bpp = BPP;          // use global value

        bool bNewVP = false;


        //Todo Add a check on last_vp.bValid
        if(VPoint.view_scale != last_vp.view_scale)
        {
            bNewVP = true;
                if(pDIB)
                {
                        delete pDIB;
                        pDIB = NULL;
                }
        }

//      Calculate the desired rectangle in the last cached image space
        rul.x = (int)round((VPoint.lon_left - last_vp.lon_left) * pix_per_deg_lon);
        rul.y = (int)round((last_vp.lat_top - VPoint.lat_top) * pix_per_deg_lat);

        rlr.x = (int)round((VPoint.lon_right - last_vp.lon_left) * pix_per_deg_lon);
        rlr.y = (int)round((last_vp.lat_top - VPoint.lat_bot) * pix_per_deg_lat);

        if((rul.x != 0) || (rul.y != 0))
                bNewVP = true;


        if(bNewVP || (pDIB == NULL))
        {
            if(pDIB)
            {
//      Using regions, calculate re-usable area of pDIB

                wxRegion rgn_last(0, 0, VPoint.pix_width, VPoint.pix_height);
                wxRegion rgn_new(rul.x, rul.y, rlr.x - rul.x, rlr.y - rul.y);

//      Get intersection

                rgn_last.Intersect(rgn_new);

                if(!rgn_last.IsEmpty())
                {

#ifdef S57USE_PIXELCACHE
                    dyMemDC dc_last;
                    pDIB->SelectIntoDC(dc_last);
                    dyMemDC dc_new;

                    PixelCache *pDIBNew = new PixelCache(VPoint.pix_width, VPoint.pix_height, BPP);
                    pDIBNew->SelectIntoDC(dc_new);
#else
                    wxMemoryDC dc_last;
                    wxMemoryDC dc_new;

                    dc_last.SelectObject(*pDIB);

#ifdef dyUSE_BITMAPO_S57
                    wxBitmapo *pDIBNew = new wxBitmapo((void *)NULL,
                            VPoint.pix_width, VPoint.pix_height, BPP);
#else
                    wxBitmap *pDIBNew = new wxBitmap(
                            VPoint.pix_width, VPoint.pix_height, BPP);
#endif
                    dc_new.SelectObject(*pDIBNew);
#endif

                    int xu, yu, wu, hu;
                    rgn_last.GetBox(xu, yu, wu, hu);

                    int desx = 0;
                    int desy = 0;
                    int srcx = xu;
                    int srcy = yu;

                    if(rul.x < 0)
                    {
                            srcx = 0;
                            desx = -rul.x;
                    }
                    if(rul.y < 0)
                    {
                            srcy = 0;
                            desy = -rul.y;
                    }


                    dc_new.Blit(desx, desy, wu, hu, (wxDC *)&dc_last, srcx, srcy);

                    dc_new.SelectObject(wxNullBitmap);
                    dc_last.SelectObject(wxNullBitmap);

                    delete pDIB;
                    pDIB = pDIBNew;

//              OK, now have the re-useable section in place
//              Next, build the new sections


#ifdef S57USE_PIXELCACHE
                    pDIB->SelectIntoDC(dc);
#else
                    dc.SelectObject(*pDIB);                         // ready to render
#endif

                    wxRegion rgn_delta(0, 0, VPoint.pix_width, VPoint.pix_height);
                                    wxRegion rgn_reused(desx, desy, wu, hu);
                                    rgn_delta.Subtract(rgn_reused);

                    wxRegionIterator upd(rgn_delta); // get the update rect list
                    while (upd)
                    {
                        wxRect rect = upd.GetRect();


//      Build temp ViewPort on this region

                        ViewPort temp_vp = VPoint;

                        temp_vp.lat_top = last_vp.lat_top - (rul.y / pix_per_deg_lat) - (rect.y / pix_per_deg_lat);
                        temp_vp.lat_bot = temp_vp.lat_top - (rect.height / pix_per_deg_lat);

                        temp_vp.lon_left  = last_vp.lon_left + (rul.x / pix_per_deg_lon) + (rect.x / pix_per_deg_lon);
                        temp_vp.lon_right = temp_vp.lon_left + (rect.width / pix_per_deg_lon);

                        temp_vp.vpBBox.SetMin(temp_vp.lon_left, temp_vp.lat_bot);
                        temp_vp.vpBBox.SetMax(temp_vp.lon_right, temp_vp.lat_top);

//      Update the helper parameters
                        lon_left = last_vp.lon_left + (rul.x / pix_per_deg_lon);
                        lat_top = last_vp.lat_top - (rul.y / pix_per_deg_lat);

//      And Render it new piece on the target dc
//     wxLogMessage("Reuse, rendering %d %d %d %d ", rect.x, rect.y, rect.width, rect.height);
                        DCRender(dc, temp_vp, &rect);

                        upd ++ ;
                    }


                    dc.SelectObject(wxNullBitmap);

                }       // if rgn empty

                else                            // cannot reuse anything
                {
#ifdef S57USE_PIXELCACHE
                    delete pDIB;
                    pDIB = new PixelCache(VPoint.pix_width, VPoint.pix_height, BPP);     // destination
                    pDIB->SelectIntoDC(dc);

#else
                    delete pDIB;
#ifdef dyUSE_BITMAPO_S57
                    pDIB = new wxBitmapo((void *)NULL,
                               VPoint.pix_width, VPoint.pix_height, BPP);
#else
                    pDIB = new wxBitmap(
                               VPoint.pix_width, VPoint.pix_height, BPP);
#endif
                    dc.SelectObject(*pDIB);

#endif


                    DCRender(dc, VPoint, NULL);

                    dc.SelectObject(wxNullBitmap);
                }


            }   // if pDIB

            else
            {
#ifdef S57USE_PIXELCACHE
              pDIB = new PixelCache(VPoint.pix_width, VPoint.pix_height, BPP);     // destination
              pDIB->SelectIntoDC(dc);

#else

#ifdef dyUSE_BITMAPO_S57
              pDIB = new wxBitmapo((void *)NULL,VPoint.pix_width, VPoint.pix_height, BPP);
#else
              pDIB = new wxBitmap(VPoint.pix_width, VPoint.pix_height, BPP);
#endif
              dc.SelectObject(*pDIB);
#endif

              DCRender(dc, VPoint, NULL);
              dc.SelectObject(wxNullBitmap);
            }

//      Update last_vp to reflect the current cached bitmap

        double dy = (last_vp.lat_top - lat_top);
        dy *= pix_per_deg_lat;
        last_vp = VPoint;

        double pwidth = VPoint.pix_width;
        double pheight = VPoint.pix_height;

        last_vp.lon_left  = lon_left;
        last_vp.lon_right = lon_left + (pwidth /pix_per_deg_lon);
        last_vp.lat_top = lat_top;
        last_vp.lat_bot = lat_top - (pheight/pix_per_deg_lat);


        }       //if NEWVP


#ifdef S57USE_PIXELCACHE
        pDIB->SelectIntoDC(dc);
#else
        dc.SelectObject(*pDIB);
#endif

}

int s57chart::DCRender(wxDC& dcinput, ViewPort& vp, wxRect* rect)
{

        int i;
        ObjRazRules *top;
        ObjRazRules *crnt;

        wxStopWatch st0;

#ifdef S57USE_PIXELCACHE
        PixelCache *pDIBRect;
#endif
        render_canvas_parms pb_spec;

//      Get some heap memory for the area renderer

      pb_spec.depth = BPP;                              // set the depth

        if(rect)
        {
                pb_spec.pb_pitch = ((rect->width * pb_spec.depth / 8 ));
                pb_spec.lclip = rect->x;
                pb_spec.rclip = rect->x + rect->width - 1;
#ifdef S57USE_PIXELCACHE
                pDIBRect = new PixelCache(rect->width, rect->height, pb_spec.depth);
                pb_spec.pix_buff = pDIBRect->pData;
#else
                pb_spec.pix_buff = (unsigned char *)malloc(rect->height * pb_spec.pb_pitch);
                if(NULL == pb_spec.pix_buff)
                    wxLogMessage("PixBuf NULL 1");

#endif
                // Preset background
                memset(pb_spec.pix_buff, 0,rect->height * pb_spec.pb_pitch);
                pb_spec.mask_buff = (unsigned char *)malloc(rect->height * pb_spec.pb_pitch);
                pb_spec.width = rect->width;
                pb_spec.height = rect->height;
                pb_spec.x = rect->x;
                pb_spec.y = rect->y;
        }
        else
        {
                pb_spec.pb_pitch = ((vp.pix_width * pb_spec.depth / 8 )) ;
                pb_spec.lclip = 0;
                pb_spec.rclip = vp.pix_width-1;
#ifdef S57USE_PIXELCACHE
                pb_spec.pix_buff = pDIB->pData;
#else
                pb_spec.pix_buff = (unsigned char *)malloc(vp.pix_height * pb_spec.pb_pitch);
                if(NULL == pb_spec.pix_buff)
                    wxLogMessage("PixBuf NULL 2");
#endif
                // Preset background
                memset(pb_spec.pix_buff, 0,vp.pix_height * pb_spec.pb_pitch);
                pb_spec.mask_buff = (unsigned char *)malloc(vp.pix_height * pb_spec.pb_pitch);
                pb_spec.width = vp.pix_width;
                pb_spec.height  = vp.pix_height;
                pb_spec.x = 0;
                pb_spec.y = 0;
        }

//      Render the areas quickly
    for (i=0; i<PRIO_NUM; ++i)
    {
                top = razRules[i][4];           // Area Symbolized Boundaries
                while ( top != NULL)
                {
                        crnt = top;
                        top  = top->next;               // next object
                        ps52plib->RenderArea(&dcinput, crnt, &vp, &pb_spec);
                }

                top = razRules[i][3];           // Area Plain Boundaries
                while ( top != NULL)
                {
                        crnt = top;
                        top  = top->next;               // next object
                        ps52plib->RenderArea(&dcinput, crnt, &vp, &pb_spec);
                }
    }

#ifdef S57USE_PIXELCACHE
      if(rect)
      {

//  Map PixelCache PCRect into a temporary DC
#ifdef __PIX_CACHE_DIBSECTION__
        dyMemDC dc_ren;
#else
        wxMemoryDC dc_ren;
#endif
        pDIBRect->SelectIntoDC(dc_ren);

//    Blit it onto the target dc
        dcinput.Blit(pb_spec.x, pb_spec.y, pb_spec.width, pb_spec.height,
                     (wxDC *)&dc_ren, 0,0);

//    And clean up the mess
        dc_ren.SelectObject(wxNullBitmap);

        delete pDIBRect;
      }

      free(pb_spec.mask_buff);      // must directly free the mask



#else       // not S57USE_PIXELCACHE



//      Convert the Private render canvas into a bitmap
#ifdef dyUSE_BITMAPO_S57
        wxBitmapo *pREN = new wxBitmapo(pb_spec.pix_buff,
                                                pb_spec.width, pb_spec.height, pb_spec.depth);
#else
        wxImage *prender_image = new wxImage(pb_spec.width, pb_spec.height);
        if(!prender_image->Ok())
            wxLogMessage("imagenok0");
        prender_image->SetData((unsigned char*)pb_spec.pix_buff);
        if(!prender_image->Ok())
            wxLogMessage("imagenok1");

        wxBitmap *pREN = new wxBitmap(*prender_image);
        if(!pREN->Ok())
            wxLogMessage("bitmapnok0");

#endif

//      Map it into a temporary DC
        wxMemoryDC dc_ren;
        dc_ren.SelectObject(*pREN);


//      Blit it onto the target dc
        bool b = dcinput.Blit(pb_spec.x, pb_spec.y, pb_spec.width, pb_spec.height,
            (wxDC *)&dc_ren, 0,0);
        if(b != true)
        {
            wxLogMessage("s57blit1false");
            if(!dcinput.Ok())
                wxLogMessage("dcinok");
            if(!dc_ren.Ok())
                wxLogMessage("dcrenok");
        }

//            wxLogMessage("57Blit %d %d %d %d",pb_spec.x, pb_spec.y, pb_spec.width, pb_spec.height);


//      And clean up the mess
        dc_ren.SelectObject(wxNullBitmap);


#ifdef dyUSE_BITMAPO_S57
        free(pb_spec.pix_buff);
        free(pb_spec.mask_buff);
#else
        delete prender_image;           // the image owns the data
                                        // and so will free it in due course
        free(pb_spec.mask_buff);        // must directly free the mask
#endif

        delete pREN;

#endif      //S57USE_PIXELCACHE

    st0.Pause();
//    printf("Render Areas                  %ldms\n", st0.Time());

//      Render the rest of the objects/primitives

    wxStopWatch stlines;
    stlines.Pause();
    wxStopWatch stsim_pt;
    stsim_pt.Pause();
    wxStopWatch stpap_pt;
    stpap_pt.Pause();
    wxStopWatch stasb;
    stasb.Pause();
    wxStopWatch stapb;
    stapb.Pause();

        for (i=0; i<PRIO_NUM; ++i)
        {
//      Set up a Clipper for Lines
                wxDCClipper *pdcc = NULL;
                if(rect)
                {
                        wxRect nr = *rect;
                        pdcc = new wxDCClipper(dcinput, nr);
                }

                stlines.Resume();
                top = razRules[i][2];           //LINES
                while ( top != NULL)
                {
                        ObjRazRules *crnt = top;
                        top  = top->next;
                        ps52plib->_draw(&dcinput, crnt, &vp);


                }
//      Destroy Clipper
                if(pdcc)
                        delete pdcc;
                stlines.Pause();

                stsim_pt.Resume();
                top = razRules[i][0];           //SIMPLIFIED Points
                while ( top != NULL)
                {
                        crnt = top;
                        top  = top->next;
                        ps52plib->_draw(&dcinput, crnt, &vp);

                }
                stsim_pt.Pause();



                stpap_pt.Resume();
                top = razRules[i][1];           //Paper Chart Points Points
                while ( top != NULL)
                {
                        crnt = top;
                        top  = top->next;
                        ps52plib->_draw(&dcinput, crnt, &vp);

                }
                stpap_pt.Pause();

                stasb.Resume();
                top = razRules[i][4];           // Area Symbolized Boundaries
                while ( top != NULL)
                {
                        crnt = top;
                        top  = top->next;               // next object
                        ps52plib->_draw(&dcinput, crnt, &vp);
                }
                stasb.Pause();

                stapb.Resume();
                top = razRules[i][3];           // Area Plain Boundaries
                while ( top != NULL)
                {
                        crnt = top;
                        top  = top->next;               // next object
                        ps52plib->_draw(&dcinput, crnt, &vp);
                }
                stapb.Pause();

        }

/*
        printf("Render Lines                  %ldms\n", stlines.Time());
        printf("Render Simple Points          %ldms\n", stsim_pt.Time());
        printf("Render Paper Points           %ldms\n", stpap_pt.Time());
        printf("Render Symbolized Boundaries  %ldms\n", stasb.Time());
        printf("Render Plain Boundaries       %ldms\n\n", stapb.Time());
*/
        return 1;
}





InitReturn s57chart::Init( const wxString& name, ChartInitFlag flags, ColorScheme cs )
{
    pFullPath = new wxString(name);


// Look for Thumbnail
        wxFileName ThumbFileNameLook(name);
        ThumbFileNameLook.SetExt("BMP");

        wxBitmap *pBMP;
        if(ThumbFileNameLook.FileExists())
        {
#ifdef dyUSE_BITMAPO_S57
                pBMP =  new wxBitmapo;
#else
                pBMP =  new wxBitmap;
#endif
                pBMP->LoadFile(ThumbFileNameLook.GetFullPath(), wxBITMAP_TYPE_BMP );
                pThumbData->pDIB = pBMP;

        }

        if(flags == THUMB_ONLY)
                return INIT_OK;

 // Really can only Init and use S57 chart if the S52 Presentation Library is OK
    if(!ps52plib->m_bOK)
      return INIT_FAIL_REMOVE;

        int build_ret_val = 1;

        bool bbuild_new_senc = false;

//      Look for S57 file in the target directory
        pS57FileName = new wxFileName(name);
        pS57FileName->SetExt("S57");

        if(pS57FileName->FileExists())
        {
                wxFile f;
                if(f.Open(pS57FileName->GetFullPath()))
                {
                        if(f.Length() == 0)
                        {
                                f.Close();
                                build_ret_val = BuildS57File( name.c_str() );
                        }
                        else                                      // file exists, non-zero
                        {                                         // so check for new updates

                                f.Seek(0);
                                wxFileInputStream *pfpx_u = new wxFileInputStream(f);
                                wxBufferedInputStream *pfpx = new wxBufferedInputStream(*pfpx_u);
                                int dun = 0;
                                int last_update = 0;
                                int senc_file_version = 0;
                                int force_make_senc = 0;
                                char buf[256];
                                char *pbuf = buf;

                                while( !dun )
                                {
                                        if(my_fgets(pbuf, 256, *pfpx) == 0)
                                        {
                                                dun = 1;
                                                force_make_senc = 1;
                                                break;
                                        }
                                        else
                                        {
                                                if(!strncmp(pbuf, "OGRF", 4))
                                                {
                                                        dun = 1;
                                                        break;
                                                }
                                                else if(!strncmp(pbuf, "UPDT", 4))
                                                {
                                                        sscanf(pbuf, "UPDT=%i", &last_update);
                                                }
                                                else if(!strncmp(pbuf, "SENC", 4))
                                                {
                                                    sscanf(pbuf, "SENC Version=%i", &senc_file_version);
                                                }
                                        }
                                }

                                delete pfpx;
                                delete pfpx_u;
                                f.Close();
//              Anything to do?

 //force_make_senc = 1;
                                wxString DirName(pS57FileName->GetPath((int)wxPATH_GET_SEPARATOR));
                                int most_recent_update_file = GetUpdateFileArray(DirName, NULL);

                                if(last_update != most_recent_update_file)
                                {
                                    bool bupdate_possible = s57_ddfrecord_test();
                                    bbuild_new_senc = bupdate_possible;
                                }

                                else if(senc_file_version != CURRENT_SENC_FORMAT_VERSION)
                                    bbuild_new_senc = true;

                                else if(force_make_senc)
                                    bbuild_new_senc = true;

                                if(bbuild_new_senc)
                                      build_ret_val = BuildS57File( name.c_str() );


                        }
                }
        }

        else                    // SENC file does not exist
        {
                build_ret_val = BuildS57File( name.c_str() );
                bbuild_new_senc = true;
        }

        if(0 == build_ret_val)
                return INIT_FAIL_RETRY;


        BuildRAZFromS57File( (pS57FileName->GetFullPath()).c_str() );


        //      Check for and if necessary build Thumbnail
        wxFileName ThumbFileName(name);
        ThumbFileName.SetExt("BMP");

      if(!ThumbFileName.FileExists() || bbuild_new_senc)
        {
#define S57_THUMB_SIZE  200
                wxMemoryDC memdc;

                //      Set up a private ViewPort
                ViewPort vp;

                vp.clon = (FullExtent.ELON + FullExtent.WLON) / 2.;
                vp.clat = (FullExtent.NLAT + FullExtent.SLAT) / 2.;
                vp.lat_top =   FullExtent.NLAT;
                vp.lat_bot =   FullExtent.SLAT;
                vp.lon_left =  FullExtent.WLON;
                vp.lon_right = FullExtent.ELON;

                float ext_max = fmax((vp.lat_top - vp.lat_bot), (vp.lon_right - vp.lon_left));

                vp.ppd_lat = S57_THUMB_SIZE/ ext_max;
                vp.ppd_lon = vp.ppd_lat;

                vp.pix_height = S57_THUMB_SIZE;
                vp.pix_width  = S57_THUMB_SIZE;

                vp.vpBBox.SetMin(vp.lon_left, vp.lat_bot);
                vp.vpBBox.SetMax(vp.lon_right, vp.lat_top);

                vp.chart_scale = 10000000 - 1;
                vp.bValid = true;
                //Todo this becomes last_vp.bValid = false;
                last_vp.view_scale = -1;                // cause invalidation of cache
                SetVPParms(&vp);


//      Borrow the OBJLArray temporarily to set the object type visibility for this render
//      First, make a copy for the curent OBJLArray viz settings, setting current value to invisible

                unsigned int OBJLCount = ps52plib->pOBJLArray->GetCount();
                int *psave_viz = new int[OBJLCount];
                int *psvr = psave_viz;
                OBJLElement *pOLE;
                unsigned int iPtr;

                for(iPtr = 0 ; iPtr < OBJLCount ; iPtr++)
                {
                        pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));
                        *psvr++ = pOLE->nViz;
                        pOLE->nViz = 0;
                }

//      Now, set up what I want for this render
                for(iPtr = 0 ; iPtr < OBJLCount ; iPtr++)
                {
                        pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));
                        if(!strncmp(pOLE->OBJLName, "LNDARE", 6))
                                pOLE->nViz = 1;
                        if(!strncmp(pOLE->OBJLName, "DEPARE", 6))
                                pOLE->nViz = 1;
                }

//      Use display category MARINERS_STANDARD to force use of OBJLArray
                DisCat dsave = ps52plib->m_nDisplayCategory;
                ps52plib->m_nDisplayCategory = MARINERS_STANDARD;

//      Do the render
                DoRenderViewOnDC(memdc, vp, DC_RENDER_ONLY, NULL, NULL);

//      Release the DIB
                memdc.SelectObject(wxNullBitmap);

//      Restore the plib to previous state
                psvr = psave_viz;
                for(iPtr = 0 ; iPtr < OBJLCount ; iPtr++)
                {
                        pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));
                        pOLE->nViz = *psvr++;
                }

                ps52plib->m_nDisplayCategory = dsave;

                delete psave_viz;

//      Clone pDIB into pThumbData;
                wxBitmap *pBMP;

#ifdef dyUSE_BITMAPO_S57
                pBMP = new wxBitmapo((void *)NULL,
                vp.pix_width, vp.pix_height, BPP);
#else
                pBMP = new wxBitmap(/*NULL,*/
                        vp.pix_width, vp.pix_height, BPP);
#endif
                wxMemoryDC dc_clone;
                dc_clone.SelectObject(*pBMP);

                wxMemoryDC dc_org;
#ifdef S57USE_PIXELCACHE
                pDIB->SelectIntoDC(dc_org);
#else
                dc_org.SelectObject(*pDIB);
#endif

                dc_clone.Blit(0,0,vp.pix_width, vp.pix_height,
                              (wxDC *)&dc_org, 0,0);

                dc_clone.SelectObject(wxNullBitmap);
                dc_org.SelectObject(wxNullBitmap);

//    May Need root to create the Thumbnail file
#ifndef __WXMSW__
                seteuid(file_user_id);
#endif
                pBMP->SaveFile(ThumbFileName.GetFullPath(), wxBITMAP_TYPE_BMP);


 //   Return to default user priveleges
#ifndef __WXMSW__
                seteuid(user_user_id);
#endif

        }
        delete pS57FileName;

        m_color_scheme = cs;
        bReadyToRender = true;

        return INIT_OK;
}

void s57chart::InvalidateCache()
{
        if(pDIB)
        {
                delete pDIB;
                pDIB = NULL;
        }

}


void s57chart::GetChartNameFromTXT(const wxString& FullPath, wxString &Name)
{

      wxFileName fn(FullPath);

      wxString target_name = fn.GetName();
 //     target_name.SetChar(target_name.Len()-1, 'E');
      target_name.RemoveLast();                             // Todo is this OK to use, eg US2EC03 ??

      wxString dir_name = fn.GetPath();

      wxDir dir(dir_name);                                  // The directory containing the file

      wxArrayString *pFileList = new wxArrayString();

      dir.GetAllFiles(fn.GetPath(), pFileList);             // list all the files

      //    Iterate on the file list...

      bool found_name = false;
      wxString name;
      name.Clear();

      for(unsigned int j=0 ; j<pFileList->GetCount() ; j++)
      {
            wxFileName file(pFileList->Item(j));
            if(((file.GetExt()).MakeUpper()) == "TXT")
            {
              //  Look for the line beginning with the name of the .000 file
              wxTextFile text_file(file.GetFullPath());
              text_file.Open();

              wxString str = text_file.GetFirstLine();
              while(!text_file.Eof())
              {
                if(0 == target_name.CmpNoCase(str.Mid(0, target_name.Len())))
                {                                                       // found it
                  wxString tname = str.AfterFirst('-');
                  name = tname.AfterFirst(' ');
                  found_name = true;
                  break;
                }
                else
                {
                  str = text_file.GetNextLine();
                }
              }

              text_file.Close();

              if(found_name)
                break;
            }
      }

      Name = name;

      delete pFileList;

}







//---------------------------------------------------------------------------------
//      S57 Database methods
//---------------------------------------------------------------------------------


//-------------------------------
//
// S57 OBJECT ACCESSOR SECTION
//
//-------------------------------

const char *s57chart::getName(OGRFeature *feature)
{
   return feature->GetDefnRef()->GetName();
}

static int ExtensionCompare(const wxString& first, const wxString& second)
{
        wxFileName fn1(first);
        wxFileName fn2(second);
        wxString ext1(fn1.GetExt());
        wxString ext2(fn2.GetExt());

    return ext1.Cmp(ext2);
}


int s57chart::GetUpdateFileArray(const wxString& DirName, wxArrayString *UpFiles)
{
        wxDir dir(DirName);
        wxString ext;
        wxArrayString *dummy_array;
        int retval = 0;

        if(UpFiles == NULL)
                dummy_array = new wxArrayString;
        else
                dummy_array = UpFiles;

        wxString filename;
        bool cont = dir.GetFirst(&filename);
        while ( cont )
        {
                wxFileName file(filename);
                ext = file.GetExt();
                if(ext.IsNumber() && ext.CmpNoCase("000"))
                {
                        wxString FileToAdd(DirName);
                        FileToAdd.Append(file.GetFullName());
                        dummy_array->Add(FileToAdd);
                }

                cont = dir.GetNext(&filename);
        }

//      Sort the candidates

        dummy_array->Sort(ExtensionCompare);

//      Get the update number of the last in the list
        if(dummy_array->GetCount())
        {
                wxString Last = dummy_array->Last();
                wxFileName fnl(Last);
                ext = fnl.GetExt();
                retval = atoi(ext.c_str());
        }

        if(UpFiles == NULL)
                delete dummy_array;

        return retval;
}


int s57chart::CountUpdates( const wxString& DirName, wxString &LastUpdateDate)
{

        int retval = 0;

        wxDir dir(DirName);
        wxArrayString *UpFiles = new wxArrayString;
        retval = GetUpdateFileArray(DirName, UpFiles);

        if(retval)
        {
            //      The s57reader of ogr requires that update set be sequentially complete
            //      to perform all the updates.  However, some NOAA ENC distributions are
            //      not complete, as apparently some interim updates have been  withdrawn.
            //      Example:  as of 20 Dec, 2005, the update set for US5MD11M.000 includes
            //      US5MD11M.017, ...018, and ...019.  Updates 001 through 016 are missing.
            //
            //      Workaround.
            //      Create temporary dummy update files to fill out the set before invoking
            //      ogr file open/ingest.  Delete after SENC file create finishes.


            tmpup_array = new wxArrayString;                // save a list of created files for later erase

            for(int iff=1 ; iff < retval ; iff++)
            {
                wxFileName ufile(*pFullPath);
                wxString sext;
                sext.Printf("%03d", iff);
                ufile.SetExt(sext);


                //      Explicit check for a short update file, possibly left over from a crash...
                int flen = 0;
                if(ufile.FileExists())
                {
                    wxFile uf(ufile.GetFullPath());
                    if(uf.IsOpened())
                    {
                        flen = uf.Length();
                        uf.Close();
                    }
                }

                if(ufile.FileExists() && (flen > 25))           // a valid update file
                    continue;

                // Create a dummy ISO8211 file with no real content

                bool bstat;
                DDFModule *dupdate = new DDFModule;
                dupdate->Initialize( '3','L','E','1','0',"!!!",3,4,4 );
                bstat = dupdate->Create(ufile.GetFullPath().c_str());
                dupdate->Close();

                if(!bstat)
                    wxLogMessage("Error creating dummy update file %s",ufile.GetFullPath().c_str());


                tmpup_array->Add(ufile.GetFullPath());
            }



            //      Extract the date field from the last of the update files
            //      which is by definition a vail, present update file....
            bool bSuccess;
            DDFModule oUpdateModule;

            bSuccess = oUpdateModule.Open( UpFiles->Last().c_str(), TRUE );

            if( bSuccess )
            {
//      Get publish/update date
                oUpdateModule.Rewind();
                DDFRecord *pr = oUpdateModule.ReadRecord();                     // Record 0

                int nSuccess;
                char *u = (char *)(pr->GetStringSubfield("DSID", 0, "ISDT", 0, &nSuccess));

                LastUpdateDate = wxString(u);
            }
        }

        delete UpFiles;
        return retval;
}



int s57chart::BuildS57File(const char *pFullPath)
{

    OGRFeature *objectDef;
    int nProg = 0;

    wxString nice_name;
    GetChartNameFromTXT(wxString(pFullPath), nice_name);


    wxFileName s57file = wxFileName(pFullPath);
    s57file.SetExt("S57");

    OGREnvelope xt;

    //      Only allow updates if ISO8211 library can do it.
    bool benable_update = s57_ddfrecord_test();


    wxString date000;
    int native_scale = 1;


    //      Fetch the Geo Feature Count, or something like it....


    DDFModule *poModule = new DDFModule();
    poModule->Open( pFullPath );
    poModule->Rewind();
    DDFRecord *pr = poModule->ReadRecord();                               // Record 0

    int nSuccess;
    nGeoRecords = pr->GetIntSubfield("DSSI", 0, "NOGR", 0, &nSuccess);

// Todo I'll use ISDT here... but what is UADT?
    char *u = (char *)(pr->GetStringSubfield("DSID", 0, "ISDT", 0, &nSuccess));
    if(u)
        date000 = u;

//      Fetch the Native Scale
    for( ; pr != NULL; pr = poModule->ReadRecord() )
    {
        if( pr->FindField( "DSPM" ) != NULL )
        {
            native_scale = pr->GetIntSubfield("DSPM",0,"CSCL",0);
                        break;
        }
    }

    delete poModule;




    wxFileName tfn;
    wxString tmp_file = tfn.CreateTempFileName("");


    FILE *fps57;
    fps57 = fopen(tmp_file.c_str(), "wb");

    if(fps57 == NULL)
    {
            wxLogMessage("s57chart::BuildS57File  Unable to create %s )",s57file.GetFullPath().c_str());
            return 0;
    }

    fprintf(fps57, "SENC Version= %d\n", CURRENT_SENC_FORMAT_VERSION);
    fprintf(fps57, "NAME=%s\n", nice_name.c_str());
    fprintf(fps57, "DATE000=%s\n", date000.c_str());
    fprintf(fps57, "NOGR=%d\n", nGeoRecords);
    fprintf(fps57, "SCALE=%d\n", native_scale);

    wxString Message = s57file.GetFullPath();
    Message.Append("...Ingesting");

    wxString Title(_T("OpenCPN S57 SENC File Create..."));
    Title.append(s57file.GetFullPath());

    wxProgressDialog    *SENC_prog;
    SENC_prog = new wxProgressDialog(  Title, Message, nGeoRecords, NULL,
                                       wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
                                       wxPD_REMAINING_TIME  | wxPD_SMOOTH );



    //      Analyze Updates
    //      The OGR library will apply updates automatically, as specified by the UPDATES=APPLY flag
    //      We need to keep track of the last sequential update applied, to look out for new updates

    int last_applied_update = 0;
    wxString LastUpdateDate = date000;
    if(benable_update)
        last_applied_update = CountUpdates( s57file.GetPath((int)wxPATH_GET_SEPARATOR), LastUpdateDate);

    fprintf(fps57, "UPDT=%d\n", last_applied_update);
    fprintf(fps57, "DATEUPD=%s\n", LastUpdateDate.c_str());


    //  Here comes the actual ISO8211 file reading
    OGRDataSource *poDS = OGRSFDriverRegistrar::Open( pFullPath );
    if( poDS == NULL )
    {
        wxLogMessage("s57chart::BuildS57File  Unable to open %s )", pFullPath);
        delete SENC_prog;
        fclose(fps57);

        return 0;
    }

    //  Now that the .000 file with updates is safely ingested, delete any temporary
    //  dummy update files

    if(tmpup_array)
    {
        for(unsigned int iff = 0 ; iff < tmpup_array->GetCount() ; iff++)
           remove(tmpup_array->Item(iff).c_str());
        delete tmpup_array;
    }


    //      Insert my local error handler to catch OGR errors,
    //      Especially CE_Fatal type errors
    //      Discovered/debugged on US5MD11M.017.  VI 548 geometry deleted
    CPLPushErrorHandler( OpenCPN_OGRErrorHandler );


    //  Read all features, layer by layer
    //  Using the api defined by <ogrsf_frmts.h>
    bool bcont = true;
    int iObj = 0;
    OGRwkbGeometryType geoType;
    wxString sobj;
    int iLayObj;

    int nLayers = poDS->GetLayerCount();


    for(int iL=0 ; iL < nLayers ; iL++)
    {
        OGRLayer *pLay = poDS->GetLayer(iL);

        pLay->ResetReading();

        iLayObj = 0;

        while (bcont)
        {
            //  Prepare for possible CE_Fatal error in GDAL
            //  Corresponding longjmp() is in the local error handler
            int setjmp_ret = 0;
            setjmp_ret = setjmp(env_ogrf);
            if(setjmp_ret == 1)                 //  CE_Fatal happened in GDAL library
                                                //  in the GetNextFeature() call below.
                                                //  Seems odd, but that's setjmp/longjmp behaviour
            {
                wxString sLay("Unknown");
                if(iLayObj)
                    sLay = sobj;

                wxLogMessage("s57chart(): GDAL/OGR Fatal Error caught on Obj #%d.\n \
                        Skipping all remaining objects on Layer %s.", iObj, sLay.c_str());
                delete objectDef;
                break;                                  // pops out of while(bcont) to next layer
            }

            objectDef = pLay->GetNextFeature();
            iObj++;
            iLayObj++;

            if(objectDef == NULL)
                break;

            sobj = wxString(objectDef->GetDefnRef()->GetName());

//  Update the progress dialog

            nProg = iObj;
            if(nProg > nGeoRecords - 1)
                nProg = nGeoRecords - 1;

            if(0 == (nProg % 100))
                bcont = SENC_prog->Update(nProg, sobj);


            geoType = wkbUnknown;
//      This test should not be necessary for real (i.e not C_AGGR) features
//      However... some update files contain errors, and have deleted some
//      geometry without deleting the corresponding feature(s).
//      So, GeometryType becomes Unknown.
//      e.g. US5MD11M.017
//      In this case, all we can do is skip the feature....sigh.

            if (objectDef->GetGeometryRef() != NULL)
                geoType = objectDef->GetGeometryRef()->getGeometryType();

// Debug
//            if(!strncmp(objectDef->GetDefnRef()->GetName(), "LIGHTS", 6))
//                int ggk = 5;

//      Look for polygons to process
            if(geoType == wkbPolygon)
            {
                OGRPolygon *poly = (OGRPolygon *)(objectDef->GetGeometryRef());


                if(1)
                {
//                    if(!strncmp(objectDef->GetDefnRef()->GetName(), "LNDARE", 6))
                    if(1)
                    {
                        bcont = SENC_prog->Update(nProg, sobj);
                        CreateSENCRecord( objectDef, fps57, 0 );
                        PolyTessGeo ppg(poly);
                        ppg.Write_PolyTriGroup( fps57 );
//                        ppgtess.Tess_and_write_PolyTriGroup(poly, fps57);
                    }



                }


            }

//      n.b  This next line causes skip of C_AGGR features w/o geometry
            else if( geoType != wkbUnknown )                                // Write only if has wkbGeometry
                CreateSENCRecord( objectDef, fps57, 1 );

            delete objectDef;


        }           // while bcont

    }               // for

    delete SENC_prog;

    fclose(fps57);

    delete poDS;

    CPLPopErrorHandler();

//    Need root to create the SENC file
#ifndef __WXMSW__
      seteuid(file_user_id);
#endif

    int ret_code = 0;
//      Was the operation cancelled?
    if(!bcont)
    {
        unlink(tmp_file.c_str());               //      Delete the temp file....
        ret_code = 0;
    }
    else
    {
        remove(s57file.GetFullPath());
        unlink(s57file.GetFullPath());       //  Delete any existing SENC file....
        int err = rename(tmp_file.c_str(), s57file.GetFullPath()); //   mv temp file to target
        if(err)
        {
            wxLogMessage("Could not rename temporary SENC file %s to %s",tmp_file.c_str(),
                                     s57file.GetFullPath().c_str());
//            wxString msg1("Could not create SENC file, perhaps permissions not set to read/write?");
//            wxMessageDialog mdlg(this, msg1, wxString("OpenCPN"),wxICON_ERROR  );
//            if(mdlg.ShowModal() == wxID_YES)

            ret_code = 0;
        }
        else
            ret_code = 1;
     }

 //   Return to default user priveleges
#ifndef __WXMSW__
      seteuid(user_user_id);
#endif


      return ret_code;
}

int s57chart::BuildRAZFromS57File( const char *pFullPath )
{

        int nProg = 0;

        wxString ifs(pFullPath);

        wxFileInputStream fpx_u(ifs);
        wxBufferedInputStream fpx(fpx_u);

        int MAX_LINE = 499999;
        char *buf = (char *)malloc(MAX_LINE + 1);

        LUPrec           *LUP;
        S52_LUP_table_t  LUPtype;

        int     nGeoFeature;

        int object_count = 0;

        OGREnvelope     Envelope;

        int dun = 0;

        hdr_buf = (char *)malloc(1);
        wxProgressDialog    *SENC_prog = NULL;
        int nGeo1000;
        wxString date_000, date_upd;

        if(my_fgets(buf, MAX_LINE, fpx) == 0)
           dun = 1;


        // Create ATON arrays, needed by S52PLIB
        pFloatingATONArray = new wxArrayPtrVoid;
        pRigidATONArray = new wxArrayPtrVoid;



        while(!dun)
        {

                if(my_fgets(buf, MAX_LINE, fpx) == 0)
                {
                        dun = 1;
                        break;
                }

                if(!strncmp(buf, "OGRF", 4))
                {

                    S57Obj *obj = new S57Obj(buf, &fpx);
                    if(obj)
                    {

//      Build/Maintain the ATON floating/rigid arrays
                         if (GEO_POINT == obj->Primitive_type)
                         {

// set floating platform
                            if ((!strncmp(obj->FeatureName, "LITFLT", 6)) ||
                                (!strncmp(obj->FeatureName, "LITVES", 6)) ||
                                (!strncmp(obj->FeatureName, "BOY",    3)))

                                pFloatingATONArray->Add(obj);

// set rigid platform
                            if (!strncmp(obj->FeatureName, "BCN",    3))
                                pRigidATONArray->Add(obj);
                         }



//      This is where Simplified or Paper-Type point features are selected
                         switch(obj->Primitive_type)
                         {
                            case GEO_POINT:
                            case GEO_META:
                            case GEO_PRIM:

                                LUPtype = S52_LUPARRAY_PT_PAPER;
//                              LUPtype = S52_LUPARRAY_PT_SIMPL;
                                break;

                             case GEO_LINE:
                                 LUPtype = S52_LUPARRAY_LINE;
                                 break;

                             case GEO_AREA:
                                 LUPtype = S52_LUPARRAY_AREA_PLN;
//                               LUPtype = S52_LUPARRAY_AREA_SYM;
                                 break;
                         }

//        if(!strncmp(obj->FeatureName, "LOCMAG", 6))
//            int ffl = 4;
                         LUP = ps52plib->S52_lookupA(LUPtype,obj->FeatureName,obj);

                         if(NULL == LUP)
                             wxLogMessage("Could not find LUP for %s", obj->FeatureName);
                         else
                         {
//              Convert LUP to rules set
                            ps52plib->_LUP2rules(LUP, obj);

//              Add linked object/LUP to the working set
                            _insertRules(obj,LUP);
                         }
                    }


                    object_count++;

                    if((object_count % 500) == 0)
                    {
                        nProg = object_count / 500;
                        if(nProg > nGeo1000 - 1)
                                nProg = nGeo1000 - 1;

                        if(SENC_prog)
                            SENC_prog->Update(nProg);
                    }


                    continue;


                }               //OGRF

            else if(!strncmp(buf, "DATEUPD", 7))
            {
                  date_upd.Append(wxString(&buf[8]).BeforeFirst('\n'));
            }

            else if(!strncmp(buf, "DATE000", 7))
            {
                  date_000.Append(wxString(&buf[8]).BeforeFirst('\n'));
            }

            else if(!strncmp(buf, "SCALE", 5))
                {
                        int ins;
                        sscanf(buf, "SCALE=%d", &ins);
                        NativeScale = ins;
                }

            else if(!strncmp(buf, "NAME", 4))
            {
                  pName->Append(wxString(&buf[5]).BeforeFirst('\n'));
            }

            else if(!strncmp(buf, "NOGR", 4))
            {
                 sscanf(buf, "NOGR=%d", &nGeoFeature);

                 nGeo1000 = nGeoFeature / 500;

#ifndef __WXGTK__
                 SENC_prog = new wxProgressDialog(  _T("OpenCPN S57 SENC File Load"),
                                    pFullPath,
                                    nGeo1000, NULL,
                                    wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME | wxPD_SMOOTH
                                 );
#endif
            }


        }                       //while(!dun)


//      fclose(fpx);

        free(buf);

        free(hdr_buf);

        delete SENC_prog;

 //   Decide on pub date to show

      int d000 = atoi(wxString(date_000).Mid(0,4));
      int dupd = atoi(wxString(date_upd).Mid(0,4));

      if(dupd > d000)
        pPubYear->Append(wxString(date_upd).Mid(0,4));
      else
        pPubYear->Append(wxString(date_000).Mid(0,4));


        return 1;
}

//------------------------------------------------------------------------------
//      Local version of fgets for Binary Mode (SENC) file
//------------------------------------------------------------------------------
 int s57chart::my_fgets( char *buf, int buf_len_max, wxBufferedInputStream& ifs )

{
    char        chNext;
    int         nLineLen = 0;
    char            *lbuf;

    lbuf = buf;


    while( !ifs.Eof() && nLineLen < buf_len_max )
    {
        chNext = ifs.GetC();

        /* each CR/LF (or LF/CR) as if just "CR" */
        if( chNext == 10 || chNext == 13 )
        {
            chNext = '\n';
        }

        *lbuf = chNext; lbuf++, nLineLen++;

        if( chNext == '\n' )
        {
            *lbuf = '\0';
            return nLineLen;
        }
    }

    *(lbuf) = '\0';

    return nLineLen;
}



int s57chart::_create_attList(S57Obj *obj)
{
   int i;
   int nAtt = 0;

   // debug DRVAL
//if (0 == strncmp(getName(obj->objectDef), "DEPARE",6))
//   int iiop =9;

   // load attributes: name and value
   nAtt = obj->objectDef->GetFieldCount();
   for (i=8; i<nAtt; ++i){                                      // skip 8 first field
     if (obj->objectDef->IsFieldSet(i)){
//        S57attVal attValTmp;
         S57attVal *pattValTmp = new S57attVal;

        if (obj->attList == NULL ){
//           obj->attList = g_string_new(obj->objectDef->GetFieldDefnRef(i)->GetNameRef());
           obj->attList = new wxString(obj->objectDef->GetFieldDefnRef(i)->GetNameRef());

//           obj->attVal  = g_array_new(FALSE, TRUE, sizeof(S57attVal));
                   obj->attVal = new wxArrayOfS57attVal();
        }else
//           obj->attList = g_string_append(obj->attList,obj->objectDef->GetFieldDefnRef(i)->GetNameRef());
           obj->attList->Append(obj->objectDef->GetFieldDefnRef(i)->GetNameRef());

        pattValTmp->value   = obj->objectDef->GetRawFieldRef(i);
        //attValTmp.valType = obj->objectDef->GetFieldDefnRef(i)->GetType();

        switch(obj->objectDef->GetFieldDefnRef(i)->GetType()){
           // Simple 32bit integer
           case OFTInteger:             pattValTmp->valType = OGR_INT;          break;
           // List of 32bit integers
           case OFTIntegerList: pattValTmp->valType = OGR_INT_LST; break;
           // Double Precision floating point
           case OFTReal:                        pattValTmp->valType = OGR_REAL;         break;
           // String of ASCII chars
           case OFTString:                      pattValTmp->valType = OGR_STR;          break;

           // ATTRIBUTE TYPE NOT VALID IN S57
           // List of doubles                                   OFTRealList             = 3,
           // Array of strings                        OFTStringList             = 5,
           // Double byte string (unsupported)        OFTWideString             = 6,
           // List of wide strings (unsupported)      OFTWideStringList = 7,
           // Raw Binary data (unsupported)           OFTBinary                         = 8,
           default:
              printf("S57query:_create_attList(): no S57 attribute type equivalence for OGR type\n");


        }


//        g_array_append_val(obj->attVal, attValTmp);
                obj->attVal->Add(pattValTmp);

//           obj->attList = g_string_append_c(obj->attList,'\037');
             obj->attList->Append('\037');
     }
   }

   return 1;
}



int s57chart::_insertRules(S57Obj *obj, LUPrec *LUP)
{
   ObjRazRules   *rzRules = NULL;
   int                          disPrioIdx = 0;
   int                          LUPtypeIdx = 0;

   if(LUP == NULL){
      printf("SEQuencer:_insertRules(): ERROR no rules to insert!!\n");
      return 0;
   }

   /*
   // find display priority index       --strait version
   disPrioIdx = LUP->DPRI - '0';
   if (disPrioIdx &lt; 0 || PRIO_NUM &lt;= disPrioIdx){
      printf("SEQuencer:_insertRules():ERROR no display priority!!!\n");
      return 0;
   }
   */

   // find display priority index       --talky version
   switch(LUP->DPRI){
      case PRIO_NODATA:         disPrioIdx = 0; break;  // no data fill area pattern
      case PRIO_GROUP1:         disPrioIdx = 1; break;  // S57 group 1 filled areas
      case PRIO_AREA_1:         disPrioIdx = 2; break;  // superimposed areas
      case PRIO_AREA_2:         disPrioIdx = 3; break;  // superimposed areas also water features
      case PRIO_SYMB_POINT:     disPrioIdx = 4; break;  // point symbol also land features
      case PRIO_SYMB_LINE:      disPrioIdx = 5; break;  // line symbol also restricted areas
      case PRIO_SYMB_AREA:      disPrioIdx = 6; break;  // area symbol also traffic areas
      case PRIO_ROUTEING:       disPrioIdx = 7; break;  // routeing lines
      case PRIO_HAZARDS:        disPrioIdx = 8; break;  // hazards
      case PRIO_MARINERS:       disPrioIdx = 9; break;  // VRM & EBL, own ship
      default:
         printf("SEQuencer:_insertRules():ERROR no display priority!!!\n");
   }

   // find look up type index
   switch(LUP->TNAM){
      case SIMPLIFIED:                          LUPtypeIdx = 0; break; // points
      case PAPER_CHART:                         LUPtypeIdx = 1; break; // points
      case LINES:                                       LUPtypeIdx = 2; break; // lines
      case PLAIN_BOUNDARIES:            LUPtypeIdx = 3; break; // areas
      case SYMBOLIZED_BOUNDARIES:       LUPtypeIdx = 4; break; // areas
      default:
         printf("SEQuencer:_insertRules():ERROR no look up type !!!\n");
   }

   // insert rules
   rzRules = (ObjRazRules *)malloc(sizeof(ObjRazRules));
   rzRules->obj   = obj;
   rzRules->LUP   = LUP;
   rzRules->chart = this;
   rzRules->next  = razRules[disPrioIdx][LUPtypeIdx];
   razRules[disPrioIdx][LUPtypeIdx] = rzRules;

   return 1;
}









void s57chart::CreateSENCRecord( OGRFeature *pFeature, FILE * fpOut, int mode )

{

#define MAX_HDR_LINE    400

        char line[MAX_HDR_LINE + 1];
        wxString sheader;

        fprintf( fpOut, "OGRFeature(%s):%ld\n", pFeature->GetDefnRef()->GetName(),
                  pFeature->GetFID() );

//      In the interests of output file size, DO NOT report fields that are not set.
        for( int iField = 0; iField < pFeature->GetFieldCount(); iField++ )
        {
                if( pFeature->IsFieldSet( iField ) )
                {
                        if((iField == 1) || (iField > 7))
                        {
                                OGRFieldDefn *poFDefn = pFeature->GetDefnRef()->GetFieldDefn(iField);

                                const char *pType = OGRFieldDefn::GetFieldTypeName(poFDefn->GetType()) ;

#ifdef __WXMSW__
                                _snprintf( line, MAX_HDR_LINE - 2, "  %s (%c) = %s",
                                         poFDefn->GetNameRef(),
                                         *pType,
                                         pFeature->GetFieldAsString( iField ));
#else
                                snprintf( line, MAX_HDR_LINE - 2, "  %s (%c) = %s",
                                         poFDefn->GetNameRef(),
                                         *pType,
                                         pFeature->GetFieldAsString( iField ));
#endif


                        sheader += line;
                        sheader += '\n';
                        }
                }

        }

        OGRGeometry *pGeo = pFeature->GetGeometryRef();

        if(mode == 1)
        {
                sprintf( line, "  %s\n", pGeo->getGeometryName());
            sheader += line;
      }

      fprintf( fpOut, "HDRLEN=%d\n", sheader.Len());
      fwrite(sheader.c_str(), 1, sheader.Len(), fpOut);

        if(( pGeo != NULL ) && (mode == 1))
        {
        int wkb_len = pGeo->WkbSize();
        unsigned char *pwkb_buffer = (unsigned char *)malloc(wkb_len);

//  Get the GDAL data representation
                pGeo->exportToWkb(wkbNDR, pwkb_buffer);

    //  Convert to opencpn SENC representation

    //  Set absurd bbox starting limits
        float xmax = -1000;
        float xmin = 1000;
        float ymax = -1000;
        float ymin = 1000;

                int i, ip, sb_len;
        float *pdf;
                double *psd;
                unsigned char *ps;
                unsigned char *pd;
                unsigned char *psb_buffer;

                OGRwkbGeometryType gType = pGeo->getGeometryType();
        switch(gType)
        {
          case wkbLineString:
            sb_len = ((wkb_len - 9) / 2) + 9 + 16;                // data will be 4 byte float, not double
                                                                  // and bbox limits are tacked on end
            fprintf( fpOut, "  %d\n", sb_len);


            psb_buffer = (unsigned char *)malloc(sb_len);
            pd = psb_buffer;
            ps = pwkb_buffer;

            memcpy(pd, ps, 9);                                    // byte order, type, and count

            ip = *((int *)(ps + 5));                              // point count

            pd += 9;
            ps += 9;
            psd = (double *)ps;
            pdf = (float *)pd;


            for(i = 0 ; i < ip ; i++)                           // convert doubles to floats
            {                                                   // computing bbox as we go
                float x = (float)*psd;
                *pdf = x;
                psd++;
                pdf++;
                xmax = fmax(x, xmax);
                xmin = fmin(x, xmin);

                float y = (float)*psd;
                *pdf = y;
                psd++;
                pdf++;
                ymax = fmax(y, ymax);
                ymin = fmin(y, ymin);


            }

            *pdf++ = xmax;
            *pdf++ = xmin;
            *pdf++ = ymax;
            *pdf =   ymin;

            fwrite(psb_buffer, 1, sb_len, fpOut);
            free(psb_buffer);
            break;

        case wkbMultiLineString:
          wxLogMessage("Warning: Unimplemented SENC wkbMultiLineString record in file %s",
                       pS57FileName->GetFullPath().c_str());

          wkb_len = pGeo->WkbSize();
          fprintf( fpOut, "  %d\n", wkb_len);
          fwrite(pwkb_buffer, 1, wkb_len, fpOut);

          break;

          //Todo Implement some other private SENC formats, e.g. wkbPoint
    /*
          wkbPoint = 1,
          wkbLineString = 2,
          wkbPolygon = 3,
          wkbMultiPoint = 4,
          wkbMultiLineString = 5,
          wkbMultiPolygon = 6,
    */
          default:
          wkb_len = pGeo->WkbSize();
          fprintf( fpOut, "  %d\n", wkb_len);
          fwrite(pwkb_buffer, 1, wkb_len, fpOut);

        break;
        }





                free(pwkb_buffer);

                fprintf( fpOut, "\n" );

        }

}

ArrayOfS57Obj *s57chart::GetObjArrayAtLatLon(float lat, float lon, float select_radius)
{

    ArrayOfS57Obj *ret_ptr = new ArrayOfS57Obj;


//    Iterate thru the razRules array, by object/rule type

    ObjRazRules *crnt;
    ObjRazRules *top;

    for (int i=0; i<PRIO_NUM; ++i)
    {
      // Points first

          top = razRules[i][0];           //SIMPLIFIED Points
          while ( top != NULL)
          {
                  crnt = top;
                  top  = top->next;

          }


          top = razRules[i][1];           //Paper Chart Points
          while ( top != NULL)
          {
                  crnt = top;
                  top  = top->next;

                  if(DoesLatLonSelectObject(lat, lon, select_radius, crnt->obj))
                        ret_ptr->Add(crnt->obj);

          }

      // Areas by boundary type

          top = razRules[i][4];           // Area Symbolized Boundaries
          while ( top != NULL)
          {
                  crnt = top;
                  top  = top->next;

                  if(ps52plib->m_nDisplayCategory == OTHER)
                  {
                    OBJLElement *pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(crnt->obj->iOBJL));
                    if(pOLE->nViz)
                      if(DoesLatLonSelectObject(lat, lon, select_radius, crnt->obj))
                        ret_ptr->Add(crnt->obj);
                  }

                  else                                              // check DISPLAYBASE or STANDARD
                  {
                      if(ps52plib->m_nDisplayCategory == DISPLAYBASE)
                      {
                         if(DISPLAYBASE == crnt->LUP->DISC)
                         {
                            if(DoesLatLonSelectObject(lat, lon, select_radius, crnt->obj))
                                ret_ptr->Add(crnt->obj);
                         }
                       }

                      else if(ps52plib->m_nDisplayCategory == STANDARD)
                      {
                             if((DISPLAYBASE == crnt->LUP->DISC) || (STANDARD == crnt->LUP->DISC))
                             {
                                if(DoesLatLonSelectObject(lat, lon, select_radius, crnt->obj))
                                    ret_ptr->Add(crnt->obj);
                             }
                      }

                  }
          }


          top = razRules[i][3];           // Area Plain Boundaries

          while ( top != NULL)
          {
                  crnt = top;
                  top  = top->next;

                  if(ps52plib->m_nDisplayCategory == OTHER)
                  {
                    OBJLElement *pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(crnt->obj->iOBJL));
                    if(pOLE->nViz)
                      if(DoesLatLonSelectObject(lat, lon, select_radius, crnt->obj))
                        ret_ptr->Add(crnt->obj);
                  }

                  else                                              // check DISPLAYBASE or STANDARD
                  {
                      if(ps52plib->m_nDisplayCategory == DISPLAYBASE)
                      {
                         if(DISPLAYBASE == crnt->LUP->DISC)
                         {
                            if(DoesLatLonSelectObject(lat, lon, select_radius, crnt->obj))
                                ret_ptr->Add(crnt->obj);
                         }
                       }

                      else if(ps52plib->m_nDisplayCategory == STANDARD)
                      {
                             if((DISPLAYBASE == crnt->LUP->DISC) || (STANDARD == crnt->LUP->DISC))
                             {
                                if(DoesLatLonSelectObject(lat, lon, select_radius, crnt->obj))
                                    ret_ptr->Add(crnt->obj);
                             }
                      }

                  }
          }



      // Finally, lines
          top = razRules[i][2];           // Lines

          while ( top != NULL)
          {
                  crnt = top;
                  top  = top->next;
                  if(DoesLatLonSelectObject(lat, lon, select_radius, crnt->obj))
                        ret_ptr->Add(crnt->obj);
          }

      }


      return ret_ptr;
}

bool s57chart::DoesLatLonSelectObject(float lat, float lon, float select_radius, S57Obj *obj)
{


      switch(obj->Primitive_type)
      {
            case  GEO_POINT:
                  if((fabs(lon - obj->x) < select_radius) && (fabs(lat - obj->y) < select_radius))
                        return true;

            case  GEO_AREA:
                {
                    return IsPointInObjArea(lat, lon, select_radius, obj);
                    break;
                }

            case  GEO_LINE:
            case  GEO_META:
            case  GEO_PRIM:

            break;
      }


      return false;
}

wxString *s57chart::CreateObjDescription(const S57Obj& obj)
{
      wxString *ret_str = new wxString;

      char *curr_att;
      int iatt;
      wxString att, value;
      S57attVal *pval;

      switch(obj.Primitive_type)
      {
            case  GEO_POINT:
            case  GEO_AREA:
                  {

                  //    Get Name
                  wxString name(obj.FeatureName);
                  *ret_str << name;
                  *ret_str << " - ";

                  //    Get the object's nice description from s57objectclasses.csv
                  //    using cpl_csv from the gdal library

                  const char *name_desc;
                  if(NULL != m_pcsv_locn)
                  {
                    wxString oc_file(*m_pcsv_locn);
                    oc_file.Append("/s57objectclasses.csv");
                    name_desc = MyCSVGetField(oc_file.c_str(),
                                     "Acronym",                  // match field
                                     obj.FeatureName,            // match value
                                     CC_ExactString,
                                     "ObjectClass");             // return field
                  }
                  else
                      name_desc = "";


                  *ret_str << name_desc;
                  *ret_str << '\n';


                  wxString index;
                  index.Printf("    Feature Index: %d\n", obj.Index);
                  *ret_str << index;


                  //    Get the Attributes and values

                  curr_att = (char *)(obj.attList->c_str());
                  iatt = 0;

                  while(*curr_att)
                  {
//    Attribute name
                        att.Clear();
                        while((*curr_att) && (*curr_att != '\037'))
                              att.Append(*curr_att++);

                        if(*curr_att == '\037')
                              curr_att++;

                        int is = 0;
                        while( is < 8)
                        {
                              *ret_str << ' ';
                              is++;
                        }

                        *ret_str << att;

                        is+= att.Len();
                        while( is < 25)
                        {
                              *ret_str << ' ';
                              is++;
                        }


// What we need to do...
// Change senc format, instead of (S), (I), etc, use the attribute types fetched from the S57attri...csv file
// This will be like (E), (L), (I), (F)
//  will affect lots of other stuff.  look for S57attVal.valType
// need to do this in creatsencrecord above, and update the senc format.

//    Attribute encoded value
                    value.Clear();

                    pval = obj.attVal->Item(iatt);
                    switch(pval->valType)
                    {
                        case OGR_STR:
                        {
                            if(pval->value)
                            {
                                wxString val_str((char *)(pval->value));
                                if(val_str.IsNumber())
                                {
                                    int ival = atoi(val_str.c_str());
                                    if(0 == ival)
                                        value.Printf("Unknown");
                                    else
                                    {
                                        wxString *decode_val = GetAttributeDecode(att, ival);
                                        if(decode_val)
                                            value.Printf("%s(%d)", decode_val->c_str(), ival);
                                        else
                                            value.Printf("(%d)", ival);
                                        delete decode_val;
                                    }
                                }

                                else if(val_str.IsEmpty())
                                    value.Printf("Unknown");

                                else
                                {
                                    value.Clear();
                                    wxString value_increment;
                                    wxStringTokenizer tk(val_str, wxT(","));
                                    int iv = 0;
                                    while ( tk.HasMoreTokens() )
                                    {
                                        wxString token = tk.GetNextToken();
                                        if(token.IsNumber())
                                        {
                                            int ival = atoi(token.c_str());
                                            wxString *decode_val = GetAttributeDecode(att, ival);
                                            if(decode_val)
                                                value_increment.Printf("%s", decode_val->c_str());
                                            else
                                                value_increment.Printf("(%d)", ival);

                                            delete decode_val;
                                            if(iv)
                                                value_increment.Prepend(wxT(", "));
                                        }
                                        value.Append(value_increment.c_str());

                                        iv++;
                                    }

                                    value.Append("(");
                                    value.Append(val_str.c_str());
                                    value.Append(")");
                                }
                            }
                            else
                            {
                                value.Printf("[NULL VALUE]");
                            }

                            break;
                        }

                        case OGR_INT:
                        {
                            int ival = *((int *)pval->value);
                            wxString *decode_val = GetAttributeDecode(att, ival);

                            if(decode_val)
                                value.Printf("%s(%d)", decode_val->c_str(), ival);
                            else
                                value.Printf("(%d)", ival);

                            delete decode_val;
                            break;
                        }
                        case OGR_INT_LST:
                        case OGR_REAL:
                        case OGR_REAL_LST:
                        {
                                break;
                        }
                    }


                        *ret_str << value;

                        *ret_str << '\n';
                        iatt++;
                  }

                  return ret_str;

                  }

//      wxString                *attList;
//      wxArrayOfS57attVal      *attVal;

#if 0
typedef enum _OGRatt_t{
   OGR_INT,
   OGR_INT_LST,
   OGR_REAL,
   OGR_REAL_LST,
   OGR_STR,
}OGRatt_t;

typedef struct _S57attVal{
   void *   value;
   OGRatt_t valType;
}S57attVal;
#endif


            case  GEO_LINE:
            case  GEO_META:
            case  GEO_PRIM:

            break;
      }

      return ret_str;
}

/*
wxString *s57chart::GetAttributeDecode(wxString& att, int ival)
{

      wxString ret_string;

      wxString file;
      wxGetEnv(wxString("S57_CSV"), &file);

      file.Append(wxT("/attdecode.csv"));

      char *attdec;

      attdec = (char *)MyCSVGetField( file.c_str(),
                              "Attribute", att.c_str(), CC_ExactString,
                              "ValueDecode" );

      if(strlen(attdec))
      {
//    Extract the required value
            wxString sattdec(attdec);
            wxStringTokenizer tk(sattdec, wxT(";"));

            while ( tk.HasMoreTokens() )
            {
                  wxString token = tk.GetNextToken();
                  long tval;
                  if(token.ToLong(&tval))
                  {
                        ret_string = tk.GetNextToken();
                        if((int)tval == ival)
                            return new wxString(ret_string);
                  }

                  else
                        tk.GetNextToken();

            }
       }

       return NULL;
}


*/
wxString *s57chart::GetAttributeDecode(wxString& att, int ival)
{

    wxString *ret_val = NULL;

    if(NULL == m_pcsv_locn)
        return NULL;

    //  Get the attribute code from the acronym
    const char *att_code;

    wxString file(*m_pcsv_locn);
    file.Append("/s57attributes.csv");
    att_code = MyCSVGetField(file.c_str(),
                                  "Acronym",                  // match field
                                  att.c_str(),               // match value
                                  CC_ExactString,
                                  "Code");             // return field


    // Now, get a nice description from s57expectedinput.csv
    //  This will have to be a 2-d search, using ID field and Code field

    bool more = true;
    wxString ei_file(*m_pcsv_locn);
    ei_file.Append("/s57expectedinput.csv");

    FILE        *fp;
    fp = VSIFOpen( ei_file.c_str(), "rb" );
    if( fp == NULL )
        return NULL;

    while(more)
    {
        char **result = CSVScanLines( fp,
                                     0,                         // first field = attribute Code
                                     att_code,
                                     CC_ExactString );

        if(NULL == result)
        {
            more = false;
            break;
        }
        if(atoi(result[1]) == ival)
        {
            ret_val = new wxString(result[2]);
        }
    }


    VSIFClose(fp);
    return ret_val;
}


extern "C" int G_PtInPolygon(MyPoint *, int, float, float) ;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


bool s57chart::IsPointInObjArea(float lat, float lon, float select_radius, S57Obj *obj)
{
//    if(obj->Index == 2041)
//        int ggk = 5;

    bool ret = false;

/*
    if(obj->pPolyGeo)
    {

//      Is the point in the PolyGeo Bounding Box?
        if(lon > obj->pPolyGeo->Get_xmax())
            return false;
        else if(lon < obj->pPolyGeo->Get_xmin())
            return false;
        else if(lat > obj->pPolyGeo->Get_ymax())
            return false;
        else if(lat < obj->pPolyGeo->Get_ymin())
            return false;


        PolyGroup *ppg = obj->pPolyGeo->Get_PolyGroup_head();

        int npoly = ppg->nPolys;

        for (int ip = 0;ip < npoly  ; ip++)
        {

//  Is point in any of the polys?
            wxBoundingBox *bbpoly = &(ppg->BBArray[ip]);
            double **pvert_array = ppg->pvert_array;
            int *pnv_array = ppg->pn_vertex;

//  Coarse test
            if(bbpoly->PointInBox(lon, lat, 0))
            {
                    int nvert = pnv_array[ip];

                    double *pvert_list = pvert_array[ip];
                    if(G_PtInPolygon((MyPoint *)pvert_list, nvert, lon, lat))
                    {
                        ret = true;
                        break;
                    }
            }
        }

        if(ret == false)                        // point is not within base poly
            return false;


        return ret;


    }           // if pPolyGeo

*/
    return false;
}




/************************************************************************/
/*                       OpenCPN_OGRErrorHandler()                      */
/************************************************************************/

void OpenCPN_OGRErrorHandler( CPLErr eErrClass, int nError,
                              const char * pszErrorMsg )

{
    static int       bLogInit = FALSE;
    static FILE *    fpLog = stderr;

    if( !bLogInit )
    {
        bLogInit = TRUE;

        fpLog = stderr;
        if( CPLGetConfigOption( "CPL_LOG", NULL ) != NULL )
        {
            fpLog = fopen( CPLGetConfigOption("CPL_LOG",""), "wt" );
            if( fpLog == NULL )
                fpLog = stderr;
        }
    }

    if( eErrClass == CE_Debug )
        fprintf( fpLog, "%s\n", pszErrorMsg );
    else if( eErrClass == CE_Warning )
        fprintf( fpLog, "Warning %d: %s\n", nError, pszErrorMsg );
    else
        fprintf( fpLog, "ERROR %d: %s\n", nError, pszErrorMsg );

    fflush( fpLog );

    //      Do not simply return on CE_Fatal errors, as we don't want to abort()

    if(eErrClass == CE_Fatal)
    {
        longjmp(env_ogrf, 1);                  // jump back to the setjmp() point
    }

}



//      In GDAL-1.2.0, CSVGetField is not exported.......
//      So, make my own simplified copy
/************************************************************************/
/*                           MyCSVGetField()                            */
/*                                                                      */
/************************************************************************/


const char *MyCSVGetField( const char * pszFilename,
                         const char * pszKeyFieldName,
                         const char * pszKeyFieldValue,
                         CSVCompareCriteria eCriteria,
                         const char * pszTargetField )

{
    char        **papszRecord;
    int         iTargetField;


/* -------------------------------------------------------------------- */
/*      Find the correct record.                                        */
/* -------------------------------------------------------------------- */
    papszRecord = CSVScanFileByName( pszFilename, pszKeyFieldName,
                                     pszKeyFieldValue, eCriteria );

    if( papszRecord == NULL )
        return "";

/* -------------------------------------------------------------------- */
/*      Figure out which field we want out of this.                     */
/* -------------------------------------------------------------------- */
    iTargetField = CSVGetFileFieldId( pszFilename, pszTargetField );
    if( iTargetField < 0 )
        return "";

    if( iTargetField >= CSLCount( papszRecord ) )
        return "";

    return( papszRecord[iTargetField] );
}


//------------------------------------------------------------------------
//
//          Some s57 Utilities
//          Meant to be called "bare", usually with no class instance.
//
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//  Initialize GDAL/OGR S57ENC support
//------------------------------------------------------------------------

int s57_initialize(const wxString& csv_dir)
{

    //  MS Windows Build Note:
    //  In a .dll GDAL build, the following _putenv() call DOES NOT properly
    //  set the environment accessible to GDAL/OGR.  So, S57 Reader options
    //  are not set AT ALL.  Defaults will apply.
    //  See the README file

#ifdef __WXMSW__
    wxString envs1("S57_CSV=");
    envs1.Append(csv_dir);
    _putenv( envs1.c_str());
#else
    wxSetEnv( "S57_CSV", csv_dir.c_str());
#endif


        //  Set some S57 OGR Options thru environment variables

        //  n.b. THERE IS A BUG in GDAL/OGR 1.2.0 wherein the sense of the flag UPDATES= is reversed.
        //  That is, anything other than UPDATES=APPLY selects update mode.
        //  Conversely, UPDATES=APPLY deselects updates.
        //  Fixed in GDAL 1.3.2, at least, maybe earlier??
        //  Detect by GDALVersion check

    const char *version_string = GDALVersionInfo("VERSION_NUM");
    int ver_num = (int)CPLScanLong((char *)version_string, 4);

    wxString set1, set2;

    set1 ="LNAM_REFS=ON";
    set1.Append(",SPLIT_MULTIPOINT=OFF");
    set1.Append(",ADD_SOUNDG_DEPTH=OFF");
    set1.Append(",PRESERVE_EMPTY_NUMBERS=OFF");
    if(ver_num >= 1320)
        set1.Append(",RETURN_PRIMITIVES=OFF");              // older GDALs handle some "off" option poorly
    set1.Append(",RETURN_LINKAGES=OFF");



    if(ver_num < 1320)
        set2 = ",UPDATES=BUGBUG";               // updates ENABLED
    else
        set2 = ",UPDATES=APPLY";

    set1.Append(set2);

#ifdef __WXMSW__
    wxString envs2("OGR_S57_OPTIONS=");
    envs2.Append(set1);
    _putenv( envs2.c_str());

#else
    wxSetEnv("OGR_S57_OPTIONS",set1.c_str());
#endif


//    CPLSetConfigOption( "CPL_DEBUG", "ON");
//    CPLSetConfigOption( "CPL_LOG", "c:\\LOG");

    RegisterOGRS57();

    return 0;
}





//----------------------------------------------------------------------------------
// Get Chart Scale
// By opening and reading directly the iso8211 file
//----------------------------------------------------------------------------------
int s57_GetChartScale(char *pFullPath)
{

    DDFModule   *poModule;
    DDFRecord   *poRecord;
    int scale;

    poModule = new DDFModule();

    if( !poModule->Open(pFullPath) )
    {
        delete poModule;
        return 0;
    }

    poRecord = poModule->ReadRecord();
    if( poRecord == NULL )
    {
        poModule->Close();
        delete poModule;
        return 0;
    }

    scale = 1;
    for( ; poRecord != NULL; poRecord = poModule->ReadRecord() )
    {
        if( poRecord->FindField( "DSPM" ) != NULL )
        {
            scale = poRecord->GetIntSubfield("DSPM",0,"CSCL",0);
            break;
        }
    }

    poModule->Close();
    delete poModule;

    return scale;

}

//----------------------------------------------------------------------------------
// Get First Chart M_COVR Object
// Directly from the ios8211 file
//              n.b. Caller owns the data source and the feature on success
//----------------------------------------------------------------------------------
bool s57_GetChartFirstM_COVR(char *pFullPath, OGRDataSource **pDS, OGRFeature **pFeature,
                                 OGRLayer **pLayer, int &catcov)
{

    OGRDataSource *poDS = OGRSFDriverRegistrar::Open( pFullPath );

    *pDS = poDS;                                    // give to caller

    if( poDS == NULL )
    {
        *pFeature = NULL;
        return false;
    }

    OGRLayer *pLay = poDS->GetLayerByName("M_COVR");
    *pLayer = pLay;                         // Give to caller
    pLay->ResetReading();
    OGRFeature *objectDef = pLay->GetNextFeature();
    *pFeature = objectDef;                  // Give to caller

    if(objectDef)
    {
    //  Fetch the CATCOV attribute
        for( int iField = 0; iField < objectDef->GetFieldCount(); iField++ )
        {
            if( objectDef->IsFieldSet( iField ) )
            {
                OGRFieldDefn *poFDefn = objectDef->GetDefnRef()->GetFieldDefn(iField);
                if(!strcmp(poFDefn->GetNameRef(), "CATCOV"))
                    catcov = objectDef->GetFieldAsInteger( iField );
            }
        }
        return true;
    }

    else
    {
        delete poDS;
        *pDS = NULL;
        return false;
    }

}

//----------------------------------------------------------------------------------
// GetNext Chart M_COVR Object
// Directly from the ios8211 file
// Companion to s57_GetChartFirstM_COVR
//              n.b. Caller still owns the data source and the feature on success
//----------------------------------------------------------------------------------
bool s57_GetChartNextM_COVR(OGRDataSource *pDS, OGRLayer *pLayer, OGRFeature *pLastFeature,
                                OGRFeature **pFeature, int &catcov)
{


    if( pDS == NULL )
        return false;

    catcov = -1;


    int fid = pLastFeature->GetFID();

    OGRFeature *objectDef = pLayer->GetFeature(fid + 1);
    *pFeature = objectDef;                  // Give to caller

    if(objectDef)
    {
        for( int iField = 0; iField < objectDef->GetFieldCount(); iField++ )
        {
            if( objectDef->IsFieldSet( iField ) )
            {
                OGRFieldDefn *poFDefn = objectDef->GetDefnRef()->GetFieldDefn(iField);
                if(!strcmp(poFDefn->GetNameRef(), "CATCOV"))
                    catcov = objectDef->GetFieldAsInteger( iField );
            }
        }
        return true;
    }
    return false;
}


//----------------------------------------------------------------------------------
// Get Chart Extents
//----------------------------------------------------------------------------------
bool s57_GetChartExtent(char *pFullPath, Extent *pext)
{
 //   Fix this  find extents of which?? layer??
/*
    OGRS57DataSource *poDS = new OGRS57DataSource;
    poDS->Open(pFullPath, TRUE);

    if( poDS == NULL )
    return false;

    OGREnvelope Env;
    S57Reader   *poReader = poDS->GetModule(0);
    poReader->GetExtent(&Env, true);

    pext->NLAT = Env.MaxY;
    pext->ELON = Env.MaxX;
    pext->SLAT = Env.MinY;
    pext->WLON = Env.MinX;

    delete poDS;
*/
    return true;

}

//----------------------------------------------------------------------------------
//      ddfrecord_test
//----------------------------------------------------------------------------------

        /*
        GDAL/OGR 1.2.0 may have a bug in ddfrecord.cpp
        The bug is manifest on ENC updates only.
        This behaviour has been observed on ddfrecord.cpp Version 1.25 and earlier,
        and is corrected in ddfrecord.cpp Version 1.27 and above.

        This (relatively) safe run-time test will identify the pathology
        allowing run-time election of ENC update policy
        */

bool s57_ddfrecord_test()
{
        // Create a ddfrecord and populate the two simple text fields

    DDFRecord dr(NULL);
    DDFFieldDefn dfd1;
    DDFFieldDefn dfd2;

    dfd1.Create("tag1", "name", NULL, dsc_elementary, dtc_char_string);
    dfd1.AddSubfield( "sub1", "A" );
    dr.AddField( &dfd1 );
    dr.SetStringSubfield( "tag1", 0, "sub1",0, "testlongrrrrrrrrrrrrrrrrrrrr11");

    dfd2.Create("tag2", "name", NULL, dsc_elementary, dtc_char_string);
    dfd2.AddSubfield( "sub21", "A" );
    dr.AddField( &dfd2 );
    dr.SetStringSubfield( "tag2", 0, "sub21",0, "testlonggggggggggggggggggggg21");


        //  The hallmark of obsolete ddfrecord code is that shortening a data record
        //  corrupts some data following the shortened target

    char buf1[40], buf2[40];
        // get reference copy
    const char *before = dr.GetStringSubfield( "tag2", 0, "sub21", 0);
    strcpy(buf1, before);

        // Shorten early data
    dr.SetStringSubfield( "tag1", 0, "sub1",0, "test12a");

        //  and get it again
    const char *after = dr.GetStringSubfield( "tag2", 0, "sub21", 0);
    strcpy(buf2, after);

    if(strcmp(buf1, buf2))
        return false;
    else
        return true;

//        dr.Dump(stderr);

}





//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



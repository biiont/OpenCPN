/******************************************************************************
 * $Id: chartdb.cpp,v 1.2 2006/09/21 01:37:36 dsr Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Chart Database Object
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
 * $Log: chartdb.cpp,v $
 * Revision 1.2  2006/09/21 01:37:36  dsr
 * Major refactor/cleanup
 *
 * Revision 1.1.1.1  2006/08/21 05:52:19  dsr
 * Initial import as opencpn, GNU Automake compliant.
 *
 * Revision 1.8  2006/08/04 11:42:01  dsr
 * no message
 *
 * Revision 1.7  2006/07/28 20:32:18  dsr
 * Eliminate duplicate charts from db
 *
 * Revision 1.6  2006/07/05 02:31:48  dsr
 * Cleanup
 *
 * Revision 1.5  2006/06/15 02:39:07  dsr
 * Support Database Update and direct search for S57 charts
 *
 * Revision 1.4  2006/06/02 02:09:31  dsr
 * Cleanup
 *
 * Revision 1.3  2006/05/19 19:17:53  dsr
 * New chart caching logic
 *
 * Revision 1.2  2006/04/23 03:55:45  dsr
 * Fix internal names
 *
 * Revision 1.1.1.1  2006/04/19 03:23:28  dsr
 * Rename/Import to OpenCPN
 *
 * Revision 1.9  2006/04/19 00:37:05  dsr
 * Implement ColorScheme
 *
 * Revision 1.8  2006/03/16 03:07:59  dsr
 * Cleanup tabs
 *
 * Revision 1.7  2006/02/24 18:03:15  dsr
 * Cleanup
 *
 * Revision 1.6  2006/02/23 01:33:17  dsr
 * Cleanup, use chart extents fully
 *
 *
 *
 */


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
      #include "wx/wx.h"
#endif //precompiled headers

#include "chartdb.h"
#include "chartimg.h"
#include "chart1.h"

#include <stdio.h>
#include <math.h>

#include "wx/generic/progdlgg.h"

#ifdef USE_S57
#include "s57chart.h"
#endif

extern ChartBase    *Current_Ch;


bool G_FloatPtInPolygon(MyFlPoint *rgpts, int wnumpts, float x, float y) ;


CPL_CVSID("$Id: chartdb.cpp,v 1.2 2006/09/21 01:37:36 dsr Exp $");

// ============================================================================
// implementation
// ============================================================================

ChartDB::ChartDB(MyFrame *parent)
{
      pParent = parent;
      ifs = 0;
      pChartTable = 0;

      pChartCache = new wxArrayPtrVoid;

      bValid = false;                           // until loaded or created
}


ChartDB::~ChartDB()
{
      if(pChartTable)
      {
            for(int i=0 ; i<nEntry ; i++)
            {
                  if(pChartTable[i].pFullPath)
                        free(pChartTable[i].pFullPath);
                  if(pChartTable[i].pPlyTable)
                        free(pChartTable[i].pPlyTable);

                  if(pChartTable[i].nAuxPlyEntries)
                  {
                      for(int j=0 ; j<pChartTable[i].nAuxPlyEntries ; j++)
                          free ( (pChartTable[i].pAuxPlyTable)[j] );
                      free( pChartTable[i].pAuxPlyTable );
                      free( pChartTable[i].pAuxCntTable );
                  }
            }

            free(pChartTable);
      }

      if(ifs)
            delete ifs;

//    Empty the cache
      unsigned int nCache = pChartCache->GetCount();
      for(unsigned int i=0 ; i<nCache ; i++)
      {
            CacheEntry *pce = (CacheEntry *)(pChartCache->Item(i));
            ChartBase *Ch = (ChartBase *)pce->pChart;
            delete Ch;
            delete pce;
      }

      delete pChartCache;
}


// ----------------------------------------------------------------------------
// Load Chart Table Database from binary .DAT file
// ----------------------------------------------------------------------------
bool ChartDB::LoadBinary(wxString *filename)
{
      unsigned char buf[200];
      wxFileInputStream *ifs;

      wxFile test;
      if (!test.Exists(*filename))
             return false;

      ifs = new wxFileInputStream(*filename);         // open the file as a read-only stream

      if(!ifs->Ok())
      {
            delete ifs;
            return false;
      }


      ifs->Read(buf, sizeof(ChartTableHeader));
      ChartTableHeader *pcth = (ChartTableHeader *)buf;
      int n_entries = pcth->nTableEntries;


      char vb[5];
      sprintf(vb, "V%03d", DB_VERSION);
      if(strncmp((const char *)buf, vb, 4))
            return false;


//    Read source directory names
      for(int iDir = 0 ; iDir < pcth->nDirEntries ; iDir++)
      {
            int ldir;
            ifs->Read(&ldir, sizeof(int));

            wxString dir;
            dir.Clear();
            for(int ic=0 ; ic<ldir ; ic++)
            {
                  char c;
                  ifs->Read(&c, 1);
                  dir.Append(c);
            }

      }

      pChartTable = (ChartTableEntry *)calloc(n_entries * sizeof(ChartTableEntry), 1);

      ChartTableEntry *pe = pChartTable;
      for(int iEntry = 0 ; iEntry < n_entries ; iEntry++)
      {
            char *s = (char *)buf;

            int ifp = 0;
            while((*s = ifs->GetC()) != 0)
            {
                  s++;
                  ifp++;
            }
            ifp++;
            char *pfp = (char *)malloc(ifp);

            s = (char *)buf;
            char *t = pfp;
            for(int ic = 0 ; ic < ifp ; ic++)
                  *t++ = *s++;

            ifs->Read(pe, sizeof(ChartTableEntry));
            pe->pFullPath = pfp;

            int nplies = pe->nPlyEntries;

            float *pfe = (float *)malloc(nplies * sizeof(float) * 2);
            ifs->Read(pfe, nplies * sizeof(float) * 2);
            pe->pPlyTable = pfe;

            //  Read Aux Ply Table Entries
            if(pe->nAuxPlyEntries)
            {
                pe->pAuxCntTable = (int *)malloc(pe->nAuxPlyEntries * sizeof(int));
                ifs->Read(pe->pAuxCntTable, pe->nAuxPlyEntries * sizeof(int));

                float **pfp = (float **)malloc(pe->nAuxPlyEntries * sizeof(float *));

                for(int j=0 ; j<pe->nAuxPlyEntries ; j++)
                {
                    float *pfl = (float *)malloc(pe->pAuxCntTable[j] * 2 * sizeof(float));
                    pfp[j] = pfl;
                    ifs->Read(pfl, pe->pAuxCntTable[j] * 2 * sizeof(float));
                }
                pe->pAuxPlyTable = pfp;
            }



            pe++;
      }

      nEntry = n_entries;


      delete ifs;

      bValid = true;
      return true;
}


// ----------------------------------------------------------------------------
// Save Chart Table Database to binary .DAT file
// ----------------------------------------------------------------------------
bool ChartDB::SaveBinary(wxString *filename, wxArrayString *pChartDirArray)
{
      wxFileOutputStream *ofs;

//    Verify that the target directory exists, make it if not
      wxFileName file(*filename);
//    Note the explicit cast on GetPath argument, required by MSVC???
      wxFileName dir(file.GetPath((int)wxPATH_GET_SEPARATOR, wxPATH_NATIVE));

      if( ! dir.DirExists() )
      {
            if(! dir.Mkdir())
                  return false;
      }



      ofs = new wxFileOutputStream(*filename);        // open the file as write only

      if(!ofs->Ok())
            return false;

      ChartTableHeader cth;
      cth.nTableEntries = nEntry;
      cth.nDirEntries = pChartDirArray->GetCount();

      char *ctp = (char *)(&cth.dbVersion);
      char vb[5];
      sprintf(vb, "V%03d", DB_VERSION);
      for(int i = 0 ; i<4 ; i++)
        *ctp++ = vb[i];


      ofs->Write(&cth, sizeof(ChartTableHeader));

//    Write source directory names
      for(int iDir = 0 ; iDir < cth.nDirEntries ; iDir++)
      {
            wxString dir = pChartDirArray->Item(iDir);
            int ldir = dir.Len();
            ofs->Write(&ldir, sizeof(int));                 // length
            ofs->Write(dir.c_str(), ldir);                  // name
      }



      ChartTableEntry *pe = pChartTable;
      for(int iEntry = 0 ; iEntry < nEntry ; iEntry++)
      {
            char *s = pe->pFullPath;

            while(*s)
            {
                  ofs->PutC(*s);
                  s++;
            }
            ofs->PutC(0);

            ofs->Write(pe, sizeof(ChartTableEntry));

            int nplies = pe->nPlyEntries;
            float *pfe = pe->pPlyTable;
            ofs->Write(pfe, nplies * sizeof(float) * 2);

            //  Write Aux Ply Table Entries
            if(pe->nAuxPlyEntries)
            {
                ofs->Write(pe->pAuxCntTable, pe->nAuxPlyEntries * sizeof(int));
                for(int j=0 ; j<pe->nAuxPlyEntries ; j++)
                {
                    float *pfl = pe->pAuxPlyTable[j];
                    ofs->Write(pfl, pe->pAuxCntTable[j] * 2 * sizeof(float));
                }
            }


            pe++;
      }

      delete ofs;

      return true;
}

// ----------------------------------------------------------------------------
// Create Chart Table Database by directory search
//    resulting in valid pChartTable in (this)
// ----------------------------------------------------------------------------
bool ChartDB::Create(wxArrayString *dir_list, bool bshow_prog)
{
      pChartTable = (ChartTableEntry *)malloc(sizeof(ChartTableEntry));
      nEntry = 0;


     //  Get the new charts
      for(unsigned int j=0 ; j<dir_list->GetCount() ; j++)
      {
            wxString dir_name = dir_list->Item(j);

            TraverseDirAndAddS57(dir_name, bshow_prog, false);

            TraverseDirAndAddBSB(dir_name, bshow_prog, false);
      }           //for

      bValid = true;
      return true;

}

// ----------------------------------------------------------------------------
// Update existing ChartTable Database by directory search
//    resulting in valid pChartTable in (this)
// ----------------------------------------------------------------------------
bool ChartDB::Update(wxArrayString *dir_list, bool bshow_prog)
{
    int i;

    if(NULL == pChartTable)
    {
        pChartTable = (ChartTableEntry *)malloc(sizeof(ChartTableEntry));
        nEntry = 0;
    }

    //  Mark all charts provisionally invalid
    for(i=0 ; i<nEntry ; i++)
    {
        pChartTable[i].bValid = false;
    }


    //  Get the new charts

    for(unsigned int j=0 ; j<dir_list->GetCount() ; j++)
    {
        wxString dir_name = dir_list->Item(j);

//        SearchDirAndAddS57FromCatalog(dir_name, bshow_prog);
        TraverseDirAndAddS57(dir_name, bshow_prog, true);

        TraverseDirAndAddBSB(dir_name, bshow_prog, true);
    }           //for

    //  It is likely that there are duplicate charts in the list now.
    //  These occur as identical large area charts are included in multiple
    //  Maptech Regions.  For example, the GreatLakes overview chart NOAA 14500
    //  is found in Maptech Regions 81 (Lake Superior), 82 (Lake Huron), 83
    //  (Lake Erie), etc....

    //  So, look for identical charts and mark redundants as invalid
    for(int ic=0 ; ic<nEntry - 1 ; ic++)
    {
        if(pChartTable[ic].bValid)
        {
            for(int jc=ic + 1 ; jc<nEntry ; jc++)
            {
                if(pChartTable[jc].bValid)
                {
                    if(!strcmp(pChartTable[ic].ChartID, pChartTable[jc].ChartID))
                    {
                        //  Todo  Could be smarter here and check chart dates????
                        pChartTable[jc].bValid = false;
//                        printf("Duplicate %s,  %s vs %s\n",pChartTable[jc].ChartID, pChartTable[jc].pFullPath,   pChartTable[ic].pFullPath);
                    }
                }
            }
        }
     }



    //  Count the valid charts
    int iValid = 0;
    for(i=0 ; i<nEntry ; i++)
    {
        if(pChartTable[i].bValid)
            iValid++;
    }

    // Allocate a new chart table
    ChartTableEntry *pTempChartTable = (ChartTableEntry *)malloc((iValid) * sizeof(ChartTableEntry));
    ChartTableEntry *pEntry = pTempChartTable;

    // compress the existing table into the new table
    int nNewEntry = 0;
    for(i=0 ; i<nEntry ; i++)
    {
        if(pChartTable[i].bValid)
        {
            memcpy(pEntry, (const void *)(&pChartTable[i]), sizeof(ChartTableEntry));

            pEntry++;
            nNewEntry++;
        }
    }

    free(pChartTable);
    pChartTable = pTempChartTable;
    nEntry = nNewEntry;

    bValid = true;
    return true;
}






// ----------------------------------------------------------------------------
//  Traverse the given directory looking for BSB charts
//  If bupdate is true, also search the existing database for a name match.
//  If target chart is already in database, mark the entry valid and skip additional processing
// ----------------------------------------------------------------------------

int ChartDB::TraverseDirAndAddBSB(wxString dir_name, bool bshow_prog, bool bupdate)
//    Use Traverser to list of sub-directories
//    Thus handling the case where user has all charts in one folder with subdirs,
//    And wants all possible charts installed in one fell swoop.
{
    int nAdd = 0;
    wxArrayString dirs;
    wxArrayString files;
    dirs.Add(dir_name);                 // add entry tree root to the list always

    wxDir dir(dir_name);                // Make a searchable dir

    if ( !dir.IsOpened() )
    {
        return 0;
    }

    if(dir.HasSubDirs())
    {
        wxDirTraverserCharts traverser(files, dirs);
        dir.Traverse(traverser);
    }
    for(unsigned int id = 0 ; id < dirs.GetCount() ; id++)
    {
        wxString this_dir(dirs[id]);
        this_dir.Append('/');
        nAdd += SearchDirAndAddBSB(this_dir, wxString("*.geo"), bshow_prog, bupdate);

        nAdd += SearchDirAndAddBSB(this_dir, wxString("*.KAP"), bshow_prog, bupdate);
    }
//          wxString filespec_geo("*.geo");
//          SearchDirAndAddBSB(dir_name, filespec_geo, bshow_prog);


// TODO Add code to prevent duplicate charts coming into the database
//          Duplicate meaning "the SAME FILE", considering VFAT's file naming conventions
//          For now, though, just assume file extensions are upper case "KAP"

//          wxString filespec_kap("*.kap");
//          SearchDirAndAddBSB(dir_name, filespec_kap);

//          SearchDirAndAddBSB(dir_name,  (const wxString &)wxString("*.KAP"), bshow_prog);



    return nAdd;
}



// ----------------------------------------------------------------------------
// Populate Chart Table by directory search for specified BSB file type
// If bupdate flag is true, search the Chart Table for matching chart.
//  if target chart is already in table, mark it valid and skip chart processing
// ----------------------------------------------------------------------------

int ChartDB::SearchDirAndAddBSB(wxString& dir_name, const wxString& filespec,
                                                      bool bshow_prog, bool bupdate)
{
      wxString filename;
      wxDir dir(dir_name);

      wxProgressDialog *pprog = NULL;

//    Count the files to setup progress dialog
      int nFile = 0;
      bool cont = dir.GetFirst(&filename, filespec);
      while ( cont )
      {
            nFile++;
            cont = dir.GetNext(&filename);
      }

      if(!nFile)
            return 0;


      ChartTableEntry *pTempChartTable = (ChartTableEntry *)calloc((nFile ) * sizeof(ChartTableEntry), 1);
      ChartTableEntry *pEntry = pTempChartTable;
      int nDirEntry = 0;

      int nFileLook = 0;
      cont = dir.GetFirst(&filename, filespec);
      while ( cont )
      {
            wxString full_name = dir_name;
            full_name.Append(filename);

            bool bAdd = true;
            if(bupdate)
            {
                for(int i=0 ; i<nEntry ; i++)
                {
                    if(!strcmp(full_name.c_str(), pChartTable[i].pFullPath))
                    {
                        pChartTable[i].bValid = true;
                        bAdd = false;
                        break;
                    }

                    //  Look at the chart name itself for a further check
                    //  Todo  think about comparing publish dates?
/*                    wxFileName base(full_name.c_str());
                    wxFileName target(pChartTable[i].pFullPath);
                    if(base.GetFullName() == target.GetFullName())
                    {
//                    printf("Comparing %s to %s\n",full_name.c_str(), pChartTable[i].pFullPath);
                        pChartTable[i].bValid = true;
                        bAdd = false;
                        break;
                    }
*/
                }
            }

            if(bAdd)
            {
                if(bshow_prog)
                {
                    if(NULL == pprog)
                       pprog = new wxProgressDialog (  _T("OpenCPN Add Chart"), full_name, nFile, NULL,
                            wxPD_AUTO_HIDE | wxPD_SMOOTH |
                            wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);

                    pprog->Update(nFileLook, full_name);
                }
                CreateBSBChartTableEntry(full_name, pEntry);
                pEntry++;
                nDirEntry++;
            }

            nFileLook++;
            cont = dir.GetNext(&filename);
      }

      ChartTableEntry *pt = (ChartTableEntry *)malloc( (nEntry + nDirEntry)*sizeof(ChartTableEntry));
      memcpy(pt, pChartTable, nEntry * sizeof(ChartTableEntry));
      memcpy((char *)pt + (nEntry * sizeof(ChartTableEntry)), pTempChartTable, nDirEntry * sizeof(ChartTableEntry));

      free(pChartTable);
      free(pTempChartTable);

      pChartTable = pt;
      nEntry += nDirEntry;

      delete pprog;
      return nDirEntry;
}





// ----------------------------------------------------------------------------
// Create BSB Chart Table entry by reading chart header info, etc.
// ----------------------------------------------------------------------------
bool ChartDB::CreateBSBChartTableEntry(wxString full_name, ChartTableEntry *pEntry)
{

      wxFileName fn(full_name);
      wxString charttype = fn.GetExt();
      ChartBaseBSB *pch = NULL;

      if(charttype.Upper() == "KAP")
      {
            pch = new ChartKAP;
            pEntry->ChartType = CHART_TYPE_KAP;
      }
      if(charttype.Upper() == "GEO")
      {
            pch = new ChartGEO;
            pEntry->ChartType = CHART_TYPE_GEO;
      }


      pch->Init(full_name, HEADER_ONLY, COLOR_SCHEME_DEFAULT);

      char *pt = (char *)malloc(full_name.Length() + 1);
      strcpy(pt, full_name.GetData());
      pEntry->pFullPath = pt;

      strcpy(pEntry->ChartID, fn.GetName());

      pEntry->Scale = pch->Chart_Scale;

      Extent ext;
      pch->GetChartExtent(&ext);

      pEntry->LatMax = ext.NLAT;
      pEntry->LatMin = ext.SLAT;
      pEntry->LonMin = ext.WLON;
      pEntry->LonMax = ext.ELON;

//          Fill in the PLY information

      pEntry->nPlyEntries = pch->GetnPlypoints();

      float *pf = (float *)malloc(2 * pch->GetnPlypoints() * sizeof(float));

      pEntry->pPlyTable = pf;

      float *pfe = pf;
      Plypoint *ppp = pch->GetPlyTable();

      for(int i=0 ; i<pch->GetnPlypoints() ; i++)
      {
            *pfe++ = ppp->ltp;
            *pfe++ = ppp->lnp;

            ppp++;
      }

      delete pch;

      pEntry->bValid = true;
      return true;
}


// ----------------------------------------------------------------------------
//  Traverse the given directory tree looking for S57 charts
//  If bupdate is true, also search the existing database for a name match.
//  If target chart is already in database, mark the entry valid and skip additional processing
// ----------------------------------------------------------------------------

int ChartDB::TraverseDirAndAddS57(wxString dir_name, bool bshow_prog, bool bupdate)
//    Use Traverser to list of sub-directories
//    Thus handling the case where user has all charts in one folder with subdirs,
//    And wants all possible charts installed in one fell swoop.
{
    int nAdd = 0;
    wxArrayString dirs;
    wxArrayString files;
    dirs.Add(dir_name);                 // add entry tree root to the list always

    wxDir dir(dir_name);                // Make a searchable dir

    if ( !dir.IsOpened() )
    {
        return 0;
    }

    if(dir.HasSubDirs())
    {
        wxDirTraverserCharts traverser(files, dirs);
        dir.Traverse(traverser);
    }
//    int a = dirs.GetCount();
//    int b = files.GetCount();

    //  This search will recursively search the selected directory
    nAdd += SearchDirAndAddS57(dir_name, bshow_prog, bupdate);

/*
        for(unsigned int id = 0 ; id < dirs.GetCount() ; id++)
    {
        wxString this_dir(dirs[id]);
        this_dir.Append('/');
        nAdd += SearchDirAndAddS57(this_dir, bshow_prog, bupdate);
    }
*/
    return nAdd;
}




// ----------------------------------------------------------------------------
// Update Chart Table by directory search for s57 charts
// ----------------------------------------------------------------------------

int ChartDB::SearchDirAndAddS57(wxString& dir_name, bool bshow_prog, bool bupdate)
{
      wxString filename;
      wxDir dir(dir_name);

      wxProgressDialog *pprog = NULL;

//    Count the files to setup progress dialog

      wxArrayString *pFileList = new wxArrayString();

      dir.GetAllFiles(dir_name, pFileList);
      int nFile = 0;

      for(unsigned int j=0 ; j<pFileList->GetCount() ; j++)
      {
            wxFileName file(pFileList->Item(j));
            if(file.GetExt() == "000")
            {
                  nFile++;
            }
      }

      if(!nFile)
      {
            delete pFileList;
            return false;
      }


      ChartTableEntry *pTempChartTable = (ChartTableEntry *)calloc((nFile ) * sizeof(ChartTableEntry), 1);
      ChartTableEntry *pEntry = pTempChartTable;
      int n000Entry = 0;

      int iProg = 0;
      for(unsigned int ifile=0 ; ifile < pFileList->GetCount() ; ifile++)
      {
          wxFileName file(pFileList->Item(ifile));
          if(file.GetExt() == "000")
          {
              bool bAdd = true;
              if(bupdate)
              {
                  for(int i=0 ; i<nEntry ; i++)
                  {
                      if(!strcmp(file.GetFullPath(), pChartTable[i].pFullPath))
                      {
                          pChartTable[i].bValid = true;
                          bAdd = false;
                          break;
                      }
                  }
              }

              if(bAdd)
              {
                  if(bshow_prog)
                  {
                        if(NULL == pprog)
                            pprog = new wxProgressDialog (  _T("OpenCPN Add Chart"), file.GetFullPath(), nFile, NULL,
                                wxPD_AUTO_HIDE | wxPD_SMOOTH |
                                wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);
                        pprog->Update(iProg, file.GetFullPath());
                  }
                  CreateS57ChartTableEntry(file.GetFullPath(), pEntry, NULL);
                  n000Entry++;
                  pEntry++;
              }
              iProg++;
          }
      }




      ChartTableEntry *pt = (ChartTableEntry *)malloc( (nEntry + n000Entry)*sizeof(ChartTableEntry));
      memcpy(pt, pChartTable, nEntry * sizeof(ChartTableEntry));
      memcpy((char *)pt + (nEntry * sizeof(ChartTableEntry)), pTempChartTable, n000Entry * sizeof(ChartTableEntry));

      free(pChartTable);
      free(pTempChartTable);

      pChartTable = pt;
      nEntry += n000Entry;


      delete pFileList;
      delete pprog;

      return n000Entry;
}

// ----------------------------------------------------------------------------
// Create S57 Chart Table entry by reading chart header info, etc.
// ----------------------------------------------------------------------------
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_ARRAY(float*, MyFloatPtrArray);

bool ChartDB::CreateS57ChartTableEntry(wxString full_name, ChartTableEntry *pEntry, Extent *pext)
{
#ifdef USE_S57
      wxFileName fn(full_name);

//    Quick sanity check for file existence
      if(!fn.FileExists())
            return false;

      pEntry->ChartType = CHART_TYPE_S57;

      char *pt = (char *)malloc(full_name.Length() + 1);
      strcpy(pt, full_name.GetData());
      pEntry->pFullPath = pt;

      strcpy(pEntry->ChartID, fn.GetName());

      pEntry->Scale = s57_GetChartScale((char *)full_name.c_str());

      OGRDataSource *pDS;
      OGRFeature *pFeat;
      OGRFeature *pLastFeat;
      OGRLayer *pLayer;
      int catcov;

//    If Extent pointer passed is null, get extents the hard way
      if(pext == NULL)
      {

          //Get the first M_COVR object
          if(   s57_GetChartFirstM_COVR((char *)full_name.c_str(), &pDS, &pFeat, &pLayer, catcov))
            {

                OGRPolygon *poly;
                OGRLinearRing *xring;
                int npt;
                float *pf;

                //  Create arrays to hold geometry objects temporarily
                MyFloatPtrArray *pAuxPtrArray = new MyFloatPtrArray;
                wxArrayInt *pAuxCntArray = new wxArrayInt;


                float LatMax, LatMin, LonMax, LonMin;
                LatMax = -90.;
                LatMin = 90.;
                LonMax = -179.;
                LonMin = 179.;

                //  Record the first M_COVR, CATCOV=1 object
                if(catcov == 1)
                {

                  poly = (OGRPolygon *)(pFeat->GetGeometryRef());
                  xring = poly->getExteriorRing();

                  npt = xring->getNumPoints();

                  pf = (float *)malloc(2 * npt * sizeof(float));

                  pAuxPtrArray->Add(pf);
                  pAuxCntArray->Add(npt);

                  for(int i = 0 ; i < npt ; i++)
                  {
                        OGRPoint p;
                        xring->getPoint(i, &p);

                        LatMax = fmax(LatMax, p.getY());
                        LatMin = fmin(LatMin, p.getY());
                        LonMax = fmax(LonMax, p.getX());
                        LonMin = fmin(LonMin, p.getX());

                        *pf++ = p.getY();             // lat
                        *pf++ = p.getX();             // lon
                  }
                }


                  pLastFeat = pFeat;

                  while( s57_GetChartNextM_COVR(pDS, pLayer, pLastFeat, &pFeat, catcov))
                  {
                      delete pLastFeat;
                      pLastFeat = pFeat;

                      if(catcov == 1)
                      {
                        //    Get the next M_COVR feature, and create possible entry for pAuxPlyTable
                        poly = (OGRPolygon *)(pFeat->GetGeometryRef());
                        xring = poly->getExteriorRing();

                        npt = xring->getNumPoints();

                        pf = (float *)malloc(2 * npt * sizeof(float));

                        pAuxPtrArray->Add(pf);
                        pAuxCntArray->Add(npt);

                        for(int i = 0 ; i < npt ; i++)
                        {
                            OGRPoint p;
                            xring->getPoint(i, &p);

                            LatMax = fmax(LatMax, p.getY());
                            LatMin = fmin(LatMin, p.getY());
                            LonMax = fmax(LonMax, p.getX());
                            LonMin = fmin(LonMin, p.getX());

                            *pf++ = p.getY();             // lat
                            *pf++ = p.getX();             // lon
                        }
                      }


                  }         // while

                  delete pLastFeat;

                  //    If only one M_COVR,CATCOV=1 object was found,
                  //    assign the geometry to the Primary PlyTable

                  if(pAuxCntArray->GetCount() == 1)
                  {
                      pEntry->pPlyTable = (float *)pAuxPtrArray->Item(0);
                      pEntry->nPlyEntries = pAuxCntArray->Item(0);
                      pEntry->nAuxPlyEntries = 0;

                  }


                  else if(pAuxCntArray->GetCount() > 1)
                  {
                        //    Create new artificial Ply table from LatLon MinMax
                        pEntry->nPlyEntries = 4;
                        float *pf1 = (float *)malloc(2 * 4 * sizeof(float));
                        free (pEntry->pPlyTable);
                        pEntry->pPlyTable = pf1;

                        float *pfe = pf1;
                        *pfe++ = LatMax;
                        *pfe++ = LonMin;

                        *pfe++ = LatMax;
                        *pfe++ = LonMax;

                        *pfe++ = LatMin;
                        *pfe++ = LonMax;

                        *pfe++ = LatMin;
                        *pfe++ = LonMin;


                        //    Fill in the structure for pAuxPlyTable

                        unsigned int nAuxPlyEntries = pAuxCntArray->GetCount();
                        float **pfp = (float **)malloc(nAuxPlyEntries * sizeof(float *));
                        float **pft0 = pfp;
                        int *pip = (int *)malloc(nAuxPlyEntries * sizeof(int));

                        for(unsigned int j=0 ; j<nAuxPlyEntries ; j++)
                        {
                            pft0[j] = (float *)pAuxPtrArray->Item(j);
                            pip[j] = pAuxCntArray->Item(j);
                        }

                        pEntry->pAuxPlyTable = pfp;
                        pEntry->pAuxCntTable = pip;
                        pEntry->nAuxPlyEntries = nAuxPlyEntries;
                  }

                  else                                     // strange case, found no CATCOV=1 M_COVR objects
                  {
                      pEntry->nPlyEntries = 4;
                      float *pf = (float *)malloc(2 * 4 * sizeof(float));
                      pEntry->pPlyTable = pf;
                      float *pfe = pf;

                      *pfe++ = LatMax;
                      *pfe++ = LonMin;

                      *pfe++ = LatMax;
                      *pfe++ = LonMax;

                      *pfe++ = LatMin;
                      *pfe++ = LonMax;

                      *pfe++ = LatMin;
                      *pfe++ = LonMin;

                  }


                  pEntry->LatMax = LatMax;
                  pEntry->LatMin = LatMin;
                  pEntry->LonMin = LonMin;
                  pEntry->LonMax = LonMax;

                  delete pDS;

                  delete pAuxPtrArray;
                  delete pAuxCntArray;
            }

            //  There is no M_COVR object in the Exchange Set.
            //  So, Look for OGR Extents
            else
            {
                  Extent ext;
                  s57_GetChartExtent((char *)full_name.c_str(), &ext);

                  pEntry->LatMax = ext.NLAT;
                  pEntry->LatMin = ext.SLAT;
                  pEntry->LonMin = ext.WLON;
                  pEntry->LonMax = ext.ELON;

                  pEntry->nPlyEntries = 4;
                  float *pf = (float *)malloc(2 * 4 * sizeof(float));
                  pEntry->pPlyTable = pf;
                  float *pfe = pf;

                  *pfe++ = ext.NLAT;
                  *pfe++ = ext.WLON;

                  *pfe++ = ext.NLAT;
                  *pfe++ = ext.ELON;

                  *pfe++ = ext.SLAT;
                  *pfe++ = ext.ELON;

                  *pfe++ = ext.SLAT;
                  *pfe++ = ext.WLON;

            }
      }

      else                                            // Extents passed in from somewhere...
      {                                                     // so use them
            pEntry->LatMax = pext->NLAT;
            pEntry->LatMin = pext->SLAT;
            pEntry->LonMin = pext->WLON;
            pEntry->LonMax = pext->ELON;

            pEntry->nPlyEntries = 4;

            float *pf = (float *)malloc(2 * 4 * sizeof(float));

            pEntry->pPlyTable = pf;

            float *pfe = pf;

            *pfe++ = pext->NLAT;
            *pfe++ = pext->WLON;

            *pfe++ = pext->NLAT;
            *pfe++ = pext->ELON;

            *pfe++ = pext->SLAT;
            *pfe++ = pext->ELON;

            *pfe++ = pext->SLAT;
            *pfe++ = pext->WLON;


      }

      pEntry->bValid = true;

      return true;

#else
      return false;
#endif


}


// ----------------------------------------------------------------------------
// Update Chart Table by Catalog File search for s57 charts
// ----------------------------------------------------------------------------

bool ChartDB::SearchDirAndAddS57FromCatalog(wxString& dir_name, bool bshow_prog)
{
#if 0
    wxString filename;
    wxDir dir(dir_name);

    wxProgressDialog *pprog = NULL;
    wxProgressDialog *pprogA = NULL;

//    Count the files to setup progress dialog

    wxArrayString *pFileList = new wxArrayString();

    dir.GetAllFiles(dir_name, pFileList);
    int nFile = 0;

    for(unsigned int j=0 ; j<pFileList->GetCount() ; j++)
    {
        wxFileName file(pFileList->Item(j));
        if(file.GetExt() == "031")
        {
            nFile++;
        }
    }

    if(!nFile)
    {
        delete pFileList;
        return false;
    }

    if(bshow_prog)
        pprog = new wxProgressDialog (  _T("OpenCPN Chart Install"),
                                        _T("Message"),
                                        nFile, NULL,
                                        wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME
                                     );


      //    Create two lists to accumulate files/extents
    wxArrayString *pS57FileList = new wxArrayString();
    wxArrayPtrVoid *pS57ExtList = new wxArrayPtrVoid();

      // Read the list of Catalog files, parsing as we go

    int i031file = 0;
    for(unsigned int i=0 ; i<pFileList->GetCount() ; i++)
    {
        wxFileName file(pFileList->Item(i));
        if(file.GetExt() == "031")
        {
            i031file++;
            if(bshow_prog)
                pprog->Update(i031file, file.GetFullPath());

      // Read the ISO8211 Catalog file, looking for....

            DDFModule   oModule;
            DDFRecord   *poRecord;

            if( !oModule.Open(file.GetFullPath().c_str()) )
                return false;

            poRecord = oModule.ReadRecord();
            if( poRecord == NULL )
                return false;



            /* -------------------------------------------------------------------- */
            /*      We presumably have a catalog.  It contains paths to files       */
            /*      that generally lack the ENC_ROOT component.  Try to find the    */
            /*      correct name for the ENC_ROOT directory if available and        */
            /*      build a base path for our purposes.                             */
            /* -------------------------------------------------------------------- */
/*
            char        *pszCatDir = CPLStrdup( CPLGetPath( pszDataset ) );
            char        *pszRootDir = NULL;

            if( CPLStat( CPLFormFilename(pszCatDir,"ENC_ROOT",NULL), &sStatBuf ) == 0
            && VSI_ISDIR(sStatBuf.st_mode) )
            {
            pszRootDir = CPLStrdup(CPLFormFilename( pszCatDir, "ENC_ROOT", NULL ));
        }
            else if( CPLStat( CPLFormFilename( pszCatDir, "enc_root", NULL ),
            &sStatBuf ) == 0 && VSI_ISDIR(sStatBuf.st_mode) )
            {
            pszRootDir = CPLStrdup(CPLFormFilename( pszCatDir, "enc_root", NULL ));
        }

            if( pszRootDir )
            CPLDebug( "S57", "Found root directory to be %s.",
            pszRootDir );
*/
            /* -------------------------------------------------------------------- */
            /*      We have a catalog.  Scan it for data files, those with an       */
            /*      IMPL of BIN.  Shouldn't there be a better way of testing        */
            /*      whether a file is a data file or another catalog file?          */
            /* -------------------------------------------------------------------- */
            const char *pszNLAT, *pszSLAT, *pszELON, *pszWLON;

            for( ; poRecord != NULL; poRecord = oModule.ReadRecord() )
            {
                if( poRecord->FindField( "CATD" ) != NULL
                    && EQUAL(poRecord->GetStringSubfield("CATD",0,"IMPL",0),"BIN") )
                {
                    const char  *pszFile;

                    pszFile = poRecord->GetStringSubfield("CATD",0,"FILE",0);

                    pszNLAT = poRecord->GetStringSubfield("CATD",0,"NLAT",0);
                    pszSLAT = poRecord->GetStringSubfield("CATD",0,"SLAT",0);
                    pszELON = poRecord->GetStringSubfield("CATD",0,"ELON",0);
                    pszWLON = poRecord->GetStringSubfield("CATD",0,"WLON",0);

/*
           // Often there is an extra ENC_ROOT in the path, try finding
            // this file.

                    const char *pszWholePath;
                    pszWholePath = CPLFormFilename( pszCatDir, pszFile, NULL );
                    if( CPLStat( pszWholePath, &sStatBuf ) != 0
                    && pszRootDir != NULL )
                    {
                    pszWholePath = CPLFormFilename( pszRootDir, pszFile, NULL );
                }

                    if( CPLStat( pszWholePath, &sStatBuf ) != 0 )
                    {
                    CPLError( CE_Warning, CPLE_OpenFailed,
                    "Can't find file %s from catalog %s.",
                    pszFile, pszDataset );
                    continue;
                }

  */


//            papszRetList = CSLAddString( papszRetList, pszWholePath );
//            CPLDebug( "S57", "Got path %s from CATALOG.", pszWholePath );


                    Extent *pext = new Extent();

                    float NLAT, SLAT, ELON, WLON;

                    sscanf(pszNLAT, "%f", &NLAT);
                    sscanf(pszSLAT, "%f", &SLAT);
                    sscanf(pszELON, "%f", &ELON);
                    sscanf(pszWLON, "%f", &WLON);

                    pext->NLAT = NLAT;
                    pext->SLAT = SLAT;
                    pext->ELON = ELON;
                    pext->WLON = WLON;


                    pS57ExtList->Add((void *)pext);

                        // Check for Dos file name separator

                    wxString sfn(pszFile);
                    if(sfn.Contains("\\"))
                    {
                        sfn.Replace("\\", "/");
                    }


                    wxString S57FileString(dir_name);
                    S57FileString += sfn;
                    pS57FileList->Add(S57FileString);

//                      wxFileName S57FileName(S57FileString);
//                      CreateS57ChartTableEntry(S57FileName.GetFullPath(), pEntry, &ext);

                }           // if


            }                 // for


            oModule.Close();

            ////////////////////////////////////////////






        }
    }

      //    Get count of all files in list, which will include the 001,002 updates
    int nPotentialFiles = pS57FileList->GetCount();

    if(bshow_prog)
        pprogA = new wxProgressDialog (  _T("OpenCPN Add Chart"),
                                         _T("Message"),
                                         nPotentialFiles, NULL,
                                         wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME
                                      );



    ChartTableEntry *pTempChartTable = (ChartTableEntry *)malloc((nPotentialFiles ) * sizeof(ChartTableEntry));
    ChartTableEntry *pEntry = pTempChartTable;
    int n000Entry = 0;

      // Iterate over the accumulated lists, building ChartTable Entries

    for(unsigned int k=0 ; k<pS57FileList->GetCount() ; k++)
    {
        wxFileName file(pS57FileList->Item(k));
        if(file.GetExt() == "000")
        {
//                Extent *pext = (Extent *)pS57ExtList->Item(k);

//                wxLogMessage("Adding S57 Chart: %s, Extent is: %f, %f, %f, %f",
//                             file.GetFullPath().c_str(),
//                                        pext->NLAT,pext->SLAT,pext->ELON,pext->WLON);
            if(CreateS57ChartTableEntry(file.GetFullPath(), pEntry, (Extent *)(pS57ExtList->Item(k))))
            {
                n000Entry++;
                pEntry++;
            }

            if(bshow_prog)
                pprogA->Update(k/2, file.GetFullPath());

        }
    }

      //    Look for and mark any duplicates

    ChartTableEntry *pe = pTempChartTable;
    for(int iEntry = 0 ; iEntry < n000Entry ; iEntry++)
    {
        wxString nProto(pe->pFullPath);

        ChartTableEntry *pts = pe;
        pts++;
        for(int iTest = iEntry+1 ; iTest < n000Entry ; iTest++)
        {
            wxString nTest(pts->pFullPath);
            if(nTest == nProto)
            {
//                      wxLogMessage("Duplicate  %s", nTest.c_str());
                strcpy(pts->pFullPath, "DUP");

            }
            pts++;
        }
        pe++;

        if(bshow_prog)
            pprogA->Update(nPotentialFiles/2 + iEntry/2, "Removing Duplicates");

    }

      //now remove the "DUP"s

    pe = pTempChartTable;

    ChartTableEntry *pNewChartTable = (ChartTableEntry *)malloc((nPotentialFiles ) * sizeof(ChartTableEntry));
    ChartTableEntry *pNew = pNewChartTable;

    int   nNoDup000Entry = 0;

    for(int iRemove = 0 ; iRemove < n000Entry ; iRemove++)
    {
        if(strcmp(pe->pFullPath, "DUP"))                      // not a duplicate
        {
            memcpy(pNew, pe, sizeof(ChartTableEntry));
            pNew++;
            nNoDup000Entry++;
        }

        pe++;


    }






      //Re-allocate the master ChartTable, copy, and append new items

    ChartTableEntry *pt = (ChartTableEntry *)malloc( (nEntry + nNoDup000Entry)*sizeof(ChartTableEntry));
    memcpy(pt, pChartTable, nEntry * sizeof(ChartTableEntry));
    memcpy((char *)pt + (nEntry * sizeof(ChartTableEntry)), pNewChartTable,
            nNoDup000Entry * sizeof(ChartTableEntry));

    free(pChartTable);
    free(pTempChartTable);
    free(pNewChartTable);

    pChartTable = pt;
    nEntry += nNoDup000Entry;


    delete pS57FileList;
    pS57ExtList->Clear();
    delete pS57ExtList;

    delete pFileList;

    if(pprog)
        delete pprog;

    if(pprogA)
        delete pprogA;
#endif          //0
    return true;
}




//-------------------------------------------------------------------
//    Build Chart stack
//-------------------------------------------------------------------
int ChartDB::BuildChartStack(ChartStack * cstk, float lat, float lon)
{
      int i=0;
      int j=0;
      char buf[20];

      if(!bValid)
            return 0;                           // Database is not properly initialized

      for(i=0 ; i<nEntry ; i++)
      {
          wxString tt(pChartTable[i].pFullPath);
//          if (tt.Contains("5NY1CM"))
//              int ggl = 5;

//    First check on rough Bounding box
            if((lat < pChartTable[i].LatMax) &&
               (lat > pChartTable[i].LatMin) &&
               (lon > pChartTable[i].LonMin) &&
               (lon < pChartTable[i].LonMax) &&
               (j < MAXSTACK))

            {
//    Double check on Ply points polygon

                  bool bInside = G_FloatPtInPolygon((MyFlPoint *)pChartTable[i].pPlyTable,
                                                             pChartTable[i].nPlyEntries,
                                                             lon, lat);

                  if(bInside )
                  {
                      if(pChartTable[i].nAuxPlyEntries)
                      {
                          for(int k=0 ; k<pChartTable[i].nAuxPlyEntries ; k++)
                          {
                              bool bAuxInside = G_FloatPtInPolygon((MyFlPoint *)pChartTable[i].pAuxPlyTable[k],
                                                        pChartTable[i].pAuxCntTable[k],lon, lat);
                              if(bAuxInside)
                              {
                                  cstk->DBIndex[j] = i;
                                  strcpy(buf, pChartTable[i].ChartID);
                                  j++;
                                  break;
                              }
                          }

                      }
                      else
                      {
                            cstk->DBIndex[j] = i;
                            strcpy(buf, pChartTable[i].ChartID);
                            j++;
                      }
                  }

            }
      }

      cstk->nEntry = j;

//    Sort the stack on scale
      int swap = 1;
      int n,m,ti;
      while(swap == 1)
      {
            swap = 0;
            for(i=0 ; i<j-1 ; i++)
            {
                  m = cstk->DBIndex[i];
                  n = cstk->DBIndex[i+1];

                  if(pChartTable[n].Scale < pChartTable[m].Scale)
                  {
                        ti = cstk->DBIndex[i];
                        cstk->DBIndex[i] = cstk->DBIndex[i+1];
                        cstk->DBIndex[i+1] = ti;
                        swap = 1;
                  }
            }
      }


      return j;
}

//-------------------------------------------------------------------
//    Compare Chart Stacks
//-------------------------------------------------------------------
bool ChartDB::EqualStacks(ChartStack *pa, ChartStack *pb)
{
      if((pa == 0) || (pb == 0))
            return false;
      if(pa->nEntry != pb->nEntry)
            return false;

      for(int i=0 ; i<pa->nEntry ; i++)
      {
            if(pa->DBIndex[i] != pb->DBIndex[i])
                  return false;
      }

      return true;
}

//-------------------------------------------------------------------
//    Copy Chart Stacks
//-------------------------------------------------------------------
bool ChartDB::CopyStack(ChartStack *pa, ChartStack *pb)
{
      if((pa == 0) || (pb == 0))
            return false;
      pa->nEntry = pb->nEntry;

      for(int i=0 ; i<pa->nEntry ; i++)
            pa->DBIndex[i] = pb->DBIndex[i];

      return true;
}

//-------------------------------------------------------------------
//    Get Full Path
//-------------------------------------------------------------------
bool ChartDB::GetFullPath(ChartStack *ps, int stackindex, char *buf)
{
      int dbIndex = ps->DBIndex[stackindex];
      strcpy(buf, pChartTable[dbIndex].pFullPath);

      return true;
}

//-------------------------------------------------------------------
//    Get PlyPoint from stack
//-------------------------------------------------------------------
int ChartDB::GetCSPlyPoint(ChartStack *ps, int stackindex, int plyindex, float *lat, float *lon)
{
      int dbIndex = ps->DBIndex[stackindex];
      if(pChartTable[dbIndex].nPlyEntries)
      {
            float *fp = pChartTable[dbIndex].pPlyTable;
            fp += plyindex*2;
            *lat = *fp;
            fp++;
            *lon = *fp;
      }

      return pChartTable[dbIndex].nPlyEntries;
}

//-------------------------------------------------------------------
//    Get PlyPoint from Database
//-------------------------------------------------------------------
int ChartDB::GetDBPlyPoint(int dbIndex, int plyindex, float *lat, float *lon)
{
      if(pChartTable[dbIndex].nPlyEntries)
      {
            float *fp = pChartTable[dbIndex].pPlyTable;
            fp += plyindex*2;
            *lat = *fp;
            fp++;
            *lon = *fp;
      }

      return pChartTable[dbIndex].nPlyEntries;
}

//-------------------------------------------------------------------
//    Get Chart ID
//-------------------------------------------------------------------
bool ChartDB::GetChartID(ChartStack *ps, int stackindex, char *buf)
{
      int dbIndex = ps->DBIndex[stackindex];
      strcpy(buf, pChartTable[dbIndex].ChartID);

      return true;
}
//-------------------------------------------------------------------
//    Get Chart Scale
//-------------------------------------------------------------------
int ChartDB::GetStackChartScale(ChartStack *ps, int stackindex, char *buf)
{
      int dbIndex = ps->DBIndex[stackindex];
      int sc = pChartTable[dbIndex].Scale;
      sprintf(buf, "%d", sc);

      return sc;
}
//-------------------------------------------------------------------
//    Find ChartStack entry index corresponding to Full Path name, if present
//-------------------------------------------------------------------
int ChartDB::GetStackEntry(ChartStack *ps, wxString *pfp)
{
      for(int i=0 ; i<ps->nEntry ; i++)
      {
            int dbIndex = ps->DBIndex[i];
            if(pfp->IsSameAs( pChartTable[dbIndex].pFullPath) )
                  return i;
      }

      return -1;
}

//-------------------------------------------------------------------
//    Get CSChart Type
//-------------------------------------------------------------------
int ChartDB::GetCSChartType(ChartStack *ps, int stackindex)
{
      if(bValid)
      {
            int dbIndex = ps->DBIndex[stackindex];
            return pChartTable[dbIndex].ChartType;
      }
      else
            return 0;
}

//-------------------------------------------------------------------
//    Get DBChart Type
//-------------------------------------------------------------------
int ChartDB::GetDBChartType(int dbIndex)
{
      if(bValid)
            return pChartTable[dbIndex].ChartType;
      else
            return 0;
}

//-------------------------------------------------------------------
//    Open Chart
//-------------------------------------------------------------------

ChartBase *ChartDB::OpenChartFromStack(ChartStack *pStack, int StackEntry, ChartInitFlag init_flag)
{

      ChartBase *Ch;
      char buf[80];
      CacheEntry *pce;

      GetFullPath(pStack, StackEntry, buf);

      wxDateTime now = wxDateTime::Now();                   // get time for LRU use

      bool bInCache = false;

//    Search the cache

      unsigned int nCache = pChartCache->GetCount();
      for(unsigned int i=0 ; i<nCache ; i++)
      {
            pce = (CacheEntry *)(pChartCache->Item(i));
            if(pce->FullPath == wxString(buf))
            {
                  Ch = (ChartBase *)pce->pChart;
                  bInCache = true;
                  break;
            }
      }

      if(bInCache)
      {
          if(FULL_INIT == init_flag)                            // asking for full init?
          {
              if(Ch->IsReadyToRender())
              {
                    pce->RecentTime = now.GetTicks();           // chart is OK
                    return Ch;
              }
              else
              {
                    delete Ch;                                  // chart is not useable
                    pChartCache->Remove(pce);                   // so remove it
                    delete pce;
                    bInCache = false;
              }
          }
          else                                                  // assume if in cache, the chart can do thumbnails
          {
               pce->RecentTime = now.GetTicks();
               return Ch;
          }

      }

      if(!bInCache)                    // not in cache
      {

#ifndef __WXMSW__

#ifdef CACHE_MEM_LIMIT
#define MEM_FREE_THRESHOLD    CACHE_MEM_LIMIT             // in KBytes

          //    Check memory status to see if enough room to open another chart
            int omem_total, omem_used;
            pParent->GetMemoryStatus(omem_total, omem_used);
            int omem_free = omem_total - omem_used;
            if(omem_free < MEM_FREE_THRESHOLD)
            {
                  // Search the cache for oldest entry that is not Current_Ch
                  unsigned int nCache = pChartCache->GetCount();
                  if(nCache > 2)
                  {
//                      wxLogMessage("Searching chart cache for oldest entry");
                        int LRUTime = now.GetTicks();
                        int iOldest = 0;
                        for(unsigned int i=0 ; i<nCache ; i++)
                        {
                              pce = (CacheEntry *)(pChartCache->Item(i));
                              if((ChartBase *)(pce->pChart) != Current_Ch)
                              {
                                    if(pce->RecentTime < LRUTime)
                                    {
                                          LRUTime = pce->RecentTime;
                                          iOldest = i;
                                    }
                              }
                        }
//                      int dt = now.GetTicks() - LRUTime;
//                      wxLogMessage("Oldest cache index is %d, delta t is %d", iOldest, dt);

                        pce = (CacheEntry *)(pChartCache->Item(iOldest));
                        ChartBase *pDeleteCandidate =  (ChartBase *)(pce->pChart);

                        if(Current_Ch == pDeleteCandidate)
                        {
//                            wxLogMessage("...However, it is Current_Ch");
                        }
                        else
                        {
//                            wxLogMessage("Deleting/Removing oldest chart from cache");
//                            wxLogMessage("oMem_Free before chart removal is %d", omem_free);

                              //    Delete the chart
                              delete pDeleteCandidate;

                              //remove the cache entry
                              pChartCache->Remove(pce);

                              pParent->GetMemoryStatus(omem_total, omem_used);
//                            int omem_free = omem_total - omem_used;
//                            wxLogMessage("oMem_Free after chart removal is %d", omem_free);

                        }
                  }
            }

#endif      //CACHE_MEM_LIMIT

#ifdef CACHE_N_LIMIT
#define N_CACHE CACHE_N_LIMIT

//      Limit cache to n charts, tossing out the oldest when space is needed
            unsigned int nCache = pChartCache->GetCount();
            if(nCache >= N_CACHE)
            {
//                wxLogMessage("Searching chart cache for oldest entry");
                  int LRUTime = now.GetTicks();
                  int iOldest = 0;
                  for(unsigned int i=0 ; i<nCache ; i++)
                  {
                        pce = (CacheEntry *)(pChartCache->Item(i));
                        if((ChartBase *)(pce->pChart) != Current_Ch)
                        {
                              if(pce->RecentTime < LRUTime)
                              {
                                    LRUTime = pce->RecentTime;
                                    iOldest = i;
                              }
                        }
                  }

                  pce = (CacheEntry *)(pChartCache->Item(iOldest));
                  ChartBase *pDeleteCandidate =  (ChartBase *)(pce->pChart);

                  if(Current_Ch != pDeleteCandidate)
                  {
//                      wxLogMessage("Deleting/Removing oldest chart from cache");
//                      wxLogMessage("oMem_Free before chart removal is %d", omem_free);

                   //    Delete the chart
                        delete pDeleteCandidate;

                   //remove the cache entry
                        pChartCache->Remove(pce);
                  }
            }
#endif      //CACHE_N_LIMIT



#endif      // ndef __WXMSW__

            if(GetCSChartType(pStack, StackEntry) == CHART_TYPE_KAP)
                  Ch = new ChartKAP();

            else if(GetCSChartType(pStack, StackEntry) == CHART_TYPE_GEO)
                  Ch = new ChartGEO();

#ifdef USE_S57
            else if(GetCSChartType(pStack, StackEntry) == CHART_TYPE_S57)
            {
                  Ch = new s57chart();
                  int dbIndex = pStack->DBIndex[StackEntry];
                  Ch->Chart_Scale = pChartTable[dbIndex].Scale;

                  s57chart *Chs57 = dynamic_cast<s57chart*>(Ch);

                  Extent ext;
                  ext.NLAT = pChartTable[dbIndex].LatMax;
                  ext.SLAT = pChartTable[dbIndex].LatMin;
                  ext.WLON = pChartTable[dbIndex].LonMin;
                  ext.ELON = pChartTable[dbIndex].LonMax;
                  Chs57->SetFullExtent(ext);
            }
#endif

            else
                  Ch = NULL;


            if(Ch)
            {
                  InitReturn ir;
                  ir = Ch->Init(buf, init_flag, pParent->GetColorScheme());    // using the passed flag
                  if(INIT_OK == ir)
                  {
//    Only add to cache if requesting a full init
                        if(1/*FULL_INIT == init_flag*/)
                        {
                              pce = new CacheEntry;
                              pce->FullPath = wxString(buf);
                              pce->pChart = Ch;
                              pce->RecentTime = now.GetTicks();

                              pChartCache->Add((void *)pce);
                        }
                  }
                  else if(INIT_FAIL_REMOVE == ir)                 // some problem in chart Init()
                  {
                        delete Ch;
                        Ch = NULL;

//          Mark this chart in the database, so that it will not be seen during this run
                        int dbIndex = pStack->DBIndex[StackEntry];
                        const char *pChartToDisable = pChartTable[dbIndex].pFullPath;
                        DisableChart(pChartToDisable);


                  }
                  else if(INIT_FAIL_RETRY == ir)                  // recoverable problem in chart Init()
                  {
                        delete Ch;
                        Ch = NULL;
                  }

            }


            return Ch;
      }

      return NULL;
}

//-------------------------------------------------------------------
//    Disable Chart
//-------------------------------------------------------------------

int ChartDB::DisableChart(const char *pPathToDisable)
{
      //    Find the chart
      for(int i=0 ; i<nEntry ; i++)
      {
            if(!strcmp(pChartTable[i].pFullPath, pPathToDisable))
            {
//          Mark this chart in the database, so that it will not be seen during this run
//          How?  By setting the chart bounding box to an absurd value
//          Todo... Fix this heinous hack
                  pChartTable[i].LatMax = 0;
                  pChartTable[i].LatMin = .0001;

                  return 1;
            }
      }

      return 0;
}

void ChartDB::ApplyColorSchemeToCachedCharts(ColorScheme cs)
{
      ChartBase *Ch;
      CacheEntry *pce;
     //    Search the cache

      unsigned int nCache = pChartCache->GetCount();
      for(unsigned int i=0 ; i<nCache ; i++)
      {
            pce = (CacheEntry *)(pChartCache->Item(i));
            Ch = (ChartBase *)pce->pChart;
            if(Ch)
                  Ch->SetColorScheme(cs, true);

      }
}

//  Private version of PolyPt testing using floats instead of doubles

bool Intersect(MyFlPoint p1, MyFlPoint p2, MyFlPoint p3, MyFlPoint p4) ;
int CCW(MyFlPoint p0, MyFlPoint p1, MyFlPoint p2) ;

/*************************************************************************


 * FUNCTION:   G_PtInPolygon
 *
 * PURPOSE
 * This routine determines if the point passed is in the polygon. It uses

 * the classical polygon hit-testing algorithm: a horizontal ray starting

 * at the point is extended infinitely rightwards and the number of
 * polygon edges that intersect the ray are counted. If the number is odd,

 * the point is inside the polygon.
 *
 * RETURN VALUE
 * (bool) TRUE if the point is inside the polygon, FALSE if not.
 *************************************************************************/


 bool G_FloatPtInPolygon(MyFlPoint *rgpts, int wnumpts, float x, float y)

{

   MyFlPoint  *ppt, *ppt1 ;
   int   i ;
   MyFlPoint  pt1, pt2, pt0 ;
   int   wnumintsct = 0 ;

   pt0.x = x;
   pt0.y = y;

   pt1 = pt2 = pt0 ;
   pt2.x = 180.;                    // Good east till Hong Kong?

   // Now go through each of the lines in the polygon and see if it
   // intersects
   for (i = 0, ppt = rgpts ; i < wnumpts-1 ; i++, ppt++)
   {
      ppt1 = ppt;
        ppt1++;
        if (Intersect(pt0, pt2, *ppt, *(ppt1)))
         wnumintsct++ ;
   }

   // And the last line
   if (Intersect(pt0, pt2, *ppt, *rgpts))
      wnumintsct++ ;

   return (wnumintsct&1) ;

}


/*************************************************************************


 * FUNCTION:   Intersect
 *
 * PURPOSE
 * Given two line segments, determine if they intersect.
 *
 * RETURN VALUE
 * TRUE if they intersect, FALSE if not.
 *************************************************************************/


bool Intersect(MyFlPoint p1, MyFlPoint p2, MyFlPoint p3, MyFlPoint p4) {
      int i;
      i = CCW(p1, p2, p3);
      i = CCW(p1, p2, p4);
      i = CCW(p3, p4, p1);
      i = CCW(p3, p4, p2);
   return ((( CCW(p1, p2, p3) * CCW(p1, p2, p4)) <= 0)
        && (( CCW(p3, p4, p1) * CCW(p3, p4, p2)  <= 0) )) ;

}
/*************************************************************************


 * FUNCTION:   CCW (CounterClockWise)
 *
 * PURPOSE
 * Determines, given three points, if when travelling from the first to
 * the second to the third, we travel in a counterclockwise direction.
 *
 * RETURN VALUE
 * (int) 1 if the movement is in a counterclockwise direction, -1 if
 * not.
 *************************************************************************/


int CCW(MyFlPoint p0, MyFlPoint p1, MyFlPoint p2) {
   float dx1, dx2 ;
   float dy1, dy2 ;

   dx1 = p1.x - p0.x ; dx2 = p2.x - p0.x ;
   dy1 = p1.y - p0.y ; dy2 = p2.y - p0.y ;

   /* This is basically a slope comparison: we don't do divisions because

    * of divide by zero possibilities with pure horizontal and pure
    * vertical lines.
    */
   return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1) ;

}

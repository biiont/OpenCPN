/******************************************************************************
 * $Id: cutil.c,v 1.9 2008/08/09 23:56:08 bdbcat Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Extern C Linked Utilities
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
 *
 * $Log: cutil.c,v $
 * Revision 1.9  2008/08/09 23:56:08  bdbcat
 * Cleanup
 *
 * Revision 1.8  2008/04/10 01:05:22  bdbcat
 * Add mysnprintf
 *
 * Revision 1.7  2008/03/30 22:31:39  bdbcat
 * Cleanup
 *
 * Revision 1.6  2008/01/02 20:47:16  bdbcat
 * Add windows exception handler
 *
 * Revision 1.5  2007/05/03 13:23:55  dsr
 * Major refactor for 1.2.0
 *
 * Revision 1.4  2007/03/02 02:02:41  dsr
 * Cleanup
 *
 * Revision 1.3  2006/10/07 03:50:27  dsr
 * *** empty log message ***
 *
 * Revision 1.2  2006/10/01 03:22:58  dsr
 * no message
 *
 *
 *
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "cutil.h"


//      Fix up CPL_CVSID if not available
//      This code block taken directly from <cpl_port.h>
#ifndef CPL_CVSID
#ifndef NULL
#define NULL 0
#endif

/***********************************************************************
 * Define CPL_CVSID() macro.  It can be disabled during a build by
 * defining DISABLE_CPLID in the compiler options.
 *
 * The cvsid_aw() function is just there to prevent reports of cpl_cvsid()
 * being unused.
 */

#ifndef DISABLE_CVSID
#  define CPL_CVSID(string)     static char cpl_cvsid[] = string; \
static char *cvsid_aw() { return( cvsid_aw() ? ((char *) NULL) : cpl_cvsid ); }
#else
#  define CPL_CVSID(string)
#endif

#endif


CPL_CVSID("$Id: cutil.c,v 1.9 2008/08/09 23:56:08 bdbcat Exp $");

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
 * Polygon is assumed OPEN, not CLOSED.
 * RETURN VALUE
 * (bool) TRUE if the point is inside the polygon, FALSE if not.
 *************************************************************************/


 int  G_PtInPolygon(MyPoint *rgpts, int wnumpts, float x, float y)

{

   MyPoint  *ppt, *ppt1 ;
   int   i ;
   MyPoint  pt1, pt2, pt0 ;
   int   wnumintsct = 0 ;


   pt0.x = x;
   pt0.y = y;

   pt1 = pt2 = pt0 ;
   pt2.x = 1.e6;

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

              0
 * FUNCTION:   Intersect
 *
 * PURPOSE
 * Given two line segments, determine if they intersect.
 *
 * RETURN VALUE
 * TRUE if they intersect, FALSE if not.
 *************************************************************************/


int Intersect(MyPoint p1, MyPoint p2, MyPoint p3, MyPoint p4) {
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


int CCW(MyPoint p0, MyPoint p1, MyPoint p2) {
   double dx1, dx2 ;
   double dy1, dy2 ;

   dx1 = p1.x - p0.x ; dx2 = p2.x - p0.x ;
   dy1 = p1.y - p0.y ; dy2 = p2.y - p0.y ;

   /* This is a slope comparison: we don't do divisions because
    * of divide by zero possibilities with pure horizontal and pure
    * vertical lines.
    */
   return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1) ;

}

#define CTRUE -1
#define CFALSE 0

typedef enum {
    LEFT, RIGHT, BOTTOM, TOP
} edge;
typedef long outcode;


/* Local variables for cohen_sutherland_line_clip: */
struct LOC_cohen_sutherland_line_clip {
    double xmin, xmax, ymin, ymax;
} ;

void CompOutCode (double x, double y, outcode *code, struct LOC_cohen_sutherland_line_clip *LINK)
{
    /*Compute outcode for the point (x,y) */
    *code = 0;
    if (y > LINK->ymax)
        *code = 1L << ((long)TOP);
    else if (y < LINK->ymin)
        *code = 1L << ((long)BOTTOM);
    if (x > LINK->xmax)
        *code |= 1L << ((long)RIGHT);
    else if (x < LINK->xmin)
        *code |= 1L << ((long)LEFT);
}


ClipResult cohen_sutherland_line_clip_d (double *x0, double *y0, double *x1, double *y1,
                                         double xmin_, double xmax_, double ymin_, double ymax_)
{
      /* Cohen-Sutherland clipping algorithm for line P0=(x1,y0) to P1=(x1,y1)
    and clip rectangle with diagonal from (xmin,ymin) to (xmax,ymax).*/
    struct LOC_cohen_sutherland_line_clip V;
    int accept = CFALSE, done = CFALSE;
    ClipResult clip = Visible;
    outcode outcode0, outcode1, outcodeOut;
    /*Outcodes for P0,P1, and whichever point lies outside the clip rectangle*/
    double x=0., y=0.;

    V.xmin = xmin_;
    V.xmax = xmax_;
    V.ymin = ymin_;
    V.ymax = ymax_;
    CompOutCode(*x0, *y0, &outcode0, &V);
    CompOutCode(*x1, *y1, &outcode1, &V);
    do {
        if (outcode0 == 0 && outcode1 == 0) {   /*Trivial accept and exit*/
            accept = CTRUE;
            done = CTRUE;
        } else if ((outcode0 & outcode1) != 0) {
            clip = Invisible;
            done = CTRUE;
        }
        /*Logical intersection is true, so trivial reject and exit.*/
        else {
            clip = Visible;
                        /*Failed both tests, so calculate the line segment to clip;
            from an outside point to an intersection with clip edge.*/
            /*At least one endpoint is outside the clip rectangle; pick it.*/
            if (outcode0 != 0)
                outcodeOut = outcode0;
            else
                outcodeOut = outcode1;
                        /*Now find intersection point;
            use formulas y=y0+slope*(x-x0),x=x0+(1/slope)*(y-y0).*/

            if (((1L << ((long)TOP)) & outcodeOut) != 0) {
                /*Divide line at top of clip rectangle*/
                x = *x0 + (*x1 - *x0) * (V.ymax - *y0) / (*y1 - *y0);
                y = V.ymax;
            } else if (((1L << ((long)BOTTOM)) & outcodeOut) != 0) {
                /*Divide line at bottom of clip rectangle*/
                x = *x0 + (*x1 - *x0) * (V.ymin - *y0) / (*y1 - *y0);
                y = V.ymin;
            } else if (((1L << ((long)RIGHT)) & outcodeOut) != 0) {
                /*Divide line at right edge of clip rectangle*/
                y = *y0 + (*y1 - *y0) * (V.xmax - *x0) / (*x1 - *x0);
                x = V.xmax;
            } else if (((1L << ((long)LEFT)) & outcodeOut) != 0) {
                /*Divide line at left edge of clip rectangle*/
                y = *y0 + (*y1 - *y0) * (V.xmin - *x0) / (*x1 - *x0);
                x = V.xmin;
            }
                        /*Now we move outside point to intersection point to clip,
            and get ready for next pass.*/
            if (outcodeOut == outcode0) {
                *x0 = x;
                *y0 = y;
                CompOutCode(*x0, *y0, &outcode0, &V);
            } else {
                *x1 = x;
                *y1 = y;
                CompOutCode(*x1, *y1, &outcode1, &V);
            }
        }
    } while (!done);
    return clip;
}

ClipResult cohen_sutherland_line_clip_i (int *x0_, int *y0_, int *x1_, int *y1_,
                                         int xmin_, int xmax_, int ymin_, int ymax_)
{
    ClipResult ret;
    double x0,y0,x1,y1;
    x0 = *x0_;
    y0 = *y0_;
    x1 = *x1_;
    y1 = *y1_;
    ret = cohen_sutherland_line_clip_d (&x0, &y0, &x1, &y1,
                                         (double)xmin_, (double)xmax_,
                                         (double)ymin_, (double)ymax_);
    *x0_ = (int)x0;
    *y0_ = (int)y0;
    *x1_ = (int)x1;
    *y1_ = (int)y1;
    return ret;
}


double      round_msvc (double x)
{
    return(floor(x + 0.5));

}

#ifdef __WXMSW__
#include <windows.h>
#include <float.h>            // for _clear87()

long __stdcall MyUnhandledExceptionFilter( struct _EXCEPTION_POINTERS *ExceptionInfo )
{
//    return EXCEPTION_EXECUTE_HANDLER ;        // terminates the app

    switch(ExceptionInfo->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
        case EXCEPTION_FLT_UNDERFLOW:
           _clear87();
            return EXCEPTION_CONTINUE_EXECUTION ;     // retry

        default:
           return EXCEPTION_CONTINUE_SEARCH ;         // standard fatal dialog box
    }
}

#endif

/*          Replacement for __MSVC__ in absence of snprintf or _snprintf  */
#ifdef __MSVC__
extern int mysnprintf( char *buffer, int count, const char *format, ... )
{
      int ret;

      va_list arg;
      va_start(arg, format);
      ret = _vsnprintf(buffer, count, format, arg);

      va_end(arg);
      return ret;
}
#endif

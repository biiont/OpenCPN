/****************************************************************************/
/*                                                                          */
/*                                                                          */
/* This file has been extracted from gpstrans                               */
/* Parts are taken from John F. Waers (jfwaers@csn.net) program MacGPS.     */
/*                                                                          */
/*                                                                          */
/*    Copyright (c) 1995 by Carsten Tschach (tschach@zedat.fu-berlin.de)    */
/*                                                                          */
/* Permission  to use, copy,  modify, and distribute  this software and its */
/* documentation for non-commercial purpose, is hereby granted without fee, */
/* providing that the  copyright notice  appear in all copies and that both */
/* the  copyright notice and  this permission  notice appear in  supporting */
/* documentation.  I make no representations about  the suitability of this */
/* software  for any  purpose.  It is  provides "as is" without  express or */
/* implid warranty.                                                         */
/*                                                                          */
/****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "georef.h"
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

#ifdef __MSVC__
#define snprintf mysnprintf
#endif

CPL_CVSID("$Id: georef.c,v 1.21 2009/12/17 02:50:09 bdbcat Exp $");


/* For NAD27 shift table */
//extern struct CTABLE conus_ct[];

//  ellipsoid: index into the gEllipsoid[] array, in which
//             a is the ellipsoid semimajor axis
//             invf is the inverse of the ellipsoid flattening f
//  dx, dy, dz: ellipsoid center with respect to WGS84 ellipsoid center
//    x axis is the prime meridian
//    y axis is 90 degrees east longitude
//    z axis is the axis of rotation of the ellipsoid

// The following values for dx, dy and dz were extracted from the output of
// the GARMIN PCX5 program. The output also includes values for da and df, the
// difference between the reference ellipsoid and the WGS84 ellipsoid semi-
// major axis and flattening, respectively. These are replaced by the
// data contained in the structure array gEllipsoid[], which was obtained from
// the Defence Mapping Agency document number TR8350.2, "Department of Defense
// World Geodetic System 1984."

struct DATUM const gDatum[] = {
//         name               ellipsoid   dx      dy       dz
    { "Adindan",                5,   -162,    -12,    206 },    // 0
    { "Afgooye",               15,    -43,   -163,     45 },    // 1
    { "Ain el Abd 1970",       14,   -150,   -251,     -2 },    // 2
    { "Anna 1 Astro 1965",      2,   -491,    -22,    435 },    // 3
    { "Arc 1950",               5,   -143,    -90,   -294 },    // 4
    { "Arc 1960",               5,   -160,     -8,   -300 },    // 5
    { "Ascension Island ï¿½8",  14,   -207,    107,     52 },     // 6
    { "Astro B4 Sorol Atoll",  14,    114,   -116,   -333 },    // 7
    { "Astro Beacon ï¿½ï¿½",      14,    145,     75,   -272 },     // 8
    { "Astro DOS 71/4",        14,   -320,    550,   -494 },    // 9
    { "Astronomic Stn ï¿½2",    14,    124,   -234,    -25 },     // 10
    { "Australian Geod ï¿½6",    2,   -133,    -48,    148 },     // 11
    { "Australian Geod ï¿½4",    2,   -134,    -48,    149 },     // 12
    { "Bellevue (IGN)",        14,   -127,   -769,    472 },    // 13
    { "Bermuda 1957",           4,    -73,    213,    296 },    // 14
    { "Bogota Observatory",    14,    307,    304,   -318 },    // 15
    { "Campo Inchauspe",       14,   -148,    136,     90 },    // 16
    { "Canton Astro 1966",     14,    298,   -304,   -375 },    // 17
    { "Cape",                   5,   -136,   -108,   -292 },    // 18
    { "Cape Canaveral",         4,     -2,    150,    181 },    // 19
    { "Carthage",               5,   -263,      6,    431 },    // 20
    { "CH-1903",                3,    674,     15,    405 },    // 21
    { "Chatham 1971",          14,    175,    -38,    113 },    // 22
    { "Chua Astro",            14,   -134,    229,    -29 },    // 23
    { "Corrego Alegre",        14,   -206,    172,     -6 },    // 24
    { "Djakarta (Batavia)",     3,   -377,    681,    -50 },    // 25
    { "DOS 1968",              14,    230,   -199,   -752 },    // 26
    { "Easter Island 1967",    14,    211,    147,    111 },    // 27
    { "European 1950",         14,    -87,    -98,   -121 },    // 28
    { "European 1979",         14,    -86,    -98,   -119 },    // 29
    { "Finland Hayford",       14,    -78,   -231,    -97 },    // 30
    { "Gandajika Base",        14,   -133,   -321,     50 },    // 31
    { "Geodetic Datum ï¿½9",    14,     84,    -22,    209 },     // 32
    { "Guam 1963",              4,   -100,   -248,    259 },    // 33
    { "GUX 1 Astro",           14,    252,   -209,   -751 },    // 34
    { "Hjorsey 1955",          14,    -73,     46,    -86 },    // 35
    { "Hong Kong 1963",        14,   -156,   -271,   -189 },    // 36
    { "Indian Bangladesh",      6,    289,    734,    257 },    // 37
    { "Indian Thailand",        6,    214,    836,    303 },    // 38
    { "Ireland 1965",           1,    506,   -122,    611 },    // 39
    { "ISTS 073 Astro ï¿½9",    14,    208,   -435,   -229 },     // 40
    { "Johnston Island",       14,    191,    -77,   -204 },    // 41
    { "Kandawala",              6,    -97,    787,     86 },    // 42
    { "Kerguelen Island",      14,    145,   -187,    103 },    // 43
    { "Kertau 1948",            7,    -11,    851,      5 },    // 44
    { "L.C. 5 Astro",           4,     42,    124,    147 },    // 45
    { "Liberia 1964",           5,    -90,     40,     88 },    // 46
    { "Luzon Mindanao",         4,   -133,    -79,    -72 },    // 47
    { "Luzon Philippines",      4,   -133,    -77,    -51 },    // 48
    { "Mahe 1971",              5,     41,   -220,   -134 },    // 49
    { "Marco Astro",           14,   -289,   -124,     60 },    // 50
    { "Massawa",                3,    639,    405,     60 },    // 51
    { "Merchich",               5,     31,    146,     47 },    // 52
    { "Midway Astro 1961",     14,    912,    -58,   1227 },    // 53
    { "Minna",                  5,    -92,    -93,    122 },    // 54
    { "NAD27 Alaska",           4,     -5,    135,    172 },    // 55
    { "NAD27 Bahamas",          4,     -4,    154,    178 },    // 56
    { "NAD27 Canada",           4,    -10,    158,    187 },    // 57
    { "NAD27 Canal Zone",       4,      0,    125,    201 },    // 58
    { "NAD27 Caribbean",        4,     -7,    152,    178 },    // 59
    { "NAD27 Central",          4,      0,    125,    194 },    // 60
    { "NAD27 CONUS",            4,     -8,    160,    176 },    // 61
    { "NAD27 Cuba",             4,     -9,    152,    178 },    // 62
    { "NAD27 Greenland",        4,     11,    114,    195 },    // 63
    { "NAD27 Mexico",           4,    -12,    130,    190 },    // 64
    { "NAD27 San Salvador",     4,      1,    140,    165 },    // 65
    { "NAD83",                 11,      0,      0,      0 },    // 66
    { "Nahrwn Masirah Ilnd",    5,   -247,   -148,    369 },    // 67
    { "Nahrwn Saudi Arbia",     5,   -231,   -196,    482 },    // 68
    { "Nahrwn United Arab",     5,   -249,   -156,    381 },    // 69
    { "Naparima BWI",          14,     -2,    374,    172 },    // 70
    { "Observatorio 1966",     14,   -425,   -169,     81 },    // 71
    { "Old Egyptian",          12,   -130,    110,    -13 },    // 72
    { "Old Hawaiian",           4,     61,   -285,   -181 },    // 73
    { "Oman",                   5,   -346,     -1,    224 },    // 74
    { "Ord Srvy Grt Britn",     0,    375,   -111,    431 },    // 75
    { "Pico De Las Nieves",    14,   -307,    -92,    127 },    // 76
    { "Pitcairn Astro 1967",   14,    185,    165,     42 },    // 77
    { "Prov So Amrican ï¿½6",   14,   -288,    175,   -376 },     // 78
    { "Prov So Chilean ï¿½3",   14,     16,    196,     93 },     // 79
    { "Puerto Rico",            4,     11,     72,   -101 },    // 80
    { "Qatar National",        14,   -128,   -283,     22 },    // 81
    { "Qornoq",                14,    164,    138,   -189 },    // 82
    { "Reunion",               14,     94,   -948,  -1262 },    // 83
    { "Rome 1940",             14,   -225,    -65,      9 },    // 84
    { "RT 90",                  3,    498,    -36,    568 },    // 85
    { "Santo (DOS)",           14,    170,     42,     84 },    // 86
    { "Sao Braz",              14,   -203,    141,     53 },    // 87
    { "Sapper Hill 1943",      14,   -355,     16,     74 },    // 88
    { "Schwarzeck",            21,    616,     97,   -251 },    // 89
    { "South American ï¿½9",    16,    -57,      1,    -41 },     // 90
    { "South Asia",             8,      7,    -10,    -26 },    // 91
    { "Southeast Base",        14,   -499,   -249,    314 },    // 92
    { "Southwest Base",        14,   -104,    167,    -38 },    // 93
    { "Timbalai 1948",          6,   -689,    691,    -46 },    // 94
    { "Tokyo",                  3,   -128,    481,    664 },    // 95
    { "Tristan Astro 1968",    14,   -632,    438,   -609 },    // 96
    { "Viti Levu 1916",         5,     51,    391,    -36 },    // 97
    { "Wake-Eniwetok ï¿½0",     13,    101,     52,    -39 },     // 98
    { "WGS 72",                19,      0,      0,      5 },    // 99
    { "WGS 84",                20,      0,      0,      0 },    // 100
    { "Zanderij",              14,   -265,    120,   -358 },    // 101
    { 0 },
};

struct ELLIPSOID const gEllipsoid[] = {
//  name                               a        1/f
    {  "Airy 1830",                  6377563.396, 299.3249646   }, // 0
    {  "Modified Airy",              6377340.189, 299.3249646   }, // 1
    {  "Australian National",        6378160.0,   298.25        }, // 2
    {  "Bessel 1841",                6377397.155, 299.15281282  }, // 3
    {  "Clarke 1866",                6378206.4,   294.9786982   }, // 4
    {  "Clarke 1880",                6378249.145, 293.465       }, // 5
    {  "Everest (India 1830)",       6377276.345, 300.8017      }, // 6
    {  "Everest (1948)",             6377304.063, 300.8017      }, // 7
    {  "Modified Fischer 1960",      6378155.0,   298.3         }, // 8
    {  "Everest (Pakistan)",         6377309.613, 300.8017      }, // 9
    {  "Indonesian 1974",            6378160.0,   298.247       }, // 10 ?
    {  "GRS 80",                     6378137.0,   298.257222101 }, // 11
    {  "Helmert 1906",               6378200.0,   298.3         }, // 12
    {  "Hough 1960",                 6378270.0,   297.0         }, // 13
    {  "International 1924",         6378388.0,   297.0         }, // 14
    {  "Krassovsky 1940",            6378245.0,   298.3         }, // 15
    {  "South American 1969",        6378160.0,   298.25        }, // 16
    {  "Everest (Malaysia 1969)",    6377295.664, 300.8017      }, // 17
    {  "Everest (Sabah Sarawak)",    6377298.556, 300.8017      }, // 18
    {  "WGS 72",                     6378135.0,   298.26        }, // 19
    {  "WGS 84",                     6378137.0,   298.257223563 }, // 20
    {  "Bessel 1841 (Namibia)",      6377483.865, 299.1528128   }, // 21
    {  "Everest (India 1956)",       6377301.243, 300.8017      }, // 22
    {  "Fischer 1960",               6378166.0,   298.3         }, // 23
    {  "WGS 60",                     6378165.0,   298.3         }, // 24
    {  "WGS 66",                     6378145.0,   298.25        }, // 25
    {  "SGS 85",                     6378136.0,   298.257       }, // 26
    { 0 },
};

/* define constants */
static const double WGSinvf  = 298.257223563;                  /* WGS84 1/f */

void datumParams(short datum, double *a, double *es)
{
    extern struct DATUM const gDatum[];
    extern struct ELLIPSOID const gEllipsoid[];

    double f = 1.0 / gEllipsoid[gDatum[datum].ellipsoid].invf;    // flattening
    *es = 2 * f - f * f;      // eccentricity^2
    *a = gEllipsoid[gDatum[datum].ellipsoid].a;       // semimajor axis
}

int GetDatumIndex(const char *str)
{
      int i = 0;
      while (i < 102)
      {
            if(!strcmp(str, gDatum[i].name))
                  return i;
            i++;
      }

      return -1;
}

/****************************************************************************/
/* Convert degrees to dd mm'ss.s" (DMS-Format)                              */
/****************************************************************************/
void toDMS(double a, char *bufp, int bufplen)
{
    short neg = 0;
    int d, m, s;
    long n;

    if (a < 0.0) {
      a = -a;
      neg = 1;
    }
    d = (int) a;
    n = (long) ((a - (double) d) * 36000.0);

    m = n / 600;
    s = n % 600;

    if (neg)
      d = -d;

    sprintf(bufp, "%d%02d'%02d.%01d\"", d, m, s / 10, s % 10);
    return;
}


/****************************************************************************/
/* Convert dd mm'ss.s" (DMS-Format) to degrees.                             */
/****************************************************************************/

double fromDMS(char *dms)
{
    int d = 0, m = 0;
    double s = 0.0;
    char buf[20];

    buf[0] = '\0';

    sscanf(dms, "%d%[ ]%d%[ ']%lf%[ \"NSWEnswe]", &d, buf, &m, buf, &s, buf);

    s = (double) (abs(d)) + ((double) m + s / 60.0) / 60.0;

    if (d >= 0 && strpbrk(buf, "SWsw") == NULL)
      return s;
    else
      return -s;
}


/****************************************************************************/
/* Convert degrees to dd mm.mmm' (DMM-Format)                               */
/****************************************************************************/

void todmm(int flag, double a, char *bufp, int bufplen)
{
    short neg = 0;
    int d;
    long m;

    if (a < 0.0) {
      a = -a;
      neg = 1;
    }
    d = (int) a;
    m = (long) ((a - (double) d) * 60000.0);

    if (neg)
      d = -d;

    if (!flag)
      snprintf(bufp, bufplen, "%d %02ld.%03ld'", d, m / 1000, m % 1000);
    else {
      if (flag == 1) {
          char c = 'N';

          if (d < 0) {
            d = -d;
            c = 'S';
          }
          snprintf(bufp, bufplen,
                 "%02d %02ld.%03ld %c", d, m / 1000, (m % 1000), c);
      } else if (flag == 2) {
          char c = 'E';

          if (d < 0) {
            d = -d;
            c = 'W';
          }
          snprintf(bufp, bufplen,
                 "%03d %02ld.%03ld %c", d, m / 1000, (m % 1000), c);
      }
    }
    return;
}

void toDMM(double a, char *bufp, int bufplen)
{
    todmm(0, a, bufp, bufplen);
    return;
}
/****************************************************************************/
/* Convert dd mm.mmm' (DMM-Format) to degree.                               */
/****************************************************************************/
double fromDMM(char *dms)
{
    int d = 0;
    double m = 0.0;
    char buf[20];
    char r[20];

    buf[0] = r[0] = '\0';
    sscanf(dms, "%d%[ ]%lf%[ 'NSWEnswe]", &d, buf, &m, buf);

    m = (double) (abs(d)) + m / 60.0;

    if (d >= 0 && strpbrk(buf, "SWsw") == NULL)
      return m;
    else
      return -m;
}




/* --------------------------------------------------------------------------------- */
/****************************************************************************/
/* Convert Lat/Lon <-> Simple Mercator                                      */
/****************************************************************************/
void toSM(double lat, double lon, double lat0, double lon0, double *x, double *y)
{
    double xlon, z, x1, s, y3, s0, y30, y4;

    xlon = lon;

/*  Make sure lon and lon0 are same phase */

    if((lon * lon0 < 0.) && (fabs(lon - lon0) > 180.))
    {
            if(lon < 0.)
                  xlon += 360.;
            else
                  xlon -= 360.;
    }

    z = WGS84_semimajor_axis_meters * mercator_k0;

    x1 = (xlon - lon0) * DEGREE * z;
    *x = x1;

     // y =.5 ln( (1 + sin t) / (1 - sin t) )
    s = sin(lat * DEGREE);
    y3 = (.5 * log((1 + s) / (1 - s))) * z;

    s0 = sin(lat0 * DEGREE);
    y30 = (.5 * log((1 + s0) / (1 - s0))) * z;
    y4 = y3 - y30;

    *y = y4;

}

void
fromSM(double x, double y, double lat0, double lon0, double *lat, double *lon)
{
      double z, s0, y0, lat3, lon1;
      z = WGS84_semimajor_axis_meters * mercator_k0;

// lat = arcsin((e^2(y+y0) - 1)/(e^2(y+y0) + 1))
/*
      double s0 = sin(lat0 * DEGREE);
      double y0 = (.5 * log((1 + s0) / (1 - s0))) * z;

      double e = exp(2 * (y0 + y) / z);
      double e11 = (e - 1)/(e + 1);
      double lat2 =(atan2(e11, sqrt(1 - e11 * e11))) / DEGREE;
*/
//    which is the same as....

      s0 = sin(lat0 * DEGREE);
      y0 = (.5 * log((1 + s0) / (1 - s0))) * z;

      lat3 = (2.0 * atan(exp((y0+y)/z)) - PI/2.) / DEGREE;
      *lat = lat3;


      // lon = x + lon0
      lon1 = lon0 + (x / (DEGREE * z));
      *lon = lon1;

}

void toSM_ECC(double lat, double lon, double lat0, double lon0, double *x, double *y)
{
      double xlon, z, x1, s, y3, s0, y30, y4;

      double falsen;
      double test;
      double ypy;

      double f = 1.0 / 298.;    // flattening .003355
      double e2 = 2 * f - f * f;      // eccentricity^2  .006700
      double e = sqrt(e2);


      xlon = lon;


      /*  Make sure lon and lon0 are same phase */

      if(lon * lon0 < 0.)
      {
            if(lon < 0.)
                  xlon += 360.;
            else
                  xlon -= 360.;
      }

      if(fabs(xlon - lon0) > 180.)
      {
            if(xlon > lon0)
                  xlon -= 360.;
            else
                  xlon += 360.;
      }

      z = WGS84_semimajor_axis_meters * mercator_k0;

      x1 = (xlon - lon0) * DEGREE * z;
      *x = x1;

// y =.5 ln( (1 + sin t) / (1 - sin t) )
      s = sin(lat * DEGREE);
      y3 = (.5 * log((1 + s) / (1 - s))) * z;

      s0 = sin(lat0 * DEGREE);
      y30 = (.5 * log((1 + s0) / (1 - s0))) * z;
      y4 = y3 - y30;
//      *y = y4;

    //testing eccentricity math


      falsen =  z *log(tan(PI/4 + lat0 * DEGREE / 2)*pow((1. - e * s0)/(1. + e * s0), e/2.));
      test =    z *log(tan(PI/4 + lat  * DEGREE / 2)*pow((1. - e * s )/(1. + e * s ), e/2.));
      ypy = test - falsen;

      *y = ypy;
}

void fromSM_ECC(double x, double y, double lat0, double lon0, double *lat, double *lon)
{
      double z, s0, lon1;
      double falsen, t, xi, esf;

      double f = 1.0 / 298.;    // flattening .003355
      double es = 2 * f - f * f;      // eccentricity^2  .006700
      double e = sqrt(es);

      z = WGS84_semimajor_axis_meters * mercator_k0;

      lon1 = lon0 + (x / (DEGREE * z));
      *lon = lon1;

//testing eccentricity math

      s0 = sin(lat0 * DEGREE);

      falsen = z *log(tan(PI/4 + lat0 * DEGREE / 2)*pow((1. - e * s0)/(1. + e * s0), e/2.));
      t = exp((y + falsen) / (z));
      xi = (PI / 2.) - 2.0 * atan(t);

      esf = (es/2. + (5*es*es/24.) + (es*es*es/12.) + (13.0 *es*es*es*es/360.)) * sin( 2 * xi);
      esf += ((7.*es*es/48.) + (29.*es*es*es/240.) + (811.*es*es*es*es/11520.)) * sin (4. * xi);
      esf += ((7.*es*es*es/120.) + (81*es*es*es*es/1120.) + (4279.*es*es*es*es/161280.)) * sin(8. * xi);


     *lat = -(xi + esf) / DEGREE;

}




#if 0
Molodensky
In the listing below, the class GeodeticPosition has three members, lon, lat, and h. They are double-precision values indicating the longitude and latitude in radians, and height in meters above the ellipsoid.

            The source code in the listing below may be copied and reused without restriction, but it is offered AS-IS with NO WARRANTY.

    /*
                        * transform
                        *
    * Parameters:
                        *     from:     The geodetic position to be translated.
                        *     from_a:   The semi-major axis of the "from" ellipsoid.
                        *     from_f:   Flattening of the "from" ellipsoid.
                        *     from_esq: Eccentricity-squared of the "from" ellipsoid.
                        *     da:       Change in semi-major axis length (meters); "to" minus "from"
                        *     df:       Change in flattening; "to" minus "from"
                        *     dx:       Change in x between "from" and "to" datum.
                        *     dy:       Change in y between "from" and "to" datum.
                        *     dz:       Change in z between "from" and "to" datum.
    */
                        public GeodeticPosition transform (GeodeticPosition from,
                                    double from_a, double from_f,

                                    double from_esq, double da, double df,
                                    double dx, double dy, double dz)

#endif
void MolodenskyTransform (double lat, double lon, double *to_lat, double *to_lon, int from_datum_index, int to_datum_index)
{
      double from_lat = lat * DEGREE;
      double from_lon = lon * DEGREE;
      double from_f = 1.0 / gEllipsoid[gDatum[from_datum_index].ellipsoid].invf;    // flattening
      double from_esq = 2 * from_f - from_f * from_f;                               // eccentricity^2
      double from_a = gEllipsoid[gDatum[from_datum_index].ellipsoid].a;             // semimajor axis
      double dx = gDatum[from_datum_index].dx;
      double dy = gDatum[from_datum_index].dy;
      double dz = gDatum[from_datum_index].dz;
      double to_f = 1.0 / gEllipsoid[gDatum[to_datum_index].ellipsoid].invf;        // flattening
      double to_a = gEllipsoid[gDatum[to_datum_index].ellipsoid].a;                 // semimajor axis
      double da = to_a - from_a;
      double df = to_f - from_f;
      double from_h = 0;


      double slat = sin (from_lat);
      double clat = cos (from_lat);
      double slon = sin (from_lon);
      double clon = cos (from_lon);
      double ssqlat = slat * slat;
      double adb = 1.0 / (1.0 - from_f);  // "a divided by b"
      double dlat, dlon, dh;

      double rn = from_a / sqrt (1.0 - from_esq * ssqlat);
      double rm = from_a * (1. - from_esq) / pow ((1.0 - from_esq * ssqlat), 1.5);

      dlat = (((((-dx * slat * clon - dy * slat * slon) + dz * clat)
                  + (da * ((rn * from_esq * slat * clat) / from_a)))
                  + (df * (rm * adb + rn / adb) * slat * clat)))
            / (rm + from_h);

      dlon = (-dx * slon + dy * clon) / ((rn + from_h) * clat);

      dh = (dx * clat * clon) + (dy * clat * slon) + (dz * slat)
                  - (da * (from_a / rn)) + ((df * rn * ssqlat) / adb);

      *to_lon = lon + dlon/DEGREE;
      *to_lat = lat + dlat/DEGREE;
//    *to_h = from.h + dh;

      return;
}




/* --------------------------------------------------------------------------------- */
/*
// Given the lat/long of starting point, and traveling a specified distance,
// at an initial bearing, calculates the lat/long of the resulting location.
//
//  Assumes spherical earth shape.  There are more accurate ellipsoidal
//  algorithms available, c.f. below.
//
//  Algorithm extracted from:
//      Aviation Formulary V1.43
//      By Ed Williams
//      http://williams.best.vwh.net/avform.htm
//
//      From the source:
//      quote:
A point {lat,lon} is a distance d out on the tc radial from point 1 if:

        lat=asin(sin(lat1)*cos(d)+cos(lat1)*sin(d)*cos(tc))
        IF (cos(lat)=0)
        lon=lon1      // endpoint a pole
        ELSE
        lon=mod(lon1-asin(sin(tc)*sin(d)/cos(lat))+pi,2*pi)-pi
        ENDIF

This algorithm is limited to distances such that dlon <pi/2, i.e those that extend around less than one quarter of the circumference of the earth in longitude. A completely general, but more complicated algorithm is necessary if greater distances are allowed:

        lat =asin(sin(lat1)*cos(d)+cos(lat1)*sin(d)*cos(tc))
        dlon=atan2(sin(tc)*sin(d)*cos(lat1),cos(d)-sin(lat1)*sin(lat))
        lon=mod( lon1-dlon +pi,2*pi )-pi
//      enquote

//  Implementers notes:
//         a)    mod(y,x) = y - x*floor(y/x)
//         b)   N.B. author's definition of positive longitude as westerly is
//              corrected to nautical navigation convention by inverting the sense of
//              sin(brg) where found.
//
//         c)   brg is expressed in conventional degrees, dist in Nautical Miles
//              example:
//                  ll_gc_ll(45, 0,, 180., 60, &lat, &lon);         //i.e. 60 miles due south
//                  yields  lat=44, lon=0
*/


void ll_gc_ll(double lat, double lon, double brg, double dist, double *dlat, double *dlon)
{
    //  Express distance as radians, knowing that there are (180*60) nautical miles in PI radians
    double d = dist * PI / (180*60);

    double dlat_rad = asin((sin(lat * PI/180.) * cos(d)) + (cos(lat * PI/180.) * sin(d) * cos(brg * PI/180.)));

    double dlon1_rad = atan2(-sin(brg * PI/180.) * sin(d) * cos(lat * PI/180.) , cos(d) - (sin(lat * PI/180.) * sin(dlat_rad)));
    double dlon2_rad = (lon * PI/180.) - dlon1_rad + PI;
    double dlon_rad = dlon2_rad - (2 * PI * floor(dlon2_rad / (2 * PI))) - PI;
    *dlat = dlat_rad * 180./PI;
    *dlon = dlon_rad * 180./PI;
}





/*
// Given the lat/long of starting point, and traveling a specified distance,
// at an initial bearing, calculates the lat/long of the resulting location.
// This corrects for the ellipsoidal shape of the earth to very high accuracy.
// This is based on a paper by T. Vincenty:
// "Direct and Inverse Solutions of Geodesics on the Ellipsoid with Application
// of Nested Equations", Survey Review XXII, 176, April 1975.
// http://www.ngs.noaa.gov/PUBS_LIB/inverse.pdf
// This implements the "direct formula."
//
// Returns:
//  [lat, long]
resultantLatLong[lat1, lon1, dist, bearing] :=
{
    f = earth_flattening
   // Calculate "reduced" latitude
            U1 = arctan[(1-f) tan[lat1]]
            cU1 = cos[U1]
            sU1 = sin[U1]
            a = earthradius_equatorial
            b = earthradius_polar

            cosalpha1 = cos[bearing]
            sinalpha1 = sin[bearing]

            sigma1 = arctan[tan[U1],cosalpha1]                              // Eq. 1
            sinalpha = cU1 sinalpha1                                        // Eq. 2
                    calpha2 = 1 - sinalpha^2
                    u2 = calpha2 (a^2-b^2)/b^2

                    A = 1 + u2/16384 (4096 + u2 (-768 + u2 (320 - 175 u2)))         // Eq. 3
                    B = u2/1024 (256 + u2 ( -128 + u2 (74 - 47 u2)))                // Eq. 4
                    baseS = dist/(b A)
                    sigma = baseS
                    do
            {
                lastsigma = sigma
                        twoSigmam = 2 sigma1 + sigma                                 // Eq. 5
                        cos2sigmam = cos[twoSigmam]
                        sinSigma = sin[sigma]
                        deltaSigma = B sinSigma ( cos2sigmam +
                        1/4 B (cos[sigma](-1 + 2 cos2sigmam) -
                        1/6 B cos2sigmam (-3 + 4 sinSigma^2)(-3+4 cos2sigmam)))
                                                                   // Eq. 6
                        sigma = baseS + deltaSigma                                   // Eq. 7
            } while (abs[lastsigma - sigma] > 1e-6 arcsec)

                sinSigma = sin[sigma]
                        cosSigma = cos[sigma]
                        twoSigmam = 2 sigma1 + sigma                                 // Eq. 5
                        cos2sigmam = cos[twoSigmam]
                        lat2 = arctan[sU1 cosSigma + cU1 sinSigma cosalpha1,
                (1-f)sqrt[sinalpha^2 + (sU1 sinSigma - cU1 cosSigma cosalpha1)^2]]
                                                                  // Eq. 8
                        lambda = arctan[sinSigma sinalpha1, cU1 cosSigma - sU1 sinSigma cosalpha1]
                                                                  //Eq.9
                        C = f/16 calpha2 (4 + f (4-3 calpha2))                         // Eq. 10
                                L = lambda - (1-C) f sinalpha (sigma +
                                C sinSigma ( cos2sigmam + C cosSigma (-1 + 2 cos2sigmam^2)))

   // TODO:  Calculate final azimuth? (eq. 12)

                                return [lat2, lon1+L]
}

*/

double DistGreatCircle(double slat, double slon, double dlat, double dlon)
{
//    Calculate distance using Great Circle Formula
//  d=2*asin(sqrt((sin((lat1-lat2)/2))^2 +
//                 cos(lat1)*cos(lat2)*(sin((lon1-lon2)/2))^2))
//  distance_nm=((180*60)/pi)*distance_radians

      double lon1 = dlon * PI / 180.;
      double lon2 = slon * PI / 180.;
      double lat1 = dlat * PI / 180.;
      double lat2 = slat * PI / 180.;

      double v = sin((lon1 - lon2)/2.0);
      double w = cos(lat1) * cos(lat2) * v * v;
      double x = sin((lat1 - lat2)/2.0);
      double d4 = 2.0 * asin(sqrt(x*x + w));

      float d5 = (float)((180. * 60. / PI) * d4);
      return d5;
}





void DistanceBearingMercator(double lat0, double lon0, double lat1, double lon1, double *brg, double *dist)
{
      double east, north, brgt, C;
      double lon0x, lon1x, dlat;
      double mlat0;

      //    Calculate bearing by conversion to SM (Mercator) coordinates, then simple trigonometry

      lon0x = lon0;
      lon1x = lon1;

      //    Make lon points the same phase
      if((lon0x * lon1x) < 0.)
      {
            if(lon0x < 0.)
                  lon0x += 360.;
            else
                  lon1x += 360.;

            //    Choose the shortest distance
            if(fabs(lon0x - lon1x) > 180.)
            {
                  if(lon0x > lon1x)
                        lon0x -= 360.;
                  else
                        lon1x -= 360.;
            }

            //    Make always positive
            lon1x += 360.;
            lon0x += 360.;
      }

      //    In the case of exactly east or west courses
      //    we must make an adjustment if we want true Mercator diatances

      //    This idea comes from Thomas(Cagney)
      //    We simply require the dlat to be (slightly) non-zero, and carry on.
      mlat0 = lat0;
      if(fabs(lat1 - lat0) < 1e-4)
            mlat0 += .001;

      toSM_ECC(lat1, lon1x, mlat0, lon0x, &east, &north);

      C = atan2(east, north);
      dlat = (lat1 - mlat0) * 60.;              // in minutes

      //    Classic formula, which fails for due east/west courses....

      if(dist)
      {
            if(cos(C))
                  *dist = (dlat /cos(C));
            else
                  *dist = DistGreatCircle(lat0, lon0, lat1, lon1);

      }

      //    Calculate the bearing using the un-adjusted original latitudes and Mercator Sailing
      if(brg)
      {
            toSM_ECC(lat1, lon1x, lat0, lon0x, &east, &north);

            C = atan2(east, north);
            brgt = 180. + (C * 180. / PI);
            if (brgt < 0)
                  brgt += 360.;
            if (brgt > 360.)
                  brgt -= 360;

            *brg = brgt;
      }


      //    Alternative formulary
      //  From Roy Williams, "Geometry of Navigation", we have p = Dlo (Dlat/DMP) where p is the departure.
      // Then distance is then:D = sqrt(Dlat^2 + p^2)

/*
          double dlo =  (lon1x - lon0x) * 60.;
          double departure = dlo * dlat / ((north/1852.));

          if(dist)
             *dist = sqrt((dlat*dlat) + (departure * departure));
*/



}


/* --------------------------------------------------------------------------------- */
/*
 * lmfit
 *
 * Solves or minimizes the sum of squares of m nonlinear
 * functions of n variables.
 *
 * From public domain Fortran version
 * of Argonne National Laboratories MINPACK
 *     argonne national laboratory. minpack project. march 1980.
 *     burton s. garbow, kenneth e. hillstrom, jorge j. more
 * C translation by Steve Moshier
 * Joachim Wuttke converted the source into C++ compatible ANSI style
 * and provided a simplified interface
 */


#include <stdlib.h>
#include <math.h>
//#include "lmmin.h"            // all moved to georef.h
#define _LMDIF

///=================================================================================
///     Customized section for openCPN georeferencing

double my_fit_function( double tx, double ty, int n_par, double* p )
{

    double ret = p[0] + p[1]*tx;

    if(n_par > 2)
          ret += p[2]*ty;
    if(n_par > 3)
    {
        ret += p[3]*tx*tx;
        ret += p[4]*tx*ty;
        ret += p[5]*ty*ty;
    }
    if(n_par > 6)
    {
        ret += p[6]*tx*tx*tx;
        ret += p[7]*tx*tx*ty;
        ret += p[8]*tx*ty*ty;
        ret += p[9]*ty*ty*ty;
    }

    return ret;
}

int Georef_Calculate_Coefficients_Onedir(int n_points, int n_par, double *tx, double *ty, double *y, double *p,
                                        double hintp0, double hintp1, double hintp2)
        /*
        n_points : number of sample points
        n_par :  3, 6, or 10,  6 is probably good
        tx:  sample data independent variable 1
        ty:  sample data independent variable 2
        y:   sample data dependent result
        p:   curve fit result coefficients
        */
{

    int i;
    lm_control_type control;
    lm_data_type data;

    lm_initialize_control( &control );


    for(i=0 ; i<12 ; i++)
        p[i] = 0.;

//    memset(p, 0, 12 * sizeof(double));

    //  Insert hints
    p[0] = hintp0;
    p[1] = hintp1;
    p[2] = hintp2;

    data.user_func = my_fit_function;
    data.user_tx = tx;
    data.user_ty = ty;
    data.user_y = y;
    data.n_par = n_par;
    data.print_flag = 0;

// perform the fit:

            lm_minimize( n_points, n_par, p, lm_evaluate_default, lm_print_default,
                         &data, &control );

// print results:
//            printf( "status: %s after %d evaluations\n",
//                    lm_infmsg[control.info], control.nfev );

            //      Control.info results [1,2,3] are success, other failure
            return control.info;
}

int Georef_Calculate_Coefficients(struct GeoRef *cp, int nlin_lon)
{
    int  r1, r2, r3, r4;
    int mp;
    int mp_lat, mp_lon;

    double *pnull;
    double *px;

    //      Zero out the points
    cp->pwx[6] = cp->pwy[6] = cp->wpx[6] = cp->wpy[6] = 0.;
    cp->pwx[7] = cp->pwy[7] = cp->wpx[7] = cp->wpy[7] = 0.;
    cp->pwx[8] = cp->pwy[8] = cp->wpx[8] = cp->wpy[8] = 0.;
    cp->pwx[9] = cp->pwy[9] = cp->wpx[9] = cp->wpy[9] = 0.;
    cp->pwx[3] = cp->pwy[3] = cp->wpx[3] = cp->wpy[3] = 0.;
    cp->pwx[4] = cp->pwy[4] = cp->wpx[4] = cp->wpy[4] = 0.;
    cp->pwx[5] = cp->pwy[5] = cp->wpx[5] = cp->wpy[5] = 0.;
    cp->pwx[0] = cp->pwy[0] = cp->wpx[0] = cp->wpy[0] = 0.;
    cp->pwx[1] = cp->pwy[1] = cp->wpx[1] = cp->wpy[1] = 0.;
    cp->pwx[2] = cp->pwy[2] = cp->wpx[2] = cp->wpy[2] = 0.;

    mp = 3;

    switch (cp->order)
    {
    case 1:
        mp = 3;
        break;
    case 2:
        mp = 6;
        break;
    case 3:
        mp = 10;
        break;
    default:
        mp = 3;
        break;
    }

    mp_lat = mp;

    //      Force linear fit for longitude if nlin_lon > 0
    mp_lon = mp;
    if(nlin_lon)
          mp_lon = 2;

    //      Make a dummay double array
    pnull = (double *)calloc(cp->count * sizeof(double), 1);

    //      pixel(tx,ty) to (lat,lon)
    //      Calculate and use a linear equation for p[0..2] to hint the solver

    r1 = Georef_Calculate_Coefficients_Onedir(cp->count, mp_lon, cp->tx, cp->ty, cp->lon, cp->pwx,
                                         cp->lonmin - (cp->txmin * (cp->lonmax - cp->lonmin) /(cp->txmax - cp->txmin)),
                                         (cp->lonmax - cp->lonmin) /(cp->txmax - cp->txmin),
                                         0.);

    //      if blin_lon > 0, force cross terms in latitude equation coefficients to be zero by making lat not dependent on tx,
    if(nlin_lon)
          px = pnull;
    else
          px = cp->tx;

    r2 = Georef_Calculate_Coefficients_Onedir(cp->count, mp_lat, px, cp->ty, cp->lat, cp->pwy,
                                         cp->latmin - (cp->tymin * (cp->latmax - cp->latmin) /(cp->tymax - cp->tymin)),
                                         0.,
                                         (cp->latmax - cp->latmin) /(cp->tymax - cp->tymin));

    //      (lat,lon) to pixel(tx,ty)
    //      Calculate and use a linear equation for p[0..2] to hint the solver

    r3 = Georef_Calculate_Coefficients_Onedir(cp->count, mp_lon, cp->lon, cp->lat, cp->tx, cp->wpx,
                                         cp->txmin - ((cp->txmax - cp->txmin) * cp->lonmin / (cp->lonmax - cp->lonmin)),
                                         (cp->txmax - cp->txmin) / (cp->lonmax - cp->lonmin),
                                         0.0);

    //      if blin_lon > 0, force cross terms in latitude equation coefficients to be zero by making ty not dependent on lon,
    if(nlin_lon)
          px = pnull;
    else
          px = cp->lon;

    r4 = Georef_Calculate_Coefficients_Onedir(cp->count, mp_lat, pnull/*cp->lon*/, cp->lat, cp->ty, cp->wpy,
                                         cp->tymin - ((cp->tymax - cp->tymin) * cp->latmin / (cp->latmax - cp->latmin)),
                                        0.0,
                                        (cp->tymax - cp->tymin) / (cp->latmax - cp->latmin));

    free (pnull);

    if((r1) && (r1 < 4) && (r2) && (r2 < 4) && (r3) && (r3 < 4) && (r4) && (r4 < 4))
        return 0;
    else
        return 1;

}



/*
 * This file contains default implementation of the evaluate and printout
 * routines. In most cases, customization of lmfit can be done by modifying
 * these two routines. Either modify them here, or copy and rename them.
 */

void lm_evaluate_default( double* par, int m_dat, double* fvec,
                          void *data, int *info )
/*
        *    par is an input array. At the end of the minimization, it contains
        *        the approximate solution vector.
        *
        *    m_dat is a positive integer input variable set to the number
        *      of functions.
        *
        *    fvec is an output array of length m_dat which contains the function
        *        values the square sum of which ought to be minimized.
        *
        *    data is a read-only pointer to lm_data_type, as specified by lmuse.h.
        *
        *      info is an integer output variable. If set to a negative value, the
        *        minimization procedure will stop.
 */
{
    int i;
    lm_data_type *mydata;
    mydata = (lm_data_type*)data;

    for (i=0; i<m_dat; i++)
    {
        fvec[i] = mydata->user_y[i] - mydata->user_func( mydata->user_tx[i], mydata->user_ty[i], mydata->n_par, par);
    }
    *info = *info; /* to prevent a 'unused variable' warning */
    /* if <parameters drifted away> { *info = -1; } */
}

void lm_print_default( int n_par, double* par, int m_dat, double* fvec,
                       void *data, int iflag, int iter, int nfev )
/*
        *       data  : for soft control of printout behaviour, add control
        *                 variables to the data struct
        *       iflag : 0 (init) 1 (outer loop) 2(inner loop) -1(terminated)
        *       iter  : outer loop counter
        *       nfev  : number of calls to *evaluate
 */
{
    double f, y, tx, ty;
    int i;
    lm_data_type *mydata;
    mydata = (lm_data_type*)data;

    if(mydata->print_flag)
    {
        if (iflag==2) {
            printf ("trying step in gradient direction\n");
        } else if (iflag==1) {
            printf ("determining gradient (iteration %d)\n", iter);
        } else if (iflag==0) {
            printf ("starting minimization\n");
        } else if (iflag==-1) {
            printf ("terminated after %d evaluations\n", nfev);
        }

        printf( "  par: " );
        for( i=0; i<n_par; ++i )
            printf( " %12g", par[i] );
        printf ( " => norm: %12g\n", lm_enorm( m_dat, fvec ) );

        if ( iflag == -1 ) {
            printf( "  fitting data as follows:\n" );
            for( i=0; i<m_dat; ++i ) {
                tx = (mydata->user_tx)[i];
                ty = (mydata->user_ty)[i];
                y = (mydata->user_y)[i];
                f = mydata->user_func( tx, ty, mydata->n_par, par );
                printf( "    tx[%2d]=%8g     ty[%2d]=%8g     y=%12g fit=%12g     residue=%12g\n",
                        i, tx, i, ty, y, f, y-f );
            }
        }
    }       // if print_flag
}





///=================================================================================

/* *********************** high-level interface **************************** */


void lm_initialize_control( lm_control_type *control )
{
    control->maxcall = 100;
    control->epsilon = 1.e-10; //1.e-14;
    control->stepbound = 100; //100.;
    control->ftol = 1.e-14;
    control->xtol = 1.e-14;
    control->gtol = 1.e-14;
}

void lm_minimize( int m_dat, int n_par, double* par,
                  lm_evaluate_ftype *evaluate, lm_print_ftype *printout,
                  void *data, lm_control_type *control )
{

// *** allocate work space.

    double *fvec, *diag, *fjac, *qtf, *wa1, *wa2, *wa3, *wa4;
    int *ipvt;

    int n = n_par;
    int m = m_dat;

    fvec = (double*) malloc(  m*sizeof(double));
    diag = (double*) malloc(n*  sizeof(double));
    qtf =  (double*) malloc(n*  sizeof(double));
    fjac = (double*) malloc(n*m*sizeof(double));
    wa1 =  (double*) malloc(n*  sizeof(double));
    wa2 =  (double*) malloc(n*  sizeof(double));
    wa3 =  (double*) malloc(n*  sizeof(double));
    wa4 =  (double*) malloc(  m*sizeof(double));
    ipvt = (int*)    malloc(n*  sizeof(int));

    if (!(fvec)    ||
          !(diag ) ||
          !(qtf )  ||
          !(fjac ) ||
          !(wa1 )  ||
          !(wa2 )  ||
          !(wa3 )  ||
          !(wa4 )  ||
          !(ipvt )) {
        control->info = 9;
        return;
          }

// *** perform fit.

          control->info = 0;
          control->nfev = 0;

    // this goes through the modified legacy interface:
          lm_lmdif( m, n, par, fvec, control->ftol, control->xtol, control->gtol,
                    control->maxcall*(n+1), control->epsilon, diag, 1,
                    control->stepbound, &(control->info),
                    &(control->nfev), fjac, ipvt, qtf, wa1, wa2, wa3, wa4,
                    evaluate, printout, data );

          (*printout)( n, par, m, fvec, data, -1, 0, control->nfev );
          control->fnorm = lm_enorm(m, fvec);
          if (control->info < 0 ) control->info = 10;

// *** clean up.

          free(fvec);
          free(diag);
          free(qtf);
          free(fjac);
          free(wa1);
          free(wa2);
          free(wa3 );
          free(wa4);
          free(ipvt);
}


// ***** the following messages are referenced by the variable info.

char *lm_infmsg[] = {
    "improper input parameters",
    "the relative error in the sum of squares is at most tol",
    "the relative error between x and the solution is at most tol",
    "both errors are at most tol",
    "fvec is orthogonal to the columns of the jacobian to machine precision",
    "number of calls to fcn has reached or exceeded 200*(n+1)",
    "ftol is too small. no further reduction in the sum of squares is possible",
    "xtol too small. no further improvement in approximate solution x possible",
    "gtol too small. no further improvement in approximate solution x possible",
    "not enough memory",
    "break requested within function evaluation"
};

char *lm_shortmsg[] = {
    "invalid input",
    "success (f)",
    "success (p)",
    "success (f,p)",
    "degenerate",
    "call limit",
    "failed (f)",
    "failed (p)",
    "failed (o)",
    "no memory",
    "user break"
};


/* ************************** implementation ******************************* */


#define BUG 0
#if BUG
#include <stdio.h>
#endif

// the following values seem good for an x86:
//#define LM_MACHEP .555e-16 /* resolution of arithmetic */
//#define LM_DWARF  9.9e-324 /* smallest nonzero number */
// the follwoing values should work on any machine:
 #define LM_MACHEP 1.2e-16
 #define LM_DWARF 1.0e-38

// the squares of the following constants shall not under/overflow:
// these values seem good for an x86:
//#define LM_SQRT_DWARF 1.e-160
//#define LM_SQRT_GIANT 1.e150
// the following values should work on any machine:
 #define LM_SQRT_DWARF 3.834e-20
 #define LM_SQRT_GIANT 1.304e19


void lm_qrfac( int m, int n, double* a, int pivot, int* ipvt,
               double* rdiag, double* acnorm, double* wa);
void lm_qrsolv(int n, double* r, int ldr, int* ipvt, double* diag,
               double* qtb, double* x, double* sdiag, double* wa);
void lm_lmpar( int n, double* r, int ldr, int* ipvt, double* diag, double* qtb,
               double delta, double* par, double* x, double* sdiag,
               double* wa1, double* wa2);

#define MIN(a,b) (((a)<=(b)) ? (a) : (b))
#define MAX(a,b) (((a)>=(b)) ? (a) : (b))
#define SQR(x)   (x)*(x)


// ***** the low-level legacy interface for full control.

void lm_lmdif( int m, int n, double* x, double* fvec, double ftol, double xtol,
               double gtol, int maxfev, double epsfcn, double* diag, int mode,
               double factor, int *info, int *nfev,
               double* fjac, int* ipvt, double* qtf,
               double* wa1, double* wa2, double* wa3, double* wa4,
               lm_evaluate_ftype *evaluate, lm_print_ftype *printout,
               void *data )
{
/*
    *   the purpose of lmdif is to minimize the sum of the squares of
    *   m nonlinear functions in n variables by a modification of
    *   the levenberg-marquardt algorithm. the user must provide a
    *   subroutine evaluate which calculates the functions. the jacobian
    *   is then calculated by a forward-difference approximation.
    *
    *   the multi-parameter interface lm_lmdif is for users who want
    *   full control and flexibility. most users will be better off using
    *   the simpler interface lmfit provided above.
    *
    *   the parameters are the same as in the legacy FORTRAN implementation,
 *   with the following exceptions:
    *      the old parameter ldfjac which gave leading dimension of fjac has
    *        been deleted because this C translation makes no use of two-
    *        dimensional arrays;
    *      the old parameter nprint has been deleted; printout is now controlled
    *        by the user-supplied routine *printout;
    *      the parameter field *data and the function parameters *evaluate and
    *        *printout have been added; they help avoiding global variables.
    *
 *   parameters:
    *
    *    m is a positive integer input variable set to the number
    *      of functions.
    *
    *    n is a positive integer input variable set to the number
    *      of variables. n must not exceed m.
    *
    *    x is an array of length n. on input x must contain
    *      an initial estimate of the solution vector. on output x
    *      contains the final estimate of the solution vector.
    *
    *    fvec is an output array of length m which contains
    *      the functions evaluated at the output x.
    *
    *    ftol is a nonnegative input variable. termination
    *      occurs when both the actual and predicted relative
    *      reductions in the sum of squares are at most ftol.
    *      therefore, ftol measures the relative error desired
    *      in the sum of squares.
    *
    *    xtol is a nonnegative input variable. termination
    *      occurs when the relative error between two consecutive
    *      iterates is at most xtol. therefore, xtol measures the
    *      relative error desired in the approximate solution.
    *
    *    gtol is a nonnegative input variable. termination
    *      occurs when the cosine of the angle between fvec and
    *      any column of the jacobian is at most gtol in absolute
    *      value. therefore, gtol measures the orthogonality
    *      desired between the function vector and the columns
    *      of the jacobian.
    *
    *    maxfev is a positive integer input variable. termination
    *      occurs when the number of calls to lm_fcn is at least
    *      maxfev by the end of an iteration.
    *
    *    epsfcn is an input variable used in determining a suitable
    *      step length for the forward-difference approximation. this
    *      approximation assumes that the relative errors in the
    *      functions are of the order of epsfcn. if epsfcn is less
    *      than the machine precision, it is assumed that the relative
    *      errors in the functions are of the order of the machine
    *      precision.
    *
    *    diag is an array of length n. if mode = 1 (see below), diag is
    *        internally set. if mode = 2, diag must contain positive entries
    *        that serve as multiplicative scale factors for the variables.
    *
    *    mode is an integer input variable. if mode = 1, the
    *      variables will be scaled internally. if mode = 2,
    *      the scaling is specified by the input diag. other
    *      values of mode are equivalent to mode = 1.
    *
    *    factor is a positive input variable used in determining the
    *      initial step bound. this bound is set to the product of
    *      factor and the euclidean norm of diag*x if nonzero, or else
    *      to factor itself. in most cases factor should lie in the
    *      interval (.1,100.). 100. is a generally recommended value.
    *
    *    info is an integer output variable that indicates the termination
 *        status of lm_lmdif as follows:
    *
    *        info < 0  termination requested by user-supplied routine *evaluate;
    *
    *      info = 0  improper input parameters;
    *
    *      info = 1  both actual and predicted relative reductions
    *              in the sum of squares are at most ftol;
    *
    *      info = 2  relative error between two consecutive iterates
    *              is at most xtol;
    *
    *      info = 3  conditions for info = 1 and info = 2 both hold;
    *
    *      info = 4  the cosine of the angle between fvec and any
    *              column of the jacobian is at most gtol in
    *              absolute value;
    *
    *      info = 5  number of calls to lm_fcn has reached or
    *              exceeded maxfev;
    *
    *      info = 6  ftol is too small. no further reduction in
    *              the sum of squares is possible;
    *
    *      info = 7  xtol is too small. no further improvement in
    *              the approximate solution x is possible;
    *
    *      info = 8  gtol is too small. fvec is orthogonal to the
    *              columns of the jacobian to machine precision;
    *
    *    nfev is an output variable set to the number of calls to the
    *        user-supplied routine *evaluate.
    *
    *    fjac is an output m by n array. the upper n by n submatrix
    *      of fjac contains an upper triangular matrix r with
    *      diagonal elements of nonincreasing magnitude such that
    *
    *           t     t       t
    *          p *(jac *jac)*p = r *r,
    *
    *      where p is a permutation matrix and jac is the final
    *      calculated jacobian. column j of p is column ipvt(j)
    *      (see below) of the identity matrix. the lower trapezoidal
    *      part of fjac contains information generated during
    *      the computation of r.
    *
    *    ipvt is an integer output array of length n. ipvt
    *      defines a permutation matrix p such that jac*p = q*r,
    *      where jac is the final calculated jacobian, q is
    *      orthogonal (not stored), and r is upper triangular
    *      with diagonal elements of nonincreasing magnitude.
    *      column j of p is column ipvt(j) of the identity matrix.
    *
    *    qtf is an output array of length n which contains
    *      the first n elements of the vector (q transpose)*fvec.
    *
    *    wa1, wa2, and wa3 are work arrays of length n.
    *
    *    wa4 is a work array of length m.
    *
 *   the following parameters are newly introduced in this C translation:
    *
    *      evaluate is the name of the subroutine which calculates the functions.
    *        a default implementation lm_evaluate_default is provided in lm_eval.c;
    *        alternatively, evaluate can be provided by a user calling program.
 *        it should be written as follows:
    *
    *        void evaluate ( double* par, int m_dat, double* fvec,
    *                       void *data, int *info )
    *        {
    *           // for ( i=0; i<m_dat; ++i )
    *           //     calculate fvec[i] for given parameters par;
    *           // to stop the minimization,
    *           //     set *info to a negative integer.
    *        }
    *
    *      printout is the name of the subroutine which nforms about fit progress.
    *        a default implementation lm_print_default is provided in lm_eval.c;
    *        alternatively, printout can be provided by a user calling program.
 *        it should be written as follows:
    *
    *        void printout ( int n_par, double* par, int m_dat, double* fvec,
    *                       void *data, int iflag, int iter, int nfev )
    *        {
    *           // iflag : 0 (init) 1 (outer loop) 2(inner loop) -1(terminated)
    *           // iter  : outer loop counter
    *           // nfev  : number of calls to *evaluate
    *        }
    *
    *      data is an input pointer to an arbitrary structure that is passed to
    *        evaluate. typically, it contains experimental data to be fitted.
    *
 */
    int i, iter, j;
    double actred, delta, dirder, eps, fnorm, fnorm1, gnorm, par, pnorm,
    prered, ratio, step, sum, temp, temp1, temp2, temp3, xnorm;
    static double p1 = 0.1;
    static double p5 = 0.5;
    static double p25 = 0.25;
    static double p75 = 0.75;
    static double p0001 = 1.0e-4;

    *nfev = 0; // function evaluation counter
    iter = 1;  // outer loop counter
    par = 0;   // levenberg-marquardt parameter
    delta = 0; // just to prevent a warning (initialization within if-clause)
    xnorm = 0; // dito

    temp = MAX(epsfcn,LM_MACHEP);
    eps = sqrt(temp); // used in calculating the Jacobian by forward differences

// *** check the input parameters for errors.

    if ( (n <= 0) || (m < n) || (ftol < 0.)
          || (xtol < 0.) || (gtol < 0.) || (maxfev <= 0) || (factor <= 0.) )
    {
        *info = 0; // invalid parameter
        return;
    }
    if ( mode == 2 )  /* scaling by diag[] */
    {
        for ( j=0; j<n; j++ )  /* check for nonpositive elements */
        {
            if ( diag[j] <= 0.0 )
            {
                *info = 0; // invalid parameter
                return;
            }
        }
    }
#if BUG
    printf( "lmdif\n" );
#endif

// *** evaluate the function at the starting point and calculate its norm.

    *info = 0;
    (*evaluate)( x, m, fvec, data, info );
    (*printout)( n, x, m, fvec, data, 0, 0, ++(*nfev) );
    if ( *info < 0 ) return;
    fnorm = lm_enorm(m,fvec);

// *** the outer loop.

    do {
#if BUG
        printf( "lmdif/ outer loop iter=%d nfev=%d fnorm=%.10e\n",
                iter, *nfev, fnorm );
#endif

// O** calculate the jacobian matrix.

        for ( j=0; j<n; j++ )
{
    temp = x[j];
    step = eps * fabs(temp);
    if (step == 0.) step = eps;
    x[j] = temp + step;
    *info = 0;
    (*evaluate)( x, m, wa4, data, info );
    (*printout)( n, x, m, wa4, data, 1, iter, ++(*nfev) );
    if ( *info < 0 ) return;  // user requested break
    x[j] = temp;
    for ( i=0; i<m; i++ )
        fjac[j*m+i] = (wa4[i] - fvec[i]) / step;
}
#if BUG>1
        // DEBUG: print the entire matrix
        for ( i=0; i<m; i++ )
{
    for ( j=0; j<n; j++ )
        printf( "%.5e ", y[j*m+i] );
    printf( "\n" );
}
#endif

// O** compute the qr factorization of the jacobian.

        lm_qrfac( m, n, fjac, 1, ipvt, wa1, wa2, wa3);

// O** on the first iteration ...

        if (iter == 1)
{
    if (mode != 2)
//      ... scale according to the norms of the columns of the initial jacobian.
    {
        for ( j=0; j<n; j++ )
        {
            diag[j] = wa2[j];
            if ( wa2[j] == 0. )
                diag[j] = 1.;
        }
    }

//      ... calculate the norm of the scaled x and
//          initialize the step bound delta.

    for ( j=0; j<n; j++ )
        wa3[j] = diag[j] * x[j];

    xnorm = lm_enorm( n, wa3 );
    delta = factor*xnorm;
    if (delta == 0.)
        delta = factor;
}

// O** form (q transpose)*fvec and store the first n components in qtf.

        for ( i=0; i<m; i++ )
            wa4[i] = fvec[i];

        for ( j=0; j<n; j++ )
{
    temp3 = fjac[j*m+j];
    if (temp3 != 0.)
    {
        sum = 0;
        for ( i=j; i<m; i++ )
            sum += fjac[j*m+i] * wa4[i];
        temp = -sum / temp3;
        for ( i=j; i<m; i++ )
            wa4[i] += fjac[j*m+i] * temp;
    }
    fjac[j*m+j] = wa1[j];
    qtf[j] = wa4[j];
}

// O** compute the norm of the scaled gradient and test for convergence.

        gnorm = 0;
        if ( fnorm != 0 )
{
    for ( j=0; j<n; j++ )
    {
        if ( wa2[ ipvt[j] ] == 0 ) continue;

        sum = 0.;
        for ( i=0; i<=j; i++ )
            sum += fjac[j*m+i] * qtf[i] / fnorm;
        gnorm = MAX( gnorm, fabs(sum/wa2[ ipvt[j] ]) );
    }
}

        if ( gnorm <= gtol )
{
    *info = 4;
    return;
}

// O** rescale if necessary.

        if ( mode != 2 )
{
    for ( j=0; j<n; j++ )
        diag[j] = MAX(diag[j],wa2[j]);
}

// O** the inner loop.

        do {
#if BUG
            printf( "lmdif/ inner loop iter=%d nfev=%d\n", iter, *nfev );
#endif

// OI* determine the levenberg-marquardt parameter.

            lm_lmpar( n,fjac,m,ipvt,diag,qtf,delta,&par,wa1,wa2,wa3,wa4 );

// OI* store the direction p and x + p. calculate the norm of p.

            for ( j=0; j<n; j++ )
            {
                wa1[j] = -wa1[j];
                wa2[j] = x[j] + wa1[j];
                wa3[j] = diag[j]*wa1[j];
            }
            pnorm = lm_enorm(n,wa3);

// OI* on the first iteration, adjust the initial step bound.

            if ( *nfev<= 1+n ) // bug corrected by J. Wuttke in 2004
                delta = MIN(delta,pnorm);

// OI* evaluate the function at x + p and calculate its norm.

            *info = 0;
            (*evaluate)( wa2, m, wa4, data, info );
            (*printout)( n, x, m, wa4, data, 2, iter, ++(*nfev) );
            if ( *info < 0 ) return;  // user requested break

            fnorm1 = lm_enorm(m,wa4);
#if BUG
            printf( "lmdif/ pnorm %.10e  fnorm1 %.10e  fnorm %.10e"
                    " delta=%.10e par=%.10e\n",
                    pnorm, fnorm1, fnorm, delta, par );
#endif

// OI* compute the scaled actual reduction.

            if ( p1*fnorm1 < fnorm )
                actred = 1 - SQR( fnorm1/fnorm );
            else
                actred = -1;

// OI* compute the scaled predicted reduction and
//     the scaled directional derivative.

            for ( j=0; j<n; j++ )
            {
                wa3[j] = 0;
                for ( i=0; i<=j; i++ )
                    wa3[i] += fjac[j*m+i]*wa1[ ipvt[j] ];
            }
            temp1 = lm_enorm(n,wa3) / fnorm;
            temp2 = sqrt(par) * pnorm / fnorm;
            prered = SQR(temp1) + 2 * SQR(temp2);
            dirder = - ( SQR(temp1) + SQR(temp2) );

// OI* compute the ratio of the actual to the predicted reduction.

            ratio = prered!=0 ? actred/prered : 0;
#if BUG
            printf( "lmdif/ actred=%.10e prered=%.10e ratio=%.10e"
                    " sq(1)=%.10e sq(2)=%.10e dd=%.10e\n",
                    actred, prered, prered!=0 ? ratio : 0.,
                    SQR(temp1), SQR(temp2), dirder );
#endif

// OI* update the step bound.

            if (ratio <= p25)
{
    if (actred >= 0.)
        temp = p5;
    else
        temp = p5*dirder/(dirder + p5*actred);
    if ( p1*fnorm1 >= fnorm || temp < p1 )
        temp = p1;
    delta = temp * MIN(delta,pnorm/p1);
    par /= temp;
}
            else if ( par == 0. || ratio >= p75 )
{
    delta = pnorm/p5;
    par *= p5;
}

// OI* test for successful iteration...

            if (ratio >= p0001)
{

//     ... successful iteration. update x, fvec, and their norms.

    for ( j=0; j<n; j++ )
    {
        x[j] = wa2[j];
        wa2[j] = diag[j]*x[j];
    }
    for ( i=0; i<m; i++ )
        fvec[i] = wa4[i];
    xnorm = lm_enorm(n,wa2);
    fnorm = fnorm1;
    iter++;
}
#if BUG
            else {
    printf( "ATTN: iteration considered unsuccessful\n" );
            }
#endif

// OI* tests for convergence ( otherwise *info = 1, 2, or 3 )

            *info = 0; // do not terminate (unless overwritten by nonzero value)
            if ( fabs(actred) <= ftol && prered <= ftol && p5*ratio <= 1 )
                *info = 1;
            if (delta <= xtol*xnorm)
                *info += 2;
            if ( *info != 0)
                return;

// OI* tests for termination and stringent tolerances.

            if ( *nfev >= maxfev)
                *info = 5;
            if ( fabs(actred) <= LM_MACHEP &&
                 prered <= LM_MACHEP && p5*ratio <= 1 )
                *info = 6;
            if (delta <= LM_MACHEP*xnorm)
                *info = 7;
            if (gnorm <= LM_MACHEP)
                *info = 8;
            if ( *info != 0)
                return;

// OI* end of the inner loop. repeat if iteration unsuccessful.

        } while (ratio < p0001);

// O** end of the outer loop.

    } while (1);

}



void lm_lmpar(int n, double* r, int ldr, int* ipvt, double* diag, double* qtb,
              double delta, double* par, double* x, double* sdiag,
              double* wa1, double* wa2)
{
/*     given an m by n matrix a, an n by n nonsingular diagonal
    *     matrix d, an m-vector b, and a positive number delta,
    *     the problem is to determine a value for the parameter
    *     par such that if x solves the system
    *
    *        a*x = b ,       sqrt(par)*d*x = 0 ,
    *
    *     in the least squares sense, and dxnorm is the euclidean
    *     norm of d*x, then either par is 0. and
    *
    *        (dxnorm-delta) .le. 0.1*delta ,
    *
    *     or par is positive and
    *
    *        abs(dxnorm-delta) .le. 0.1*delta .
    *
    *     this subroutine completes the solution of the problem
    *     if it is provided with the necessary information from the
    *     qr factorization, with column pivoting, of a. that is, if
    *     a*p = q*r, where p is a permutation matrix, q has orthogonal
    *     columns, and r is an upper triangular matrix with diagonal
    *     elements of nonincreasing magnitude, then lmpar expects
    *     the full upper triangle of r, the permutation matrix p,
    *     and the first n components of (q transpose)*b. on output
    *     lmpar also provides an upper triangular matrix s such that
    *
    *         t       t               t
    *        p *(a *a + par*d*d)*p = s *s .
    *
    *     s is employed within lmpar and may be of separate interest.
    *
    *     only a few iterations are generally needed for convergence
    *     of the algorithm. if, however, the limit of 10 iterations
    *     is reached, then the output par will contain the best
    *     value obtained so far.
    *
 *     parameters:
    *
    *    n is a positive integer input variable set to the order of r.
    *
    *    r is an n by n array. on input the full upper triangle
    *      must contain the full upper triangle of the matrix r.
    *      on output the full upper triangle is unaltered, and the
    *      strict lower triangle contains the strict upper triangle
    *      (transposed) of the upper triangular matrix s.
    *
    *    ldr is a positive integer input variable not less than n
    *      which specifies the leading dimension of the array r.
    *
    *    ipvt is an integer input array of length n which defines the
    *      permutation matrix p such that a*p = q*r. column j of p
    *      is column ipvt(j) of the identity matrix.
    *
    *    diag is an input array of length n which must contain the
    *      diagonal elements of the matrix d.
    *
    *    qtb is an input array of length n which must contain the first
    *      n elements of the vector (q transpose)*b.
    *
    *    delta is a positive input variable which specifies an upper
    *      bound on the euclidean norm of d*x.
    *
    *    par is a nonnegative variable. on input par contains an
    *      initial estimate of the levenberg-marquardt parameter.
    *      on output par contains the final estimate.
    *
    *    x is an output array of length n which contains the least
    *      squares solution of the system a*x = b, sqrt(par)*d*x = 0,
    *      for the output par.
    *
    *    sdiag is an output array of length n which contains the
    *      diagonal elements of the upper triangular matrix s.
    *
    *    wa1 and wa2 are work arrays of length n.
    *
 */
    int i, iter, j, nsing;
    double dxnorm, fp, fp_old, gnorm, parc, parl, paru;
    double sum, temp;
    static double p1 = 0.1;
    static double p001 = 0.001;

#if BUG
    printf( "lmpar\n" );
#endif

// *** compute and store in x the gauss-newton direction. if the
//     jacobian is rank-deficient, obtain a least squares solution.

    nsing = n;
    for ( j=0; j<n; j++ )
    {
        wa1[j] = qtb[j];
        if ( r[j*ldr+j] == 0 && nsing == n )
            nsing = j;
        if (nsing < n)
            wa1[j] = 0;
    }
#if BUG
    printf( "nsing %d ", nsing );
#endif
    for ( j=nsing-1; j>=0; j-- )
{
    wa1[j] = wa1[j]/r[j+ldr*j];
    temp = wa1[j];
    for ( i=0; i<j; i++ )
        wa1[i] -= r[j*ldr+i]*temp;
}

    for ( j=0; j<n; j++ )
        x[ ipvt[j] ] = wa1[j];

// *** initialize the iteration counter.
//     evaluate the function at the origin, and test
//     for acceptance of the gauss-newton direction.

    iter = 0;
    for ( j=0; j<n; j++ )
        wa2[j] = diag[j]*x[j];
    dxnorm = lm_enorm(n,wa2);
    fp = dxnorm - delta;
    if (fp <= p1*delta)
{
#if BUG
      printf( "lmpar/ terminate (fp<delta/10\n" );
#endif
        *par = 0;
        return;
}

// *** if the jacobian is not rank deficient, the newton
//     step provides a lower bound, parl, for the 0. of
//     the function. otherwise set this bound to 0..

    parl = 0;
    if (nsing >= n)
{
    for ( j=0; j<n; j++ )
        wa1[j] = diag[ ipvt[j] ] * wa2[ ipvt[j] ] / dxnorm;

    for ( j=0; j<n; j++ )
    {
        sum = 0.;
        for ( i=0; i<j; i++ )
            sum += r[j*ldr+i]*wa1[i];
        wa1[j] = (wa1[j] - sum)/r[j+ldr*j];
    }
    temp = lm_enorm(n,wa1);
    parl = fp/delta/temp/temp;
}

// *** calculate an upper bound, paru, for the 0. of the function.

    for ( j=0; j<n; j++ )
{
    sum = 0;
    for ( i=0; i<=j; i++ )
        sum += r[j*ldr+i]*qtb[i];
    wa1[j] = sum/diag[ ipvt[j] ];
}
    gnorm = lm_enorm(n,wa1);
    paru = gnorm/delta;
    if (paru == 0.)
        paru = LM_DWARF/MIN(delta,p1);

// *** if the input par lies outside of the interval (parl,paru),
//     set par to the closer endpoint.

    *par = MAX( *par,parl);
    *par = MIN( *par,paru);
    if ( *par == 0.)
        *par = gnorm/dxnorm;
#if BUG
    printf( "lmpar/ parl %.4e  par %.4e  paru %.4e\n", parl, *par, paru );
#endif

// *** iterate.

    for ( ; ; iter++ ) {

// *** evaluate the function at the current value of par.

    if ( *par == 0.)
        *par = MAX(LM_DWARF,p001*paru);
    temp = sqrt( *par );
    for ( j=0; j<n; j++ )
        wa1[j] = temp*diag[j];
    lm_qrsolv( n, r, ldr, ipvt, wa1, qtb, x, sdiag, wa2);
    for ( j=0; j<n; j++ )
        wa2[j] = diag[j]*x[j];
    dxnorm = lm_enorm(n,wa2);
    fp_old = fp;
    fp = dxnorm - delta;

// ***       if the function is small enough, accept the current value
//     of par. also test for the exceptional cases where parl
//     is 0. or the number of iterations has reached 10.

    if ( fabs(fp) <= p1*delta
         || (parl == 0. && fp <= fp_old && fp_old < 0.)
         || iter == 10 )
        break; // the only exit from this loop

// *** compute the Newton correction.

    for ( j=0; j<n; j++ )
        wa1[j] = diag[ ipvt[j] ] * wa2[ ipvt[j] ] / dxnorm;

    for ( j=0; j<n; j++ )
    {
        wa1[j] = wa1[j]/sdiag[j];
        for ( i=j+1; i<n; i++ )
            wa1[i] -= r[j*ldr+i]*wa1[j];
    }
    temp = lm_enorm( n, wa1);
    parc = fp/delta/temp/temp;

// *** depending on the sign of the function, update parl or paru.

    if (fp > 0)
        parl = MAX(parl, *par);
    else if (fp < 0)
        paru = MIN(paru, *par);
        // the case fp==0 is precluded by the break condition

// *** compute an improved estimate for par.

    *par = MAX(parl, *par + parc);

    }

}



void lm_qrfac(int m, int n, double* a, int pivot, int* ipvt,
              double* rdiag, double* acnorm, double* wa)
{
/*
    *     this subroutine uses householder transformations with column
    *     pivoting (optional) to compute a qr factorization of the
    *     m by n matrix a. that is, qrfac determines an orthogonal
    *     matrix q, a permutation matrix p, and an upper trapezoidal
    *     matrix r with diagonal elements of nonincreasing magnitude,
    *     such that a*p = q*r. the householder transformation for
    *     column k, k = 1,2,...,min(m,n), is of the form
    *
    *                    t
    *        i - (1/u(k))*u*u
    *
    *     where u has 0.s in the first k-1 positions. the form of
    *     this transformation and the method of pivoting first
    *     appeared in the corresponding linpack subroutine.
    *
 *     parameters:
    *
    *    m is a positive integer input variable set to the number
    *      of rows of a.
    *
    *    n is a positive integer input variable set to the number
    *      of columns of a.
    *
    *    a is an m by n array. on input a contains the matrix for
    *      which the qr factorization is to be computed. on output
    *      the strict upper trapezoidal part of a contains the strict
    *      upper trapezoidal part of r, and the lower trapezoidal
    *      part of a contains a factored form of q (the non-trivial
    *      elements of the u vectors described above).
    *
    *    pivot is a logical input variable. if pivot is set true,
    *      then column pivoting is enforced. if pivot is set false,
    *      then no column pivoting is done.
    *
    *    ipvt is an integer output array of length lipvt. ipvt
    *      defines the permutation matrix p such that a*p = q*r.
    *      column j of p is column ipvt(j) of the identity matrix.
    *      if pivot is false, ipvt is not referenced.
    *
    *    rdiag is an output array of length n which contains the
    *      diagonal elements of r.
    *
    *    acnorm is an output array of length n which contains the
    *      norms of the corresponding columns of the input matrix a.
    *      if this information is not needed, then acnorm can coincide
    *      with rdiag.
    *
    *    wa is a work array of length n. if pivot is false, then wa
    *      can coincide with rdiag.
    *
 */
    int i, j, k, kmax, minmn;
    double ajnorm, sum, temp;
    static double p05 = 0.05;

// *** compute the initial column norms and initialize several arrays.

    for ( j=0; j<n; j++ )
    {
        acnorm[j] = lm_enorm(m, &a[j*m]);
        rdiag[j] = acnorm[j];
        wa[j] = rdiag[j];
        if ( pivot )
            ipvt[j] = j;
    }
#if BUG
    printf( "qrfac\n" );
#endif

// *** reduce a to r with householder transformations.

    minmn = MIN(m,n);
    for ( j=0; j<minmn; j++ )
    {
        if ( !pivot ) goto pivot_ok;

// *** bring the column of largest norm into the pivot position.

        kmax = j;
        for ( k=j+1; k<n; k++ )
            if (rdiag[k] > rdiag[kmax])
                kmax = k;
        if (kmax == j) goto pivot_ok; // bug fixed in rel 2.1

        for ( i=0; i<m; i++ )
        {
            temp        = a[j*m+i];
            a[j*m+i]    = a[kmax*m+i];
            a[kmax*m+i] = temp;
        }
        rdiag[kmax] = rdiag[j];
        wa[kmax] = wa[j];
        k = ipvt[j];
        ipvt[j] = ipvt[kmax];
        ipvt[kmax] = k;

    pivot_ok:

// *** compute the Householder transformation to reduce the
//     j-th column of a to a multiple of the j-th unit vector.

            ajnorm = lm_enorm( m-j, &a[j*m+j] );
    if (ajnorm == 0.)
    {
        rdiag[j] = 0;
        continue;
    }

    if (a[j*m+j] < 0.)
        ajnorm = -ajnorm;
    for ( i=j; i<m; i++ )
        a[j*m+i] /= ajnorm;
    a[j*m+j] += 1;

// *** apply the transformation to the remaining columns
//     and update the norms.

    for ( k=j+1; k<n; k++ )
    {
        sum = 0;

        for ( i=j; i<m; i++ )
            sum += a[j*m+i]*a[k*m+i];

        temp = sum/a[j+m*j];

        for ( i=j; i<m; i++ )
            a[k*m+i] -= temp * a[j*m+i];

        if ( pivot && rdiag[k] != 0. )
        {
            temp = a[m*k+j]/rdiag[k];
            temp = MAX( 0., 1-temp*temp );
            rdiag[k] *= sqrt(temp);
            temp = rdiag[k]/wa[k];
            if ( p05*SQR(temp) <= LM_MACHEP )
            {
                rdiag[k] = lm_enorm( m-j-1, &a[m*k+j+1]);
                wa[k] = rdiag[k];
            }
        }
    }

    rdiag[j] = -ajnorm;
    }
}



void lm_qrsolv(int n, double* r, int ldr, int* ipvt, double* diag,
               double* qtb, double* x, double* sdiag, double* wa)
{
/*
    *     given an m by n matrix a, an n by n diagonal matrix d,
    *     and an m-vector b, the problem is to determine an x which
    *     solves the system
    *
    *        a*x = b ,       d*x = 0 ,
    *
    *     in the least squares sense.
    *
    *     this subroutine completes the solution of the problem
    *     if it is provided with the necessary information from the
    *     qr factorization, with column pivoting, of a. that is, if
    *     a*p = q*r, where p is a permutation matrix, q has orthogonal
    *     columns, and r is an upper triangular matrix with diagonal
    *     elements of nonincreasing magnitude, then qrsolv expects
    *     the full upper triangle of r, the permutation matrix p,
    *     and the first n components of (q transpose)*b. the system
    *     a*x = b, d*x = 0, is then equivalent to
    *
    *             t     t
    *        r*z = q *b ,  p *d*p*z = 0 ,
    *
    *     where x = p*z. if this system does not have full rank,
    *     then a least squares solution is obtained. on output qrsolv
    *     also provides an upper triangular matrix s such that
    *
    *         t       t           t
    *        p *(a *a + d*d)*p = s *s .
    *
    *     s is computed within qrsolv and may be of separate interest.
    *
    *     parameters
    *
    *    n is a positive integer input variable set to the order of r.
    *
    *    r is an n by n array. on input the full upper triangle
    *      must contain the full upper triangle of the matrix r.
    *      on output the full upper triangle is unaltered, and the
    *      strict lower triangle contains the strict upper triangle
    *      (transposed) of the upper triangular matrix s.
    *
    *    ldr is a positive integer input variable not less than n
    *      which specifies the leading dimension of the array r.
    *
    *    ipvt is an integer input array of length n which defines the
    *      permutation matrix p such that a*p = q*r. column j of p
    *      is column ipvt(j) of the identity matrix.
    *
    *    diag is an input array of length n which must contain the
    *      diagonal elements of the matrix d.
    *
    *    qtb is an input array of length n which must contain the first
    *      n elements of the vector (q transpose)*b.
    *
    *    x is an output array of length n which contains the least
    *      squares solution of the system a*x = b, d*x = 0.
    *
    *    sdiag is an output array of length n which contains the
    *      diagonal elements of the upper triangular matrix s.
    *
    *    wa is a work array of length n.
    *
 */
    int i, kk, j, k, nsing;
    double qtbpj, sum, temp;
    double sin, cos, tan, cotan; // these are local variables, not functions
    static double p25 = 0.25;
    static double p5 = 0.5;

// *** copy r and (q transpose)*b to preserve input and initialize s.
//     in particular, save the diagonal elements of r in x.

    for ( j=0; j<n; j++ )
    {
        for ( i=j; i<n; i++ )
            r[j*ldr+i] = r[i*ldr+j];
        x[j] = r[j*ldr+j];
        wa[j] = qtb[j];
    }
#if BUG
    printf( "qrsolv\n" );
#endif

// *** eliminate the diagonal matrix d using a givens rotation.

    for ( j=0; j<n; j++ )
{

// ***       prepare the row of d to be eliminated, locating the
//     diagonal element using p from the qr factorization.

    if (diag[ ipvt[j] ] == 0.)
        goto L90;
    for ( k=j; k<n; k++ )
        sdiag[k] = 0.;
    sdiag[j] = diag[ ipvt[j] ];

// ***       the transformations to eliminate the row of d
//     modify only a single element of (q transpose)*b
//     beyond the first n, which is initially 0..

    qtbpj = 0.;
    for ( k=j; k<n; k++ )
    {

//        determine a givens rotation which eliminates the
//        appropriate element in the current row of d.

        if (sdiag[k] == 0.)
            continue;
        kk = k + ldr * k; // <! keep this shorthand !>
        if ( fabs(r[kk]) < fabs(sdiag[k]) )
        {
            cotan = r[kk]/sdiag[k];
            sin = p5/sqrt(p25+p25*SQR(cotan));
            cos = sin*cotan;
        }
        else
        {
            tan = sdiag[k]/r[kk];
            cos = p5/sqrt(p25+p25*SQR(tan));
            sin = cos*tan;
        }

// ***          compute the modified diagonal element of r and
//        the modified element of ((q transpose)*b,0).

        r[kk] = cos*r[kk] + sin*sdiag[k];
        temp = cos*wa[k] + sin*qtbpj;
        qtbpj = -sin*wa[k] + cos*qtbpj;
        wa[k] = temp;

// *** accumulate the tranformation in the row of s.

        for ( i=k+1; i<n; i++ )
        {
            temp = cos*r[k*ldr+i] + sin*sdiag[i];
            sdiag[i] = -sin*r[k*ldr+i] + cos*sdiag[i];
            r[k*ldr+i] = temp;
        }
    }
    L90:

// *** store the diagonal element of s and restore
//     the corresponding diagonal element of r.

            sdiag[j] = r[j*ldr+j];
    r[j*ldr+j] = x[j];
}

// *** solve the triangular system for z. if the system is
//     singular, then obtain a least squares solution.

    nsing = n;
    for ( j=0; j<n; j++ )
{
    if ( sdiag[j] == 0. && nsing == n )
        nsing = j;
    if (nsing < n)
        wa[j] = 0;
}

    for ( j=nsing-1; j>=0; j-- )
{
    sum = 0;
    for ( i=j+1; i<nsing; i++ )
        sum += r[j*ldr+i]*wa[i];
    wa[j] = (wa[j] - sum)/sdiag[j];
}

// *** permute the components of z back to components of x.

    for ( j=0; j<n; j++ )
        x[ ipvt[j] ] = wa[j];
}



double lm_enorm( int n, double *x )
{
/*     given an n-vector x, this function calculates the
    *     euclidean norm of x.
    *
    *     the euclidean norm is computed by accumulating the sum of
    *     squares in three different sums. the sums of squares for the
    *     small and large components are scaled so that no overflows
    *     occur. non-destructive underflows are permitted. underflows
    *     and overflows do not occur in the computation of the unscaled
    *     sum of squares for the intermediate components.
    *     the definitions of small, intermediate and large components
    *     depend on two constants, LM_SQRT_DWARF and LM_SQRT_GIANT. the main
    *     restrictions on these constants are that LM_SQRT_DWARF**2 not
    *     underflow and LM_SQRT_GIANT**2 not overflow.
    *
    *     parameters
    *
    *    n is a positive integer input variable.
    *
    *    x is an input array of length n.
 */
    int i;
    double agiant, s1, s2, s3, xabs, x1max, x3max, temp;

    s1 = 0;
    s2 = 0;
    s3 = 0;
    x1max = 0;
    x3max = 0;
    agiant = LM_SQRT_GIANT/( (double) n);

    for ( i=0; i<n; i++ )
    {
        xabs = fabs(x[i]);
        if ( xabs > LM_SQRT_DWARF && xabs < agiant )
        {
// **  sum for intermediate components.
            s2 += xabs*xabs;
            continue;
        }

        if ( xabs >  LM_SQRT_DWARF )
        {
// **  sum for large components.
            if (xabs > x1max)
            {
                temp = x1max/xabs;
                s1 = 1 + s1*SQR(temp);
                x1max = xabs;
            }
            else
            {
                temp = xabs/x1max;
                s1 += SQR(temp);
            }
            continue;
        }
// **  sum for small components.
        if (xabs > x3max)
        {
            temp = x3max/xabs;
            s3 = 1 + s3*SQR(temp);
            x3max = xabs;
        }
        else
        {
            if (xabs != 0.)
            {
                temp = xabs/x3max;
                s3 += SQR(temp);
            }
        }
    }

// *** calculation of norm.

    if (s1 != 0)
        return x1max*sqrt(s1 + (s2/x1max)/x1max);
    if (s2 != 0)
    {
        if (s2 >= x3max)
            return sqrt( s2*(1+(x3max/s2)*(x3max*s3)) );
        else
            return sqrt( x3max*((s2/x3max)+(x3max*s3)) );
    }

    return x3max*sqrt(s3);
}


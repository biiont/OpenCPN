#include "nmea0183.h"

/*
** Author: Samuel R. Blackburn
** CI$: 76300,326
** Internet: sammy@sed.csc.com
**
** You can use it any way you like.
*/


RTE::RTE()
{
   Mnemonic = _T("RTE");
   Empty();
}

RTE::~RTE()
{
   Mnemonic.Empty();
   Empty();
}

void RTE::Empty( void )
{
   total_number_of_messages     = 0;
   last_message_number_received = 0;
   message_number               = 0;
   last_waypoint_number_written = 0;

   TypeOfRoute = RouteUnknown;
   RouteName.Empty();

   delete_all_entries();
}

bool RTE::Parse( const SENTENCE& sentence )
{
   /*
   ** RTE - Routes
   **
   **        1   2   3 4	 5		       x    n
   **        |   |   | |    |           |    |
   ** $--RTE,x.x,x.x,a,c--c,c--c, ..... c--c*hh<CR><LF>
   **
   ** Field Number:
   **  1) Total number of messages being transmitted
   **  2) Message Number
   **  3) Message mode
   **     c = complete route, all waypoints
   **     w = working route, the waypoint you just left, the waypoint you're heading to then all the rest
   **  4) Waypoint ID
   **  x) More Waypoints
   **  n) Checksum
   */

   delete_all_entries();

   int field_number = 1;

   total_number_of_messages = sentence.Integer( 1 );
//   total_number_of_messages = sentence.Double( 1 );

   int this_message_number = sentence.Integer( 2 );
//   double this_message_number = sentence.Double( 2 );

   if ( this_message_number == 1 )
   {
      /*
      ** Make sure we've got a clean list
      */

      delete_all_entries();
   }


   if ( sentence.Field( 3 ).StartsWith(_T("c")) )
   {
      TypeOfRoute = CompleteRoute;
   }
   else if ( sentence.Field( 3 ).StartsWith(_T("w")))
   {
      TypeOfRoute = WorkingRoute;
   }
   else
   {
      TypeOfRoute = RouteUnknown;
   }

   RouteName = sentence.Field( 4 );

   int number_of_data_fields = sentence.GetNumberOfDataFields();
   field_number = 5;

   while( field_number < number_of_data_fields )
   {
      Waypoints.Add( ( sentence.Field( field_number ) )) ;
      field_number++;
   }

   return( TRUE );
}

bool RTE::Write( SENTENCE& sentence )
{

   /*
   ** Let the parent do its thing
   */

   RESPONSE::Write( sentence );

   sentence += total_number_of_messages;
   sentence += message_number;

   switch( TypeOfRoute )
   {
      case CompleteRoute:

            sentence += _T("C");             // uppercase required for GPS MLR FFX312
         break;

      case WorkingRoute:

         sentence += _T("w");
         break;

      default:

//         sentence += "";
         break;
   }

   sentence += RouteName;

   for(unsigned int i=0 ; i < Waypoints.GetCount() ; i++)
         sentence += Waypoints[i];

   sentence.Finish();

   return( TRUE );
}

bool RTE::AddWaypoint(const wxString& name)
{
      Waypoints.Add(name);

      return( TRUE );
}

void RTE::delete_all_entries( void )
{
   Waypoints.Clear();
}

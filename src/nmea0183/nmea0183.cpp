#include "nmea0183.h"

/*
** Author: Samuel R. Blackburn
** CI$: 76300,326
** Internet: sammy@sed.csc.com
**
** You can use it any way you like.
*/

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(MRL);


NMEA0183::NMEA0183()
{
   initialize();

/*
   response_table.Add( (RESPONSE *) &Aam );
   response_table.Add( (RESPONSE *) &Alm );
   response_table.Add( (RESPONSE *) &Apb );
   response_table.Add( (RESPONSE *) &Asd );
   response_table.Add( (RESPONSE *) &Bec );
   response_table.Add( (RESPONSE *) &Bod );
   response_table.Add( (RESPONSE *) &Bwc );
   response_table.Add( (RESPONSE *) &Bwr );
   response_table.Add( (RESPONSE *) &Bww );
   response_table.Add( (RESPONSE *) &Dbt );
   response_table.Add( (RESPONSE *) &Dcn );
   response_table.Add( (RESPONSE *) &Dpt );
   response_table.Add( (RESPONSE *) &Fsi );
   response_table.Add( (RESPONSE *) &Gga );
   response_table.Add( (RESPONSE *) &Glc );
   response_table.Add( (RESPONSE *) &Gll );
   response_table.Add( (RESPONSE *) &Gxa );
   response_table.Add( (RESPONSE *) &Hdg );
   response_table.Add( (RESPONSE *) &Hdt );
   response_table.Add( (RESPONSE *) &Hsc );
   response_table.Add( (RESPONSE *) &Lcd );
   response_table.Add( (RESPONSE *) &Mtw );
   response_table.Add( (RESPONSE *) &Mwv );
   response_table.Add( (RESPONSE *) &Oln );
   response_table.Add( (RESPONSE *) &Osd );
   response_table.Add( (RESPONSE *) &Proprietary );
   response_table.Add( (RESPONSE *) &Rma );
*/
   response_table.Append( (RESPONSE *) &Rmb );
   response_table.Append( (RESPONSE *) &Rmc );
/*
   response_table.Add( (RESPONSE *) &Rot );
   response_table.Add( (RESPONSE *) &Rpm );
   response_table.Add( (RESPONSE *) &Rsa );
   response_table.Add( (RESPONSE *) &Rsd );
   response_table.Add( (RESPONSE *) &Rte );
   response_table.Add( (RESPONSE *) &Sfi );
   response_table.Add( (RESPONSE *) &Stn );
   response_table.Add( (RESPONSE *) &Trf );
   response_table.Add( (RESPONSE *) &Ttm );
   response_table.Add( (RESPONSE *) &Vbw );
   response_table.Add( (RESPONSE *) &Vhw );
   response_table.Add( (RESPONSE *) &Vdr );
   response_table.Add( (RESPONSE *) &Vlw );
   response_table.Add( (RESPONSE *) &Vpw );
   response_table.Add( (RESPONSE *) &Vtg );
   response_table.Add( (RESPONSE *) &Wcv );
   response_table.Add( (RESPONSE *) &Wnc );
   response_table.Add( (RESPONSE *) &Wpl );
   response_table.Add( (RESPONSE *) &Xdr );
   response_table.Add( (RESPONSE *) &Xte );
   response_table.Add( (RESPONSE *) &Xtr );
   response_table.Add( (RESPONSE *) &Zda );
   response_table.Add( (RESPONSE *) &Zfo );
   response_table.Add( (RESPONSE *) &Ztg );
*/
   sort_response_table();
   set_container_pointers();
}

NMEA0183::~NMEA0183()
{
   initialize();
}

void NMEA0183::initialize( void )
{
//   ASSERT_VALID( this );

   ErrorMessage.Empty();
}

void NMEA0183::set_container_pointers( void )
{
//   ASSERT_VALID( this );

   int index = 0;
   int number_of_entries_in_table = response_table.GetCount();

   RESPONSE *this_response = (RESPONSE *) NULL;

   index = 0;

   while( index < number_of_entries_in_table )
   {
      this_response = (RESPONSE *) response_table[ index ];

      this_response->SetContainer( this );

      index++;
   }
}

void NMEA0183::sort_response_table( void )
{
//   ASSERT_VALID( this );

/*
   int index = 0;
   int number_of_entries_in_table = response_table.GetSize();

   RESPONSE *this_response = (RESPONSE *) NULL;
   RESPONSE *that_response = (RESPONSE *) NULL;

   bool sorted = FALSE;

   while( sorted == FALSE )
   {
      sorted = TRUE;

      index = 0;

      while( index < number_of_entries_in_table )
      {
         this_response = (RESPONSE *) response_table.Item( index     );
         that_response = (RESPONSE *) response_table.Item( index + 1 );

         if ( this_response->Mnemonic.Compare( that_response->Mnemonic ) > 0 )
         {
            response_table[ index     ] = that_response;
            response_table[ index + 1 ] = this_response;

            sorted = FALSE;
         }

         index++;
      }
   }
*/
}

/*
** Public Interface
*/

bool NMEA0183::IsGood( void ) const
{
//   ASSERT_VALID( this );

   /*
   ** NMEA 0183 sentences begin with $ and and with CR LF
   */

   if ( sentence.Sentence[ 0 ] != '$' )
   {
      return( FALSE );
   }

   /*
   ** Next to last character must be a CR
   */

   if ( sentence.Sentence.Mid( sentence.Sentence.Len() - 2, 1 ) != CARRIAGE_RETURN )
   {
      return( FALSE );
   }

   if ( sentence.Sentence.Right( 1 ) != LINE_FEED )
   {
      return( FALSE );
   }

   return( TRUE );
}

bool NMEA0183::Parse( void )
{
//   ASSERT_VALID( this );

   bool return_value = FALSE;

   if ( IsGood() )
   {
//      int index       = 0;
      int comparison  = 0;
//      int drop_dead   = 0;
//      int exit_loop   = 0;
//      int lower_limit = 0;
//      int upper_limit = 0;

      wxString mnemonic;

      RESPONSE *response_p = (RESPONSE *) NULL;

      mnemonic = sentence.Field( 0 );

      /*
      ** See if this is a proprietary field
      */

      if ( mnemonic.Left( 1 ) == 'P' )
      {
         mnemonic = "P";
      }
      else
      {
         mnemonic = mnemonic.Right( 3 );
      }

      /*
      ** Set up our default error message
      */

      ErrorMessage = mnemonic;
      ErrorMessage += " is an unknown type of sentence";

      LastSentenceIDReceived = mnemonic;


//		Traverse the response list to find a mnemonic match
      
		wxMRLNode *node = response_table.GetFirst();

	  while(node)
	  {
		  RESPONSE *resp = node->GetData();

	      comparison = mnemonic.Cmp( resp->Mnemonic );

			 if ( comparison == 0 )
			 {
				response_p = (RESPONSE *) resp;
				return_value = response_p->Parse( sentence );

				/*
				** Set your ErrorMessage
				*/

				if ( return_value == TRUE )
				{
				   ErrorMessage = "No Error";
				   LastSentenceIDParsed = response_p->Mnemonic;
				   TalkerID = talker_id( sentence );
				   ExpandedTalkerID = expand_talker_id( TalkerID );
				}
				else
				{
				   ErrorMessage = response_p->ErrorMessage;
				}

				break;
			 }

		  node = node->GetNext();
	  }
	  
   }
   else
   {
      return_value = FALSE;
   }

   return( return_value );
}

NMEA0183& NMEA0183::operator << ( const char *source )
{
//   ASSERT_VALID( this );

   sentence = source;

   return( *this );
}

NMEA0183& NMEA0183::operator >> ( wxString& destination )
{
//   ASSERT_VALID( this );

   destination = sentence;

   return( *this );
}
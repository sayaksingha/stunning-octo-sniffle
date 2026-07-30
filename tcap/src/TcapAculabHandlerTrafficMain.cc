static const char *id="@(#) $Id: TcapAculabHandlerTrafficMain.cc,v 1.1.4.2.10.1 2021/12/01 12:43:27 jamesjac Exp $";
//---------------------------------------------------------------
// NAME : SsfcTrafficMain.cc
//
// COPYRIGHT
// TSS : - Copyright (C) 2001 Tayana Software Solution Pvt Ltd.,
//         All rights reserved. No part of this computer program
//         may be used or reproduced in any form by any
//         means without prior written permission of
//         Tayana Software Solution Pvt Ltd.
//
// DESCRIPTION
//
// Originated: Paresh                          Date:29-Mar-2010
//----------------------------------------------------------------
#include "TcapAculabHandlerTraffic.h"

TEXT                 gProcessName[SS7_MAX_PROCESS_NAME + 1] = "";
INT32                gDelaySeconds;
CTrace               gTrace("TRACE_TRAFFIC_ENV");
Log                  gLog;

//------------------------------------------------------------------------
// METHOD      : main 
// DESCRIPTION : main function for Aculab tcap traffic display 
// PARAMETER   : int, char **
// RETURN      : int 
//-------------------------------------------------------------------------

int main(int argc,char* argv[])
{

   if(argc > 2)
   {
      printf("\n USAGE:<ProcessName> <Refresh time> \n");
      printf("export %s=1\n","TRACE_TRAFFIC_ENV");
      exit(1);
   }

   if(argc == 2)
   {
      gDelaySeconds = atoi(argv[1]);
      if(gDelaySeconds < 1)
      {
         printf(" Refresh time should be greater than or equal to 1 second\n");
         printf(" Refresh time is being set to 1 second...\n");
         gDelaySeconds = 1;
      }
   }

   else 
      if (argc == 1)
      {
         gDelaySeconds = DELAY_DEFAULT;
         printf("Default Refresh time = %ld\n",gDelaySeconds);
      }

   sprintf(gProcessName, "%s", basename(argv[0]));
   printf("%s: Process Starting....\n", gProcessName); 

   //Load signals
   //Util::LoadSignals();

   //Set Trace Key
   gTrace.SetTraceKey("TRACE_TRAFFIC_ENV");

   Traffic              lTraffic;
   //Initialize Spam Traffic object
   if( true != lTraffic.Init())
   {
      printf ("%s: \33[31mFailed to initialize Spam Traffic object.Exiting...\33[0m\n", gProcessName);
      return 1;
   }

   printf ("%s: \33[32mProcess initialization okay...\33[0m\n", gProcessName);

   //Taking the snap shot of shared memory for events
   if (true != lTraffic.GetInitialSnapShot())
   {
      printf("Failed to read from shared memory\n");
   }

   sleep(gDelaySeconds);       
   system("clear");

   while(1)
   {

      if(true !=lTraffic.ProcessTraffic())
      {
         printf("The Traffic request failed...\n");
      }

      sleep(gDelaySeconds);
      system("clear");
   }

   lTraffic.DeInit();
   printf("%s: Process is Shutting Down....\n",gProcessName);

   return 0;
}



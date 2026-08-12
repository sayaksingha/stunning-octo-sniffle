// @(#) $Id: SigtranConstDef.h,v 1.1.6.2 2014/04/21 10:47:46 sreenu Exp $
//---------------------------------------------------------------
// NAME : SigtranConstDef.h 
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
// Originated By: Mohammed Rafi A.              Date:25-APR-2008
//----------------------------------------------------------------
#ifndef  SIGTRAN_COSNT_DEF_H
#define  SIGTRAN_COSNT_DEF_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <libgen.h>
#include <math.h>

#include "prelude.h"
//GenType
#include "Types.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ProcessLock.h"
#include "StrTokenizer.h"
#include "LogApi.h"
//#include "OamLogCodes.h"
#include "CfgRead.h"
#include "tdapi.h"
#include "tdapiM3UA.h"


#define MAP_NAME          	    "MAP"
#define CAP_NAME                    "CAP" 
#define TCAP_NAME          	    "TCAP"
#define ISUP_NAME                   "ISUP"

#define SIGTRC                      12
#define SIGCFG                      10

#define TRACE_SIG_ENV 		   "TRACE_SIGTRAN"

#define SIGTRAN_CFG                 "sigtran.cfg"  

#define MAX_SS7_PROCESS_NAME        30


#endif

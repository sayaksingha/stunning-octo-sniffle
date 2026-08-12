// @(#) $Id: SigtranLimits.h,v 1.1.6.1 2011/04/12 09:53:29 shijuc Exp $
//---------------------------------------------------------------
// NAME : SigtranLimits.h
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

#ifndef  SIGTRAN_LIMITS_H
#define  SIGTRAN_LIMITS_H

#define  MAX_STR_LEN                150 

#define MIN_NODE_NAME_LEN             1
#define MAX_NODE_NAME_LEN            80

#define MAX_PROVIDER_LEN             80

#define MIN_DESTINATIONS              1 
#define MAX_DESTINATIONS            999

#define MIN_DEST_ID                   0
#define MAX_DEST_ID                 998

#define MIN_POINTCODE                 1
#define MAX_POINTCODE             16383

#define MIN_CIC_RANGES                0

#define MIN_ROUTING_CONTEXT           0
#define MAX_ROUTING_CONTEXT        9999

#define MIN_CFG_VAL_LEN               1
#define MAX_CFG_VAL_LEN             200       // max len of config param values

#define MIN_REMOTE_SGP                0
#define MAX_REMOTE_SGP               10

#define  MIN_NODE_LEN                 1
#define  MAX_NODE_LEN                20  

#define  MIN_NET_IND                  0
#define  MAX_NET_IND                255

#define  MIN_SERVICE_NODE             0
#define  MAX_SERVICE_NODE            10

#define  MIN_SIO                      0
#define  MAX_SIO                      5

#define  MIN_APP_NUMBER               1
#define  MAX_APP_NUMBER              10

#define  MAX_NUM_OF_LAS              10

#define  MAX_IP_LEN                  25

#define  MAX_NUM_OF_SSN              10

#define  MIN_CIC_RANGE                0
#define  MAX_CIC_RANGE             9999

#define  MAX_PORTS                 9999

#define  MIN_CONGESTION_VAL           0
#define  MAX_CONGESTION_VAL        9999

#endif

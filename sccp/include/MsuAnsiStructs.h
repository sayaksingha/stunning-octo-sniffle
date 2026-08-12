//--------------------------------------------------------------------------
// NAME : MsuAnsiStructs.h
//
// COPYRIGHT
// TSS : - Copyright (C) 2016 Tayana Software Solution Pvt Ltd.,
//         All rights reserved. No part of this computer program
//         may be used or reproduced in any form by any
//         means without prior written permission of
//         Tayana Software Solution Pvt Ltd.
//
// DESCRIPTION
// ANSI TCAP constant definitions and structures for sccp_ansi handler.
//--------------------------------------------------------------------------

#ifndef INCL_SS7_MSU_ANSI_STRUCTS_H
#define INCL_SS7_MSU_ANSI_STRUCTS_H

// ANSI TCAP Package (Transaction) Tags
#define SS7_ANSI_TRANS_QUERY_WO_PERM        0xE1
#define SS7_ANSI_TRANS_QUERY_W_PERM         0xE2
#define SS7_ANSI_TRANS_RESP                 0xE4
#define SS7_ANSI_TRANS_CONV_W_PERM          0xE5
#define SS7_ANSI_TRANS_CONV_WO_PERM         0xE6
#define SS7_ANSI_TRANS_UNI                  0xE8
#define SS7_ANSI_TRANS_ABORT                0xF6

// ANSI TCAP Transaction ID Tag
#define SS7_ANSI_TRANS_ID_TAG               0xC7

// Ab Comment : Fixed completely incorrect ASN.1 tags according to ANSI T1.114 BER encoding.
// ANSI TCAP Component Portion Tag
#define SS7_ANSI_COMP_PORTION_TAG           0xE8

// ANSI TCAP Component Tags
#define SS7_ANSI_COMP_INVOKE_LAST           0xE9
#define SS7_ANSI_COMP_INVOKE_NOT_LAST       0xE1
#define SS7_ANSI_COMP_RET_RESULT_LAST       0xEA
#define SS7_ANSI_COMP_RET_RESULT_NOT_LAST   0xE2
#define SS7_ANSI_COMP_RET_ERROR             0xEB
#define SS7_ANSI_COMP_REJECT                0xEC

// ANSI TCAP Component Sub-tags
#define SS7_ANSI_INVOKE_ID_TAG              0xCF
#define SS7_ANSI_LINKED_ID_TAG              0xDA

#define SS7_ANSI_NATIONAL_OP_CODE_TAG       0xD0
#define SS7_ANSI_PRIVATE_OP_CODE_TAG        0xD1

#define SS7_ANSI_NATIONAL_ERR_CODE_TAG      0xD3
#define SS7_ANSI_PRIVATE_ERR_CODE_TAG       0xD4

// ANSI TCAP Problem Code Tags
#define SS7_ANSI_PBLM_GENERAL               0xD5
#define SS7_ANSI_PBLM_INVOKE                0xD6
#define SS7_ANSI_PBLM_RET_RESULT            0xD7
#define SS7_ANSI_PBLM_RET_ERROR             0xD8
#define SS7_ANSI_PBLM_TRANSACTION           0xD9

// ANSI Parameter Set/Sequence tags
#define SS7_ANSI_PARAM_SEQUENCE_TAG         0xF2
#define SS7_ANSI_PARAM_SET_TAG              0xF3

#endif // INCL_SS7_MSU_ANSI_STRUCTS_H

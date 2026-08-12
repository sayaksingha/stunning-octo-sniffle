/* trace_sprt_show.h - helper functions for SPRT API tracing */

#ifndef INCLUDED_TRACE_SPRT_SHOW_H
#define INCLUDED_TRACE_SPRT_SHOW_H

#include "smsprt.h"

LOCALDEC void show_sprt_message_tx_in(SM_SPRT_SEND_MESSAGE_PARMS *pp);
LOCALDEC void show_sprt_message_rx_in(SM_SPRT_READ_MESSAGE_PARMS *pp);
LOCALDEC void show_sprt_message_rx_out(SM_SPRT_READ_MESSAGE_PARMS *pp);

#endif

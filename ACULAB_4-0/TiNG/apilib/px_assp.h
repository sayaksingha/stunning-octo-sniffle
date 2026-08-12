#ifndef INCLUDED_PX_ASSP_H
#define INCLUDED_PX_ASSP_H

#include "TiNGcommon.h"
#include "../assp/assp_packet.h"

struct assp_conn;
LOCALDEC void dump_assp_pkt(struct assp_conn *cnx, PACKET *pkt, char *txt);

#endif

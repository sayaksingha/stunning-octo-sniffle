/* px_ioctls.c - ioctl implementation for Prosody X */

#include "px_ioctls.h"
#include "TiNGcommon.h"
#include "trace.h"
#include "px_ioctl_allocts.h"
#include "px_ioctl_cardinfo.h"
#include "px_ioctl_chevent.h"
#include "px_ioctl_confin.h"
#include "px_ioctl_confinfo.h"
#include "px_ioctl_dbg.h"
#include "px_ioctl_dest.h"
#include "px_ioctl_friend.h"
#include "px_ioctl_intslist.h"
#include "px_ioctl_io.h"
#include "px_ioctl_mem.h"
#include "px_ioctl_modinfo.h"
#include "px_ioctl_msg.h"
#include "px_ioctl_mstate.h"
#include "px_ioctl_outts.h"
#include "px_ioctl_stats.h"
#include "px_ioctl_strinfo.h"
#include "px_ioctl_switch.h"
#include "px_ioctl_task.h"
#include "px_ioctl_tasklist.h"
#include "px_ioctl_tsdest.h"
#include "px_ioctl_xfer.h"

#include <stdio.h>

STATIC int px_ioctl_ting_rdio(PX_CARDCONN_VTBL_DATA *pvp, struct ting_cardreg *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_cardreg\n");
 }
 if (pvp->dest_type != TiNG_DEST_CARD) {
 	if (TiNGtrace) olog("rdio: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdio(pvp, p);
}

STATIC int px_ioctl_ting_rdmem(PX_CARDCONN_VTBL_DATA *pvp, struct ting_sharcmem *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_sharcmem\n");
 }
 if (pvp->dest_type != TiNG_DEST_MODULE) {
 	if (TiNGtrace) olog("rdmem: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdmem(pvp->u.mod, p);
}

STATIC int px_ioctl_ting_wrmem(PX_CARDCONN_VTBL_DATA *pvp, struct ting_sharcmem *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_sharcmem\n");
 }
 if (pvp->dest_type != TiNG_DEST_MODULE) {
 	if (TiNGtrace) olog("wrmem: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrmem(pvp->u.mod, p);
}

STATIC int px_ioctl_ting_rdstats(PX_CARDCONN_VTBL_DATA *pvp, struct ting_statistics *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_statistics\n");
 }
 if (pvp->dest_type != TiNG_DEST_MODULE) {
 	if (TiNGtrace) olog("rdstats: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdstats(pvp->u.mod, p);
}

STATIC int px_ioctl_ting_rdmsg(PX_CARDCONN_VTBL_DATA *pvp, struct ting_message *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_message\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdtask: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdmsg(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrmsg(PX_CARDCONN_VTBL_DATA *pvp, struct ting_message *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_message\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdtask: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrmsg(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rddest(PX_CARDCONN_VTBL_DATA *pvp, struct ting_destination *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_destination\n");
 }
 return px_rddest(pvp, p);
}

STATIC int px_ioctl_ting_wrdest(PX_CARDCONN_VTBL_DATA *pvp, struct ting_destination *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_destination\n");
 }
 return px_wrdest(pvp, p);
}

STATIC int px_ioctl_ting_rdtask(PX_CARDCONN_VTBL_DATA *pvp, struct ting_task *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_task\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdtask: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdtask(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrtask(PX_CARDCONN_VTBL_DATA *pvp, struct ting_task *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_task\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrtask: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrtask(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rdoutts(PX_CARDCONN_VTBL_DATA *pvp, struct ting_tslist *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_tslist\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdoutts: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdoutts(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wroutts(PX_CARDCONN_VTBL_DATA *pvp, struct ting_tslist *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_tslist\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wroutts: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wroutts(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rdintslist(PX_CARDCONN_VTBL_DATA *pvp, struct ting_tslist *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_tslist\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdintslist: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdintslist(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrintslist(PX_CARDCONN_VTBL_DATA *pvp, struct ting_tslist *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_tslist\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrintslist: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrintslist(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rdswitch(PX_CARDCONN_VTBL_DATA *pvp, struct ting_switch *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_switch\n");
 }
 if (pvp->dest_type != TiNG_DEST_CARD) {
 	if (TiNGtrace) olog("rdswitch: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdswitch(pvp, p);
}

STATIC int px_ioctl_ting_rdxfer(PX_CARDCONN_VTBL_DATA *pvp, struct ting_xfer *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_xfer\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrintslist: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdxfer(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrxfer(PX_CARDCONN_VTBL_DATA *pvp, struct ting_xfer *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_xfer\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrintslist: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrxfer(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rddbg(PX_CARDCONN_VTBL_DATA *pvp, struct ting_debug *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_debug\n");
 }
 return px_rddbg(pvp, p);
}

STATIC int px_ioctl_ting_wrdbg(PX_CARDCONN_VTBL_DATA *pvp, struct ting_debug *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_debug\n");
 }
 return px_wrdbg(pvp, p);
}

STATIC int px_ioctl_ting_rdmodinfo(PX_CARDCONN_VTBL_DATA *pvp, struct ting_modinfo *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_modinfo\n");
 }
 if (pvp->dest_type != TiNG_DEST_MODULE) {
 	if (TiNGtrace) olog("rdmodinfo: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdmodinfo(pvp->u.mod, p);
}

STATIC int px_ioctl_ting_rdstrinfo(PX_CARDCONN_VTBL_DATA *pvp, struct ting_strinfo *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_strinfo\n");
 }
 if (pvp->dest_type != TiNG_DEST_MODULE) {
 	if (TiNGtrace) olog("rdmodinfo: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdstrinfo(pvp->u.mod, p);
}

STATIC int px_ioctl_ting_rdmstate(PX_CARDCONN_VTBL_DATA *pvp, struct ting_mstate *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_mstate\n");
 }
 if (pvp->dest_type != TiNG_DEST_MODULE) {
 	if (TiNGtrace) olog("rdmstate: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdmstate(pvp->u.mod, p);
}

STATIC int px_ioctl_ting_wrmstate(PX_CARDCONN_VTBL_DATA *pvp, struct ting_mstate *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_mstate\n");
 }
 if (pvp->dest_type != TiNG_DEST_MODULE) {
 	if (TiNGtrace) olog("wrmstate: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrmstate(pvp->u.mod, p);
}

STATIC int px_ioctl_ting_rdfriend(PX_CARDCONN_VTBL_DATA *pvp, struct ting_friend *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_friend\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdfriend: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdfriend(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrfriend(PX_CARDCONN_VTBL_DATA *pvp, struct ting_friend *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_friend\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrfriend: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrfriend(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rdchevent(PX_CARDCONN_VTBL_DATA *pvp, struct ting_chevent *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_chevent\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdchevent: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdchevent(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrchevent(PX_CARDCONN_VTBL_DATA *pvp, struct ting_chevent *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_chevent\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrchevent: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrchevent(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rdtasklist(PX_CARDCONN_VTBL_DATA *pvp, struct ting_taskitem *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_taskitem\n");
 }
 if (pvp->dest_type != TiNG_DEST_MODULE) {
 	if (TiNGtrace) olog("rdtasklist: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdtasklist(pvp->u.mod, p);
}

STATIC int px_ioctl_ting_rdcardinfo(PX_CARDCONN_VTBL_DATA *pvp, struct ting_cardinfo *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_cardinfo\n");
 }
 return px_rdcardinfo(pvp, p);
}

STATIC int px_ioctl_ting_rdconfin(PX_CARDCONN_VTBL_DATA *pvp, struct ting_confin *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_confin\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdconfin: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdconfin(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrconfin(PX_CARDCONN_VTBL_DATA *pvp, struct ting_confin *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_confin\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrconfin: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrconfin(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rdconfinfo(PX_CARDCONN_VTBL_DATA *pvp, struct ting_confinfo *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_confinfo\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdconfinfo: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdconfinfo(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rdallocts(PX_CARDCONN_VTBL_DATA *pvp, struct ting_allocts *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_allocts\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdallocts: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdallocts(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrallocts(PX_CARDCONN_VTBL_DATA *pvp, struct ting_allocts *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_allocts\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrallocts: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrallocts(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_rdtsdest(PX_CARDCONN_VTBL_DATA *pvp, struct ting_tsdest *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_tsdest\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("rdtsdest: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_rdtsdest(pvp->u.chan, p);
}

STATIC int px_ioctl_ting_wrtsdest(PX_CARDCONN_VTBL_DATA *pvp, struct ting_tsdest *p, unsigned asiz)
{
 if (asiz < sizeof(*p)) {
 	die("assertion failed: size < sizeof struct ting_tsdest\n");
 }
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("wrtsdest: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 return px_wrtsdest(pvp->u.chan, p);
}

LOCALDEF int px_ioctls(PX_CARDCONN_VTBL_DATA *pvp, unsigned long cmd, void *ap, unsigned asiz)
{
 switch (cmd) {
 case IOCTL_TiNG_RDIO:
	return px_ioctl_ting_rdio(pvp, ap, asiz);
 case IOCTL_TiNG_RDMEM:
	return px_ioctl_ting_rdmem(pvp, ap, asiz);
 case IOCTL_TiNG_WRMEM:
	return px_ioctl_ting_wrmem(pvp, ap, asiz);
 case IOCTL_TiNG_RDSTATS:
	return px_ioctl_ting_rdstats(pvp, ap, asiz);
 case IOCTL_TiNG_RDMSG:
	return px_ioctl_ting_rdmsg(pvp, ap, asiz);
 case IOCTL_TiNG_WRMSG:
	return px_ioctl_ting_wrmsg(pvp, ap, asiz);
 case IOCTL_TiNG_RDDEST:
	return px_ioctl_ting_rddest(pvp, ap, asiz);
 case IOCTL_TiNG_WRDEST:
	return px_ioctl_ting_wrdest(pvp, ap, asiz);
 case IOCTL_TiNG_RDTASK:
	return px_ioctl_ting_rdtask(pvp, ap, asiz);
 case IOCTL_TiNG_WRTASK:
	return px_ioctl_ting_wrtask(pvp, ap, asiz);
 case IOCTL_TiNG_RDOUTTS:
	return px_ioctl_ting_rdoutts(pvp, ap, asiz);
 case IOCTL_TiNG_WROUTTS:
	return px_ioctl_ting_wroutts(pvp, ap, asiz);
 case IOCTL_TiNG_RDINTSLIST:
	return px_ioctl_ting_rdintslist(pvp, ap, asiz);
 case IOCTL_TiNG_WRINTSLIST:
	return px_ioctl_ting_wrintslist(pvp, ap, asiz);
 case IOCTL_TiNG_RDSWITCH:
	return px_ioctl_ting_rdswitch(pvp, ap, asiz);
 case IOCTL_TiNG_RDXFER:
	return px_ioctl_ting_rdxfer(pvp, ap, asiz);
 case IOCTL_TiNG_WRXFER:
	return px_ioctl_ting_wrxfer(pvp, ap, asiz);
 case IOCTL_TiNG_RDDBG:
	return px_ioctl_ting_rddbg(pvp, ap, asiz);
 case IOCTL_TiNG_WRDBG:
	return px_ioctl_ting_wrdbg(pvp, ap, asiz);
 case IOCTL_TiNG_RDMSTATE:
	return px_ioctl_ting_rdmstate(pvp, ap, asiz);
 case IOCTL_TiNG_WRMSTATE:
	return px_ioctl_ting_wrmstate(pvp, ap, asiz);
 case IOCTL_TiNG_RDMODINFO:
	return px_ioctl_ting_rdmodinfo(pvp, ap, asiz);
 case IOCTL_TiNG_RDSTRINFO:
	return px_ioctl_ting_rdstrinfo(pvp, ap, asiz);
 case IOCTL_TiNG_RDFRIEND:
	return px_ioctl_ting_rdfriend(pvp, ap, asiz);
 case IOCTL_TiNG_WRFRIEND:
	return px_ioctl_ting_wrfriend(pvp, ap, asiz);
 case IOCTL_TiNG_RDCHEVENT:
	return px_ioctl_ting_rdchevent(pvp, ap, asiz);
 case IOCTL_TiNG_WRCHEVENT:
	return px_ioctl_ting_wrchevent(pvp, ap, asiz);
 case IOCTL_TiNG_RDTASKLIST:
	return px_ioctl_ting_rdtasklist(pvp, ap, asiz);
 case IOCTL_TiNG_RDCARDINFO:
	return px_ioctl_ting_rdcardinfo(pvp, ap, asiz);
 case IOCTL_TiNG_RDCONFIN:
	return px_ioctl_ting_rdconfin(pvp, ap, asiz);
 case IOCTL_TiNG_WRCONFIN:
	return px_ioctl_ting_wrconfin(pvp, ap, asiz);
 case IOCTL_TiNG_RDCONFINFO:
	return px_ioctl_ting_rdconfinfo(pvp, ap, asiz);
 case IOCTL_TiNG_RDALLOCTS:
	return px_ioctl_ting_rdallocts(pvp, ap, asiz);
 case IOCTL_TiNG_WRALLOCTS:
	return px_ioctl_ting_wrallocts(pvp, ap, asiz);
 case IOCTL_TiNG_RDTSDEST:
	return px_ioctl_ting_rdtsdest(pvp, ap, asiz);
 case IOCTL_TiNG_WRTSDEST:
	return px_ioctl_ting_wrtsdest(pvp, ap, asiz);
 default:
 	if (TiNGtrace) olog("px_ioctl: unknown ioctl code: %d\n", cmd);
	return 1; 
 }
}

#include <stdlib.h>
#define LOCALDEC static
#define LOCALDEF static
#ifndef STATIC
#define STATIC static
#endif

// prefix all visible assp related functions
#define MD5Final aculab_TiNG_MD5Final
#define MD5Init aculab_TiNG_MD5Init
#define MD5Update64 aculab_TiNG_MD5Update64
#define SHATransform aculab_TiNG_SHATransform
#define assp_abort aculab_TiNG_assp_abort
#define assp_conn_vtbl aculab_TiNG_assp_conn_vtbl
#define assp_connect aculab_TiNG_assp_connect
#define assp_error aculab_TiNG_assp_error
#define assp_handle_rx aculab_TiNG_assp_handle_rx
#define assp_log aculab_TiNG_assp_log
#define assp_rx_cid aculab_TiNG_assp_rx_cid
#define assp_tcb_dtor aculab_TiNG_assp_tcb_dtor
#define assp_tcb_init aculab_TiNG_assp_tcb_init
#define assp_tcb_stats aculab_TiNG_assp_tcb_stats
#define assp_trace aculab_TiNG_assp_trace
#define assp_txpacket aculab_TiNG_assp_txpacket
#define assp_txready aculab_TiNG_assp_txready
#define assp_validate aculab_TiNG_assp_validate
#define bake_cookie aculab_TiNG_bake_cookie
#define decode_str aculab_TiNG_decode_str
#define enqueue aculab_TiNG_enqueue
#define md5_stream aculab_TiNG_md5_stream
#define md5s_final aculab_TiNG_md5s_final
#define md5s_include aculab_TiNG_md5s_include
#define nice_cookie aculab_TiNG_nice_cookie
#define pstate aculab_TiNG_pstate
#define randent32 aculab_TiNG_randent32
#define randent_addrand aculab_TiNG_randent_addrand
#define randent_init aculab_TiNG_randent_init
#define seprintf_varargs aculab_TiNG_seprintf_varargs
#define time_string aculab_TiNG_time_string
#define time_string_now aculab_TiNG_time_string_now
#define time_tai_uS aculab_TiNG_time_tai_uS
#define time_until aculab_TiNG_time_until
#define time_when aculab_TiNG_time_when
#define txq_clear aculab_TiNG_txq_clear
#define txq_dequeue aculab_TiNG_txq_dequeue
#define txq_enqueue aculab_TiNG_txq_enqueue
#define txq_init aculab_TiNG_txq_init
#define vseprintf_varargs aculab_TiNG_vseprintf_varargs


//#define TIMEQUEUE_SPARE
#define INLINE_TIMEFUNC

#ifdef TiNGTYPE_LINUX
#include "../assp/randent_dev.c"
#define USE_GETTIMEOFDAY
#endif

#ifdef TiNGTYPE_QNX
#include "../assp/randent.c"
#define USE_GETTIMEOFDAY
#endif

#ifdef TiNGTYPE_WINNT
//#define abort()  __asm { __asm int 3 }
	// get this in before any of the rest has a chance to include windows.h
#include "../libutil/WINNT/wind.h"
#include "WINNT/condvar.c"
#include "WINNT/mutx.c"
#include "../assp/randent.c"
#define USE_RDTSC
#endif

#include "TiNGo_dtlsrx.c"
#include "TiNGo_dtlstx.c"
#include "TiNGo_fmprx.c"
#include "TiNGo_fmptx.c"
#include "TiNGo_h223mux.c"
#include "TiNGo_rtcph.c"
#include "TiNGo_sntp.c"
#include "TiNGo_sprt.c"
#include "TiNGo_vidplay.c"
#include "TiNGo_vidrec.c"
#include "TiNGo_vidmprx.c"
#include "TiNGo_vidmptx.c"
#include "TiNGo_vmprx.c"
#include "TiNGo_vmptx.c"
#include "TiNGo_vmptone.c"
#include "TiNGo_vmpcsrc.c"
#include "TiNGdtls.c"
#include "TiNGfmplib.c"
#include "TiNGh223lib.c"
#include "TiNGrtplib.c"
#include "TiNGsntp.c"
#include "TiNGsprt.c"
#include "TiNGvideo.c"
#define PROVIDE_TRACE
#include "../assp/assp_main.c"
#include "../assp/asspdbg.c"
#include "../assp/assptime.c"
#include "../assp/cookie.c"
#include "../assp/decode_str.c"
#include "../assp/md5.c"
#include "../assp/md5_stream.c"
#include "../assp/pktqueue.c"
#include "../assp/sha.c"
#define TIME_TAI64N
#include "../assp/timefn.c"
#include "../assp/txqueue.c"
#include "../assp/vseprintf.c"
#include "assp_conn.c"
#include "assp_group.c"
#include "cardconn_px.c"
#include "create_socket.c"
#include "pathipv4.c"
#include "px_assp.c"
#include "px_chan.c"
#include "px_event.c"
#include "px_ioctl_allocts.c"
#include "px_ioctl_cardinfo.c"
#include "px_ioctl_chevent.c"
#include "px_ioctl_confin.c"
#include "px_ioctl_confinfo.c"
#include "px_ioctl_dbg.c"
#include "px_ioctl_dest.c"
#include "px_ioctl_friend.c"
#include "px_ioctl_intslist.c"
#include "px_ioctl_io.c"
#include "px_ioctl_mem.c"
#include "px_ioctl_msg.c"
#include "px_ioctl_mstate.c"
#include "px_ioctl_modinfo.c"
#include "px_ioctl_outts.c"
#include "px_ioctl_stats.c"
#include "px_ioctl_strinfo.c"
#include "px_ioctl_switch.c"
#include "px_ioctl_task.c"
#include "px_ioctl_tasklist.c"
#include "px_ioctl_tsdest.c"
#include "px_ioctl_xfer.c"
#include "px_ioctls.c"
#include "px_mod.c"
#include "px_util.c"
#include "pros_x.c"
#include "refcnt.c"
#include "timequeue_pq_evnt.c"
#include "trace_rtp_show.c"
#include "trace_smvideo_show.c"
#include "trace_sprt_show.c"

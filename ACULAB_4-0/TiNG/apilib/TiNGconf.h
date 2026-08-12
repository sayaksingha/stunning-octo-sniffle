/* TiNGconf.h - conferencing */

#ifndef INCLUDED_TINGCONF_H
#define INCLUDED_TINGCONF_H

#include "smbesp.h"

LOCALDEC int real_sm_conf_prim_attach(struct sm_conf_prim_attach_parms *cp);
LOCALDEC int real_sm_conf_prim_detach(struct sm_conf_prim_detach_parms *cp);
LOCALDEC int real_sm_conf_prim_start(struct sm_conf_prim_start_parms *cp);
LOCALDEC int real_sm_conf_prim_clone(struct sm_conf_prim_clone_parms *cp);
LOCALDEC int real_sm_conf_prim_add(struct sm_conf_prim_add_parms *cp);
LOCALDEC int real_sm_conf_prim_leave(struct sm_conf_prim_leave_parms *cp);
LOCALDEC int real_sm_conf_prim_info(struct sm_conf_prim_info_parms *cp);
LOCALDEC int real_sm_conf_prim_adj_input(struct sm_conf_prim_adj_input_parms *cp);
LOCALDEC int real_sm_conf_prim_adj_input_settings(struct sm_conf_prim_adj_input_settings_parms *cp);
LOCALDEC int real_sm_conf_prim_adj_tracking(struct sm_conf_prim_adj_tracking_parms *cp);
LOCALDEC int real_sm_conf_prim_adj_output(struct sm_conf_prim_adj_output_parms *cp);
LOCALDEC int real_sm_conf_prim_abort(tSMChannelId cp);
LOCALDEC int real_sm_conf_prim_stop(struct sm_conf_prim_stop_parms *stopp);
LOCALDEC int real_sm_conf_prim_status(struct sm_conf_prim_status_parms *statusp);
LOCALDEC int real_sm_conf_prim_config_activity_reporting(struct sm_conf_prim_config_activity_reporting_parms *activityp);
LOCALDEC int real_sm_set_sidetone_channel(struct sm_set_sidetone_channel_parms *cp);

#endif

#ifndef IFP_H_INCLUDED
#define IFP_H_INCLUDED

/* ifp.h defines stuff used in both the api library and the firmware */

typedef enum T38_TYPE_OF_MSG_t30indicator
{	
	e_no_signal,
    e_cng,
    e_ced,
    e_v21_preamble,
    e_v27_2400_training,
    e_v27_4800_training,
    e_v29_7200_training,
    e_v29_9600_training,
    e_v17_7200_short_training,
    e_v17_7200_long_training,
    e_v17_9600_short_training,
    e_v17_9600_long_training,
    e_v17_12000_short_training,
    e_v17_12000_long_training,
    e_v17_14400_short_training,
    e_v17_14400_long_training,
	e_v8_ansam,
	e_v8_signal,
	e_v34_cntl_channel_1200,
	e_v34_pri_channel,
	e_v34_cc_retrain,
}T38_T30INDICATOR;


typedef enum T38_TYPE_OF_MSG_data
{
    e_v21,
    e_v27_2400,
    e_v27_4800,
    e_v29_7200,
    e_v29_9600,
    e_v17_7200,
    e_v17_9600,
    e_v17_12000,
    e_v17_14400,
	e_v8,
	e_v34_pri_rate,
	e_v34_cc_1200,
	e_v34_pri_ch,
}T38_DATA;      

typedef enum T38_field_type 
{
    e_hdlc_data,
    e_hdlc_sig_end,
    e_hdlc_fcs_OK,
    e_hdlc_fcs_BAD,
    e_hdlc_fcs_OK_sig_end,
    e_hdlc_fcs_BAD_sig_end,
    e_t4_non_ecm_data,
    e_t4_non_ecm_sig_end,
	e_cm_message,
	e_jm_message,
	e_ci_message,
	e_v34_rate,
}T38FIELDTYPE;

#endif

/*
 * File        : rmsm.h
 * Description : header for TiNG resource manager API
 * Created     : 6/7/2005
 * Author      : Charles MacNeill
 *
 * $Revision: 1.26 $
 * $Date: 2016/10/27 08:15:20 $
 *
 * Copyright Aculab plc, 2005-2006
 * All Rights Reserved
 */
 
/****************************************************************/
/* The following inputs are used in the functions below:        */
/* Card_id is the value returned by acu_open_card.				*/
/* module_ix is the physical module number of the module to open*/
/* This is the zero-based index of the module.                  */
/* filename is the path to a resourcefile                       */
/* resource_string is a string containing resource names and    */
/*   quantities.                                                */
/* call_type is a string containing a resource name             */
/* number is the number of call_types to be allocated/deallocated*/
/* ERROR_NOT_FOUND is returned if the card_id is invalid.       */
/* ACU_ERROR_COMMAND_FAILED is returned if the command failed to*/
/* complete.                                                    */
/* ACU_NO_ERROR is returned if the command completed successfuly*/
/****************************************************************/

#ifndef RES_MAN_RMSM_H
#define RES_MAN_RMSM_H

#include "acu_type.h"
#include "res_lib.h"
#include "smdrvr.h"

#ifdef __cplusplus
extern "C"{
#endif


typedef struct timeslot
{
	int tdm_in_st;  /* in stream */
	int tdm_in_ts;  /* in timeslot */
	int tdm_out_st; /* out stream */
	int tdm_out_ts; /* out timeslot */
} Timeslot ;

/****************************************************************/
/* rmsm_open_resource.											*/
/* Required to be called for each board before the resource		*/
/* manager can be used for that board.							*/
/*																*/
/* returns ACU_NO_ERROR or ACU_ERROR_COMMAND_FAILED				*/
/****************************************************************/
ACU_ERR ACU_EXPORT  rmsm_open_resource( ACU_CARD_ID card_id,
					               const char * passed_ipaddress);

/****************************************************************/
/* rmsm_close_resource.											*/
/* Called when finished using the resource manager				*/
/* with a board.												*/
/*																*/
/* returns ACU_NO_ERROR or ACU_ERROR_COMMAND_FAILED				*/
/****************************************************************/
ACU_ERR ACU_EXPORT  rmsm_close_resource( ACU_CARD_ID card_id);

/****************************************************************/
/* rmsm_dnld_module												*/
/* Causes the resource manager to be loaded with the resources  */  
/* available to a DSP on a board from the file passed           */
/*																*/
/*  res=rmsm_dnld_module(1,1 , "c:/prosodyx/resourcefile1.txt");*/
/*																*/
/*																*/
/* returns ACU_NO_ERROR, ACU_ERROR_COMMAND_FAILED or            */
/* ERROR_NOT_FOUND				                                */
/****************************************************************/

ACU_ERR ACU_EXPORT  rmsm_dnld_module(ACU_CARD_ID card_id,
							tSM_UT32 module_ix ,const char * filename);

/****************************************************************/
/* rmsm_reset_module											*/
/* Causes the resource manager to be reloaded with the resources*/  
/* available to a DSP on a board from the values previously		*/
/* passed using rmsm_dnld_module()								*/
/****************************************************************/
ACU_ERR ACU_EXPORT  rmsm_reset_module(ACU_CARD_ID card_id,tSM_UT32 module_ix);

/****************************************************************/
/* rmsm_initialise												*/
/* Causes the resource manager to be loaded with the resources  */  
/* available to a DSP on a board from string passed             */
/*  i.e.  res = rmsm_initialise(  2, "fred 2 jim 1 ");        */
/*																*/
/* returns ACU_NO_ERROR or ACU_ERROR_COMMAND_FAILED				*/
/****************************************************************/
ACU_ERR ACU_EXPORT  rmsm_initialise(ACU_CARD_ID card_id, 
					tSM_UT32 module_ix,const char * resource_string);

/****************************************************************/
/* rmsm_allocate                                                */
/* Asks the resource manager to allocate a resource. Where      */
/* card_id  is the handle returned by acu_open_card and the     */
/* module_ix is the module number and call type is the name of  */
/* the resource. Same_timeslots should be set to true or false, */ 
/* this controls whether two disimilar timesslots will be       */
/* returned if two identical one are not available.             */
/*   I.E.                                                       */  
/*     handle =  rmsm_allocate(1,1 ,true, "Fred", &ts);         */ 
/*                                                              */
/* returns a handle to the allocated resource or                */
/* ACU_ERROR_COMMAND_FAILED or ACU_ERROR_NO_TRM	and a structure */
/* holding any timeslots allocated                              */
/*                                                              */
/* If module_ix is set to ACU_ANY_DSP resource will be allocated*/
/* on any dsp that has sufficient resources. The dsp number will*/
/* be returned in module_ix.                                    */
/****************************************************************/
ACU_INT ACU_EXPORT rmsm_allocate(ACU_CARD_ID card_id, tSM_UT32 * module_ix ,
  int same_timeslots,const char * calltype, struct timeslot *ts);

/****************************************************************/
/* rmsm_deallocate												*/
/* Asks the resource manager to deallocate a resource.	Where   */
/* card_id  is the handle returned by acu_open_card and the     */
/* handle is the value returned by rmsm_allocate				*/
/*    I.E.														*/  
/*    res =  rmsm_deallocate(card_id, handle);                  */
/*																*/
/* returns ACU_NO_ERROR or ACU_ERROR_COMMAND_FAILED				*/
/****************************************************************/
ACU_ERR ACU_EXPORT  rmsm_deallocate(ACU_CARD_ID card_id,int handle );


/****************************************************************/
/* finds a string in a TRM file and returns the rest of the		*/
/* line. Used to parse the TRM file for resources etc			*/
/****************************************************************/

ACU_ERR ACU_EXPORT  rmsm_find_in_file(const char *  filename,
			const char * string_to_find, char * string_to_return,
            unsigned int length_of_return_string);


/****************************************************************/
/* rmsm_total_resources											*/
/* Returns a sring with the total resources available to a DSP. */
/* If the module ID is set to -1 the resources for all DSPs on  */
/* the card are returned.										*/  
/****************************************************************/
ACU_ERR ACU_EXPORT  rmsm_total_resources(ACU_CARD_ID card_id, 
					tSM_UT32 module_ix , char * return_string,
                    unsigned int length_of_return_string);


/****************************************************************/
/* rmsm_available_resources										*/
/* Returns a string with the current resources available to a	*/
/* DSP. If the module ID is set to -1 the resources for all DSPs*/
/* on the card are returned.									*/  
/****************************************************************/
ACU_ERR ACU_EXPORT rmsm_available_resources(ACU_CARD_ID card_id, 
                       tSM_UT32 module_ix , char * return_string,
                       unsigned int length_of_return_string);

/****************************************************************/
/* ACT management functions                                     */
/****************************************************************/
ACU_ERR ACU_EXPORT rmsm_get_app_context_token(ACU_APP_CONTEXT_TOKEN_PARMS* token_parms);
ACU_ERR ACU_EXPORT rmsm_set_app_context_token(ACU_APP_CONTEXT_TOKEN_PARMS* token_parms);

/****************************************************************/
/* V6 event queue management functions                          */
/****************************************************************/
ACU_ERR ACU_EXPORT rmsm_notification_get_wait_object(ACU_CARD_ID card_id, ACU_WAIT_OBJECT *wait_object);
ACU_ERR ACU_EXPORT rmsm_set_notification_queue(ACU_QUEUE_PARMS* queue_parms);
ACU_ERR ACU_EXPORT rmsm_get_notification(ACU_CARD_ID card_id, ACU_INT *event);

/****************************************************************/
/* Visible version for config summaries                         */
/****************************************************************/
char* ACU_EXPORT  rmsm_lib_version(void);


#ifdef __cplusplus
}
#endif

/* Event types reported. */
#define RMSM_EVT_NO_EVENT                 0
#define RMSM_EVT_TOTAL_RESOURCES_CHANGED  1
#define RMSM_EVT_NO_TRM                   2

#define ACU_ANY_DSP 0xFFFFFFFF

/* Compatibility */
#define ACU_NO_ERROR ERR_NO_ERROR
#define ACU_ERROR_NOT_FOUND ERR_INVALID_RESOURCE
#define ACU_ERROR_NO_SPACE ERR_NO_MEMORY
#define ACU_ERROR_ALREADY_DEFINED ERR_ALREADY_EXISTS
#define ACU_ERROR_COMMAND_FAILED ERR_FAIL
#define ACU_ERROR_NO_TRM ERR_SERVER_NOT_RESPONDING


#endif /* if RES_MAN_RMSM_H */


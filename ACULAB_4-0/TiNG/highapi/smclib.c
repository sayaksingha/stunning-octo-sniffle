/*------------------------------------------------------------*/
/* ACULAB plc                                                 */
/*------------------------------------------------------------*/
/*                                                            */
/*                                                            */
/* Program File Name : smclib.c                               */
/*                                                            */
/*           Purpose : Conferencing High Level Library        */
/*                     provides simple, N party to N party    */
/*                     conferencing.                          */
/*                                                            */
/*            Author : Jonathan Pound                         */
/*                                                            */
/*       Create Date : 26th Febraray 1997                     */
/*                                                            */
/*                                                            */
/*                                                            */
/*                                                            */
/*                                                            */
/*------------------------------------------------------------*/

#include "smclib.h"
#include "../apilib/trace.h"
#include "../apilib/once.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __STDC__
#define __NEWC__
#endif


#ifndef ACUAPI
#define ACUAPI
#endif

/* 
 * This is a single party in a conference.
 */
typedef struct sm_conference_party_struct {
    tSMChannelId    channelIn;          
    tSMChannelId    channelOut;        
    tSM_INT         id;
} SM_CONFERENCE_PARTY_STRUCT;


/* 
 * This is a conference.
 */
struct sm_conference {
    pthread_mutex_t mx;
    unsigned int Nmembers;
    SM_CONFERENCE_PARTY_STRUCT *parties;
	unsigned int max_parties;
};

ACUAPI char *smConfError;
ACUAPI int smConfErrno;

static int conf_lock(tSMConference conf)
{
 int e = pthread_mutex_lock(&conf->mx);
 if (e) {
	smConfErrno = e;
	smConfError = "pthread_mutex_lock failed";
 }
 return e;
}

static int conf_unlock(tSMConference conf)
{
 int e = pthread_mutex_unlock(&conf->mx);
 if (e) {
	smConfErrno = e;
	smConfError = "pthread_mutex_unlock failed";
 }
 return e;
}


/*
 * A useful intermediate function.
 */
static int sm_add( tSMChannelId Out, tSMChannelId In )
{
    int result;
    SM_CONF_PRIM_ADD_PARMS  parms;

    parms.channel       = Out;
    parms.participant   = In;
    
    result = sm_conf_prim_add ( &parms );
    
    if ( result != 0 ) {
        smConfError = "sm_conf_prim_add()";
        return result;
    }

    return parms.id;
}



/*
 * The actions performed by the external calls.
 */
static int unsafe_sm_conference_add_party(SM_CONFERENCE_ADD_PARTY_PARMS *add_params)
{
    tSMConference conf = add_params->conf;
    int                          result;
    unsigned int                          party;

    for (party=0; party<conf->Nmembers; party++) {
    	if (conf->parties[party].channelIn == add_params->channelIn) {
	    smConfError = "party already in this conference";
	    return err(ERR_SM_BAD_PARAMETER);
    	}
    }
    if (conf->Nmembers >= conf->max_parties) {
		SM_CONFERENCE_PARTY_STRUCT *newp = realloc(conf->parties,sizeof(SM_CONFERENCE_PARTY_STRUCT)*conf->max_parties*2);
		if (!newp) {
	        smConfError = "Failure to reallocate conference parties structure";
		    return err(ERR_SM_OS_RESOURCE_PROBLEM);
		}
		conf->parties = newp;
		conf->max_parties *= 2;
    }

    conf->parties[conf->Nmembers].channelOut = add_params->channelOut;
    conf->parties[conf->Nmembers].channelIn  = add_params->channelIn;
    	// now prepare the input
    { // do the attach first. if we're out of conf ids we won't start tasks that can't be stopped (except by releasing the channel)
	SM_CONF_PRIM_ATTACH_PARMS ap;
	memset(&ap, 0, sizeof(ap));
	ap.channel = add_params->channelIn;
	ap.conf_type = kSMConfTypeStandard;
	result = sm_conf_prim_attach(&ap);
	if ( result != 0 ) {
	    smConfError = "sm_conf_prim_attach()";
	    return result;
	}
    }

		// prepare the output
    {
	SM_CONF_PRIM_CLONE_PARMS    parms;
	parms.channel = add_params->channelOut;
	if (!conf->Nmembers) {
		parms.model = 0;
	} else {
		parms.model = conf->parties[0].channelOut;
	}	
	result = sm_conf_prim_clone( &parms );
	if ( result != 0 ) {
	    smConfError = "sm_conf_prim_clone()";
	    return result;
	}
	if (conf->Nmembers) {		/* not first - add in channel that was cloned */
            result = sm_add(add_params->channelOut, conf->parties[0].channelIn); 
            if (result < 0) return result;
            conf->parties[0].id = result; 
        }    
    }
		// and adjust the output
    {
	SM_CONF_PRIM_ADJ_OUTPUT_PARMS parms;
	parms.channel = add_params->channelOut;
	parms.volume  = add_params->out_volume;
	parms.agc     = add_params->out_agc;
	result = sm_conf_prim_adj_output( &parms );
	if ( result != 0 ) {
	    smConfError = "sm_conf_prim_adj_output()";
	    return result;
	}
    }
    		// and adjust the input
    {
	SM_CONF_PRIM_ADJ_INPUT_PARMS adjustInputParms;
	adjustInputParms.channel = add_params->channelIn;
	adjustInputParms.agc     = add_params->in_agc;
	adjustInputParms.volume  = add_params->in_volume;
	result = sm_conf_prim_adj_input ( &adjustInputParms );
	if (result != 0) {
	    smConfError = "sm_conf_prim_adj_input()";
	    return result;
	}
    }
		// set up the sidetone
    {
	SM_SET_SIDETONE_CHANNEL_PARMS sidetoneParms;
	sidetoneParms.channel   = add_params->channelIn;
	sidetoneParms.output    = add_params->channelOut;
	result = sm_set_sidetone_channel ( &sidetoneParms );
	if (result != 0) {
            smConfError = "sm_set_sidetone_channel()";
            return result;
	}
    }

	/* Add the new party to the other parties' low-level conferences. */
    for (party=0; party<conf->Nmembers; party++) {
        result = sm_add ( conf->parties[party].channelOut, add_params->channelIn ); 
        if (result < 0) return result;
    }
    if (conf->Nmembers) conf->parties[conf->Nmembers].id = result; 
    conf->Nmembers++;
    return 0;
}

static int unsafe_sm_conference_remove_party(tSMConference conf, tSMChannelId channelIn, tSMChannelId channelOut)
{
    int result;
    unsigned int party;
    unsigned int leavingParty;
    
	/* find the party to be removed */
    for (leavingParty=0; ; leavingParty++) {
        if (leavingParty >= conf->Nmembers) {
	        smConfError = "Party not found in this conference";
	        return err(ERR_SM_BAD_PARAMETER);
        }
        if (conf->parties[leavingParty].channelIn == channelIn) break;
    }
    
	    /* check we have the right one */
    if(conf->parties[leavingParty].channelOut != channelOut){
        smConfError = "channelOut is not related to channelIn";
        return err(ERR_SM_BAD_PARAMETER);
    }
    
	    /* We now need to do the opposite of joining: delete the
	     * leaving party from the low-level conferences belonging to
	     * each of the remaining parties, and delete all parties from
	     * the low-level conference belonging to the leaving party.
	     * This second action is automatically performed when we abort
	     * the low-level conference, so we just have to do the first
	     * of these.
		 */
    for (party = 0; party < conf->Nmembers; party++) {
        if (party != leavingParty) {
	    SM_CONF_PRIM_LEAVE_PARMS parms;
	    parms.channel    = conf->parties[party].channelOut;
	    parms.id         = conf->parties[leavingParty].id;
	    result = sm_conf_prim_leave( &parms );
	    if ( result != 0 ) {
		smConfError = "sm_conf_prim_leave()";
		return result;
	    }
        }
    }
 
    {
	SM_CONF_PRIM_DETACH_PARMS dp;
	memset(&dp, 0, sizeof(dp));
	dp.channel = conf->parties[leavingParty].channelIn;
	result = sm_conf_prim_detach(&dp);
	if ( result != 0 ) {
	    smConfError = "sm_conf_prim_detach()";
	    return result;
	}
	}
    sm_conf_prim_abort(conf->parties[leavingParty].channelOut);

    conf->Nmembers--;

	    /* move up end party to fill gap */
    conf->parties[leavingParty] = conf->parties[conf->Nmembers];

    return 0;
}



/* The external entry points themselves. While these are thread-safe,
 * the actions defined above are not.
 */

/* ---------------------------------------------------------------------
 * sm_conference_create:
 * ---------------------------------------------------------------------
 * input : none
 * output: none
 * return: new conference
 * ------------------------------------------------------------------ */
STATIC tSMConference real_sm_conference_create(void)
{
    tSMConference conf;
    int e;
    conf = (tSMConference)malloc(sizeof(*conf));
    if (!conf) {
        smConfError = "Failure to allocate conference structure";
        return 0;
    }
    conf->Nmembers = 0;
	conf->max_parties = 2;
	conf->parties = malloc(sizeof(SM_CONFERENCE_PARTY_STRUCT)*conf->max_parties);
	if (!conf->parties) {
		free(conf);
        smConfError = "Failure to allocate conference parties structure";
        return 0;
	}
    e = pthread_mutex_init(&conf->mx, 0);
    if (e) {
        smConfErrno = e;
        smConfError = "pthread_mutex_init failed";
        free(conf);
        return 0;
    }
    return conf;
}

/* ---------------------------------------------------------------------
 * sm_conference_add_party : Add a new conference party
 * ---------------------------------------------------------------------
 * input : SM_CONFERENCE_PARTY_PARMS *add_params
 * output: none
 * return: -ve value is ERR_SM_
 * ------------------------------------------------------------------ */
STATIC int real_sm_conference_add_party(SM_CONFERENCE_ADD_PARTY_PARMS *add_params)
{
    int result;
    if (conf_lock(add_params->conf)) return err(ERR_SM_OS_OTHER_PROBLEM);
    result = unsafe_sm_conference_add_party(add_params);
    if (conf_unlock(add_params->conf)) return err(ERR_SM_OS_OTHER_PROBLEM);
    return result;
}

/* ---------------------------------------------------------------------
 * sm_conference_remove_party : Remove a party from an existing conference
 * ---------------------------------------------------------------------
 * input : SM_CONFERENCE_PARTY_PARMS* remove_params
 * output: none
 * return: -ve value is ERR_SM_
 * ------------------------------------------------------------------ */
STATIC int real_sm_conference_remove_party( SM_CONFERENCE_REMOVE_PARTY_PARMS* remove_params)
{
    int result;
    if (conf_lock(remove_params->conf)) return err(ERR_SM_OS_OTHER_PROBLEM);
    result = unsafe_sm_conference_remove_party(
		remove_params->conf,
		remove_params->channelIn,
		remove_params->channelOut);
    if (conf_unlock(remove_params->conf)) return err(ERR_SM_OS_OTHER_PROBLEM);
    return result;
}

STATIC int real_sm_conference_info(struct sm_conference_info_parms *info_parms)
{
 const tSMConference conf = info_parms->conf;
 tSMChannelId *lp = info_parms->activeChannelList;
 int rc = 0;
 if (conf_lock(conf)) return err(ERR_SM_OS_OTHER_PROBLEM);
 if (conf->Nmembers >= 2) {
	SM_CONF_PRIM_INFO_PARMS	info;
	info.channel = conf->parties[1].channelOut;
	rc = sm_conf_prim_info(&info);
	if (rc) {
	        smConfError = "sm_conf_prim_info()";
	} else {
		int byte, bit;
		unsigned int i;
			// special case: party[0] will never show up
			// in the info fetched from party[0], so we handle
			// it first by asking party[1]
		bit = conf->parties[0].id;
		byte = bit >> 3;
		bit &= 7;
		if (info.speakers[byte] & (1<<bit))
			*lp++ = conf->parties[0].channelIn;
		info.channel = conf->parties[1].channelOut;
		rc = sm_conf_prim_info(&info);
		if (rc) {
			smConfError = "sm_conf_prim_info()";
		} else {
			for (i=1; i < conf->Nmembers; i++) {
				bit = conf->parties[i].id;
				byte = bit >> 3;
				bit &= 7;
				if (info.speakers[byte] & (1<<bit))
					*lp++ = conf->parties[i].channelIn;
			}
		}
	}
 }
 if (conf_unlock(conf)) return err(ERR_SM_OS_OTHER_PROBLEM);
 info_parms->activeChannelCount = lp - info_parms->activeChannelList;
 return rc;
}

/* ---------------------------------------------------------------------
 * sm_conference_delete:
 * ---------------------------------------------------------------------
 * input : conference to be deleted
 * output: none
 * return: none
 * ------------------------------------------------------------------ */
STATIC void real_sm_conference_delete(tSMConference conf)
{
 unsigned int i;
 conf_lock(conf);
 conf_unlock(conf);
 for (i=0; i<conf->Nmembers; i++) {
    sm_conf_prim_abort(conf->parties[i].channelOut);
 }  
 // now none of the inputs are in use, detach them
 for (i=0; i<conf->Nmembers; i++) {
	SM_CONF_PRIM_DETACH_PARMS dp;
	memset(&dp, 0, sizeof(dp));
	dp.channel = conf->parties[i].channelIn;
	sm_conf_prim_detach(&dp);
 }
 pthread_mutex_destroy(&conf->mx);
 free(conf->parties);
 free(conf);
}

#include "../apilib/timestamp.h"
#include "../pubdoc/gen/highapi_conf.i"

/* TiNGiwr.h - Isolated Word Recognition */

#ifndef INCLUDED_TINGIWR_H
#define INCLUDED_TINGIWR_H

#include "smbesp.h"

LOCALDEC int real_sm_reset_input_vocabs(struct sm_reset_input_vocabs_parms *vocabp);
LOCALDEC int real_sm_add_input_vocab(struct sm_input_vocab_parms *vocabp);
LOCALDEC int real_sm_asr_listen_for(struct sm_asr_listen_for_parms *pp);

#endif

#ifndef V6HINCLUDED
#define V6HINCLUDED



#define TEST_CALL_TYPE ACU_CALL_HANDLE
#define TEST_CARD_TYPE ACU_CARD_ID
#define TEST_PORT_TYPE ACU_PORT_ID
// the v6 aculab types.
#include "acu_type.h"
#include "res_lib.h"
#include "cl_lib.h"
#include "sw_lib.h"
	// Oops! acu_type accidentally got tested with a definition of
	// TiNG_USE_DLL in it. Undefine it here, which lets everything
	// build ok for now, and fix acu_type.h to delete its fixed
	// definition of TiNG_USE_DLL when it's next released
#undef TiNG_USE_DLL

typedef struct v6resources
{
	ACU_CARD_ID		card;
	ACU_PORT_ID		port;
	int				opened_card;
	int				opened_port;
	int				opened_call;
	int				opened_switch;
}V6RESOURCES;

#endif

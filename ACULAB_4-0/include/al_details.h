#ifndef ACU_LICENCE_DETAILS_API_H
#define ACU_LICENCE_DETAILS_API_H

// errors returned by this lib
#define AL_ERR_WINSOCK_VER			-5000 // Could not find version 2.2 of WinSock DLL
#define AL_ERR_WINSOCK_USE			-5001 // Could not find a usable WinSock DLL
#define AL_ERR_SOCKET				-5002 // Failed to create a socket
#define AL_ERR_CONNECT				-5003 // Socket connect failed
#define AL_ERR_COMMS				-5004 // Communication with the product failed
#define AL_ERR_LICENCEMANAGER		-5005 // Licence manager returned an error
#define AL_ERR_BAD_PARAMETER		-5006 // Invalid input parameter(s)
#define AL_ERR_NOT_INITIALISED		-5007 // al_init() is not called or failed
#define AL_ERR_CONNECT_FAILED       -5008 // The LM failed to contact the server

#define UNITS_STR_MAX_LENGTH		16
#define LICENCE_ARRAY_SIZE			50
#define MAX_PRODUCT_NAME_LENGTH		64
#define MAX_FEATURE_DESCRIPTION		64
#define MAX_FEATURE_NAME			16
#define MAX_SUPPORTED_FEATURES		100
#define MAX_IP_ADDRESS_LENGTH		16

#define ADMIN_INSTALLED				1

//value retired as activation of subscriptions will no longer be neccessary
//#define SUBSCRIPTION_NOT_VALIDATED	2
#define SUBSCRIPTION_VALIDATED		4
#define SUBSCRIPTION_EXPIRED		8
//value retired as activation of subscriptions will no longer be neccessary
//#define VALIDATION_EXPIRED			16

typedef struct al_time
{
	unsigned int Days;
	unsigned int Hours;
	unsigned int Minutes;
	unsigned int Seconds;

} AL_TIME;

typedef struct al_validation
{
	char val[256];

} AL_ACTIVATION;


typedef struct al_machine_id
{
	char mId[256];

} AL_MACHINE_ID;

//PN273 - Fail string when installing  anew licence
typedef struct al_version_string
{
	char vs[256];
} AL_VERSION_STRING;


//PN273 - Fail string when installing  anew licence
typedef struct al_fail_string
{
	char fS[256];
} AL_FAIL_STRING;

//PN273 - Fail string when installing  anew licence
typedef struct al_delete_string
{
	char dS[256];
} AL_DELETE_STRING;


//PN273 - Fail string when installing  anew licence
typedef struct al_quantity_string
{
	char quantity[10];
} AL_QUANTITY_STRING;


typedef struct al_licence_key
{
	char alk[256];

} AL_LICENCE_KEY;

//	V3 Licence details
typedef struct al_product_licence_details
{
	unsigned int SecondsLeft;
	unsigned int Licences;
	unsigned int Expires;
	unsigned int UnlimitedLicences;
	char Units[UNITS_STR_MAX_LENGTH];

} AL_PRODUCT_LICENCE_DETAILS;

typedef struct al_feature_licence_details
{
	unsigned int SecondsLeft;
	unsigned int Licences;
	unsigned int Expires;
	unsigned int UnlimitedLicences;
	char Units[UNITS_STR_MAX_LENGTH];

} AL_FEATURE_LICENCE_DETAILS;

typedef struct al_licence_information
{
	char ProductName[MAX_PRODUCT_NAME_LENGTH];
	unsigned int NumberOfProductLicences;
	unsigned int NumberOfSupporttedFeatures;
	unsigned int TotalProductLicencesAvailable;
	unsigned int TotalUsedProductLicence;
	unsigned int AdminPrivilages;
	unsigned int SupporttedFeatures[MAX_SUPPORTED_FEATURES];
	AL_PRODUCT_LICENCE_DETAILS ProductLicences[LICENCE_ARRAY_SIZE];

} AL_LICENCE_INFORMATION;

typedef struct al_feature_information
{
	unsigned int FeatureNumber;
	unsigned int NumberOfFeatureLicences;
	unsigned int TotalFeatureLicencesAvailable;
	unsigned int TotalUsedFeatureLicence;
	AL_FEATURE_LICENCE_DETAILS FeatureLicences[LICENCE_ARRAY_SIZE];

} AL_FEATURE_INFORMATION;

typedef struct al_feature_name_units
{
	unsigned int FeatureNumber;
	char	Name[MAX_FEATURE_NAME];
	char	Description[MAX_FEATURE_DESCRIPTION];
	char	Units[UNITS_STR_MAX_LENGTH];
} AL_FEATURE_NAME_UNITS;

typedef struct al_licence_product_details
{
	char IPAddress[MAX_IP_ADDRESS_LENGTH];
	unsigned int Port;
} AL_LICENCE_PRODUCT_DETAILS;

typedef struct al_metrics_string
{
	char mc[256];
} AL_METRICS_STRING;





#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ALM_LIB_API
#ifdef _WIN32
#define ALM_LIB_API __declspec(dllimport)
#else
#define ALM_LIB_API __attribute__((visibility("default")))
#endif
#endif

ALM_LIB_API int al_init( AL_LICENCE_PRODUCT_DETAILS* pLicenceProductDetails );	// initialise sockets and library (IPv4 only)
ALM_LIB_API int al_init_host(const char *host, unsigned int port);				// initialise sockets and library
ALM_LIB_API int check_init (void);												// Check to see if init has been called
ALM_LIB_API int al_stop(void);													// clean up the library

ALM_LIB_API int al_get_details( AL_LICENCE_INFORMATION* pLicenceInfo );			// licence information of the product
ALM_LIB_API int al_get_feat_details( AL_FEATURE_INFORMATION* pFeatureInfo );	// licence information of the features
ALM_LIB_API int al_get_name_units( AL_FEATURE_NAME_UNITS* pFeatureNameUnits );	// supported features of the product

ALM_LIB_API int al_admin( const char* ak );										// admin key installation
ALM_LIB_API int al_sub_clear (void);
ALM_LIB_API int al_sub_activate (AL_ACTIVATION *val);
ALM_LIB_API int al_sub_activate_offline (AL_ACTIVATION *code, AL_ACTIVATION *response);
ALM_LIB_API int al_sub_renew (AL_ACTIVATION *response);

// for getting machine ID and installing licence key
ALM_LIB_API int al_prep_for_new_key( AL_MACHINE_ID* mId );						// return machine ID to install a product key
ALM_LIB_API int al_prep_for_new_feature( AL_MACHINE_ID* mId );					// return machine ID to install a feature key
ALM_LIB_API int al_install_new_key( AL_LICENCE_KEY* lk );						// install new key

ALM_LIB_API AL_TIME al_seconds_to_time( unsigned int Seconds );					// converts seconds to AL_TIME sturcture

//Functions to support server working
ALM_LIB_API int al_Check_server(void);													// check for a server connection
ALM_LIB_API int al_set_server( const char *ip_address,  unsigned int);				    //Aculab Testing Purposes Only
ALM_LIB_API int al_install_new_licence( char *licence, AL_FAIL_STRING *failString  );	//function to install a licence that gets converted to a key by the server at install time
ALM_LIB_API int al_renew_licences( AL_FAIL_STRING *failString );						//function to renew licences
ALM_LIB_API int al_renew_licences_quantity( AL_QUANTITY_STRING* quant );				//function to find out how many renewable licences are in their "renewal" period

//Function to get the installed Licence Keys
//mode 0 - Create List and return the first licence in the list. 
//mode 1 - Return the next licence in the list. Will be empty when complete
//Make initial call with mode 0 and then subsequent calls with mode 1 to get all licences
ALM_LIB_API int al_get_licence_key( AL_LICENCE_KEY* lk , unsigned int mode );


//API will delete a licence and if the licence manager is "offline" it will 
//return a delete string that can be passed to the server via the offline tool
//to allow the licence to be installed elsewhere
//if online then the function will return the reference licence
ALM_LIB_API int al_delete_licence_key( AL_LICENCE_KEY* lk , AL_DELETE_STRING* ds );
ALM_LIB_API int al_delete_licence_key_on_server(AL_DELETE_STRING* ds );
ALM_LIB_API int al_get_lm_version( AL_VERSION_STRING* vs );

//Call to check to the result from the licence managers virtual manager code
ALM_LIB_API int al_get_is_vm (int* rs);					

//Not Documented or supported
ALM_LIB_API int al_get_lm_stats( AL_METRICS_STRING* st );


#ifdef __cplusplus
}
#endif

#endif // ACU_LICENCE_DETAILS_API_H

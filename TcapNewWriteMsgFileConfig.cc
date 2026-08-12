#include <iostream>
#include <fstream>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_FILE_NAME         200
#define MAX_KEY_CONF_LEN      2000
#define MAX_KEY_LEN           25
#define MAX_NO_OF_ARG         65
#define MAX_PDU_SIZE          1000
#define MAX_NORAML_ARG_LEN    50
#define PARAMETRE_DELIM       "!"
#define PDU_END               "END"
#define PARAM_START_INDICATOR ":"
#define DATA_START_INDICATOR  "--"
#define PROMPT_FOR_PARAM      "ASK"
#define PARAM_NOT_PRESENT     "ZERO"
#define PARAM_SKIP            "SKIP"

enum KeyWordIndex
{
   KEY_IDX_UNKOWN = 0,
   QKEY = 1,
   O_AI = 2,
   O_SSN = 3,
   O_PT = 4,
   O_NOA = 5,
   O_TT = 6,
   O_NP = 7,
   O_GT = 8,
   D_AI = 9,
   D_SSN = 10,
   D_PT = 11,
   D_NOA = 12,
   D_TT = 13,
   D_NP = 14,
   D_GT = 15,
   ISACN = 16,
   ISUSERINFO =17,
   DLG_TYPE = 18,
   COMP_PRESENT = 19,
   INVOKEID = 20,
   COMPONENT_TYPE = 21,
   OPCODE = 22, // ERR_TYPE, PROBLEM_TYPE,  ABORT_TYPE
   LAST_INDICATOR = 23, // ERR_CODE, PROBLEM_CODE
   REJECT_TYPE = 24,
   DLGID = 25,
   HDLR_SSN = 26,
   MSG_TYPE = 27,
   O_TRANSID = 28,
   TCUSERID = 29,
   USERINFO = 30,
   APPL_CONTEXT = 31,
   ACN_VER = 32,
   PARAM_DATA = 33,
   ERR_TYPE = 34,
   PROBLEM_TYPE = 35,
   ABORT_TYPE = 36,
   ERR_CODE = 37,
   PROBLEM_CODE = 38,
   LOAD = 39,
   DLG_MAX   = 40,
   NO_OF_PDU = 41,
   PDU_0     = 42,
   PDU_1     = 43,
   PDU_2     = 44,
   PDU_3     = 45,
   PDU_4     = 46,
   PDU_5     = 47,
   PDU_6     = 48,
   PDU_7     = 49,
   PDU_8     = 50,
   PDU_9     = 51
};

int           GetConfigString(char*);
int           ProcessConfigString(char*);
KeyWordIndex  GetKeyWordIndex(char*);
int           InitKeyWord();

ostream& Width(ostream& );
ostream& Fill(ostream & );

ostream& Width(ostream& print)
{
   print.width(50);
   return print;
}

ostream& Fill(ostream & print)
{
   print.fill(' ');
   return print;
}

char gConStr         [MAX_KEY_CONF_LEN + 1] = "";
char *gArgPtr        [MAX_NO_OF_ARG];
char gArgList        [MAX_NO_OF_ARG][MAX_PDU_SIZE +1];
char gArgDiscription [MAX_NO_OF_ARG][MAX_NORAML_ARG_LEN +1];
char gFileName       [MAX_FILE_NAME +1] = "";
char gKeyWord        [MAX_KEY_LEN +1] = "";

int main(int argc, char* argv[])
{
   if(3 != argc)
   {
      cout << "Usage: <ProcessName> <ConfigFile> <Tcap Exe File>" << endl;
      return 0;
   }

   char lProductHome    [MAX_FILE_NAME +1] = "";
   char lConfigPath     [MAX_FILE_NAME +1] = "";
   char lExeName        [MAX_FILE_NAME +1] = "";
   char * lptr = NULL;
   
   lptr = getenv("PRODUCT_HOME");
   if(NULL == lptr)
   {
      cout << "Error Getting Env PRODUCT_HOME.. Exiting" << endl;
      return -1;
   }
   strcpy(lProductHome, lptr);
   
   lptr = getenv("PRODUCT_CFG_PATH");
   if(NULL == lptr)
   {
      cout << "Error Getting Env PRODUCT_CFG_PATH.. Exiting" << endl;
      return -1;
   }
   strcpy(lConfigPath, lptr);
   
   cout << Width << Fill << right << "PRODUCT_HOME:" << lProductHome << endl;
   cout << Width << Fill << right << "PRODUCT_CFG_PATH:" << lConfigPath << endl;
   cout << "PRODUCT_CFG_PATH:" << lConfigPath << endl;
   snprintf(gFileName, MAX_FILE_NAME, "%s/%s/%s", lProductHome, lConfigPath, argv[1]);
   cout << Width << Fill << right << "Looking For Configuration File:" << gFileName << endl;

   snprintf(lExeName, MAX_FILE_NAME, "%s/bin/stubs/%s", lProductHome, argv[2]);
   cout << Width << Fill << right << "Looking For Executable File:" << lExeName << endl;

   int lRetVal = 0;

   // Reset All Global Pointer;
   for(int i=0; i < MAX_NO_OF_ARG; i++)
   {
      gArgPtr[i] = (char*) 0;
      gArgPtr[i] = gArgList[i];
   }

   signal(SIGCHLD, SIG_IGN);

   while(1)
   {
      cout << Width << Fill << right << "Looking For Configuration File:" << gFileName << endl;
      cout << Width << Fill << right << "Looking For Executable File:" << lExeName << endl;
      memset(gKeyWord, '\0', sizeof(gKeyWord));
      cout << "\t\t" <<"----------------------------------\n";
      cout << "\t\t" <<"|*** Enter The Config KeyWord *** |\n";
      cout << "\t\t" <<"----------------------------------\n";
      cout << endl;

      cin >> gKeyWord;

      if(-1 == GetConfigString(gKeyWord))
         continue;

      if(-1 == ProcessConfigString(gKeyWord))
         continue;


      if(0 == (lRetVal = fork()))
      {
         // excec inside the child
	 // ANSI 60 ar
         cout << "Success Creating Child.. Calling Exec" << endl;
         if(-1 == execlp(lExeName, gArgPtr[0], gArgPtr[1],gArgPtr[2],gArgPtr[3],gArgPtr[4],gArgPtr[5],gArgPtr[6],gArgPtr[7],gArgPtr[8],gArgPtr[9],gArgPtr[10],gArgPtr[11],gArgPtr[12],gArgPtr[13],gArgPtr[14],gArgPtr[15],gArgPtr[16],gArgPtr[17],gArgPtr[18],gArgPtr[19],gArgPtr[20],gArgPtr[21],gArgPtr[22],gArgPtr[23],gArgPtr[24],gArgPtr[25],gArgPtr[26],gArgPtr[27],gArgPtr[28],gArgPtr[29],gArgPtr[30],gArgPtr[31],gArgPtr[32],gArgPtr[33],gArgPtr[34],gArgPtr[35],gArgPtr[36],gArgPtr[37],gArgPtr[38],gArgPtr[39],gArgPtr[40],gArgPtr[41],gArgPtr[42],gArgPtr[43],gArgPtr[44],gArgPtr[45],gArgPtr[46],gArgPtr[47],gArgPtr[48],gArgPtr[49],gArgPtr[50],gArgPtr[51],gArgPtr[52],gArgPtr[53],gArgPtr[54],gArgPtr[55],gArgPtr[56],gArgPtr[57],gArgPtr[58],(char*)0,12 ))
         {
            cout <<"exec Failed ErrNo:" << errno << strerror(errno)<< endl;
            exit(2);
         }
         else       //This will never come...
         {
            cout << "Exec success child exiting...\n" << endl;
            exit(0);
         }

         exit(-1);
      }
      else if(-1 == lRetVal)
      {
         cout << "Cann't fork... System Error..." << endl;
      }
      else
      {
         // Parent
         continue;
      }
   }
   return 0;
}

//-----------------------------------------------------
int GetConfigString(char* pConStr)
{
   ifstream lFileObj(gFileName);
   int lLen = strlen(pConStr);
   while(!lFileObj.eof())
   {
      memset(gConStr, '\0', sizeof(gConStr));
      lFileObj.getline(gConStr, MAX_KEY_CONF_LEN);
      if('\0' == gConStr[0] && lFileObj.eof())
      {
         cout << "End of File Reached....Keyword Not Found" << endl;
         lFileObj.close();
         return -1;
      }

      if(0 == strncasecmp(gConStr, pConStr, lLen))
      {
         cout << pConStr <<":Found in the File.." << endl;
         return 0;
      }
   }

   lFileObj.close();
   return 0;
}
//-----------------------------------------------------


//-----------------------------------------------------
int ProcessConfigString(char* pKeyword)
{
   char* pDataPtr = gConStr;
   pDataPtr += strlen(pKeyword);
   pDataPtr += strlen(DATA_START_INDICATOR);

   KeyWordIndex  lKeyIndx = KEY_IDX_UNKOWN;

   for(int i=0; i < MAX_NO_OF_ARG; i++)
   {
      memset(gArgDiscription[i], '\0', sizeof(gArgDiscription[i]));
      memset(gArgList[i], '\0', sizeof(gArgList[i]));
      strcpy(gArgList[i],PARAM_NOT_PRESENT);
   }

   InitKeyWord();

   char* lptr = strtok(pDataPtr, PARAM_START_INDICATOR);
   if(NULL == lptr)
   {
      cout << "!!!! Wrong Configuration.. Please Check..." << endl;
      return -1;
   }
   lKeyIndx = GetKeyWordIndex(lptr);
   strcpy(gArgDiscription[lKeyIndx], lptr);

   lptr = strtok(NULL, PARAMETRE_DELIM);
   if(NULL == lptr)
   {
      cout << "!!!! Wrong Configuration.. Please Check..." << endl;
      return -1;
   }
   strcpy(gArgList[lKeyIndx], lptr);

   while(lptr != NULL)
   {
      lptr = strtok(NULL, PARAM_START_INDICATOR);
      if(NULL == lptr)
      {
         cout << "!!!! PARAM_START_INDICATOR Wrong Configuration.. Please Check..."
            << endl;
         return -1;
      }

      //*****
      lKeyIndx = GetKeyWordIndex(lptr);
      strcpy(gArgDiscription[lKeyIndx], lptr);
      if(0 == strncasecmp(gArgDiscription[lKeyIndx],PDU_END, strlen(PDU_END)))
      {
         cout <<"-------------------------------------------------" << endl;
         cout << "Values Toknised for:"<<pKeyword << endl;
         cout <<"-------------------------------------------------" << endl;
         for(int i =0; i < MAX_NO_OF_ARG; i++ )
         {
            //if('\0' != gArgDiscription[i][0] && '\0' != gArgList[0])
            cout << right << Width << Fill << i << "--->"
               << gArgDiscription[i] << ":" << left
               << Width << Fill << gArgList[i] << endl;
         }
         cout << endl << endl;

         return 0;
      }

      lptr = strtok(NULL, PARAMETRE_DELIM);
      if(NULL == lptr)
      {
         cout << "!!!! PARAMETRE_DELIM Wrong Configuration.. Please Check..." << endl;
         return -1;
      }
      strcpy(gArgList[lKeyIndx], lptr);

      if(0 ==
            strncasecmp(PROMPT_FOR_PARAM, gArgList[lKeyIndx], strlen(PROMPT_FOR_PARAM)))
      {
         // Ask the Param
         memset(gArgList[lKeyIndx], '\0', sizeof(gArgList[lKeyIndx]));
         cout << gKeyWord <<": Please Enter the " << gArgDiscription[lKeyIndx] << ":";
         fgetc(stdin);
         fgets(gArgList[lKeyIndx], MAX_NORAML_ARG_LEN, stdin);
      }
      else if
         (
          0 == strncasecmp(PARAM_SKIP, gArgList[lKeyIndx], strlen(PARAM_SKIP)) ||
          0 ==
          strncasecmp(PARAM_NOT_PRESENT,gArgList[lKeyIndx],strlen(PARAM_NOT_PRESENT))
         )
         {
            memset(gArgList[lKeyIndx], '\0', sizeof(gArgList[lKeyIndx]));
            //strcpy(gArgList[lKeyIndx],PARAM_NOT_PRESENT);
         }

         cout << "lKeyIndx:" << lKeyIndx << "====>" << gArgList[lKeyIndx] << endl;
   }

   return 0;
}

//------------------------------------------------------
KeyWordIndex  GetKeyWordIndex(char* pString)
{
   if(0 == strncasecmp("QKEY", pString, strlen("QKEY")))
      return QKEY;

   else if(0 == strncasecmp("O_AI", pString, strlen("O_AI")))
      return O_AI;

   else if(0 == strncasecmp("O_SSN", pString, strlen("O_SSN")))
      return O_SSN;

   else if(0 == strncasecmp("O_PT", pString, strlen("O_PT")))
      return O_PT;

   else if(0 == strncasecmp("O_NOA", pString, strlen("O_NOA")))
      return O_NOA;

   else if(0 == strncasecmp("O_TT", pString, strlen("O_TT")))
      return O_TT;

   else if(0 == strncasecmp("O_NP", pString, strlen("O_NP")))
      return O_NP;

   else if(0 == strncasecmp("O_GT", pString, strlen("O_GT")))
      return O_GT;

   else if(0 == strncasecmp("D_AI", pString, strlen("D_AI")))
      return D_AI;

   else if(0 == strncasecmp("D_SSN", pString, strlen("D_SSN")))
      return D_SSN;

   else if(0 == strncasecmp("D_PT", pString, strlen("D_PT")))
      return D_PT;

   else if(0 == strncasecmp("D_NOA", pString, strlen("D_NOA")))
      return D_NOA;

   else if(0 == strncasecmp("D_TT", pString, strlen("D_TT")))
      return D_TT;

   else if(0 == strncasecmp("D_NP", pString, strlen("D_NP")))
      return D_NP;

   else if(0 == strncasecmp("D_GT", pString, strlen("D_GT")))
      return D_GT;

   else if(0 == strncasecmp("ISACN", pString, strlen("ISACN")))
      return ISACN;

   else if(0 == strncasecmp("ISUSERINFO", pString, strlen("ISUSERINFO")))
      return ISUSERINFO;

   else if(0 == strncasecmp("DLG_TYPE", pString, strlen("DLG_TYPE")))
      return DLG_TYPE;

   else if(0 == strncasecmp("COMP_PRESENT", pString, strlen("COMP_PRESENT")))
      return COMP_PRESENT;

   else if(0 == strncasecmp("INVOKEID", pString, strlen("INVOKEID")))
      return INVOKEID;

   else if(0 == strncasecmp("COMPONENT_TYPE", pString, strlen("COMPONENT_TYPE")))
      return COMPONENT_TYPE;

   else if(0 == strncasecmp("OPCODE", pString, strlen("OPCODE")))
      return OPCODE;

   else if(0 == strncasecmp("ERR_TYPE", pString, strlen("ERR_TYPE")))
      return OPCODE;

   else if(0 == strncasecmp("PROBLEM_TYPE", pString, strlen("PROBLEM_TYPE")))
      return OPCODE;

   else if(0 == strncasecmp("ABORT_TYPE", pString, strlen("ABORT_TYPE")))
      return OPCODE;

   else if(0 == strncasecmp("LAST_INDICATOR", pString, strlen("LAST_INDICATOR")))
      return LAST_INDICATOR;

   else if(0 == strncasecmp("ERR_CODE", pString, strlen("ERR_CODE")))
      return LAST_INDICATOR;

   else if(0 == strncasecmp("REJECT_TYPE", pString,
            strlen("REJECT_TYPE")))
      return LAST_INDICATOR;

   else if(0 == strncasecmp("PROBLEM_CODE", pString,
            strlen("PROBLEM_CODE")))
      return LAST_INDICATOR;

   else if(0 == strncasecmp("DLGID", pString, strlen("DLGID")))
      return DLGID;

   else if(0 == strncasecmp("HDLR_SSN", pString, strlen("HDLR_SSN")))
      return HDLR_SSN;

   else if(0 == strncasecmp("MSG_TYPE", pString, strlen("MSG_TYPE")))
      return MSG_TYPE;

   else if(0 == strncasecmp("O_TRANSID", pString, strlen("O_TRANSID")))
      return O_TRANSID;

   else if(0 == strncasecmp("TCUSERID", pString, strlen("TCUSERID")))
      return TCUSERID;

   else if(0 == strncasecmp("USERINFO", pString, strlen("USERINFO")))
      return USERINFO;

   else if(0 == strncasecmp("APPL_CONTEXT", pString, strlen("APPL_CONTEXT")))
      return APPL_CONTEXT;

   else if(0 == strncasecmp("ACN_VER", pString, strlen("ACN_VER")))
      return ACN_VER;

   else if(0 == strncasecmp("PARAM_DATA", pString, strlen("PARAM_DATA")))
      return PARAM_DATA;

   else if(0 == strncasecmp("LOAD", pString, strlen("LOAD")))
      return LOAD;

   else if(0 == strncasecmp("DLG_MAX", pString, strlen("DLG_MAX")))
      return DLG_MAX;

   else if(0 == strncasecmp("NO_OF_PDU", pString, strlen("NO_OF_PDU")))
      return NO_OF_PDU;

   else if(0 == strncasecmp("PDU_0", pString, strlen("PDU_0")))
      return PDU_0;

   else if(0 == strncasecmp("PDU_1", pString, strlen("PDU_1")))
      return PDU_1;

   else if(0 == strncasecmp("PDU_2", pString, strlen("PDU_2")))
      return PDU_2;

   else if(0 == strncasecmp("PDU_3", pString, strlen("PDU_3")))
      return PDU_3;

   else if(0 == strncasecmp("PDU_4", pString, strlen("PDU_4")))
      return PDU_4;

   else if(0 == strncasecmp("PDU_5", pString, strlen("PDU_5")))
      return PDU_5;

   else if(0 == strncasecmp("PDU_6", pString, strlen("PDU_6")))
      return PDU_6;

   else if(0 == strncasecmp("PDU_7", pString, strlen("PDU_7")))
      return PDU_7;

   else if(0 == strncasecmp("PDU_8", pString, strlen("PDU_8")))
      return PDU_8;

   else if(0 == strncasecmp("PDU_9", pString, strlen("PDU_9")))
      return PDU_9;

   else
      return KEY_IDX_UNKOWN;
}

//------------------------------------------------------

int InitKeyWord()
{
   strcpy(gArgDiscription[QKEY], "QKEY");
   strcpy(gArgDiscription[O_AI], "O_AI");
   strcpy(gArgDiscription[O_SSN], "O_SSN");
   strcpy(gArgDiscription[O_PT], "O_PT");
   strcpy(gArgDiscription[O_NOA], "O_NOA");
   strcpy(gArgDiscription[O_TT], "O_TT");
   strcpy(gArgDiscription[O_NP], "O_NP");
   strcpy(gArgDiscription[O_GT], "O_GT");
   strcpy(gArgDiscription[D_AI], "D_AI");
   strcpy(gArgDiscription[D_SSN], "D_SSN");
   strcpy(gArgDiscription[D_PT], "D_PT");
   strcpy(gArgDiscription[D_NOA], "D_NOA");
   strcpy(gArgDiscription[D_TT], "D_TT");
   strcpy(gArgDiscription[D_NP], "D_NP");
   strcpy(gArgDiscription[D_GT], "D_GT");
   strcpy(gArgDiscription[ISACN], "ISACN");
   strcpy(gArgDiscription[ISUSERINFO], "ISUSERINFO");
   strcpy(gArgDiscription[DLG_TYPE], "DLG_TYPE");
   strcpy(gArgDiscription[COMP_PRESENT], "COMP_PRESENT");
   strcpy(gArgDiscription[INVOKEID], "INVOKEID");
   strcpy(gArgDiscription[COMPONENT_TYPE], "COMPONENT_TYPE");
   strcpy(gArgDiscription[OPCODE], "OPCODE");
   strcpy(gArgDiscription[ERR_TYPE], "ERR_TYPE");
   strcpy(gArgDiscription[PROBLEM_TYPE], "PROBLEM_TYPE");
   strcpy(gArgDiscription[ABORT_TYPE], "ABORT_TYPE");
   strcpy(gArgDiscription[LAST_INDICATOR], "LAST_INDICATOR");
   strcpy(gArgDiscription[ERR_CODE], "ERR_CODE");
   strcpy(gArgDiscription[REJECT_TYPE], "REJECT_TYPE");
   strcpy(gArgDiscription[PROBLEM_CODE], "PROBLEM_CODE");
   strcpy(gArgDiscription[DLGID], "DLGID");
   strcpy(gArgDiscription[HDLR_SSN], "HDLR_SSN");
   strcpy(gArgDiscription[MSG_TYPE], "MSG_TYPE");
   strcpy(gArgDiscription[O_TRANSID], "O_TRANSID");
   strcpy(gArgDiscription[TCUSERID],"TCUSERID" );
   strcpy(gArgDiscription[USERINFO], "USERINFO");
   strcpy(gArgDiscription[APPL_CONTEXT], "APPL_CONTEXT");
   strcpy(gArgDiscription[ACN_VER], "ACN_VER");
   strcpy(gArgDiscription[PARAM_DATA], "PARAM_DATA");
   strcpy(gArgDiscription[DLG_MAX], "DLG_MAX");
   strcpy(gArgDiscription[LOAD], "LOAD");
   
   strcpy(gArgDiscription[NO_OF_PDU], "NO_OF_PDU");
   strcpy(gArgDiscription[PDU_0], "PDU_0");
   strcpy(gArgDiscription[PDU_1], "PDU_1");
   strcpy(gArgDiscription[PDU_2], "PDU_2");
   strcpy(gArgDiscription[PDU_3], "PDU_3");
   strcpy(gArgDiscription[PDU_4], "PDU_4");
   strcpy(gArgDiscription[PDU_5], "PDU_5");
   strcpy(gArgDiscription[PDU_6], "PDU_6");
   strcpy(gArgDiscription[PDU_7], "PDU_7");
   strcpy(gArgDiscription[PDU_8], "PDU_8");
   strcpy(gArgDiscription[PDU_9], "PDU_9");
   
   strcpy(gArgDiscription[KEY_IDX_UNKOWN], "KEY_IDX_UNKOWN" );

   return 0;
}

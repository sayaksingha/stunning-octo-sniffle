#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sccp_api.h"
#include "sccp_synch.h"

char * GetAcuTcapMsgTypeStr(acu_sccp_msg_type_t &lMsgType)
{
   switch(lMsgType)
   {
      case ACU_SCCP_MSG_DATA_IND:
         return "DATA";

      case ACU_SCCP_MSG_NOTICE:
         return "NOTICE";

      case ACU_SCCP_MSG_USER_STATUS:
         return "USER_STATUS";

      case ACU_SCCP_MSG_SP_STATUS:
         return "SP_STATUS";

      case ACU_SCCP_MSG_TIMEOUT:
         return "MSG_TIMEOUT";

      case ACU_SCCP_MSG_CON_STATE:
         return "CON_STATE";

      case ACU_SCCP_MSG_UNITDATA:
         return "MSG_UNITDATA";

      default:
         return "UNHANDLED";
         break;
   }
}

acu_sccp_msg_type_t HandleEventAcuSccpMsg(acu_sccp_msg_t *lAcuSccpMsgPtr)
{

   switch(lAcuSccpMsgPtr->tm_msg_type)
   {
      case ACU_SCCP_MSG_DATA_IND:
         {
            return ACU_SCCP_MSG_DATA_IND;
         }
      case ACU_SCCP_MSG_NOTICE:
         {
            return ACU_SCCP_MSG_NOTICE;
         }
      case ACU_SCCP_MSG_USER_STATUS:
         {
            return ACU_SCCP_MSG_USER_STATUS;
         }
      case ACU_SCCP_MSG_SP_STATUS:
         {
            return ACU_SCCP_MSG_SP_STATUS;
         }
      case ACU_SCCP_MSG_TIMEOUT:
         {
            return ACU_SCCP_MSG_TIMEOUT;
         }
      case ACU_SCCP_MSG_CON_STATE:
         {
            return ACU_SCCP_MSG_CON_STATE;
         }
      case ACU_SCCP_MSG_UNITDATA:
         {
            return ACU_SCCP_MSG_UNITDATA;
         }
      default:
         printf("%d Unexpected Message Type\n",lAcuSccpMsgPtr->tm_msg_type);
         return lAcuSccpMsgPtr->tm_msg_type;
         break;
   }
}


char * GetAcuUserStatusMsgTypeStr(const acu_sccp_u_svals &lStatus)
{
   switch(lStatus)
   {
      case ACU_SCCP_UIS:
         return "ACU_SCCP_UIS";
      case ACU_SCCP_UOS:
         return "ACU_SCCP_UOS";
      default:
         return "UNHANDLED";
         break;
   }
}

char * GetAcuSpStatusTypeStr(const acu_sccp_sp_svals &lStatus)
{
   switch(lStatus)
   {
      case ACU_SCCP_SP_PROHIBIT:
         return "ACU_SCCP_SP_PROHIBIT";
      case ACU_SCCP_SP_ACCESS:
         return "ACU_SCCP_SP_ACCESS";
      default:
         return "UNHANDLED";
         break;
   }
}

char * GetAcuSccpStatusTypeStr(const acu_sccp_sccp_svals &lStatus)
{
   switch(lStatus)
   {
      case ACU_SCCP_REM_SCCP_PROHIBIT:
         return "ACU_SCCP_REM_SCCP_PROHIBIT";
      case ACU_SCCP_REM_SCCP_UNAVAIL:
         return "ACU_SCCP_REM_SCCP_UNAVAIL";
      case ACU_SCCP_REM_SCCP_UNEQUIP:
         return "ACU_SCCP_REM_SCCP_UNEQUIP";
      case ACU_SCCP_REM_SCCP_INACCESS:
         return "ACU_SCCP_REM_SCCP_INACCESS";
      case ACU_SCCP_REM_SCCP_CONGEST:
         return "ACU_SCCP_REM_SCCP_CONGEST";
      case ACU_SCCP_REM_SCCP_AVAIL:
         return "ACU_SCCP_REM_SCCP_AVAIL";
      default:
         return "UNHANDLED";
         break;
   }
}

int DecodeAddress(acu_sccp_msg_t *lAcuSccpMsgPtr, unsigned char *orig)
{
   //for(int i = 0; i < lAcuSccpMsgPtr->tm_data_length; i++)
   {
   //   printf("%02X|", data[i]);
   }
  // printf("\n");

   if(NULL == lAcuSccpMsgPtr->tm_local_addr)
   {
      printf("Local Address Not Found\n");
   }
   printf("Local Address Found :%d\n", lAcuSccpMsgPtr->tm_local_addr->sa_gt.sag_num);

   if(NULL == lAcuSccpMsgPtr->tm_remote_addr)
   {
      printf("Remote Address Not Found\n");
   }
   printf("Remote Address Found :%d\n", lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num);
   if(NULL == lAcuSccpMsgPtr->tm_con)
   {
      printf("Remote Address Not Found\n");
   }
   printf("Connection Found :%d\n", lAcuSccpMsgPtr->tm_con);
   for(int i = 0; i < lAcuSccpMsgPtr->tm_local_addr->sa_gt.sag_num; i++)
   {
      printf("%02X|", lAcuSccpMsgPtr->tm_local_addr->sa_gt.sag_digits[i]);
   }
   printf("\n");
   for(int i = 0; i < lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num; i++)
   {
      printf("%02X|", lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_digits[i]);
   }
   printf("\n");

   int lTempCount = 0;
   if(0 == (lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num % 2 ))
   {
      lTempCount = lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num / 2;
   }
   else
   {
      lTempCount = lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num / 2 + 1 ;
   }
   printf("lTempCount:%d\n", lTempCount);

   int j = 0;
   for(int i = 0; i < lTempCount; i++)
   {
      orig[j++] = lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_digits[i] & 0x0F;
      printf("j:%d %X\n", j, orig[j-1]);
      orig[j++] = (lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_digits[i] >> 4) & 0x0F;
      printf("j:%d %X\n", j, orig[j-1]);
   }
   lTempCount = j;
   printf("j:%d  lTempCount:%d\n",j,lTempCount);
   printf("\t%-15s:","digits");
   for (int i = 0; i <  lTempCount; i++)
      printf("%X", orig[i]);
   printf("\n");
   return true;
}

int main(int argc, char *argv[])
{
   acu_sccp_ssap_t *lSccpSsap;
   const acu_sccp_con_state_t *pConA_State, *pConB_State;
   acu_sccp_msg_t *lAcuSccpMsgPtr;
   const acu_sccp_sccp_status_t *lSccp_status;
   acu_sccp_addr_t *lSccpAddr;
   acu_sccp_con_t *lSccpCon;


   lSccpSsap = acu_sccp_ssap_create("Sccp_6.cfg", ACU_SCCP_LOG_STDERR);
   if(NULL == lSccpSsap)
   {
      printf("SCCP: SSAP creation failed\n");
      return -1;
   }
   printf("SCCP: SSAP creation success\n");
   sleep(1);

   lSccpAddr = acu_sccp_ssap_get_locaddr(lSccpSsap);
   printf("SCCP: PC:%d\n", lSccpAddr->sa_pc);

   if(220 != lSccpAddr->sa_pc)
   {
      printf("SCCP: SSAP point code difference : %d\n", lSccpAddr->sa_pc);
      acu_sccp_ssap_delete(lSccpSsap);
      return false;
   }
   printf("SCCP: SSAP point code : %d\n", lSccpAddr->sa_pc);

   lSccpAddr = acu_sccp_ssap_get_remaddr(lSccpSsap);

   if(221 != lSccpAddr->sa_pc)
   {
      printf("SCCP: SSAP point code difference : %d:%d\n", lSccpAddr->sa_pc, lSccpAddr->sa_ssn);
      acu_sccp_ssap_delete(lSccpSsap);
      return false;
   }
   printf("SCCP: SSAP point code : %d:%d\n", lSccpAddr->sa_pc, lSccpAddr->sa_ssn);

   acu_sccp_enable_sp_status(lSccpSsap, lSccpAddr->sa_pc);
   //acu_sccp_enable_sp_status(lSccpSsap, ~0u);
   printf("SSAP SP status enable Success remote PC: %d\n",  lSccpAddr->sa_pc);

   //acu_sccp_enable_user_status(lSccpSsap, lSccpAddr->sa_pc ? lSccpAddr->sa_pc : ~0u,
     //              lSccpAddr->sa_ssn ? lSccpAddr->sa_ssn : ~0u);
   //acu_sccp_enable_user_status(lSccpSsap,  ~0u, ~0u);
   printf("SSAP user status recieve enabled remote(PC:%d SSN:%d)\n", lSccpAddr->sa_pc, lSccpAddr->sa_ssn);

   if(0 != acu_sccp_ssap_connect_sccp(lSccpSsap))
   {
      printf("SSAP connected to SCCP driver code failed\n");
      return false;
   }

   printf("SSAP connected to SCCP driver code success\n");


   while(true)
   {
      if(0 != acu_sccp_ssap_msg_get(lSccpSsap, 500, &lAcuSccpMsgPtr))
      {
         //printf("Tcap Msg recieved Failed with msgType: %s\n", GetAcuTcapMsgTypeStr((lAcuSccpMsgPtr)->tm_msg_type));
         //return false;
         continue;
      }
      else
      {
         printf("Tcap Msg recieved Success with msgType: %s\n", GetAcuTcapMsgTypeStr((lAcuSccpMsgPtr)->tm_msg_type));
      }
      printf("----------------------------------------------------------------------\n");
   
      acu_sccp_msg_type_t lMsgType; 
      lMsgType = HandleEventAcuSccpMsg(lAcuSccpMsgPtr);
      
      switch(lMsgType)
      {
         case ACU_SCCP_MSG_UNITDATA:
            {
               unsigned char data[300];
               unsigned char orig[100];
               unsigned char dest[100];

               printf("ACU_SCCP_MSG_UNITDATA Indication Len:%d\n", lAcuSccpMsgPtr->tm_data_length);
               memcpy(data, lAcuSccpMsgPtr->tm_data, lAcuSccpMsgPtr->tm_data_length);
               for(int i = 0; i < lAcuSccpMsgPtr->tm_data_length; i++)
               {
                  printf("%02X|", data[i]);
               }
               printf("\n");

               if(NULL == lAcuSccpMsgPtr->tm_local_addr)
               {
                  printf("Local Address Not Found\n");
               }
               printf("Local Address Found :%d\n", lAcuSccpMsgPtr->tm_local_addr->sa_gt.sag_num);

               if(NULL == lAcuSccpMsgPtr->tm_remote_addr)
               {
                  printf("Remote Address Not Found\n");
               }
               printf("Remote Address Found :%d\n", lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num);
               if(NULL == lAcuSccpMsgPtr->tm_con)
               {
                  printf("Remote Address Not Found\n");
               }
               printf("Connection Found :%d\n", lAcuSccpMsgPtr->tm_con);
               for(int i = 0; i < lAcuSccpMsgPtr->tm_local_addr->sa_gt.sag_num; i++)
               {
                  printf("%02X|", lAcuSccpMsgPtr->tm_local_addr->sa_gt.sag_digits[i]);
               }
                  printf("\n");
               for(int i = 0; i < lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num; i++)
               {
                  printf("%02X|", lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_digits[i]);
               }
               printf("\n");

               int lTempCount = 0;
               if(0 == (lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num % 2 ))
               {
                  lTempCount = lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num / 2;
               }
               else
               {
                  lTempCount = lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num / 2 + 1 ;
               }
                     printf("lTempCount:%d\n", lTempCount);

               int j = 0;
               for(int i = 0; i < lTempCount; i++)
               {
                  orig[j++] = lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_digits[i] & 0x0F;
                           printf("j:%d %X\n", j, orig[j-1]);
                  orig[j++] = (lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_digits[i] >> 4) & 0x0F;
                           printf("j:%d %X\n", j, orig[j-1]);
               }
               lTempCount = j;
               printf("j:%d  lTempCount:%d\n",j,lTempCount);
               printf("\t%-15s:","digits");
                  for (int i = 0; i <  lTempCount; i++)
                           printf("%X", orig[i]);
                     printf("\n");
                     //printf("%X\n",lAcuSccpMsgPtr->tm_data[2]);
                     printf("%X\n",data[2]);
                     data[2] = 0x49;
                     printf("%X\n",data[2]);
               //memcpy(&lAcuSccpMsgPtr->tm_data, data, lAcuSccpMsgPtr->tm_data_length);
               //memcpy(lSccpAddr, lAcuSccpMsgPtr->tm_local_addr, sizeof(lAcuSccpMsgPtr->tm_local_addr));
               //memcpy(&lAcuSccpMsgPtr->tm_local_addr, lAcuSccpMsgPtr->tm_remote_addr,sizeof(lAcuSccpMsgPtr->tm_local_addr));
               //memcpy(&lAcuSccpMsgPtr->tm_remote_addr, lSccpAddr,sizeof(lAcuSccpMsgPtr->tm_local_addr));

               //if(0 != acu_sccp_unitdata_request(lAcuSccpMsgPtr->tm_con, lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_digits,lAcuSccpMsgPtr->tm_remote_addr->sa_gt.sag_num))
               //{
               //   printf("Failed to send acu_sccp_unitdata_request\n");
              // }
               //   printf("Success to send acu_sccp_unitdata_request\n");
              //   void acu_sccp_con_unblock(acu_sccp_con_t *con);
               //DecodeAddress();
                 acu_sccp_con_unblock(lAcuSccpMsgPtr->tm_con);
                 printf("----------------------------------------------------------------------\n");
               break;
            }
         case ACU_SCCP_MSG_DATA_IND:
            {
               printf("Message Indication\n");
               break;
            }
         case ACU_SCCP_MSG_CON_STATE:
            {
               printf("Message Connection Status\n");
               acu_sccp_msg_get_con_state(lAcuSccpMsgPtr, &pConA_State, &pConB_State);
               if(pConA_State->cs_state & ACU_SCCP_CON_STATE_IDLE)
               {
                  printf("SCCP: Disconnected, %s\n",pConA_State->cs_fail_text);
               }

               if(pConA_State->cs_state & ACU_SCCP_CON_STATE_CONNECTING)
               {
                  printf("SCCP: Connecting\n");
               }
               printf("------------------111111\n");

               if(pConA_State->cs_state & ACU_SCCP_CON_STATE_CONNECTED)
               {
                  printf("SCCP: Connected\n");
               }
               printf("------------------222222\n");

               if(pConA_State->cs_state & ACU_SCCP_CON_STATE_IN_SERVICE)
               {
                  printf("SCCP: IN Service, %s\n",pConA_State->cs_fail_text);
               }
               printf("------------------333333\n");
               sleep(2);

               acu_sccp_get_con_state(lSccpSsap,0, &pConA_State);
               printf("SCCP: %d %s\n",pConA_State->cs_state, pConA_State->cs_fail_text);
               printf("------------------444444\n");

               if(pConA_State->cs_state & ACU_SCCP_CON_STATE_IDLE)
               {
                  printf("SCCP: Disconnected, %s\n",pConA_State->cs_fail_text);
               }
               printf("------------------555555\n");

               if(pConA_State->cs_state & ACU_SCCP_CON_STATE_CONNECTING)
               {
                  printf("SCCP: Connecting\n");
               }
               printf("------------------666666\n");

               if(pConA_State->cs_state & ACU_SCCP_CON_STATE_CONNECTED)
               {
                  printf("SCCP: Connected\n");
               }
               printf("------------------777777\n");

               if(pConA_State->cs_state & ACU_SCCP_CON_STATE_IN_SERVICE)
               {
                  printf("SCCP: IN Service, %s\n",pConA_State->cs_fail_text);
               }
               printf("------------------888888\n");
               printf("--------------------------------------------------------------------------------------------\n");
               
               break;

            }

         case ACU_SCCP_MSG_SP_STATUS:
         //case ACU_SCCP_MSG_USER_STATUS:
            {
               printf("Message SP/USER status\n");
               //if(0 == acu_sccp_get_sccp_status(lSccpSsap, 000, 7, &lSccp_status))
               if(0 == acu_sccp_get_sccp_status(lSccpSsap, 221, 7, &lSccp_status))
               {
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Success\n");
                  printf("SCCP: pc:%d ssn:%d host:%c\n", lSccp_status->tsp_pc, lSccp_status->tsp_ssn, lSccp_status->tsp_host);
                  printf("SCCP: User Status:%s\n",GetAcuUserStatusMsgTypeStr(lSccp_status->tsp_user_status));
                  printf("SCCP: SP Status:%s\n",GetAcuSpStatusTypeStr(lSccp_status->tsp_sp_status));
                  printf("SCCP: SCCP Status:%s\n",GetAcuSccpStatusTypeStr(lSccp_status->tsp_sccp_status));
                  printf("--------------------------------------------------------\n");
               }
               else
               {
                  printf("--------------------------------------------------------\n");
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Failed\n");
               }
               if(0 == acu_sccp_get_sccp_status(lSccpSsap, 222, 7, &lSccp_status))
               {
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Success\n");
                  printf("SCCP: pc:%d ssn:%d host:%c\n", lSccp_status->tsp_pc, lSccp_status->tsp_ssn, lSccp_status->tsp_host);
                  printf("SCCP: User Status:%s\n",GetAcuUserStatusMsgTypeStr(lSccp_status->tsp_user_status));
                  printf("SCCP: SP Status:%s\n",GetAcuSpStatusTypeStr(lSccp_status->tsp_sp_status));
                  printf("SCCP: SCCP Status:%s\n",GetAcuSccpStatusTypeStr(lSccp_status->tsp_sccp_status));
                  printf("--------------------------------------------------------\n");
               }
               else
               {
                  printf("--------------------------------------------------------\n");
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Failed\n");
               }
               if(0 == acu_sccp_get_sccp_status(lSccpSsap, 223, 7, &lSccp_status))
               {
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Success\n");
                  printf("SCCP: pc:%d ssn:%d host:%c\n", lSccp_status->tsp_pc, lSccp_status->tsp_ssn, lSccp_status->tsp_host);
                  printf("SCCP: User Status:%s\n",GetAcuUserStatusMsgTypeStr(lSccp_status->tsp_user_status));
                  printf("SCCP: SP Status:%s\n",GetAcuSpStatusTypeStr(lSccp_status->tsp_sp_status));
                  printf("SCCP: SCCP Status:%s\n",GetAcuSccpStatusTypeStr(lSccp_status->tsp_sccp_status));
                  printf("--------------------------------------------------------\n");
               }
               else
               {
                  printf("--------------------------------------------------------\n");
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Failed\n");
               }
               if(0 == acu_sccp_get_sccp_status(lSccpSsap, 224, 7, &lSccp_status))
               {
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Success\n");
                  printf("SCCP: pc:%d ssn:%d host:%c\n", lSccp_status->tsp_pc, lSccp_status->tsp_ssn, lSccp_status->tsp_host);
                  printf("SCCP: User Status:%s\n",GetAcuUserStatusMsgTypeStr(lSccp_status->tsp_user_status));
                  printf("SCCP: SP Status:%s\n",GetAcuSpStatusTypeStr(lSccp_status->tsp_sp_status));
                  printf("SCCP: SCCP Status:%s\n",GetAcuSccpStatusTypeStr(lSccp_status->tsp_sccp_status));
                  printf("--------------------------------------------------------\n");
               }
               else
               {
                  printf("--------------------------------------------------------\n");
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Failed\n");
               }
               break;
            }
         case ACU_SCCP_MSG_USER_STATUS:
            {
               printf("Message SP/USER status\n");
              /* if(0 == acu_sccp_get_sccp_status(lSccpSsap, 215, 8, &lSccp_status))
               {
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Success\n");
                  printf("SCCP: pc:%d ssn:%d host:%c\n", lSccp_status->tsp_pc, lSccp_status->tsp_ssn, lSccp_status->tsp_host);
                  printf("SCCP: User Status:%s\n",GetAcuUserStatusMsgTypeStr(lSccp_status->tsp_user_status));
                  printf("SCCP: SP Status:%s\n",GetAcuSpStatusTypeStr(lSccp_status->tsp_sp_status));
                  printf("SCCP: SCCP Status:%s\n",GetAcuSccpStatusTypeStr(lSccp_status->tsp_sccp_status));
                  //acu_sccp_con_t *acu_sccp_con_create(acu_sccp_ssap_t ssap);

                  lSccpCon = acu_sccp_con_create(lSccpSsap);
                  if(NULL == lSccpCon)
                  {
                     printf("Creating SCCP Conection Failed\n");
                  }
                  else
                  {
                     printf("Creating SCCP Conection Success\n");
                  }*/
                 // {9F843F8312A23589098003101B0D120600720419892400013372540B120700120419893200000250624E48047A0ED8F06B1E281C060700118605010101A011600F80020780A1090607040000010001026C26A124020100020102301C040804440910302143F5810791198934000002040791198934000002}
               //}
               //else
               {
                  printf("SCCP: SSAP acu_sccp_get_sccp_status Failed\n");
               }
               break;
            }

         default:
            {
               printf("Default Case\n");
            }
      }
   }
      acu_sccp_ssap_delete(lSccpSsap);
      return 0;
   }

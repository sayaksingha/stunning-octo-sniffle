#include<iostream>
#include<stdlib.h>
#include<string>
#include<stdio.h>
#include<sys/time.h>
#include<sstream>
#include <string.h>

using namespace std;
string ToString(int);
int LiRand1dig();
int LiRand2dig();
string EncryptLIC(int,string);
int LiLineNo, LiRandom1 ,LiRandom2 =0;
string LiCardNo,LiLicenseNo;

int main()
{
   int lPassInt1 = 0;
   int lPassInt2 = 0;
   struct   timeval time_ptr;
   struct   tm *time_tm;
   time_t   t;
   char lBuffer[200], lDateMonthString[3] = "";
   char lPassFinal[7] = "";
   char lPassWord[7] = "";

   gettimeofday(&time_ptr, 0);
   t = time_ptr.tv_sec;
   time_tm = localtime(&t);

   sprintf(lBuffer,"%04d-%02d-%02d %02d:%02d:%02d:%03ld ", 1900 + time_tm->tm_year,
         1 + time_tm->tm_mon,
         time_tm->tm_mday,
         time_tm->tm_hour,
         time_tm->tm_min,
         time_tm->tm_sec,
         time_ptr.tv_usec/1000);
   cout << "CURRENT TIME:" << lBuffer << endl;
   lPassInt1 = 1901 + time_tm->tm_year + time_tm->tm_mon + time_tm->tm_mday;
   lPassInt2 = 1 + time_tm->tm_mon + time_tm->tm_mday + (time_tm->tm_min / 2);
   sprintf(lDateMonthString, "%d", lPassInt2);
   lPassInt2 = (lDateMonthString[0] - 48 ) + (lDateMonthString[1] - 48);
   sprintf( lPassFinal,"%04d%02d", lPassInt1,lPassInt2);
#ifdef FOR_LAB_USE   
   sprintf(lPassWord, "%s", lPassFinal);
#else
   cout << lPassFinal;
   cout << "\nEnter PASSWORD :";
   cin >> lPassWord;
#endif

   if ( strcmp(lPassFinal,lPassWord) != 0 )
   {
      cout << "\nInalid PASSWORD...\n" ;
      exit(0);
   }

   cout << "\nEnter the number of messages for licensing :" ;
   cin >> LiLineNo ;
   if(LiLineNo < 10)
   {   
      cout << "Min number of messages for licensing is 10" << endl;
      cout << "Please enter a value between 10 to 12800" << endl;
   }   
   if(LiLineNo > 12800)
   {
      cout << "Max number of messages for licensing is 12800" << endl;
      cout << "Please enter a value between 10 to 12800" << endl;
   }

#ifdef FOR_LAB_USE
   LiCardNo = "T@Y@N@TCAP@ACU";
#else
   cout << "\nEnter the Pass string for licensing :" ;
   cin >> LiCardNo;
#endif
   LiLicenseNo = EncryptLIC(LiLineNo , LiCardNo);
   cout <<"LICENSE_KEY is : "<<LiLicenseNo << endl;
   return 0;
}

string EncryptLIC(int LiLineNo , string li_cardno)
{
   int LiConst1 = 56025 , LiConst2 = 2587 , LiSum1 ,
       LiNum=0 , i , LiSum4 , LiSum3 ;
   string LiLicNo1 , LiLicno2 , TostringR1;
   LiRandom1 = LiRand1dig();
   LiSum1 = (LiConst1 - LiLineNo) * LiRandom1;
   for(i =0 ;i<li_cardno.length();++i)
   {
      LiNum = LiNum + (toascii(li_cardno[i]) * (i+1));
   }
   LiSum3 = LiSum1 + LiNum;
   TostringR1 = "00" + ToString(LiRandom1);
   LiLicNo1 = ToString(LiSum3) + TostringR1;
   srand(LiRandom1);
   LiRandom2 = LiRand2dig();
   LiSum4 = LiConst2 + LiRandom2 + LiLineNo ;
   LiLicno2 = ToString(LiRandom2) + LiLicNo1;
   LiLicenseNo = LiLicno2 + ToString(LiSum4);
   return(LiLicenseNo);
}
int LiRand1dig()
{

   int high = 9 , low = 1 , interval , offset , Random1;
   interval = (high - low) + 1;
   srand(Random1);
   offset = rand()%interval;
   Random1 = low + offset;
   return(Random1);
}

int LiRand2dig()
{

   int high = 9, low = 1 , interval , offset , Random2;
   interval = (high - low) + 1;
   offset = rand()%interval;
   Random2 = low + offset;
   return(Random2);
}

string ToString(int num)
{
   stringstream dd;
   string strn;
   dd<<num;
   strn = dd.str();
   return(strn);
}

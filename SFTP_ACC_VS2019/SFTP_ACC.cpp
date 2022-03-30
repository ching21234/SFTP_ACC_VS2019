// SFTP_ACC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



// SFTP_acc.cpp : 定義主控台應用程式的進入點。
//
//  最新放最上面   20210504  ching2
//  
// main()
// 	if (argv[1] == NULL)
//	{
//	printf("SFTP_ACC 20210609   V1.1  \r\n");
//
//
//
// 2021***  V1.1


// 20210609   V1.1   ching2      add  conf_SQL_Server, conf_sftp_ip, conf_sftp
// 20210514   V1.0   ching2      init ver




#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>       // for difftime

char conf_SQLID[500];            //
char conf_SQLPW[500];            //
char conf_SQL_Server[500];		 //

// 20210609   V1.1   ching2      add  conf_SQL_Server, conf_sftp_ip, conf_sftp
char conf_sftp_ip[500];          //
char conf_sftp[500];             //



//1081125 new
//1080523    V2.40  ching2  1080523 弱掃 
//-----------------------------------------------------------------------------
//  NO:3-6 system_new()
//  IN :  char *scmd
//  OUT:  0: 成功 , -1: 失敗
//-----------------------------------------------------------------------------

int  system_new(char* scmd)
{
	FILE* pPipe = NULL;

	if ((pPipe = _popen(scmd, "rt")) == NULL)
	{
		printf("執行失敗. %s-%s(%d)\r\n", __FILE__, __FUNCTION__, __LINE__);
		return(-1);
	}
	else
	{
		//printf("執行成功. %s-%s(%d)\r\n", __FILE__, __FUNCTION__, __LINE__);
		_pclose(pPipe);
		return(0);
	}
}



//-----------------------------------------------------------------------------
// for DES
//-----------------------------------------------------------------------------

extern _declspec(dllexport) int _stdcall des_decrypt(BYTE* data, int count, BYTE* key);
extern _declspec(dllexport) int _stdcall des_encrypt(BYTE* data, int count, BYTE* key);

typedef unsigned char   uchar;
typedef unsigned char   BYTE;



//------------------------------------------------------------------------------
//  NO:5-1 *PackHEX( uchar *str, uchar *hex, int sz )
// 
//        
//  IN :
//  OUT: 
//------------------------------------------------------------------------------
int Toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return('A' + c - 'a');
	return(c);
}


//5-2
//---------------------------------------------------------------------------
char* PackHEX(uchar* str, uchar* hex, int sz)
{
	int i = 0, j = 0;

	while (sz-- > 0) {
		int c;

		if ((c = Toupper(*(hex + j++))) >= 'A')
			*(str + i) = (c - 'A' + 10) << 4;
		else
			*(str + i) = (c - '0') << 4;
		if ((c = Toupper(*(hex + j++))) >= 'A')
			*(str + i++) += c - 'A' + 10;
		else
			*(str + i++) += c - '0';
	}
	return((char*)str);
}



//----------------------------------------------------------------------------------------------
// 開 hotTPH_L.txt  參數檔   讀進sql 帳號 及 密碼
//
//-----------------------------------------------------------------------------
int funRead_set()
{
	FILE* fd;
	char tmp[2048] = { 0x00 };
	char func[200] = { 0x00 }, data[200] = { 0x00 };
	int i = 0;

	char s1[5000] = { 0x00 };

	//  show_time();
	printf("\r\n");

	printf(s1, sizeof(s1), "funRead_set() start !!! --------------------------------------------- \r\n");

	strcpy_s(tmp, sizeof(tmp), "..\\run\\conf\\hotfar.txt"); // 開hotfar.txt 設定檔

	errno_t myErrNoT = fopen_s(&fd, tmp, "r");
	if (myErrNoT != 0) {
		// 1081107 new
		sprintf_s(s1, sizeof(s1), "open %s error !!! \r\n", tmp);
		sprintf_s(s1, sizeof(s1), "\r\n funRead_set() end   !!! --------------------------------------------- \r\n");
		//1081203 old printf(s1);           // 1081203 V1.4 弱掃1  L138
		printf("%s", s1);                    // 1081203 V1.4 弱掃1  L138
		return(-1);

	}


	memset(tmp, 0x20, sizeof(tmp));

	while (fgets(tmp, sizeof(tmp), fd) != 0) {
		memset(func, 0x20, sizeof(func));
		memset(data, 0x20, sizeof(data));

		sprintf_s(s1, sizeof(s1), "tmp=[%s] \r\n", tmp);
		//1081203 oldprintf(s1);           // 1081203 V1.4 弱掃1  L151
		printf("%s", s1);                  // 1081203 V1.4 弱掃1  L151

		sscanf_s(tmp, "%s %s ", func, sizeof(func), data, sizeof(data));

		if (!strncmp(func, "#", 1))   continue;
		if (!strncmp(func, " ", 1))   continue;

		//1
		if (strncmp(tmp, "conf_SQL_Server", strnlen_s("conf_SQL_Server", sizeof("conf_SQL_Server"))) == 0)
		{
			sscanf_s(tmp, "%*s %s", data, sizeof(data));
			strcpy_s(conf_SQL_Server, sizeof(conf_SQL_Server), data);
			continue;
		}
		//2
		if (strncmp(tmp, "conf_SQLID1", strnlen_s("conf_SQLID1", sizeof("conf_SQLID1"))) == 0)
		{
			sscanf_s(tmp, "%*s %s", data, sizeof(data));
			strcpy_s(conf_SQLID, sizeof(conf_SQLID), data);
			continue;
		}
		//3
		if (strncmp(tmp, "conf_SQLPW1", strnlen_s("conf_SQLPW1", sizeof("conf_SQLPW1"))) == 0)
		{
			sscanf_s(tmp, "%*s %s", data, sizeof(data));
			strcpy_s(conf_SQLPW, sizeof(conf_SQLPW), data);
			continue;
		}

		// 20210609   V1.1   ching2      add  conf_SQL_Server, conf_sftp_ip, conf_sftp
		//4 conf_SQL_Server
		if (strncmp(tmp, "conf_SQL_Server", strnlen_s("conf_SQL_Server", sizeof("conf_SQL_Server"))) == 0)
		{
			sscanf_s(tmp, "%*s %s", data, sizeof(data));
			strcpy_s(conf_SQL_Server, sizeof(conf_SQL_Server), data);
			continue;
		}
		//5 conf_sftp_ip
		if (strncmp(tmp, "conf_sftp_ip", strnlen_s("conf_sftp_ip", sizeof("conf_sftp_ip"))) == 0)
		{
			sscanf_s(tmp, "%*s %s", data, sizeof(data));
			strcpy_s(conf_sftp_ip, sizeof(conf_sftp_ip), data);
			continue;
		}
		//6 conf_sftp
		if (strncmp(tmp, "conf_sftp", strnlen_s("conf_sftp", sizeof("conf_sftp"))) == 0)
		{
			sscanf_s(tmp, "%*s %s", data, sizeof(data));
			strcpy_s(conf_sftp, sizeof(conf_sftp), data);
			continue;
		}


	}

	// 1081129 new
	sprintf_s(s1, sizeof(s1), "conf_SQL_Server  = [%s] \r\n", conf_SQL_Server);
	//1081203 old printf(s1);           // 1081203 V1.4 弱掃1  L206
	printf("%s", s1);                   // 1081203 V1.4 弱掃1  L206

	sprintf_s(s1, sizeof(s1), "conf_SQLID = [%s] \r\n", conf_SQLID);
	//1081203 old printf(s1);           // 1081203 V1.4 弱掃1  L208
	printf("%s", s1);                    // 1081203 V1.4 弱掃1  L208

	sprintf_s(s1, sizeof(s1), "conf_SQLPW = [%s] \r\n\r\n", conf_SQLPW);
	//1081203 old printf(s1);           // 1081203 V1.4 弱掃1  L210
	printf("%s", s1);                    // 1081203 V1.4 弱掃1  L210


	// 20210609   V1.1   ching2      add  conf_SQL_Server, conf_sftp_ip, conf_sftp
	//4
	sprintf_s(s1, sizeof(s1), "conf_SQL_Server = [%s] \r\n\r\n", conf_SQL_Server);
	//1081203 old printf(s1);            // 1081203 V1.4 弱掃1  L210
	printf("%s", s1);                    // 1081203 V1.4 弱掃1  L210

	//5
	sprintf_s(s1, sizeof(s1), "conf_sftp_ip = [%s] \r\n\r\n", conf_sftp_ip);
	//1081203 old printf(s1);            // 1081203 V1.4 弱掃1  L210
	printf("%s", s1);                    // 1081203 V1.4 弱掃1  L210

	//6
	sprintf_s(s1, sizeof(s1), "conf_sftp = [%s] \r\n\r\n", conf_sftp);
	//1081203 old printf(s1);            // 1081203 V1.4 弱掃1  L210
	printf("%s", s1);                    // 1081203 V1.4 弱掃1  L210




	//970727  ching2 add for DES PWD
	//----------------------------------------------------
	// 1081129 new sql pw

	BYTE  sMkey_2[40];
	BYTE  sTemp_2[2000];

	memcpy_s(sMkey_2, sizeof(sMkey_2), "22730558", 8);

	PackHEX((unsigned char*)sTemp_2, (unsigned char*)conf_SQLPW, 32);
	sTemp_2[16] = 0x00;

	des_decrypt(sTemp_2, 16, sMkey_2);
	memcpy_s(conf_SQLPW, sizeof(conf_SQLPW), sTemp_2, 16);
	conf_SQLPW[16] = 0x00;

	for (i = 15; i > 0; i--)
	{
		if (conf_SQLPW[i] != 0x20)  break;
		if (conf_SQLPW[i] == 0x20)
		{
			conf_SQLPW[i] = 0x00;
		}
	}

	//funPrint_Log("conf_SQLPW = [%s] \r\n\r\n" , conf_SQLPW);
	//printf("conf_SQLPW = [%s] \r\n\r\n", conf_SQLPW);




	// 20210609   V1.1   ching2      add  conf_SQL_Server, conf_sftp_ip, conf_sftp

	//970727  ching2 add for DES PWD
	//----------------------------------------------------
	// 1081129 new sql pw

	//BYTE  sMkey_2[40];
	//BYTE  sTemp_2[2000];

	memcpy_s(sMkey_2, sizeof(sMkey_2), "22730558", 8);

	PackHEX((unsigned char*)sTemp_2, (unsigned char*)conf_sftp, 32);
	sTemp_2[16] = 0x00;

	des_decrypt(sTemp_2, 16, sMkey_2);
	memcpy_s(conf_sftp, sizeof(conf_sftp), sTemp_2, 16);
	conf_sftp[16] = 0x00;

	for (i = 15; i > 0; i--)
	{
		if (conf_sftp[i] != 0x20)  break;
		if (conf_sftp[i] == 0x20)
		{
			conf_sftp[i] = 0x00;
		}
	}

	//funPrint_Log("conf_sftp = [%s] \r\n\r\n" , conf_sftp);
	//printf("conf_sftp = [%s] \r\n\r\n", conf_sftp);




	printf("funRead_set() end   !!! --------------------------------------------- \r\n\r\n");
	return(0);
}








int main(int argc, char** argv)
{
    //std::cout << "Hello World!\n";


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file



	if (argv[1] == NULL)
	{   
		printf("SFTP_ACC 20210609   V1.1  \r\n");
		printf("SFTP_ACC Get_Acc          \r\n");
		printf("SFTP_ACC Get_Acc_Ctl      \r\n");
		printf("SFTP_ACC CURL_SFTP        \r\n");

		return 0;
	}


	FILE* fd1;
	char sFile1[50] = { 0x00 };
	FILE* fd1_memo;
	char sFile1_memo[50] = { 0x00 };
	char sSys1[200] = { 0x00 };
	char sYYYYMMDD[50] = { 0x00 };

	funRead_set();


	//1. Get_Acc  (大小寫有差)

	if (strcmp(argv[1], "Get_Acc") == 0)
	{
		time_t now;
		struct tm newTime;
		time(&now);
		errno_t errNoT = localtime_s(&newTime, &now);

		sprintf_s(sYYYYMMDD, sizeof(sYYYYMMDD), "ACC-OTS-%04d%02d%02d.csv", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday);

		sprintf_s(sFile1, sizeof(sFile1), "%s.bat", argv[1]);
		sprintf_s(sFile1_memo, sizeof(sFile1_memo), "%s_memo.bat", argv[1]);

		printf("sFile1      = [%s]\r\n", sFile1);
		printf("sFile1_memo = [%s]\r\n", sFile1_memo);

		//1.刪檔 Get_Acc.bat
		sprintf_s(sSys1, sizeof(sSys1), "del %s", sFile1);
		printf("system() sSys1=[%s]    \r\n", sSys1);
		system_new(sSys1);


		//2.建檔 Get_Acc.bat
		errno_t myErrNoT = fopen_s(&fd1, sFile1, "w");
		if (myErrNoT != 0)
		{
			printf("myErrNoT != 0    \r\n");
			return(-1);
		}
		else
		{
			//-W -s , -h -1    -s ,  用, 隔;			-W 移除句尾空格;				- h 標頭 - 1 即可
			//sqlcmd -S conf_SQL_Server -U user -P pwd -i Get_Acc.SQL -o ACC-OTS-20210422.csv
			sprintf_s(sSys1, sizeof(sSys1), "sqlcmd -S %s -U %s -P %s -i Get_Acc.SQL -W -s , -h -1 -o .\\log\\%s \r\n", conf_SQL_Server, conf_SQLID, conf_SQLPW, sYYYYMMDD);
			printf("sSys1=[%s]    \r\n", sSys1);
			fputs(sSys1, fd1);
		}
		fclose(fd1);


		//3.刪 Get_Acc_memo.bat 
		sprintf_s(sSys1, sizeof(sSys1), "del %s", sFile1_memo);
		printf("system() sSys1=[%s]    \r\n", sSys1);
		system_new(sSys1);

		//4.建 Get_Acc_memo.bat 檔
		errno_t myErrNoT2 = fopen_s(&fd1_memo, sFile1_memo, "w");
		if (myErrNoT2 != 0)
		{
			printf("myErrNoT2 != 0    \r\n");
			return(-2);
		}
		else
		{
			printf("myErrNoT2 == 0    \r\n");
			//sqlcmd -S SQL_Server -U user -P pwd -i Get_Acc.SQL -o ACC-OTS-20210422.csv
			sprintf_s(sSys1, sizeof(sSys1), "sqlcmd -S %s -U %s -P %s -i Get_Acc.SQL -W -s , -h -1 -o .\\log\\%s \r\n", conf_SQL_Server, conf_SQLID, "****", sYYYYMMDD);
			printf("sSys1=[%s]    \r\n", sSys1);

			fputs(sSys1, fd1_memo);
		}
		fclose(fd1_memo);

		return 0;

	} // if (strcmp(argv[1], "Get_Acc") == 0)


	//2. Get_Acc_Ctl  (大小寫有差)

	if (strcmp(argv[1], "Get_Acc_Ctl") == 0)
	{
		time_t now;
		struct tm newTime;
		time(&now);
		errno_t errNoT = localtime_s(&newTime, &now);

		sprintf_s(sYYYYMMDD, sizeof(sYYYYMMDD), "ACC-OTS-%04d%02d%02d-Ctl.csv", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday);

		sprintf_s(sFile1, sizeof(sFile1), "%s.bat", argv[1]);
		sprintf_s(sFile1_memo, sizeof(sFile1_memo), "%s_memo.bat", argv[1]);

		printf("sFile1      = [%s]\r\n", sFile1);
		printf("sFile1_memo = [%s]\r\n", sFile1_memo);

		//1.刪檔 Get_Acc_Ctl.bat
		sprintf_s(sSys1, sizeof(sSys1), "del %s", sFile1);
		printf("system() sSys1=[%s]    \r\n", sSys1);
		system_new(sSys1);


		//2.建檔 Get_Acc_Ctl.bat
		errno_t myErrNoT = fopen_s(&fd1, sFile1, "w");
		if (myErrNoT != 0)
		{
			printf("myErrNoT != 0    \r\n");
			return(-1);
		}
		else
		{
			//-W -s , -h -1    -s ,  用, 隔;			-W 移除句尾空格;				- h 標頭 - 1 即可
			//sqlcmd -S conf_SQL_Server -U user -P pwd -i Get_Acc.SQL -o ACC-OTS-20210422_Ctl.csv
			sprintf_s(sSys1, sizeof(sSys1), "sqlcmd -S %s -U %s -P %s -i Get_Acc_Ctl.SQL -W -s , -h -1 -o .\\log\\%s \r\n", conf_SQL_Server, conf_SQLID, conf_SQLPW, sYYYYMMDD);
			printf("sSys1=[%s]    \r\n", sSys1);
			fputs(sSys1, fd1);
		}
		fclose(fd1);


		//3.刪 Get_Acc_Ctl_memo.bat 
		sprintf_s(sSys1, sizeof(sSys1), "del %s", sFile1_memo);
		printf("system() sSys1=[%s]    \r\n", sSys1);
		system_new(sSys1);

		//4.建 Get_Acc_Ctl_memo.bat 檔
		errno_t myErrNoT2 = fopen_s(&fd1_memo, sFile1_memo, "w");
		if (myErrNoT2 != 0)
		{
			printf("myErrNoT2 != 0    \r\n");
			return(-2);
		}
		else
		{
			printf("myErrNoT2 == 0    \r\n");
			//sqlcmd -S SQL_Server -U user -P pwd -i Get_Acc.SQL -o ACC-OTS-20210422-ctl.csv
			sprintf_s(sSys1, sizeof(sSys1), "sqlcmd -S %s -U %s -P %s -i Get_Acc_Ctl.SQL -W -s , -h -1 -o .\\log\\%s \r\n", conf_SQL_Server, conf_SQLID, "****", sYYYYMMDD);
			printf("sSys1=[%s]    \r\n", sSys1);

			fputs(sSys1, fd1_memo);
		}
		fclose(fd1_memo);

		return 0;

	} // if (strcmp(argv[1], "Get_Acc_Ctl") == 0)



	// 20210609   V1.1   ching2      add  conf_SQL_Server, conf_sftp_ip, conf_sftp

	//3. CRUL_SFTP  (大小寫有差)

	if (strcmp(argv[1], "CURL_SFTP") == 0)
	{
		time_t now;
		struct tm newTime;
		time(&now);
		errno_t errNoT = localtime_s(&newTime, &now);


		sprintf_s(sFile1, sizeof(sFile1), "%s.bat", argv[1]);
		sprintf_s(sFile1_memo, sizeof(sFile1_memo), "%s_memo.bat", argv[1]);

		printf("sFile1      = [%s]\r\n", sFile1);
		printf("sFile1_memo = [%s]\r\n", sFile1_memo);

		//1.刪檔 CURL_SFTP.bat
		sprintf_s(sSys1, sizeof(sSys1), "del %s", sFile1);
		printf("system() sSys1=[%s]    \r\n", sSys1);
		system_new(sSys1);


		//2.建檔 CURL_FTP.bat
		errno_t myErrNoT = fopen_s(&fd1, sFile1, "w");
		if (myErrNoT != 0)
		{
			printf("myErrNoT != 0    \r\n");
			return(-1);
		}
		else
		{
	
			//1111 
			//curl.exe - m 60 - T ".\\log\\ACC-OTS-%date:~0,4%%date:~5,2%%date:~8,2%.csv"      sftp://127.0.0.1/C:/Users/ots/ -u OTS:1qaz@WSX -k   
			sprintf_s(sYYYYMMDD, sizeof(sYYYYMMDD), "ACC-OTS-%04d%02d%02d.csv", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday);

			sprintf_s(sSys1, sizeof(sSys1), "curl.exe -m 60 -T \".\\log\\%s\" %s -u %s -k \r\n", sYYYYMMDD, conf_sftp_ip, conf_sftp);
			printf("sSys1=[%s]    \r\n", sSys1);
			fputs(sSys1, fd1);

			//2222 
			//curl.exe - m 60 - T ".\\log\\ACC-OTS-%date:~0,4%%date:~5,2%%date:~8,2%-ctl.csv"  sftp ://127.0.0.1/C:/Users/ots/ -u OTS:1qaz@WSX -k   
			sprintf_s(sYYYYMMDD, sizeof(sYYYYMMDD), "ACC-OTS-%04d%02d%02d-ctl.csv", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday);

			sprintf_s(sSys1, sizeof(sSys1), "curl.exe -m 60 -T \".\\log\\%s\" %s -u %s -k \r\n", sYYYYMMDD, conf_sftp_ip, conf_sftp);
			printf("sSys1=[%s]    \r\n", sSys1);
			fputs(sSys1, fd1);


		}
		fclose(fd1);





		//3.刪 CURL_SFTP_memo.bat 
		sprintf_s(sSys1, sizeof(sSys1), "del %s", sFile1_memo);
		printf("system() sSys1=[%s]    \r\n", sSys1);
		system_new(sSys1);

		//4.建 CURL_SFTP_memo.bat 檔
		errno_t myErrNoT2 = fopen_s(&fd1_memo, sFile1_memo, "w");
		if (myErrNoT2 != 0)
		{
			printf("myErrNoT2 != 0    \r\n");
			return(-2);
		}
		else
		{
			printf("myErrNoT2 == 0    \r\n");
			
			//1111 
			//curl.exe - m 60 - T ".\\log\\ACC-OTS-%date:~0,4%%date:~5,2%%date:~8,2%.csv"      sftp://127.0.0.1/C:/Users/ots/ -u OTS:1qaz@WSX -k   
			sprintf_s(sYYYYMMDD, sizeof(sYYYYMMDD), "ACC-OTS-%04d%02d%02d.csv", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday);

			sprintf_s(sSys1, sizeof(sSys1), "curl.exe -m 60 -T \".\\log\\%s\" %s -u %s -k \r\n", sYYYYMMDD, conf_sftp_ip, "***:*****");
			printf("sSys1=[%s]    \r\n", sSys1);
			fputs(sSys1, fd1_memo);

			//2222 
			//curl.exe - m 60 - T ".\\log\\ACC-OTS-%date:~0,4%%date:~5,2%%date:~8,2%-ctl.csv"  sftp ://127.0.0.1/C:/Users/ots/ -u OTS:1qaz@WSX -k   
			sprintf_s(sYYYYMMDD, sizeof(sYYYYMMDD), "ACC-OTS-%04d%02d%02d-ctl.csv", newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday);

			sprintf_s(sSys1, sizeof(sSys1), "curl.exe -m 60 -T \".\\log\\%s\" %s -u %s -k \r\n", sYYYYMMDD, conf_sftp_ip, "***:*****");
			printf("sSys1=[%s]    \r\n", sSys1);
			fputs(sSys1, fd1_memo);


		}
		fclose(fd1_memo);

		return 0;

	} // if (strcmp(argv[1], "CURL_SFTP") == 0)

}


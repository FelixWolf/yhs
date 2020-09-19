#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "yhs.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef WIN32

// Windows
#define _WIN32_WINNT 0x500
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <shlwapi.h>
#include <direct.h>
#include <conio.h>

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__

// Mac/iBlah
#include <unistd.h>

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef __linux__
#include <cstring>
#include <ctype.h>
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <getopt.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static bool g_quit=false;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static void Log(yhsLogCategory cat,const char *str,void *context)
{
	(void)context;

	switch(cat)
	{
	case YHS_LOG_ERROR:
		fputs(str,stderr);
		break;

	case YHS_LOG_INFO:
		fputs(str,stdout);
		break;

	case YHS_LOG_DEBUG:
		fputs(str,stdout);
		break;
	}
	fputs("\n", stdout);

#ifdef _WIN32
	OutputDebugStringA(str);
#endif//_WIN32
}

#ifdef WIN32

static void WaitForKey()
{
	if(IsDebuggerPresent())
	{
		if(!g_quit)
		{
			fprintf(stderr,"press enter to exit.\n");
			getchar();
		}
	}
}
#endif

int main(int argc,char *argv[])
{
#ifdef WIN32
    atexit(&WaitForKey);

    WSADATA wd;
    if(WSAStartup(MAKEWORD(2,2),&wd)!=0)
    {
        fprintf(stderr,"FATAL: failed to initialize Windows Sockets.\n");
        return EXIT_FAILURE;
    }

	printf("Press Esc to finish.\n");
#endif

#ifdef _MSC_VER
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
//	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_CHECK_ALWAYS_DF);
#endif
	bool verbose = false;
	bool debug = false;
    
    char directory[1000];
    getcwd(directory, sizeof directory);
    
    u_int16_t port = 80;
    char sname[256] = "Yocto HTTP server";
    
    int c;
    while(true){
        static struct option long_options[] = {
            {"verbose", no_argument,        0, 'v'},
            {"debug",   no_argument,        0, 'q'},
            {"dir",     required_argument,  0, 'd'},
            {"port",    required_argument,  0, 'p'},
            {"name",    required_argument,  0, 'n'},
            {"help",    no_argument,        0, 'h'},
            {0, 0, 0, 0}
        };
        int option_index = 0;

        c = getopt_long(argc, argv, "vqd:p:n:h", long_options, &option_index);

        if (c == -1) break;

        switch (c){
            case 'v':
                verbose = true;
            break;
            
            case 'q':
                debug = true;
            break;

            case 'd':
                strcat(directory, optarg);
            break;

            case 'p':
                port = atoi(optarg);
            break;

            case 'n':
                strcat(sname, optarg);
            break;

            case 'h':
                printf(
                    "–num <n>:           Set number of program\n"
                    "–beep:              Beep the user\n"
                    "–sigma <val>:       Set sigma of program\n"
                    "–writeFile <fname>: File to write to\n"
                    "–help:              Show help\n"
                );
                exit(1);
            break;
        }
    }
    
    yhsServer *server=yhs_new_server(port);
    
    if(!server)
    {
        fprintf(stderr,"FATAL: failed to start server.\n");
        return EXIT_FAILURE;
    }

	yhs_set_server_name(server,sname);

	yhs_set_server_log_callback(server,&Log,0);
	yhs_set_server_log_enabled(server,YHS_LOG_INFO,verbose);
	yhs_set_server_log_enabled(server,YHS_LOG_DEBUG,debug);
    
    yhs_add_to_toc(yhs_add_res_path_handler(server,"/",&yhs_file_server_handler,directory));

    while(!g_quit)
    {
        yhs_update(server);
        
#ifdef WIN32
        Sleep(10);

		if(_kbhit())
		{
			if(_getch()==27)
			{
				g_quit=true;
				break;
			}
		}

#else
        usleep(10000);
#endif
    }

	yhs_delete_server(server);
	server=0;

// #ifdef _MSC_VER
// 	_CrtDumpMemoryLeaks();
// #endif

	return EXIT_SUCCESS;
}

#include "Log.h"

#include <Euphorbe/Core/Common.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

void LogInfo(const char* message)
{
    time_t now;
    time(&now);
    printf(RESET);
    printf(GREEN);
    char* time_string = ctime(&now);
    time_string[strcspn(time_string, "\n")] = 0;
    printf("%s [EUPHORBE] : %s\n", time_string, message);
    printf(RESET);
}

void LogWarn(const char* message)
{   
    time_t now;
    time(&now);
    printf(RESET);
    printf(YELLOW);
    char* time_string = ctime(&now);
    time_string[strcspn(time_string, "\n")] = 0;
    printf("%s [EUPHORBE] : %s\n", time_string, message);
    printf(RESET);
}

void LogError(const char* message)
{
    time_t now;
    time(&now);
    printf(RESET);
    printf(RED);
    char* time_string = ctime(&now);
    time_string[strcspn(time_string, "\n")] = 0;
    printf("%s [EUPHORBE] : %s\n", time_string, message);
    printf(RESET);
}

void E_LogInfo(const char* message, ...)
{
    char buf[100];    
    va_list vl;
    va_start(vl, message);

    vsnprintf(buf, sizeof(buf), message, vl);

    va_end(vl);

    LogInfo(buf);
}

void E_LogWarn(const char* message, ...)
{
    char buf[100];
    va_list vl;
    va_start(vl, message);

    vsnprintf(buf, sizeof(buf), message, vl);

    va_end(vl);

    LogWarn(buf);
}

void E_LogError(const char* message, ...)
{
    char buf[100];
    va_list vl;
    va_start(vl, message);

    vsnprintf(buf, sizeof(buf), message, vl);

    va_end(vl);

    LogError(buf);
}
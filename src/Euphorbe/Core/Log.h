#ifndef EUPHORBE_LOG_H
#define EUPHORBE_LOG_H

// Print a message in green to the console
void E_LogInfo(char* message, ...);

// Print a message in yellow to the console
void E_LogWarn(char* message, ...);

// Print a message in red to the console
void E_LogError(char* message, ...);

#endif
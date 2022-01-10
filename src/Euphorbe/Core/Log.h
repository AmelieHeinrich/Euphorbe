#pragma once

#include <cimgui.h>
#include <Euphorbe/Core/Common.h>

typedef struct E_LogGUI E_LogGUI;
struct E_LogGUI
{
	ImGuiTextBuffer text_buffer;
	b32 scroll_to_bottom;
};

extern E_LogGUI log_gui;

// Print a message in green to the console
void E_LogInfo(const char* message, ...);

// Print a message in yellow to the console
void E_LogWarn(const char* message, ...);

// Print a message in red to the console
void E_LogError(const char* message, ...);

// Draw the Log GUI
void E_LogDraw();
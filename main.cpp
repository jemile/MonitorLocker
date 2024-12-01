#include <windows.h>
#include <iostream>
#include <vector>
#include "main.h"

bool attached = false;
bool printed = false;
int monitor = 0;

struct MonitorData
{
	HMONITOR hMonitor;
	RECT rect;
};

// https://learn.microsoft.com/en-us/previous-versions/windows/embedded/ms932091(v=msdn.10)
// If hdcMonitor is NULL, this rectangle is the screen rectangle, and the rectangle coordinates are virtual screen coordinates.
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    auto* monitors = reinterpret_cast<std::vector<MonitorData>*>(dwData);
    monitors->push_back({ hMonitor, *lprcMonitor });
    // TRUE specifies that you want to continue enumerating screens. FALSE specifies that you want to stop the enumerating screens.
    return TRUE; 
}

void LockCursorToMonitor(const RECT& monitorRect) {
    if (ClipCursor(&monitorRect)) {
        attached = true;
    }
    else {
        attached = false;
    }
}

int main() {
    std::vector<MonitorData> monitors;
    // EnumDisplayMonitors calls a specified MonitorEnumProc function once for each screen in the calculated enumeration set. EnumDisplayMonitors always passes a handle to the screen to MonitorEnumProc.
    if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors))) {
        std::cerr << "Failed to enumerate monitors.\n";
        return 1;
    }

    if (monitors.size() < 2) {
        std::cerr << "Less than two monitors detected.\n";
        return 1;
    }

    std::cout << "Which monitor would you like to lock to? (0 is your first monitor)\n";
    std::cin >> monitor;
   
    if (monitor + 1 > monitors.size() || monitor < 0)
    {
        std::cout << "You do not have a monitor at the value: " << monitor << "\n";
        return 1;
    }


    std::cout << "Press Escape to close\n";
    while (true) {
        LockCursorToMonitor(monitors[monitor].rect);
        if (!printed && attached)
        {
            std::cout << "Cursor locked to the monitor (" << monitor << ") successfully.\n";
            printed = true;
        }
        else if (!printed && !attached)
        {
            std::cout << "Cursor could not be locked to monitor (" << monitor << ")\n";
            printed = true;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
    }

    ClipCursor(NULL);

    return 0;
}
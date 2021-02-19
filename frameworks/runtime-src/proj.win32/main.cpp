#include "main.h"
#include "SimulatorWin.h"
#include <shellapi.h>

int WINAPI _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
#ifdef USE_WIN32_CONSOLE
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
    auto simulator = SimulatorWin::getInstance();
    return simulator->run();
#ifdef USE_WIN32_CONSOLE
	FreeConsole();
#endif
}

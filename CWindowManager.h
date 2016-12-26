#ifndef SOURCE_WINDOWMANAGER_H_
#define SOURCE_WINDOWMANAGER_H_

#include <windows.h>

class CWindowManager {
private:
	static HWND m_hWnd;
	static HDC m_hDC;
	static HGLRC m_hRC;
	static bool m_bQuit;
	static bool m_bFullscreen;

	static int m_iClientWidth;
	static int m_iClientHeight;

	static void (*KeyPressCallback)(int);
	static void (*KeyReleaseCallback)(int);
	static void (*MousePressedCallback)(int);
	static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
public:
	static HWND GetWindowHandle();
	static HDC GetWindowDC();
	static HGLRC GetWindowRC();
	static bool GetKeyState(int vKey);

	static void setKeyPressCallback(void (*)(int));
	static void setKeyReleaseCallback(void (*)(int));
	static void setMousePressedCallback(void (*)(int));

	static bool Create(int Width, int Height, const char* Title, bool Fullscreen = false, LPSTR Icon = IDI_APPLICATION);
	static void Destroy();
	static void ClearBuffers();
	static bool SwitchBuffers();

	static bool HandleMessages();	// If there is message in the queue, it will handle it and return true
	static void SendQuitMessage();
	static bool IsQuitMessage();

	static int GetClientWidth();
	static int GetClientHeight();

	static void CentreMouse();
	static int* GetMouseDisplacementFromCentre();
	static int* GetMousePosition();
	static void ShowMouse(bool Show);
	static bool GetFocused();

	static void Error(const char* Title, const char* Text);
};

#endif /* SOURCE_WINDOWMANAGER_H_ */

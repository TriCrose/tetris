#include "CWindowManager.h"

#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>

HWND CWindowManager::m_hWnd = NULL;
HDC CWindowManager::m_hDC = NULL;
HGLRC CWindowManager::m_hRC = NULL;
bool CWindowManager::m_bQuit = false;
bool CWindowManager::m_bFullscreen;

int CWindowManager::m_iClientWidth;
int CWindowManager::m_iClientHeight;

void (*CWindowManager::KeyPressCallback)(int vKey) = NULL;
void (*CWindowManager::KeyReleaseCallback)(int vKey) = NULL;
void (*CWindowManager::MousePressedCallback)(int MouseButton) = NULL;

LRESULT CWindowManager::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_SYSCOMMAND:
		switch (wParam) {
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			break;
		}
		break;
	case WM_KEYDOWN:
		if (KeyPressCallback != NULL) (*KeyPressCallback)(wParam);
		break;
	case WM_KEYUP:
		if (KeyReleaseCallback != NULL) (*KeyReleaseCallback)(wParam);
		break;
    case WM_LBUTTONDOWN:
        if (MousePressedCallback != NULL) (*MousePressedCallback)(0);
        break;
    case WM_RBUTTONDOWN:
        if (MousePressedCallback != NULL) (*MousePressedCallback)(1);
        break;
    case WM_MOUSEWHEEL:
        if ((short) HIWORD(wParam) > 0) {
            if (MousePressedCallback != NULL) (*MousePressedCallback)(2);
        } else {
            if (MousePressedCallback != NULL) (*MousePressedCallback)(3);
        }
        break;
	case WM_CLOSE:
		m_bQuit = true;
		break;
	case WM_DESTROY:
		m_bQuit = true;
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CWindowManager::GetWindowHandle() {
	return m_hWnd;
}

HDC CWindowManager::GetWindowDC() {
	return m_hDC;
}

HGLRC CWindowManager::GetWindowRC() {
	return m_hRC;
}

bool CWindowManager::GetKeyState(int vKey) {
	return GetAsyncKeyState(vKey) && (GetFocus() == m_hWnd);
}

void CWindowManager::setKeyPressCallback(void (*Callback)(int vKey)) {
	KeyPressCallback = Callback;
}

void CWindowManager::setKeyReleaseCallback(void (*Callback)(int vKey)) {
	KeyReleaseCallback = Callback;
}

void CWindowManager::setMousePressedCallback(void (*Callback)(int MouseButton)) {
	MousePressedCallback = Callback;
}

bool CWindowManager::Create(int Width, int Height, const char* Title, bool Fullscreen, LPSTR Icon) {
    m_bFullscreen = Fullscreen;

	WNDCLASSEX WindowClass = {
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		WindowProcedure, 0, 0, GetModuleHandle(NULL),
		LoadIcon(GetModuleHandle(0), Icon),
		LoadCursor(NULL, IDC_ARROW),
		CreateSolidBrush(0), NULL, "WC",
		LoadIcon(GetModuleHandle(0), Icon)
	};

	DWORD dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	if (Fullscreen) {
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = Width;
		dmScreenSettings.dmPelsHeight = Height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) MessageBox(NULL, "The fullscreen mode you selected is not supported. Switching to windowed mode.", "Error", MB_OK | MB_ICONERROR);
		else {
			dwStyle = WS_POPUP;
			dwExStyle = WS_EX_APPWINDOW;
		}
	}

	if (!RegisterClassEx(&WindowClass)) return false;
	m_hWnd = CreateWindowEx(dwExStyle, "WC", Title, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, HWND_DESKTOP, NULL, GetModuleHandle(NULL), NULL);
	if (m_hWnd == NULL) return false;

	m_hDC = GetDC(m_hWnd);
	if (m_hDC == NULL) return false;
	ShowWindow(m_hWnd, SW_SHOW);
	ShowCursor(true);

	RECT ClientRect;
	GetClientRect(m_hWnd, &ClientRect);
	m_iClientWidth = (int)(ClientRect.right - ClientRect.left);
	m_iClientHeight = (int)(ClientRect.bottom - ClientRect.top);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,						// Version, always set to 1
		PFD_DRAW_TO_WINDOW |	// Support window
		PFD_SUPPORT_OPENGL |	// Support OpenGL
		PFD_DOUBLEBUFFER,		// Support double buffering
		PFD_TYPE_RGBA,			// RGBA colour mode
		32,						// Colour depth (if not supported by graphics card, it will choose the highest)
		0, 0, 0, 0, 0, 0,		// Ignore colour bits
		0, 0, 0,				// No alpha buffer, ignore shift bit, no accumulation buffer
		0, 0, 0, 0,				// Ignore accumulation bits
		24,						// 16-bit z-buffer size
		0, 0,					// No stencil or auxiliary buffer
		PFD_MAIN_PLANE,			// Main drawing plane
		0,						// Reserved
		0, 0, 0					// Layer masks ignored
	};

	SetPixelFormat(m_hDC, ChoosePixelFormat(m_hDC, &pfd), &pfd);
	m_hRC = wglCreateContext(m_hDC);
	if (m_hRC == NULL) return false;
	wglMakeCurrent(m_hDC, m_hRC);
	return true;
}

void CWindowManager::Destroy() {
	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hRC);
	ReleaseDC(m_hWnd, m_hDC);
	UnregisterClass("WC", GetModuleHandle(0));
	DestroyWindow(m_hWnd);
}

void CWindowManager::ClearBuffers() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool CWindowManager::SwitchBuffers() {
	return SwapBuffers(CWindowManager::GetWindowDC());
}

bool CWindowManager::HandleMessages() {
	static MSG WindowMessage;
	if (PeekMessage(&WindowMessage, NULL, 0, 0, PM_REMOVE)) {
		if (WindowMessage.message == WM_QUIT) m_bQuit = true;
		TranslateMessage(&WindowMessage);
		DispatchMessage(&WindowMessage);
		return true;
	} else {
		return false;
	}
}

void CWindowManager::SendQuitMessage() {
	m_bQuit = true;
}

bool CWindowManager::IsQuitMessage() {
	return m_bQuit;
}

int CWindowManager::GetClientWidth() {
	return m_iClientWidth;
}

int CWindowManager::GetClientHeight() {
	return m_iClientHeight;
}

void CWindowManager::Error(const char* Title, const char* Text) {
	MessageBox(m_hWnd, Text, Title, MB_OK | MB_ICONERROR);
}

void CWindowManager::CentreMouse() {
    if (!GetFocus()) return;
	static RECT rect;
	GetWindowRect(m_hWnd, &rect);
	SetCursorPos(m_iClientWidth/2 + rect.left, m_iClientHeight/2 + rect.top);
}

int* CWindowManager::GetMouseDisplacementFromCentre() {
	static int disp[2];
	static POINT p;
	static RECT rect;
	GetCursorPos(&p);
	GetWindowRect(m_hWnd, &rect);
	disp[0] = p.x - (m_iClientWidth/2 + rect.left);
	disp[1] = (m_iClientHeight/2 + rect.top) - p.y;
	return disp;
}

int* CWindowManager::GetMousePosition() {
    static int disp[2];
	static POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_hWnd, &p);
	disp[0] = p.x;
	disp[1] = p.y;
	return disp;
}

void CWindowManager::ShowMouse(bool Show) {
	if (Show) while (ShowCursor(true) < 0) ShowCursor(true);
	else while (ShowCursor(false) >= 0) ShowCursor(false);
}

bool CWindowManager::GetFocused() {
    return m_hWnd == GetActiveWindow() && m_hWnd == GetFocus();
}

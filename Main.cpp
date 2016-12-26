#include <time.h>

#define GLEW_STATIC
#include <glew/glew.h>
#include <gl/gl.h>

#include "Resource.h"

#include "CWindowManager.h"
#include "CResourceManager.h"
#include "CRenderer.h"
#include "CUIText.h"
#include "CCamera.h"

#define MOUSE_SENSITIVITY 0.07f

void KeyPress(int vKey);
void MousePressed(int button);
void RotateBlocksBasedOnYaw(float angle);
void Reset();

CGrid grid;
CCamera *camera;
LARGE_INTEGER TicksPerSecond, StartTime, EndTime;
std::vector<CUIText*> UIs;
int CountdownTimer;

struct ScreenResolution {
    short Width;
    short Height;
};

INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
std::vector<ScreenResolution> GetAllFullscreenResolutions();
std::vector<ScreenResolution> PossibleResolutions;
bool CloseApplication = false;
bool Fullscreen = true;
int WindowWidth = 800, WindowHeight = 600;

bool lButtonDown = false;
bool rButtonDown = false;
float theta;
int GameStage = 0;
bool GamePaused = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(time(NULL));

    PossibleResolutions = GetAllFullscreenResolutions();
    WindowWidth = PossibleResolutions.back().Width;
    WindowHeight = PossibleResolutions.back().Height;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), 0, DialogProc);
	if (CloseApplication) return 0;

	CWindowManager::Create(WindowWidth, WindowHeight, "Tetris³", Fullscreen, MAKEINTRESOURCE(IDI_TETRISICON));
	CWindowManager::setKeyPressCallback(KeyPress);
	CWindowManager::setMousePressedCallback(MousePressed);
	CWindowManager::ShowMouse(false);
	CWindowManager::CentreMouse();

	SetTextAlign(CWindowManager::GetWindowDC(), TA_CENTER);
	SetBkColor(CWindowManager::GetWindowDC(), RGB(0, 0, 0));
	SetTextColor(CWindowManager::GetWindowDC(), RGB(255, 255, 255));
	TextOut(CWindowManager::GetWindowDC(), CWindowManager::GetClientWidth() / 2, CWindowManager::GetClientHeight() / 2, "Loading...", strlen("Loading..."));

    CRenderer::Initialize(&grid);

    static float TextScale = 1.2f * float(CWindowManager::GetClientHeight())/float(1052);
    CFont* font = new CFont(CResourceManager::LoadFileIntoMemory("alsina100.font"));
    CUIText textPlay(font, "Play", CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2 - TextScale*30, 4, TextScale);
    CUIText textHelp(font, "Help", CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2 - TextScale*150, 4, TextScale);
    CUIText textQuit(font, "Quit", CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2 - TextScale*270, 4, TextScale);
    CUIText textRestart(font, "Restart", CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2 - TextScale*30, 4, TextScale);
    CUIText textQuitToMenu(font, "Quit to Menu", CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2 - TextScale*150, 4, TextScale);
    CUIText textQuitToWindows(font, "Quit to Windows", CWindowManager::GetClientWidth()/2, CWindowManager::GetClientHeight()/2 - TextScale*270, 4, TextScale);
    CUIText textBack(font, "Back", TextScale * 20, 0, 0, TextScale);
    UIs.push_back(&textPlay);
    UIs.push_back(&textHelp);
    UIs.push_back(&textQuit);
    UIs.push_back(&textRestart);
    UIs.push_back(&textQuitToMenu);
    UIs.push_back(&textQuitToWindows);
    UIs.push_back(&textBack);

    CRenderer::LoadResources(
        CResourceManager::LoadTGAFromFile("floor.tga"),
        CResourceManager::LoadTGAFromFile("normalmap.tga"),
        CResourceManager::LoadTGAFromFile("skybox.tga"),
        CResourceManager::LoadTModelFromFile("grid.tmodel"),
        CResourceManager::LoadTModelFromFile("block.tmodel"),
        CResourceManager::LoadTModelFromFile("marker.tmodel"),
        CResourceManager::LoadFileIntoMemory("alsina100.font"),
        CResourceManager::LoadTGAFromFile("background.tga"),
        CResourceManager::LoadTGAFromFile("helpimage.tga"),
        CResourceManager::LoadTGAFromFile("title.tga"),
        UIs
    );

    CRenderer::LoadBlockTextureImages(
        CResourceManager::LoadTGAFromFile("blocks/cyan.tga"),
        CResourceManager::LoadTGAFromFile("blocks/blue.tga"),
        CResourceManager::LoadTGAFromFile("blocks/orange.tga"),
        CResourceManager::LoadTGAFromFile("blocks/yellow.tga"),
        CResourceManager::LoadTGAFromFile("blocks/green.tga"),
        CResourceManager::LoadTGAFromFile("blocks/magenta.tga"),
        CResourceManager::LoadTGAFromFile("blocks/red.tga")
    );

    camera = new CCamera();
    theta = 3.14159/2.0f;
    QueryPerformanceFrequency(&TicksPerSecond);
    QueryPerformanceCounter(&StartTime);
    grid.Tick(0);

	while (!CWindowManager::IsQuitMessage()) {
        CWindowManager::HandleMessages();
        CWindowManager::ClearBuffers();
        if (CWindowManager::GetWindowHandle() != GetActiveWindow() || CWindowManager::GetWindowHandle() != GetForegroundWindow()) {
            if (Fullscreen) ShowWindow(CWindowManager::GetWindowHandle(), SW_MINIMIZE);
        }

        if (GameStage == 0) {
            CWindowManager::ShowMouse(true);
            GamePaused = false;
            CRenderer::SetPaused(false);
            CRenderer::RenderTitleScreen(false);
            for (unsigned i = 0; i < 3; i++) UIs[i]->Update(CWindowManager::GetMousePosition()[0], CWindowManager::GetMousePosition()[1]);
        } else if (GameStage == 3) {
            CWindowManager::ShowMouse(true);
            GamePaused = false;
            CRenderer::SetPaused(false);
            CRenderer::RenderTitleScreen(true);
            UIs[6]->Update(CWindowManager::GetMousePosition()[0], CWindowManager::GetMousePosition()[1]);
        } else if (GameStage == 2 || GameStage == 1) {
            if (GamePaused || grid.GetGameOver() || GameStage == 1) CWindowManager::ShowMouse(true);
            else CWindowManager::ShowMouse(false);

            if (GameStage == 2 && (GamePaused || grid.GetGameOver())) for (unsigned i = 3; i < 6; i++) UIs[i]->Update(CWindowManager::GetMousePosition()[0], CWindowManager::GetMousePosition()[1]);
            if (GameStage == 2 && !CWindowManager::GetFocused() && !grid.GetGameOver()) {
                GamePaused = true;
                CRenderer::SetPaused(true);
            }

            if (!GamePaused && !grid.GetGameOver()) {
                // Handle various inputs
                static bool spaceDown = false;
                if (CWindowManager::GetKeyState(VK_SPACE) && !spaceDown) {
                    grid.SetFallFaster(true);
                    spaceDown = true;
                } else if (!CWindowManager::GetKeyState(VK_SPACE)) {
                    grid.SetFallFaster(false);
                    spaceDown = false;
                }

                if (CWindowManager::GetKeyState(VK_LBUTTON) && !lButtonDown) {
                    RotateBlocksBasedOnYaw(90.0f);
                    lButtonDown = true;
                } else if (!CWindowManager::GetKeyState(VK_LBUTTON)) lButtonDown = false;
                if (CWindowManager::GetKeyState(VK_RBUTTON) && !rButtonDown) {
                    RotateBlocksBasedOnYaw(-90.0f);
                    rButtonDown = true;
                } else if (!CWindowManager::GetKeyState(VK_RBUTTON)) rButtonDown = false;

                if (GameStage == 2) theta += MOUSE_SENSITIVITY * 0.07 * (float) CWindowManager::GetMouseDisplacementFromCentre()[0];
                camera->SetPosition(glm::vec3(12.5f * cos(theta), camera->GetPosition().y, 12.5f * sin(theta)));
                if (GameStage == 2) camera->Translate(glm::vec3(0.0f, -MOUSE_SENSITIVITY * (float) CWindowManager::GetMouseDisplacementFromCentre()[1], 0.0f));
                if (camera->GetPosition().y > 16.0f) camera->SetPosition(glm::vec3(camera->GetPosition().x, 16.0f,camera->GetPosition().z));
                if (camera->GetPosition().y < 2.0f) camera->SetPosition(glm::vec3(camera->GetPosition().x, 2.0f,camera->GetPosition().z));
                camera->LookAt(glm::vec3(0.0f, 5.0f, 0.0f));
            }
            CRenderer::RenderAll(camera);
            if (GameStage == 1) {
                CRenderer::RenderCountdown(CountdownTimer);
            } else {
                if (GamePaused) CRenderer::RenderOverlayScreen(false);
                else if (grid.GetGameOver()) CRenderer::RenderOverlayScreen(true);
            }

            static int ms;
            QueryPerformanceCounter(&EndTime);
            ms = int(float (EndTime.QuadPart - StartTime.QuadPart) * 1000.0f / (float) TicksPerSecond.QuadPart);
            if (GameStage == 1) {
                CountdownTimer -= ms;
                if (CountdownTimer <= 0) {
                    GameStage = 2;
                    CRenderer::SetPaused(false);
                }
            }
            StartTime = EndTime;
            if (!GamePaused && !grid.GetGameOver() && GameStage == 2) {
                grid.Tick(ms);
                CWindowManager::CentreMouse();
            }
        }

		CWindowManager::SwitchBuffers();
		Sleep(1);
	}

    delete camera;
    delete font;
    CRenderer::CleanUp();
	CWindowManager::Destroy();
	return 0;
}

void KeyPress(int vKey) {
    if (vKey == VK_ESCAPE && GameStage == 2 && !grid.GetGameOver()) {
        GamePaused ^= 1;
        CRenderer::SetPaused(GamePaused);
        if (!GamePaused) {
            CWindowManager::CentreMouse();
            lButtonDown = true;
            rButtonDown = true;
        }
    } else if (vKey == VK_ESCAPE && GameStage == 3) GameStage = 0;
    if (GameStage == 2 && !GamePaused && !grid.GetGameOver()) {
        switch (vKey) {
        case 'W':
            if (camera->GetYaw() <= 45.0f || camera->GetYaw() > 315.0f) grid.MoveBlocksVertical(1);
            else if (camera->GetYaw() > 45.0f && camera->GetYaw() <= 135.0f) grid.MoveBlocksHorizontal(1);
            else if (camera->GetYaw() > 135.0f && camera->GetYaw() <= 225.0f) grid.MoveBlocksVertical(-1);
            else if (camera->GetYaw() > 225.0f && camera->GetYaw() <= 315.0f) grid.MoveBlocksHorizontal(-1);
            break;
        case 'A':
            if (camera->GetYaw() <= 45.0f || camera->GetYaw() > 315.0f) grid.MoveBlocksHorizontal(-1);
            else if (camera->GetYaw() > 45.0f && camera->GetYaw() <= 135.0f) grid.MoveBlocksVertical(1);
            else if (camera->GetYaw() > 135.0f && camera->GetYaw() <= 225.0f) grid.MoveBlocksHorizontal(1);
            else if (camera->GetYaw() > 225.0f && camera->GetYaw() <= 315.0f) grid.MoveBlocksVertical(-1);
            break;
        case 'S':
            if (camera->GetYaw() <= 45.0f || camera->GetYaw() > 315.0f) grid.MoveBlocksVertical(-1);
            else if (camera->GetYaw() > 45.0f && camera->GetYaw() <= 135.0f) grid.MoveBlocksHorizontal(-1);
            else if (camera->GetYaw() > 135.0f && camera->GetYaw() <= 225.0f) grid.MoveBlocksVertical(1);
            else if (camera->GetYaw() > 225.0f && camera->GetYaw() <= 315.0f) grid.MoveBlocksHorizontal(1);
            break;
        case 'D':
            if (camera->GetYaw() <= 45.0f || camera->GetYaw() > 315.0f) grid.MoveBlocksHorizontal(1);
            else if (camera->GetYaw() > 45.0f && camera->GetYaw() <= 135.0f) grid.MoveBlocksVertical(-1);
            else if (camera->GetYaw() > 135.0f && camera->GetYaw() <= 225.0f) grid.MoveBlocksHorizontal(-1);
            else if (camera->GetYaw() > 225.0f && camera->GetYaw() <= 315.0f) grid.MoveBlocksVertical(1);
            break;
        case 'E':
            grid.DropBlock();
            grid.MakeAllStationary();
            break;
        case '1':
            grid.m_iSelectedAxis = 0;
            break;
        case '2':
            grid.m_iSelectedAxis = 1;
            break;
        case '3':
            grid.m_iSelectedAxis = 2;
            break;
        }
    }
}

void MousePressed(int button) {
    if (GameStage == 0) {
        if (button == 0) {
            if (UIs[0]->GetSelected()) {
                Reset();
                GameStage = 1;
                CRenderer::SetPaused(true);
            } else if (UIs[1]->GetSelected()) {
                GameStage = 3;
            } else if (UIs[2]->GetSelected()) CWindowManager::SendQuitMessage();
        }
    } else if (GameStage == 3) {
        if (button == 0 && UIs[6]->GetSelected()) GameStage = 0;
    } else if (GameStage == 2 && !GamePaused && !grid.GetGameOver()) {
        if (button == 2) grid.m_iSelectedAxis--;
        else if (button == 3) grid.m_iSelectedAxis++;
        if (grid.m_iSelectedAxis > 2) grid.m_iSelectedAxis = 2;
        if (grid.m_iSelectedAxis < 0) grid.m_iSelectedAxis = 0;
    } else if (GameStage == 2) {
        if (UIs[3]->GetSelected()) {
            Reset();
            GameStage = 1;
            CRenderer::SetPaused(true);
        } else if (UIs[4]->GetSelected()) GameStage = 0;
        else if (UIs[5]->GetSelected()) CWindowManager::SendQuitMessage();
    }
}

void RotateBlocksBasedOnYaw(float angle) {
    if (grid.m_iSelectedAxis == 1) grid.RotateBlocks(grid.m_iSelectedAxis, angle);
    else {
        if (camera->GetYaw() > 45.0f && camera->GetYaw() <= 135.0f) {
            if (grid.m_iSelectedAxis == 0) grid.RotateBlocks(2, -angle);
            if (grid.m_iSelectedAxis == 2) grid.RotateBlocks(0, angle);
        } else if (camera->GetYaw() > 135.0f && camera->GetYaw() <= 225.0f) {
            if (grid.m_iSelectedAxis == 0) grid.RotateBlocks(0, -angle);
            if (grid.m_iSelectedAxis == 2) grid.RotateBlocks(2, -angle);
        } else if (camera->GetYaw() > 225.0f && camera->GetYaw() <= 315.0f) {
            if (grid.m_iSelectedAxis == 0) grid.RotateBlocks(2, angle);
            if (grid.m_iSelectedAxis == 2) grid.RotateBlocks(0, -angle);
        } else {
            if (grid.m_iSelectedAxis == 0) grid.RotateBlocks(0, angle);
            if (grid.m_iSelectedAxis == 2) grid.RotateBlocks(2, angle);
        }
    }
}

void Reset() {
    lButtonDown = true;
    rButtonDown = true;
    theta = 3.14159/2.0f;
    CWindowManager::CentreMouse();
    GamePaused = false;
    CRenderer::SetPaused(false);
    grid.Reset();
    QueryPerformanceCounter(&EndTime);
    StartTime = EndTime;
    camera->SetPosition(glm::vec3(0.0f, 9.0f, 0.0f));
    CountdownTimer = 1500;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			if (SendMessage(GetDlgItem(hWnd, IDC_FULLSCREEN), BM_GETCHECK, 0, 0) == BST_CHECKED) Fullscreen = true;
			else if (SendMessage(GetDlgItem(hWnd, IDC_FULLSCREEN), BM_GETCHECK, 0, 0) == BST_UNCHECKED) Fullscreen = false;

			int ItemIndex = SendMessage(GetDlgItem(hWnd, IDC_RESOLUTION), CB_GETCURSEL, 0, 0);
            WindowWidth = PossibleResolutions[ItemIndex].Width;
            WindowHeight = PossibleResolutions[ItemIndex].Height;

			EndDialog(hWnd, IDOK);
			break; }
		case IDCANCEL:
            CloseApplication = true;
            EndDialog(hWnd, IDCANCEL);
			break;
		case IDC_FULLSCREEN:
            if (SendMessage(GetDlgItem(hWnd, IDC_FULLSCREEN), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				SendMessage(GetDlgItem(hWnd, IDC_FULLSCREEN), BM_SETCHECK, 0, 0);
            } else if (SendMessage(GetDlgItem(hWnd, IDC_FULLSCREEN), BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
            	SendMessage(GetDlgItem(hWnd, IDC_FULLSCREEN), BM_SETCHECK, 1, 0);
            }
			break;
		}
		return true;
		break;
	case WM_INITDIALOG:
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_TETRISICON)));
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_TETRISICON)));
		for (unsigned i = 0; i < PossibleResolutions.size(); i++) {
			std::string String = ToString(PossibleResolutions[i].Width) + "x" + ToString(PossibleResolutions[i].Height);
			SendMessage(GetDlgItem(hWnd, IDC_RESOLUTION), CB_ADDSTRING, 0, (LPARAM) String.c_str());
			if (PossibleResolutions[i].Width == WindowWidth && PossibleResolutions[i].Height == WindowHeight) {
				SendMessage(GetDlgItem(hWnd, IDC_RESOLUTION), CB_SETCURSEL, (WPARAM) i, 0);
			}
		}
		SendMessage(GetDlgItem(hWnd, IDC_FULLSCREEN), BM_SETCHECK, Fullscreen ? 1 : 0, 0);
		return true;
		break;
	default:
		return false;
		break;
    }
}

std::vector<ScreenResolution> GetAllFullscreenResolutions() {
	std::vector<ScreenResolution> Result;
	DEVMODE DisplayMode;
	ZeroMemory(&DisplayMode, sizeof(DisplayMode));
	DisplayMode.dmSize = sizeof(DisplayMode);
    for (int i = 0; EnumDisplaySettings(NULL, i, &DisplayMode) != 0; i++) {
        ScreenResolution TempRes = { (short) DisplayMode.dmPelsWidth, (short) DisplayMode.dmPelsHeight };
        Result.push_back(TempRes);
    } for (unsigned i = 0; i < Result.size(); i++) {
		if (float(Result[i].Width) / float(Result[i].Height) < 1.33f) {
			Result.erase(Result.begin() + i);
			i--;
		} if (GetSystemMetrics(SM_CXSCREEN) > 800 && GetSystemMetrics(SM_CYSCREEN) > 600 && Result[i].Width < 800 && Result[i].Height < 600) {
			Result.erase(Result.begin() + i);
			i--;
		} if (i != 0 && Result[i].Width == Result[i-1].Width && Result[i].Height == Result[i-1].Height) {
            Result.erase(Result.begin() + i);
            i--;
		}
    }
    return Result;
}

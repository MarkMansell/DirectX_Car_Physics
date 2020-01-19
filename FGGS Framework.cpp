#include "Game.h"

void gameLoopDelay(DWORD frameStartTime)
{
	DWORD frameProcessingTime = GetTickCount() - frameStartTime;

	DWORD presentFrameInterval = 8;

	if (frameProcessingTime < presentFrameInterval)
	{
		Sleep(presentFrameInterval - frameProcessingTime);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	float timeStep = 1.0f / 60.0f;

	Game * theGame = new Game();

	//Initialise directx window and such first
	//then load all the mesh data and models in the game
	//then continue with the rest of the directx cpp file

	if (FAILED(theGame->Initialise(hInstance, nCmdShow)))
	{
		return -1;
	}

	theGame->LoadContent();//maybe add a hresult here

    // Main message loop
    MSG msg = {0};

    while (WM_QUIT != msg.message)
    {
		DWORD frameStartTime = GetTickCount();
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			theGame->Update(timeStep);
            theGame->Draw();
			gameLoopDelay(frameStartTime);
        }
    }

	delete theGame;
	theGame = nullptr;

    return (int) msg.wParam;
}
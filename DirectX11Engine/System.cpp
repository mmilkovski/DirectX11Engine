#include "System.h"
#include "VectorMath.h"
#include <DirectXMath.h>
using namespace VectorMath;

System::System()
	:
	_Input(nullptr),
	_Graphics(nullptr),
	_Fps(nullptr),
	_Cpu(nullptr),
	_Timer(nullptr),
	_Position(nullptr)
{}

System::System(const System& other)
{}

System::~System()
{}

bool System::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	_Input = new Input;
	if (!_Input)
	{
		return false;
	}

	//@OLD
	// Initialize the input object.
	//_Input->Initialize();

	// Initialize the input object.
	result = _Input->Initialize(_hinstance, _hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	_Graphics = new Graphics;
	if (!_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = _Graphics->Initialize(screenWidth, screenHeight, _hwnd);
	if (!result)
	{
		return false;
	}

	// Create the timer object.
	_Timer = new TimerClass;
	if (!_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = _Timer->Initialize();
	if (!result)
	{
		MessageBox(_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the fps object.
	_Fps = new FpsClass;
	if (!_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	_Fps->Initialize();

	// Create the cpu object.
	_Cpu = new CpuClass;
	if (!_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	_Cpu->Initialize();

	// Create the position object.
	_Position = new PositionClass;
	if (!_Position)
	{
		return false;
	}

	return true;
}

void System::Shutdown()
{
	// Release the position object.
	if (_Position)
	{
		delete _Position;
		_Position = 0;
	}

	// Release the cpu object.
	if (_Cpu)
	{
		_Cpu->Shutdown();
		delete _Cpu;
		_Cpu = 0;
	}

	// Release the fps object.
	if (_Fps)
	{
		delete _Fps;
		_Fps = 0;
	}

	// Release the timer object.
	if (_Timer)
	{
		delete _Timer;
		_Timer = 0;
	}
	
	// Release the graphics object.
	if (_Graphics)
	{
		_Graphics->Shutdown();
		delete _Graphics;
		_Graphics = 0;
	}

	// Release the input object.
	if (_Input)
	{
		_Input->Shutdown();
		delete _Input;
		_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

void System::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				MessageBox(_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				done = true;
			}
		}

		// Check if the user pressed escape and wants to quit.
		if (_Input->IsEscapePressed() == true)
		{
			done = true;
		}
	}
	
	return;
}

bool System::Frame()
{
	bool keyDown, result;
	int mouseX = 0; 
	int mouseY = 0;
	float rotationY = 0;

	// Update the system stats.
	_Timer->Frame();
	_Fps->Frame();
	_Cpu->Frame();

	// Do the input frame processing.
	result = _Input->Frame();
	if (!result)
	{
		return false;
	}

	// Set the frame time for calculating the updated position.
	_Position->SetFrameTime(_Timer->GetTime());

	// Check if the left or right arrow key has been pressed, if so rotate the camera accordingly.
	keyDown = _Input->IsLeftArrowPressed();
	_Position->TurnLeft(keyDown);

	keyDown = _Input->IsRightArrowPressed();
	_Position->TurnRight(keyDown);

	//@custom
	//result = ProcessInput();
	//if (!result)
	//{
	//	return false;
	//}


	//// Get the current view point rotation.
	_Position->GetRotation(rotationY);

	// Do the frame processing for the graphics object.
	//result = _Graphics->Frame(rotationY, mouseX, mouseY, _Fps->GetFps(), _Cpu->GetCpuPercentage(), _Timer->GetTime());
	result = _Graphics->Frame(_Timer->GetTime(), _Fps->GetFps(), /*_Position.X, _Position.Y, _Position.Z,*/ 0.f,0.f,0.f, 0.f, rotationY, 0.f);
	if (!result)
	{
		return false;
	}

	// Finally render the graphics to the screen.
	result = _Graphics->Render();
	if (!result)
	{
		return false;
	}

	return true;
}

//@CUSTOM @TODO - rewrite
//bool System::ProcessInput()
//{
	//// Check if the user pressed escape and wants to exit the application.
	//if (_Input->IsKeyDown(VK_ESCAPE))
	//{
	//	return false;
	//}
	//// @TODO: 
	//// 1. Move this into another class
	//float moveIncrement = 0.02f;
	//float turnIncrement = 0.2f;
	//XMFLOAT3 positionOffset(0 , 0 , 0);
	//XMFLOAT3 rotationOffset(0, 0, 0);
	//
	//// ROTATION
	//if (_Input->IsKeyDown(VK_UP))
	//{
	//	rotationOffset.x -= turnIncrement;
	//}
	//if (_Input->IsKeyDown(VK_DOWN))
	//{
	//	rotationOffset.x += turnIncrement;
	//}
	//if (_Input->IsKeyDown(VK_LEFT))
	//{
	//	rotationOffset.y -= turnIncrement;
	//}
	//if (_Input->IsKeyDown(VK_RIGHT))
	//{
	//	rotationOffset.y += turnIncrement;
	//}
	//// DISPLACEMENT
	//if (_Input->IsKeyDown('W'))
	//{
	//	positionOffset.z += moveIncrement;
	//}
	//if (_Input->IsKeyDown('A'))
	//{
	//	positionOffset.x -= moveIncrement;
	//}
	//if (_Input->IsKeyDown('S'))
	//{
	//	positionOffset.z -= moveIncrement;
	//}	
	//if (_Input->IsKeyDown('D'))
	//{
	//	positionOffset.x += moveIncrement;
	//}
	//if (_Input->IsKeyDown(VK_SPACE))
	//{
	//	positionOffset.y += moveIncrement;
	//}
	//if (_Input->IsKeyDown(VK_CONTROL))
	//{
	//	positionOffset.y -= moveIncrement;
	//}
	//if (_Graphics)
	//{
	//	if (Camera* cam = _Graphics->GetCamera())
	//	{
	//		cam->MoveInDirectionRelative(positionOffset);
	//		cam->RotateInDirection(rotationOffset);
	//	}
	//}
//	return true;
//}

LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	//switch (umsg)
	//{
	//	// Check if a key has been pressed on the keyboard.
	//case WM_KEYDOWN:
	//{
	//	// If a key is pressed send it to the input object so it can record that state.
	//	_Input->KeyDown((unsigned int)wparam);
	//	return 0;
	//}
	//// Check if a key has been released on the keyboard.
	//case WM_KEYUP:
	//{
	//	// If a key is released then send it to the input object so it can unset the state for that key.
	//	_Input->KeyUp((unsigned int)wparam);
	//	return 0;
	//}
	//// Any other messages send to the default message handler as our application won't make use of them.
	//default:
	//{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	//}
	//}
}

void System::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, _hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void System::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(_hwnd);
	_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(_applicationName, _hinstance);
	_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
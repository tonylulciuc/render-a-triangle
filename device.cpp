/**
 *	Author: Antony Lulciuc
 *	Description: Defines process of creating a Client Window
 */
#include "device.h"

namespace
{
	// Pointer to device
	Device* g_pDevice = NULL;
}

// Window Procedure Handler
LRESULT CALLBACK WndProc(HWND _hWnd, UINT _uiMessage, WPARAM _wParam, LPARAM _lParam)
{
	return (g_pDevice->wndProc(_hWnd, _uiMessage, _wParam, _lParam));
}

// Constructor
Device::Device(HINSTANCE _hInstance, LPCTSTR _lpszDeviceName,
	UINT _uiWindowWidth, UINT _uiWindowHeight)
{
	m_hInstance = _hInstance;
	m_uiClientWidth = _uiWindowWidth;
	m_uiClientHeight = _uiWindowHeight;
	m_strDeviceName = _lpszDeviceName;
	g_pDevice = this;
}

Device::~Device(void)
{
	if (m_hMain)
		CloseHandle(m_hMain);

	m_hMain = 0;
}

// Initialize device
int Device::init(void)
{
	// Register Device
	if (!registerDevice())
		return (1);

	// Create Device, 
	// NOTE: When creating a window (i.e. a window) it must be registered first
	if (!createDevice())
		return (2);

	return (0);
}

// Method that handles window events
LRESULT Device::wndProc(HWND _hWnd, UINT _uiMessage, WPARAM _wParam, LPARAM _lParam)
{
	// What type of message is it?
	switch (_uiMessage)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		// Handles mouse clicks
		onMouseDown(_hWnd, _wParam, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return (0);
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		// Handles mouse click release
		onMouseUp(_hWnd, _wParam, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return (0);
	case WM_MOUSEMOVE:
		// Handles mouse movement
		onMouseMove(_hWnd, _wParam, GET_X_LPARAM(_lParam), GET_Y_LPARAM(_lParam));
		return (0);
	case WM_SIZE:
		// Resizes winodw buffers
		resize();
		return (0);
	case WM_KEYDOWN:
		// Handle keyboard press
		return (0);
	case WM_DESTROY:
		// Window termination message
		PostQuitMessage(0);
		return (0);
	}

	// Return default message (i.e. unhandled message valid)
	return (DefWindowProc(_hWnd, _uiMessage, _wParam, _lParam));
}

// Create device
bool Device::createDevice(void)
{
	m_hMain = CreateWindow(m_strDeviceName.c_str(),	
							m_strDeviceName.c_str(),
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							m_uiClientWidth,
							m_uiClientHeight,
							0,
							0,
							m_hInstance,
							NULL);

	// If HANDLE returned equals NULL or 0 it failed to create the window
	if (!m_hMain)
	{
		MessageBox(NULL, TEXT("ERROR-Failed to create main device!"),
			TEXT("Creation Error!"), MB_ICONERROR | MB_OK);
		return (false);
	}

	// Update Display
	ShowWindow(m_hMain, SW_SHOWNORMAL);
	UpdateWindow(m_hMain);

	return (true);
}

// Register device
bool Device::registerDevice(void)
{
	// Window Registration information
	WNDCLASS wc;
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.lpfnWndProc  = WndProc;
	wc.hInstance	= m_hInstance;
	wc.hbrBackground = static_cast<HBRUSH>(CreateSolidBrush(RGB(255.0f, 255.0f, 255.0f)));
	wc.hCursor	= LoadIcon(m_hInstance, IDC_ARROW);
	wc.hIcon	= LoadIcon(m_hInstance, IDI_APPLICATION);
	wc.lpszClassName = m_strDeviceName.c_str();
	wc.lpszMenuName	 = NULL;

	// If failed t register the device
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("ERROR-Failed to register main device!"),
			TEXT("Registry Error!"), MB_ICONERROR | MB_OK);
		return (false);
	}

	// Store registration information for later use (i.e. create child windows)
	m_wcWndClass = wc;
	return (true);

}

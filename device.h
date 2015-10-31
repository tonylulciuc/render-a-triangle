/**
 *	Author: Antony Lulciuc
 *	Description: Defines a window Device
 *	Date: 10/30/2015
 */
#ifndef DEVICE_H
#define DEVICE_H

#include <Windows.h>
#include <assert.h>
#include <windowsx.h>
#include <string>
#include <vector>

using namespace std;

class Device
{
public:
	/**
	 *	Constructor
	 */
	Device(HINSTANCE _hInstance, LPCTSTR _lpszDeviceName,
		UINT _uiWindowWidth, UINT _uiWindowHeight);

	/**
	 *	Deconstructor
	 */
	virtual ~Device(void);

	/**
	*	Method Name:	wndProc
	*	Description:	Handles the main messages
	*	Parameter:		HWND& _hWnd[in] handle to a window
	*					UINT& _uiMessage[in] message that has been sent
	*					WPARAM&	_wParam[in] message description
	*					LPARAM& _lParam[in] message size/description
	*	Return:			0
	*/
	LRESULT wndProc(HWND, UINT, WPARAM, LPARAM);

	/**
	 *	Method Name:	init
	 *	Description:	Initializes device
	 *	Parameter:		n/a
	 *	Return:			0 on success, 1 iff failed to register, 2 iff failed to create
 	 */
	virtual int init(void);

	/**
	 *	Method Name:	onMouseDown
	 *	Description:	Handles mouse clicks
	 *	Parameter:		HWND[in] handle to window
	 *					WPARAM[in] event information
	 *					int[in] x-coordinate
	 *					int[in] y-coordinate
	 *	Return:			n/a
	 */
	virtual void onMouseDown(HWND, WPARAM, int, int) = 0;

	// Set mouse position when mouse moves 
	virtual void onMouseMove(HWND, WPARAM, int, int) = 0;

	// Set mouse position when mouse btn_clk is up
	virtual void onMouseUp(HWND, WPARAM, int, int) = 0;

	/**
	 *	Method Name:	resize
	 *	Description:	Resize display buffers
	 *	Parameter:		n/a
	 *	Return:			n/a
	 */
	virtual void resize(void) = 0;

protected:
	/**
	 *	Method Name:	createDevice
	 *	Description:	Creates the main device
	 *	Parameter:		n/a
	 *	Return:			returns true if device is created, else false.
	 */
	virtual bool createDevice(void);

	/**
	 *	Method Name:	registerDevice
	 *	Description:	Registers the main device
	 *	Parameter:		n/a
	 *	Return:			returns true when the device is registered, else false
	 */
	virtual bool registerDevice(void);

protected:
	HINSTANCE m_hInstance;	// instance of program
	WNDCLASS m_wcWndClass;	// window description
	HWND m_hMain;			// Main Window Handle
	UINT m_uiClientWidth;	// Window width;
	UINT m_uiClientHeight;	// Window Height
	string m_strDeviceName;
};

#endif

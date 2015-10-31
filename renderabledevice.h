/*
*	Author: Antony Lulciuc
*	Description: Enables direct x to render onto host device
*	Date: 10/30/2015
*/
#ifndef RenderableDevice_H
#define RenderableDevice_H

#include "device.h"
#include<D3D11.h>
#include<D3Dcompiler.h>
#include<D3DX11.h>
#include<DXGI.h>
#include <xnamath.h>


#define releaseCom(x){if(x)x->Release();}

typedef struct vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
}vertex;


class RenderableDevice : public Device
{
public:
	RenderableDevice(HINSTANCE _hInstance, LPCTSTR _lpszDeviceName,
		UINT _uiWindowWidth, UINT _uiWindowHeight);

	virtual ~RenderableDevice(void);

	/**
	*	Method Name:	drawScreen
	*	Description:	Renders the image contained in a buffer onto the screen
	*	Parameter:		n/a
	*	Return:			n/a
	*/
	void drawScreen(void);

	/**
	*	Method Name:	update
	*	Description:	Updates objects based on time
	*	Parameter:		double _dCurrentTime[in] current system time
	*	Return:			n/a
	*/
	virtual void update(double) = 0;

	/**
	*	Method Name:	resize
	*	Description:	Resize buffers to display image correctly onto screen
	*	Parameter:		n/a
	*	Return:			n/a
	*/
	virtual void resize(void);

	// Set mouse position when mouse btn_clk is down
	virtual void onMouseDown(HWND _hWnd, WPARAM, int, int){};

	// Set mouse position when mouse moves 
	virtual void onMouseMove(HWND _hWnd, WPARAM, int, int){};

	// Set mouse position when mouse btn_clk is up
	virtual void onMouseUp(HWND _hWnd, WPARAM, int, int){};

	// initialize device
	int init(void);

	// run program
	int run(void);

	// Add Blob to render
	virtual bool addBlob(const char*, vertex*, unsigned int*, int, int) = 0;

	// draw blobs
	virtual void drawBlobs(void) = 0;
	
	// Do we draw this object
	virtual bool clipBlob(const char*, bool) = 0;
protected:
	/**
	*	Method Name:	attachDXToMainDevice
	*	Description:	Attach a direct x application to a window
	*	Parameter:		n/a
	*	Return:			returns true when the direct x has a host, else false
	*/
	virtual bool attachDXToMainDevice(void);

	/**
	*	Method Name:	getShaderFromFile
	*	Description:	Retrieves shaders from an HLSL file
	*	Parameter:		string& _strFileName[in] shader file name
	*					string& _strEntryPoint[in] shader function name 
	*					string& _strShaderModel[in] vs_4_0
	*					ID3DBlob** _ppBlob[in, out] What we save compiled info to
	*	Return:			n/a
	*/
	HRESULT getShaderFromFile(string& _strFileName, string& _strEntryPoint, string& _strShaderModel, ID3DBlob** _ppBlob);

	/**
	*	Method Name:	setVertexShader
	*	Description:	Get vertex shader
	*	Parameter:		n/a
	*	Return:			true if successfully extracted, else false
	*/
	bool setVertexShader(void);

	/**
	*	Method Name:	setPixelShader
	*	Description:	Get pixel shader
	*	Parameter:		n/a
	*	Return:			true if successfully extracted, else false
	*/
	bool setPixelShader(void);

protected:
	ID3D11Device*			m_id3d11Device;
	ID3D11DeviceContext*	m_id3d11DeviceContext;
	IDXGISwapChain*			m_idxgiSwapChain;
	ID3D11RenderTargetView* m_id3d11RenderTargetView;
	ID3D11DepthStencilView* m_id3d11DepthStencilView;
	ID3D11Texture2D*		m_id3d11DepthStencilBuffer2D;
	ID3D11VertexShader*     m_id3d11VertexShader;
	ID3D11PixelShader*		m_id3d11PixelShader;
	ID3D11InputLayout*		m_id3d11InputLayout;
	D3D11_VIEWPORT			m_d3d11Viewport;
	D3D_DRIVER_TYPE			m_d3dDriverType;
	UINT					m_ui4xMsaaQuality;
	bool					m_bEnable4xMsaa;

};

#endif

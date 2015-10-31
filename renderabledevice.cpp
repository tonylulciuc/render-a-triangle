/*
*	Author: Antony Lulciuc
*
*/

#include "renderabledevice.h"

RenderableDevice::RenderableDevice(HINSTANCE _hInst, LPCTSTR _lpszDeviceName, UINT _uiWindowWidth,
	UINT _uiWindowHeight) : Device(_hInst, _lpszDeviceName, _uiWindowWidth, _uiWindowHeight),
							m_id3d11Device(0),
							m_id3d11DeviceContext(0),
							m_id3d11DepthStencilView(0),
							m_idxgiSwapChain(0),
							m_id3d11RenderTargetView(0),
							m_id3d11DepthStencilBuffer2D(0),
							m_d3dDriverType(D3D_DRIVER_TYPE_HARDWARE)
{
	m_bEnable4xMsaa = false;
	m_ui4xMsaaQuality = 4U;
}

RenderableDevice::~RenderableDevice(void)
{
	releaseCom(m_id3d11Device);
	releaseCom(m_idxgiSwapChain);
	releaseCom(m_id3d11DepthStencilView);
	releaseCom(m_id3d11RenderTargetView);

	if (m_id3d11DeviceContext)
	{
		m_id3d11DeviceContext->ClearState();
		m_id3d11DeviceContext->Release();
	}
}

int RenderableDevice::init(void)
{
	int result = 0;

	// create device
	if ((result = Device::init()) == 0)
	{

		// Create direct x renderable device
		if (!attachDXToMainDevice())
			return (3);

		// create vertex shader
		if (!m_id3d11VertexShader)
		{
			if (!setVertexShader())
				return (4);
		}

		// create pixel shader
		if (!m_id3d11PixelShader)
		{
			if (!setPixelShader())
				return (5);
		}

		// draw screen (triangle)
		drawScreen();
	}
		
	return (0);
}

void RenderableDevice::drawScreen(void)
{
	// Draw is devices created
	if (m_idxgiSwapChain && m_id3d11DeviceContext)
	{
		// Background color
		FLOAT color[4] = { 0.5f, 0.5f, 0.8f };

		// Set space with specified color, and prepare to render buffers
		m_id3d11DeviceContext->ClearRenderTargetView(m_id3d11RenderTargetView, color);
		m_id3d11DeviceContext->ClearDepthStencilView(m_id3d11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		m_id3d11DeviceContext->VSSetShader(m_id3d11VertexShader, NULL, 0);
		m_id3d11DeviceContext->PSSetShader(m_id3d11PixelShader, NULL, 0);

		// draw blobs 
		drawBlobs();

		// present finished buffer
		m_idxgiSwapChain->Present(0, 0);
	}
}

bool RenderableDevice::attachDXToMainDevice(void)
{
	UINT debugFlag = 0;

#if defined(DEBUG) || defined(_DEBUG)
	debugFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	releaseCom(m_idxgiSwapChain);

	D3D_FEATURE_LEVEL featuredLevel;
	HRESULT hr;

	// Create dx device
	if (!m_id3d11Device && !m_id3d11DeviceContext)
	{
		// Create the device
		hr = (D3D11CreateDevice(0,
			m_d3dDriverType,
			0,
			0,
			0, 0,
			D3D11_SDK_VERSION,
			&m_id3d11Device,
			&featuredLevel,
			&m_id3d11DeviceContext
			));
		if (hr != S_OK)
			return(false);

		// Does this device support DX11?
		if (featuredLevel != D3D_FEATURE_LEVEL_11_0)
		{
			MessageBox(NULL, TEXT("ERROR-Your device does not suppot Direct3D Level 11!"),
				0, MB_ICONERROR | MB_OK);
			return(false);
		}
	}

	// Create Swap Chain
	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	swapchainDesc.BufferDesc.Width = m_uiClientWidth;
	swapchainDesc.BufferDesc.Height = m_uiClientHeight;
	swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// 4XMSAA Enabled?
	if (m_bEnable4xMsaa)
	{
		swapchainDesc.SampleDesc.Count = 4;
		swapchainDesc.SampleDesc.Quality = m_ui4xMsaaQuality - 1;
	}
	else
	{
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
	}

	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 1;
	swapchainDesc.OutputWindow = m_hMain; 
	swapchainDesc.Windowed = true;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchainDesc.Flags = 0;

	// Process used to create swap chain, and to attach to device
	IDXGIDevice* idxgiDevice = 0;
	IDXGIAdapter* idxgiAdapter = 0;
	IDXGIFactory* idxgiFactory = 0;

	hr = (m_id3d11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&idxgiDevice));
	if (hr != S_OK)
	{
		releaseCom(idxgiDevice);
		releaseCom(idxgiAdapter);
		releaseCom(idxgiFactory);
		return(false);
	}

	hr = (idxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&idxgiAdapter));
	if (hr != S_OK)
	{
		releaseCom(idxgiDevice);
		releaseCom(idxgiAdapter);
		releaseCom(idxgiFactory);
		return(false);
	}
	hr = (idxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&idxgiFactory));
	if (hr != S_OK)
	{
		releaseCom(idxgiDevice);
		releaseCom(idxgiAdapter);
		releaseCom(idxgiFactory);
		return(false);
	}

	hr = (idxgiFactory->CreateSwapChain(m_id3d11Device, &swapchainDesc, &m_idxgiSwapChain));

	if (hr != S_OK)
	{
		releaseCom(idxgiDevice);
		releaseCom(idxgiAdapter);
		releaseCom(idxgiFactory);
		return(false);
	}
	releaseCom(idxgiDevice);
	releaseCom(idxgiAdapter);
	releaseCom(idxgiFactory);

	// Set buffers
	resize();

	return (true);
}

HRESULT RenderableDevice::getShaderFromFile(string& _strFileName, string& _strEntryPoint,
	string& _strShaderModel, ID3DBlob** _ppBlob)
{
	HRESULT hr;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrBlob = 0;

	hr = D3DX11CompileFromFile(_strFileName.c_str(), NULL, NULL, _strEntryPoint.c_str(),
		_strShaderModel.c_str(), dwShaderFlags, 0, NULL, _ppBlob, &pErrBlob, NULL);

	if (FAILED(hr))
	{
		if (pErrBlob != NULL)
			OutputDebugString((char*)pErrBlob->GetBufferPointer());
		releaseCom(pErrBlob);
		return (hr);
	}
	releaseCom(pErrBlob);
	return (hr);
}

bool RenderableDevice::setVertexShader(void)
{
	HRESULT hr;
	ID3DBlob* pVertexShaderBlob = 0;
	string fileName = "fxEffects.fx";
	string entryPoint = "VS";
	string shaderModel = "vs_4_0";

	releaseCom(m_id3d11VertexShader);
	releaseCom(m_id3d11InputLayout);

	hr = getShaderFromFile(fileName, entryPoint, shaderModel, &pVertexShaderBlob);

	if (FAILED(hr))
	{
		MessageBox(NULL,
			"The FX file cannot be compiled.", 
			"Error", MB_OK);
		return (false);
	}

	hr = m_id3d11Device->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(), NULL, &m_id3d11VertexShader);

	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("ERROR-Failed to create the vertex shader!"),
			TEXT("ERROR!"), MB_ICONERROR | MB_OK);
		return (false);
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = m_id3d11Device->CreateInputLayout(layout, numElements, pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(), &m_id3d11InputLayout);
	
	m_id3d11DeviceContext->IASetInputLayout(m_id3d11InputLayout);

	releaseCom(pVertexShaderBlob);	
	return (true);
}


bool RenderableDevice::setPixelShader(void)
{
	HRESULT hr;
	ID3DBlob* pPixelShaderBlob = 0;
	string fileName = "fxEffects.fx";
	string entryPoint = "PS";
	string shaderModel = "ps_4_0";

	releaseCom(m_id3d11PixelShader);

	hr = getShaderFromFile(fileName, entryPoint, shaderModel, &pPixelShaderBlob);

	if (FAILED(hr))
	{
		MessageBox(NULL,
			"The FX file cannot be compiled.",
			"Error", MB_OK);
		return (false);
	}

	hr = m_id3d11Device->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(),
		NULL, &m_id3d11PixelShader);

	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("ERROR-Failed to create the pixel shader!"),
			TEXT("ERROR!"), MB_ICONERROR | MB_OK);
		return (false);
	}

	releaseCom(pPixelShaderBlob);
	return (true);
}


void RenderableDevice::resize(void)
{
	// resize buffer
	if (m_id3d11Device)
	{
		assert(m_id3d11DeviceContext);
		assert(m_idxgiSwapChain);

		releaseCom(m_id3d11RenderTargetView);
		releaseCom(m_id3d11DepthStencilView);
		releaseCom(m_id3d11DepthStencilBuffer2D);

		HRESULT hr;
		hr = (m_idxgiSwapChain->ResizeBuffers(1, m_uiClientWidth, m_uiClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

		ID3D11Texture2D* backBuffer = 0;

		hr = (m_idxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

		if (hr != S_OK)
			return;

		hr = (m_id3d11Device->CreateRenderTargetView(backBuffer, 0, &m_id3d11RenderTargetView));
		if (hr != S_OK)
			return;
		releaseCom(backBuffer);

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = m_uiClientWidth;
		depthStencilDesc.Height = m_uiClientHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		if (m_bEnable4xMsaa)
		{
			depthStencilDesc.SampleDesc.Count = 4;
			depthStencilDesc.SampleDesc.Quality = m_ui4xMsaaQuality - 1;
		}
		else
		{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}

		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		hr = (m_id3d11Device->CreateTexture2D(&depthStencilDesc, 0, &m_id3d11DepthStencilBuffer2D));
		hr = (m_id3d11Device->CreateDepthStencilView(m_id3d11DepthStencilBuffer2D, 0, &m_id3d11DepthStencilView));
		assert(hr == S_OK);
		m_id3d11DeviceContext->OMSetRenderTargets(1, &m_id3d11RenderTargetView, m_id3d11DepthStencilView);

		m_d3d11Viewport.Width = static_cast<FLOAT>(m_uiClientWidth);
		m_d3d11Viewport.Height = static_cast<FLOAT>(m_uiClientHeight);
		m_d3d11Viewport.MaxDepth = 1.0f;
		m_d3d11Viewport.MinDepth = 0.0f;
		m_d3d11Viewport.TopLeftX = 0;
		m_d3d11Viewport.TopLeftY = 0;

		m_id3d11DeviceContext->RSSetViewports(1, &m_d3d11Viewport);
	}
}

int RenderableDevice::run(void)
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			update(0.0);
			drawScreen();
		}
	}

	return (static_cast<int>(msg.wParam));
}


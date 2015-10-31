#include "gamedevice.h"

GameDevice::GameDevice(HINSTANCE _hInstance, LPCTSTR _lpszDeviceName,
	UINT _uiWindowWidth, UINT _uiWindowHeight) : RenderableDevice(_hInstance, _lpszDeviceName,
	_uiWindowWidth, _uiWindowHeight)
{

}

GameDevice::~GameDevice(void)
{
	unsigned int uiSize = m_vVertexBuffer.size();

	// free buffers
	for (unsigned int i = 0U; i < uiSize; i++)
	{
		releaseCom(m_vVertexBuffer[i]);
		releaseCom(m_vIndexBuffer[i]);
	}
}

// add blob to render
bool GameDevice::addBlob(const char* _pchBlobName, vertex* _pBlobVertex, unsigned int* _pBlobIndex, int _iVertexCount, int _iIndexCount)
{
	ID3D11Buffer* id3d11VertexBuffer = NULL;
	ID3D11Buffer* id3d11IndexBuffer = NULL;
	UINT uiSize = m_vBlobName.size();
	animation anima;

	if (!_pchBlobName || !_pBlobVertex || !_pBlobIndex || _iVertexCount < 1 || _iIndexCount < 1)
		return (false);

	for (unsigned int i = 0U; i < uiSize; i++)
	{
		if (m_vBlobName[i] == _pchBlobName)
			return (false);
	}

	// Add blob and relative data to list
	m_vVertexBuffer.push_back(id3d11VertexBuffer);
	m_vIndexBuffer.push_back(id3d11IndexBuffer);
	m_vVertexCount.push_back(_iVertexCount);
	m_vIndexCount.push_back(_iIndexCount);
	m_vVertex.push_back(_pBlobVertex);
	m_vIndices.push_back(_pBlobIndex);
	m_vBlobName.push_back(string(_pchBlobName));
	m_vClipBlob.push_back(false);

	// no instruction
	anima.anima = NULL;
	m_vAnimation.push_back(anima);

	// update buffers
	updateVertexBuffer(m_vVertexBuffer.size() - 1);
	updateIndexBuffer(m_vIndexBuffer.size() - 1);

	return (true);
}

// draw blobs
void GameDevice::drawBlobs(void)
{
	unsigned int uiSize = m_vVertexBuffer.size();
	unsigned int stride = sizeof(vertex);
	unsigned int offset = 0U;

	for (unsigned int i = 0U; i < uiSize; i++)
	{
		if (!m_vClipBlob[i])
		{
			// Update Buffers
			updateVertexBuffer(i);
			updateIndexBuffer(i);

			// Set vertex buffer
			m_id3d11DeviceContext->IASetVertexBuffers(0, 1, &m_vVertexBuffer[i], &stride, &offset);

			// Set buffer
			m_id3d11DeviceContext->IASetIndexBuffer(m_vIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

			// How to use indices
			m_id3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// draw blob
			m_id3d11DeviceContext->DrawIndexed(m_vIndexCount[i], 0, 0);
		}
	}
}

// Do we draw the blob?
bool GameDevice::clipBlob(const char* _pchBlobName, bool _bClip)
{
	unsigned int uiSize = 0U;

	if (_pchBlobName)
	{
		for (unsigned int i = 0U; i < uiSize; i++)
		{
			if (m_vBlobName[i] == _pchBlobName)
			{
				m_vClipBlob[i] = _bClip;
				return (true);
			}
		}
	}

	return (false);
}

void GameDevice::update(double _dt)
{
	unsigned int uiSize = m_vVertex.size();
	blob_cycle bc;

	// Update blob
	for (unsigned int i = 0U; i < uiSize; i++)
	{
		if (m_vAnimation[i].anima)
		{
			bc.v  = m_vVertex[i];
			bc.i  = m_vIndices[i];
			bc.cv = m_vVertexCount[i];
			bc.ci = m_vIndexCount[i];
			(*m_vAnimation[i].anima)(reinterpret_cast<void*>(&bc));
		}
	}
}

bool GameDevice::addAnimation(const char* _pchBlobName, const animation& _anima)
{
	unsigned int uiSize = m_vBlobName.size();

	if (_pchBlobName)
	{
		for (unsigned int i = 0U; i < uiSize; i++)
		{
			if (m_vBlobName[i] == _pchBlobName)
			{
				m_vAnimation[i] = _anima;
				return (true);
			}
		}
	}

	return (false);
}

bool GameDevice::updateVertexBuffer(int _iIndex)
{
	D3D11_BUFFER_DESC vd;
	D3D11_SUBRESOURCE_DATA init;
	UINT offset = 0U;
	UINT stride = sizeof(vertex);

	if (_iIndex < 0 || static_cast<unsigned int>(_iIndex) >= m_vVertexBuffer.size())
		return (false);

	// Zero Memory
	releaseCom(m_vVertexBuffer[_iIndex]);
	ZeroMemory(&vd, sizeof(vd));
	ZeroMemory(&init, sizeof(init));

	// How to represent data
	vd.Usage = D3D11_USAGE_DEFAULT;
	vd.ByteWidth = stride * m_vVertexCount[_iIndex];
	vd.CPUAccessFlags = 0;
	vd.MiscFlags = 0;
	vd.StructureByteStride = 0;
	init.pSysMem = m_vVertex[_iIndex];

	// Create vertex buffer
	if (FAILED(m_id3d11Device->CreateBuffer(&vd, &init, &m_vVertexBuffer[_iIndex])))
		return (false);

	return (true);
}

bool GameDevice::updateIndexBuffer(int _iIndex)
{
	D3D11_BUFFER_DESC vd;
	D3D11_SUBRESOURCE_DATA init;
	UINT stride = sizeof(UINT);

	if (_iIndex < 0 || static_cast<unsigned int>(_iIndex) >= m_vIndexBuffer.size())
		return (false);

	// Zero Memory
	releaseCom(m_vIndexBuffer[_iIndex]);
	ZeroMemory(&vd, sizeof(vd));
	ZeroMemory(&init, sizeof(init));

	// How to use and describe data 
	vd.Usage = D3D11_USAGE_IMMUTABLE;
	vd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vd.ByteWidth = stride * m_vIndexCount[_iIndex];
	vd.CPUAccessFlags = 0;
	vd.MiscFlags = 0;
	vd.StructureByteStride = 0;
	init.pSysMem = m_vIndices[_iIndex];

	// Create buffer
	if (FAILED(m_id3d11Device->CreateBuffer(&vd, &init, &m_vIndexBuffer[_iIndex])))
		return (false);

	return (true);
}

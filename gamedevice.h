/**
*	Author: Antony Lulciuc
*	Description: Defines a Game Device
*	Date: 10/30/2015
*/
#ifndef GAMEDEVICE_H
#define GAMEDEVICE_H

#include "renderabledevice.h"

typedef struct blob_cycle
{
	vertex* v;  // vertices
	UINT*   i;  // indices
	UINT	cv; // vertex count
	UINT	ci; // index count
	double	dt; // delta t
}blob_cycle, *pblob_cycle;

typedef struct animation
{
	void*(*anima)(void*);
}animation, *panimation;

class GameDevice : public RenderableDevice
{
public:
	GameDevice(HINSTANCE _hInstance, LPCTSTR _lpszDeviceName,
		UINT _uiWindowWidth, UINT _uiWindowHeight);

	virtual ~GameDevice(void);

	/**
	 *	Method Name:	addBlob
	 *	Description:	Adds blob to render
	 *	Parameter:		const char*[in] blob name
	 *					vertex*[in] verticies of blob
	 *					unsigned int*[in] indices of blob
	 *					int[in] vertex count
	 *					int[in] index count
	 *	Return:			bool, true on success
	 */
	virtual bool addBlob(const char*, vertex*, unsigned int*, int, int);

	/**
	*	Method Name:	addAnimation
	*	Description:	Adds animation to blob
	*	Parameter:		const char*[in] blob to animate
	*					const animation&[in] animation 
	*	Return:			bool, true is succeeded
	*/
	virtual bool addAnimation(const char*, const animation&);

	/**
	*	Method Name:	drawBlobs
	*	Description:	Draws blobs
	*	Parameter:		n/a
	*	Return:			n/a
	*/
	virtual void drawBlobs(void);

	/**
	*	Method Name:	clipBlob
	*	Description:	Determins if blob is rendered
	*	Parameter:		const char*[in] blob name
	*					bool[in] true blob is not rendered
	*							 false blob is rendered
	*	Return:			boolean, true on success
	*/
	virtual bool clipBlob(const char*, bool);

	/**
	 *	Method Name:	update
	 *	Description:	Updates objects based on time
	 *	Parameter:		double[in] current system time
	 *	Return:			n/a
	 */
	void update(double);

protected:
	/**
	*	Method Name:	updateVertexBuffer
	*	Description:	Updates vertex buffer
	*	Parameter:		int[in] buffer index
	*	Return:			boolean, true on success
	*/
	bool updateVertexBuffer(int);

	/**
	*	Method Name:	updateIndexBuffer
	*	Description:	Updates index buffer
	*	Parameter:		int[in] buffer index
	*	Return:			boolean, true on success
	*/
	bool updateIndexBuffer(int);
protected:
	vector<ID3D11Buffer*> m_vVertexBuffer;
	vector<ID3D11Buffer*> m_vIndexBuffer;
	vector<vertex*> m_vVertex;
	vector<unsigned int*> m_vIndices;
	vector<unsigned int> m_vVertexCount;
	vector<unsigned int> m_vIndexCount;
	vector<animation> m_vAnimation;
	vector<string> m_vBlobName;
	vector<bool>	m_vClipBlob;

};



#endif

/**
 *	Author: Antony Lulciuc
 *	Description: Primitive game device
 *	Date: 10/30/2015
 */
#include "gamedevice.h"

// Lets add a simple animation
// Lets move the triangle
void* move(void* _pblobCycle)
{
	pblob_cycle pbc = reinterpret_cast<pblob_cycle>(_pblobCycle); // Blob info
	register float x = 0.0001f; // Movement along x-axis
	register float y = 0.0001f; // Movement along y-axis
	static UINT called = 0U;	 // How many times was this function called

	// if called 10K+
	if (called > 10000U)
		called = 0U; // reset

	// if called 5K+
	// reverse direction
	if (called++ > 5000U)
	{
		x *= -1.0f;
		y *= -1.0f;
	}

	// Move the triangle
	for (unsigned int i = 0U; i < pbc->cv; i++)
	{
		pbc->v[i].pos.x += x;
		pbc->v[i].pos.y += y;
	}

	// Return does nothing for this function
	return (NULL);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmd, int iCmdShow)
{
	// Create device
	GameDevice gd(hInstance, "Game Device", 800U, 640U);

	// Initialize device
	if (gd.init() != 0)
		return (1);

	// Lets create a triangle!
	vertex v[3]; // vertices
	UINT i[3]; // indices NOTE UINT same as unsigned int

	// XMFLOAT3 and XMFLOAT4 are defined in xnamath
	v[0].pos = XMFLOAT3(0.25f, 0.0f, 0.0f);
	v[0].color = XMFLOAT4(1.0F, 0.0f, 0.0f, 1.0f);

	v[1].pos = XMFLOAT3(0.0f, 0.25f, 0.0f);
	v[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	v[2].pos = XMFLOAT3(-0.25f, 0.0f, 0.0f);
	v[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// indices
	i[0] = 2;
	i[1] = 1;
	i[2] = 0;

	// Create Triangle
	gd.addBlob("triangle", v, i, 3, 3);

	// Add simple movement animation to triangle blob
	animation animate;
	animate.anima = &move;

	gd.addAnimation("triangle", animate);

	// run device
	return (gd.run());
}

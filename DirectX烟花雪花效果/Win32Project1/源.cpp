#include<Windows.h>
#include <ctime>
#include<d3d9.h>
#pragma comment(lib,"d3d9.lib")
#include <Mmsystem.h>
#pragma comment( lib,"winmm.lib" )
#include <d3dx9math.h>
#pragma comment(lib,"d3dx9.lib")
#include<vector>
#include<iostream>
#include"jl.h"
#include<list>
using std::vector;
const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

//
// Lights
//

D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);

//
// Materials
//

D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);
D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = a;
	mtrl.Diffuse = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power = p;
	return mtrl;
}



//
// Globals
//
D3DXVECTOR3 _look(0, 0, 1);
D3DXVECTOR3 _up(0, 1, 0);
D3DXVECTOR3 _right(1, 0, 0);
D3DXVECTOR3 _pos(0, 20, -30);
D3DMATERIAL9 mat = WHITE_MTRL;
D3DMATERIAL9 mat1 = RED_MTRL;
D3DXMATRIX V;
IDirect3DDevice9* Device = 0;
IDirect3DVertexBuffer9 *ve;
IDirect3DIndexBuffer9 *ind;
ID3DXMesh* Teapot = 0;
ID3DXMesh* ship = 0;
vector <D3DMATERIAL9> shipmat(0);
vector <IDirect3DTexture9*> shiptex(0);
IDirect3DTexture9 *tex1;
IDirect3DTexture9 *tex2;
IDirect3DTexture9 *wall;
LPD3DXFONT font;
DWORD framecnt = 0;
float timeElapsed = 0;
float FPS = 0;
PSystem* Sno;
struct vertex
{
	vertex(float a, float b, float c, D3DVECTOR co)
	{
		x = a;
		y = b;
		z = c;
		ve = co;
	}
	vertex(float a, float b, float c, float f, float g, float h, float d, float e)
	{
		x = a;
		y = b;
		z = c;
		ve.x = f;
		ve.y = g;
		ve.z = h;
		u = d;
		v = e;
	}
	float x, y, z;
	D3DVECTOR ve;
	float u, v;
	static const DWORD FVF;
};
const DWORD vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
//
// Framework Functions
//
VOID createlight();
VOID createfps();
VOID calcfps(float timedelta);
VOID meshXinit();
void getViewMatrix(D3DXMATRIX* V);
void keycon();
bool Setup()
{
	HRESULT st;
	srand((unsigned int)time(0));

	
	D3DXVECTOR3 a(0, 20, 0);
	Sno = new Firework(&a, 10000);
	Sno->init(Device, "flare.bmp");


	/*BoundingBox boundingBox;
	boundingBox._min = D3DXVECTOR3(-10.0f, -10.0f, -10.0f);
	boundingBox._max = D3DXVECTOR3(10.0f, 10.0f, 10.0f);
	Sno = new Snow(&boundingBox, 2000);
	Sno->init(Device, "snowflake.dds");*/

	/*createlight();*/
	D3DXCreateTeapot(Device, &Teapot, 0);
	Device->CreateVertexBuffer(16 * sizeof(vertex), D3DUSAGE_WRITEONLY, vertex::FVF, D3DPOOL_MANAGED, &ve, 0);
	Device->CreateIndexBuffer(66 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ind, 0);
	vertex *tex;
	ve->Lock(0, 0, (void**)&tex, 0);
	tex[0] = vertex(-1, -1, -1, 0.0f, 1.0f, 0.0f, 0, 1);
	tex[1] = vertex(-1, 1, -1, 0, 1, 0, 0, 0);
	tex[2] = vertex(1, 1, -1, 0, 1, 0, 1, 0);
	tex[3] = vertex(1, -1, -1, 0, 1, 0, 1, 1);
	tex[4] = vertex(-1, -1, 1, 0, 1, 0, 1, 1);
	tex[5] = vertex(-1, 1, 1, 0, 1, 0, 1, 0);
	tex[6] = vertex(1, 1, 1, 0, 1, 0, 0, 0);
	tex[7] = vertex(1, -1, 1, 0, 1, 0, 0, 1);
	tex[8] = vertex(100, -1, 100, 0, 1, 0, 0, 2);
	tex[9] = vertex(-100, -1, 100, 0, 1, 0, 0, 0);
	tex[10] = vertex(-100, -1, -100, 0, 1, 0, 2, 0);
	tex[11] = vertex(100, -1, -100, 0, 1, 0, 2, 2);
	tex[12] = vertex(100, 500, 100, 0, 1, 0, 0, 0);
	tex[13] = vertex(-100, 500, 100, 0, 1, 0, 0, 2);
	tex[14] = vertex(-100, 500, -100, 0, 1, 0, 2, 2);
	tex[15] = vertex(100, 500, -100, 0, 1, 0, 2, 0);
	ve->Unlock();
	WORD *indices;
	ind->Lock(0, 0, (void**)&indices, 0);
	// front side
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// back side
	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 4; indices[10] = 7; indices[11] = 6;

	// left side
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	// right side
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;

	// top
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;

	// bottom
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;
	indices[36] = 9; indices[37] = 8; indices[38] = 10;
	indices[39] = 10; indices[40] = 8; indices[41] = 11;
	indices[42] = 15; indices[43] = 10; indices[44] = 11;
	indices[45] = 14; indices[46] = 10; indices[47] = 15;
	indices[48] = 9; indices[49] = 10; indices[50] = 14;
	indices[51] = 13; indices[52] = 9; indices[53] = 14;
	indices[54] = 12; indices[55] = 8; indices[56] = 9;
	indices[57] = 9; indices[58] = 13; indices[59] = 12;
	indices[60] = 15; indices[61] = 11; indices[62] = 8;
	indices[63] = 12; indices[64] = 15; indices[65] = 8;
	Device->SetTransform(D3DTS_VIEW, &V);
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveLH(&proj, D3DX_PI / 2, 1920 / 1080, 1, 1000);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	ind->Unlock();
	st = D3DXCreateTextureFromFile(Device, TEXT("201311140304289111.jpg"), &tex1);
	st = D3DXCreateTextureFromFile(Device, TEXT("crate.jpg"), &tex2);
	st = D3DXCreateTextureFromFile(Device, TEXT("wall.jpg"), &wall);
	meshXinit();
	createfps();
	return true;
}
void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(ve);
	d3d::Release<IDirect3DIndexBuffer9*>(ind);
	// Nothing to cleanup in this sample.
}

bool Display(float timeDelta)
{
	if (Device) // Only use Device methods if we have a valid device.
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		RECT rec = { 0, 0, 200, 200 };
		calcfps(timeDelta);
		TCHAR *fps = (TCHAR*)malloc(50);
		swprintf(fps, 50, TEXT("FPS:%f"), FPS);
		font->DrawText(NULL, fps, -1, &rec, DT_CENTER, D3DCOLOR_XRGB(255, 0, 0));
		free(fps);
		keycon();
		D3DXMATRIX V;
		getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);
		D3DXMATRIX rx, ry, rz;
		D3DXMatrixRotationX(&rx, 0);
		static float y = 0;
		static float y1 = 0;
		D3DXMatrixRotationY(&ry, y * 3);
		D3DXMatrixTranslation(&rz, 2, 0, 0);
		/* y+= timeDelta;
		if (y >= 6.28f)
		{
		y = 0;
		}*/
		D3DXMATRIX world = rx*ry*rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->BeginScene();
		Device->SetStreamSource(0, ve, 0, sizeof(vertex));
		Device->SetIndices(ind);
		Device->SetFVF(vertex::FVF);
		Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		// Draw cube.
		Device->SetTexture(0, tex2);
		Device->SetMaterial(&mat);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
		D3DXMatrixTranslation(&rz, 5, 0, 0);
		world = rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
		D3DXMatrixTranslation(&rz, 0, 0, 0);
		world = rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->SetTexture(0, tex1);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 8, 4, 36, 2);
		D3DXMatrixTranslation(&rz, 0, 0, 0);
		world = rz;
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->SetTexture(0, wall);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 12, 4, 42, 8);
		static float tea = 0;
		static float x = 1;
		tea = tea + 0.001f*x;
		if (tea > 5)
		{
			x = -1;
		}
		if (tea < -5)
		{
			x = 1;
		}
		D3DXMatrixTranslation(&rz, -2, 0, tea);
		D3DXMatrixRotationY(&ry, y1*-3);
		D3DXMATRIX world1 = rx*ry*rz;
		y1 += timeDelta;
		if (y1 >= 6.28f)
		{
			y1 = 0;
		}
		Device->SetTransform(D3DTS_WORLD, &world1);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		Device->SetMaterial(&mat1);
		Teapot->DrawSubset(0);
		D3DXMatrixScaling(&rx, 0.1f, 0.1f, 0.1f);
		D3DXMatrixTranslation(&rz, 0, 80, 0);
		world = rz*rx;
		Device->SetTransform(D3DTS_WORLD, &world);
		for (unsigned int i = 0; i < shipmat.size(); i++)
		{
			Device->SetMaterial(&shipmat[i]);
			Device->SetTexture(0, shiptex[i]);
			ship->DrawSubset(i);
		}


		Sno->update(timeDelta);
		if (Sno->isDead())
		{
			Sno->reset();
		}
		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		Device->SetTransform(D3DTS_WORLD, &I);
		Sno->render();
		Device->EndScene();
		// Swap the back and front buffers.
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,
		1920, 1080, false, D3DDEVTYPE_HAL, &Device))
	{
		MessageBox(0, TEXT("InitD3D() - FAILED"), 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, TEXT("Setup() - FAILED"), 0, 0);
		return 0;
	}
	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}
bool d3d::InitD3D(
	HINSTANCE hInstance,
	int width, int height,
	bool windowed,
	D3DDEVTYPE deviceType,
	IDirect3DDevice9** device)
{
	//
	// Create the main application window.
	//

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)d3d::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = TEXT("Direct3D9App");

	if (!RegisterClass(&wc))
	{
		MessageBox(0, TEXT("RegisterClass() - FAILED"), 0, 0);
		return false;
	}

	HWND hwnd = 0;
	hwnd = CreateWindow(TEXT("Direct3D9App"), TEXT("Direct3D9App"),
		WS_EX_TOPMOST,
		0, 0, width, height,
		0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/);

	if (!hwnd)
	{
		MessageBox(0, TEXT("CreateWindow() - FAILED"), 0, 0);
		return false;
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	//
	// Init D3D: 
	//

	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.

	IDirect3D9* d3d9 = 0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d9)
	{
		::MessageBox(0, TEXT("Direct3DCreate9() - FAILED"), 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);
	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.
	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
		&d3dpp,             // present parameters
		device);            // return created device

	if (FAILED(hr))
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			hwnd,
			vp,
			&d3dpp,
			device);

		if (FAILED(hr))
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, TEXT("CreateDevice() - FAILED"), 0, 0);
			return false;
		}
	}
	d3d9->Release(); // done with d3d9 object
	return true;
}

int d3d::EnterMsgLoop(bool(*ptr_display)(float timeDelta))
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			ptr_display(timeDelta);

			lastTime = currTime;
		}
	}
	return msg.wParam;
}
void calcfps(float timedelta)
{
	framecnt++;
	timeElapsed += timedelta;
	if (timeElapsed >= 1.0f)
	{
		FPS = (float)framecnt / timedelta;
		framecnt = 0;
		timeElapsed = 0;
	}
}
void createlight()
{
	Device->SetRenderState(D3DRS_LIGHTING, true);
	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = WHITE;
	dir.Specular = WHITE * 0.2f;
	dir.Ambient = WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);
	Device->SetLight(0, &dir);
	Device->LightEnable(0, true);
}
VOID createfps()
{
	D3DXFONT_DESC ds;
	ds.Weight = 500;
	ds.Height = 50;
	ds.Width = 10;
	ds.MipLevels = D3DX_DEFAULT;
	ds.Italic = true;
	ds.CharSet = DEFAULT_CHARSET;
	ds.OutputPrecision = 0;
	ds.Quality = 0;
	ds.PitchAndFamily = 0;
	wcscpy_s(ds.FaceName, 32, TEXT("Times New Roman"));
	D3DXCreateFontIndirect(Device, &ds, &font);
}

VOID meshXinit()
{
	ID3DXBuffer *matbuf;
	ID3DXBuffer *adjbuf;
	DWORD matnum;
	IDirect3DTexture9 *text;
	D3DXLoadMeshFromX(TEXT("bigship1.x"), D3DXMESH_MANAGED, Device, &adjbuf, &matbuf, 0, &matnum, &ship);
	D3DXMATERIAL *mtr = (D3DXMATERIAL *)matbuf->GetBufferPointer();
	for (unsigned int i = 0; i < matnum; i++)
	{
		mtr[i].MatD3D.Ambient = mtr[i].MatD3D.Diffuse;
		shipmat.push_back(mtr[i].MatD3D);
		if (mtr[i].pTextureFilename != NULL)
		{
			D3DXCreateTextureFromFileA(Device, mtr[i].pTextureFilename, &text);
			shiptex.push_back(text);
		}
		else
		{
			shiptex.push_back(0);
		}
	}
}
void getViewMatrix(D3DXMATRIX* V)
{
	// Keep camera's axes orthogonal to eachother
	D3DXVec3Normalize(&_look, &_look);

	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);

	// Build the view matrix:
	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	(*V)(0, 0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1, 0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2, 0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3, 0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;
}
void keycon()
{
	if (GetAsyncKeyState('W') & 0x8000f)
	{
		_pos = _pos + _look*0.1f;
	}
	if (GetAsyncKeyState('S') & 0x8000f)
	{
		_pos = _pos - _look*0.1f;
	}
	if (GetAsyncKeyState('D') & 0x8000f)
	{
		_pos = _pos + _right*0.1f;
	}
	if (GetAsyncKeyState('A') & 0x8000f)
	{
		_pos = _pos - _right*0.1f;
	}
	if (GetAsyncKeyState('Q') & 0x8000f)
	{
		_pos.y = _pos.y - 0.1f;
	}
	if (GetAsyncKeyState('E') & 0x8000f)
	{
		_pos.y = _pos.y + 0.1f;
	}
	if (GetAsyncKeyState('U') & 0x8000f)
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_right, -D3DX_PI / 360);

		// rotate _up and _look around _right vector
		D3DXVec3TransformCoord(&_up, &_up, &T);
		D3DXVec3TransformCoord(&_look, &_look, &T);
	}
	if (GetAsyncKeyState('N') & 0x8000f)
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_right, D3DX_PI / 360);

		// rotate _up and _look around _right vector
		D3DXVec3TransformCoord(&_up, &_up, &T);
		D3DXVec3TransformCoord(&_look, &_look, &T);
	}
	if (GetAsyncKeyState('H') & 0x8000f)
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_up, -D3DX_PI / 360);
		D3DXVec3TransformCoord(&_right, &_right, &T);
		D3DXVec3TransformCoord(&_look, &_look, &T);
	}
	if (GetAsyncKeyState('J') & 0x8000f)
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_up, D3DX_PI / 360);
		D3DXVec3TransformCoord(&_right, &_right, &T);
		D3DXVec3TransformCoord(&_look, &_look, &T);
	}
}
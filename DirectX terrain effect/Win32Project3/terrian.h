#include<iostream>
#include<vector>
#include<string>
#include <fstream>
IDirect3DDevice9* Device = 0;
struct vertexinfo
{
	float x, y, z;
	float u, v;
};
float GetRandomFloat(float lowBound, float highBound);
void changeattribute(int num, DWORD *attributebuffer, vertexinfo* ve);
class terrian
{
public:
	terrian(float w, float d, int x, int z);
	void initvertex(float min, float max);
	int sumver();
	int pointx;
	int pointz;
	void initvertexbyRaw(std::string name, float maxhigh);
	std::vector<vertexinfo> sa;
	static float maxhigh;
private:
	void initdistance();
	float width;
	float depth;
	float distanceperpointx;
	float distanceperpointz;
};
float terrian::maxhigh = 0;
terrian::terrian(float w, float d, int x, int z)
{
	width = w;
	depth = d;
	pointx = x;
	pointz = z;
	initdistance();
}
void terrian::initdistance()
{
	distanceperpointx = width / (pointx-1);
	distanceperpointz = depth / (pointz-1);
}
void terrian::initvertex(float min,float max)
{
	for (int i = 0; i < pointz; i++)
	{
		for (int j = 0; j < pointx; j++)
		{
			vertexinfo vi;
			vi.x = -width / 2 + j*distanceperpointx;
			vi.y = GetRandomFloat(min, max);
			vi.z = depth / 2 - i*distanceperpointz;
			sa.push_back(vi);
		}
	}
}
int terrian::sumver()
{
	return pointx*pointz;
}
void terrian::initvertexbyRaw(std::string name,float maxhigh)
{
	int num=0;
	std::vector<byte> buf(sumver());
	std::ifstream input(name.c_str(), std::ios_base::binary);
	input.read((char*)&buf[0], buf.size());
	terrian::maxhigh = maxhigh;
	for (int j = 0; j < sumver(); j++)
	{
		sa[j].y = buf[j] * (maxhigh / 255);
	}
	input.close();
}
float GetRandomFloat(float lowBound, float highBound)
{
	if (lowBound >= highBound) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f;

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound;
}
LPD3DXMESH createMeshbyTerrian(terrian te)
{
	LPD3DXMESH mesh;
	static int num = 0;
	static int texnum = 0;
	DWORD facenum = ((te.pointx - 1)*(te.pointz - 1) * 2);
	DWORD vernum = (te.pointx)*(te.pointz);
	D3DXCreateMeshFVF(facenum, vernum, D3DXMESH_MANAGED, D3DFVF_XYZ|D3DFVF_TEX1, Device, &mesh);
	vertexinfo *ve;
	mesh->LockVertexBuffer(0, (void**)&ve);
	for (int j = 0; j < te.pointx*te.pointz; j++)
	{
		ve[j].x = te.sa[j].x;
		ve[j].y = te.sa[j].y;
		ve[j].z = te.sa[j].z;
	}
	for (int i = 0; i <= te.pointz - 1; i++)
	{
		for (int j = 0; j <= te.pointz - 1; j++)
		{
			if (i % 2 == 0)
			{
				ve[texnum].u= 0;
				ve[texnum].v= (float)(j % 2);
			}
			else
			{
				ve[texnum].u = 1;
				ve[texnum].v = (float)(j % 2);
			}
			texnum++;
		}
	}
	WORD *in = 0;
	DWORD *attributebuffer;
	mesh->LockAttributeBuffer(0, &attributebuffer);
	mesh->LockIndexBuffer(0, (void**)&in);
	for (int i = 0; i < te.pointz - 1; i++)
	{
		for (int j = 0; j < te.pointx - 1; j++)
		{
			in[num] = j + (i * te.pointx);
			in[num + 1] = j + 1 + (i * te.pointx);
			in[num + 2] = j + te.pointx + (i * te.pointx);
			changeattribute(in[num], attributebuffer, ve);
			in[num + 3] = j + 1 + (i * te.pointx);
			in[num + 4] = j + 1 + te.pointx + (i * te.pointx);
			in[num + 5] = j + te.pointx + (i * te.pointx);
			changeattribute(in[num + 3], attributebuffer, ve);
			num += 6;
		}
	}
	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
	mesh->UnlockAttributeBuffer();
	return mesh;
}
void changeattribute(int num, DWORD *attributebuffer, vertexinfo* ve)
{
	static int sum = 0;
	if (ve[num].y / (terrian::maxhigh / 255) < 90.0f)
	{
		attributebuffer[sum] = 0;
	}
	else if (ve[num].y / (terrian::maxhigh / 255) < 180.0f)
	{
		attributebuffer[sum] = 1;
	}
	else if (ve[num].y / (terrian::maxhigh / 255) < 300.0f)
	{
		attributebuffer[sum] = 2;
	}
	sum++;
}
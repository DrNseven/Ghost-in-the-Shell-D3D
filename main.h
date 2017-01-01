#include <windows.h>
#include <vector>
#include <fstream>
#include <time.h>

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include "DXSDK\d3dx9.h"
#if defined _M_X64
#pragma comment(lib, "DXSDK/x64/d3dx9.lib") 
#elif defined _M_IX86
#pragma comment(lib, "DXSDK/x86/d3dx9.lib")
#endif
 
#pragma comment(lib, "winmm.lib")
#include "MinHook/include/MinHook.h" //detour
using namespace std;

#pragma warning (disable: 4244) //

//==========================================================================================================================

HMODULE dllHandle;

//Stride
UINT Stride;

//vertexshader
IDirect3DVertexShader9* vShader;
UINT vSize;

//pixelshader
IDirect3DPixelShader9* pShader;
UINT pSize;

//vertexshaderconstantf
UINT mStartRegister;
UINT mVector4fCount;
//float* pConstantDataFloat;

//elementcount
D3DVERTEXELEMENT9 decl[MAXD3DDECLLENGTH];
UINT numElements;

bool InitOnce = true;

D3DVIEWPORT9 Viewport;			//use this viewport
float ScreenCenterX;
float ScreenCenterY;

//logger
bool logger = false;
float countnum = -1;

//visual settings
int wallhack = 1;				//wallhack
int chams = 0;					//chams
int esp = 0;					//esp

//aimbot settings
int aimbot = 1;
int aimkey = 2;
DWORD Daimkey = VK_RBUTTON;		//aimkey
int aimsens = 2;				//aim sensitivity
int aimfov = 2;					//aim fov in % 
//int aimheight = 1;			//aim height

//autoshoot settings
int autoshoot = 1;
bool IsPressed = false;			//

//timer
DWORD frametime = timeGetTime();

//==========================================================================================================================

// getdir & log
char dlldir[320];
char* GetDirectoryFile(char *filename)
{
	static char path[320];
	strcpy_s(path, dlldir);
	strcat_s(path, filename);
	return path;
}

void Log(const char *fmt, ...)
{
	if (!fmt)	return;

	char		text[4096];
	va_list		ap;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

	ofstream logfile(GetDirectoryFile("log.txt"), ios::app);
	if (logfile.is_open() && text)	logfile << text << endl;
	logfile.close();
}

//==========================================================================================================================

//get distance
float GetDistance(float Xx, float Yy, float xX, float yY)
{
	return sqrt((yY - Yy) * (yY - Yy) + (xX - Xx) * (xX - Xx));
}

//aim worldtoscreen
struct AimInfo_t
{
	float vOutX, vOutY;
	INT       iTeam;
	float CrosshairDistance;
};
std::vector<AimInfo_t>AimInfo;
//float RealDistance;

void DrawPoint(LPDIRECT3DDEVICE9 pDevice, int x, int y, int w, int h, D3DCOLOR color)
{
	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}

// Registers:
//
//   Name               Reg   Size
//   ------------------ ----- ----
//   g_World            c0       4
//   g_ViewProj         c4       4
//   g_MaterialEmissive c8       1
//   g_MaterialDiffuse  c9       1
//   g_MaterialAmbient  c10      1
//   g_AmbientLight     c11      1

void AddAim(LPDIRECT3DDEVICE9 Device, int iTeam, int aimvecX, int aimvecY, int aimvecZ)
{
	//D3DVIEWPORT9 Viewport;
	//Device->GetViewport(&Viewport);

	D3DXMATRIX Projection, View, World;
	D3DXVECTOR3 vOut(0, 0, 0), vIn(aimvecX, aimvecY, aimvecZ);//0, 0, 30

	Device->GetVertexShaderConstantF(4, Projection, 4);
	Device->GetVertexShaderConstantF(0, View, 4);

	D3DXMatrixTranspose(&Projection, &Projection);
	D3DXMatrixTranspose(&View, &View);

	D3DXMatrixIdentity(&World);
	D3DXVec3Project(&vOut, &vIn, &Viewport, &Projection, &View, &World);

	AimInfo_t pAimInfo = { static_cast<float>(vOut.x), static_cast<float>(vOut.y), iTeam };
	AimInfo.push_back(pAimInfo);
}

//=====================================================================================================================

LPD3DXSPRITE lpSprite, lpSprite2, lpSprite3 = NULL;
LPDIRECT3DTEXTURE9 lpSpriteImage, lpSpriteImage2, lpSpriteImage3 = NULL;
bool bSpriteCreated, bSpriteCreated2, bSpriteCreated3 = false;

bool CreateOverlaySprite(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr;

	hr = D3DXCreateTextureFromFile(pd3dDevice, GetDirectoryFile("team1.png"), &lpSpriteImage); //png in hack dir
	if (FAILED(hr))
	{
		//Log("D3DXCreateTextureFromFile failed");
		bSpriteCreated = false;
		return false;
	}

	hr = D3DXCreateSprite(pd3dDevice, &lpSprite);
	if (FAILED(hr))
	{
		//Log("D3DXCreateSprite failed");
		bSpriteCreated = false;
		return false;
	}

	bSpriteCreated = true;

	return true;
}

bool CreateOverlaySprite2(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr;

	hr = D3DXCreateTextureFromFile(pd3dDevice, GetDirectoryFile("menu.png"), &lpSpriteImage2); //png in hack dir
	if (FAILED(hr))
	{
		//Log("D3DXCreateTextureFromFile failed");
		bSpriteCreated2 = false;
		return false;
	}

	hr = D3DXCreateSprite(pd3dDevice, &lpSprite2);
	if (FAILED(hr))
	{
		//Log("D3DXCreateSprite failed");
		bSpriteCreated2 = false;
		return false;
	}

	bSpriteCreated2 = true;

	return true;
}

bool CreateOverlaySprite3(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr;

	hr = D3DXCreateTextureFromFile(pd3dDevice, GetDirectoryFile("team3.png"), &lpSpriteImage3); //png in hack dir
	if (FAILED(hr))
	{
		//Log("D3DXCreateTextureFromFile failed");
		bSpriteCreated3 = false;
		return false;
	}

	hr = D3DXCreateSprite(pd3dDevice, &lpSprite3);
	if (FAILED(hr))
	{
		//Log("D3DXCreateSprite failed");
		bSpriteCreated3 = false;
		return false;
	}

	bSpriteCreated3 = true;

	return true;
}

// COM utils
template<class COMObject>
void SafeRelease(COMObject*& pRes)
{
	IUnknown *unknown = pRes;
	if (unknown)
	{
		unknown->Release();
	}
	pRes = NULL;
}

// This will get called before Device::Clear(). If the device has been reset
// then all the work surfaces will be created again.
void PreClear(IDirect3DDevice9* device)
{
	if (!bSpriteCreated)
		CreateOverlaySprite(device);

	if (!bSpriteCreated2)
		CreateOverlaySprite2(device);

	if (!bSpriteCreated3)
		CreateOverlaySprite3(device);
}

// Delete work surfaces when device gets reset
void DeleteRenderSurfaces()
{
	if (lpSprite != NULL)
	{
		//Log("SafeRelease(lpSprite)");
		SafeRelease(lpSprite);
	}

	if (lpSprite2 != NULL)
	{
		//Log("SafeRelease(lpSprite2)");
		SafeRelease(lpSprite2);
	}

	if (lpSprite3 != NULL)
	{
		//Log("SafeRelease(lpSprite3)");
		SafeRelease(lpSprite3);
	}

	bSpriteCreated = false;
	bSpriteCreated2 = false;
	bSpriteCreated3 = false;
}

// This gets called right before the frame is presented on-screen - Device::Present().
// First, create the display text, FPS and info message, on-screen. Then then call
// CopySurfaceToTextureBuffer() to downsample the image and copy to shared memory
void PrePresent(IDirect3DDevice9* Device, int cx, int cy)
{
	int textOffsetLeft;

	//draw sprite
	if (bSpriteCreated)
	{
		if (lpSprite != NULL)
		{
			D3DXVECTOR3 position;
			position.x = (float)cx;
			position.y = (float)cy;
			position.z = 0.0f;

			textOffsetLeft = (int)position.x; //for later to offset text from image

			lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
			lpSprite->Draw(lpSpriteImage, NULL, NULL, &position, 0xFFFFFFFF);
			lpSprite->End();
		}
	}

	// draw text
}

void PrePresent2(IDirect3DDevice9* Device, int cx, int cy)
{
	int textOffsetLeft;

	//draw sprite
	if (bSpriteCreated2)
	{
		if (lpSprite2 != NULL)
		{
			D3DXVECTOR3 position;
			position.x = (float)cx;
			position.y = (float)cy;
			position.z = 0.0f;

			textOffsetLeft = (int)position.x; //for later to offset text from image

			lpSprite2->Begin(D3DXSPRITE_ALPHABLEND);
			lpSprite2->Draw(lpSpriteImage2, NULL, NULL, &position, 0xFFFFFFFF);
			lpSprite2->End();
		}
	}

	// draw text
}

void PrePresent3(IDirect3DDevice9* Device, int cx, int cy)
{
	int textOffsetLeft;

	//draw sprite
	if (bSpriteCreated3)
	{
		if (lpSprite3 != NULL)
		{
			D3DXVECTOR3 position;
			position.x = (float)cx;
			position.y = (float)cy;
			position.z = 0.0f;

			textOffsetLeft = (int)position.x; //for later to offset text from image

			lpSprite3->Begin(D3DXSPRITE_ALPHABLEND);
			lpSprite3->Draw(lpSpriteImage3, NULL, NULL, &position, 0xFFFFFFFF);
			lpSprite3->End();
		}
	}

	// draw text
}

//==========================================================================================================================

//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Saves Menu Item states for later Restoration
//-----------------------------------------------------------------------------

//void Save(char* szSection, char* szKey, int iValue, LPCSTR file)
//{
//char szValue[255];
//sprintf_s(szValue, "%d", iValue);
//WritePrivateProfileString(szSection, szKey, szValue, file);
//}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Loads Menu Item States From Previously Saved File
//-----------------------------------------------------------------------------

//int Load(char* szSection, char* szKey, int iDefaultValue, LPCSTR file)
//{
//int iResult = GetPrivateProfileInt(szSection, szKey, iDefaultValue, file);
//return iResult;
//}

#include <string>
#include <fstream>
void SaveSettings()
{
	ofstream fout;
	fout.open(GetDirectoryFile("gitssettings.ini"), ios::trunc);
	fout << "Wallhack " << wallhack << endl;
	fout << "Chams " << chams << endl;
	fout << "Aimbot " << aimbot << endl;
	fout << "Aimkey " << aimkey << endl;
	fout << "Aimsens " << aimsens << endl;
	//fout << "Aimheight " << aimheight << endl;
	fout << "Aimfov " << aimfov << endl;
	fout << "Esp " << esp << endl;
	fout << "Autoshoot " << autoshoot << endl;
	fout.close();
}

void LoadSettings()
{
	ifstream fin;
	string Word = "";
	fin.open(GetDirectoryFile("gitssettings.ini"), ifstream::in);
	fin >> Word >> wallhack;
	fin >> Word >> chams;
	fin >> Word >> aimbot;
	fin >> Word >> aimkey;
	fin >> Word >> aimsens;
	//fin >> Word >> aimheight;
	fin >> Word >> aimfov;
	fin >> Word >> esp;
	fin >> Word >> autoshoot;
	fin.close();
}

//==========================================================================================================================

// colors
#define Green				D3DCOLOR_ARGB(255, 000, 255, 000)
#define Red					D3DCOLOR_ARGB(255, 255, 000, 000)
#define Blue				D3DCOLOR_ARGB(255, 000, 000, 255)
#define Orange				D3DCOLOR_ARGB(255, 255, 165, 000)
#define Yellow				D3DCOLOR_ARGB(255, 255, 255, 000)
#define Pink				D3DCOLOR_ARGB(255, 255, 192, 203)
#define Cyan				D3DCOLOR_ARGB(255, 000, 255, 255)
#define Purple				D3DCOLOR_ARGB(255, 160, 032, 240)
#define Black				D3DCOLOR_ARGB(255, 000, 000, 000) 
#define White				D3DCOLOR_ARGB(255, 255, 255, 255)
#define Grey				D3DCOLOR_ARGB(255, 112, 112, 112)
#define SteelBlue			D3DCOLOR_ARGB(255, 033, 104, 140)
#define LightSteelBlue		D3DCOLOR_ARGB(255, 201, 255, 255)
#define LightBlue			D3DCOLOR_ARGB(255, 026, 140, 306)
#define Salmon				D3DCOLOR_ARGB(255, 196, 112, 112)
#define Brown				D3DCOLOR_ARGB(255, 168, 099, 020)
#define Teal				D3DCOLOR_ARGB(255, 038, 140, 140)
#define Lime				D3DCOLOR_ARGB(255, 050, 205, 050)
#define ElectricLime		D3DCOLOR_ARGB(255, 204, 255, 000)
#define Gold				D3DCOLOR_ARGB(255, 255, 215, 000)
#define OrangeRed			D3DCOLOR_ARGB(255, 255, 69, 0)
#define GreenYellow			D3DCOLOR_ARGB(255, 173, 255, 047)
#define AquaMarine			D3DCOLOR_ARGB(255, 127, 255, 212)
#define SkyBlue				D3DCOLOR_ARGB(255, 000, 191, 255)
#define SlateBlue			D3DCOLOR_ARGB(255, 132, 112, 255)
#define Crimson				D3DCOLOR_ARGB(255, 220, 020, 060)
#define DarkOliveGreen		D3DCOLOR_ARGB(255, 188, 238, 104)
#define PaleGreen			D3DCOLOR_ARGB(255, 154, 255, 154)
#define DarkGoldenRod		D3DCOLOR_ARGB(255, 255, 185, 015)
#define FireBrick			D3DCOLOR_ARGB(255, 255, 048, 048)
#define DarkBlue			D3DCOLOR_ARGB(255, 000, 000, 204)
#define DarkerBlue			D3DCOLOR_ARGB(255, 000, 000, 153)
#define DarkYellow			D3DCOLOR_ARGB(255, 255, 204, 000)
#define LightYellow			D3DCOLOR_ARGB(255, 255, 255, 153)
#define DarkOutline			D3DCOLOR_ARGB(255, 37,   48,  52)
#define TBlack				D3DCOLOR_ARGB(180, 000, 000, 000) 


// menu

int MenuSelection = 0;
int Current = true;

int PosX = 30;
int PosY = 27;

int Show = false; //off by default

POINT cPos;

#define ItemColorOn Green
#define ItemColorOff Red
#define ItemCurrent White
#define GroupColor Yellow
#define KategorieFarbe Yellow
#define ItemText White

LPD3DXFONT pFont; //font
bool m_bCreated = false;

int CheckTabs(int x, int y, int w, int h)
{
	if (Show)
	{
		GetCursorPos(&cPos);
		ScreenToClient(GetForegroundWindow(), &cPos);
		if (cPos.x > x && cPos.x < x + w && cPos.y > y && cPos.y < y + h)
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 1)
			{
				return 1;
			}
			return 2;
		}
	}
	return 0;
}

HRESULT DrawRectangle(LPDIRECT3DDEVICE9 Device, FLOAT x, FLOAT y, FLOAT w, FLOAT h, DWORD Color)
{
	HRESULT hRet;

	const DWORD D3D_FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	struct Vertex
	{
		float x, y, z, ht;
		DWORD vcolor;
	}
	V[4] =
	{
		{ x, (y + h), 0.0f, 0.0f, Color },
		{ x, y, 0.0f, 0.0f, Color },
		{ (x + w), (y + h), 0.0f, 0.0f, Color },
		{ (x + w), y, 0.0f, 0.0f, Color }
	};

	hRet = D3D_OK;

	if (SUCCEEDED(hRet))
	{
		Device->SetPixelShader(0); //fix black color
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		Device->SetFVF(D3D_FVF);
		Device->SetTexture(0, NULL);
		hRet = Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex));
	}

	return hRet;
}

VOID DrawBorder(LPDIRECT3DDEVICE9 Device, INT x, INT y, INT w, INT h, INT px, DWORD BorderColor)
{
	DrawRectangle(Device, x, (y + h - px), w, px, BorderColor);
	DrawRectangle(Device, x, y, px, h, BorderColor);
	DrawRectangle(Device, x, y, w, px, BorderColor);
	DrawRectangle(Device, (x + w - px), y, px, h, BorderColor);
}

VOID DrawBoxWithBorder(LPDIRECT3DDEVICE9 Device, INT x, INT y, INT w, INT h, DWORD BoxColor, DWORD BorderColor)
{
	DrawRectangle(Device, x, y, w, h, BoxColor);
	DrawBorder(Device, x, y, w, h, 1, BorderColor);
}

VOID DrawBox(LPDIRECT3DDEVICE9 Device, INT x, INT y, INT w, INT h, DWORD BoxColor)
{
	DrawBorder(Device, x, y, w, h, 1, BoxColor);
}

void WriteText(int x, int y, DWORD color, char *text)
{
	RECT rect;
	SetRect(&rect, x, y, x, y);
	pFont->DrawText(0, text, -1, &rect, DT_NOCLIP | DT_LEFT, color);
}

void lWriteText(int x, int y, DWORD color, char *text)
{
	RECT rect;
	SetRect(&rect, x, y, x, y);
	pFont->DrawText(0, text, -1, &rect, DT_NOCLIP | DT_RIGHT, color);
}

void cWriteText(int x, int y, DWORD color, char *text)
{
	RECT rect;
	SetRect(&rect, x, y, x, y);
	pFont->DrawText(0, text, -1, &rect, DT_NOCLIP | DT_CENTER, color);
}

HRESULT DrawString(LPD3DXFONT pFont, INT X, INT Y, DWORD dColor, CONST PCHAR cString, ...)
{
	HRESULT hRet;

	CHAR buf[512] = { NULL };
	va_list ArgumentList;
	va_start(ArgumentList, cString);
	_vsnprintf_s(buf, sizeof(buf), sizeof(buf) - strlen(buf), cString, ArgumentList);
	va_end(ArgumentList);

	RECT rc[2];
	SetRect(&rc[0], X, Y, X, 0);
	SetRect(&rc[1], X, Y, X + 50, 50);

	hRet = D3D_OK;

	if (SUCCEEDED(hRet))
	{
		pFont->DrawTextA(NULL, buf, -1, &rc[0], DT_NOCLIP, 0xFF000000);
		hRet = pFont->DrawTextA(NULL, buf, -1, &rc[1], DT_NOCLIP, dColor);
	}

	return hRet;
}

void Category(LPDIRECT3DDEVICE9 pDevice, char *text)
{
	if (Show)
	{
		int Check = CheckTabs(PosX, PosY + (Current * 15), 190, 10);
		DWORD ColorText;

		ColorText = KategorieFarbe;

		if (Check == 2)
			ColorText = ItemCurrent;

		if (MenuSelection == Current)
			ColorText = ItemCurrent;

		WriteText(PosX - 5, PosY + (Current * 15) - 1, ColorText, text);
		lWriteText(PosX + 175, PosY + (Current * 15) - 1, ColorText, "[-]");
		Current++;
	}
}

void AddItem(LPDIRECT3DDEVICE9 pDevice, char *text, int &var, char **opt, int MaxValue)
{
	if (Show)
	{
		int Check = CheckTabs(PosX, PosY + (Current * 15), 190, 10);
		DWORD ColorText;

		if (var)
		{
			DrawBox(pDevice, PosX, PosY + (Current * 15), 10, 10, Green);
			ColorText = ItemColorOn;
		}
		if (var == 0)
		{
			DrawBox(pDevice, PosX, PosY + (Current * 15), 10, 10, Red);
			ColorText = ItemColorOff;
		}

		if (Check == 1)
		{
			var++;
			if (var > MaxValue)
				var = 0;
		}

		if (Check == 2)
			ColorText = ItemCurrent;

		if (MenuSelection == Current)
		{
			if (GetAsyncKeyState(VK_RIGHT) & 1)
			{
				var++;
				if (var > MaxValue)
					var = 0;
			}
			else if (GetAsyncKeyState(VK_LEFT) & 1)
			{
				var--;
				if (var < 0)
					var = MaxValue;
			}
		}

		if (MenuSelection == Current)
			ColorText = ItemCurrent;

		WriteText(PosX + 13, PosY + (Current * 15) - 1, ColorText, text);
		lWriteText(PosX + 148, PosY + (Current * 15) - 1, ColorText, opt[var]);
		Current++;
	}
}

//==========================================================================================================================

// menu part
char *opt_OnOff[] = { "[OFF]", "[ON]" };
char *opt_Chams[] = { "[OFF]", "[ON]" };
char *opt_Teams[] = { "[OFF]", "[ON]" };
char *opt_Keys[] = { "[OFF]", "[Shift]", "[RMouse]", "[LMouse]", "[Ctrl]", "[Alt]", "[Space]", "[X]", "[C]" };
char *opt_Sensitivity[] = { "[1]", "[2]", "[3]", "[4]", "[5]", "[6]", "[7]", "[8]", "[9]" };
//char *opt_Aimheight[] = { "[0]", "[1]", "[2]" };
char *opt_Aimfov[] = { "[0]", "[10%]", "[20%]", "[30%]", "[40%]", "[50%]", "[60%]", "[70%]", "[80%]", "[90%]" };
char *opt_Autoshoot[] = { "[OFF]", "[OnKeyDown]" };

void BuildMenu(LPDIRECT3DDEVICE9 pDevice)
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		Show = !Show;

		//save settings
		SaveSettings();
	}

	if (Show && pFont)
	{
		if (GetAsyncKeyState(VK_UP) & 1)
			MenuSelection--;

		if (GetAsyncKeyState(VK_DOWN) & 1)
			MenuSelection++;

		//Background
		//DrawPoint(pDevice, 25, 38, 177, 124, TBlack);
		PrePresent2(pDevice, 48, 38); //draw menu background png

		DrawBox(pDevice, 20, 15, 168, 20, DarkOutline);
		cWriteText(112, 18, White, "GitS D3D");
		DrawBox(pDevice, 20, 34, 168, Current * 15, DarkOutline);

		Current = 1;
		//Category(pDevice, " [D3D]");
		AddItem(pDevice, " Wallhack", wallhack, opt_OnOff, 1);
		AddItem(pDevice, " Chams", chams, opt_Chams, 1);
		AddItem(pDevice, " Esp", esp, opt_Teams, 1);
		AddItem(pDevice, " Aimbot", aimbot, opt_Teams, 1);
		AddItem(pDevice, " Aimkey", aimkey, opt_Keys, 8);
		AddItem(pDevice, " Aimsens", aimsens, opt_Sensitivity, 8);
		//AddItem(pDevice, " Aimheight", aimheight, opt_Aimheight, 2);
		AddItem(pDevice, " Aimfov", aimfov, opt_Aimfov, 9);
		AddItem(pDevice, " Autoshoot", autoshoot, opt_Autoshoot, 2);

		//if (MenuSelection >= Current)
			//MenuSelection = 1;

		if (MenuSelection > 8)
			MenuSelection = 1;//Current;

		if (MenuSelection < 1)
			MenuSelection = 8;//Current;
	}
}

//=====================================================================================================================
/*
HRESULT GenerateTexture(LPDIRECT3DDEVICE9 Device, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
{
	if (FAILED(Device->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)))
		return E_FAIL;

	WORD colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12)
		| (WORD)(((colour32 >> 20) & 0xF) << 8)
		| (WORD)(((colour32 >> 12) & 0xF) << 4)
		| (WORD)(((colour32 >> 4) & 0xF) << 0);

	D3DLOCKED_RECT d3dlr;
	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	WORD *pDst16 = (WORD*)d3dlr.pBits;

	for (int xy = 0; xy < 8 * 8; xy++)
		*pDst16++ = colour16;

	(*ppD3Dtex)->UnlockRect(0);

	return S_OK;
}

IDirect3DPixelShader9 *shadRed;
IDirect3DPixelShader9 *shadGreen;
//generate shader
HRESULT GenerateShader(IDirect3DDevice9 *pDevice, IDirect3DPixelShader9 **pShader, float r, float g, float b, float a, bool setzBuf)
{
	char szShader[256];
	ID3DXBuffer *pShaderBuf = NULL;
	D3DCAPS9 caps;
	pDevice->GetDeviceCaps(&caps);
	int PXSHVER1 = (D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion));
	int PXSHVER2 = (D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion));
	if (setzBuf)
		sprintf_s(szShader, "ps.%d.%d\ndef c0, %f, %f, %f, %f\nmov oC0,c0\nmov oDepth, c0.x", PXSHVER1, PXSHVER2, r, g, b, a);
	else
		sprintf_s(szShader, "ps.%d.%d\ndef c1, %f, %f, %f, %f\nmov oC1,c1", PXSHVER1, PXSHVER2, r, g, b, a);
	D3DXAssembleShader(szShader, sizeof(szShader), NULL, NULL, 0, &pShaderBuf, NULL);
	if (FAILED(pDevice->CreatePixelShader((const DWORD*)pShaderBuf->GetBufferPointer(), pShader)))return E_FAIL;
	return S_OK;
}
*/
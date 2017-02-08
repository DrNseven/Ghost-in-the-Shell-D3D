//Ghost in the Shell : Stand Alone Complex - First Assault Online
//D3D Multihack beta 0.4

/*
How to compile:
- download and install "Microsoft Visual Studio Express 2015 for Windows DESKTOP" https://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx

- open ghostd3d.sln or ghostd3d.vcxproj with Visual Studio 2015 (Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\WDExpress.exe)
- select Release x86(32bit) 
- compile dll, press f7 or click the green triangle
- dll will be created in Release folder

If you share your dll with others, remove dependecy on vs runtime before compiling:
- click: project -> properties -> configuration properties -> C/C++ -> code generation -> runtime library: Multi-threaded (/MT)
*/


#include "main.h" //less important stuff & helper funcs here

typedef HRESULT(APIENTRY *SetVertexShaderConstantF)(IDirect3DDevice9*, UINT, const float*, UINT);
HRESULT APIENTRY SetVertexShaderConstantF_hook(IDirect3DDevice9*, UINT, const float*, UINT);
SetVertexShaderConstantF SetVertexShaderConstantF_orig = 0;

typedef HRESULT(APIENTRY *DrawIndexedPrimitive)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
HRESULT APIENTRY DrawIndexedPrimitive_hook(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
DrawIndexedPrimitive DrawIndexedPrimitive_orig = 0;

typedef HRESULT(APIENTRY *SetVertexDeclaration)(IDirect3DDevice9*, IDirect3DVertexDeclaration9 *);
HRESULT APIENTRY SetVertexDeclaration_hook(IDirect3DDevice9*, IDirect3DVertexDeclaration9 *);
SetVertexDeclaration SetVertexDeclaration_orig = 0;

typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
HRESULT APIENTRY EndScene_hook(IDirect3DDevice9*);
EndScene EndScene_orig = 0;

typedef HRESULT(APIENTRY *Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT APIENTRY Reset_hook(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Reset Reset_orig = 0;

typedef HRESULT(APIENTRY *SetStreamSource)(IDirect3DDevice9*, UINT, IDirect3DVertexBuffer9*, UINT, UINT);
HRESULT APIENTRY SetStreamSource_hook(IDirect3DDevice9*, UINT, IDirect3DVertexBuffer9*, UINT, UINT);
SetStreamSource SetStreamSource_orig = 0;

typedef HRESULT(APIENTRY *SetVertexShader)(IDirect3DDevice9*, IDirect3DVertexShader9*);
HRESULT APIENTRY SetVertexShader_hook(IDirect3DDevice9*, IDirect3DVertexShader9*);
SetVertexShader SetVertexShader_orig = 0;

typedef HRESULT(APIENTRY *SetPixelShader)(IDirect3DDevice9*, IDirect3DPixelShader9*);
HRESULT APIENTRY SetPixelShader_hook(IDirect3DDevice9*, IDirect3DPixelShader9*);
SetPixelShader SetPixelShader_orig = 0;

typedef HRESULT(APIENTRY *SetTexture)(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9*);
HRESULT APIENTRY SetTexture_hook(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9*);
SetTexture SetTexture_orig = 0;

typedef HRESULT(APIENTRY *SetViewport)(IDirect3DDevice9*, CONST D3DVIEWPORT9*);
HRESULT APIENTRY SetViewport_hook(IDirect3DDevice9*, CONST D3DVIEWPORT9*);
SetViewport SetViewport_orig = 0;

//=====================================================================================================================

//this may or may not be complete
#define models ((Stride == 12 || Stride == 72 || Stride == 80) && \
(mVector4fCount == 99 || mVector4fCount == 101))

//#define models ((Stride == 12 || Stride == 72 || Stride == 80) && \
//(numElements == 8 || numElements == 9 || numElements == 10) && \
//(mVector4fCount == 99 || mVector4fCount == 101))

//the eyes to aim at
#define eyes (Stride == 32 && NumVertices == 4 && primCount == 2 && numElements == 4 && decl->Type == 2 && vSize == 536 && pSize == 872 && mStartRegister == 0 && mVector4fCount == 9) //glowing light around the eyes

HRESULT APIENTRY DrawIndexedPrimitive_hook(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	//wallhack, chams
	if (wallhack == 1 || chams == 1)
		if (models||eyes) //models, eyes
		{
			//wallhack 
			pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

			//chams 
			if (chams == 1)
			{
				float vals[] =
				{
					//340
					((pSize*vSize >> 24) & (DWORD)2) / 1.0f,
					((pSize*vSize >> 16) & (DWORD)2) / 1.0f,
					((pSize*vSize >> 8) & (DWORD)2) / 1.0f,
					1.0f
				};
				//pDevice->SetPixelShaderConstantF(7, vals, 1);
				pDevice->SetPixelShaderConstantF(18, vals, 2);//8, 9, 13, 14
			}

			//chams 
			if (chams == 2)
			{
				float vals[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
				//pDevice->SetPixelShaderConstantF(7, vals, 1);
				pDevice->SetPixelShaderConstantF(18, vals, 2);
			}

			DrawIndexedPrimitive_orig(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

			if (chams == 1)
			{
				float vals[] =
				{
					((pSize*vSize >> 24) & (DWORD)2) / 1.0f,
					((pSize*vSize >> 16) & (DWORD)2) / 1.0f,
					((pSize*vSize >> 8) & (DWORD)2) / 1.0f,
					1.0f
				};
				//pDevice->SetPixelShaderConstantF(7, vals, 1);
				pDevice->SetPixelShaderConstantF(18, vals, 2);
			}

			if (chams == 2)
			{
				float vals[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
				//pDevice->SetPixelShaderConstantF(7, vals, 1);
				pDevice->SetPixelShaderConstantF(18, vals, 2);
			}

			pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		}

	//aimbot, esp
	if (aimbot == 1 || esp > 0)
		if (eyes)
		{
			AddAim(pDevice, 0, aimheightxy, 0);
		}


	/*
	//get ptex (may reduce fps, only use if needed)
	HRESULT hr;
	if (models|| eyes) //reduce fps loss1
	{
		hr = pDevice->GetTexture(0, &pTexture);
		if (FAILED(hr)) { goto out; }
		if (pTexture != nullptr)
		{
			CurrentTex = static_cast<IDirect3DTexture9*>(pTexture);

			D3DSURFACE_DESC surfaceDesc;

			if (FAILED(CurrentTex->GetLevelDesc(0, &surfaceDesc)))
			{
				//Log("surfaceDesc failed");
				goto out;
			}

			if (SUCCEEDED(CurrentTex->GetLevelDesc(0, &surfaceDesc)))//
				if (surfaceDesc.Pool == D3DPOOL_MANAGED && CurrentTex->GetType() == D3DRTYPE_TEXTURE) //reduce fps loss2
				{
					sWidth = surfaceDesc.Width;
					sHeight = surfaceDesc.Height;
					//dFormat = surfaceDesc.Format;

					D3DLOCKED_RECT pLockedRect;

					if (CurrentTex->LockRect(0, &pLockedRect, NULL, D3DLOCK_READONLY | D3DLOCK_DONOTWAIT | D3DLOCK_NOSYSLOCK) == S_OK)
						//pCurrentTex->LockRect(0, &pLockedRect, NULL, D3DLOCK_NOOVERWRITE | D3DLOCK_READONLY);
						//pCurrentTex->LockRect(0, &pLockedRect, NULL, D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK | D3DLOCK_DONOTWAIT | D3DLOCK_READONLY);

						if (pLockedRect.pBits != NULL)
						{
							// get crc from the algorithm
							qCRC = QChecksum((DWORD*)pLockedRect.pBits, 12);// pLockedRect.Pitch);
							//pCurrentTex->UnlockRect(0);
						}
					CurrentTex->UnlockRect(0);
				}

		out:
			//uh, should CurrentTex be released
			if (pTexture != NULL) { pTexture->Release(); pTexture = NULL; }
		}
		if (pTexture != NULL) { pTexture->Release(); pTexture = NULL; }
	}
	*/
	
	//test if eyes are green, if not then update broke it
	//if(eyes)
		//pDevice->SetPixelShader(shadGreen);

	//log the eyes
	//if (eyes)
	//if (GetAsyncKeyState(VK_F10) & 1)
		//Log("qCRC == %x && Stride == %d && NumVertices == %d && primCount == %d && numElements == %d && decl->Type == %d && vSize == %d && pSize == %d && mStartRegister == %d && mVector4fCount == %d && sWidth == %d && sHeight == %d", qCRC, Stride, NumVertices, primCount, numElements, decl->Type, vSize, pSize, mStartRegister, mVector4fCount, sWidth, sHeight);
	//qCRC == 377503c2 && Stride == 32 && NumVertices == 4 && primCount == 2 && numElements == 4 && decl->Type == 2 && vSize == 536 && pSize == 872 && mStartRegister == 0 && mVector4fCount == 9 && sWidth == 256 && sHeight == 256

	//log shader of eyes
	//if (eyes)
	//if (GetAsyncKeyState(VK_F10) & 1)
	//doDisassembleShader(pDevice, GetDirectoryFile("shader1.txt"));


	//small bruteforce logger
	if (logger)
	{
		//hold down P key until a texture changes, press I to log values of those textures
		if (GetAsyncKeyState('O') & 1) //-
			countnum--;
		if (GetAsyncKeyState('P') & 1) //+
			countnum++;
		if ((GetAsyncKeyState(VK_MENU)) && (GetAsyncKeyState('9') & 1)) //reset, set to -1
			countnum = -1;
		if (countnum == numElements)
			if (GetAsyncKeyState('I') & 1) //press I to log to log.txt
				Log("qCRC == %x && Stride == %d && NumVertices == %d && primCount == %d && numElements == %d && decl->Type == %d && vSize == %d && pSize == %d && mStartRegister == %d && mVector4fCount == %d && sWidth == %d && sHeight == %d", qCRC, Stride, NumVertices, primCount, numElements, decl->Type, vSize, pSize, mStartRegister, mVector4fCount, sWidth, sHeight);
		if (countnum == numElements)
		{
			pDevice->SetPixelShader(NULL);
			return D3D_OK; //delete texture
		}
	}

	return DrawIndexedPrimitive_orig(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

//==========================================================================================================================

bool DoInit = true;
HRESULT APIENTRY EndScene_hook(IDirect3DDevice9* pDevice)
{
	//sprite
	PreClear(pDevice);

	//init only once
	if (DoInit)
	{
		LoadSettings();

		//generate circle shader
		//DX9CreateEllipseShader(pDevice);

		//circle vb
		//pDevice->CreateVertexBuffer(sizeof(VERTEX) * 4, D3DUSAGE_WRITEONLY, FVF, D3DPOOL_DEFAULT, &vb, NULL);

		//circle ib
		//pDevice->CreateIndexBuffer((3 * 2) * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, NULL);

		//init fps optimized box
		//pDevice->CreateVertexBuffer(768 * sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vb_box, NULL);

		DoInit = false;
	}

	//create menu
	if (pFont == NULL)
	{
		HRESULT hr = D3DXCreateFont(pDevice, 13, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &pFont);

		if (FAILED(hr)) {
			//Log("D3DXCreateFont failed");
		}
	}

	//draw menu
	if (pFont)
	{
		//Shift|RMouse|LMouse|Ctrl|Alt|Space|X|C
		if (aimkey == 0) Daimkey = 0;
		if (aimkey == 1) Daimkey = VK_SHIFT;
		if (aimkey == 2) Daimkey = VK_RBUTTON;
		if (aimkey == 3) Daimkey = VK_LBUTTON;
		if (aimkey == 4) Daimkey = VK_CONTROL;
		if (aimkey == 5) Daimkey = VK_MENU;
		if (aimkey == 6) Daimkey = VK_SPACE;
		if (aimkey == 7) Daimkey = 0x58; //X
		if (aimkey == 8) Daimkey = 0x43; //C

		BuildMenu(pDevice);
	}

	//esp part 2
	if (esp > 0 && AimInfo.size() != NULL)
	{
		for (unsigned int i = 0; i < AimInfo.size(); i++)
		{
			if (esp == 1 && /*AimInfo[i].iTeam == 1 && */AimInfo[i].vOutX > 1 && AimInfo[i].vOutY > 1)
			{
				//draw aimpoint
				DrawPoint(pDevice, (int)AimInfo[i].vOutX, (int)AimInfo[i].vOutY, 4, 4, D3DCOLOR_ARGB(255, 0, 255, 0)); //draw aimpoint
			}

			if (esp == 2 && /*AimInfo[i].iTeam == 1 && */AimInfo[i].vOutX > 1 && AimInfo[i].vOutY > 1)
			{
				//draw pic
				PrePresent(pDevice, (int)AimInfo[i].vOutX - 32, (int)AimInfo[i].vOutY - 20);
			}
			/*
			if (esp == 3 && AimInfo[i].vOutX > 1 && AimInfo[i].vOutY > 1)
			{
				//draw box 
				addbox(AimInfo[i].vOutX-9, AimInfo[i].vOutY, 20, 2, White, pDevice); //w ->, h = ^
				addbox(AimInfo[i].vOutX-9, AimInfo[i].vOutY+20, 20, 2, White, pDevice);
				addbox(AimInfo[i].vOutX-9, AimInfo[i].vOutY, 2, 20, White, pDevice);
				addbox(AimInfo[i].vOutX+9, AimInfo[i].vOutY, 2, 20, White, pDevice);//+18-9=+9
				renderbox(pDevice);
				//DrawBorder(pDevice, (int)AimInfo[i].vOutX - 9, (int)AimInfo[i].vOutY, 20, 30, 1, D3DCOLOR_ARGB(0, 255, 0, 0));
			}

			if (esp == 4 && AimInfo[i].vOutX > 1 && AimInfo[i].vOutY > 1)
			{
				DWORD col[4] = { 0xffffff00, 0xffffff00, 0xffffff00, 0xffffff00 };//yellow
				DX9DrawEllipse(pDevice, AimInfo[i].vOutX-9, AimInfo[i].vOutY - 10, 25, 25, 6, &col[4]);
				//DrawString(pFont, (int)AimInfo[i].vOutX - 9, (int)AimInfo[i].vOutY, D3DCOLOR_ARGB(255, 255, 0, 0), "[]");
			}
			*/
		}
	}

	//aimbot part 2
	if (aimbot == 1 && AimInfo.size() != NULL && GetAsyncKeyState(Daimkey))
	//if (aimbot == 1 && AimInfo.size() != NULL)
	{
		UINT BestTarget = -1;
		DOUBLE fClosestPos = 99999;

		for (unsigned int i = 0; i < AimInfo.size(); i++)
		{
			//aimfov
			float radiusx = (aimfov*2.5f) * (ScreenCenterX / 100);
			float radiusy = (aimfov*2.5f) * (ScreenCenterY / 100);

			if (aimfov == 0)
			{
				radiusx = 2.0f * (ScreenCenterX / 100);
				radiusy = 2.0f * (ScreenCenterY / 100);
			}

			//get crosshairdistance
			AimInfo[i].CrosshairDistance = GetDistance(AimInfo[i].vOutX, AimInfo[i].vOutY, ScreenCenterX, ScreenCenterY);

			//aim at team 1, 2 or 3
			//if (aimbot == AimInfo[i].iTeam)

			//if in fov
			if (AimInfo[i].vOutX >= ScreenCenterX - radiusx && AimInfo[i].vOutX <= ScreenCenterX + radiusx && AimInfo[i].vOutY >= ScreenCenterY - radiusy && AimInfo[i].vOutY <= ScreenCenterY + radiusy)

				//get closest/nearest target to crosshair
				if (AimInfo[i].CrosshairDistance < fClosestPos)
				{
					fClosestPos = AimInfo[i].CrosshairDistance;
					BestTarget = i;
				}
		}


		//if nearest target to crosshair
		if (BestTarget != -1)
		{
			double DistX = AimInfo[BestTarget].vOutX - ScreenCenterX;
			double DistY = AimInfo[BestTarget].vOutY - ScreenCenterY;

			DistX /= (1 + aimsens);
			DistY /= (1 + aimsens);

			//aim
			//if (GetAsyncKeyState(Daimkey) & 0x8000)
				mouse_event(MOUSEEVENTF_MOVE, (int)DistX, (int)DistY, 0, NULL);

			//autoshoot on
			if ((!GetAsyncKeyState(VK_LBUTTON) && (autoshoot == 1) && (GetAsyncKeyState(Daimkey) & 0x8000))) //
			{
				if (autoshoot == 1 && !IsPressed)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					IsPressed = true;
				}
			}
		}
	}
	AimInfo.clear();

	//autoshoot off
	if (autoshoot == 1 && IsPressed)
	{
		if (timeGetTime() - frametime >= 50) //50ms
		{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			IsPressed = false;
			frametime = timeGetTime();
		}
	}

	//draw logger
	if ((GetAsyncKeyState(VK_MENU)) && (GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x4C) & 1)) //ALT + CTRL + L toggles logger
		logger = !logger;
	if (pFont && logger) //&& countnum >= 0)
	{
		char szString[255];
		sprintf_s(szString, "countnum = %d", (int)countnum);
		DrawString(pFont, 220, 100, White, (char*)&szString[0]);
		DrawString(pFont, 220, 110, Yellow, "hold P to +");
		DrawString(pFont, 220, 120, Yellow, "hold O to -");
		DrawString(pFont, 220, 130, Green, "press I to log");
	}

	//DWORD col[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
	//DX9DrawEllipse(pDevice, 150, 150 - 10, 25, 25, 6, &col[4]);

	return EndScene_orig(pDevice);
}

//==========================================================================================================================

HRESULT APIENTRY SetStreamSource_hook(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT sStride)
{
	//get Stride
	if (StreamNumber == 0)
	{
		Stride = sStride;

		//if (pStreamData)
		//{
		//pStreamData->GetDesc(&vdesc);
		//}
	}

	return SetStreamSource_orig(pDevice, StreamNumber, pStreamData, OffsetInBytes, sStride);
}

//==========================================================================================================================

HRESULT APIENTRY SetVertexShaderConstantF_hook(LPDIRECT3DDEVICE9 pDevice, UINT StartRegister, const float *pConstantData, UINT Vector4fCount)
{
	//get mVector4fCount
	if (pConstantData != NULL)
	{
		//pConstantDataFloat = (float*)pConstantData;

		mStartRegister = StartRegister;
		mVector4fCount = Vector4fCount;
	}

	return SetVertexShaderConstantF_orig(pDevice, StartRegister, pConstantData, Vector4fCount);
}

//==========================================================================================================================

HRESULT APIENTRY SetVertexShader_hook(LPDIRECT3DDEVICE9 pDevice, IDirect3DVertexShader9 *veShader)
{
	//get vShader size
	if (veShader != NULL)
	{
		vShader = veShader;
		vShader->GetFunction(NULL, &vSize);
	}

	return SetVertexShader_orig(pDevice, veShader);
}

//==========================================================================================================================

HRESULT APIENTRY SetPixelShader_hook(LPDIRECT3DDEVICE9 pDevice, IDirect3DPixelShader9 *piShader)
{
	//get pShader size
	if (piShader != NULL)
	{
		pShader = piShader;
		pShader->GetFunction(NULL, &pSize);
	}

	return SetPixelShader_orig(pDevice, piShader);
}

//==========================================================================================================================

HRESULT APIENTRY SetVertexDeclaration_hook(IDirect3DDevice9* pDevice, IDirect3DVertexDeclaration9 *pDecl)
{
	if (pDecl != NULL)
	{
		HRESULT hr = pDecl->GetDeclaration(decl, &numElements);
		if (FAILED(hr))
		{
			//Log("GetDeclaration failed");
		}
	}

	return SetVertexDeclaration_orig(pDevice, pDecl);
}

//==========================================================================================================================

HRESULT APIENTRY SetTexture_hook(IDirect3DDevice9* pDevice, DWORD Sampler, IDirect3DBaseTexture9 *pTexture)
{
	mStage = Sampler;

	return SetTexture_orig(pDevice, Sampler, pTexture);
}

//==========================================================================================================================

HRESULT APIENTRY SetViewport_hook(IDirect3DDevice9* pDevice, CONST D3DVIEWPORT9* pViewport)
{
	//get viewport/screensize
	Viewport = *pViewport;
	ScreenCenterX = (float)Viewport.Width / 2.0f;
	ScreenCenterY = (float)Viewport.Height / 2.0f;

	return SetViewport_orig(pDevice, pViewport);
}

//==========================================================================================================================

HRESULT APIENTRY Reset_hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	DeleteRenderSurfaces();

	if (pFont)
		pFont->OnLostDevice();

	HRESULT ResetReturn = Reset_orig(pDevice, pPresentationParameters);

	if (SUCCEEDED(ResetReturn))
	{
		if (pFont)
			pFont->OnResetDevice();
	}

	return ResetReturn;
}

//==========================================================================================================================

DWORD WINAPI DirectXInit(__in  LPVOID lpParameter)
{
	while (GetModuleHandle("d3d9.dll") == 0)
	{
		Sleep(100);
	}

	IDirect3D9* d3d = NULL;
	IDirect3DDevice9* d3ddev = NULL;

	HWND tmpWnd = CreateWindowA("BUTTON", "Temp Window", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, dllHandle, NULL);
	if(tmpWnd == NULL)
	{
		//Log("[DirectX] Failed to create temp window");
		return 0;
	}

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(d3d == NULL)
	{
		DestroyWindow(tmpWnd);
		//Log("[DirectX] Failed to create temp Direct3D interface");
		return 0;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp)); 
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = tmpWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if(result != D3D_OK)
	{
		d3d->Release();
		DestroyWindow(tmpWnd);
		//Log("[DirectX] Failed to create temp Direct3D device");
		return 0;
	}

	// We have the device, so walk the vtable to get the address of all the dx functions in d3d9.dll
	#if defined _M_X64
	DWORD64* dVtable = (DWORD64*)d3ddev;
	dVtable = (DWORD64*)dVtable[0];
	#elif defined _M_IX86
	DWORD* dVtable = (DWORD*)d3ddev;
	dVtable = (DWORD*)dVtable[0]; // == *d3ddev
	#endif
	//Log("[DirectX] dvtable: %x", dVtable);

	//for(int i = 0; i < 95; i++)
	//{
		//Log("[DirectX] vtable[%i]: %x, pointer at %x", i, dVtable[i], &dVtable[i]);
	//}
	
	// Set EndScene_orig to the original EndScene etc.
	EndScene_orig = (EndScene)dVtable[42];
	SetVertexShaderConstantF_orig = (SetVertexShaderConstantF)dVtable[94];
	DrawIndexedPrimitive_orig = (DrawIndexedPrimitive)dVtable[82];
	SetVertexDeclaration_orig = (SetVertexDeclaration)dVtable[87];
	Reset_orig = (Reset)dVtable[16];
	SetStreamSource_orig = (SetStreamSource)dVtable[100];
	SetVertexShader_orig = (SetVertexShader)dVtable[92];
	SetPixelShader_orig = (SetPixelShader)dVtable[107];
	SetViewport_orig = (SetViewport)dVtable[47];
	//SetTexture_orig = (SetTexture)dVtable[65];

	// Detour functions x86 & x64
	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[42], &EndScene_hook, reinterpret_cast<void**>(&EndScene_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[42]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[94], &SetVertexShaderConstantF_hook, reinterpret_cast<void**>(&SetVertexShaderConstantF_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[94]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[82], &DrawIndexedPrimitive_hook, reinterpret_cast<void**>(&DrawIndexedPrimitive_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[82]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[87], &SetVertexDeclaration_hook, reinterpret_cast<void**>(&SetVertexDeclaration_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[87]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[16], &Reset_hook, reinterpret_cast<void**>(&Reset_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[16]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[100], &SetStreamSource_hook, reinterpret_cast<void**>(&SetStreamSource_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[100]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[92], &SetVertexShader_hook, reinterpret_cast<void**>(&SetVertexShader_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[92]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[107], &SetPixelShader_hook, reinterpret_cast<void**>(&SetPixelShader_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[107]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[47], &SetViewport_hook, reinterpret_cast<void**>(&SetViewport_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[47]) != MH_OK) { return 1; }
	//if (MH_CreateHook((DWORD_PTR*)dVtable[65], &SetTexture_hook, reinterpret_cast<void**>(&SetTexture_orig)) != MH_OK) { return 1; }
	//if (MH_EnableHook((DWORD_PTR*)dVtable[65]) != MH_OK) { return 1; }

	//Log("[Detours] Detours attached\n");

	d3ddev->Release();
	d3d->Release();
	DestroyWindow(tmpWnd);
		
	return 1;
}

//==========================================================================================================================

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: // A process is loading the DLL.
		dllHandle = hinstDLL;
		DisableThreadLibraryCalls(hinstDLL); // disable unwanted thread notifications to reduce overhead
		GetModuleFileNameA(hinstDLL, dlldir, 512);
		for (int i = (int)strlen(dlldir); i > 0; i--)
		{
			if (dlldir[i] == '\\')
			{
				dlldir[i + 1] = 0;
				break;
			}
		}
		CreateThread(0, 0, DirectXInit, 0, 0, 0); //init our hooks
		break;

	case DLL_PROCESS_DETACH: // A process unloads the DLL.
		/*
		if (MH_Uninitialize() != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)dVtable[42]) != MH_OK) { return 1; }
		*/
		break;
	}
	return TRUE;
}

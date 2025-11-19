#ifndef __INPUT_H__
#define __INPUT_H__

#include <Windows.h>
#include <DirectXMath.h>
#undef max
#undef min

#define MOUSEBUTTON_L	0
#define MOUSEBUTTON_R	1
#define MOUSEBUTTON_M	2
#define MOUSEBUTTON_X1	3
#define MOUSEBUTTON_X2	4

HRESULT InitInput();
void UninitInput();
void UpdateInput();

bool IsKeyPress(BYTE key);
bool IsKeyTrigger(BYTE key);
bool IsKeyRelease(BYTE key);
bool IsKeyRepeat(BYTE key);
POINT* GetMousePosition(bool CenterIsZero = true);
void SetMousePosition(POINT inPos, bool CenterIsZero = true);
bool IsMouseButtonPress(DWORD dwBtnID);
bool IsMouseButtonTrigger(DWORD dwBtnID);
bool IsMouseButtonRelease(DWORD dwBtnID);
bool IsMouseHover(DirectX::XMFLOAT2 inPos, DirectX::XMFLOAT2 inSize);

#endif // __INPUT_H__

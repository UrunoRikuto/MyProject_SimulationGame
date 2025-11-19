#include "Input.h"
#include "Main.h"
#include "Defines.h"

//--- グローバル変数
BYTE g_keyTable[256];
BYTE g_oldTable[256];
POINT g_Mouse;
const int g_nMouseBtn[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };

HRESULT InitInput()
{
	// 一番最初の入力
	GetKeyboardState(g_keyTable);
	return S_OK;
}

void UninitInput()
{

}

void UpdateInput()
{
	// 古い入力を更新
	memcpy_s(g_oldTable, sizeof(g_oldTable), g_keyTable, sizeof(g_keyTable));
	// 現在の入力を取得
	GetKeyboardState(g_keyTable);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(GetMyWindow(), &pt);
    g_Mouse = pt;
}

bool IsKeyPress(BYTE key)
{
	return g_keyTable[key] & 0x80;
}

bool IsKeyTrigger(BYTE key)
{
	return (g_keyTable[key] ^ g_oldTable[key]) & g_keyTable[key] & 0x80;
}

bool IsKeyRelease(BYTE key)
{
	return (g_keyTable[key] ^ g_oldTable[key]) & g_oldTable[key] & 0x80;
}

bool IsKeyRepeat(BYTE key)
{
	return false;
}

// マウス座標取得
POINT* GetMousePosition(bool CenterIsZero)
{
    POINT pt = g_Mouse;
    if (CenterIsZero)
    {
        pt.x -= SCREEN_WIDTH / 2;
        pt.y -= SCREEN_HEIGHT / 2;
    }
    return &pt;
}

void SetMousePosition(POINT inPos, bool CenterIsZero)
{
    POINT pt = inPos;
    if (CenterIsZero)
    {
        pt.x += SCREEN_WIDTH / 2;
        pt.y += SCREEN_HEIGHT / 2;
    }

    g_Mouse.x = pt.x;
    g_Mouse.y = pt.y;

    ClientToScreen(GetMyWindow(), &pt);
    SetCursorPos(pt.x,pt.y);
}

// マウス ボタン情報取得
bool IsMouseButtonPress(DWORD dwBtnID)
{
    if (dwBtnID >= _countof(g_nMouseBtn)) return false;
    return IsKeyPress(g_nMouseBtn[dwBtnID]);
}

// マウス トリガ情報取得
bool IsMouseButtonTrigger(DWORD dwBtnID)
{
    if (dwBtnID >= _countof(g_nMouseBtn)) return false;
    return IsKeyTrigger(g_nMouseBtn[dwBtnID]);
}

// マウス リリース情報取得
bool IsMouseButtonRelease(DWORD dwBtnID)
{
    if (dwBtnID >= _countof(g_nMouseBtn)) return false;
    return IsKeyRelease(g_nMouseBtn[dwBtnID]);
}

bool IsMouseHover(DirectX::XMFLOAT2 inPos, DirectX::XMFLOAT2 inSize)
{
    POINT pt = *GetMousePosition();
    if (inPos.x - inSize.x * 0.5f <= pt.x && inPos.x + inSize.x * 0.5f >= pt.x)
    {
        if (inPos.y - inSize.y * 0.5f <= pt.y && inPos.y + inSize.y * 0.5f >= pt.y)
        {
            return true;
        }
    }

    return false;
}

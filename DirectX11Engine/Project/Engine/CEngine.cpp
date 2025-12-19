#include "pch.h"
#include "CEngine.h"
#include "CDevice.h"

int CEngine::init(HWND _hWnd, POINT _Resolution)
{
	m_hMainHwnd = _hWnd;
	m_Resolution = _Resolution;

	RECT rt = { 0, 0, m_Resolution.x, m_Resolution.y };

	// 윈도우 영역 계산 후 rt에 넣어준다. (메뉴바나 기타 영역을 제외한 영역)
	// WS_OVERLAPPEDWINDOW : 가장 기본적인 윈도우 스타일(갖출건 다 갖춘 상태), 윈도우 스타일 상수이다.
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	// 윈도우 크기 설정
	SetWindowPos(m_hMainHwnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

	// DirectX11
	if (FAILED(CDevice::GetInst()->init(m_hMainHwnd, m_Resolution)))
	{
		MessageBox(m_hMainHwnd, L"Device 초기화 실패", L"엔진 초기화 실패", MB_OK);
		return E_FAIL;
	}


	return S_OK; // 성공 반환(0의 값이다)
}

CEngine::CEngine()
{

}

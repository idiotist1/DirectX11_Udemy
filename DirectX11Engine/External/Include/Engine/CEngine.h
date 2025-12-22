#pragma once

// 데이터 영역 메모리 : 전역 변수
// 프로세스 시작할때 초기화
// 프로세스 종료시 해제
// 프로세스가 실행도중 상시 유지되는 메모리

class CEngine
{
private:
	HWND m_hMainHwnd = nullptr;
	POINT m_Resolution = {};

public:
	int init(HWND _hWnd, POINT _Resolution);

public:
	static CEngine* GetInst()
	{
		static CEngine mgr;
		return &mgr;
	}

private:
	CEngine();
	// 삭제된 함수, 의도치 않은 함수 호출을 방지하는 역할
	CEngine(const CEngine& _origin) = delete; //싱글톤이기 때문에 복사생성자도 막아준다.
};
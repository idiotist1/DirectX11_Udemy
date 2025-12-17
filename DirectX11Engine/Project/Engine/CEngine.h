#pragma once

// 데이터 영역 메모리 : 전역 변수
// 프로세스 시작할때 초기화
// 프로세스 종료시 해제
// 프로세스가 실행도중 상시 유지되는 메모리

class CEngine
{
private:
	HWND m_hMainHwnd;
	POINT m_Resolution;
public:
	static CEngine* GetInst()
	{
		static CEngine mgr;
		return &mgr;
	}

private:
	CEngine();
	CEngine(const CEngine& _origin); //싱글톤이기 때문에 복사생성자도 막아준다.
};

// * 복사생성자를 안막았을 경우
//class Test
//{
//	void TTT()
//	{
//		CEngine* pEngine = CEngine::GetInst();
//		CEngine a = *pEngine; //복사생성자로 생성시킬 수 있다.(싱글톤이기 때문에 막아야한다)
//		CEngine b = *pEngine;
//		CEngine c = *pEngine;
//		CEngine d = *pEngine; // 계속 만들 수 있다.
//	}
//};


// * 싱글톤 첫번째
//class CEngine
//{
//private:
//	static CEngine* g_This;
//private:
//	HWND m_hMainWnd;
//public:
//	// 정적 멤버함수 : 객체가 없어도 호출 가능
//	static CEngine* GetInst()
//	{
//		if (g_This == nullptr)
//		{
//			g_This = new CEngine();
//		}
//		return g_This;
//	}
//	
//	static void Destroy() //Destroy를 까먹을 수 있다는 단점이 있다.
//	{
//		if (g_This != nullptr)
//		{
//			delete g_This;
//			g_This = nullptr; // 삭제된 주소를 들고 있으므로 nullptr로 채워야 한다.
//		}
//	}
//private:
//	CEngine();
//};
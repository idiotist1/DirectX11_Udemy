#include "pch.h"
#include "CDevice.h"

CDevice::CDevice()
{

}

CDevice::~CDevice()
{
	// 동적라이브러리는 동적라이브러리에서 해제해야 한다.(동적 라이브러리에서 생성했기 때문)
	m_Deivce->Release();
	m_Context->Release();
}

int CDevice::init(HWND _hWnd, POINT _Resolution)
{
	m_hMainWnd = _hWnd;
	m_RenderResolution = _Resolution;

	// * 정적 라이브러리
	//- 우리는 엔진을 동적라이브러리로 만들고 있다.
	//- 컴파일시에 Client.exe파일에 붙어버린다.
	
	// * 동적 라이브러리
	//- DX11은 동적라이브러리다.
	//- 런타임시 DLL코드가 링크가 되서 붙는다.

	// GPU 장치가 있다는 가정하에 명령어를 수행
	// 만약 PC에 GPU가 설치되어 있지 않다면 DX11 함수 호출시 CPU에게 일을 시킨다.
	// DX 라이브러리를 수행할 때 D3D_DRIVER_TYPE_HARDWARE 옵션으로 초기화 시도했는데 실패했다면
	// 그건 PC에 GPU가 안꼽혀있다는 것 GPU가 없다면 초기화가 안되도록 하드웨어 옵션으로 강제했다.

	UINT iFlag = 0;
#ifdef _DEBUG
	// * D3D11_CREATE_DEVICE_DEBUG
	//- DLL쪽에서 문제가 생기면 예외처리를 해놨는데 예외처리한 문제들을 Visual Studio 출력시키도록 해준다.
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0; //피쳐레벨
	//D3D11_SDK_VERSION : 7레벨
	
	// 디바이스와 컨텍스트를 생성해서 m_Device, m_Context로 받아온다.
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_Deivce, &level ,&m_Context);

	// SwapChain 생성
	if (FAILED(CreateSwapchain()))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CDevice::CreateSwapchain()
{
	// 스왑체인에 필요한 옵션을 채우기 위한 구조체
	DXGI_SWAP_CHAIN_DESC Desc = {};



	Desc.BufferDesc.RefreshRate.Denominator = 1;
	Desc.BufferDesc.RefreshRate.Numerator = 60;
	Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //정의하지 않음
	Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //화면에 표시되는 방법, 정의하지 않음
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;



	// * SwapChain 생성
	// DirectX 버전과 상관 없이 렌더링 할때 중간층 역할이라 사용법은 버전 상관 없이 동일
	IDXGIDevice* pDXGIDevice = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;

	m_Deivce->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice);
	// 어댑터를 얻어낸다.
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);
	// 팩토리를 얻어낸다.
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	if (FAILED(pFactory->CreateSwapChain(m_Deivce, &Desc, &m_SwapChain)))
		return E_FAIL;
	
	// SwapChain 생성 했으면 해제해야 한다.
	pDXGIDevice->Release(); 
	pAdapter->Release();
	pFactory->Release();

	return S_OK;
}

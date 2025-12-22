#include "pch.h"
#include "CDevice.h"

CDevice::CDevice()
{

}

CDevice::~CDevice()
{
	// 동적라이브러리는 동적라이브러리에서 해제해야 한다.(동적 라이브러리에서 생성했기 때문)
	// Release함수는 메모리해제가 아닌 레퍼런스카운트를 줄이는 역할(0이되면 자동으로 해제된다)
	// 얻어온만큼 줄여줘야 한다.
	m_Device->Release();
	m_Context->Release();
	m_SwapChain->Release();

	m_RenderTargetTex->Release();
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
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE
		, nullptr, iFlag, nullptr, 0, D3D11_SDK_VERSION
		, &m_Device, &level, &m_Context);

	// SwapChain 생성
	if (FAILED(CreateSwapchain()))
	{
		return E_FAIL;
	}

	// 뷰 생성
	if (FAILED(CreateView()))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CDevice::CreateSwapchain()
{
	// * 스왑체인
	//- 어떤식으로 화면에 출력을 할지 스왑체인이 결정
	//- 모니터는 화면을 1초에 60~144번 갱신
	//- 스왑체인도 1초에 60번정도 그려서 모니터에 보내줘야함

	// 스왑체인에 필요한 옵션을 채우기 위한 구조체
	DXGI_SWAP_CHAIN_DESC Desc = {};

	// * SwapChain이 화면을 게시(Present) 할때 출력 목적지 윈도우
	Desc.OutputWindow = m_hMainWnd; // 출력할 윈도우
	Desc.Windowed = true; // 창모드

	// * SwapChain 이 만들어질때 버퍼 옵션
	Desc.BufferCount = 1; // 스왑체인이 관리하는 백퍼버 갯수
	Desc.BufferDesc.Width = (UINT)m_RenderResolution.x; // 백버퍼 크기(해상도와 동일하게)
	Desc.BufferDesc.Height = (UINT)m_RenderResolution.y; // 백버퍼 크기(해상도와 동일하게)
	Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 픽셀 포멧
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 렌더링의 목적지(렌더 타깃)

	Desc.BufferDesc.RefreshRate.Denominator = 1;
	Desc.BufferDesc.RefreshRate.Numerator = 60;
	Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //정의하지 않음
	Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //화면에 표시되는 방법, 정의하지 않음
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	// * 스왑체인이 데이터를 백버퍼로 화면에 출력하는 방식
	// 1. 비트 블렛 전송 모델
	//- 백버퍼에서 DWM(데스크톱 윈도우 매니저)라는 화면에 복사, 리디렉션 하기 위해서
	//- 데이터를 복사받는 중간 버퍼가 있다. 만든 버퍼를 중간 버퍼에 복사하여 보내준다.
	//- DXGI_SWAP_EFFECT_DISCARD : 기존 장면을 버림
	//- DXGI_SWAP_EFFECT_SEQUENTIAL : 기존 장면 유지
	// 2. 대칭 이동 프레젠테이션 모델
	//- 그린 버퍼의 포인터를 전달한다. (복사방식이 아니므로 성능상 이점이 있다)
	//- 테어링 현상을 안정적으로 제어할 수 있다.
	//- 버퍼 카운트가 2개(백버퍼가 2개) 필요하다. (1개 그리고 1개는 전달하는 방식을 사용)
	//- Desc.BufferCount = 2를 하면 된다.
	//- 단점은 모니터 갱신 비율로 프로그램의 FPS가 고정되어 버린다.
	//- DX11에서는 GetBuffer하면 자동으로 그릴 버퍼를 줬지만 
	//- DX12부터는 직접 구별해서 포인트를 얻어와야 한다.
	//- DXGI_SWAP_EFFECT_FLIP_DISCARD : 기존 장면 버림
	//- DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL : 기존 장면 유지

	// * 테어링 현상(화면 찢어짐 현상)
	//- 그림을 보내서 그리는 도중에 새로운 그림을 또 보내서 화면이 찢어지는 현상
	//- V-Sync를 활용하면 해결할 수 있는데 대칭이동 방식을 쓰는 방식이 있다.
	Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	Desc.Flags = 0;

	// * SwapChain 생성
	// DirectX 버전과 상관 없이 렌더링 할때 중간층 역할이라 사용법은 버전 상관 없이 동일
	IDXGIDevice* pDXGIDevice = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;

	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	// 어댑터를 얻어낸다.
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);
	// 팩토리를 얻어낸다.
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	if (FAILED(pFactory->CreateSwapChain(m_Device, &Desc, &m_SwapChain)))
		return E_FAIL;
	
	// SwapChain 생성 했으면 해제해야 한다.
	pDXGIDevice->Release(); 
	pAdapter->Release();
	pFactory->Release();

	return S_OK;
}

int CDevice::CreateView()
{
	// 1. 스왑체인으로부터 RenderTarget Texture 가져오기
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_RenderTargetTex);

	// 2. RenderTargetView 생성 (버퍼들을 필요한 사용처로 전달하는 중간객체)
	m_Device->CreateRenderTargetView(m_RenderTargetTex, nullptr, &m_RTV);

	// 3. DepthStencil용 Texture를 제작
	D3D11_TEXTURE2D_DESC Desc = {};

	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Desc.Width = (UINT)m_RenderResolution.x;
	Desc.Height = (UINT)m_RenderResolution.y;
	Desc.ArraySize = 1;

	Desc.CPUAccessFlags = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.MipLevels = 1;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //텍스쳐의 용도

	// Depthstencil View 생성 (깊이 정보)

	// RenderTarget 과 DepthStencilTarget을 출력으로 지정한다.
	//m_Context->OMSetRenderTargets();

	return S_OK;
}

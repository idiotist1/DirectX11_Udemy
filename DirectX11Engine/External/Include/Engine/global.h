#pragma once

#include <d3d11.h> // DirectX11 관련 헤더파일
#include <d3dcompiler.h> // 쉐이더 코드 컴파일 할때 필요한 헤더
#include <DirectXMath.h> // DirectX 수학 관련 헤더(행렬, 벡터 등)
#include <DirectXPackedVector.h> // 자료형 및 구조체

// .lib는 생략해도 된다.
// #pragma comment(lib, "d3d11")
// #pragma comment(lib, "d3dpcomiler")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// namespace 제거
using namespace DirectX;
using namespace DirectX::PackedVector;
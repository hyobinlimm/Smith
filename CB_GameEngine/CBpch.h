/// 2022.08.03
/// Component 들을 담아놓을 GameEngine
/// 미리 컴파일 할 헤더들 추가
/// 
#define _XM_NO_INTRINSICS_

#include "GEpch.h"
#include "SimpleMath.h"
#include <DirectXMath.h>
#include <list>
#include <map>

#include <random>

using namespace DirectX;
using namespace DirectX::SimpleMath;

///#define TRACE __noop
#include <atltrace.h>
#define TRACE ATLTRACE
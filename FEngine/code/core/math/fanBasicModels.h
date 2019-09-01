#pragma once

#include "fanGlobalIncludes.h"

namespace fan
{
	inline std::vector< btVector3 > GetCube		( const float _halfSize);
	inline std::vector< btVector3 > GetSphere	( const float _radius, const int _numSubdivisions);
	inline std::vector< btVector3 > GetCone		( const float _radius, const float _height, const int _numSubdivisions);
}

#include "core/math/fanBasicModels.inl"
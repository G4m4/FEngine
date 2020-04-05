#pragma once

#include "core/fanCorePrecompiled.hpp"
#include "core/math/shapes/fanShape.hpp"

namespace fan
{
	//================================================================================================================================
	// 3D plane
	// @todo we don't need the m_point, only the distance to the plane when following the normal
	//================================================================================================================================
	class Plane : public Shape
	{
	public:
		Plane( const btVector3 _point, const btVector3 _normal );

		bool RayCast( const btVector3 _origin, const btVector3 _dir, btVector3& _outIntersection ) const override;

	private:
		btVector3 m_point;
		btVector3 m_normal;
	};
}

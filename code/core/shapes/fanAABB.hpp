#pragma once

#include <vector>
#include "core/shapes/fanTriangle.hpp"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()

namespace fan
{
	class Plane;

	//================================================================================================================================
	// axis aligned bounding box
	//================================================================================================================================
	class AABB : public Shape
	{
	public:
		AABB();
		AABB( const btVector3 _low, const btVector3 _high );
		AABB( const std::vector<btVector3> _pointCloud, const glm::mat4 _modelMatrix );

		void					    Clear() { m_low = btVector3::Zero(); m_high = btVector3::Zero(); }
		btVector3					GetLow() const { return m_low; }
		btVector3					GetHigh() const { return m_high; }
		std::vector< btVector3 >	GetCorners() const;

		virtual bool RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& outIntersection ) const override;

	private:
		btVector3 m_low;
		btVector3 m_high;
	};
}
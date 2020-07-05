#include "scene/singletons/fanRenderDebug.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/math/fanBasicModels.hpp"
#include "core/fanDebug.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::RENDER_DEBUG16;
		_info.group = EngineGroups::SceneRender;
		_info.onGui = &RenderDebug::OnGui;
		_info.name = "render debug";
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		RenderDebug& renderDebug= static_cast<RenderDebug&>( _component );
		( void ) renderDebug;
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::Clear()
	{
		debugLines.clear();
		debugLinesNoDepthTest.clear();
		debugTriangles.clear();		
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugPoint( const btVector3 _pos, const Color _color )
	{
		const float size = 0.2f;
		DebugLine( _pos - size * btVector3::Up(), _pos + size * btVector3::Up(), _color );
		DebugLine( _pos - size * btVector3::Left(), _pos + size * btVector3::Left(), _color );
		DebugLine( _pos - size * btVector3::Forward(), _pos + size * btVector3::Forward(), _color );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugLine( const btVector3 _start, const btVector3 _end, const Color _color, const bool _depthTestEnable )
	{
		if( _depthTestEnable )
		{
			debugLines.push_back( DebugVertex( ToGLM( _start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
			debugLines.push_back( DebugVertex( ToGLM( _end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
		}
		else
		{
			debugLinesNoDepthTest.push_back( DebugVertex( ToGLM( _start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
			debugLinesNoDepthTest.push_back( DebugVertex( ToGLM( _end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
		}

	}

	//================================================================================================================================
	// takes a list of triangle and a list of colors
	// 3 vertices per triangle
	// 1 color per triangle
	//================================================================================================================================
	void RenderDebug::DebugTriangles( const std::vector<btVector3>& _triangles, const std::vector<Color>& _colors )
	{
		assert( _triangles.size() % 3 == 0 );
		assert( _colors.size() == _triangles.size() / 3 );

		debugTriangles.resize( debugTriangles.size() + _triangles.size() );
		for( int triangleIndex = 0; triangleIndex < _triangles.size() / 3; triangleIndex++ )
		{
			const btVector3 v0 = _triangles[3 * triangleIndex + 0];
			const btVector3 v1 = _triangles[3 * triangleIndex + 1];
			const btVector3 v2 = _triangles[3 * triangleIndex + 2];
			const glm::vec3 normal = glm::normalize( ToGLM( ( v1 - v2 ).cross( v0 - v2 ) ) );

			debugTriangles[3 * triangleIndex + 0] = DebugVertex( ToGLM( v0 ), normal, _colors[triangleIndex].ToGLM() );
			debugTriangles[3 * triangleIndex + 1] = DebugVertex( ToGLM( v1 ), normal, _colors[triangleIndex].ToGLM() );
			debugTriangles[3 * triangleIndex + 2] = DebugVertex( ToGLM( v2 ), normal, _colors[triangleIndex].ToGLM() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugCircle( const btVector3 _pos, const float _radius, btVector3 _axis, uint32_t _nbSegments, const Color _color, const bool _depthTestEnable )
	{
		assert( _nbSegments > 2 && _radius >= 0.f );

		const btVector3 other = btVector3( -_axis[1], -_axis[2], _axis[0] );
		btVector3 orthogonal = _radius * _axis.cross( other ).normalized();
		const float angle = 2.f * SIMD_PI / (float)_nbSegments;

		std::vector<DebugVertex>& lines = _depthTestEnable ? debugLines : debugLinesNoDepthTest;
		for( uint32_t segmentIndex = 0; segmentIndex < _nbSegments; segmentIndex++ )
		{

			btVector3 start = _pos + orthogonal;
			orthogonal = orthogonal.rotate( _axis, angle );
			btVector3 end = _pos + orthogonal;

			lines.push_back( DebugVertex( ToGLM( start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
			lines.push_back( DebugVertex( ToGLM( end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugTriangle( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color )
	{
		const glm::vec3 normal = glm::normalize( ToGLM( ( _v1 - _v2 ).cross( _v0 - _v2 ) ) );
		debugTriangles.push_back( DebugVertex( ToGLM( _v0 ), normal, _color.ToGLM() ) );
		debugTriangles.push_back( DebugVertex( ToGLM( _v1 ), normal, _color.ToGLM() ) );
		debugTriangles.push_back( DebugVertex( ToGLM( _v2 ), normal, _color.ToGLM() ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugCube( const btTransform _transform, const btVector3 _halfExtent, const Color _color, const bool _depthTestEnable )
	{
		std::vector< btVector3 > cube = GetCube( _halfExtent );

		for( int vertIndex = 0; vertIndex < cube.size(); vertIndex++ )
		{
			cube[vertIndex] = _transform * cube[vertIndex];
		}

		glm::vec4 glmColor = _color.ToGLM();

		std::vector<DebugVertex>& lines = _depthTestEnable ? debugLines : debugLinesNoDepthTest;

		lines.push_back( DebugVertex( ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );

		lines.push_back( DebugVertex( ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );

		lines.push_back( DebugVertex( ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
		lines.push_back( DebugVertex( ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugIcoSphere( const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color, const bool _depthTestEnable )
	{
		if( _radius <= 0 )
		{
			Debug::Warning( "Debug sphere radius cannot be zero or negative" );
			return;
		}

		std::vector<btVector3> sphere = GetSphere( _radius, _numSubdivisions );

		for( int vertIndex = 0; vertIndex < sphere.size(); vertIndex++ )
		{
			sphere[vertIndex] = _transform * sphere[vertIndex];
		}

		for( int triangleIndex = 0; triangleIndex < sphere.size() / 3; triangleIndex++ )
		{
			const btVector3 v0 = sphere[3 * triangleIndex + 0];
			const btVector3 v1 = sphere[3 * triangleIndex + 1];
			const btVector3 v2 = sphere[3 * triangleIndex + 2];
			DebugLine( v0, v1, _color, _depthTestEnable );
			DebugLine( v1, v2, _color, _depthTestEnable );
			DebugLine( v2, v0, _color, _depthTestEnable );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugSphere( const btTransform _transform, const float _radius, const Color _color, const bool _depthTestEnable )
	{
		DebugCircle( _transform.getOrigin(), _radius, btVector3::Up(), 32, _color, _depthTestEnable );
		DebugCircle( _transform.getOrigin(), _radius, btVector3::Left(), 32, _color, _depthTestEnable );
		DebugCircle( _transform.getOrigin(), _radius, btVector3::Forward(), 32, _color, _depthTestEnable );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugCone( const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color )
	{
		std::vector<btVector3> cone = GetCone( _radius, _height, _numSubdivisions );

		for( int vertIndex = 0; vertIndex < cone.size(); vertIndex++ )
		{
			cone[vertIndex] = _transform * cone[vertIndex];
		}

		for( int triangleIndex = 0; triangleIndex < cone.size() / 3; triangleIndex++ )
		{
			DebugTriangle( cone[3 * triangleIndex + 0], cone[3 * triangleIndex + 1], cone[3 * triangleIndex + 2], _color );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::DebugAABB( const AABB& _aabb, const Color _color )
	{
		std::vector< btVector3 > corners = _aabb.GetCorners();
		// Top
		DebugLine( corners[0], corners[1], _color );
		DebugLine( corners[1], corners[2], _color );
		DebugLine( corners[2], corners[3], _color );
		DebugLine( corners[3], corners[0], _color );
		// Bot
		DebugLine( corners[4], corners[5], _color );
		DebugLine( corners[5], corners[6], _color );
		DebugLine( corners[6], corners[7], _color );
		DebugLine( corners[7], corners[4], _color );
		//Vertical sides
		DebugLine( corners[0], corners[4], _color );
		DebugLine( corners[1], corners[5], _color );
		DebugLine( corners[2], corners[6], _color );
		DebugLine( corners[3], corners[7], _color );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderDebug::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		RenderDebug& renderDebug = static_cast<RenderDebug&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "debugLines:            %u", renderDebug.debugLines.size() );
			ImGui::Text( "debugLinesNoDepthTest: %u", renderDebug.debugLinesNoDepthTest.size() );
			ImGui::Text( "debugTriangles:        %u", renderDebug.debugTriangles.size() );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}
#include <glm/vec3.hpp>

#include "core/shared/Const.hpp"

#include "StudioSorting.hpp"

namespace studiomdl
{
bool CompareSortedMeshes( const SortedMesh_t& lhs, const SortedMesh_t& rhs )
{
	if( ( lhs.flags & ( STUDIO_NF_ADDITIVE ) ) == 0 && rhs.flags & ( STUDIO_NF_ADDITIVE ) )
		return true;

	if( lhs.flags & ( STUDIO_NF_MASKED ) && ( rhs.flags & ( STUDIO_NF_MASKED ) ) == 0 )
		return true;

	return false;
}
}
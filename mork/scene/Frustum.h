#ifndef _MORK_FRUSTUM_H_
#define _MORK_FRUSTUM_H_

#include <mork/math/vec4.h>
#include <mork/math/mat4.h>

namespace mork {

    /**
     *      * The visibility of a bounding box in a frustum.
     *           */
    enum Visibility {
        FULLY_VISIBLE, //!< The bounding box is fully visible
        PARTIALLY_VISIBLE, //!< The bounding box is partially visible
        INVISIBLE //!< The bounding box is invisible
    };


    class Frustum {
            
        public:
            void setPlanes(const mat4d& toScreen);

            // Returns wether a point is inside this frustum
            bool isInside(const vec3d& point) const;

            // Returns the visibility of a bounding box
            Visibility getVisibility(const box3d& bb) const;

        private:

			// Returns the visibility of a bb compared to one plane
			Visibility getVisibility(const box3d& bb, const vec4d& plane) const;
			
			// Returns the visibility of a point compared to one plane
			Visibility getVisibility(const vec3d& point, const vec4d& plane) const;


            vec4d   frustumPlanes[6];

    };
}

#endif

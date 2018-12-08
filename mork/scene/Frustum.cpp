#include "Frustum.h"

namespace mork {

void Frustum::setPlanes(const mat4d& toScreen) {

    const double *m = toScreen.coefficients();
    // Extract the LEFT plane
    frustumPlanes[0].x = m[12] + m[0];
    frustumPlanes[0].y = m[13] + m[1];
    frustumPlanes[0].z = m[14] + m[2];
    frustumPlanes[0].w = m[15] + m[3];
    // Extract the RIGHT plane
    frustumPlanes[1].x = m[12] - m[0];
    frustumPlanes[1].y = m[13] - m[1];
    frustumPlanes[1].z = m[14] - m[2];
    frustumPlanes[1].w = m[15] - m[3];
    // Extract the BOTTOM plane
    frustumPlanes[2].x = m[12] + m[4];
    frustumPlanes[2].y = m[13] + m[5];
    frustumPlanes[2].z = m[14] + m[6];
    frustumPlanes[2].w = m[15] + m[7];
    // Extract the TOP plane
    frustumPlanes[3].x = m[12] - m[4];
    frustumPlanes[3].y = m[13] - m[5];
    frustumPlanes[3].z = m[14] - m[6];
    frustumPlanes[3].w = m[15] - m[7];
    // Extract the NEAR plane
    frustumPlanes[4].x = m[12] + m[8];
    frustumPlanes[4].y = m[13] + m[9];
    frustumPlanes[4].z = m[14] + m[10];
    frustumPlanes[4].w = m[15] + m[11];
    // Extract the FAR plane
    frustumPlanes[5].x = m[12] - m[8];
    frustumPlanes[5].y = m[13] - m[9];
    frustumPlanes[5].z = m[14] - m[10];
    frustumPlanes[5].w = m[15] - m[11];
}

// Returns wether a point is inside this frustum
bool Frustum::isInside(const vec3d& point) const {
    Visibility v0 = getVisibility(point, frustumPlanes[0]);
    if (v0 == INVISIBLE) {
            return false;
    }
    Visibility v1 = getVisibility(point, frustumPlanes[1]);
    if (v1 == INVISIBLE) {
            return false;
    }
    Visibility v2 = getVisibility(point, frustumPlanes[2]);
    if (v2 == INVISIBLE) {
            return false;
    }
    Visibility v3 = getVisibility(point, frustumPlanes[3]);
    if (v3 == INVISIBLE) {
            return false;
    }
    Visibility v4 = getVisibility(point, frustumPlanes[4]);
    if (v4 == INVISIBLE) {
            return false;
    }

/*    if (v0 == FULLY_VISIBLE && v1 == FULLY_VISIBLE &&
                v2 == FULLY_VISIBLE && v3 == FULLY_VISIBLE &&
                    v4 == FULLY_VISIBLE)
    {
            return FULLY_VISIBLE;
    }
    return PARTIALLY_VISIBLE;*/
   return true;
}

// Returns the visibility of a bounding box
Visibility Frustum::getVisibility(const box3d& bb) const {

    Visibility v0 = getVisibility(bb, frustumPlanes[0]);
    if (v0 == INVISIBLE) {
            return INVISIBLE;
    }
    Visibility v1 = getVisibility(bb, frustumPlanes[1]);
    if (v1 == INVISIBLE) {
            return INVISIBLE;
    }
    Visibility v2 = getVisibility(bb, frustumPlanes[2]);
    if (v2 == INVISIBLE) {
            return INVISIBLE;
    }
    Visibility v3 = getVisibility(bb, frustumPlanes[3]);
    if (v3 == INVISIBLE) {
            return INVISIBLE;
    }
    Visibility v4 = getVisibility(bb, frustumPlanes[4]);
    if (v4 == INVISIBLE) {
            return INVISIBLE;
    }
    if (v0 == FULLY_VISIBLE && v1 == FULLY_VISIBLE &&
                v2 == FULLY_VISIBLE && v3 == FULLY_VISIBLE &&
                    v4 == FULLY_VISIBLE)
    {
            return FULLY_VISIBLE;
    }
    return PARTIALLY_VISIBLE;
}

Visibility Frustum::getVisibility(const box3d& bb, const vec4d& plane) const {
    double x0 = bb.xmin * plane.x;
    double x1 = bb.xmax * plane.x;
    double y0 = bb.ymin * plane.y;
    double y1 = bb.ymax * plane.y;
    double z0 = bb.zmin * plane.z + plane.w;
    double z1 = bb.zmax * plane.z + plane.w;
    double p1 = x0 + y0 + z0;
    double p2 = x1 + y0 + z0;
    double p3 = x1 + y1 + z0;
    double p4 = x0 + y1 + z0;
    double p5 = x0 + y0 + z1;
    double p6 = x1 + y0 + z1;
    double p7 = x1 + y1 + z1;
    double p8 = x0 + y1 + z1;
    if (p1 <= 0 && p2 <= 0 && p3 <= 0 && p4 <= 0 && p5 <= 0 && p6 <= 0 && p7 <= 0 && p8 <= 0) {
        return INVISIBLE;
    }
    if (p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0 && p5 > 0 && p6 > 0 && p7 > 0 && p8 > 0) {
        return FULLY_VISIBLE;
    }
    return PARTIALLY_VISIBLE;
}

Visibility Frustum::getVisibility(const vec3d& p, const vec4d& plane) const {
    double x = p.x * plane.x;
    double y = p.y * plane.y;
    double z = p.z * plane.z + plane.w;
    double p1 = x + y + z;
    if (p1 <= 0) {
        return INVISIBLE;
    } else {
        return FULLY_VISIBLE;
    }
}



}

#include "ransacPlaneDetection.h"

std::pair<Points, Points> RansacPlaneDetection::segment(const Points& input) {
  Points inliers;
  Points outliers;

  const auto kNumPoints = input.size();
  const auto seedId = getRandomInt(kNumPoints);

  /// the point contains the normal and center of the plane
  const auto plane = input[seedId];
  const float planeOffset =
      (plane.x * plane.nx) + (plane.y * plane.ny) + (plane.z * plane.nz);

  for (const auto p : input) {
    const float dist = dot(plane, p) - planeOffset;
    const float angle = find3DAngle(plane, p);

    if (dist < distThreshold && angle < angleThreshold) {
      inliers.push_back(p);
    } else {
      outliers.push_back(p);
    }
  }
  return std::pair<Points, Points>(inliers, outliers);
}

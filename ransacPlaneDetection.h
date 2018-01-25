#pragma once

#include <cmath>
#include <random>
#include <utility>
#include <vector>

#include "common.h"

class RansacPlaneDetection {
 private:
  const unsigned int randomSeed;
  const float distThreshold;
  const float angleThreshold;

 public:
  RansacPlaneDetection(const float& distThresh, const float& angleThresh,
                       const unsigned int& seed)
      : randomSeed(seed),
        distThreshold(distThresh),
        angleThreshold(angleThresh) {}

  std::pair<Points, Points> segment(const Points& input);

 private:
  unsigned int getRandomInt(const unsigned int& numPoints) {
    static std::default_random_engine generator(randomSeed);
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return (unsigned int)distribution(generator) * numPoints;
  }

  float dot(const Point& plane, const Point& p) {
    return (plane.nx * p.x) + (plane.ny * p.y) + (plane.nz * p.z);
  }

  float find3DAngle(const Point& v1, const Point& v2) {
    const float norm =
        sqrt(std::pow((v1.nx - v2.nx), 2) + std::pow((v1.ny - v2.ny), 2) +
             std::pow((v1.nz - v2.nz), 2));
    if (norm < 0.1) return 0.0f;

    const float dotProd = (v1.nx * v2.nx) + (v1.ny * v2.ny) + (v1.nz * v2.nz);
    float theta = std::acos(dotProd) * (180 / M_PI);
    if (theta > 90) {
      theta -= 180;
    }
    return std::abs(theta);
  }
};

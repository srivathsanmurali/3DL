#ifndef _TYPES_H_
#define _TYPES_H_

// STL
#include <cstdint>
#include <vector>
#include <string>
#include <cmath>

// pcLib
#include "common.h"

/*! \brief Point structure definition
 *
 *  Defines the strucutre for Point.
 *  It contains the necessary information that describes a point
 */
struct Point{
  float       x,y,z;
  float       nx, ny, nz;
  uint8_t     r,g,b,a;
  int         flags=0;

  Point()
  : x(0),
    y(0),
    z(0),
    nx(0),
    ny(0),
    nz(0),
    r(0),
    g(0),
    b(0),
    a(0),
    flags(0) {}

  void print() {
    LOG << "Point:" << std::endl
        << "[" << x << " " << y << " " << z << "]" << std::endl
        << "[" << nx << " " << ny << " " << nz << "]" << std::endl
        << "[" << r << " " << g << " " << b << " " << a  << "]"<< std::endl
        << "[" << flags << "]";
  }

  /// Used in find specific octree node
  unsigned int octreeCode = 0;
};

struct Face {
  std::vector<int>          ind;
  std::vector<float>        texCoords;
  uint8_t                   r,g,b,a;
  int32_t                   flags;
};

using Points = std::vector<Point>;
using Faces  = std::vector<Face>;

#endif // _TYPES_H_

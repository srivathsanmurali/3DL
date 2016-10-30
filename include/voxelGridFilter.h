#ifndef _VOXEL_GRID_FILTER_H_
#define _VOXEL_GRID_FILTER_H_

// STL
#include <vector>
#include <limits>
#include <map>
#include <cmath>

// 3DL headers
#include <common.h>
#include <plyIO.h>

class VoxelGridFilter {
  protected:
    const float               leafSize;
    const bool                useMediod;

  public:
    VoxelGridFilter(const float & _leafSize = 0.01f,
                    const bool & _useMediod = true)
      : leafSize(_leafSize), useMediod(_useMediod) {
      if(leafSize <= 0.0f) throwRuntimeError("Leaf size cannot be <= 0.0f");
    }

    void filter(const std::vector<Point>& inputPointCloud,
              std::vector<Point>& resultPointCloud);

  protected:
    void findMediod(const std::vector<Point>& points,
                    const std::vector<size_t>& ids, Point & np);
    void findMean(const std::vector<Point>& points,
                    const std::vector<size_t>& ids, Point & np);

}; // class VoxelGridFilter

#endif // _VOXEL_GRID_FILTER_H_

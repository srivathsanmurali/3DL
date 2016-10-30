#include <voxelGridFilter.h>
#include <omp.h>

void VoxelGridFilter::filter(const std::vector<Point> & inputPointCloud,
    std::vector<Point>& resultPointCloud) {
  const std::size_t numPoints(inputPointCloud.size());
  if(numPoints == 0) throwRuntimeError("Input point cloud cant be empty");

  // finding min max
  Point minP, maxP;
  minP.x = minP.y = minP.z = std::numeric_limits<float>::max();
  maxP.x = maxP.y = maxP.z = std::numeric_limits<float>::min();

  for (const auto & p : inputPointCloud) {
    if(p.x < minP.x) minP.x = p.x;
    if(p.y < minP.y) minP.y = p.y;
    if(p.z < minP.z) minP.z = p.z;
    if(p.x > maxP.x) maxP.x = p.x;
    if(p.y > maxP.y) maxP.y = p.y;
    if(p.z > maxP.z) maxP.z = p.z;
  }

  const size_t numVoxelY(static_cast<size_t>(ceil((maxP.y - minP.y)/leafSize)));
  const size_t numVoxelZ(static_cast<size_t>(ceil((maxP.z - minP.z)/leafSize)));

  std::map<size_t, std::vector<size_t> > voxels;
  std::vector<size_t> voxelIds;

  #pragma omp parallel for
  for (size_t i = 0; i < numPoints; i++) {
    const Point & p = inputPointCloud[i];
    const size_t x(static_cast<size_t>(floor((p.x - minP.x)/leafSize)));
    const size_t y(static_cast<size_t>(floor((p.y - minP.y)/leafSize)));
    const size_t z(static_cast<size_t>(floor((p.z - minP.z)/leafSize)));
    const size_t id((z * numVoxelZ) + (y * numVoxelY) + x);
    #pragma omp critical(VoxelGridUpdate)
    {
      if(voxels.find(id) == voxels.end()) {
        voxels[id] = std::vector<size_t> ();
        voxelIds.push_back(id);
      }
      voxels[id].push_back(i);
    }
  }

  const size_t numNewPoints(voxelIds.size());
  resultPointCloud.clear();
  resultPointCloud.reserve(numNewPoints);

  LOG << "Number of voxels = " << numNewPoints;
  #pragma omp parallel for
  for (size_t i = 0; i < numNewPoints; i++) {
    const size_t vId(voxelIds[i]);
    const size_t numVoxelPoints(voxels[vId].size());

    if (numVoxelPoints == 0) continue;
    Point np;
    if(useMediod) findMediod(inputPointCloud, voxels[vId], np);
    else findMean(inputPointCloud, voxels[vId], np);

    #pragma omp critical (PointsUpdate)
    {
      resultPointCloud.push_back(np);
      voxels[vId].clear();
    }
  }
}
void VoxelGridFilter::findMean(const std::vector<Point>& points,
    const std::vector<size_t>& ids, Point & np) {
  const size_t numVoxelPoints(ids.size());
  for (const auto & j : ids) {
    const Point & p = points[j];
    np.x += p.x;
    np.y += p.y;
    np.z += p.z;
    np.nx += p.nx;
    np.ny += p.ny;
    np.nz += p.nz;
    np.r += p.r;
    np.g += p.g;
    np.b += p.b;
    np.a += p.a;
  }
  np.x /= numVoxelPoints;
  np.y /= numVoxelPoints;
  np.z /= numVoxelPoints;
  np.nx /= numVoxelPoints;
  np.ny /= numVoxelPoints;
  np.nz /= numVoxelPoints;
  np.r /= numVoxelPoints;
  np.g /= numVoxelPoints;
  np.b /= numVoxelPoints;
  np.a /= numVoxelPoints;
}

void VoxelGridFilter::findMediod(const std::vector<Point>& points,
    const std::vector<size_t>& ids, Point & np) {
  float minDist = std::numeric_limits<float>::max();
  size_t minId = 0;

  for (const size_t & i : ids) {
    const Point & a = points[i];
    float dist = 0;

    for (const size_t j : ids) {
      if (i == j) continue;
      const Point & b = points[j];
      float curDist =
        sqrt(pow((a.x- b.x),2) + pow((a.y - b.y),2) + pow((a.z-b.z),2));
      dist += curDist;
    }

    if (dist < minDist) {
      minId = i;
      minDist = dist;
    }
  }
  np = points[minId];
}

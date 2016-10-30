#include <common.h>
#include <plyIO.h>
#include <voxelGridFilter.h>

int main(int argc, char **argv) {
    if (argc < 3) {
      LOG << "Need to more arguments" << std::endl
          << "\tUsage: ./voxelGridFilterEx pointCloud.ply out.ply";
      return EXIT_FAILURE;
    }

    PlyReader pr(argv[1]);
    pr.readFile();
    LOG << "Number of points = " << pr.points.size();

    VoxelGridFilter vgf(0.01, false);
    std::vector<Point> outPC;
    vgf.filter(pr.points, outPC);

    PlyWriter pw(argv[2]);
    pw.addVertexElement(true, true, true);
    pw.points = outPC;
    pw.writeToFile();

    return EXIT_SUCCESS;
}

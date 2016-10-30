#include <common.h>
#include <plyIO.h>

int main(int argc, char **argv) {
    if (argc < 2) {
      LOG << "Need to more arguments" << std::endl
          << "\tUsage: ./plyWrite_ex pointCloud.ply";
      return EXIT_FAILURE;
    }

    PlyWriter pw(argv[1], false);
    pw.addVertexElement(true, true);

    pw.points.resize(10);
    for ( float i = 0; i < 10.0; i += 1.0f) {
      pw.points[i].x = pw.points[i].y = pw.points[i].z = i;
      pw.points[i].nx = pw.points[i].ny = pw.points[i].nz = i;
    }

    pw.writeToFile();
    return EXIT_SUCCESS;
}


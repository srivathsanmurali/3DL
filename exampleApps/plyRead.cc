#include <common.h>
#include <plyIO.h>

int main(int argc, char **argv) {
    if (argc < 2) {
      LOG << "Need to more arguments" << std::endl
          << "\tUsage: ./plyRead_ex pointCloud.ply";
      return EXIT_FAILURE;
    }

    PlyReader pr(argv[1]);

    bool done = false;
    int i =0;
    do {
      Point p;
      done = pr.readPoint(p);
      i++;
      p.print();
    } while ( done && i < 10) ;
    return EXIT_SUCCESS;
}


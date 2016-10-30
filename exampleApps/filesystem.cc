#include <file_system.hpp>
#include <common.h>
#include <fstream>

int main(int argc, char **argv) {
  if(!stlplus::folder_exists ("./testFolder")){
    LOG << "Folder doesnts exist. Creating it!";
    stlplus::folder_create ("./testFolder");
  } else {
    LOG << "Folder exists :)";
  }
  return EXIT_SUCCESS;
}


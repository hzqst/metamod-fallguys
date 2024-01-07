#include "procmap/MemoryMap.hpp"
#include "procmap/Logger.hpp"

namespace procmap {

MemoryMap::MemoryMap() {
  char *line = NULL;
  size_t line_size = 0;

  // open maps file
  FILE *maps = fopen("/proc/self/maps", "r");
  DIEIF(maps == nullptr, "error opening maps file");

  // parse the maps file
  while (getline(&line, &line_size, maps) > 0) {
    emplace_back(line);
  }

  // cleanup
  free(line);
  DIEIF(!feof(maps) || ferror(maps), "error parsing maps file");
  DIEIF(fclose(maps), "error closing maps file");
}

void MemoryMap::print() {
  for (auto &segment : *this) {
    segment.print();
  }
}

}  // namespace procmap

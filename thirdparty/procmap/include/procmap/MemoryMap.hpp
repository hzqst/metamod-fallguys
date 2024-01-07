#ifndef PROCMAP_MEMORY_MAP
#define PROCMAP_MEMORY_MAP

#include "procmap/MemorySegment.hpp"

namespace procmap {

class MemoryMap : private std::vector<MemorySegment> {
 public:
  MemoryMap();
  void print();

  //allowed methods from std::vector
  using vector::operator[];
  using vector::begin;
  using vector::end;

};

}  // namespace procmap

#endif  // PROCMAP_MEMORY_MAP

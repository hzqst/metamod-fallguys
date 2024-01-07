#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <procmap/MemoryMap.hpp>

int main() {
	procmap::MemoryMap m;
	m.print();
  return 0;
}

#ifndef PROCMAP_MEMORY_SEGMENT
#define PROCMAP_MEMORY_SEGMENT

#include <stdlib.h>
#include <string>
#include <vector>

namespace procmap {

class MemorySegment {
  void*         _startAddress;
  void*         _endAddress;
  unsigned long _offset;
  unsigned int  _deviceMajor;
  unsigned int  _deviceMinor;
  ino_t         _inode;
  unsigned char _permissions;
	std::string   _name;

 public:
  MemorySegment(char *unparsed_line);
  // getters
  void* startAddress();
  void* endAddress();
  std::string name();
  size_t length();
  dev_t device();
  // getters for the permissions bitmask
  bool isReadable();
  bool isWriteable();
  bool isExecutable();
  bool isShared();
  bool isPrivate();
  // other functions
  bool isBindable();
  bool isAnonymous();
  bool isHeap();
  bool isStack();
  void print();
};

}  // namespace procmap

#endif  // PROCMAP_MEMORY_SEGMENT

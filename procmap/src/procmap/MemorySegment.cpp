#include "procmap/MemorySegment.hpp"
#include "procmap/Logger.hpp"
#include <sys/sysmacros.h>

namespace procmap {

MemorySegment::MemorySegment(char *line) {
  int name_start = 0, name_end = 0;
  unsigned long addr_start, addr_end;
  char perms_str[8];
  //printf("line: %s", line);

  // parse string
  DIEIF(sscanf(line, "%lx-%lx %7s %lx %u:%u %lu %n%*[^\n]%n",
                     &addr_start, &addr_end, perms_str, &_offset,
                     &_deviceMajor, &_deviceMinor, &_inode,
                     &name_start, &name_end) < 7,
        "FAILED TO PARSE");

  // convert addresses
  _startAddress = reinterpret_cast<void*>(addr_start);
  _endAddress = reinterpret_cast<void*>(addr_end);

  // copy permissions
  _permissions = 0U;
  if (strchr(perms_str, 'r'))
    _permissions |= 1U << 0;
  if (strchr(perms_str, 'w'))
    _permissions |= 1U << 1;
  if (strchr(perms_str, 'x'))
    _permissions |= 1U << 2;
  if (strchr(perms_str, 's'))
    _permissions |= 1U << 3;
  if (strchr(perms_str, 'p'))
    _permissions |= 1U << 4;

  // copy name
  if (name_end > name_start) {
    line[name_end] = '\0';
    _name.assign(&line[name_start]);
  }
}

void* MemorySegment::startAddress() {
  return _startAddress;
}

void* MemorySegment::endAddress() {
  return _endAddress;
}

std::string MemorySegment::name() {
  return _name;
}

size_t MemorySegment::length() {
  return ((char*)_endAddress) - ((char*)_startAddress);
}

dev_t MemorySegment::device() {
  return makedev(_deviceMajor, _deviceMinor);
}

bool MemorySegment::isReadable() {
  return (_permissions & 1U) != 0;
}

bool MemorySegment::isWriteable() {
  return (_permissions & 2U) != 0;
}

bool MemorySegment::isExecutable() {
  return (_permissions & 4U) != 0;
}

bool MemorySegment::isShared() {
  return (_permissions & 8U) != 0;
}

bool MemorySegment::isPrivate() {
  return (_permissions & 16U) != 0;
}

void MemorySegment::print() {
  char info[1024];
  snprintf(info, sizeof(info),
           "[%18p-%18p] (%5lu pages) [off=%7lu] [dev=%u:%u] [inode=%8lu] %c%c%c%c '%s'",
           _startAddress, _endAddress,
           length() / sysconf(_SC_PAGESIZE),
           _offset,
           _deviceMajor, _deviceMinor,
           _inode,
           (isPrivate() ?    'P' : 'S'),
           (isExecutable() ? 'X' : '-'),
           (isWriteable() ?  'W' : '-'),
           (isReadable() ?   'R' : '-'),
           _name.c_str());
  L->printHorizontalRule(info, (isWriteable() ? 2 : 1));
}

bool MemorySegment::isBindable() {
  return name() != "[vsyscall]";
}

bool MemorySegment::isHeap() {
  return name() == "[heap]";
}

bool MemorySegment::isStack() {
  return name() == "[stack]";
}

bool MemorySegment::isAnonymous() {
  return name().length() == 0;
}

}  // namespace procmap

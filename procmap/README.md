[![Build Status](https://travis-ci.com/joaomlneto/procmap.svg?branch=master)](https://travis-ci.com/joaomlneto/procmap)

# procmap
A simple C++ wrapper around `/proc/self/maps`

This tool makes it simpler to help visualize the memory map of your process.

It's super simplistic and was created to suit my own needs. Hopefully it provides a good start for you and you'll enhance it with a few more options (and submit a PR!) :-)

## Examples

### Reading `/proc/self/maps`

Couldn't be easier.
```cpp
MemoryMap m;
```

### Visualizing `/proc/self/maps`

```cpp
MemoryMap m;
m.print();
```

![screenshot 2018-11-05 at 19 04 40](https://user-images.githubusercontent.com/1539767/48020367-a9011b00-e12d-11e8-8f2c-03d29bf786ad.png)

### Iterating through the entries and selecting only those that are writeable

```cpp
MemoryMap map;
for (auto &segment : map) {
  if (segment.isWriteable()) {
    segment.print();
  }
}
```

![image](https://user-images.githubusercontent.com/1539767/48020839-d1d5e000-e12e-11e8-87be-5e7ced7e1e88.png)

## Reference

MemoryMap is basically a collection of MemorySegments. Use it as you would a `std::vector`.

MemorySegment represents a single entry/line in `/proc/*/maps`. It provides a plethora of utility functions to make life more pleasant. Check the [header file](https://github.com/joaomlneto/process-memory-map/blob/master/include/procmap/MemorySegment.hpp) for a complete list.

## Suggestions / Complaints
Open an issue! :-)

## Contributing
PRs are more than welcome.

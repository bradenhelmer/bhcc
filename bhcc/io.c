// io.c
// ~~~~
// IO routine implementations.
#include "io.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

const char *map_source_file(const char *file_name, size_t *length) {
  int fd = open(file_name, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Error reading source file %s! Exiting...", file_name);
    ;
    exit(ENOENT);
  }
  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    fprintf(stderr, "Error obtaining file size!\n");
    exit(EIO);
  }

  *length = sb.st_size;
  madvise(NULL, *length, MADV_SEQUENTIAL);
  const char *addr =
      (const char *)mmap(NULL, *length, PROT_READ, MAP_PRIVATE, fd, 0u);
  if (addr == MAP_FAILED) {
    fprintf(stderr, "Mmap error!\n");
    exit(EIO);
  }
  return addr;
}

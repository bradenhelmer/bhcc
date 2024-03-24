// io.c
// ~~~~
// IO routine implementations.
#include "util/error.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

char *map_source_file(const char *file_name, size_t *length) {
  int fd = open(file_name, O_RDONLY);
  if (fd == -1) {
    char msg[512];
    sprintf(msg, "Error reading source file %s! Exiting...", file_name);
    bhcc_errorln_code(msg, ENOENT);
  }
  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    bhcc_errorln_code("Error obtaining file size!\n", EIO);
  }

  *length = sb.st_size;
  madvise(NULL, *length, MADV_SEQUENTIAL);
  char *addr = (char *)mmap(NULL, *length, PROT_READ, MAP_PRIVATE, fd, 0u);
  if (addr == MAP_FAILED) {
    bhcc_errorln_code("Mmap error!\n", EIO);
  }
  return addr;
}

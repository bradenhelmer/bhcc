// io.h
// ~~~~
// IO routine definitions.
#ifndef BHCC_IO_H
#define BHCC_IO_H
#include <unistd.h>

// Gets file contents from file name.
const char *map_source_file(const char *file_name, size_t *length);

#endif /* ifndef BHCC_IO_H */

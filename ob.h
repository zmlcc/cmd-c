#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct output_buffer {
  char *buffer;
  size_t buffer_size;
  size_t used_size;
} ob;

int
ob_nprintf(ob *buf, const char *fmt, ...);

ob *
ob_new(size_t buffer_size);

void
ob_free(ob *buf);

void
ob_reset(ob *buf);
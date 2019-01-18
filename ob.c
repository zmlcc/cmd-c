#include <stdarg.h>
#include "ob.h"

int ob_nprintf(ob *buf, const char *fmt, ...)
{
    size_t unused_size = buf->buffer_size - buf->used_size;
    if (unused_size == 0)
    {
        return 0;
    }
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf->buffer + buf->used_size, unused_size, fmt, ap);
    va_end(ap);
    buf->used_size += n;
    return n;
}

ob *ob_new(size_t buffer_size)
{
    ob *buf;
    buf = calloc(1, sizeof(ob));
    buf->buffer = calloc(1, buffer_size);
    buf->buffer_size = buffer_size;
    buf->used_size = 0;
    return buf;
}

void ob_free(ob *buf)
{
    free(buf->buffer);
    free(buf);
    buf = NULL;
}

void ob_reset(ob *buf)
{
    buf->used_size = 0;
    memset(buf->buffer, 0, buf->buffer_size);
}
#include "parser.h"

#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define TMPBUF_SIZE 1024

enum {
    EMPTY,
    COPY,
    CUT,
};

static char *src_path;
static char *filename;
static int otype = EMPTY;

void copy(const char *path, const char *name) {
    if (src_path == NULL) {
        src_path = calloc(1, PATH_MAX);
        if (src_path == NULL) {
            return;
        }
    }
    if (filename == NULL) {
        filename = calloc(1, NAME_MAX);
        if (filename == NULL) {
            return;
        }
    }

    strcpy(src_path, path);
    strcpy(filename, name);

    otype = COPY;
}

void paste(const char *path) {
    if (otype == EMPTY) {
        return;
    }
    if (strcmp(path, src_path) == 0 || has_such_file(path, filename) != 0) {
        otype = EMPTY;
        return;
    }

    char *dst_path = calloc(1, PATH_MAX);
    if (dst_path == NULL) {
        return;
    }

    concat_path(src_path, filename);
    snprintf(dst_path, PATH_MAX, "%s/%s", path, filename);

    int src = open(src_path, O_RDONLY);
    if (src == -1) {
        otype = EMPTY;
        return;
    }

    int dst = open(dst_path, O_WRONLY | O_CREAT);
    if (dst == -1) {
        close(src);
        otype = EMPTY;
        return;
    }

    char buffer[TMPBUF_SIZE];
    size_t bytes_read;
    while ((bytes_read = read(src, &buffer, TMPBUF_SIZE)) > 0) {
        char *offset = buffer;
        ssize_t bytes_write = write(dst, offset, bytes_read);
        if (bytes_write > -1) {
            offset += bytes_write;
            bytes_read -= bytes_write;
        }
        while (bytes_read > 0) {
            ssize_t bytes_write = write(dst, offset, bytes_read);
            if (bytes_write > -1) {
                offset += bytes_write;
                bytes_read -= bytes_write;
            }
        }
    }

    close(src);
    close(dst);

    struct stat st;
    stat(src_path, &st);
    chmod(dst_path, st.st_mode);

    free(dst_path);

    if (otype == CUT) {
        unlink(src_path);
    }

    otype = EMPTY;
}

void cut(const char *path, const char *name) {
    if (src_path == NULL) {
        src_path = calloc(1, PATH_MAX);
        if (src_path == NULL) {
            return;
        }
    }
    if (filename == NULL) {
        filename = calloc(1, NAME_MAX);
        if (filename == NULL) {
            return;
        }
    }

    strcpy(src_path, path);
    strcpy(filename, name);

    otype = CUT;
}
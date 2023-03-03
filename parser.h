#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct direct {
    int size;
    struct dirent *dirs;
} direct;

enum {
    NO_SUCH_FILE = 0,
    HAS_SUCH_FILE = 1,
};

enum {
    ERR_NO_DIR_GIVEN = -1,
    ERR_NO_DIR = -2,
    ERR_ALLOC = -3,
};

/* deletes last elem on path */
void pop_path(char *path) {
    size_t len = strlen(path);
    --len;
    while (path[len] != '/') {
        --len;
    }
    if (len == 0) {
        return;
    }
    path[len] = '\0';
}

/* adds next_dir to path */
void concat_path(char *path, const char *next_dir) {
    char delimetr[] = "/";
    strncat(path, delimetr, 1);
    strncat(path, next_dir, PATH_MAX - strlen(path));
}

int cmpstruct(const void *a, const void *b) {
    const struct dirent *d1 = a;
    const struct dirent *d2 = b;
    return strcmp(d1->d_name, d2->d_name);
}

/* Returns an array of directory elements sorted by name */
int get_dir_content(const char *path, direct *cur_dir, const bool show_hid) {
    if (cur_dir == NULL) {
        return ERR_NO_DIR_GIVEN;
    }
    cur_dir->size = 0;

    DIR *directory = opendir(path);
    if (directory == NULL) {
        return ERR_NO_DIR;
    }

    struct dirent *dir;
    while ((dir = readdir(directory))) {
        if (strcmp(dir->d_name, ".") == 0) {
            continue;
        }
        if (strcmp(dir->d_name, "..") == 0) {
            ++cur_dir->size;
            continue;
        }
        if (dir->d_name[0] != '.' || show_hid) {
            ++cur_dir->size;
        }
    }

    cur_dir->dirs = calloc(cur_dir->size, sizeof(*cur_dir->dirs));
    if (cur_dir->dirs == NULL) {
        return ERR_ALLOC;
    }

    rewinddir(directory);

    int i = 0;
    while ((dir = readdir(directory))) {
        if (strcmp(dir->d_name, ".") == 0) {
            continue;
        }
        if (strcmp(dir->d_name, "..") == 0) {
            cur_dir->dirs[i] = *dir;
            ++i;
            continue;
        }
        if (dir->d_name[0] != '.' || show_hid) {
            cur_dir->dirs[i] = *dir;
            ++i;
        }
    }

    closedir(directory);

    qsort(cur_dir->dirs, cur_dir->size, sizeof(struct dirent), cmpstruct);

    return 0;
}

int has_such_file(const char *path, const char *name) {
    DIR *directory = opendir(path);
    if (directory == NULL) {
        return ERR_NO_DIR;
    }

    struct dirent *dir;
    while ((dir = readdir(directory))) {
        if (strcmp(dir->d_name, name) == 0) {
            return HAS_SUCH_FILE;
        }
    }
    return NO_SUCH_FILE;
}
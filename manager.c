#include "manager.h"
#include "parser.h"

#include <linux/limits.h>
#include <sys/stat.h>
#include <unistd.h>

static char *cur_path;
static direct cur_dir;

static int CURSOR_POS = 0;

static int CURSOR_PADDING = 0;
static int FILENAME_PADDING = 2;
static int FILEINFO_PADDING = 40;

static int HEIGHT;
static int WIDTH;

void DrawWindow() {
    getmaxyx(stdscr, HEIGHT, WIDTH);

    // parse dir
    int err = get_dir_content(cur_path, &cur_dir);
    if (err == ERR_NO_DIR_GIVEN) {
        return;
    }
    if (err == ERR_NO_DIR) {
        pop_path(cur_path);
        DrawWindow();
        return;
    }
    if (err == ERR_ALLOC) {
        return;
    }

    erase();  // clear screen

    // count margins
    FILEINFO_PADDING = WIDTH - (WIDTH - FILENAME_PADDING) / 5;

    // printing header
    char files[] = "FILES in ";
    mvprintw(0, 0, "%s", files);
    {
        size_t bufsize = FILEINFO_PADDING - strlen(files) - 1;
        char buf[bufsize];
        snprintf(buf, bufsize, "%s", cur_path);
        mvprintw(0, strlen(files), "%s", buf);
    }
    mvprintw(0, FILEINFO_PADDING, "SIZE");

    // adjusting cursor pos
    if (CURSOR_POS >= cur_dir.size) {
        CURSOR_POS = 0;
    }

    // printing directory files
    int height_shift = 0;
    if (CURSOR_POS + N_FIRST_LINE >= HEIGHT) {
        height_shift = CURSOR_POS - HEIGHT + 2;
    }

    for (int i = height_shift; i < cur_dir.size; ++i) {
        int color;

        switch (cur_dir.dirs[i].d_type) {
        case DT_DIR:
            color = DIR_COLOR_PALETE;
            break;

        case DT_LNK:
            color = LNK_COLOR_PALETE;
            break;

        case DT_FIFO:
            color = FIFO_COLOR_PALETE;
            break;

        default:
            color = DFLT_COLOR_PATETE;
            break;
        }

        int y = N_FIRST_LINE + i - height_shift;
        struct dirent cur_dirent = cur_dir.dirs[i];

        size_t bufsize = FILEINFO_PADDING - FILENAME_PADDING - 1;
        char buf[bufsize];
        snprintf(buf, bufsize, "%s", cur_dirent.d_name);

        attron(COLOR_PAIR(color));
        mvprintw(y, FILENAME_PADDING, buf);
        attroff(COLOR_PAIR(color));

        // printing size
        if (strcmp(cur_dirent.d_name, "..") == 0) {
            continue;
        }

        struct stat dir_stat;
        concat_path(cur_path, cur_dirent.d_name);
        if (lstat(cur_path, &dir_stat) != 0) {
            continue;
        }
        pop_path(cur_path);
        mvprintw(y, FILEINFO_PADDING, "%ld", dir_stat.st_size);
    }

    // printing cursor
    mvprintw(CURSOR_POS + N_FIRST_LINE - height_shift, CURSOR_PADDING, "->");
}

void HandleKey(int key) {
    switch (key) {

    case KEY_UP:
        if (CURSOR_POS > 0) {
            --CURSOR_POS;
        }
        break;

    case KEY_DOWN:
        ++CURSOR_POS;
        break;

    case '\n':
        if (cur_dir.dirs[CURSOR_POS].d_type == DT_DIR) {
            if (strcmp(cur_dir.dirs[CURSOR_POS].d_name, "..") == 0) {
                pop_path(cur_path);
            } else {
                concat_path(cur_path, cur_dir.dirs[CURSOR_POS].d_name);
            }
        }
        break;

    case 'd':
        concat_path(cur_path, cur_dir.dirs[CURSOR_POS].d_name);
        if (cur_dir.dirs[CURSOR_POS].d_type == DT_DIR) {
            rmdir(cur_path);
        } else {
            unlink(cur_path);
        }
        pop_path(cur_path);
        break;
    }
}

void OnStart() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    cur_path = calloc(1, PATH_MAX);
    if (cur_path == NULL) {
        endwin();
        exit(1);
    }

    cur_path = getcwd(cur_path, PATH_MAX);
    CURSOR_POS = 0;

    InitColors();

    DrawWindow();
}

void OnFinish() {
    free(cur_path);
    endwin();
}

void InitColors() {
    if (has_colors() == 0) {
        OnFinish();
        exit(1);
    }
    start_color();

    init_pair(DFLT_COLOR_PATETE, COLOR_WHITE, COLOR_BLACK);
    init_pair(DIR_COLOR_PALETE, COLOR_BLUE, COLOR_BLACK);
    init_pair(LNK_COLOR_PALETE, COLOR_RED, COLOR_BLACK);
    init_pair(FIFO_COLOR_PALETE, COLOR_GREEN, COLOR_BLACK);
}
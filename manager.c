#include "manager.h"
#include "utils.h"

#include <sys/stat.h>

static char *CUR_PATH;
static direct CUR_DIR;

static int CURSOR_POS = 0;

static int CURSOR_PADDING = 0;
static int FILENAME_PADDING = 2;
static int FILEINFO_PADDING = 40;

static int HEIGHT;
static int WIDTH;

static int SHOW_HIDDEN = true;

void DrawWindow() {
    // count margins
    getmaxyx(stdscr, HEIGHT, WIDTH);
    FILEINFO_PADDING = WIDTH - (WIDTH - FILENAME_PADDING) / 5;

    // parse dir
    int err = get_dir_content(CUR_PATH, &CUR_DIR, SHOW_HIDDEN);
    if (err == ERR_NO_DIR_GIVEN) {
        return;
    }
    if (err == ERR_NO_DIR) {
        pop_path(CUR_PATH);
        DrawWindow();
        return;
    }
    if (err == ERR_ALLOC) {
        return;
    }

    erase();  // clear screen

    // printing header
    char files[] = "FILES in ";
    mvprintw(0, 0, "%s", files);
    {
        size_t bufsize = FILEINFO_PADDING - strlen(files) - 1;
        char buf[bufsize];
        snprintf(buf, bufsize, "%s", CUR_PATH);
        mvprintw(0, strlen(files), "%s", buf);
    }
    mvprintw(0, FILEINFO_PADDING, "SIZE");

    // adjusting cursor pos
    if (CURSOR_POS >= CUR_DIR.size) {
        CURSOR_POS = 0;
    }

    // printing directory files
    int height_shift = 0;
    if (CURSOR_POS + N_FIRST_LINE >= HEIGHT) {
        height_shift = CURSOR_POS - HEIGHT + 2;
    }

    for (int i = height_shift; i < CUR_DIR.size; ++i) {
        int color;

        switch (CUR_DIR.dirs[i].d_type) {
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
        struct dirent cur_dirent = CUR_DIR.dirs[i];

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
        concat_path(CUR_PATH, cur_dirent.d_name);
        if (lstat(CUR_PATH, &dir_stat) != 0) {
            continue;
        }
        pop_path(CUR_PATH);
        mvprintw(y, FILEINFO_PADDING, "%ld", dir_stat.st_size);
    }

    // printing cursor
    mvprintw(CURSOR_POS + N_FIRST_LINE - height_shift, CURSOR_PADDING, "->");
}

void HandleKey(const int key) {
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
        if (CUR_DIR.dirs[CURSOR_POS].d_type == DT_DIR) {
            if (strcmp(CUR_DIR.dirs[CURSOR_POS].d_name, "..") == 0) {
                pop_path(CUR_PATH);
            } else {
                concat_path(CUR_PATH, CUR_DIR.dirs[CURSOR_POS].d_name);
            }
        }
        break;

    case 'd':
        concat_path(CUR_PATH, CUR_DIR.dirs[CURSOR_POS].d_name);
        if (CUR_DIR.dirs[CURSOR_POS].d_type == DT_DIR) {
            rmdir(CUR_PATH);
        } else {
            unlink(CUR_PATH);
        }
        pop_path(CUR_PATH);
        break;

    case 'h':
        SHOW_HIDDEN = !SHOW_HIDDEN;
        break;

    case 'c':
        if (CUR_DIR.dirs[CURSOR_POS].d_type == DT_REG) {
            copy(CUR_PATH, CUR_DIR.dirs[CURSOR_POS].d_name);
        }
        break;

    case 'x':
        if (CUR_DIR.dirs[CURSOR_POS].d_type == DT_REG) {
            cut(CUR_PATH, CUR_DIR.dirs[CURSOR_POS].d_name);
        }
        break;

    case 'p':
        paste(CUR_PATH);
        break;
    }
}

void OnStart() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    CUR_PATH = calloc(1, PATH_MAX);
    if (CUR_PATH == NULL) {
        endwin();
        exit(1);
    }

    CUR_PATH = getcwd(CUR_PATH, PATH_MAX);
    CURSOR_POS = 0;

    InitColors();

    DrawWindow();
}

void OnFinish() {
    free(CUR_PATH);
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
#include <ncurses.h>
#include <stdlib.h>

enum {
    N_FIRST_LINE = 1,

    DFLT_COLOR_PATETE = 1,
    DIR_COLOR_PALETE = 2,
    LNK_COLOR_PALETE = 3,
    FIFO_COLOR_PALETE = 4,
};

void DrawWindow();

void HandleKey(const int key);

void OnFinish();

void InitColors();

void OnStart();
#include "manager.h"

int main() {
    OnStart();

    int key = 0;
    while (key != 'q') {
        key = getch();
        HandleKey(key);
        DrawWindow();
    }

    OnFinish();
    return 0;
}
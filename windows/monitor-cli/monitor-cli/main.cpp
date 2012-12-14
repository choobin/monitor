#include <windows.h>
#include <stdio.h>
#include <conio.h> // _kbhit

#include "monitor.h"
#pragma comment(lib, "monitor.lib")

#define SAMPLE_INTERVAL_MS 500

int main(int argc, char **argv)
{
    Monitor m;

    while (!_kbhit()) {
        Data d = m.poll();

        system("cls");

        printf("rx: %.2f KB/s\r\n"
               "tx: %.2f KB/s\r\n",
               d.rx,
               d.tx);

        Sleep(SAMPLE_INTERVAL_MS);
    }

    return EXIT_SUCCESS;
}

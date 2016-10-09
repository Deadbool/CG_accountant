#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Shoot enemies before they collect all the incriminating data!
 * The closer you are to an enemy, the more damage you do but don't get too close or you'll get killed.
 **/
int main()
{

    // game loop
    while (1) {
        int x;
        int y;
        scanf("%d%d", &x, &y);
        int dataCount;
        scanf("%d", &dataCount);
        for (int i = 0; i < dataCount; i++) {
            int dataId;
            int dataX;
            int dataY;
            scanf("%d%d%d", &dataId, &dataX, &dataY);
        }
        int enemyCount;
        scanf("%d", &enemyCount);
        for (int i = 0; i < enemyCount; i++) {
            int enemyId;
            int enemyX;
            int enemyY;
            int enemyLife;
            scanf("%d%d%d%d", &enemyId, &enemyX, &enemyY, &enemyLife);
        }

        // Write an action using printf(). DON'T FORGET THE TRAILING \n
        // To debug: fprintf(stderr, "Debug messages...\n");

        printf("MOVE 8000 4500\n"); // MOVE x y or SHOOT id
    }

    return 0;
}

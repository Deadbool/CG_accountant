#ifndef SRC_GAME_H_
#define SRC_GAME_H_

typedef struct {
	// Wolff position
	int x;
	int y;

	// Data
	int dcount;
	// TODO add data and annemies
}Input;

typedef struct {
	int turn;
}Game;

void Game_init(Game *game);


#endif /* SRC_GAME_H_ */

#ifndef __ARENA_H__
#define __ARENA_H__

//update forbiddenMoves
void dontGoBack(int forbiddenMoves[4], int myMove);
void dontTouchSnakes(int forbiddenMoves[4], int myX, int myY, int longueur, int* mySnake, int* hisSnake);
void dontTouchBorders(int forbiddenMoves[4], int myX, int myY, int X, int Y);
void dontTouchWalls(int forbiddenMoves[4], int myX, int myY, int nbWalls, int* walls);

//update arena info

//màj tab coord d'un snake
void updateSnake(int* snake, int longueur, int curPosx, int curPosy);

//crée arène + met les forbidden de base (murs+bords)
int*** initArena(int X, int Y, int nbWalls, int* walls);

//prend en compte la position du snake (à placer après les màj de forbiddenMoves) + expansion (réglage distance max expansion)
void updateArena(int*** updtatedArena, int*** baseArena, int X, int Y,  int myX, int myY, int* mySnake, int* hisSnake, int longueur, int forbiddenMoves[4], int distMaxExp);
//remonte depuis la distance max jusqu'à ta vieille tete (retourne -1 si ya eu un pb)
t_move chooseMyMove(int*** updatedArena, int X, int Y, int myX, int myY, int distMax);

#endif
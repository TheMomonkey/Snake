/*
 * arena.h
 * (header des fonctions de arena.c)
 * 
 * SADOUKI Aya
 * 
 */


#ifndef __ARENA_H__
#define __ARENA_H__

//met à jour le tableau des mouvements interdits
void dontGoBack(int forbiddenMoves[4], int myMove);
void dontTouchSnakes(int forbiddenMoves[4], int myX, int myY, int longueur, int* mySnake, int* hisSnake);
void dontTouchBorders(int forbiddenMoves[4], int myX, int myY, int X, int Y);
void dontTouchWalls(int forbiddenMoves[4], int myX, int myY, int nbWalls, int* walls);

//màj tab coord d'un snake
void updateSnake(int* snake, int longueur, int curPosx, int curPosy);

//crée une arène de base avec les interdits de base (murs+bords)
int*** initArena(int X, int Y, int nbWalls, int* walls);

//màj infos arène
//prend en compte la nouvelle position des snakes + phase d'expansion (marque les cases de la distance)
void updateArena(int*** updatedArena, int*** baseArena, int X, int Y,  int myX, int myY, int* mySnake, int* hisSnake, int longueur, int forbiddenMoves[4], int distMaxExp);

//remonte depuis la distance max jusqu'à moi (retourne -1 si pb). Retourne la direction choisie pour aller le plus loin possible
t_move chooseMyMove(int*** updatedArena, int X, int Y, int myX, int myY, int distMax);



//affichages divers
//affichage des infos des cases de l'arène par exemple : arena[3][2][north] = 1 (impossible d'aller en haut pour la case 2;3)
void printArenaInfo(int*** arena, int X, int Y);
//affichage des coordonnées d'un snake de la forme : snakeX[1]=2, snakeY[1]=1 (la 2e case occupée par le serpent est la case 2;1)
void printSnakeInfo(int* snake, int X, int Y);

#endif

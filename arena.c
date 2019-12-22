#include <stdio.h>
#include <stdlib.h>
#include "snakeAPI.h"
#include "arena.h"

void dontGoBack(int forbiddenMoves[4], int myMove){
	switch(myMove){
		case 0:
			forbiddenMoves[2] = 1;
			//printf("here pas 1\n");
			break;
		case 1:
			forbiddenMoves[3]= 1;
			//printf("here pas 2\n");
			break;
		case 2:
			forbiddenMoves[0]= 1;
			//printf("here pas 3\n");
			break;
		case 3:
			forbiddenMoves[1]= 1;
			printf("here dontGoBack east\n");
			break;
		default:
			break;
	}
}

void dontTouchSnakes(int forbiddenMoves[4], int myX, int myY, int longueur, int* mySnake, int* hisSnake){
	for (int i = 0; i < 2*longueur; i += 2){
		if ((myX-1 == mySnake[i] && myY == mySnake[i+1]) || (myX-1 == hisSnake[i] && myY == hisSnake[i+1])){
			forbiddenMoves[3] = 1;
			//printf("here toi 1\n");
		}
		if ((myX+1 == mySnake[i] && myY == mySnake[i+1]) || (myX+1 == hisSnake[i] && myY == hisSnake[i+1])){
			forbiddenMoves[1] = 1;
			printf("here toi 2\n");
		}
		if ((myY-1 == mySnake[i+1] && myX == mySnake[i]) || (myY-1 == hisSnake[i+1] && myX == hisSnake[i])){
			forbiddenMoves[0] = 1;
			//printf("here toi 3\n");
		}
		if ((myY+1 == mySnake[i+1] && myX == mySnake[i]) || (myY+1 == hisSnake[i+1] && myX == hisSnake[i])){
			forbiddenMoves[2] = 1;
			//printf("here toi 4\n");
		}
	}
}

void dontTouchBorders(int forbiddenMoves[4], int myX, int myY, int X, int Y){
	if (myX == 0){
		forbiddenMoves[3] = 1;
		//printf("here bords 1\n");
	}
	if (myX == X-1){
		forbiddenMoves[1] = 1;
		printf("here bords east\n");
	}
	if (myY == 0){
		forbiddenMoves[0] = 1;
		//printf("here bords 3\n");
	}
	if (myY == Y-1){
		forbiddenMoves[2] = 1;
		//printf("here bords 4\n");
	}
}

void dontTouchWalls(int forbiddenMoves[4], int myX, int myY, int nbWalls, int* walls){
	for (int i = 0; i < 4*nbWalls; i+=4){
		// walls[i]   = x1
		// walls[i+1] = y1
		// walls[i+2] = x2
		// walls[i+3] = y2
		if (walls[i] == myX && walls[i+1] == myY){
			if(walls[i+2] == myX-1){ //mur à gauche
				forbiddenMoves[3] = 1;
				//printf("here mur 1\n");
			}
			if(walls[i+2] == myX+1){ //mur à droite
				forbiddenMoves[1] = 1;
				printf("here mur east 1\n");
			}
			if(walls[i+3] == myY-1){ //mur en haut
				forbiddenMoves[0] = 1;
				//printf("here mur 3\n");
			}
			if(walls[i+3] == myY+1){ //mur en bas
				forbiddenMoves[2] = 1;
				//printf("here mur 4\n");
			}
		}
		if (walls[i+2] == myX && walls[i+3] == myY){
			if(walls[i] == myX-1){ //mur à gauche
				forbiddenMoves[3] = 1;
				//printf("here mur 5\n");
			}
			if(walls[i] == myX+1){ //mur à droite
				forbiddenMoves[1] = 1;
				printf("here mur east 2\n");
			}
			if(walls[i+1] == myY-1){ //mur en haut
				forbiddenMoves[0] = 1;
				//printf("here mur 7\n");
			}
			if(walls[i+1] == myY+1){ //mur en bas
				forbiddenMoves[2] = 1;
				//printf("here mur 8\n");
			}
		}
	}
}

void updateSnake(int* snake, int longueur, int newPosx, int newPosy){
	for (int i = 2*longueur-2; i > 1 ; i -= 2){
		snake[i] = snake[i-2]; //ex(1er tour):si long 4, mySnake[7] = mySnake[5]
		snake[i+1] = snake[i-1]; //ex(1er tour):si long 4, mySnake[6] = mySnake[4]
		// printf("mySnake[%d] = %d\n", i, mySnake[i]);
		// printf("mySnake[%d] = %d\n", i+1, mySnake[i+1]);
		// printf("longueur = %d\n", longueur);
	}
	snake[0] = newPosx; //append head position
	snake[1] = newPosy;
	// printf("mySnake[0] = %d\n", mySnake[0]);
	// printf("mySnake[1] = %d\n", mySnake[1]);
}

int*** initArena(int X, int Y, int nbWalls, int* walls){

	int*** arena = (int***)malloc(X*sizeof(int**));
	for (int i = 0; i < X; i++){

		arena[i] = (int**)malloc(Y*sizeof(int*));
		for (int j = 0; j < Y; j++){

			arena[i][j] = (int*)malloc(5*sizeof(int));
		}
	}
	for (int i = 0; i < X; i++){

		for (int j = 0; j < Y; j++){

			//màj des impossibilités de mvts pour chaque case
			//incrémentation du POINTEUR arena[i][j] pour avoir les indices 1 à 4
			arena[i][j][0] = -1;
			arena[i][j][1] = 0;
			arena[i][j][2] = 0;
			arena[i][j][3] = 0;
			arena[i][j][4] = 0;

			dontTouchBorders(arena[i][j]+1, i, j, X, Y);
			dontTouchWalls(arena[i][j]+1, i, j, nbWalls, walls);

			//mise à 0 des distances pour là où le serpent est
			// for (int k = 0; k < 2*longueur; k += 2){
			// 	arena[mySnake[k]][mySnake[k+1]][0] = 0;
			// }

		}
	}
	return arena;
}

void updateArena(int*** updatedArena, int*** baseArena, int X, int Y, int myX, int myY, int* mySnake, int* hisSnake, int longueur, int forbiddenMoves[4], int distMaxExp){

	//projection du serpent pendant l'expansion
	// int* projMySnake = (int*)malloc(X*Y*sizeof(int));
	// for (int i = 0; i < X*Y; i++){
	// 	projMySnake[i] = mySnake[i];
	// }

	//"remise à zéro" de updatedArena (par rapport à l'arène de base)
	for (int i = 0; i < X; i++){
		for (int j = 0; j < Y; j++){
			for (int k = 0; k < 5; k++){
				updatedArena[i][j][k] = baseArena[i][j][k];
			}
		}
	}

	//prise en compte nouvelle position
	printf("forbiddenMoves[1] = %d\n",forbiddenMoves[1]);
	updatedArena[myX][myY][0] = 0;
	updatedArena[myX][myY][1] = forbiddenMoves[0];
	printf("forbidden east avant 1,5 = %d\n", updatedArena[1][5][2]);
	updatedArena[myX][myY][2] = forbiddenMoves[1];
	printf("forbidden east après 1,5 = %d\n", updatedArena[1][5][2]);
	updatedArena[myX][myY][3] = forbiddenMoves[2];
	updatedArena[myX][myY][4] = forbiddenMoves[3];

	//expansion
	for (int distance = 0; distance < distMaxExp; distance++){
		for (int i = 0; i < X; i++){
			for (int j = 0; j < Y; j++){
				if (updatedArena[i][j][0] == distance){
					//updateSnake(projMySnake,longueur,i,j);

					for (int direction = 0; direction < 4; direction++){
						//interdiction de revenir sur ses pas (pas ouf avec dontGoBack()...)
						if (distance != 0){
							// dontGoBack(updatedArena[i][j]+1,direction);
							switch(direction){
								case 0:
									if (j != 0){ //on sait jamais...
										if (updatedArena[i][j-1][0] == distance-1){
											updatedArena[i][j][1] = 1; //pas au nord
										}
									}
									break;
								case 1:
									if (i != X-1){
										if (updatedArena[i+1][j][0] == distance-1){
											updatedArena[i][j][2] = 1; //pas à l'est
										}
									}
									break;
								case 2:
									if (j != Y-1){
										if (updatedArena[i][j+1][0] == distance-1){
											updatedArena[i][j][3] = 1; //pas au sud
										}
									}
									break;
								case 3:
									if (i != 0){
										if (updatedArena[i-1][j][0] == distance-1){
											updatedArena[i][j][4] = 1; //pas à l'ouest
										}
									}
									break;
							}
						}

						dontTouchSnakes(updatedArena[i][j]+1,mySnake[0],mySnake[1],longueur,mySnake,hisSnake);
						//printf("INTERDIT HAUT X,Y = %d\n", updatedArena[mySnake[0]][mySnake[1]][1]);
						

						//marquage des cases
						if (updatedArena[i][j][direction+1] == 0){ //la voie est libre
							switch(direction){
								case 0: //north
									if (j != 0 && updatedArena[i][j-1][0] == -1){ //on sait jamais...
										updatedArena[i][j-1][0] = distance+1;
									}
									break;
								case 1: //east
									if (i != X-1 && updatedArena[i+1][j][0] == -1){
										updatedArena[i+1][j][0] = distance+1;
									}
									break;
								case 2: //south
									if (j != Y-1 && updatedArena[i][j+1][0] == -1){
										updatedArena[i][j+1][0] = distance+1;
									}
									break;
								case 3:	//west
									if (i != 0 && updatedArena[i-1][j][0] == -1){
										updatedArena[i-1][j][0] = distance+1;
									}
									break;
							}
						}
						// for (int m = 0; m < X*Y; m++){
						// 	projMySnake[m] = mySnake[m];
						// }
					}
					printf("after %d,%d je suis à la distance %d\n",i,j, updatedArena[i][j][0]);
				}
			}
		}	
	}

	//pour effacer les interdictions de revenir en arrière sur toutes les cases
	for (int i = 0; i < X; i++){
		for (int j = 0; j < Y; j++){
			for (int k = 1; k < 5; k++){
				updatedArena[i][j][k] = baseArena[i][j][k];
			}
		}
	}
	updatedArena[myX][myY][1] = forbiddenMoves[0];
	updatedArena[myX][myY][2] = forbiddenMoves[1];
	updatedArena[myX][myY][3] = forbiddenMoves[2];
	updatedArena[myX][myY][4] = forbiddenMoves[3];
	// free(projMySnake);
}

t_move chooseMyMove(int*** updatedArena, int X, int Y, int myX, int myY, int distMax){

	t_move myMove;
	int distance = distMax;
	int x,y;

	//phase de remontée
	//on cherche une case marquée de la distance max
	for (int i = 0; i < X; i++){
		for (int j = 0; j < Y; j++){
			if (updatedArena[i][j][0] == distance){
				x = i;
				y = j;
				printf("choose : x = %d, y = %d\n", x, y);
				goto END_OF_LOOP; //pour sortir de la boucle dès la 1re case trouvée
			}
		}
	}

	END_OF_LOOP :

	//on remonte jusqu'à la position courante
	while (distance > 0){

		//printf("choose : %d,%d je suis à la distance %d\n",i,j, updatedArena[i][j][0]);

		for (int direction = 0; direction < 4; direction++){
			//printf("updatedArena[x][y][%d+1] = %d\n", direction, updatedArena[x][y][direction+1]);
			if (updatedArena[x][y][direction+1] == 0){ //si la voie est libre
				//printf("direction de distance %d = %d\n", distance, direction);
				printf("x = %d, y = %d\n", x, y);
				switch(direction){
					case 0: //north
						if (y != 0){
							if (updatedArena[x][y-1][0] == distance-1){
								updatedArena[x][--y][0] = -1;
								printf("choose north : x = %d, y = %d\n", x, y);
							}
						}
						break;
					case 1: //east
						if (x != X-1){
							if (updatedArena[x+1][y][0] == distance-1){
								updatedArena[++x][y][0] = -1;
								printf("choose east : x = %d, y = %d\n", x, y);
							}
						}
						break;
					case 2: //south
						if (y != Y-1){
							if (updatedArena[x][y+1][0] == distance-1){
								updatedArena[x][++y][0] = -1;
								printf("choose south : x = %d, y = %d\n", x, y);
							}
						}
						break;
					case 3:	//west
						if (x != 0){
							if (updatedArena[x-1][y][0] == distance-1){
								updatedArena[--x][y][0] = -1;
								printf("choose west : x = %d, y = %d\n", x, y);
							}
						}
						break;
				}
			}
		}
		distance--;
	}

	if (myY != 0 && updatedArena[myX][myY-1][0] == -1){
		myMove = 0;
		return myMove;
	}
	if (myX != X-1 && updatedArena[myX+1][myY][0] == -1){
		myMove = 1;
		return myMove;
	}
	if (myY != Y-1 && updatedArena[myX][myY+1][0] == -1){
		myMove = 2;
		return myMove;
	}
	if (myX != 0 && updatedArena[myX-1][myY][0] == -1){
		myMove = 3;
		return myMove;
	}

	return -1;
}

void printArenaInfo(int*** arena, int X, int Y){

	for (int i = 0; i < 7; i++){
		for (int j = 0; j < 9; j++){
			printf("%d,%d je suis à la distance %d\n",i,j, arena[i][j][0]);
			printf("arena[%d][%d][north] = %d\n", i,j, arena[i][j][1]);
			printf("arena[%d][%d][east ] = %d\n", i,j, arena[i][j][2]);
			printf("arena[%d][%d][south] = %d\n", i,j, arena[i][j][3]);
			printf("arena[%d][%d][west ] = %d\n", i,j, arena[i][j][4]);
		}
	}

}

void printSnakeInfo(int* snake, int X, int Y){
	for (int i = 0; i < X*Y; i+=2){
		printf("snakeX[%d] = %d, snakeY[%d] = %d\n", i/2, snake[i], i/2, snake[i+1]);
	}
}








			// for (int k = 0; k < 4*nbWalls; k+=4){
			// 	if (walls[k] == i && walls[k+1] == j){
			// 		if(walls[k+2] == i-1){ //mur à gauche
			// 			arena[i][j][4] = 1;
			// 		}
			// 		if(walls[k+2] == i+1){ //mur à droite
			// 			arena[i][j][2] = 1;
			// 		}
			// 		if(walls[k+3] == j-1){ //mur en haut
			// 			arena[i][j][1] = 1;
			// 		}
			// 		if(walls[k+3] == j+1){ //mur en bas
			// 			arena[i][j][3] = 1;
			// 		}
			// 	}
			// 	if (walls[i+2] == i && walls[i+3] == j){
			// 		if(walls[i] == i-1){ //mur à gauche
			// 			arena[i][j][4] = 1;
			// 		}
			// 		if(walls[i] == i+1){ //mur à droite
			// 			arena[i][j][2] = 1;
			// 		}
			// 		if(walls[i+1] == j-1){ //mur en haut
			// 			arena[i][j][1] = 1;
			// 		}
			// 		if(walls[i+1] == j+1){ //mur en bas
			// 			arena[i][j][3] = 1;
			// 		}
			// 	}
			// }
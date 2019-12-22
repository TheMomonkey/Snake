#include <stdio.h>
#include <stdlib.h>
#include "ret_type.h"
#include "clientAPI.h"
#include "snakeAPI.h"
#include "arena.h"

int main(){

	char gameName[50];
	int X, Y, nbWalls, hePlays;
	t_return_code returnCode = NORMAL_MOVE;
	t_move myMove = -1;
	t_move hisMove;
	int nbTours = 0;
	int longueur = 1;
	int forbiddenMoves[4] = {0,0,0,0}; //{N,E,S,W} 1 si interdit
	int distanceMax = 15;

	//connexion au serveur
	//en externe:
	connectToServer("polydev.cia-polytech-sorbonne.fr", 8080, "sadouki");
	//en interne: (avec ssh)
	//connectToServer("localhost", 1234, "asadouki");
	
	//récupération des infos
	waitForSnakeGame("SUPER_PLAYER difficulty=2 timeout=1000 start=0 seed=2", gameName, &X, &Y, &nbWalls);
	int* walls = (int*)malloc(4*nbWalls*sizeof(int));
	hePlays = getSnakeArena(walls);

	//get positions
	int myX = hePlays ? X-3 : 2;
	int myY = Y/2;
	int hisX = hePlays ? 2 : X-3;
	int hisY = Y/2;

	//init snakes
	int* mySnake = (int*)malloc(X*Y*sizeof(int)); //tab coord snake, au max occupe la moitié de l'arène (/2 *2 pcq coord)
	int* hisSnake = (int*)malloc(X*Y*sizeof(int));
	for (int i = 0; i < X*Y; i++){
		mySnake[i] = -1;
		hisSnake[i] = -1;
	}
	updateSnake(mySnake,longueur,myX,myY);
	updateSnake(hisSnake,longueur,myX,myY);

	//modélisation arène (matrice X*Y*5) arena[i][j] = {dist,N,E,S,W}
	//dist = distance de toi (0 si dessus), le reste pareil que pour forbiddenMoves
	//-1 non visitée, 0 ya qqun dessus (toi ou lui), val>0 à val cases de ta vieille tete
	int*** baseArena = initArena(X,Y,nbWalls,walls);
	int*** updatedArena = (int***)malloc(X*sizeof(int**));
	for (int i = 0; i < X; i++){
		updatedArena[i] = (int**)malloc(Y*sizeof(int*));
		for (int j = 0; j < Y; j++){
			updatedArena[i][j] = (int*)malloc(5*sizeof(int));
		}
	}
	for (int i = 0; i < X; i++){
		for (int j = 0; j < Y; j++){
			for (int k = 0; k < 5; k++){
				updatedArena[i][j][k] = baseArena[i][j][k];
			}
		}
	}
	//affichage infos arène
	printf("X:%d\tY:%d\n", X, Y);

	while(returnCode == NORMAL_MOVE){
		printArena();
		if(hePlays){
			returnCode = getMove(&hisMove);
			//printf("hisMove = %d\n", hisMove);
			//maj de sa position
			updateSnake(hisSnake,longueur,hisX,hisY);
			switch(hisMove){
				case 0:
					hisY -= 1;
					break;
				case 1:
					hisX += 1;
					break;
				case 2:
					hisY += 1;
					break;
				case 3:
					hisX -= 1;
			}
		}
		else{
			//idée 2: va tout droit tant qu'il peut
			// Faire gaffe aux murs, aux bords, à pas revenir sur ses pas, à sa queue, à l'autre, aux cul-de-sac
			// Expansion
			// Si le temps (lol) désavantager l'adversaire : algos min-max/alpha-beta

			//INFOS UTILES:
			// player 0 starts at ( 2  , H/2)
			// player 1 starts at (L-3 , H/2)
			// NORTH = 0,
			// EAST = 1,
			// SOUTH = 2,
			// WEST = 3
			// NORMAL_MOVE = 0,
			// WINNING_MOVE = 1 (he wins)
			// LOOSING_MOVE = -1 (I win)

			//ne pas revenir sur ses pas:
			dontGoBack(forbiddenMoves,myMove);

			//faire gaffe à lui et à moi:
			dontTouchSnakes(forbiddenMoves,myX,myY,longueur,mySnake,hisSnake);

			//faire gaffe aux bords:
			dontTouchBorders(forbiddenMoves,myX,myY,X,Y);

			//faire gaffe aux murs:
			dontTouchWalls(forbiddenMoves,myX,myY,nbWalls,walls);


			updateArena(updatedArena,baseArena,X,Y,myX,myY,mySnake,hisSnake,longueur,forbiddenMoves,distanceMax);
			//printf("updatedArena[myX=%d][myY=%d][0] = %d\n", myX, myY, updatedArena[myX][myY][0]);
			//printArenaInfo(updatedArena,X,Y);
			//printSnakeInfo(mySnake,X,Y);

			myMove = chooseMyMove(updatedArena,X,Y,myX,myY,distanceMax);
			//printf("updatedArena[myX+1][myY][0] = %d\n", updatedArena[myX+1][myY][0]);

			//mise à jour des variables utilisées
			
			//position dans le code: après choix myMove avant màj myX/myY (oui oui avant)
			//parkour depuis la fin jusqu'à la 2e valeur (ou 2e couple de valeurs)
			updateSnake(mySnake,longueur,myX,myY);

			switch(myMove){
				case 0:
					myY -= 1;
					break;
				case 1:
					myX += 1;
					break;
				case 2:
					myY += 1;
					break;
				case 3:
					myX -= 1;
					break;
			}

			nbTours++;
			if (nbTours%10 == 0){
				longueur++;
			}

			returnCode = sendMove(myMove);
			//choix de myMove (à modifier pcq privilégie le nord)
			// for (int i = 3; i >= 0; i--)
			// {
			// 	printf("forbiddenMoves[%d] = %d\n", i, forbiddenMoves[i]);
			// 	if (!forbiddenMoves[i]){
			// 		myMove = i;
			// 	}
			// 	forbiddenMoves[i] = 0;
			// }
			//printf("Final myX = %d, myY = %d\n", myX, myY);
			//printf("Final myMove = %d\n", myMove);
		}
		hePlays = !hePlays;
		
	}



	//affichage final
	if (returnCode == -1){
		printf(R"EOF(


 /$$     /$$ /$$$$$$  /$$   /$$       /$$      /$$  /$$$$$$  /$$   /$$       /$$     /$$ /$$$$$$  /$$     /$$       /$$
|  $$   /$$//$$__  $$| $$  | $$      | $$  /$ | $$ /$$__  $$| $$$ | $$      |  $$   /$$//$$__  $$|  $$   /$$/      | $$
 \  $$ /$$/| $$  \ $$| $$  | $$      | $$ /$$$| $$| $$  \ $$| $$$$| $$       \  $$ /$$/| $$  \ $$ \  $$ /$$/       | $$
  \  $$$$/ | $$  | $$| $$  | $$      | $$/$$ $$ $$| $$  | $$| $$ $$ $$        \  $$$$/ | $$$$$$$$  \  $$$$/        | $$
   \  $$/  | $$  | $$| $$  | $$      | $$$$_  $$$$| $$  | $$| $$  $$$$         \  $$/  | $$__  $$   \  $$/         |__/
    | $$   | $$  | $$| $$  | $$      | $$$/ \  $$$| $$  | $$| $$\  $$$          | $$   | $$  | $$    | $$              
    | $$   |  $$$$$$/|  $$$$$$/      | $$/   \  $$|  $$$$$$/| $$ \  $$          | $$   | $$  | $$    | $$           /$$
    |__/    \______/  \______/       |__/     \__/ \______/ |__/  \__/          |__/   |__/  |__/    |__/          |__/
                                                                                                                     
                                                                                                                     
		)EOF");
		printf("\n");
	}
	if (returnCode == 1){
		printf(R"EOF(


 /$$        /$$$$$$   /$$$$$$  /$$$$$$$$ /$$$$$$$ 
| $$       /$$__  $$ /$$__  $$| $$_____/| $$__  $$
| $$      | $$  \ $$| $$  \__/| $$      | $$  \ $$
| $$      | $$  | $$|  $$$$$$ | $$$$$   | $$$$$$$/
| $$      | $$  | $$ \____  $$| $$__/   | $$__  $$
| $$      | $$  | $$ /$$  \ $$| $$      | $$  \ $$
| $$$$$$$$|  $$$$$$/|  $$$$$$/| $$$$$$$$| $$  | $$
|________/ \______/  \______/ |________/|__/  |__/
                                                  
        )EOF");
        printf("\n");
	}


	closeConnection();
	free(walls);
	free(mySnake);
	free(hisSnake);

	for (int i = 0; i < X; i++){
		for (int j = 0; j < Y; j++){
			free(baseArena[i][j]);
		}
		free(baseArena[i]);
	}
	free(baseArena);

	for (int i = 0; i < X; i++){
		for (int j = 0; j < Y; j++){
			free(updatedArena[i][j]);
		}
		free(updatedArena[i]);
	}
	free(updatedArena);
}

#include <stdio.h>
#include <stdlib.h>
#include "ret_type.h"
#include "clientAPI.h"
#include "snakeAPI.h"

int main(){

	char gameName[50];
	int X, Y, nbWalls, hePlays;
	t_return_code returnCode = NORMAL_MOVE;
	t_move myMove = -1;
	t_move hisMove;
	int nbTours = 0;
	int longueur = 1;
	int forbiddenMoves[4] = {0,0,0,0}; //{N,E,S,W} 1 si interdit

	//connexion au serveur
	//en externe:
	connectToServer("polydev.cia-polytech-sorbonne.fr", 8080, "sadouki");
	//en interne: (avec ssh)
	//connectToServer("localhost", 1234, "asadouki");
	
	//récupération des infos
	waitForSnakeGame("RANDOM_PLAYER difficulty=2 timeout=1000 start=0 seed=2", gameName, &X, &Y, &nbWalls);
	int* walls = (int*)malloc(4*nbWalls*sizeof(int));
	hePlays = getSnakeArena(walls);

	//get positions
	int myX = hePlays ? X-3 : 2;
	int myY = Y/2;
	int hisX = hePlays ? 2 : X-3;
	int hisY = Y/2;

	int mySnake[X*Y]; //tab coord snake, au max occupe la moitié de l'arène (/2 *2 pcq coord)
	int hisSnake[X*Y];
	int myMoves[X*Y/2];
	int hisMoves[X*Y/2];

	for (int i = 0; i < X*Y/2; i++){
		myMoves[i] = -1;
		hisMoves[i] = -1;
	}

	//modélisation arène (matrice X*Y)
	//-1 non visitée, 0 ya qqun dessus (toi ou lui), val>0 à val cases de ta vieille tete
	int** arena = (int**)malloc(X*sizeof(int));
	for (int i = 0; i < X; i++)
	{
		arena[i] = (int*)malloc(Y*sizeof(int));
	}

	//affichage infos arène
	printf("X:%d\tY:%d\n", X, Y);

	while(returnCode == NORMAL_MOVE){
		printArena();
		if(hePlays){
			returnCode = getMove(&hisMove);
			for (int i = longueur-1; i > 0 ; i--){
				hisMoves[i] = hisMoves[i-1];
				hisSnake[2*i+1] = hisSnake[2*i-1];
				hisSnake[2*i] = hisSnake[2*i-2];
			}
			hisSnake[0] = hisX; //append head position
			hisSnake[1] = hisY;
			hisMoves[0] = hisMove; //append my dernier move
			//maj de sa position
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
			switch(myMove){
				case 0:
					forbiddenMoves[2] = 1;
					printf("here pas 1\n");
					break;
				case 1:
					forbiddenMoves[3]= 1;
					printf("here pas 2\n");
					break;
				case 2:
					forbiddenMoves[0]= 1;
					printf("here pas 3\n");
					break;
				case 3:
					forbiddenMoves[1]= 1;
					printf("here pas 4\n");
					break;
				default:
					break;
			}

			//faire gaffe à lui et à moi:
			for (int i = 0; i < 2*longueur; i += 2){
				if ((myX-1 == mySnake[i] && myY == mySnake[i+1]) || (myX-1 == hisSnake[i] && myY == hisSnake[i+1])){
					forbiddenMoves[3] = 1;
					printf("here toi 1\n");
				}
				if ((myX+1 == mySnake[i] && myY == mySnake[i+1]) || (myX+1 == hisSnake[i] && myY == hisSnake[i+1])){
					forbiddenMoves[1] = 1;
					printf("mySnake[%d] = %d\n", i, mySnake[i]);
					printf("here toi 2\n");
				}
				if ((myY-1 == mySnake[i+1] && myX == mySnake[i]) || (myY-1 == hisSnake[i+1] && myX == hisSnake[i])){
					forbiddenMoves[0] = 1;
					printf("here toi 3\n");
				}
				if ((myY+1 == mySnake[i+1] && myX == mySnake[i]) || (myY+1 == hisSnake[i+1] && myX == hisSnake[i])){
					forbiddenMoves[2] = 1;
					printf("here toi 4\n");
				}
			}

			//faire gaffe aux bords:
			if (myX == 0){
				forbiddenMoves[3] = 1;
				printf("here bords 1\n");
			}
			if (myX == X-1){
				forbiddenMoves[1] = 1;
				printf("here bords 2\n");
			}
			if (myY == 0){
				forbiddenMoves[0] = 1;
				printf("here bords 3\n");
			}
			if (myY == Y-1){
				forbiddenMoves[2] = 1;
				printf("here bords 4\n");
			}

			//faire gaffe aux murs:
			for (int i = 0; i < 4*nbWalls; i+=4){
				// walls[i]   = x1
				// walls[i+1] = y1
				// walls[i+2] = x2
				// walls[i+3] = y2
				if (walls[i] == myX && walls[i+1] == myY){
					if(walls[i+2] == myX-1){ //mur à gauche
						forbiddenMoves[3] = 1;
						printf("here mur 1\n");
					}
					if(walls[i+2] == myX+1){ //mur à droite
						forbiddenMoves[1] = 1;
						printf("here mur 2\n");
					}
					if(walls[i+3] == myY-1){ //mur en haut
						forbiddenMoves[0] = 1;
						printf("here mur 3\n");
					}
					if(walls[i+3] == myY+1){ //mur en bas
						forbiddenMoves[2] = 1;
						printf("here mur 4\n");
					}
				}
				if (walls[i+2] == myX && walls[i+3] == myY){
					if(walls[i] == myX-1){ //mur à gauche
						forbiddenMoves[3] = 1;
						printf("here mur 5\n");
					}
					if(walls[i] == myX+1){ //mur à droite
						forbiddenMoves[1] = 1;
						printf("here mur 6\n");
					}
					if(walls[i+1] == myY-1){ //mur en haut
						forbiddenMoves[0] = 1;
						printf("here mur 7\n");
					}
					if(walls[i+1] == myY+1){ //mur en bas
						forbiddenMoves[2] = 1;
						printf("here mur 8\n");
					}
				}
			}

			//faire gaffe aux cul-de-sac:
			// for (int i = 0; i < X; i++)
			// {
			// 	for (int j = 0; j < Y; j++)
			// 	{
			// 		for (int k = 0; k < 4*nbWalls; k++)
			// 		{
			// 			if (walls[i] == i && walls[i+1] == j){
			// 				if(walls[i+2] == i-1){ //mur à gauche
			// 					forbiddenMoves[3] = 1;
			// 					printf("here mur 1\n");
			// 				}
			// 				if(walls[i+2] == i+1){ //mur à droite
			// 					forbiddenMoves[1] = 1;
			// 					printf("here mur 2\n");
			// 				}
			// 				if(walls[i+3] == j-1){ //mur en haut
			// 					forbiddenMoves[0] = 1;
			// 					printf("here mur 3\n");
			// 				}
			// 				if(walls[i+3] == j+1){ //mur en bas
			// 					forbiddenMoves[2] = 1;
			// 					printf("here mur 4\n");
			// 				}
			// 			}
			// 			if (walls[i+2] == i && walls[i+3] == j){
			// 				if(walls[i] == i-1){ //mur à gauche
			// 					forbiddenMoves[3] = 1;
			// 					printf("here mur 5\n");
			// 				}
			// 				if(walls[i] == i+1){ //mur à droite
			// 					forbiddenMoves[1] = 1;
			// 					printf("here mur 6\n");
			// 				}
			// 				if(walls[i+1] == j-1){ //mur en haut
			// 					forbiddenMoves[0] = 1;
			// 					printf("here mur 7\n");
			// 				}
			// 				if(walls[i+1] == j+1){ //mur en bas
			// 					forbiddenMoves[2] = 1;
			// 					printf("here mur 8\n");
			// 				}
			// 			}
			// 		}
			// 	}
			// }

			//choix de myMove (à modifier pcq privilégie le nord)
			for (int i = 3; i >= 0; i--)
			{
				printf("forbiddenMoves[%d] = %d\n", i, forbiddenMoves[i]);
				if (!forbiddenMoves[i]){
					myMove = i;
				}
				forbiddenMoves[i] = 0;
			}
			printf("Final move = %d\n", myMove);
			
			//position dans le code: après choix myMove avant màj myX/myY
			//parkour depuis la fin jusqu'à la 2e valeur (ou 2e couple de valeurs)
			for (int i = 2*longueur-2; i > 1 ; i -= 2){
				myMoves[i/2] = myMoves[i/2-1];
				mySnake[i] = mySnake[i-2]; //ex(1er tour):si long 4, mySnake[7] = mySnake[5]
				mySnake[i+1] = mySnake[i-1]; //ex(1er tour):si long 4, mySnake[6] = mySnake[4]
				printf("mySnake[%d] = %d\n", i, mySnake[i]);
				printf("mySnake[%d] = %d\n", i+1, mySnake[i+1]);
				printf("longueur = %d\n", longueur);
			}
			mySnake[0] = myX; //append head position
			mySnake[1] = myY;
			printf("mySnake[0] = %d\n", mySnake[0]);
			printf("mySnake[1] = %d\n", mySnake[1]);
			myMoves[0] = myMove; //append my dernier move

			//mise à jour des variables utilisées
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

			arena[myX][myY] = 0;
			for (int i = 0; i < 2*longueur; i += 2){
				arena[mySnake[i]][mySnake[i+1]] = 0;
			}

			nbTours++;
			if (nbTours%10 == 0){
				longueur++;
			}

			returnCode = sendMove(myMove);
		}
		hePlays = !hePlays;
		
	}



	//affichage final
	if (returnCode == 1){
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
	if (returnCode == -1){
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
}

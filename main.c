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
	int* myMoves, hisMoves;
	int nbTours = 0;
	int longueur = 0;
	int forbiddenMoves[4] = {0,0,0,0}; //{N,S,E,W} 1 si interdit

	//connexion au serveur
	//en externe:
	//connectToServer("polydev.cia-polytech-sorbonne.fr", 8080, "asadouki");
	//en interne:
	//connectToServer("localhost", 1234, "asadouki");
	
	//récupération des infos
	waitForSnakeGame("RANDOM_PLAYER difficulty=2 timeout=1000 start=0 seed=1", gameName, &X, &Y, &nbWalls);
	int* walls = (int*)malloc(4*nbWalls*sizeof(int));
	hePlays = getSnakeArena(walls);

	int myX = hePlays ? X-3 : 2;
	int myY = Y/2;
	int hisX = hePlays ? 2 : X-3;
	int hisY = Y/2;

	int** arena = (int**)malloc(X*sizeof(int));
	for (int i = 0; i < X; i++)
	{
		arena[i] = (int*)malloc(Y*sizeof(int));
	} //-1 non visitée, 0 ya qqun dessus (toi ou lui), val>0 à val cases de ta vieille tete

	//affichage infos arène
	printf("X:%d\tY:%d\n", X, Y);

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
	
	while(returnCode == NORMAL_MOVE){
		printArena();
		if(hePlays){
			returnCode = getMove(&hisMove);
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
			//idée 1: tourne en rond (myMove++) tant qu'il n'y a pas de mur qui le bloque
			//sinon le contourne, le tout en ayant conscience de sa taille

			// if (myMove%3 == 0){ //pour tourner en rond
			// 	myMove = 0;
			// }
			// if (longueur != 0 && nbTours%longueur == 0){ //pour ne pas se gener avec sa queue
			// 	myMove++;
			// }


			//idée 2: va tout droit tant qu'il peut
			// To do : merge the switches my move my man
			// Faire gaffe aux murs, aux bords, à pas revenir sur ses pas, à sa queue, à l'autre, aux cul-de-sac
			// Expansion
			// Si le temps (lol) désavantager l'adversaire : algos min-max/alpha-beta

			myMoves = (int*)malloc(longueur*sizeof(int));
			for (int i = 0; i < longueur-1; i++){ //pour pas dépasser
				myMoves[i+1] = myMoves[i];
			}
			myMoves[0] = myMove; //append my dernier move

			//avoir conscience de soi:
			arena[myX][myY] = 0;
			for (int i = 0; i < longueur; i++)
			{
				
			}
			//HERE rajouter la longueur

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
			//faire gaffe aux cul-de-sac:
			for (int i = 0; i < X; i++)
			{
				for (int j = 0; j < Y; j++)
				{
					for (int k = 0; k < 4*nbWalls; k++)
					{
						if (walls[i] == i && walls[i+1] == j){
							if(walls[i+2] == i-1){ //mur à gauche
								forbiddenMoves[3] = 1;
								printf("here mur 1\n");
							}
							if(walls[i+2] == i+1){ //mur à droite
								forbiddenMoves[1] = 1;
								printf("here mur 2\n");
							}
							if(walls[i+3] == j-1){ //mur en haut
								forbiddenMoves[0] = 1;
								printf("here mur 3\n");
							}
							if(walls[i+3] == j+1){ //mur en bas
								forbiddenMoves[2] = 1;
								printf("here mur 4\n");
							}
						}
						if (walls[i+2] == i && walls[i+3] == j){
							if(walls[i] == i-1){ //mur à gauche
								forbiddenMoves[3] = 1;
								printf("here mur 5\n");
							}
							if(walls[i] == i+1){ //mur à droite
								forbiddenMoves[1] = 1;
								printf("here mur 6\n");
							}
							if(walls[i+1] == j-1){ //mur en haut
								forbiddenMoves[0] = 1;
								printf("here mur 7\n");
							}
							if(walls[i+1] == j+1){ //mur en bas
								forbiddenMoves[2] = 1;
								printf("here mur 8\n");
							}
						}
					}
				}
			}
			// switch(myMove){
			// 	case 0:
			// 		while(projX++ )
			// 		break;
			// 	case 1:
			// 		forbiddenMoves[3]= 1;
			// 		break;
			// 	case 2:
			// 		forbiddenMoves[0]= 1;
			// 		break;
			// 	case 3:
			// 		forbiddenMoves[1]= 1;
			// 		break; 
			// }

			//faire gaffe à l'autre:

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

			//mise à jour des variables utilisées
			if (nbTours%10 == 0){
				longueur++;
			}
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
			if (nbTours%longueur == 0) //ne libère pas le tableau tout le temps pour garder un tableau de taille "longueur"
			{
				free(myMoves);
			}
			nbTours++;
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

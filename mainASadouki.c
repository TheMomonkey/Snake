/*
 * mainASadouki.c
 * (programme principal)
 * 
 * SADOUKI Aya
 * 
 * Exécution : ./mainASadouki [adresse serveur] [port] [paramètres jeu]
 * exemple: ./mainASadouki polydev.cia-polytech-sorbonne.fr 8080 "SUPER_PLAYER difficulty=2"
 *
 * Avancement : choisit la direction lui permettant d'aller le plus loin grâce à une stratégie d'expansion/remontée
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include "ret_type.h"
#include "clientAPI.h"
#include "snakeAPI.h"
#include "arena.h"

int main(int argc, char* argv[]){

	int port = strtol(argv[2], NULL, 10);

	//connexion au serveur
	connectToServer(argv[1], port, "asadouki");

	while(1){

		char gameName[50];
		unsigned int nbWalls;
		int X, Y, hePlays, myX, myY, hisX, hisY;
		t_return_code returnCode = NORMAL_MOVE;
		t_move myMove = -1;
		t_move hisMove;
		int nbTours = 0;
		int longueur = 1;
		int forbiddenMoves[4] = {0,0,0,0}; //{N,E,S,W} 1 si interdit
		int distanceMax = 30; //distance max pour phase d'expansion (jusqu'à combien de coups regarder)
		
		//récupération des infos de l'arène
		waitForSnakeGame(argv[3], gameName, &X, &Y, &nbWalls);
		int* walls = (int*)malloc(4*nbWalls*sizeof(int));
		hePlays = getSnakeArena(walls);

		//positions initiales
		myX = hePlays ? X-3 : 2;
		myY = Y/2;
		hisX = hePlays ? 2 : X-3;
		hisY = Y/2;

		//initialisation des snakes
		int* mySnake = (int*)malloc(X*Y*sizeof(int)); //tab coord snake, au max occupe la moitié de l'arène (/2 *2 pcq coord)
		int* hisSnake = (int*)malloc(X*Y*sizeof(int));
		for (int i = 0; i < X*Y; i++){
			mySnake[i] = -1;
			hisSnake[i] = -1;
		}
		updateSnake(mySnake,longueur,myX,myY);
		updateSnake(hisSnake,longueur,myX,myY);

		//modélisation arène (matrice X*Y*5) arena[i][j] = {dist,N,E,S,W}
		//dist = distance de moi (0 si dessus), le reste pareil que pour forbiddenMoves
		//-1 non visitée, 0 ya qqun dessus, val>0 à val cases de ma tete, -2 si appartient au chemin retenu
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

		while(returnCode == NORMAL_MOVE){
			printArena();
			if(hePlays){
				returnCode = getMove(&hisMove);
				//printf("hisMove = %d\n", hisMove);
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
				updateSnake(hisSnake,longueur,hisX,hisY);
			}
			else{

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

				//faire attention à lui et à moi:
				dontTouchSnakes(forbiddenMoves,myX,myY,longueur,mySnake,hisSnake);

				//faire attention aux bords:
				dontTouchBorders(forbiddenMoves,myX,myY,X,Y);

				//faire attention aux murs:
				dontTouchWalls(forbiddenMoves,myX,myY,nbWalls,walls);


				updateArena(updatedArena,baseArena,X,Y,myX,myY,mySnake,hisSnake,longueur,forbiddenMoves,distanceMax);
				//printArenaInfo(updatedArena,X,Y);
				//printSnakeInfo(mySnake,X,Y);

				//choix final du mouvement à faire
				myMove = chooseMyMove(updatedArena,X,Y,myX,myY,distanceMax);

				//mise à jour des variables utilisées
				
				//parcours le tableau depuis la fin jusqu'à la 2e valeur (ou 2e couple de valeurs)
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



		//affichage final stylé...
		/*
		if (returnCode == -1){
			sendComment("ಥ_ಥ");
		}
		if (returnCode == 1){
			sendComment("ಠᴗಠ hehehe...");
		}
		*/

		//free divers
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

	closeConnection();

}

/*
	 /$$     /$$ /$$$$$$  /$$   /$$       /$$      /$$  /$$$$$$  /$$   /$$       /$$     /$$ /$$$$$$  /$$     /$$       /$$
	|  $$   /$$//$$__  $$| $$  | $$      | $$  /$ | $$ /$$__  $$| $$$ | $$      |  $$   /$$//$$__  $$|  $$   /$$/      | $$
	 \  $$ /$$/| $$  \ $$| $$  | $$      | $$ /$$$| $$| $$  \ $$| $$$$| $$       \  $$ /$$/| $$  \ $$ \  $$ /$$/       | $$
	  \  $$$$/ | $$  | $$| $$  | $$      | $$/$$ $$ $$| $$  | $$| $$ $$ $$        \  $$$$/ | $$$$$$$$  \  $$$$/        | $$
	   \  $$/  | $$  | $$| $$  | $$      | $$$$_  $$$$| $$  | $$| $$  $$$$         \  $$/  | $$__  $$   \  $$/         |__/
	    | $$   | $$  | $$| $$  | $$      | $$$/ \  $$$| $$  | $$| $$\  $$$          | $$   | $$  | $$    | $$              
	    | $$   |  $$$$$$/|  $$$$$$/      | $$/   \  $$|  $$$$$$/| $$ \  $$          | $$   | $$  | $$    | $$           /$$
	    |__/    \______/  \______/       |__/     \__/ \______/ |__/  \__/          |__/   |__/  |__/    |__/          |__/

	 /$$        /$$$$$$   /$$$$$$  /$$$$$$$$ /$$$$$$$
	| $$       /$$__  $$ /$$__  $$| $$_____/| $$__  $$
	| $$      | $$  \ $$| $$  \__/| $$      | $$  \ $$
	| $$      | $$  | $$|  $$$$$$ | $$$$$   | $$$$$$$/
	| $$      | $$  | $$ \____  $$| $$__/   | $$__  $$
	| $$      | $$  | $$ /$$  \ $$| $$      | $$  \ $$
	| $$$$$$$$|  $$$$$$/|  $$$$$$/| $$$$$$$$| $$  | $$
	|________/ \______/  \______/ |________/|__/  |__/
*/
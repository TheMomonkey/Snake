#include <stdio.h>
#include <stdlib.h>
#include "ret_type.h"
#include "clientAPI.h"
#include "snakeAPI.h"

int main(){

	char gameName[50];
	int X, Y, nbWalls, hePlays;
	t_return_code returnCode = NORMAL_MOVE; //-1 he wins, 1 i win
	t_move myMove = 0;
	t_move hisMove;
	int lecture = 0; //lecture clavier
	int nbTours = 0;
	int longueur = 0;

	//connexion au serveur
	//connectToServer("polydev.cia-polytech-sorbonne.fr", 8080, "asadouki");
	connectToServer("localhost", 1234, "asadouki");

	//récupération des infos
	waitForSnakeGame("RANDOM_PLAYER difficulty=2 timeout=100 start=0 seed=1", gameName, &X, &Y, &nbWalls);
	int* walls = (int*)malloc(4*nbWalls*sizeof(int));
	hePlays = getSnakeArena(walls);

	//INFOS UTILES
	//joueur 0 commence à ( 2  , H/2)
	//joueur 1 commence à (L-3 , H/2)
	// NORTH = 0,
	// EAST = 1,
	// SOUTH = 2,
	// WEST = 3
	
	//affichage arène + infos arène
	printf("X:%d\tY:%d\n", X, Y);
	int j = 0;
	for (int i = 0; i < 4*nbWalls; i=i+4)
	{
		printf("coordonnées mur %d : (%d ; %d) to (%d ; %d)\n",j, walls[i], walls[i+1], walls[i+2], walls[i+3]);
		j++;
	}
	
	while(returnCode == NORMAL_MOVE){ //NORMAL_MOVE = 0
		printArena();
		if(hePlays){
			returnCode = getMove(&hisMove);
		}
		else{

			printf("eh jou wesh ! (nbTours = %d, longueur = %d) : ",nbTours, longueur);
			scanf("%d", &lecture);
			switch(lecture){
				case 8:
					myMove = NORTH;
					break;
				case 6:
					myMove = EAST;
					break;
				case 2:
					myMove = SOUTH;
					break;
				case 4:
					myMove = WEST;
					break;
			}
			if (nbTours%10 == 0){
				longueur++;
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

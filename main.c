#include <stdio.h>
#include <stdlib.h>
#include "ret_type.h"
#include "clientAPI.h"
#include "snakeAPI.h"

int main(){
	char* gameName = "";
	int X, Y, nbWalls, hePlays;
	t_return_code ret = NORMAL_MOVE; //-1 he wins, 1 i win
	t_move myMove, hisMove;
	int lecture = 0;
	
	connectToServer("polydev.cia-polytech-sorbonne.fr", 8080, "momonkey");
	
	waitForSnakeGame("RANDOM_PLAYER difficulty=2 timeout=10 start=0 seed=12", gameName, &X, &Y, &nbWalls);
	int* walls = (int*)malloc(4*nbWalls*sizeof(int));
	hePlays = getSnakeArena(walls);
	printArena();
	
	while(ret == NORMAL_MOVE){ //NORMAL_MOVE = 0
		if(hePlays){
			ret = getMove(&hisMove);
		}
		else{
			printf("eh jou wesh");
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
			ret = sendMove(myMove);
		}
		hePlays = !hePlays;
		printArena();
	}
	
	closeConnection();
	free(walls);
}

#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mouse.h>
#include <sstream>
#include <string>
#include <fstream> //Open & load files
using namespace std;

//Our total cells, live or dead
int CELLS = 0;
int BLOCKW = 8;
int BLOCKH = 8;
int HOWMANYBLOCKSW = 83;
int HOWMANYBLOCKSH = 80;

#include "init.h"

///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// CELLS DATA ////////////////////////////////////
//-------------------------------------------------------------------------------//
//These are structs, similiar to Classes
struct Cell {
public:
	int x, y, w, h;
	int neiN;
	int neiS;
	int neiW;
	int neiE;
	int neiNW;
	int neiNE;
	int neiSW;
	int neiSE;
	bool alive;
	bool mouseOnCell;
	bool onScreen;
	int total;
	string state;
};

//Initialize the cell
void initCell(Cell cell[]) {
	for (int i = 0; i < 6640; i++) {
		cell[i].alive = false;
		cell[i].mouseOnCell = false;
		cell[i].onScreen = false;
		cell[i].state = "dead";
	}
}

//Revive a cell (used if you left click)
void reviveCell(Cell cell[]) {
	for (int i = 0; i < 6640; i++) {
		if (cell[i].alive) {
			if (cell[i].state == "dead") {
				if (cell[i].mouseOnCell) {
					cell[i].state = "live";
				}
			}
		}
	}
}

//Kill a cell (used if you right click)
void killCell(Cell cell[]) {
	for (int i = 0; i < 6640; i++) {
		if (cell[i].alive) {
			if (cell[i].state == "live") {
				if (cell[i].mouseOnCell) {
					cell[i].state = "dead";
				}
			}
		}
	}
}

//This spawns a cell, dead or alive
void spawnCell(Cell cell[], int x, int y, int w, int h, string state, bool alive) {
	for (int i = 0; i < 6640; i++) {
		if (!cell[i].alive) {
			cell[i].x = x;
			cell[i].y = y;
			cell[i].w = w;
			cell[i].h = h;
			cell[i].state = state;
			cell[i].alive = alive;
			CELLS++;
			break;
		}
	}
}

//Spawn multiple cells (dead)
void spawnDeadCells(Cell cell[]) {
	for (int w = 0; w < HOWMANYBLOCKSW; w++) {
		for (int h = 0; h < HOWMANYBLOCKSH; h++) {
			spawnCell(cell, 0 + w * BLOCKW, 0 + h * BLOCKH , BLOCKW, BLOCKH, "dead", true);
		}
	}
}

//Check Cell surroundings
void updateCell(Cell cell[], int mx, int my) {
	for (int i = 0; i < 6640; i++) {
		if (cell[i].alive) {
			//Always set the number of cells around a cell to 0, unless stated otherwise
			cell[i].neiN = 0;
			cell[i].neiS = 0;
			cell[i].neiW = 0;
			cell[i].neiE = 0;
			cell[i].neiNW = 0;
			cell[i].neiNE = 0;
			cell[i].neiSW = 0;
			cell[i].neiSE = 0;
			cell[i].total = 0;

			//Check number of cells around a LIVE cell
			if (cell[i].state == "live") {
				//Cell has a cell North of it
				if (i > 0){ //Only check what is North of cell, if there is a cell there
					if (cell[i-1].state == "live"){
						cell[i].neiN = 1;
					}
				}
				//Cell has a cell South of it
				if (i < 6639){ //Only check South of you, if there is a cell there
					if (cell[i+1].state == "live"){
						cell[i].neiS = 1;
					}
				}
				//Cell has a cell West of it
				if (i > 80){ //Only check what is West of you, if there is a cell there
					if (cell[i-80].state == "live"){
						cell[i].neiW = 1;
					}
				}
				//Cell has a cell East of it
				if (i < 6560){ //Only check East of you, if there is a cell there
					if (cell[i+80].state == "live"){
						cell[i].neiE = 1;
					}
				}
				//Cell has a cell North-West of it
				if (i > 81){ //Only check what is NW of you, if there is a cell there
					if (cell[i-81].state == "live"){
						cell[i].neiNW = 1;
					}
				}
				//Cell has a cell North-East of it
				if (i < 6560){ //Only check East of you, if there is a cell there
					if (cell[i+79].state == "live"){
						cell[i].neiNE = 1;
					}
				}
				//Cell has a cell South-West  of it
				if (i > 79){ //Only check what is SW of you, if there is a cell there
					if (cell[i-79].state == "live"){
						cell[i].neiSW = 1;
					}
				}
				//Cell has a cell South-East of it
				if (i < 6559){ //Only check East of you, if there is a cell there
					if (cell[i+81].state == "live"){
						cell[i].neiSE = 1;
					}
				}
			}

			//Check number of cells around a LIVE cell
			if (cell[i].state == "dead") {
				//Cell has a cell North of it
				if (i > 0){ //Only check what is North of cell, if there is a cell there
					if (cell[i-1].state == "live"){
						cell[i].neiN = 1;
					}
				}
				//Cell has a cell South of it
				if (i < 6639){ //Only check South of you, if there is a cell there
					if (cell[i+1].state == "live"){
						cell[i].neiS = 1;
					}
				}
				//Cell has a cell West of it
				if (i > 80){ //Only check what is West of you, if there is a cell there
					if (cell[i-80].state == "live"){
						cell[i].neiW = 1;
					}
				}
				//Cell has a cell East of it
				if (i < 6560){ //Only check East of you, if there is a cell there
					if (cell[i+80].state == "live"){
						cell[i].neiE = 1;
					}
				}
				//Cell has a cell North-West of it
				if (i > 81){ //Only check what is NW of you, if there is a cell there
					if (cell[i-81].state == "live"){
						cell[i].neiNW = 1;
					}
				}
				//Cell has a cell North-East of it
				if (i < 6560){ //Only check East of you, if there is a cell there
					if (cell[i+79].state == "live"){
						cell[i].neiNE = 1;
					}
				}
				//Cell has a cell South-West  of it
				if (i > 79 && i < 6639){ //Only check what is SW of you, if there is a cell there
					if (cell[i-79].state == "live"){
						cell[i].neiSW = 1;
					}
				}
				//Cell has a cell South-East of it
				if (i < 6559){ //Only check East of you, if there is a cell there
					if (cell[i+81].state == "live"){
						cell[i].neiSE = 1;
					}
				}
			}
			//Add up how many cells are around you
			cell[i].total = cell[i].neiN + cell[i].neiS + cell[i].neiW + cell[i].neiE + cell[i].neiNW + cell[i].neiNE + cell[i].neiSW + cell[i].neiSE;
		}
	}
}

//Check the number of cells around the cell
void checkCells(Cell cell[]) {
	for (int i = 0; i < 6640; i++) {
		if (cell[i].alive) {

			//Determines if a "live" cell, lives, dies, or is born
			if (cell[i].state == "live"){
				//Dies of under-population
				if (cell[i].total < 2){
					cell[i].state = "dead";
				}
				//Dies of over-population
				if (cell[i].total > 3){
					cell[i].state = "dead";
				}
				//Stays alive, or is born
				if (cell[i].total == 2 || cell[i].total == 3){
					cell[i].state = "live";
				}
			}
			//Determines if a dead cell is born
			if (cell[i].state == "dead"){
				if (cell[i].total == 3){
					cell[i].state = "live";
				}
			}
		}
	}
}

//Render all the cells
void renderCell(Cell cell[], int camX, int camY, int mx, int my) {
	for (int i = 0; i < 6640; i++) {
		//Cells that are alive
		if (cell[i].alive) {

			//Only render if cell is on screen (the player has view of the cell)
			if (cell[i].onScreen ){
				//Live cells
				if (cell[i].state == "live") {
					SDL_Rect cellRect = { cell[i].x-camX,cell[i].y-camY, cell[i].w,cell[i].h };
					SDL_SetRenderDrawColor(gRenderer, 154,205,50, 255);
					SDL_RenderFillRect(gRenderer, &cellRect);
				} else{
				//Dead cells
					SDL_Rect cellRect = { cell[i].x-camX,cell[i].y-camY, cell[i].w,cell[i].h };
					SDL_SetRenderDrawColor(gRenderer, 205,20,50, 255);
					SDL_RenderFillRect(gRenderer, &cellRect);
				}

				//Black border around cells
				SDL_Rect cellRect = { cell[i].x-camX,cell[i].y-camY, cell[i].w,cell[i].h };
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 100);
				SDL_RenderDrawRect(gRenderer, &cellRect);
			}
			//Check if cell is on screen (the player has view of the cell)
			cell[i].onScreen = false;
			if (cell[i].x + cell[i].w >= camX && cell[i].x <= camX + gWindow.getWidth() &&
				cell[i].y + cell[i].h >= camY && cell[i].y <= camY + gWindow.getHeight()) {
				cell[i].onScreen = true;
			}

			//Check if mouse is hovering over cell, and render if true
			if (mx > cell[i].x && mx < cell[i].x + cell[i].w &&
				my > cell[i].y && my < cell[i].y + cell[i].h) {
				cell[i].mouseOnCell = true;
				gBlock.render(cell[i].x-camX,cell[i].y-camY, cell[i].w, cell[i].h);
			} else {
				cell[i].mouseOnCell = false;
			}
		}
	}
}
//-------------------------------------------------------------------------------//
/////////////////////////////////// CELLS DATA ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////




//Camera
struct Cam{
public:
	int x = 0, y = 0;
	bool moveLeft = false,
		 moveRight = false,
		 moveUp = false,
		 moveDown = false;
} cam;


//Our main
int main(int argc, char **argv) {

	init();
	loadMedia();
	SDL_Event e;

	//Our for loop bool
	bool running = true;

	//Auto-save
	int savtimer = 0;
	bool saving = false;
	bool autosav = true;

	//Mouse properties
	int mx;
	int my;
	bool leftClick = false;
	bool rightClick = false;

	//Start simulation
	bool start = false;

	//Timer for our checks
	int tick = 0;

	//Initialize Cells
	static Cell cell[6640];
	initCell(cell);



	//////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////-----------------------------------------------------/////////////////////
	//////////////////////////////////// Loading planet data /////////////////////////////////////
	//Remove ALL Cell(s), before loading a new file
	CELLS = 0;
	spawnDeadCells(cell);

	//Load Cell(s)
	fstream fileCellsc("data/cellNum.dat");
	if (fileCellsc.is_open()) {
		fileCellsc >> CELLS;
	}
	fileCellsc.close();
	//Place BLANK Cell(s)
	spawnDeadCells(cell);

	//Load Cell(s) data and apply to BLANK Cell(s)
	fstream fileCellCoor("data/cellData.dat");
	if (fileCellCoor.is_open()) {
		for (int i = 0; i < CELLS; ++i) {
				fileCellCoor    >> cell[i].x
							    >> cell[i].y
							    >> cell[i].w
							    >> cell[i].h
							    >> cell[i].state
							    >> cell[i].alive;
		}
	}
	fileCellCoor.close();
	//////////////////////////////////// Loading planet data /////////////////////////////////////
	////////////////////-----------------------------------------------------/////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////


	//This is called later in the loop to constantly check how many cells are around a cell
	//but for now we will check it ONCE, to get how many cells are around the cell
	updateCell(cell, mx+cam.x, my+cam.y);




	//This is the start of our loop
	while (running) {

		//Get mouse position
		SDL_GetMouseState(&mx, &my);

		while (SDL_PollEvent(&e) != 0) {
			//If the player pressed the "X" button, the running button, then set running = false; we running
			if (e.type == SDL_QUIT) {
				running = false;
			}

			//Handle window events
			gWindow.handleEvent(e);
			//If Key Pressed
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
				switch (e.key.keysym.sym) {
					case SDLK_LEFT:
						cam.moveLeft = true;
						break;
					case SDLK_RIGHT:
						cam.moveRight = true;
						break;
					case SDLK_UP:
						cam.moveUp = true;
						break;
					case SDLK_DOWN:
						cam.moveDown = true;
						break;
				}
			}
			//If Key Released
			else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
				switch (e.key.keysym.sym) {
					case SDLK_LEFT:
						cam.moveLeft = false;
						break;
					case SDLK_RIGHT:
						cam.moveRight = false;
						break;
					case SDLK_UP:
						cam.moveUp = false;
						break;
					case SDLK_DOWN:
						cam.moveDown = false;
						break;
				}
			}

			//If Pressed mouse button
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				//Left click pressed
				if (e.button.button == SDL_BUTTON_LEFT) {
					leftClick = true;
				}
				//Right click pressed
				if (e.button.button == SDL_BUTTON_RIGHT) {
					rightClick = true;
				}
			}

			//If Released mouse button
			if (e.type == SDL_MOUSEBUTTONUP) {
				//Left click released
				if (e.button.button == SDL_BUTTON_LEFT) {
					leftClick = false;
					 if (mx > cam.x && mx < cam.x + SCREEN_WIDTH && my > cam.y + 640 && my <= cam.y + SCREEN_HEIGHT){
							if (start){
								start = false;
							}else{
								start = true;
							}
					 }
				}
				//Right click released
				if (e.button.button == SDL_BUTTON_RIGHT) {
					rightClick = false;
				}
			}
		} // end controls

		//If the player if left clicking then revive the cell the mouse if over
		if (leftClick){
			reviveCell(cell);
		}

		//If the player if right clicking then kill the cell the mouse if over
		if (rightClick){
			killCell(cell);
		}

		//Move cam by WASD
		if (cam.moveLeft) {
			cam.x -= 20;
		} else if (cam.moveRight) {
			cam.x += 20;
		}
		if (cam.moveUp) {
			cam.y -= 20;
		}else if (cam.moveDown) {
			cam.y += 20;
		}

		//Update cell data; this checks how many cells are around the cell
		updateCell(cell, mx+cam.x, my+cam.y);

		//Timer for how often we check if cell dies, lives, or is born
		tick += 1;
		//Checks every 5 frames
		if (tick > 15) {
			tick = 0;
			if (start) {
				//Check if cell dies, lives, or is born
				checkCells(cell);
			}
		}

		//Clear Screen
		SDL_SetRenderDrawColor(gRenderer, 70, 70, 70, 255);
		SDL_RenderClear(gRenderer);

		//Render Cells
		renderCell(cell, cam.x, cam.y, mx + cam.x, my + cam.y);

		//Background yellow border for START/STOP
		SDL_Rect startRect = { 0, 640, gWindow.getWidth(), 32 };
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 255);
		SDL_RenderFillRect(gRenderer, &startRect);

		//If player if left clicking then draw darker shade of yellow
		if (leftClick) {
			 if (mx > cam.x && mx < cam.x + SCREEN_WIDTH && my > cam.y + 640 && my <= cam.y + SCREEN_HEIGHT){
				SDL_Rect startRect = { 0, 640, gWindow.getWidth(), 32 };
				SDL_SetRenderDrawColor(gRenderer, 199, 199, 0, 255);
				SDL_RenderFillRect(gRenderer, &startRect);
			 }
		}
		//Draw black border around yellow border
		SDL_Rect startRect2 = { 0, 640, gWindow.getWidth(), 32 };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &startRect2);

		//Print start/stop depending if the simulation has started or not
		if (start) {
			gText.loadFromRenderedText("STOP", { 0, 0, 0, 255 }, gFont25);
			gText.render(gWindow.getWidth() / 2 - gText.getWidth() / 2, 633,gText.getWidth(), gText.getHeight());
		} else {
			gText.loadFromRenderedText("START", { 0, 0, 0, 255 }, gFont25);
			gText.render(gWindow.getWidth() / 2 - gText.getWidth() / 2, 633,gText.getWidth(), gText.getHeight());
		}

		//Update the screen with rendering performed
		SDL_RenderPresent(gRenderer);

		//If auto-save is enabled, start the auto-save timer for saving
		if (autosav) {
			savtimer += 1;
			if (savtimer > 60) { //Save every 2 seconds
				savtimer = 0;
				saving = true;
			}
		}

		/////////////////////////////////////////////
		/////////////////// SAVING //////////////////
		if (saving){
			//Save # of Cells
			ofstream myfileCellc;
			myfileCellc.open(
					"data/cellNum.dat");
			myfileCellc << CELLS;
			myfileCellc.close();

			//Save Cell Data
			ofstream myfileplanetsc;
			myfileplanetsc.open("data/cellData.dat");
			for (int i = 0; i < 6640; i++) {
				if (cell[i].alive) {
					myfileplanetsc  << cell[i].x 	  << "\n"
									<< cell[i].y 	  << "\n"
									<< cell[i].w 	  << "\n"
									<< cell[i].h 	  << "\n"
									<< cell[i].state  << "\n"
									<< cell[i].alive  << "\n";
				}
			}
			myfileplanetsc.close();


			saving = false;
		}
		/////////////////// SAVING //////////////////
		/////////////////////////////////////////////



	}
	//Free resources and close SDL if running = true; running if we stop loop
	close();
	return 0;
}

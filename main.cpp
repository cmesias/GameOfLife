#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mouse.h>
#include <sstream>
#include <string>
#include <fstream> //Open & load files
using namespace std;

//Editing variables
int BLOCKW = 8;
int BLOCKH = 8;

/* 1080p screen */
int SCREEN_WIDTH = 960;
int SCREEN_HEIGHT = 960;
int HOWMANYBLOCKSW = 120;
int HOWMANYBLOCKSH = 116;
int CELLS = 0;
const int MAX_CELLS = 13920;

//Loading and initializing
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
	for (int i = 0; i < MAX_CELLS; i++) {
		cell[i].alive = false;
		cell[i].mouseOnCell = false;
		cell[i].onScreen = false;
		cell[i].state = "dead";
	}
}

//Revive a cell (used if you left click)
void reviveCell(Cell cell[]) {
	for (int i = 0; i < MAX_CELLS; i++) {
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
	for (int i = 0; i < MAX_CELLS; i++) {
		if (cell[i].alive) {
			if (cell[i].state == "live") {
				if (cell[i].mouseOnCell) {
					cell[i].state = "dead";
				}
			}
		}
	}
}

//Set all cells to dead (used if you pressed the space bar)
void setCellsDead(Cell cell[]) {
	for (int i = 0; i < MAX_CELLS; i++) {
		if (cell[i].alive) {
			cell[i].state = "dead";
		}
	}
}

//This cells, dead or alive
void spawnCell(Cell cell[], int x, int y, int w, int h, string state, bool alive) {
	for (int i = 0; i < MAX_CELLS; i++) {
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
	for (int i = 0; i < MAX_CELLS; i++) {
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
				if (i < MAX_CELLS-1){ //Only check South of you, if there is a cell there
					if (cell[i+1].state == "live"){
						cell[i].neiS = 1;
					}
				}
				//Cell has a cell West of it
				if (i > HOWMANYBLOCKSH){ //Only check what is West of you, if there is a cell there
					if (cell[i-HOWMANYBLOCKSH].state == "live"){
						cell[i].neiW = 1;
					}
				}
				//Cell has a cell East of it
				if (i < MAX_CELLS-HOWMANYBLOCKSH){ //Only check East of you, if there is a cell there
					if (cell[i+HOWMANYBLOCKSH].state == "live"){
						cell[i].neiE = 1;
					}
				}
				//Cell has a cell North-West of it
				if (i > (HOWMANYBLOCKSH+1)){ //Only check what is NW of you, if there is a cell there
					if (cell[i-(HOWMANYBLOCKSH+1)].state == "live"){
						cell[i].neiNW = 1;
					}
				}
				//Cell has a cell North-East of it
				if (i < MAX_CELLS-HOWMANYBLOCKSH){ //Only check East of you, if there is a cell there
					if (cell[i+(HOWMANYBLOCKSH-1)].state == "live"){
						cell[i].neiNE = 1;
					}
				}
				//Cell has a cell South-West  of it
				if (i > HOWMANYBLOCKSH-1){ //Only check what is SW of you, if there is a cell there
					if (cell[i-(HOWMANYBLOCKSH-1)].state == "live"){
						cell[i].neiSW = 1;
					}
				}
				//Cell has a cell South-East of it
				if (i < MAX_CELLS-(HOWMANYBLOCKSH+1)){ //Only check East of you, if there is a cell there
					if (cell[i+(HOWMANYBLOCKSH+1)].state == "live"){
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
				if (i < MAX_CELLS-1){ //Only check South of you, if there is a cell there
					if (cell[i+1].state == "live"){
						cell[i].neiS = 1;
					}
				}
				//Cell has a cell West of it
				if (i > HOWMANYBLOCKSH){ //Only check what is West of you, if there is a cell there
					if (cell[i-(HOWMANYBLOCKSH)].state == "live"){
						cell[i].neiW = 1;
					}
				}
				//Cell has a cell East of it
				if (i < MAX_CELLS-(HOWMANYBLOCKSH)){ //Only check East of you, if there is a cell there
					if (cell[i+(HOWMANYBLOCKSH)].state == "live"){
						cell[i].neiE = 1;
					}
				}
				//Cell has a cell North-West of it
				if (i > HOWMANYBLOCKSH+1){ //Only check what is NW of you, if there is a cell there
					if (cell[i-(HOWMANYBLOCKSH+1)].state == "live"){
						cell[i].neiNW = 1;
					}
				}
				//Cell has a cell North-East of it
				if (i < MAX_CELLS-HOWMANYBLOCKSH){ //Only check East of you, if there is a cell there
					if (cell[i+(HOWMANYBLOCKSH-1)].state == "live"){
						cell[i].neiNE = 1;
					}
				}
				//Cell has a cell South-West  of it
				if (i > HOWMANYBLOCKSH && i < MAX_CELLS-1){ //Only check what is SW of you, if there is a cell there
					if (cell[i-(HOWMANYBLOCKSH-1)].state == "live"){
						cell[i].neiSW = 1;
					}
				}
				//Cell has a cell South-East of it
				if (i < MAX_CELLS-(HOWMANYBLOCKSH+1)){ //Only check East of you, if there is a cell there
					if (cell[i+(HOWMANYBLOCKSH+1)].state == "live"){
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
	for (int i = 0; i < MAX_CELLS; i++) {
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
	for (int i = 0; i < MAX_CELLS; i++) {
		//Cells that are alive
		if (cell[i].alive) {

			//Only render if cell is on screen (the player has view of the cell)
			if (cell[i].onScreen ){
				//Live cells
				if (cell[i].state == "live") {
					gCells.render(cell[i].x-camX,cell[i].y-camY, cell[i].w,cell[i].h, &gCellsClips[0]);
				} else{
				//Dead cells
					gCells.render(cell[i].x-camX,cell[i].y-camY, cell[i].w,cell[i].h, &gCellsClips[1] );
				}

				//Black border around cells
				gCells.render(cell[i].x-camX,cell[i].y-camY, cell[i].w,cell[i].h, &gCellsClips[2]);
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
				gCells.render(cell[i].x-camX,cell[i].y-camY, cell[i].w,cell[i].h, &gCellsClips[0]);
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

//Set a declared rectangle's x and y position and their width and height
void setRect (int x, int y, int w, int h, SDL_Rect &inputRect){
	inputRect.x = x;
	inputRect.y = y;
	inputRect.w = w;
	inputRect.h = h;
};

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

	//Auto-save rect
	SDL_Rect autoSavRect = { };
	bool pressingAutoSavRect = false;

	//Mouse properties
	int mx;
	int my;
	bool leftClick = false;
	bool rightClick = false;

	//Start simulation
	bool start = false;

	//Timer for our checks
	int tick = 0;
	int tickFrames = 60;
	int tickSpeed = 10;
	int addTimer = 0;

	//Start simulation rect
	SDL_Rect startRect = { };
	bool pressingStartRect = false;

	//Increase or decrease simulation speed
	SDL_Rect speedRect = { };
	bool pressingSpeedRect = false;

	//Initialize Cells
	static Cell cell[MAX_CELLS];
	initCell(cell);

	//////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////-----------------------------------------------------/////////////////////
	//////////////////////////////////// Loading Cells data //////////////////////////////////////
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
	//////////////////////////////////// Loading Cells data //////////////////////////////////////
	////////////////////-----------------------------------------------------/////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////

	//This is called later in the loop to constantly check how many cells are around a cell
	//but for now we will check it ONCE before the loop, to get how many cells are around the cell
	updateCell(cell, mx+cam.x, my+cam.y);

	//This is the start of our loop
	while (running) {

		//Keep clickable blocks at certain x and y coordinates base on screen size
		setRect(0, gWindow.getHeight()-32, 32, 32, autoSavRect);
		setRect(32, gWindow.getHeight()-32, gWindow.getWidth()-64, 32, startRect);
		setRect(gWindow.getWidth()-32, gWindow.getHeight()-32,32,32, speedRect);

		//Get mouse position
		SDL_GetMouseState(&mx, &my);

		//Events
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
					case SDLK_SPACE:
						setCellsDead(cell);;
						break;
					case SDLK_0:
						saving = true;
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
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				//Left click pressed
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					leftClick = true;
				}
				//Right click pressed
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					rightClick = true;
				}
			}

			//If Released mouse button
			if (e.type == SDL_MOUSEBUTTONUP)
			{
				//Left click released
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					//Set left clicking to false after release of mouse click
					leftClick = false;

					//If Player releases mouse click on top of the start rect
					if (pressingStartRect)
					{
						if (!start)
						{
							start = true;
						}else{
							start = false;
						}
					}

					//If Player releases mouse click on top of the auto-sav rect
					if (pressingAutoSavRect)
					{
						if (!autosav)
						{
							autosav = true;
						}else{
							autosav = false;
						}
					}

					//If Player releases mouse click on top of the increase-simulation-speed rect
					if (pressingSpeedRect)
					{
						if (tickSpeed < 60){
							tickSpeed += 1;
						}
					}

					//Reset add timer after release of mouse click
					addTimer = 0;
				}
				//Right click released
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					rightClick = false;

					//If Player releases mouse click on top of the simulation-speed rect: decrease simulation speed
					if (pressingSpeedRect)
					{
						if (tickSpeed > 1){
							tickSpeed -= 1;
						}
					}

					//Reset subtract timer after release of mouse click
					addTimer = 0;
				}
				//Turn pressing the start and auto-sav rect to false after use
				pressingStartRect = false;
				pressingAutoSavRect = false;
				pressingSpeedRect = false;
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
			cam.x -= 10;
		} else if (cam.moveRight) {
			cam.x += 10;
		}
		if (cam.moveUp) {
			cam.y -= 10;
		}else if (cam.moveDown) {
			cam.y += 10;
		}

		//Update cell data; this checks how many cells are around the cell
		updateCell(cell, mx+cam.x, my+cam.y);

		//Timer for how often we check if cell dies, lives, or is born
		tick += tickSpeed;

		//Checks every 'tickSpeed' frames
		if (tick > tickFrames) {
			tick = 1;
			if (start) {
				//Check if a cell dies, lives, or is born
				checkCells(cell);
			}
		}

		//Check's what the Player is left-clicking on
		if (leftClick) {
			//Check if Player is clicking on our start rect
			if (mx > startRect.x && mx < startRect.x + startRect.w  && my > startRect.y && my < startRect.y + startRect.h){
				pressingStartRect = true;
			}else{
				pressingStartRect = false;
			}

			//Check if Player is clicking on our auto-sav rect
			if (mx > autoSavRect.x && mx < autoSavRect.x + autoSavRect.w  && my > autoSavRect.y && my < autoSavRect.y + autoSavRect.h){
				pressingAutoSavRect = true;
			}else{
				pressingAutoSavRect = false;
			}

			//Check if Player is clicking on our simulation-speed rect
			if (mx > speedRect.x && mx < speedRect.x + speedRect.w  && my > speedRect.y && my < speedRect.y + speedRect.h){
				pressingSpeedRect = true;
			}else{
				pressingSpeedRect = false;
			}

			//If Player releases mouse click on top of the increase-simulation-speed rect
			if (pressingSpeedRect){
				//If Player is still clicking to increase simulation-speed, keep increasing it after a set time
				addTimer += 1;
				if (addTimer > 15){
					if (tickSpeed < 60){
						tickSpeed += 1;
					}
				}
			}
		}

		//Check's what the Player is right-clicking on
		if (rightClick)  {
			//Check if Player is clicking on our simulation-speed rect
			if (mx > speedRect.x && mx < speedRect.x + speedRect.w  && my > speedRect.y && my < speedRect.y + speedRect.h){
				pressingSpeedRect = true;
			}else{
				pressingSpeedRect = false;
			}

			//If Player releases mouse click on top of the increase-simulation-speed rect
			if (pressingSpeedRect){
				//If Player is still clicking to decrease simulation-speed, keep increasing it after a set time
				addTimer += 1;
				if (addTimer > 15){
					if (tickSpeed > 1){
						tickSpeed -= 1;
					}
				}
			}
		}

		//If we started simulation then if auto-save is enabled, start the auto-save timer for saving
		if (start) {
			if (autosav) {
				savtimer += 1;
				if (savtimer > 60) { //Save every 60 frames
					savtimer = 0;
					saving = true;
				}
			}
		}

		//Clear Screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderClear(gRenderer);

			//Render Cells
			renderCell(cell, cam.x, cam.y, mx + cam.x, my + cam.y);

			//Render green rect (player clicking start/stop button, red rect (button not being clicked) start rect
			if (pressingStartRect) {
				SDL_SetRenderDrawColor(gRenderer, 199, 199, 0, 255);
				SDL_RenderFillRect(gRenderer, &startRect);
			}else{
				SDL_SetRenderDrawColor(gRenderer, 0, 200, 0, 255);
				SDL_RenderFillRect(gRenderer, &startRect);

			}
			//Render a black border around start rect
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(gRenderer, &startRect);

			//----------------------------------------------------------------------------------------//

			//Render a green rect if auto-saving is enabled, else orange rect
			if (autosav){
				SDL_SetRenderDrawColor(gRenderer, 0, 200, 0, 255);
				SDL_RenderFillRect(gRenderer, &autoSavRect);
			}else{
				SDL_SetRenderDrawColor(gRenderer, 200, 0, 0, 255);
				SDL_RenderFillRect(gRenderer, &autoSavRect);
			}

			//Render auto-sav rect if player is clicking over it
			if (pressingAutoSavRect){
				if (autosav){
					SDL_SetRenderDrawColor(gRenderer, 0, 134, 0, 255);
					SDL_RenderFillRect(gRenderer, &autoSavRect);
				}else{
					SDL_SetRenderDrawColor(gRenderer, 134, 0, 0, 255);
					SDL_RenderFillRect(gRenderer, &autoSavRect);
				}
			}

			//Render a red rect if saving
			if (saving){
				SDL_SetRenderDrawColor(gRenderer, 200, 144, 25, 255);
				SDL_RenderFillRect(gRenderer, &autoSavRect);
			}

			//Render a black border around auto-sav rect
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(gRenderer, &autoSavRect);

			//----------------------------------------------------------------------------------------//

			//Render white rect (Player is clicking it), grey rect (rect not being clicked) rect
			if (pressingSpeedRect) {
				SDL_SetRenderDrawColor(gRenderer, 100, 100, 100, 255);
				SDL_RenderFillRect(gRenderer, &speedRect);
			}else{
				SDL_SetRenderDrawColor(gRenderer, 212, 212, 212, 255);
				SDL_RenderFillRect(gRenderer, &speedRect);
			}

			//Render a black border around simulation-speed rect
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(gRenderer, &speedRect);

			//Print start/stop depending if the simulation has started or not
			if (start) {
				gText.loadFromRenderedText("STOP", { 0, 0, 0, 255 }, gFont25);
				gText.render(startRect.x+startRect.w/2 - gText.getWidth()/2, startRect.y+startRect.h/2 - gText.getHeight()/2,gText.getWidth(), gText.getHeight());
			} else {
				gText.loadFromRenderedText("START", { 0, 0, 0, 255 }, gFont25);
				gText.render(startRect.x+startRect.w/2 - gText.getWidth()/2, startRect.y+startRect.h/2 - gText.getHeight()/2,gText.getWidth(), gText.getHeight());
			}

			//Render sav text
			gText.loadFromRenderedText("S", { 0, 0, 0, 255 }, gFontMunro16);
			gText.render(autoSavRect.x+autoSavRect.w/2 - gText.getWidth()/2, autoSavRect.y+autoSavRect.h/2 - gText.getHeight()/2,gText.getWidth(), gText.getHeight());

			//Render simulation text
			std::stringstream simuss;
			simuss << tickSpeed;
			gText.loadFromRenderedText(simuss.str().c_str(), { 0, 0, 0, 255 }, gFontMunro16);
			gText.render(speedRect.x+speedRect.w/2 - gText.getWidth()/2, speedRect.y+speedRect.h/2 - gText.getHeight()/2,gText.getWidth(), gText.getHeight());

		//Update the screen with rendering performed
		SDL_RenderPresent(gRenderer);

		//If saving
		if (saving){
			//////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////-----------------------------------------------------/////////////////////
			///////////////////////////////////// Saving Cells data //////////////////////////////////////
			//Save # of Cells
			ofstream myfileCellc;
			myfileCellc.open(
					"data/cellNum.dat");
			myfileCellc << CELLS;
			myfileCellc.close();

			//Save Cell Data
			ofstream myfileplanetsc;
			myfileplanetsc.open("data/cellData.dat");
			for (int i = 0; i < MAX_CELLS; i++) {
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
			///////////////////////////////////// Saving Cells data //////////////////////////////////////
			////////////////////-----------------------------------------------------/////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////

			//Stop saving
			saving = false;
		}
	}
	//Free resources and close SDL if running = true; running if we stop loop
	close();
	return 0;
}

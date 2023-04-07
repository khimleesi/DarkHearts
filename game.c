//===========================================================================================
// Declare the Header Files
//===========================================================================================
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <tonc.h>


#include "background.h"
#include "player.h"
#include "event.h"
#include "ss.h"
#include "floor.h"
#include "textbox.h"

//===========================================================================================
// Hash table / Defines
//===========================================================================================

//WALLS
#define SBB_0 27

//TEXT
#define SBB_1 31

//FLOORS
#define SBB_2 25

//TEXT BOX
#define SBB_3 29

#define MAP_WIDTH 1024
#define TILE_SIZE 32
#define SPRITE_SIZE 8
#define NUM_EVENTS 10
#define MAX_TIMER 1000
#define MAX_HEARTS 10
#define TIMER_COUNT 40


//===========================================================================================
// Create our Object Data
//===========================================================================================
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *player = &obj_buffer[0];		//Add the player sprite to our buffer at element 0
OBJ_ATTR *event  = &obj_buffer[1];		//Add the player sprite to our buffer at element 1

SCR_ENTRY *lava = se_mem[SBB_0];

typedef struct Event {					//Create our Event struct

	int current_x;
	int current_y;
	int event_x;
	int event_y;
	u32 event_tid; 
	u32 event_pb;
	int type;

} Event;

int numEvents = NUM_EVENTS;						//Number of Events (same as Event objects)
Event events[10];						//Create x amount of Event objects

//===========================================================================================
// Forward Declare the Functions
//===========================================================================================
void Init();
void Update();
void Draw();
void Clear();

void Input();
void SpawnLocation(Event*, int);
void RandPosition();
bool Collision();
void UpdateFrames();
void Guide();

void LoadIntoMemory();
void LavaAnimation();

void SwitchOnBackgrounds();
void SwitchOnEverything();
void SwitchOnTextbox();
void SwitchOnMap();
void ResetGame();
void PositionBackground();

//===========================================================================================
// Create the Global Variables
//===========================================================================================
int gameState = 0;						//gameState starts at 0 (Menu Screen)
int menuState = 0;
int time_counter = 0;						//A counter, used as a timer to slow down the game
int fps = 0;
int screen_width = 240;
int screen_height = 160;
int background_x;
int background_y;
int player_x;
int player_y;
int player_height = 16;
int player_width = 16;
int frameCount = 0;
int animOffset = 0;//for animation 0=down 3=up right=6 and left=9
int animationSet = 0;//0=stationary 1=walking
int timer = 90;						//Player starting score
int hearts = 0;

bool hasSpawned[10];
bool isPaused = false;
bool gameOver = false;
bool randEvent = false;

//Events
bool reduceSpeed = false;
bool playerStuck = false;
bool playerInvisible = false;
bool reverseKeys = false;
bool playerZombie = false;
bool blackHeart = false;
int speedFrame = 0;
int speedTimer = 0;
int stuckTimer = 0;
int invisibleTimer = 0;
int reverseTimer = 0;
int zombieTimer = 0;
int blackTimer = 0;


bool isColliding[10];

u32 player_tid = 0, player_pb = 0;

char buffer[256];						//An array for our text output

unsigned short world_grid[64][32];	//Initialize our world grid 2D array

typedef enum { DOWN, UP, RIGHT, LEFT } Directions;
Directions direction;


void Unpause() {
	SwitchOnEverything();
}


void PositionBackground() {
	//SET UP FLOOR POSITION
	REG_BG3HOFS= 100; //start menu
	REG_BG3VOFS= 30;
			
	//SET UP WALL POSITION
	REG_BG2HOFS= 100; //start menu
	REG_BG2VOFS= 30;
			
	//SET UP TEXT BOX POSITION
	REG_BG1HOFS =- 44; //start menu
	REG_BG1VOFS =- 18;

}


void UpdateFrames() {

	if(animationSet) {
		frameCount++;
			
		if (frameCount > 3) {
			frameCount = 0;
		}

		switch (direction) {
	
			case DOWN:
				animOffset = 0;
				break;
			
			case UP:
				animOffset = 3;
				break;
			
			case RIGHT:
				animOffset = 6;
				break;
			
			case LEFT:
				animOffset = 9;
				break;
		}
	
		switch(frameCount) {
	
			case 0:
				player_tid = (0 + animOffset);
				break;
					
			case 1:
				player_tid = (1 + animOffset);
				break;
					
			case 2:
				player_tid = (0 + animOffset);
				break;
					
			case 3:
				player_tid = (2 + animOffset);
				break;
		}
	}
	
	else {
		frameCount = 0;
	}
}


//===========================================================================================
// Game Objects (Event) Locations
//===========================================================================================
void SpawnLocation(Event* event, int location) {
	
	//A function that points to our Event struct and updates the events X and Y location
	event->type = rand() % 10;
	
	switch (location) {
		
		case 0:
			event->event_x = 33 * SPRITE_SIZE;
			event->event_y = 5 * SPRITE_SIZE;
		
			break;
		
		case 1:
			event->event_x = 38 * SPRITE_SIZE;
			event->event_y = 14 * SPRITE_SIZE;
			break;
			
		case 2:
			event->event_x = 44 * SPRITE_SIZE;
			event->event_y = 17 * SPRITE_SIZE;
			break;	

		case 3:
			event->event_x = 35 * SPRITE_SIZE;
			event->event_y = 13 * SPRITE_SIZE;
			break;	

		case 4:
			event->event_x = 43 * SPRITE_SIZE;
			event->event_y = 9 * SPRITE_SIZE;
			break;
	
		case 5:
			event->event_x = 20 * SPRITE_SIZE;
			event->event_y = 13 * SPRITE_SIZE;
			break;
			
		case 6:
			event->event_x = 28 * SPRITE_SIZE;
			event->event_y = 22 * SPRITE_SIZE;
			break;
			
		case 7:
			event->event_x = 18 * SPRITE_SIZE;
			event->event_y = 21 * SPRITE_SIZE;
			break;
	
		case 8:
			event->event_x = 8 * SPRITE_SIZE;
			event->event_y = 19 * SPRITE_SIZE;
			break;
			
		case 9:
			event->event_x = 9 * SPRITE_SIZE;
			event->event_y = 11 * SPRITE_SIZE;
			break;
		
		case 10:
			event->event_x = 5 * SPRITE_SIZE;
			event->event_y = 9 * SPRITE_SIZE;
			break;
		
		case 11:
			event->event_x = 11 * SPRITE_SIZE;
			event->event_y = 5 * SPRITE_SIZE;
			break;
			
		case 12:
			event->event_x = 11 * SPRITE_SIZE;
			event->event_y = 17 * SPRITE_SIZE;
			break;
			
		case 13:
			event->event_x = 2 * SPRITE_SIZE;
			event->event_y = 22 * SPRITE_SIZE;
			break;
			
		case 14:
			event->event_x = 21 * SPRITE_SIZE;
			event->event_y = 21 * SPRITE_SIZE;
			break;
		
		case 15:
			event->event_x = 40 * SPRITE_SIZE;
			event->event_y = 18 * SPRITE_SIZE;
			break;
			
	}
}	



//===========================================================================================
// Update the Screen
//===========================================================================================
void Update() {
//===========================================================================================
// Check if player is stuck (GLUE HEART EVENT is active or not)
//===========================================================================================
	if (playerStuck) {
		stuckTimer++;
	}
		
	if (stuckTimer == MAX_TIMER) {
		playerStuck = false;
		stuckTimer = 0;
	}
	
//===========================================================================================
// Check if player is Invisible (INVISIBLE HEART EVENT is active or not)
//===========================================================================================
	if (playerInvisible) {
		SwitchOnMap();
		invisibleTimer++;
	}
		
	if (invisibleTimer == MAX_TIMER) {
		playerInvisible = false;
		invisibleTimer = 0;
		SwitchOnEverything();
	}
	
//===========================================================================================
// Check if player keys are reversed (REVERSE HEART EVENT is active or not)
//===========================================================================================
	if (reverseKeys) {
		reverseTimer++;
	}
		
	if (reverseTimer == MAX_TIMER) {
		reverseKeys = false;
		reverseTimer = 0;
	}
	
//===========================================================================================
// Check if player is zombie (ZOMBIE HEART EVENT is active or not)
//===========================================================================================
	if (playerZombie) {
		zombieTimer++;
		player_pb = 1;
	}
		
	if (zombieTimer == MAX_TIMER) {
		playerZombie = false;
		zombieTimer = 0;
		player_pb = 0;
	}

//===========================================================================================
// Check if hearts are black (BLACK HEART EVENT is active or not)
//===========================================================================================
	int b;
	
	if (blackHeart) {
		
		for (b = 0; b < numEvents; b++) {
			events[b].event_pb = 3;
		}
		
		blackTimer++;
	}
		
	if (blackTimer == MAX_TIMER) {
		blackHeart = false;
		blackTimer = 0;
		
			for (b = 0; b < numEvents; b++) {
				events[b].event_pb = 0;
		}
	}
	
	if (hearts == MAX_HEARTS && !gameOver) {
		gameState = 5;
	
	}
	
	if (timer <= 0) {
		timer = 0;
		gameState = 4;
		gameOver = true;
		
		if (gameOver) {
			Clear();
			REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;//disable background 1
		}
	}

//===========================================================================================
// Background Animation Effect (changing Palette Bank)
//===========================================================================================
	LavaAnimation();
		
//===========================================================================================
// Player Object
//===========================================================================================
	
	int x = (screen_width - player_width) / 2;
	int y = (screen_height) / 2 - player_height;
	
	obj_set_attr(player, 
			ATTR0_SQUARE,				//Square, regular sprite
			ATTR1_SIZE_8,				//8x8tile / 64x64pixels
			ATTR2_PALBANK(player_pb) | player_tid);	//Pal bank 0, tile 0
	
	player->attr2= ATTR2_BUILD(player_tid, player_pb, 0);
	obj_set_pos(player, x, y);			//Set the position of our object
	
	
//===========================================================================================
// Event Object
//===========================================================================================
	
	int e;
	
	for(e = 0; e < numEvents; e++) {
	
		obj_set_attr(&event[e], 
		ATTR0_SQUARE,					//Square, regular sprite
		ATTR1_SIZE_8,					//8x8tile / 64x64pixels
		ATTR2_PALBANK(events[e].event_pb) | events[e].event_tid);	//Pal bank 0, tile 1
		
		obj_set_pos(&event[e], events[e].current_x, events[e].current_y);
	}

	oam_copy(oam_mem, obj_buffer, 1 + numEvents);	//Update our objects, was 2 +numEvents
}


//===========================================================================================
// Render Everything to the Screen
//===========================================================================================
void Draw() {

	//DRAW THE FLOORS
	REG_BG3HOFS= background_x; //start menu
	REG_BG3VOFS= background_y;
	
	//DRAW THE WALLS
	REG_BG2HOFS= background_x; //start menu
	REG_BG2VOFS= background_y;

	tte_write("#{P:10,10}");
	sprintf(buffer, "#{cx:0x6000}Time: \%d", timer);
	tte_write(buffer);
	
	tte_write("#{P:10,30}");
	sprintf(buffer, "#{cx:0x5000}Hearts: \%d", hearts);
	tte_write(buffer);
}


void SwitchOnBackgrounds() {
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG3 | DCNT_BG2 | DCNT_BG1;
}


void SwitchOnEverything() {

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG3 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_2D;

}

void SwitchOnTextbox() {

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1;

}


void SwitchOnMap() {

REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG2 | DCNT_BG3;

}

//===========================================================================================
// Initialize Function to Start Everything
//===========================================================================================
void Init() {

	//DISPLAY TEXT, FLOORS AND WALLS
	SwitchOnEverything();
	
	int e;
	
	for (e = 0; e < numEvents; e++) {
		events[e].event_tid= 32;			//Set our tile ID for our Event objects
		events[e].event_pb =0;			//Set our pal bank for our Event object
	}
		
	direction = DOWN;
	background_x = 0; 				//Initialize background X to 0
	background_y = 0;				//Initialize background Y to 0
	player_x = 14;					//Initialize player X starting position to 14 (in tiles)
	player_y = 8; 					//Initialize player X starting position to 8 (in tiles)
	
	//Initialise the world grid
	int map_x, map_y, map;
	
	//The map is 64x32, effectively 2 32x32 maps to be addressed seperately
	for(map = 0; map < 2; map++) {
		for(map_y = 0; map_y < TILE_SIZE; map_y++) {
			for(map_x = 0; map_x < TILE_SIZE; map_x++) {
				world_grid[map_x + (TILE_SIZE * map)][map_y] = backgroundMap[map_x + (map_y * TILE_SIZE) + (MAP_WIDTH * map)];
			}
		}
	}
}


void ResetGame() {

	randEvent = false;
	reduceSpeed = false;
	playerStuck = false;
	playerInvisible = false;
	reverseKeys = false;
	playerZombie = false;
	blackHeart = false;

	speedFrame = 0;
	speedTimer = 0;
	stuckTimer = 0;
	invisibleTimer = 0;
	reverseTimer = 0;
	zombieTimer = 0;
	blackTimer = 0;
	timer = 90;
	hearts = 0;
 
	int i;
 
	for (i = 0; i < 16; i++) {
		isColliding[i] = false;
	}
}

void LoadIntoMemory() {

	PositionBackground();
	
	//FLOORS
	//Load our floor background under our walls background
	memcpy(pal_bg_mem, floorPal, floorPalLen);
	memcpy(&tile_mem[2][0], floorTiles, floorTilesLen);
	memcpy(&se_mem[SBB_2][0], floorMap, floorMapLen);
	
	//WALLS
	//Load our background wall
	memcpy(pal_bg_mem, backgroundPal, backgroundPalLen);
	memcpy(&tile_mem[0][0], backgroundTiles, backgroundTilesLen);
	memcpy(&se_mem[SBB_0][0], backgroundMap, backgroundMapLen);
	
	//TEXTBOX
	//Load our textbox
	memcpy(&tile_mem[3][0], textboxTiles, textboxTilesLen);
	memcpy(&se_mem[SBB_3][0], textboxMap, textboxMapLen);
	
	//SPRITESHEET
	//Load our spritesheet
	memcpy(&tile_mem[4][0], ssTiles, ssTilesLen);
	memcpy(pal_obj_mem, ssPal, ssPalLen);
	
	oam_init(obj_buffer, 128);
	
	//Initialise our screen and load our sprites
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG3 | DCNT_BG2 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_2D;
    
	//Loads background sizes into registry
	REG_BG3CNT= BG_CBB(2) | BG_SBB(SBB_2) | BG_4BPP | BG_REG_64x32; //LOAD THE FLOORS
	REG_BG2CNT= BG_CBB(0) | BG_SBB(SBB_0) | BG_4BPP | BG_REG_64x32; //LOAD THE WALLS
	REG_BG1CNT= BG_CBB(3) | BG_SBB(SBB_3) | BG_4BPP | BG_REG_64x32; //LOAD THE TEXTBOX
	
	//Initialise BG 1 for text on screen entries
	tte_init_se_default(0, BG_CBB(1)|BG_SBB(SBB_1));
}


//===========================================================================================
// Collision Detection
//===========================================================================================
bool Collision() {

	//A boolean function that checks for collision between objects
	
	//Get the players X and Y position * 8 and store it in our temp X and Y variables
	//We do this as we have to convert players position from tiles to pixels
	int temp_x = player_x * SPRITE_SIZE;
	int temp_y = player_y * SPRITE_SIZE;

	int i;

	//Loop through our Event game objects
	for (i = 0; i < numEvents; i++) {
	
		if (isColliding[i] == true) {
			events[i].event_tid = 64;
		}
		
		//And if the players X and Y collide with game objects X and Y
		if (temp_x == events[i].event_x && temp_y == events[i].event_y) {

			switch(events[i].type) {
			
				case 0:
				
					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent) {

						SwitchOnBackgrounds();
						tte_write("#{P:90,60}");
						sprintf(buffer, "#{cx:0x5000}SLOW HEART");
						tte_write(buffer);
						tte_write("#{P:75,70}");
						sprintf(buffer, "#{cx:0x5000}Speed reduced!");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x5000}[PRESS START]");
						tte_write(buffer);
				
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							Clear();
							reduceSpeed = true;
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
				
				case 1:
					
					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent && !isPaused && !gameOver) {
						

						SwitchOnBackgrounds();
						tte_write("#{P:90,60}");
						sprintf(buffer, "#{cx:0x5000}GLUE HEART");
						tte_write(buffer);
						tte_write("#{P:75,70}");
						sprintf(buffer, "#{cx:0x5000}Stuck in glue!");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x5000}[PRESS START]");
						tte_write(buffer);
					
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							Clear();
							playerStuck = true;
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
			
				case 2:
					
					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent && !isPaused && !gameOver) {
						
						SwitchOnBackgrounds();
						tte_write("#{P:85,60}");
						sprintf(buffer, "#{cx:0x5000}RESET HEART");
						tte_write(buffer);
						tte_write("#{P:65,70}");
						sprintf(buffer, "#{cx:0x5000}Return to start!");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x5000}[PRESS START]");
						tte_write(buffer);
					
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							Clear();
							player_x = 14;
							player_y = 8;
							background_x = 0; 
							background_y = 0;
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
				
				case 3:
		
					if (!isColliding[i]) {
						randEvent = true;
					}
		
					if (randEvent && !isPaused && !gameOver) {

						SwitchOnBackgrounds();
						tte_write("#{P:85,60}");
						sprintf(buffer, "#{cx:0x5000}BROKEN HEART");
						tte_write(buffer);
						tte_write("#{P:65,70}");
						sprintf(buffer, "#{cx:0x5000}Time reduction!");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x5000}[PRESS START]");
						tte_write(buffer);
						
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							Clear();
							timer -= 15;
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
				
				case 4:

					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent && !isPaused && !gameOver) {

						SwitchOnBackgrounds();
						tte_write("#{P:90,60}");
						sprintf(buffer, "#{cx:0x6000}LOVE HEART");
						tte_write(buffer);
						tte_write("#{P:65,70}");
						sprintf(buffer, "#{cx:0x6000}Time increased!");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x6000}[PRESS START]");
						tte_write(buffer);
				
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							Clear();
							timer += 15;
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
				
				case 5:

					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent && !isPaused && !gameOver) {

						SwitchOnBackgrounds();
						tte_write("#{P:70,60}");
						sprintf(buffer, "#{cx:0x5000}INVISIBLE HEART");
						tte_write(buffer);
						tte_write("#{P:60,70}");
						sprintf(buffer, "#{cx:0x5000}I can't see shit!");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x5000}[PRESS START]");
						tte_write(buffer);
				
						if (key_hit(KEY_START)) {
						
							hearts++;
							playerInvisible = true;
							Clear();
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
				
				case 6:
					
					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent && !isPaused && !gameOver) {
						VBlankIntrWait();
						key_poll();

						SwitchOnBackgrounds();
						tte_write("#{P:75,60}");
						sprintf(buffer, "#{cx:0x5000}REVERSE HEART");
						tte_write(buffer);
						tte_write("#{P:60,70}");
						sprintf(buffer, "#{cx:0x5000}Reversed movement");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x5000}[PRESS START]");
						tte_write(buffer);
				
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							Clear();
							reverseKeys = true;
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
		
				case 7:
					
					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent && !isPaused && !gameOver) {

						SwitchOnBackgrounds();
						tte_write("#{P:80,60}");
						sprintf(buffer, "#{cx:0x6000}ZOMBIE HEART");
						tte_write(buffer);
						tte_write("#{P:70,70}");
						sprintf(buffer, "#{cx:0x6000}Feeeed meeeeee!");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x6000}[PRESS START]");
						tte_write(buffer);
						
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							playerZombie = true;
							Clear();
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
				
				case 8:

					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent && !isPaused && !gameOver) {

						SwitchOnBackgrounds();
						tte_write("#{P:80,60}");
						sprintf(buffer, "#{cx:0x5000}BLACK HEART");
						tte_write(buffer);
						tte_write("#{P:65,70}");
						sprintf(buffer, "#{cx:0x5000}I have no soul!");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x5000}[PRESS START]");
						tte_write(buffer);
				
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							blackHeart = true;
							Clear();
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
				
				case 9:
			
					if (!isColliding[i]) {
						randEvent = true;
					}
				
					if (randEvent && !isPaused && !gameOver) {

						SwitchOnBackgrounds();
						tte_write("#{P:80,60}");
						sprintf(buffer, "#{cx:0x5000}DEATH HEART");
						tte_write(buffer);
						tte_write("#{P:60,70}");
						sprintf(buffer, "#{cx:0x5000}Time halved! Haha");
						tte_write(buffer);
						tte_write("#{P:75,80}");
						sprintf(buffer, "#{cx:0x5000}[PRESS START]");
						tte_write(buffer);
						
						if (key_hit(KEY_START)) {
						
							hearts++;
							SwitchOnEverything();
							timer = timer/2;
							Clear();
							isColliding[i] = true;
							randEvent = false;
						}
					}
				break;
			}
			
		return true;
					
		}
	}	
	
	Clear();
	return false;					//Else collision didn't happen			
}


void Clear() {

tte_write("#{es}");		//Clear any text from the screen

}



//===========================================================================================
// Handle User Input
//===========================================================================================
void Input() {

	//===========================================================================================
	// Get our key codes
	//===========================================================================================
	
	key_poll();
	bool moving = false;
	
	//===========================================================================================
	// UP button is pressed
	//===========================================================================================
	
	if ((!reverseKeys && key_is_down(KEY_UP))&&(!world_grid[player_x][player_y-1])&& !randEvent && !playerStuck) {

		if (!reduceSpeed) {
			direction = UP;
			animationSet = 1;
			moving = true;
			background_y -= SPRITE_SIZE;		//Player sprite is 8x8 so move the background Y position by -8
			player_y--;
		}
		
		else {
			speedFrame++;
			speedTimer++;

			if (speedFrame == 3 && speedTimer != TIMER_COUNT) {
				speedFrame = 0;
				direction = UP;
				animationSet = 1;
				moving = true;
				background_y -= SPRITE_SIZE;		//Player sprite is 8x8 so move the background Y position by -8
				player_y--;
			}

			else if (speedTimer == TIMER_COUNT) {
				speedTimer = 0;
				reduceSpeed = false;
			}
		}
	}
	else if ((reverseKeys && key_is_down(KEY_UP))&&(!world_grid[player_x][player_y+1])&& !randEvent && !playerStuck) {
	
			direction = DOWN;
			animationSet = 1;
			moving = true;
			background_y += SPRITE_SIZE;
			player_y++;
	}
	//===========================================================================================
	// DOWN button is pressed
	//===========================================================================================
	
	if ((!reverseKeys && key_is_down(KEY_DOWN))&& (!world_grid[player_x][player_y+1])&& !randEvent && !playerStuck) {

		if (!reduceSpeed) {
			direction = DOWN;
			animationSet = 1;
			moving = true;
			background_y += SPRITE_SIZE;
			player_y++;
		}
			
		else {
			speedFrame++;
			speedTimer++;

			if (speedFrame == 3 && speedTimer != TIMER_COUNT) {
				speedFrame = 0;
				direction = DOWN;
				animationSet = 1;
				moving = true;
				background_y += SPRITE_SIZE;
				player_y++;
			}

			else if (speedTimer == 40) {
				speedTimer = 0;
				reduceSpeed = false;
			}
		}
	}
	
	else if ((reverseKeys && key_is_down(KEY_DOWN))&& (!world_grid[player_x][player_y-1])&& !randEvent && !playerStuck) {
			
			direction = UP;
			animationSet = 1;
			moving = true;
			background_y -= SPRITE_SIZE;		//Player sprite is 8x8 so move the background Y position by -8
			player_y--;
	}
	
	//===========================================================================================
	// RIGHT button is pressed
	//===========================================================================================
	
	if ((!reverseKeys && key_is_down(KEY_RIGHT))&&(!world_grid[player_x+1][player_y])&& !randEvent && !playerStuck) {
			
		if (!reduceSpeed) {
			direction = RIGHT;
			animationSet = 1;
			moving = true;
			background_x += SPRITE_SIZE;		//Player sprite is 8x8 so move the background X position by +8
			player_x++;				//Increase player X position by 1
		}

		else {
			speedFrame++;
			speedTimer++;

			if (speedFrame == 3 && speedTimer != TIMER_COUNT) {
				speedFrame = 0;
				direction = RIGHT;
				animationSet = 1;
				moving = true;
				background_x += SPRITE_SIZE;		//Player sprite is 8x8 so move the background X position by +8
				player_x++;				//Increase player X position by 1
			}

			else if (speedTimer == 40) {
				speedTimer = 0;
				reduceSpeed = false;
			}
		}
	}
	
	else if ((reverseKeys && key_is_down(KEY_RIGHT))&&(!world_grid[player_x-1][player_y])&& !randEvent && !playerStuck) {
	
			direction = LEFT;
			animationSet = 1;
			moving = true;
			background_x -= SPRITE_SIZE;		//This time, instead move the background X position by -8
			player_x--;				//And decrease player X position by 1
			
	}
	
	//===========================================================================================
	// LEFT button is pressed
	//===========================================================================================
	
	if ((!reverseKeys && key_is_down(KEY_LEFT))&&(!world_grid[player_x-1][player_y])&& !randEvent && !playerStuck) {
		
		if (!reduceSpeed) {
			direction = LEFT;
			animationSet = 1;
			moving = true;
			background_x -= SPRITE_SIZE;		//This time, instead move the background X position by -8
			player_x--;				//And decrease player X position by 1
		}

		else {
			
			speedFrame++;
			speedTimer++;
				
			if (speedFrame == 3 && speedTimer != TIMER_COUNT) {
				speedFrame = 0;
				direction = LEFT;
				animationSet = 1;
				moving = true;
				background_x -= SPRITE_SIZE;		//This time, instead move the background X position by -8
				player_x--;				//And decrease player X position by 1
			}

			else if (speedTimer == 40) {
				speedTimer = 0;
				reduceSpeed = false;
			}
		}	
	}
	
	else if ((reverseKeys && key_is_down(KEY_LEFT))&&(!world_grid[player_x+1][player_y])&& !randEvent && !playerStuck) {
			direction = RIGHT;
			animationSet = 1;
			moving = true;
			background_x += SPRITE_SIZE;		//Player sprite is 8x8 so move the background X position by +8
			player_x++;				//Increase player X position by 1
	}
	
	if (!moving) {
		animationSet = 0;
		player_tid = animOffset ;
	}
	
	//===========================================================================================
	// SELECT is pressed, go to gameState 2 (Pause the game)
	//===========================================================================================
	
	if(key_hit(KEY_SELECT)) {
		gameState = 2;
		isPaused = true;
			
		if(isPaused) {
			Clear();
			REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;//disable background 1
		}
	}
	
	//===========================================================================================
	// Update the Event position
	//===========================================================================================
	
	//We have to update the other game objects positions depending on user input
	//And render them to the screen, minus the background_x & background_y
	//And store the new x and y in another variable (current_x & current_y)
	//We use this X and Y to draw our actual game object in the Update() function!
	
	int e;
	
	//Loop through our game objects (events)
	for(e = 0; e < numEvents; e++) {
		events[e].current_x = events[e].event_x - background_x;
		events[e].current_y = events[e].event_y - background_y;
	}
}



void Guide() {

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;
	Clear();	
	tte_write("#{P:10,10}");
	tte_write("DARK HEARTS INSTRUCTIONS");
	tte_write("#{P:10,30}");
	tte_write("You are stuck in a maze.\n Your objective is to collect\n all the hearts before the\n time runs out!\n\n Each heart will generate a\n random event. Be careful!\n Some hearts are bad! You\n only have 90 seconds to\n collect them all - good luck!");
		
	tte_write("#{P:10,120}");
	tte_write("Press the arrow keys to move around.");
	
	tte_write("#{P:10,150}");
	tte_write("Created by Kim Kane");
}


void LavaAnimation() {

	SCR_ENTRY *animation = lava;
	
	int i;
	int j;
	
	int num = (MAP_WIDTH / 2) + TILE_SIZE;

	for (i = 0; i < num; i++) {	
		for(j= 0; j < 3; j++) {
			*animation++ ^= SE_PALBANK(j);
			}
	}
}



//===========================================================================================
// Function that updates our game objects in different positions every time a new game starts
//===========================================================================================
void randPosition() {
	
	int e;
	
	//Loop through our Event objects
	for (e = 0; e < numEvents; e++) {
	
		int i = rand() % 16;		//A variable that spawns a random integer between 0-16
		
		while (hasSpawned[i]) {		//While hasSpawned bool is false
			i = rand() % 16;		//Generate objects at random positions
		}
		
		hasSpawned[i] = true;		//Then set them to true, so they don't overlap
		
		SpawnLocation(&events[e], i);	//Call our spawnLocation function
	}
	
	int j;
					
		for (j = 0; j < 16; j++) {
			hasSpawned[j] = false;
		}
}



//===========================================================================================
// Main - Call all of our Functions
//===========================================================================================
int main() {	

	//Enable ISR switchboard and VBlank interrupt
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	
	LoadIntoMemory();
	
	srand(time(0));
	
	while(1) {

		switch (gameState) {
		
			//===========================================================================================
			// Case 0 - gameState 0 - Menu Screen
			//===========================================================================================
			case 0:
						
				//Begin an infinite loop until we get user input	
				while(1) {
					
					VBlankIntrWait();
					key_poll();
				
					if (menuState == 2 || menuState < 0) {
						menuState = 0;
					}
				
					LavaAnimation();
					
					tte_write("#{P:82,40}");
					tte_write("#{cx:0x6000}DARK HEARTS");
					
					tte_write("#{P:100,70}");
					tte_write("#{cx:0x6000}*Start*");
					
					tte_write("#{P:100,80}");
					tte_write("#{cx:0x6000}*Guide*");
					
					if (menuState == 0){
						tte_write("#{P:100,70}");
						tte_write("#{cx:0x5000}*Start*");
					}
					
					if (menuState == 1){
						tte_write("#{P:100,80}");
						tte_write("#{cx:0x5000}*Guide*");
					}
					
					if(key_hit(KEY_UP)){
						menuState --;
						Clear();
					}
					
					if(key_hit(KEY_DOWN)){
						menuState ++;//start the game
						Clear();
					}
					
					if(key_hit(KEY_START) && menuState == 0) {
						gameState = 1;			//Start the Game - set gameState to 1		
						Init();					//Run our Init() function
						Clear();
						break;					//Leave infinite loop
					}
					
					if(key_hit(KEY_START) && menuState == 1) {	
						gameState = 3;
						Guide();
						break;					//Leave infinite loop
					}
					randPosition(); 			//Generate our random Event positions and types
				}
			break;
			
			//===========================================================================================
			// Case 1 - gameState 1 - Game Screen
			//===========================================================================================
			case 1:
				
				//Begin an infinite loop until player loses game or chooses to exit game
				while(1) {
	
					VBlankIntrWait();
					time_counter++;
					fps++;
					
					if (time_counter > 100) {			//This slows down the FPS so we can see the player move
						time_counter = 0;
						timer--;				//Decrease the score by 1 every 15 frames				//Get user input
					}
					
					if (fps > 15) {
						fps = 0;
						Input();
					}

					//Call our game functions as long as we are within the infinite loop
					Update();
					UpdateFrames();
					Collision();
					Draw();
				
					//If player loses or chooses to exit the game we break out of our game loop
					if (gameState != 1){
						break;
					}
				}
			break;
			
			//===========================================================================================
			// Case 2 - gameState 2 - Pause Screen
			//===========================================================================================
			case 2:
		
				//Begin an infinite loop showing the pause screen until we get player input
				while(1) {
					
					VBlankIntrWait();
					key_poll();
					SwitchOnBackgrounds();
					
					tte_write("#{P:100,54}");
					tte_write("#{cx:0x5000}*PAUSED*");	
					tte_write("#{P:60,74}");
					tte_write("#{cx:0x5000}Press [SELECT] to");	
					tte_write("#{P:75,84}");
					tte_write("#{cx:0x5000}return to game");
					
					tte_write("#{P:60,104}");
					tte_write("#{cx:0x5000}Press [START] to");
					tte_write("#{P:75,114}");
					tte_write("#{cx:0x5000}return to menu");
					
					if(key_hit(KEY_SELECT)) {
						gameState = 1;//start the game
						
						if(isPaused) {
							isPaused = false;
							Unpause();	
							Clear();
						}
						break;//leave infinite loop
					}
					
					if (key_hit(KEY_START)) {
						PositionBackground();
						SwitchOnBackgrounds();
						isPaused = false;
						gameState = 0;
						Clear();
						ResetGame();
							
						break;
					}	
				}
			break;
				
			//===========================================================================================
			// Case 3 - gameState 3 - Guide Screen
			//===========================================================================================
			case 3:
			
				while(1) {
					
					VBlankIntrWait();
					key_poll();
					
					if(key_hit(KEY_SELECT)) {
						gameState = 0;//back to menu
						Clear();
						SwitchOnBackgrounds();
						break;
					}
				}
			break;
			
			//===========================================================================================
			// Case 4 - gameState 4 - Lose Screen
			//===========================================================================================
			case 4:

				VBlankIntrWait();
				key_poll();
				SwitchOnBackgrounds();
				
				tte_write("#{P:90,54}");
				tte_write("#{cx:0x5000}TIME OUT!!");	
				tte_write("#{P:60,74}");
				tte_write("#{cx:0x5000}Press [SELECT] to");	
				tte_write("#{P:70,84}");
				tte_write("#{cx:0x5000}return to menu");	
					
				if(key_hit(KEY_SELECT)) {
					PositionBackground();
					SwitchOnBackgrounds();
					gameState = 0;//back to menu
					gameOver = false;
					Clear();
					ResetGame(); 
					break;
				}
			break;
			//===========================================================================================
			// Case 5 - gameState 5 - Win Screen
			//===========================================================================================
			case 5:

				VBlankIntrWait();
				key_poll();
				SwitchOnBackgrounds();
				
				int finalScore = hearts*timer;
				
				tte_write("#{P:90,54}");
				tte_write("#{cx:0x6000}YOU WON!!");	
				tte_write("#{P:80,74}");
				sprintf(buffer, "#{cx:0x6000}Score: \%d", finalScore);
				tte_write(buffer);
				tte_write("#{P:60,104}");
				tte_write("#{cx:0x6000}Press [SELECT] to");	
				tte_write("#{P:70,114}");
				tte_write("#{cx:0x6000}return to menu");	
					
				if(key_hit(KEY_SELECT)) {
					PositionBackground();
					SwitchOnBackgrounds();
					gameState = 0;//back to menu
					Clear();
					ResetGame();
					break;
				}
			break;
		}
	}
	return 0;	//Return 0, everything ran OK, end of program
}



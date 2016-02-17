#include <SDL2/SDL.h>
#include <SDL/SDL_image.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <array>
#include <sstream>

//compiler args
//g++ sdl.cpp -w -std=c++11 -lSDL2 -lSDL2_image -o sdl

void handler(int signal)
{
	void *array[10];
	size_t size;
	
	size = backtrace(array, 10);
	
	fprintf(stderr, "[ERROR] signal %d:\n", signal);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}

//window dimensions
const int SCREEN_W = 1920;
const int SCREEN_H = 1080;

//function to initialise screen
bool init();

//function to clean memory
void close();

//collision
bool checkCollision(SDL_Rect x, SDL_Rect y);

//rendering window
SDL_Window* window = NULL;
//window renderer
SDL_Renderer* renderer = NULL;
//rectfunc
SDL_Renderer* rend();
//window surface
SDL_Surface* screenSurface = NULL;
//current surf
SDL_Surface* currentSurf = NULL;

bool init()
{
	//init sdl
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL init error\n");	
		return false;
	}
	else
	{
		//create window
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);

		if (window == NULL)
		{
			printf("Window init error\n");
			return false;
		}
		else
		{
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
			{
				printf("renderer fail %s\n", SDL_GetError());
				return false;
			}
			else
			{
				//renderer color init
				
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image fail");
					return false;
				}
				else
				{	//get window surface ??
					screenSurface = SDL_GetWindowSurface(window);
				}
			}
		}
	}
	return true;
}

SDL_Renderer* rend(int x, int y, int w, int h, int r, int g, int b, int a, SDL_Renderer* renderer)
{
	SDL_Rect fillRect; // = (x, y, w, h);
	fillRect.x = x;
	fillRect.y = y;
	fillRect.w = w;
	fillRect.h = h;
	if (0 > SDL_SetRenderDrawColor(renderer, r, g, b, a))
	{
		printf("[ERROR] setrenderdrawcolor failed : %s\n", SDL_GetError());
		return NULL;
	}
	SDL_RenderFillRect(renderer, &fillRect);
	return renderer;
}

void close()
{
	SDL_FreeSurface(currentSurf);
	currentSurf = NULL;
	
	SDL_DestroyRenderer(renderer);
	//destroy window and deallocate screensurf
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	SDL_Quit();
	IMG_Quit();
}

class Player
{
	public:
		//height width jumpheight of player
	  static const int PLAYER_W = 10;
	  static const int PLAYER_H = 17;
	  static const int PLAYER_J = 20;
	  
	  //init
	  Player();
	  
	  //events
	  void handle(SDL_Event& e);
	  
	  //movement
	  void move(SDL_Rect& wall);
	  
	  //show 
	  void render();
	  
	//private:
	  //x, y position
	  float playerX, playerY;
	  //x, y velocity
	  float playerXvel, playerYvel;
	  //player veloxity (magnitude)
	  float playerV = 0.03;
	  //shape to define hitbox
	  SDL_Rect hitbox;
	  //text data
	  std::stringstream data;
	  //jumping check
	  bool isJumping;
	  //jump delta, (used to enforce defined player jump height PLAYER_J)
	  float dJump;
  
};

Player::Player()
{
	playerX = 0;
	playerY = 0;
	playerXvel = 0;
	playerYvel = 0;
	hitbox.w = PLAYER_W;
	hitbox.h = PLAYER_H;
	isJumping = false;
	dJump = 0.0f;
}

void Player::handle(SDL_Event& e)
{
	//KEY HELD DOWN
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//set appropriate velocities
		switch(e.key.keysym.sym)
		{
		  case SDLK_UP: isJumping = true; playerYvel -= playerV; break;
		  case SDLK_DOWN: playerYvel += playerV; break;
		  case SDLK_LEFT: playerXvel -= playerV; break;
		  case SDLK_RIGHT: playerXvel += playerV; break;
		}
	}
	//KEY RELEASED
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch(e.key.keysym.sym)
		{
		  case SDLK_UP: isJumping = false; playerYvel += playerV; break;
		  case SDLK_DOWN: playerYvel -= playerV; break;
		  case SDLK_LEFT: playerXvel += playerV; break;
		  case SDLK_RIGHT: playerXvel -= playerV; break;
		}
	}
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	if((a.y + a.h) <= b.y)
	{
		return false;
	}
	if(a.y >= (b.y + b.h))
	{
		return false;
	}
	if((a.x + a.w) <= b.x)
	{
		return false;
	}
	if(a.x >= (b.x + b.w))
	{
		return false;
	}
	return true;
}
 
void Player::move(SDL_Rect& wall)
{
	//move player by its x velocity
	playerX += playerXvel;
	hitbox.x = playerX;
	
	if (playerX < 0 || (playerX + PLAYER_W > SCREEN_W) || checkCollision(hitbox, wall))
	{
		//stop player on collision/offscreen
		//printf("X HIT");
		playerX -= playerXvel;
		hitbox.x = playerX;
	}
	
	//move player by its y velocity
	playerY += playerYvel;
	hitbox.y = playerY;
	
	playerY += 0.01; //gravity
	hitbox.y = playerY;
	
	
	if (playerY < 0 || (playerY + PLAYER_H > SCREEN_H) || checkCollision(hitbox, wall))
	{
		//stop player on collision//offscreen
		//printf("Y HIT");
		playerY -= playerYvel;
		hitbox.y = playerY;
		
		
		playerY -= 0.01; //cant gravity through the floor m9
		hitbox.y = playerY;
		//isJumping = false;
	}
	
	//jump
	if (isJumping == true)
	{
		printf("TITTER\n");
		//set jumping velocity
		playerYvel += playerV;
		
		//if max jump height not reached
		if (dJump < PLAYER_J)
		{
			printf("RJAM");
			//move player up by velocity
			playerY -= playerYvel;
			hitbox.y = playerY;
			//change delta by jump velocity
			dJump += playerYvel;
			printf("%f\n", dJump);
		}
		//if max jump height reached
		else
		{
			dJump = 0.0f; //reset delta
			isJumping = false; //height reached, stop jumping
			printf("%f\n", dJump);
		}
		//isJumping = false;
// 		while (dJump < PLAYER_J)
// 		{
// 			playerY -= playerYvel;
// 			hitbox.y = playerY;
// 			dJump += playerYvel;
// 		}
// 		playerYvel -= playerV;
// 		isJumping = false;
// 		dJump = 0.0f;
	}
	else
	{
		//playerY += 0.01; 
		//hitbox.y = playerY; //what
	}
}


void Player::render()
{
	SDL_RenderPresent(rend(playerX, playerY, PLAYER_W, PLAYER_H, 0, 0, 255, 255, renderer));
}

int main()
{
	signal(SIGSEGV, handler);
	bool quit = false;

	SDL_Event e;
	
	Player player;
	
	SDL_Rect wall;
	wall.x = 1;
	wall.y = 700;
	wall.w = 1500;
	wall.h = 30;
	
	//init screen
	if (!init())
	{
		printf("init fail\n");
	}

	//game loop
	while (!quit)
	{
		//event loop
		while (SDL_PollEvent(&e))
		{
 			if (e.type == SDL_QUIT)
 			{
 				quit = true;
 			}
 			player.handle(e);
		}
		player.move(wall);
		
		//clear screen
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		
		//wall
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderDrawRect(renderer, &wall);
		
		player.render();
		
		if (renderer == NULL)
		{
			printf("[ERROR] renderer is null%s\n", SDL_GetError());
			return NULL;
		}
		
		
	} //end of game loop
	close();
	return NULL;
}


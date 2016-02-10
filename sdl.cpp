#include <SDL2/SDL.h>
#include <execinfo.h>
#include <signal.h>
#include "SDL/SDL_image.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <array>

//g++ sdl.cpp -w -lSDL2 -lSDL2_image -o sdl
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

enum keyPressSurfaces
{
	KB_DEFAULT,	//0
	KB_UP,		//1
	KB_DOWN,	//2
	KB_LEFT,	//3
	KB_RIGHT,	//4
	KB_TOTAL	//5
};

bool init();

void close();

//rendering window
SDL_Window* window = NULL;
//window renderer
SDL_Renderer* renderer = NULL;
//rectfunc
SDL_Renderer* rend();
//window surface
SDL_Surface* screenSurface = NULL;
SDL_Surface* loadImg(std::string path);
//keypress images
SDL_Surface* wKeyPressSurfaces[ KB_TOTAL ];
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

SDL_Surface* loadImg(std::string path) //HOW TO OPTIMISE W/O SEGFAULT?!
{	
	//SDL_Surface* optIMG = NULL;
	
	SDL_Surface* loadedIMG = SDL_LoadBMP(path.c_str());
	
	if (loadedIMG == NULL)
	{
		printf("img fail too much ass: %s FAIL %s\n", path.c_str(), IMG_GetError());
	}
	//else
	//{
	//optIMG = SDL_ConvertSurface(SDL_LoadBMP(path.c_str()), screenSurface->format, NULL);
	//	if (optIMG == NULL)
	//	{
	//		printf( "img opt fail too many ass: %s FAIL %s\n", path.c_str(), IMG_GetError());
	//	}
		//SDL_FreeSurface(loadedIMG);
	//}

	return loadedIMG;
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
}

class Player
{
	public:
	  static const int PLAYER_W = 50;
	  static const int PLAYER_H = 100;
	  
	  static const int PLAYER_V = 1;
	  
	  //init
	  Player();
	  
	  //events
	  void handle(SDL_Event& e);
	  
	  //movement
	  void move();
	  
	  //show 
	  void render();
	  
	//private:
	  int playerX, playerY;
	  int playerXvel, playerYvel;
  
};

Player::Player()
{
	playerX = 0;
	playerY = 0;
	playerXvel = 0;
	playerYvel = 0;
}

void Player::handle(SDL_Event& e)
{
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch(e.key.keysym.sym)
		{
		  case SDLK_UP: playerYvel -= PLAYER_V; break;
		  case SDLK_DOWN: playerYvel += PLAYER_V; break;
		  case SDLK_LEFT: playerXvel -= PLAYER_V; break;
		  case SDLK_RIGHT: playerXvel += PLAYER_V; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch(e.key.keysym.sym)
		{
		  case SDLK_UP: playerYvel += PLAYER_V; break;
		  case SDLK_DOWN: playerYvel -= PLAYER_V; break;
		  case SDLK_LEFT: playerXvel += PLAYER_V; break;
		  case SDLK_RIGHT: playerXvel -= PLAYER_V; break;
		}
	}
}

void Player::move()
{
	playerX += playerXvel;
	
	if (playerX < 0 || (playerX + PLAYER_W > SCREEN_W))
	{
		playerX -= playerXvel;
	}
	
	playerY += playerYvel;
	
	if (playerY < 0 || (playerY + PLAYER_H > SCREEN_H))
	{
		playerY -= playerYvel;
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
		
		player.move();
		
		//clear screen		
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		
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















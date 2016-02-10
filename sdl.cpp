#include <SDL2/SDL.h>
#include "SDL/SDL_image.h"
#include <stdio.h>
#include <string>

//g++ sdl.cpp -w -lSDL2 -lSDL2_image -o sdl

//window dimensions
const int SCREEN_W = 1920;
const int SCREEN_H = 1080;

const int PLAYER_W = 50;
const int PLAYER_H = 100;

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
bool loadAss();
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

bool loadAss() //dumb function remove/redo
{
	bool complete = true;

	wKeyPressSurfaces[ KB_DEFAULT ] = loadImg("placehold.bmp");
	if (wKeyPressSurfaces[ KB_DEFAULT ] == NULL)
	{
		printf("failed to load\n");
		complete = false;
	}

	wKeyPressSurfaces[ KB_UP ] = loadImg("placehold.bmp");
	if (wKeyPressSurfaces[ KB_UP ] == NULL)
	{
		printf("failed to load\n");
		complete = false;
	}

	wKeyPressSurfaces[ KB_DOWN ] = loadImg("placehold.bmp");
	if (wKeyPressSurfaces[ KB_DOWN ] == NULL)
	{
		printf("failed to load\n");
		complete = false;
	}

	wKeyPressSurfaces[ KB_LEFT ] = loadImg("placehold.bmp");
	if (wKeyPressSurfaces[ KB_LEFT ] == NULL)
	{
		printf("failed to load\n");
		complete = false;
	}

	wKeyPressSurfaces[ KB_RIGHT ] = loadImg("placehold.bmp");
	if (wKeyPressSurfaces[ KB_RIGHT ] == NULL)
	{
		printf("failed to load\n");
		complete = false;
	}

	printf("loaded\n");

	return complete;
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


int main()
{
	printf("please master");
	bool quit = false;

	SDL_Event e;
	
	int playerX = 0;
	int playerY = 0;
	
	bool keysHeld[500] = {false};

	//loadAss();
	printf("please");
	currentSurf = wKeyPressSurfaces[ KB_DEFAULT ];

	//init screen
	if (!init())
	{
		printf("init fail\n");
	}

	//game loop
	while (!quit)
	{
		printf("tester");
		//event loop
		while (SDL_PollEvent(&e))
		{
			//printf("hey");
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		
			if (e.type == SDL_KEYDOWN)
			{
				keysHeld[e.key.keysym.sym] = true;
			}
			if (e.type == SDL_KEYUP)
			{
				keysHeld[e.key.keysym.sym] = false;
			}
			
			if (keysHeld[SDLK_ESCAPE])
			{
				quit = true;
			}
			if (keysHeld[SDLK_UP])
			{
				playerY -= 5;
			}
			if (keysHeld[SDLK_DOWN])
			{
				playerY += 5;
			}
			if (keysHeld[SDLK_LEFT])
			{
				playerX -= 5;
			}
			if (keysHeld[SDLK_RIGHT])
			{
				playerX += 5;
			}
// 				switch (e.key.keysym.sym)
// 				{
// 					case SDLK_UP:
// 						//currentSurf = wKeyPressSurfaces[ KB_UP ];
// 						playerY -= 5;
// 						break;
// 	
// 					case SDLK_DOWN:
// 						//currentSurf = wKeyPressSurfaces[ KB_DOWN ];
// 						playerY += 5;
// 						break;
// 	
// 					case SDLK_LEFT:
// 						//currentSurf = wKeyPressSurfaces[ KB_LEFT ];
// 						playerX -= 5;
// 						break;
// 	
// 					case SDLK_RIGHT:
// 						//currentSurf = wKeyPressSurfaces[ KB_RIGHT ];
// 						playerX += 5;
// 						break;
// 	
// 					default:
// 						//currentSurf = wKeyPressSurfaces[ KB_DEFAULT ];
// 						break;
// 				}
			//}
		}
		
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		//SDL_Rect fillRect = {SCREEN_W / 4, SCREEN_H / 4, SCREEN_W / 2, SCREEN_H / 2};

		if (renderer == NULL)
		{
			printf("[ERROR] renderer is null%s\n", SDL_GetError());
			return NULL;
		}
		
		SDL_RenderPresent(rend(playerX, playerY, PLAYER_W, PLAYER_H, 0, 0, 255, 255, renderer));
		
		//if (0 > SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255))
		//{
		//	printf("[ERROR] setrenderdrawcolor failed : %s\n", SDL_GetError());
		//	return NULL;
		//}

		//SDL_RenderFillRect(renderer, &fillRect);
		//SDL_RenderPresent(renderer);
	} //end of game loop
	close();
	return NULL;
}















#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <array>
#include <sstream>
#include <ctime>
#include <cstdlib>
using namespace std; //whatever dude
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
const int SCREEN_W = 800;
const int SCREEN_H = 600;
//function to initialise screen
bool init();

//function to clean memory
void close();

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
			renderer = SDL_CreateRenderer(window, -1, 0);
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
	printf("fillrects: x:%d y:%d w:%d h:%d\n", fillRect.x, fillRect.y, fillRect.w, fillRect.h);
	if (0 > SDL_SetRenderDrawColor(renderer, r, g, b, a))
	{
		printf("[ERROR] setrezzznderdrawcolor failed : %s\n", SDL_GetError());
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

//void Player::render()
//{
//	SDL_RenderPresent(rend(playerX, playerY, PLAYER_W, PLAYER_H, 0, 0, 255, 255, renderer));
//}

int main()
{
	signal(SIGSEGV, handler);
	bool quit = false;
	
	char buf[100];
	getcwd(buf,sizeof(buf));
	int r1, r2, r3;
	srand(time(0));
	SDL_Event e;
	
	//init screen
	if (!init())
	{
		printf("init fail\n");
	}

	//game loop
	while (!quit)
	{
		//print cwd
		printf("#> %s\n", buf);
		//event loop		
		while (SDL_PollEvent(&e))
		{
 			if (e.type == SDL_QUIT)
 			{
 				quit = true;
 			} 			
		}
		
		//clear screen
		//SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		//SDL_RenderClear(renderer);
		
		//iterate thru pixels
		for (int x = 0; x < SCREEN_W; x++)
		{
			for (int y = 0; y < SCREEN_H; y++)
			{
				
				r1 = rand()%256;
				r2 = rand()%256;
				r3 = rand()%256;
				SDL_SetRenderDrawColor(renderer, r1, r2, r3, 0xFF);
				SDL_RenderDrawPoint(renderer, x, y);
			}			
		}

		SDL_RenderPresent(renderer);
		
		if (renderer == NULL)
		{
			printf("[ERROR] renderer is null%s\n", SDL_GetError());
			return NULL;
		}
	} //end of game loop
	close();
	return NULL;
}


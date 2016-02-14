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

// TT_Font *freeFont = NULL;
// 
// textureWrap textTexture;

bool init();
void close();
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
				
// 				if (TTF_Init() == -1)
// 				{
// 					printf("SDL_TTF fail: %s\n", TTF_GetError();
// 				}
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
// 	textTexture.free();
// 	
// 	TTF_CloseFont(freeFont);
// 	freeFont = NULL;
	
	SDL_FreeSurface(currentSurf);
	currentSurf = NULL;
	
	SDL_DestroyRenderer(renderer);
	//destroy window and deallocate screensurf
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	SDL_Quit();
	IMG_Quit();
// 	TTF_Quit();
}

// class textureWrap
// {
// 	public:
// 		textureWrap();
// 		
// 		~textureWrap();
// 		
// 		bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
// 		
// 		void free();
// 		
// 		void setColor(Uint8 red, Uint8 green, Uint8 blue);
// 		
// 		void setBlendMode(SDL_BlendMode blending);
// 		
// 		void setAlpha(Uint8 alpha);
// 		
// 		void render(int x, int y, SDL_Rect* clip == NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip = SDL_FLIP_NONE);
// 		
// 		int tW, tH;
// 		
// 		SDL_Texture* texture;
// 		
// 		
// 		
// };

// textureWrap::textureWrap()
// {
// 	texture = NULL;
// 	tW = 0;
// 	tH = 0;
// }

// textureWrap::~textureWrap()
// {
// 	free();
// }

// bool textureWrap::loadFromFile(std::string path)
// {
// 	free();
// 	
// 	SDL_Texture* newTexture = NULL;
// 	
// 	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
// 	
// 	if (loadedSurface == NULL)
// 	{
// 		printf("img load fail %s\n", IMG_GetError());
// 	}
// 	else
// 	{
// 		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
// 		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
// 		if (newTexture == NULL)
// 		{
// 			printf("tex fail: %s\n", SDL_GetError());
// 		}
// 		else
// 		{
// 			tW = loadedSurface->w;
// 			tH = loadedSurface->h;
// 		}
// 		
// 		SDL_FreeSurface(loadedSurface);
// 	}
// 	
// 	texture = newTexture;
// 	return texture != NULL;
// }

// void textureWrap::free()
// {
// 	if (texture != NULL)
// 	{
// 		SDL_DestroyTexture(texture);
// 		texture = NULL;
// 		tW = 0;
// 		tH = 0;
// 	}
// }

// void textureWrap::render(int x, int y)
// {
// 	SDL_Rect renderQuad = {x, y, tW, tH};
// 	SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
// }

// bool textureWrap::loadFromRenderedText(std::string textureText, SDL_Color textColor)
// {
// 	free();
// 	
// 	SDL_Surface* textSurface = TTF_RenderText_Solid(freeFont, textureText.c_str(), textColor);
// 	if (textSurface == NULL)
// 	{
// 		printf("text fuck: %s\n", TTF_GetError());
// 	}
// 	else
// 	{
// 		texture = SDL_CreateTextureFromSurface(renderer, textSurface);
// 		if (texture == NULL)
// 		{
// 			printf("tex rend fuckkk: %s\n", SDL_GetError());
// 		}
// 		else
// 		{
// 			tW = textSurface->w;
// 			tH = textSurface->h;
// 		}
// 		SDL_FreeSurface(textSurface);
// 	}
// 	return texture != NULL;
// }

class Player
{
	public:
	  static const int PLAYER_W = 10;
	  static const int PLAYER_H = 17;
	  
	  //init
	  Player();
	  
	  //events
	  void handle(SDL_Event& e);
	  
	  //movement
	  void move(SDL_Rect& wall);
	  
	  //show 
	  void render();
	  
	//private:
	  float playerX, playerY;
	  float playerXvel, playerYvel;
	  float playerV = 0.01;
	  SDL_Rect hitbox;
	  std::stringstream data;
  
};

Player::Player()
{
	playerX = 0;
	playerY = 0;
	playerXvel = 0;
	playerYvel = 0;
	hitbox.w = PLAYER_W;
	hitbox.h = PLAYER_H;
}

void Player::handle(SDL_Event& e)
{
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch(e.key.keysym.sym)
		{
		  case SDLK_UP: playerYvel -= playerV; break;
		  case SDLK_DOWN: playerYvel += playerV; break;
		  case SDLK_LEFT: playerXvel -= playerV; break;
		  case SDLK_RIGHT: playerXvel += playerV; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch(e.key.keysym.sym)
		{
		  case SDLK_UP: playerYvel += playerV; break;
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
	playerX += playerXvel;
	hitbox.x = playerX;
	
	if (playerX < 0 || (playerX + PLAYER_W > SCREEN_W) || checkCollision(hitbox, wall))
	{
		printf("X HIT");
		playerX -= playerXvel;
		hitbox.x = playerX;
	}
	
	playerY += playerYvel;
	hitbox.y = playerY;
	playerY += 0.01;
	hitbox.y = playerY;
	
	if (playerY < 0 || (playerY + PLAYER_H > SCREEN_H) || checkCollision(hitbox, wall))
	{
		printf("Y HIT");
		playerY -= playerYvel;
		hitbox.y = playerY;
		playerY -= 0.01;
		hitbox.y = playerY;
	}
}


void Player::render()
{
	SDL_RenderPresent(rend(playerX, playerY, PLAYER_W, PLAYER_H, 0, 0, 255, 255, renderer));
// 	data << "playerXvel: " << playerXvel << " playerYvel: " << playerYvel << "\n" << "playerX: " << playerX << "playerY: " << playerY;
// 	freeFont = TTF_OpenFont("freefont.ttf", 28);
// 	if (freeFont == NULL)
// 	{
// 		printf("FONT FAIL %s\n", TTF_GetError());
// 	}
// 	else
// 	{
// 		SDL_Color textColor = {0, 0, 0};
// 		if (!textTexture.loadFromRenderedText(data, textColor))
// 		{
// 			printf("Fail\n");
// 		}
// 	}
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
		
// 		textTexture.render(1, 1);

		if (renderer == NULL)
		{
			printf("[ERROR] renderer is null%s\n", SDL_GetError());
			return NULL;
		}
		
		
	} //end of game loop
	close();
	return NULL;
}


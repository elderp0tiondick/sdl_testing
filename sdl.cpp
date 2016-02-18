#include <SDL2/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <array>
#include <sstream>
using namespace std; //whatever dude
//compiler args
//g++ sdl.cpp -w -std=c++11 -lSDL2_TTF -lSDL2 -lSDL2_image -o sdl

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
const float GRAV = 0.02;

float gravity = 0;



TTF_Font *freeFont = NULL;



//function to initialise screen
bool init();

//function to clean memory
void close();

//collision
bool checkCollision(SDL_Rect a, SDL_Rect b, SDL_Rect c);

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

class Time
{
	public:
		unsigned int get(int unit);
		Time();
		~Time();
		
	private:
		unsigned int time;
		unsigned int past;
	
};

Time::Time()
{
	time = 0;
	past = 0;
}

Time::~Time()
{
	//fucking deconstruct time you cooked cunt
	time = NULL;
	past = NULL;
}

unsigned int Time::get(int unit)
{
	time = SDL_GetTicks();
	
	switch(unit)
	{
		case 0: return time; break; //0 = return in ms
		case 1: return time/1000; break; //1 = return in s
		//case 2: return past; break; //2 = return past
	}
}



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
				
				if (TTF_Init() == -1)
				{
					printf("SDL_TTF fail: %s\n", TTF_GetError());
				}
			}
		}
	}
	return true;
}

class TextureWrap
{
	public:
		TextureWrap();
		
		~TextureWrap();
		
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
		
		void free();
		
		void setColor(Uint8 red, Uint8 green, Uint8 blue);
		
		void setBlendMode(SDL_BlendMode blending);
		
		void setAlpha(Uint8 alpha);
		
		void render(int x, int y);
		
		int tW, tH;
		
		bool loadFromFile(std::string path);
		
		SDL_Texture* texture;
		
		
		
};

TextureWrap::TextureWrap()
{
	texture = NULL;
	tW = 0;
	tH = 0;
}

TextureWrap::~TextureWrap()
{
	free();
}

bool TextureWrap::loadFromFile(std::string path)
{
	free();
	
	SDL_Texture* newTexture = NULL;
	
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	
	if (loadedSurface == NULL)
	{
		printf("img load fail %s\n", IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("tex fail: %s\n", SDL_GetError());
		}
		else
		{
			tW = loadedSurface->w;
			tH = loadedSurface->h;
		}
		
		SDL_FreeSurface(loadedSurface);
	}
	
	texture = newTexture;
	return texture != NULL;
}

void TextureWrap::free()
{
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
		tW = 0;
		tH = 0;
	}
}

void TextureWrap::render(int x, int y)
{
	SDL_Rect renderQuad = {x, y, tW, tH};
	SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
}

bool TextureWrap::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	free();
	
	SDL_Surface* textSurface = TTF_RenderText_Solid(freeFont, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("text fuck: %s\n", TTF_GetError());
	}
	else
	{
		texture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (texture == NULL)
		{
			printf("tex rend fuckkk: %s\n", SDL_GetError());
		}
		else
		{
			tW = textSurface->w;
			tH = textSurface->h;
		}
		SDL_FreeSurface(textSurface);
	}
	return texture != NULL;
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
	
	TTF_CloseFont(freeFont);
	freeFont = NULL;
	
	SDL_DestroyRenderer(renderer);
	//destroy window and deallocate screensurf
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
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
	  void move(SDL_Rect& wall, SDL_Rect& plat, Time worldTime, unsigned int oldTime);
	  
	  //show 
	  void render();
	  
	//private:
	  //x, y position
	  float playerX, playerY;
	  //x, y velocity
	  float playerXvel, playerYvel;
	  //player veloxity (magnitude)
	  float playerV = 0.02;//ms-1
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
	playerX = 20;
	playerY = 20;
	playerXvel = 0; //ms-1
	playerYvel = 0; //ms-1
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

bool checkCollision(SDL_Rect a, SDL_Rect b, SDL_Rect c)
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
	if((a.y + a.h) <= c.y)
	{
		return false;
	}
	if(a.y >= (c.y + c.h))
	{
		return false;
	}
	if((a.x + a.w) <= c.x)
	{
		return false;
	}
	if(a.x >= (c.x + c.w))
	{
		return false;
	}
	return true;
}
 
void Player::move(SDL_Rect& wall, SDL_Rect& plat, Time worldTime, unsigned int oldTime) //hack
{
	gravity = GRAV*((worldTime.get(0))/1000);
	//move player by its x velocity
	if ((playerXvel < 2) || (playerXvel > -2))
	{
		playerX += playerXvel;
	}
	else
	{
		playerX -= playerXvel;
	}
	
	hitbox.x = playerX;
	printf("playerX: %f\nplayerXvel: %f\n", playerX, playerXvel);
	
	if (playerX < 0 || (playerX + PLAYER_W > SCREEN_W) || checkCollision(hitbox, wall, plat))
	{
		//stop player on collision/offscreen
		playerX -= playerXvel;
		hitbox.x = playerX;
		printf("ARE WE DETECTING COLLISION?!\n");
	}

	
	if ((playerYvel < 2) || (playerYvel > -2))
	{
		playerY += gravity;
		playerY += playerYvel;
	}
	else
	{
		playerY += gravity;
		playerY -= playerYvel;
	}
	
	
	
	hitbox.y = playerY;
	printf("playerY: %f\nplayerYvel: %f\n", playerY, playerYvel);
	
	if (playerY < 0 || (playerY + PLAYER_H) > (SCREEN_H) || checkCollision(hitbox, wall, plat))
	{
		//stop player on collision//offscreen
		playerY -= gravity;
		playerY -= playerYvel;
		
		hitbox.y = playerY;
		printf("ARE WE DETECTING 2222 COLLISION?!\n");
	}
	
	if (isJumping == true)
	{
		if (dJump < 60)
		{
			dJump += 10 + gravity;
			playerY -= 10 + gravity;
			hitbox.y = playerY;
		}
		else 
		{
			dJump -= 10 + gravity;
			playerY += 10 + gravity;
			hitbox.y = playerY;
		}
	}
	else
	{
		dJump = 0;
		isJumping = false;
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
	
	char buf[100];
	getcwd(buf,sizeof(buf));
	

	SDL_Event e;
	
	Player player;
	
	TextureWrap textTexture;
	
	stringstream data (stringstream::in | stringstream::out);
	
	//std::string data = "";
	
	Time worldTime;
	unsigned int pastTime;
	
	SDL_Rect wall;
	wall.x = 1;
	wall.y = 700;
	wall.w = 1500;
	wall.h = 30;
	
	SDL_Rect plat;
	plat.x = 700;
	plat.y = 670;
	plat.w = 800;
	plat.h = 30;
	
	freeFont = TTF_OpenFont("/home/dean/dev/sdl_testing/fonts/font.ttf", 6);
	
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
		pastTime = worldTime.get(0);
		//event loop
		
		printf("time(ms) : %u\n", worldTime.get(0));
		
		while (SDL_PollEvent(&e))
		{
 			if (e.type == SDL_QUIT)
 			{
 				quit = true;
 			}
 			player.handle(e);
		}
		player.move(wall, plat, worldTime, pastTime);
		
		//clear screen
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		
		//wall
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderDrawRect(renderer, &wall);
		
		//plat
		SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_RenderDrawRect(renderer, &plat);
		
		player.render();
		
		data << "playerXvel: " << player.playerXvel << " playerYvel: " << player.playerYvel << "\n" << "playerX: " << player.playerX << "playerY: " << player.playerY;
		
		
		if (freeFont == NULL)
		{
			printf("FONT FAIL %s\n", TTF_GetError());
		}
		else
		{
			SDL_Color textColor = {0, 0, 0};
			if (!textTexture.loadFromRenderedText(data.str(), textColor))
			{
				printf("Fail: %s | %s\n", SDL_GetError(), TTF_GetError());
			}
		}
		
		if (renderer == NULL)
		{
			printf("[ERROR] renderer is null%s\n", SDL_GetError());
			return NULL;
		}
	} //end of game loop
	close();
	return NULL;
}


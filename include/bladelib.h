// LIBRARY PROGRAMMED BY DIODOGHATER
// Github: github.com/DioDogHater
// ANYONE TRYING TO COPY MY CODE SHALT BE DECLARED AS A STEALING LOSER
// GET A BRAIN DUMBASS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#undef main

#ifndef BLADELIB_H
#define BLADELIB_H

// Custom types
#define Vec2 SDL_Point

// Preprocessor constants
#define MAP_ASSET_COUNT 122
#define MAP_GRASS_END 32
#define MAP_GRASS_STONE_END 64
#define MAP_STONE_FLOOR_END 115
#define MAP_NULL 255
#define GRID_SIZE 48
#define HGRID_SIZE 24

// Structures, Unions and Enums
typedef struct {
	float x; float y;
}Vector2;

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	Vec2 size;
	Vec2 hsize;
}Window;

typedef struct {
	Window* win;
	Vector2 pos;
}Camera;

typedef struct {
	SDL_Texture* t;
	Vec2 size;
}Texture;

typedef struct {
	TTF_Font* font;
	SDL_Texture* t;
	Vec2 size;
}Text;

typedef struct { // a 16x16 group of grid snapped squares
	uint8_t blocks[256]; // array holding all blocks' id
	uint8_t bg[256]; // array holding all background's id (floor)
	uint8_t colls[256]; // array holding all of the positions of the colliders in the chunk
	int16_t x; // position of the chunk's upper left corner (in jumps of 16 grid sizes)
	int16_t y;
}MapChunk;

typedef struct { // a simple struct that holds the chunks of a map
	MapChunk* chunks; // array that holds the chunks
	uint8_t chunkSize; // size of the chunks array
}Map;

typedef struct {
	Texture* t;
	SDL_Rect clip;
}Asset;

typedef struct {
	uint8_t fps;
	uint8_t currFrame;
	uint8_t frameCount;
	Asset* frames;
}Animation;

// HERE IS HOW THE FILE HANDLING WORKS -> we use .bin (binary files)
// Writing:
// 	We parse through every map chunk then write that struct in the file
// Reading: 
// 	We parse through singular map chunks until we reach the end of the file (by getting the size of the file with fseek and ftell)
// ----

// Math functions
#define square(x) (x)*(x)

// General functions
bool initEverything(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		printf("SDL_Init error: %s\n",SDL_GetError());
		return false;
	}if(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0){
		printf("IMG_Init error: %s\n",IMG_GetError());
		return false;
	}if(TTF_Init() < 0){
		printf("TTF_Init error: %s\n",TTF_GetError());
		return false;
	}return true;
}
void quitEverything(){
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

// Vector2 functions
Vector2 addVector2(Vector2 a, Vector2 b){return (Vector2){a.x+b.x,a.y+b.y};}
Vector2 subVector2(Vector2 a, Vector2 b){return (Vector2){a.x-b.x,a.y-b.y};}
Vector2 multVector2(Vector2 a, Vector2 b){return (Vector2){a.x*b.x,a.y*b.y};}
Vector2 scaleVector2(Vector2 vec, float scalar){return (Vector2){vec.x*scalar,vec.y*scalar};}
Vector2 divVector2(Vector2 a, Vector2 b){return (Vector2){a.x/b.x,a.y/b.y};}
Vector2 divScaleVector2(Vector2 vec, float scalar){return (Vector2){vec.x/scalar,vec.y/scalar};}
float Vector2_magnitude(Vector2 vec){return (float)sqrt(square(vec.x)+square(vec.y));}
float Vector2_distance(Vector2 a, Vector2 b){return (float)sqrt(square(a.x-b.x)+square(a.y-b.y));}
float Vector2_distance_square(Vector2 a, Vector2 b){return square(a.x-b.x)+square(a.y-b.y);}
Vector2 Vector2_diff(Vector2 a, Vector2 b){return (Vector2){(float)fabs(a.x-b.x),(float)fabs(a.y-b.y)};}
Vector2 Vector2_normalized(Vector2 vec){if(Vector2_magnitude(vec) == 0) return (Vector2){0.0f,0.0f};return divScaleVector2(vec,Vector2_magnitude(vec));}
void Vector2_print(Vector2 vec){printf("%.5f, %.5f\n",vec.x,vec.y);}
// ---

// Window functions
bool Window_init(Window* window, char* caption, Vec2 size, Uint32 flags){
	window->size = size;
	window->hsize = (Vec2){size.x/2,size.y/2};
	SDL_Window* nwin = SDL_CreateWindow(caption,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,size.x,size.y,flags);
	if(nwin == NULL){
		printf("Window creation error: %s\n",SDL_GetError());
		return false;
	}window->window = nwin;
	SDL_Renderer* nrend = SDL_CreateRenderer(nwin,-1,SDL_RENDERER_ACCELERATED);
	if(nrend == NULL){
		printf("Renderer creation error: %s\n",SDL_GetError());
		return false;
	}window->renderer = nrend;
	return true;
}
void Window_destroy(Window* window){SDL_DestroyRenderer(window->renderer);SDL_DestroyWindow(window->window);window->renderer=NULL;window->window=NULL;}
void Window_setColor(Window* window, SDL_Color color){SDL_SetRenderDrawColor(window->renderer,color.r,color.g,color.b,color.a);}
void Window_clear(Window* window){Window_setColor(window,(SDL_Color){0,0,0,255});SDL_RenderClear(window->renderer);}
void Window_update(Window* window){SDL_RenderPresent(window->renderer);}
// ---

// Texture functions
void Texture_free(Texture* texture){
	SDL_DestroyTexture(texture->t);
	texture->size = (Vec2){0,0};
}
bool Texture_load(Window* win, Texture* texture, char* path){
	SDL_Surface* loadedSurface = IMG_Load(path);
	if(loadedSurface == NULL){
		printf("IMG_Load error: %s\n",IMG_GetError());
		return false;
	}SDL_Texture* newTexture = SDL_CreateTextureFromSurface(win->renderer,loadedSurface);
	SDL_FreeSurface(loadedSurface);
	if(newTexture == NULL){
		printf("SDL_CreateTextureFromSurface error: %s\n",SDL_GetError());
		return false;
	}texture->t = newTexture;
	SDL_QueryTexture(newTexture,NULL,NULL,&texture->size.x,&texture->size.y);
	printf("Successfully loaded texture %s!\n",path);
	return true;
}
void Texture_render(Window* win, Texture* t, SDL_Rect r){SDL_RenderCopy(win->renderer,t->t,NULL,&r);}
void Texture_render_clip(Window* win, Texture* t, SDL_Rect r, SDL_Rect c){SDL_RenderCopy(win->renderer,t->t,&c,&r);}
void Texture_renderEx(Window* win, Texture* t, SDL_Rect r, double angle, Vec2* center, SDL_RendererFlip flip){
	SDL_RenderCopyEx(win->renderer,t->t,NULL,&r,angle,center,flip);
}
void Texture_render_clipEx(Window* win, Texture* t, SDL_Rect r, SDL_Rect c, double angle, Vec2* center, SDL_RendererFlip flip){
	SDL_RenderCopyEx(win->renderer,t->t,&c,&r,angle,center,flip);
}
void Texture_render_scaled(Window* win, Texture* t, Vec2 pos, Vec2 scale){
	SDL_Rect r=(SDL_Rect){pos.x,pos.y,scale.x*t->size.x,scale.y*t->size.y}; Texture_render(win,t,r);
}
// ---

// Asset functions
void Asset_render(Window* win, Asset asset, SDL_Rect r){
	Texture_render_clip(win,asset.t,r,asset.clip);
}
void Asset_renderEx(Window* win, Asset asset, SDL_Rect r, SDL_RendererFlip flip){
	Texture_render_clipEx(win,asset.t,r,asset.clip,0.0d,NULL,flip);
}
// ---

// Text functions
TTF_Font* Text_loadFont(char* path, int size){
	TTF_Font* newFont=TTF_OpenFont(path,size);
	if(newFont == NULL) printf("TTF_OpenFont error: %s\n",TTF_GetError());
	return newFont;
}
void Text_free(Text* text){
	TTF_CloseFont(text->font);
	SDL_DestroyTexture(text->t);
	text->size = (Vec2){0,0};
}
bool Text_load(Window* win, Text* text, char* contents, SDL_Color color){
	SDL_Surface* loadedSurface = TTF_RenderText_Solid(text->font,contents,color);
	if(loadedSurface == NULL){
		printf("TTF_RenderText_Solid error: %s\n",TTF_GetError());
		return false;
	}SDL_Texture* newTexture = SDL_CreateTextureFromSurface(win->renderer,loadedSurface);
	SDL_FreeSurface(loadedSurface);
	if(newTexture == NULL){
		printf("SDL_CreateTextureFromSurface error: %s\n",SDL_GetError());
		return false;
	}text->t = newTexture;
	SDL_QueryTexture(newTexture,NULL,NULL,&text->size.x,&text->size.y);
	return true;
}
void Text_render(Window* win, Text* t, SDL_Rect r){SDL_RenderCopy(win->renderer,t->t,NULL,&r);}
void Text_render_clip(Window* win, Text* t, SDL_Rect c, SDL_Rect r){SDL_RenderCopy(win->renderer,t->t,&c,&r);}
void Text_renderEx(Window* win, Text* t, SDL_Rect r, double angle, Vec2* center, SDL_RendererFlip flip){
	SDL_RenderCopyEx(win->renderer,t->t,NULL,&r,angle,center,flip);
}
void Text_render_clipEx(Window* win, Text* t, SDL_Rect r, SDL_Rect c, double angle, Vec2* center, SDL_RendererFlip flip){
	SDL_RenderCopyEx(win->renderer,t->t,&c,&r,angle,center,flip);
}
void Text_render_scaled(Window* win, Text* t, Vec2 pos, Vector2 scale){
	SDL_Rect r=(SDL_Rect){pos.x,pos.y,(int)(scale.x*(float)t->size.x),(int)(scale.y*(float)t->size.y)}; Text_render(win,t,r);
}
// ---

// Camera functions
SDL_Rect Camera_rect_render(Camera* cam,int gridX, int gridY){
	return (SDL_Rect){gridX*GRID_SIZE+cam->win->hsize.x-(int)cam->pos.x,gridY*GRID_SIZE+cam->win->hsize.y-(int)cam->pos.y,GRID_SIZE,GRID_SIZE};
}
void Camera_render(Camera* cam, Map* map, Asset* assets){
	for(int i=0; i<map->chunkSize; i++){
		int chunkX=map->chunks[i].x*16, chunkY=map->chunks[i].y*16;
		if(Vector2_distance_square(cam->pos,(Vector2){(float)chunkX*GRID_SIZE,(float)chunkY*GRID_SIZE}) >= square((float)cam->win->size.x*1.5f)) continue;
		for(int u=0; u<256; u++){
			int x=chunkX+u%16-8, y=chunkY+u/16-8;
			SDL_Rect r = (SDL_Rect){
				x*GRID_SIZE+cam->win->hsize.x-(int)cam->pos.x,
				y*GRID_SIZE+cam->win->hsize.y-(int)cam->pos.y/*-GRID_SIZE*(assets[map->chunks[i].bg[u]].clip.h/32)*/,
				GRID_SIZE/* *(assets[map->chunks[i].bg[u]].clip.w/32)*/, GRID_SIZE/* *(assets[map->chunks[i].bg[u]].clip.h/32)*/
			};
			Asset_render(cam->win,assets[map->chunks[i].bg[u]],r);
		}
	}
	for(int i=0; i<map->chunkSize; i++){
		int chunkX=map->chunks[i].x*16, chunkY=map->chunks[i].y*16;
		if(Vector2_distance_square(cam->pos,(Vector2){(float)chunkX*GRID_SIZE,(float)chunkY*GRID_SIZE}) >= square((float)cam->win->size.x*1.5f)) continue;
		for(int u=0; u<256; u++){
			if(map->chunks[i].blocks[u] == MAP_NULL) continue;
			int x=chunkX+u%16-8, y=chunkY+u/16-8;
			Asset block_asset = assets[map->chunks[i].blocks[u]+MAP_STONE_FLOOR_END];
			SDL_Rect r = (SDL_Rect){
				x*GRID_SIZE+cam->win->hsize.x-(int)cam->pos.x,
				y*GRID_SIZE+cam->win->hsize.y-GRID_SIZE*(block_asset.clip.h/32-1)-(int)cam->pos.y,
				GRID_SIZE*(block_asset.clip.w/32), GRID_SIZE*(block_asset.clip.h/32)
			};
			Asset_render(cam->win,block_asset,r);
		}
	}
}
Vector2 Camera_getGlobalMousePos(Camera* cam, Vec2 mousePos){
	return (Vector2){cam->pos.x+(float)mousePos.x-(float)cam->win->hsize.x,cam->pos.y+(float)mousePos.y-(float)cam->win->hsize.y};
}
// ---

// Animation functions
void Animation_free(Animation* anim){free(anim->frames);}
void Animation_setup(Animation* anim, uint8_t fps, uint8_t frameCount, Asset frames[]){
	anim->fps=fps;
	anim->frameCount=frameCount;
	anim->currFrame=0;
	anim->frames=(Asset*)malloc(sizeof(Asset)*frameCount);
	for(int i=0; i<frameCount; i++){
		anim->frames[i]=frames[i];
	}
}
void Animation_update(Animation* anim, Uint32* lastTick, Uint32 currTick){
	if(currTick-(*lastTick) >= (unsigned int)(1000/anim->fps)){
		anim->currFrame++;
		if(anim->currFrame >= anim->frameCount) anim->currFrame = 0;
		(*lastTick)=currTick;
	}
}
// ---

// Map/MapChunk functions
void MapChunk_default(MapChunk* MC_default, int16_t x, int16_t y){
	for(int i=0;i<256;i++){
		MC_default->blocks[i]=MAP_NULL;
		MC_default->bg[i]=rand()%MAP_GRASS_END;
		MC_default->colls[i]=0;
	}MC_default->x=x; MC_default->y=y;
}
void Map_add(Map* map){
	map->chunkSize++;
	if(map->chunkSize == 1) map->chunks=(MapChunk*)malloc(sizeof(MapChunk)*map->chunkSize);
	else map->chunks=(MapChunk*)realloc(map->chunks,sizeof(MapChunk)*map->chunkSize);
}
void Map_remove(Map* map, uint8_t index){}
int Map_getChunk(Map* map, int8_t x, int8_t y){
	for(int i=0;i<map->chunkSize;i++){if(map->chunks[i].x == x && map->chunks[i].y == y) return i;}
	return -1;
}
// ---
const uint8_t COLL_FULL=0,COLL_LEFT=1,COLL_RIGHT=2,COLL_UP=3,COLL_DOWN=4,COLL_LEFT_UP=5,COLL_RIGHT_UP=6,COLL_LEFT_DOWN=7,COLL_RIGHT_DOWN=8;
const SDL_Rect collShapes[9]={
(SDL_Rect){0,0,GRID_SIZE,GRID_SIZE},(SDL_Rect){0,0,HGRID_SIZE,GRID_SIZE},
(SDL_Rect){HGRID_SIZE,0,HGRID_SIZE,GRID_SIZE},(SDL_Rect){0,0,GRID_SIZE,HGRID_SIZE},
(SDL_Rect){0,HGRID_SIZE,GRID_SIZE,HGRID_SIZE},(SDL_Rect){0,0,HGRID_SIZE,HGRID_SIZE},
(SDL_Rect){HGRID_SIZE,0,HGRID_SIZE,HGRID_SIZE},(SDL_Rect){0,HGRID_SIZE,HGRID_SIZE,HGRID_SIZE},
(SDL_Rect){HGRID_SIZE,HGRID_SIZE,HGRID_SIZE,HGRID_SIZE}
};

// Other universal utilities
void loadMapAssets(Window* win,
	Texture* T_grass_tileset, Texture* T_stone_tileset, Texture* T_struct_tileset, Texture* T_wall_tileset,
	Asset* map_assets)
{
	// Random initialization
	srand(time(NULL));
	// Setup assets for map
	Texture_load(win,T_grass_tileset,"assets/TilesetGrass.png");
	Texture_load(win,T_stone_tileset,"assets/TilesetStoneFloor.png");
	Texture_load(win,T_wall_tileset,"assets/TilesetWalls.png");
	Texture_load(win,T_struct_tileset,"assets/TilesetStruct.png");
	for(int i=0; i<64; i++){map_assets[i]=(Asset){T_grass_tileset,(SDL_Rect){(i%8)*32,(i/8)*32,32,32}};}
	for(int i=0; i<51; i++){map_assets[i+MAP_GRASS_STONE_END]=(Asset){T_stone_tileset,(SDL_Rect){(i%8)*32,(i/8)*32,32,32}};}
	int T_positions[6] = {0,64,129,194,258,323};
	for(int i=0; i<6; i++){map_assets[i+MAP_STONE_FLOOR_END]=(Asset){T_struct_tileset,(SDL_Rect){T_positions[i],0,(i==0||i==3)?64:65,96}};}
	map_assets[MAP_STONE_FLOOR_END+6]=(Asset){T_struct_tileset,(SDL_Rect){80,96,64,64}};
}
long int F_getSize(FILE* F_ptr){
	fseek(F_ptr,0L,SEEK_END);
	unsigned long int sz = ftell(F_ptr);
	rewind(F_ptr);
	return sz;
}
bool F_loadMap(FILE* F_ptr, Map* map){ // Loads the entirety of the file pointed to by F_ptr into the Map struct pointed to by map (Check yourself if F_ptr == NULL)
	unsigned long int F_size = F_getSize(F_ptr);
	if(F_size%sizeof(MapChunk) != 0){printf("File of size %lu is not valid!",F_size);return false;}
	map->chunkSize = F_size/sizeof(MapChunk);
	map->chunks = (MapChunk*)malloc(F_size);
	for(int i=0;i<map->chunkSize;i++) fread(&map->chunks[i],sizeof(MapChunk),1,F_ptr);
	return true;
}
void F_writeMap(FILE* F_ptr, Map* map){// Writes the map chunks inside of the file pointed to by F_ptr
	// (Since we already have the entire map info, we can override the old map info with the new one) -> so we use mode "wb" and not "ab"
	for(int i=0;i<map->chunkSize;i++) fwrite(&map->chunks[i],sizeof(MapChunk),1,F_ptr);
}
// ---

#endif
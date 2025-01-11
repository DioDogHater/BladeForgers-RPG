// SOURCE CODE PROGRAMMED BY DIODOGHATER
// Github: github.com/DioDogHater
// ANYONE TRYING TO COPY MY CODE SHALT BE DECLARED AS A STEALING LOSER
// GET A BRAIN DUMBASS

#include "include/bladelib.h"
#include <string.h>

// Constants
const Vec2 WINDOW_SIZE = (Vec2){800,800};
const Uint32 FPS_MAX = 1000/120;

// graphics/event related variables
Window win;
SDL_Event event;

// Player variables
Texture T_shadow;
Texture T_player;
Animation A_player_idle;
Animation A_player_walk;
Animation A_player_swing;
Animation* A_player_curr_anim;
bool player_swinging;

// Tileset assets
Texture T_grass_tileset;
Texture T_stone_tileset;
Texture T_wall_tileset;
Texture T_struct_tileset;
Asset map_assets[MAP_ASSET_COUNT];

// Other assets and variables
Text TXT_fpsCounter;

void loadMedia(){
	// Load terminal.ttf
	TXT_fpsCounter.font = Text_loadFont("assets/terminal.ttf",20);
	
	// Load textures in one go
	Texture_load(&win,&T_player,"assets/Soldier.png");
	Texture_load(&win,&T_shadow,"assets/shadow.png");
	
	// Setup player animations
	Asset A_player_idle_assets[5]; for(int i=0; i<5; i++){A_player_idle_assets[i] = (Asset){&T_player,(SDL_Rect){i*100,0,70,64}};}
	Animation_setup(&A_player_idle, 3, 5, A_player_idle_assets);
	Asset A_player_walk_assets[8]; for(int i=0; i<8; i++){A_player_walk_assets[i] = (Asset){&T_player,(SDL_Rect){i*100,100,70,64}};}
	Animation_setup(&A_player_walk, 20, 8, A_player_walk_assets);
	Asset A_player_swing_assets[6]; for(int i=0; i<6; i++){A_player_swing_assets[i] = (Asset){&T_player,(SDL_Rect){i*100,200,70,64}};}
	Animation_setup(&A_player_swing, 20, 6, A_player_swing_assets);
	A_player_curr_anim=&A_player_idle;
	
	// Setup map assets
	loadMapAssets(&win,&T_grass_tileset,&T_stone_tileset,&T_struct_tileset,&T_wall_tileset,map_assets);
}

void freeEverything(){
	Animation_free(&A_player_idle);
	Animation_free(&A_player_walk);
	Texture_free(&T_grass_tileset);
	Texture_free(&T_stone_tileset);
	Texture_free(&T_wall_tileset);
	Texture_free(&T_struct_tileset);
	Texture_free(&T_player);
	Texture_free(&T_shadow);
	Text_free(&TXT_fpsCounter);
	Window_destroy(&win);
}

int main(int argv, char* args[]){
	// Load map
	Map mainmap;
	FILE* F_ptr=fopen("maps/default.bin","rb");
	if(F_ptr == NULL){
		fclose(F_ptr);
		char file_path[32]; char real_file_path[36];
		printf("Your map should be in the folder /maps.\nPlease enter your custom map's name (with .bin):\n>");
		scanf("%s",file_path);
		strcat(real_file_path,file_path);
		F_ptr = fopen(real_file_path,"rb");
		if(F_ptr == NULL){printf("File unreadable / does not exist!"); return -1;}
	}
	F_loadMap(F_ptr,&mainmap);
	fclose(F_ptr);
	
	// Initialize and quit if something goes wrong
	if(!initEverything()) return -1;
	if(!Window_init(&win,"Test",WINDOW_SIZE,SDL_WINDOW_SHOWN)) return -1;
	
	// Camera and player variables
	Camera cam = {&win, (Vector2){0,0}};
	Vector2 plVel = (Vector2){0.0f,0.0f};
	SDL_RendererFlip plFlip = SDL_FLIP_NONE;
	int k_left=0, k_right=0, k_up=0, k_down=0;
	
	loadMedia();
	
	// Time variables
	Uint32 lastTick=0, nowTick=SDL_GetTicks(), lastAnimTick=0;
	Uint64 NOW=SDL_GetPerformanceCounter(), LAST=0;
	double deltaTime;
	
	// Game loop
	bool running = true;
	while(running){
		// FPS locking
		nowTick=SDL_GetTicks();
		if(nowTick-lastTick < FPS_MAX) continue;
		lastTick=nowTick;
		
		// Calculating the deltaTime
		LAST=NOW;
		NOW=SDL_GetPerformanceCounter();
		deltaTime=(double)((NOW-LAST)*1000 / (double)SDL_GetPerformanceFrequency());
		
		char TXT_fpsCounter_text[10];
		sprintf(TXT_fpsCounter_text,"%.1f FPS",1000.0d/deltaTime);
		Text_load(&win,&TXT_fpsCounter,TXT_fpsCounter_text,(SDL_Color){255,255,255,255});
		
		// Event handling
		while(SDL_PollEvent(&event) != 0){
			if(event.type == SDL_QUIT) running = false;
			else if(event.type == SDL_KEYDOWN){
				switch(event.key.keysym.sym){
					case SDLK_w:
						k_up=1;
						break;
					case SDLK_s:
						k_down=1;
						break;
					case SDLK_a:
						k_left=1;
						if(!player_swinging) plFlip = SDL_FLIP_HORIZONTAL;
						break;
					case SDLK_d:
						k_right=1;
						if(!player_swinging) plFlip = SDL_FLIP_NONE;
						break;
				}
			}else if(event.type == SDL_KEYUP){
				switch(event.key.keysym.sym){
					case SDLK_w:
						k_up=0;
						break;
					case SDLK_s:
						k_down=0;
						break;
					case SDLK_a:
						k_left=0;
						break;
					case SDLK_d:
						k_right=0;
						break;
				}
			}else if(event.type == SDL_MOUSEBUTTONDOWN){
				if(!player_swinging)player_swinging=true;
			}
		}
		
		plVel.x = k_right-k_left;
		plVel.y = k_down-k_up;
		
		// Update animations if necessary
		if(A_player_curr_anim == &A_player_swing && A_player_swing.currFrame == 5){
			A_player_swing.currFrame=0;A_player_curr_anim=&A_player_idle;player_swinging=false;
			if(plVel.x > 0) plFlip=SDL_FLIP_NONE; else if(plVel.y < 0) plFlip=SDL_FLIP_HORIZONTAL;
		}
		if(A_player_curr_anim != &A_player_idle && plVel.x == 0 && plVel.y == 0 && !player_swinging){A_player_curr_anim->currFrame=0;A_player_curr_anim=&A_player_idle;}
		if(A_player_curr_anim != &A_player_walk && (plVel.x != 0 || plVel.y != 0) && !player_swinging){A_player_curr_anim->currFrame=0;A_player_curr_anim=&A_player_walk;}
		if(A_player_curr_anim != &A_player_swing && player_swinging){A_player_curr_anim->currFrame=0;A_player_curr_anim=&A_player_swing;}
		Animation_update(A_player_curr_anim,&lastAnimTick,nowTick);
		
		// Clear the current window display
		Window_clear(&win);
		
		// Move and render camera
		Vector2 plVel_nm=scaleVector2(Vector2_normalized(plVel),(float)deltaTime*(player_swinging ? 0.05f:0.21f));
		cam.pos.x += plVel_nm.x;
		Vector2 pl_coll_pos=(Vector2){8.0f*(float)GRID_SIZE,8.0f*(float)GRID_SIZE+(float)HGRID_SIZE*0.35f};
		Vector2 pl_coll_size=(Vector2){(float)HGRID_SIZE*1.1f,(float)HGRID_SIZE};
		bool colliding=Coll_check_rect(addVector2(cam.pos,pl_coll_pos),pl_coll_size,&mainmap);
		if(colliding) cam.pos.x -= plVel_nm.x;
		cam.pos.y += plVel_nm.y;
		colliding=Coll_check_rect(addVector2(cam.pos,pl_coll_pos),pl_coll_size,&mainmap);
		if(colliding) cam.pos.y -= plVel_nm.y;
		Camera_render(&cam,&mainmap,map_assets);
		
		// Render colliders as a debug feature
		//Camera_render_debug_colls(&cam,&mainmap);
		
		// Render player
		Texture_render(&win,&T_shadow,(SDL_Rect){win.hsize.x-(int)((float)HGRID_SIZE*0.75f),win.hsize.y-(int)((float)HGRID_SIZE*0.4f),(int)((float)GRID_SIZE*0.75f),(int)((float)GRID_SIZE*0.75f)});
		Asset_renderEx(&win,
			A_player_curr_anim->frames[A_player_curr_anim->currFrame],
			(SDL_Rect){win.hsize.x-(plFlip==SDL_FLIP_NONE ? (float)GRID_SIZE*2.89f:(float)GRID_SIZE*1.15f),win.hsize.y-GRID_SIZE*3,(int)((float)GRID_SIZE*3.75f*1.093f),(int)((float)GRID_SIZE*3.75f)},
			plFlip
		);
		Text_render_scaled(&win,&TXT_fpsCounter,(Vec2){0,0},(Vector2){1.0f,1.0f});
		
		// Render the player's collider
		//SDL_Rect r=(SDL_Rect){win.hsize.x-(int)(pl_coll_size.x/2.0f),win.hsize.y-(int)(pl_coll_size.y/2.0f-(pl_coll_pos.y-GRID_SIZE*8)),(int)pl_coll_size.x,(int)pl_coll_size.y};
		//Window_setColor(&win,(SDL_Color){255,255,255,255});
		//SDL_RenderDrawRect(win.renderer,&r);
		
		// Update the current window display (Show everything rendered)
		Window_update(&win);
	}
	
	free(mainmap.chunks);
	
	// Quitting preparations
	freeEverything();
	quitEverything();
	return 0;
}
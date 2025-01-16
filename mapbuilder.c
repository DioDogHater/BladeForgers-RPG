// SOURCE CODE PROGRAMMED BY DIODOGHATER
// Github: github.com/DioDogHater
// ANYONE TRYING TO COPY MY CODE SHALT BE DECLARED AS A STEALING LOSER
// GET A BRAIN DUMBASS

#include "include/bladelib.h"
#include <string.h>

// Display/Event related variables
Window win;
SDL_Event event;

// Constants
const Vec2 WIN_SIZE=(Vec2){1000,800};
const int FPS_MAX=1000/120;

// Assets
Texture T_grass_tileset;
Texture T_stone_tileset;
Texture T_struct_tileset;
Texture T_wall_tileset;
Asset map_assets[MAP_ASSET_COUNT];
Text TXT_fpsCounter;

void loadMedia(){
	loadMapAssets(&win,&T_grass_tileset,&T_stone_tileset,&T_struct_tileset,&T_wall_tileset,map_assets);
	TXT_fpsCounter.font = Text_loadFont("assets/terminal.ttf",20);
}

void freeEverything(){
	Texture_free(&T_grass_tileset);
	Texture_free(&T_stone_tileset);
	Texture_free(&T_struct_tileset);
	Texture_free(&T_wall_tileset);
	Text_free(&TXT_fpsCounter);
	Window_destroy(&win);
}

int main(int argv, char* args[]){
	// Create current map in case of map editing
	Map curr_map;
	
	// File handling and user input
	char file_path[32]; char real_file_path[36] = "maps/";
	printf("---- MAP BUILDER ----\n!! MAP WILL BE STORED IN ./maps !!\nPLEASE ENTER FILE TO CREATE/EDIT (must be .bin):\n> ");
	scanf("%s",file_path); strcat(real_file_path,file_path);
	// Check if map already exists!
	FILE* F_map = fopen(real_file_path,"rb");
	if(F_map == NULL){
		printf("\n%s does not exist in \"maps/\"!\nThis program will create a new file when saving...\n",file_path);
		curr_map.chunkSize=0; // Set the map chunk count to 0 so the rendering doesnt raise errors
	}else{
		printf("\nLoading the contents of %s!\n",real_file_path);
		if(F_loadMap(F_map,&curr_map)) printf("Successfully loaded map %s!\n",file_path);
	}
	fclose(F_map);
	
	// Initialize everything for SDL
	if(!initEverything()) return -1;
	if(!Window_init(&win,"Map Builder - Blade Forgers",WIN_SIZE,SDL_WINDOW_SHOWN)) return -1;
	
	// Load all media
	loadMedia();
	
	// Time management
	Uint32 nowTick=0, lastTick=SDL_GetTicks();
	Uint64 NOW=SDL_GetPerformanceCounter(), LAST=0;
	double deltaTime = 0.0;
	
	// Camera used to render map
	Vector2 camVel=(Vector2){0.0f,0.0f};
	Camera cam={&win,(Vector2){0.0f,0.0f}};
	
	uint8_t layer_selection=0;
	uint8_t curr_select=0;
	int16_t scroll_offset = 0;
	
	bool dragging_mouse=false;
	SDL_Rect mouse_grid_hover={0,0,0,0};
	
	// App loop
	bool running = true;
	while(running){
		// Locking FPS
		nowTick = SDL_GetTicks();
		if(nowTick-lastTick < FPS_MAX) continue;
		lastTick=nowTick;
		
		// Calculating deltaTime
		LAST=NOW;
		NOW=SDL_GetPerformanceCounter();
		deltaTime = (double)((NOW-LAST)*1000 / (double)SDL_GetPerformanceFrequency());
		
		// Update FPS counter in the upper left corner
		char TXT_fpsCount[12];
		sprintf(TXT_fpsCount,"%.1f FPS",1000.0/deltaTime);
		Text_load(&win,&TXT_fpsCounter,TXT_fpsCount,(SDL_Color){255,255,255,255});
		
		// Event handling
		while(SDL_PollEvent(&event) != 0){
			switch(event.type){
			case SDL_QUIT:
				running=false;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_w:
						camVel.y = -1.0f;
						break;
					case SDLK_s:
						camVel.y = 1.0f;
						break;
					case SDLK_a:
						camVel.x = -1.0f;
						break;
					case SDLK_d:
						camVel.x = 1.0f;
						break;
					case SDLK_x:{
						int chunkX, chunkY;
						printf("--- CREATE A NEW CHUNK ---\nEnter X position of the chunk:\n>");
						scanf("%d",&chunkX);
						printf("\nEnter Y position of the chunk:\n>");
						scanf(" %d",&chunkY);
						Map_add(&curr_map);
						MapChunk_default(&curr_map.chunks[curr_map.chunkSize-1],chunkX,chunkY);
						printf("\nCreated new chunk with position: %d, %d\n",chunkX,chunkY);
						break;
					}case SDLK_z:{
						printf("\n--- SAVING MAP ---\nFILE %s WILL BE OVERWRITTEN!\n",file_path);
						FILE* F_ptr = fopen(real_file_path,"wb");
						if(F_ptr == NULL) printf("Failed to write in file %s\n",real_file_path);
						else{F_writeMap(F_ptr,&curr_map); printf("SUCCESSFULLY SAVED MAP!\n");}
						fclose(F_ptr);
						break;
					}case SDLK_f:
						layer_selection++;
						if(layer_selection > 2) layer_selection=0;
						curr_select=0;
						printf("Switching asset type to %d\n",layer_selection);
						break;
				}break;
			case SDL_KEYUP:
				switch(event.key.keysym.sym){
					case SDLK_w:
						if(camVel.y < 0) camVel.y=0.0f;
						break;
					case SDLK_s:
						if(camVel.y > 0) camVel.y=0.0f;
						break;
					case SDLK_a:
						if(camVel.x < 0) camVel.x=0.0f;
						break;
					case SDLK_d:
						if(camVel.x > 0) camVel.x=0.0f;
						break;
				}break;
			case SDL_MOUSEBUTTONDOWN:
				//printf("mouse button: %d at: %d, %d\n",event.button.button,event.button.x,event.button.y);
				if(event.button.button == 1){
					if(event.button.x >= 800){
						if(layer_selection == 0){
							int8_t gridX=(event.button.x-800)/40, gridY=(event.button.y-scroll_offset)/40;
							if(gridX >= 0 && gridX < 5 && gridY >= 0 && gridY*5+gridX < MAP_STONE_FLOOR_END){
								uint8_t asset_index=gridY*5+gridX;
								curr_select = asset_index;
							}
						}else if(layer_selection == 1){
							int8_t gridX=(event.button.x-800)/100, gridY=(event.button.y-scroll_offset)/100;
							if(gridX >= 0 && gridX < 2 && gridY >= 0 && gridY*2+gridX < MAP_ASSET_COUNT-MAP_STONE_FLOOR_END) curr_select=(uint8_t)(gridY*2+gridX);
							else if(gridY*2+gridX == MAP_ASSET_COUNT-MAP_STONE_FLOOR_END) curr_select=MAP_ASSET_COUNT-MAP_STONE_FLOOR_END;
						}else if(layer_selection == 2){
							int8_t gridX=(event.button.x-800)/50, gridY=(event.button.y-scroll_offset)/50;
							if(gridX >= 0 && gridX < 4 && gridY >= 0 && gridY*4+gridX <= COLL_SHAPE_LEN) curr_select=(uint8_t)(gridY*4+gridX);
						}
					}else{
						Vector2 global_mspos = Camera_getGlobalMousePos(&cam,(Vec2){event.button.x,event.button.y});
						int gridX=(int)floor((double)global_mspos.x/(double)GRID_SIZE)+8, gridY=(int)floor((double)(global_mspos.y)/(double)GRID_SIZE)+8;
						int chunkX=(int)floor((double)gridX/16.0), chunkY=(int)floor((double)gridY/16.0);
						int mapIndex = Map_getChunk(&curr_map,chunkX,chunkY);
						if(mapIndex != -1){
							gridX-=chunkX*16; gridY-=chunkY*16;
							uint8_t chunk_index=gridY*16+gridX;
							if(layer_selection == 0){
								curr_map.chunks[mapIndex].bg[chunk_index]=curr_select;
							}else if(layer_selection == 1){
								if(curr_select == MAP_ASSET_COUNT-MAP_STONE_FLOOR_END) curr_map.chunks[mapIndex].blocks[chunk_index]=MAP_NULL;
								else curr_map.chunks[mapIndex].blocks[chunk_index]=curr_select;
							}else if(layer_selection == 2){
								if(curr_select < COLL_SHAPE_LEN) curr_map.chunks[mapIndex].colls[chunk_index]=curr_select+1;
								else curr_map.chunks[mapIndex].colls[chunk_index]=0;
							}
						}
					}
				}else if(event.button.button == 2) dragging_mouse=true;
				break;
			case SDL_MOUSEBUTTONUP:
				if(dragging_mouse) dragging_mouse=false;
				break;
			case SDL_MOUSEMOTION:
				if(dragging_mouse){cam.pos.x-=(float)event.motion.xrel;cam.pos.y-=(float)event.motion.yrel;}
				if(event.motion.x >= 800){mouse_grid_hover=(SDL_Rect){0,0,0,0}; break;}
				Vector2 global_mspos = Camera_getGlobalMousePos(&cam,(Vec2){event.motion.x,event.motion.y});
				int gridX=(int)floor((double)global_mspos.x/(double)GRID_SIZE), gridY=(int)floor((double)(global_mspos.y)/(double)GRID_SIZE);
				mouse_grid_hover=Camera_rect_render(&cam,gridX,gridY);
				break;
			case SDL_MOUSEWHEEL:
				scroll_offset += (int)(event.wheel.preciseY*15.0f);
				break;
			}
		}
		
		// Clear the screen
		Window_clear(&win);
		
		// Render the map
		cam.pos=addVector2(cam.pos,scaleVector2(Vector2_normalized(camVel),(float)deltaTime*0.75f));
		Camera_render(&cam,&curr_map,map_assets);
		if(layer_selection == 2) Camera_render_debug_colls(&cam,&curr_map);
		
		// Render the white hover square when we are hovering a block
		Window_setColor(&win,(SDL_Color){255,255,255,255});
		SDL_RenderDrawRect(win.renderer,&mouse_grid_hover);
		
		// Render the block options
		SDL_Rect r=(SDL_Rect){800,0,200,800};
		Window_setColor(&win,(SDL_Color){0,0,0,255});
		SDL_RenderFillRect(win.renderer,&r);
		
		if(layer_selection == 0){
			for(int i=0;i<MAP_STONE_FLOOR_END;i++){
				r=(SDL_Rect){800+(i%5)*40,i/5*40+scroll_offset,40,40};
				Asset_render(&win,map_assets[i],r);
				if(i+1 == MAP_STONE_FLOOR_END){Window_setColor(&win,(SDL_Color){255,255,255,255});SDL_RenderDrawRect(win.renderer,&r);}
				if(i == curr_select){Window_setColor(&win,(SDL_Color){255,0,0,255});SDL_RenderDrawRect(win.renderer,&r);}
			}
		}else if(layer_selection == 1){
			for(int i=MAP_STONE_FLOOR_END;i<=MAP_ASSET_COUNT;i++){
				r=(SDL_Rect){800+((i-MAP_STONE_FLOOR_END)%2)*100,(i-MAP_STONE_FLOOR_END)/2*100+scroll_offset,100,100};
				if(i < MAP_ASSET_COUNT) Asset_render(&win,map_assets[i],r);
				else{Window_setColor(&win,(SDL_Color){255,255,255,255});SDL_RenderDrawRect(win.renderer,&r);}
				if(i-MAP_STONE_FLOOR_END == curr_select){Window_setColor(&win,(SDL_Color){255,0,0,255});SDL_RenderDrawRect(win.renderer,&r);}
			}
		}else if(layer_selection == 2){
			for(int i=0;i<=COLL_SHAPE_LEN;i++){
				SDL_Rect cr=coll_shapes[i]; SDL_Rect r;
				if(i < COLL_SHAPE_LEN){
					r=(SDL_Rect){
						(int)((float)cr.x/(float)HGRID_SIZE*25.0f)+800+(i%4)*50,
						(int)((float)cr.y/(float)HGRID_SIZE*25.0f)+(i/4)*50+scroll_offset,
						(int)((float)cr.w/(float)HGRID_SIZE*25.0f),
						(int)((float)cr.h/(float)HGRID_SIZE*25.0f)
					};
					Window_setColor(&win,(SDL_Color){0,0,255,150});
					SDL_RenderFillRect(win.renderer,&r);
				}r=(SDL_Rect){800+(i%4)*50,(i/4)*50+scroll_offset,50,50};
				Window_setColor(&win, i == curr_select ? (SDL_Color){255,0,0,255}:(SDL_Color){255,255,255,255});
				SDL_RenderDrawRect(win.renderer,&r);
			}
		}
		
		// Render the white outline of the asset selection "box"
		r=(SDL_Rect){799,0,201,800};
		Window_setColor(&win,(SDL_Color){255,255,255,255});
		SDL_RenderDrawRect(win.renderer,&r);
		
		// Render the fps counter
		Text_render_scaled(&win,&TXT_fpsCounter,(Vec2){0,0},(Vector2){1.0f,1.0f});
		
		// Update the screen
		Window_update(&win);
	}
	
	free(curr_map.chunks);
	freeEverything();
	
	return 0;
}
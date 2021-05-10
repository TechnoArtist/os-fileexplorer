#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h> 

#define WIDTH 800
#define HEIGHT 600

// TODO warning: Images are using relative paths, *must* run program from main folder not bin. 
/* Suggestions / notes

Convert spaces to tabs (including makefile)
Use first icon in assignment doc list, if a file has multiple types. 
Single click works fine, double click is optional
Use OOP principals, don't just program in main (e.g. make a class for files, directories, images... Take inspiration from the mmu of last project)
Take inspiration (and some code) from ls.c
Teacher says make a bool per item that can be clicked on; I think they should be objects to make an easy array. Or have a single non-bool for "selected" (with an available "none" value). 

https://upload.wikimedia.org/wikipedia/commons/0/09/Tux%2C_gray%EF%BC%8Fgrey_background.png 
https://github.com/tmarrinan/os-fileexplorer/blob/main/resrc/OpenSans-Regular.ttf 
https://wiki.libsdl.org/SDL_Event 

sudo apt-get install libsdl2-2.0-0 libsdl2-dev
sudo apt-get install libsdl2-image-2.0-0 libsdl2-image-dev
sudo apt-get install libsdl2-ttf-2.0-0 libsdl2-ttf-dev
*/

typedef struct AppData {
	SDL_Font *font; 
    vector<ScreenFile> files; 
    
	SDL_Texture *specific_texture; 
	SDL_Texture *text_example; 
	SDL_Rect penguin_location; 
	SDL_Rect phrase_location; 
} AppData; 

void initialize(SDL_Renderer *renderer, AppData* data);
void render(SDL_Renderer *renderer, AppData* data);

int main(int argc, char **argv)
{
	char *home = getenv("HOME");
	printf("HOME: %s\n", home);

	// initializing SDL as Video
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG); // TODO if you want non-png images, change this (currently is efficient)
	TTF_Init(); 

	// create window and renderer
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

	// initialize and perform rendering loop
	AppData data; 
	initialize(renderer, &data);
	render(renderer, &data);
	SDL_Event event;
	SDL_WaitEvent(&event);
	while (event.type != SDL_QUIT)
	{
		SDL_WaitEvent(&event);
		switch(event.type) {
			case SDL_MOUSEMOTION:
				printf("Mouse: %d, %d\n", event.motion.x, event.motion.y); 
				break; 
			case SDL_MOUSEBUTTONDOWN:
				printf("Button down\n"); 
				// TODO finish
				// Note: keep if-else in order of render layer (top first)
				if (event.button.x > phrase_location.x && <= phrase_location.x + phrase_location.w) {
					
				}
				break; 
			case SDL_MOUSEBUTTONUP:
				printf("Button up\n"); 
				break; 
			case :
		}
		
		render(renderer, &data); // loops rendering
	}

	// clean up
	SDL_DestroyTexture(data.specific_texture); 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit(); 
	IMG_Quit(); 
	SDL_Quit();

	return 0;
}

void initialize(SDL_Renderer *renderer, AppData* data)
{
	// set color of background when erasing frame (rgba)
	SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
	
	// Load fonts
	data->specific_font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 18); 
	
	// Load sprites 
	SDL_Surface *surf = IMG_Load("resrc/images/image.png"); 
	data->specific_texture *texture = SDL_CreateTextureFromSurface(renderer, surf); 
	SDL_FreeSurface(surf); 
	penguin_location.x = 200; 
	penguin_location.y = 100; 
	penguin_location.w = 165; 
	penguin_location.h = 200; 
	
	SDL_Color text_color = {0, 0, 0}; 
	SDL_Surface *text_surf = TTF_RenderText_Solid(data->specific_font, "Hello World!", text_color); 
	data->text_example *text_texture = SDL_CreateTextureFromSurface(renderer, text_surf); 
	SDL_FreeSurface(text_surf); 
	phrase_location.x = 10; 
	phrase_location.y = 500; 
}

void render(SDL_Renderer *renderer, AppData* data)
{
	// erase renderer content
	SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
	SDL_RenderClear(renderer);
	
	// TODO: draw!
	
	// --- How to rectangle ---
	// coords are in pixels
	SDL_Rect rect; // shorthand {x, y, w, h} for the values (in order)
	rect.x = 200; // left
	rect.y = 150; // top
	rect.w = 300; // width
	rect.h = 100; // height
	SDL_SetRenderDrawColor(renderer, 182, 64, 64, 255);
	//SDL_RenderFillRect(renderer, &rect); 
	
	// --- How to sprite ---
	// 1. Download image, save in resrc/images/[filename]
	
	// Look in properties to get exact image size if needed (calculator to scale)
	SDL_RenderCopy(renderer, data->specific_texture, NULL, &(data->penguin_location); 
	
	// --- How to text ---
	SDL_QueryTexture(data->text_example, NULL, NULL, &(width_returned), &(height_returned)); 
	SDL_RenderPresent(renderer, data->text_example, NULL, &(data->phrase_location); 
	

	// show rendered frame
	SDL_RenderPresent(renderer);
}



/* Suggestions / notes

WARNING Images are using relative paths, *must* run program from main folder not bin. 

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

new dir
	destroy old text (names), don't need 'em anymore
	load new text
	don't need to recreate while scrolling / interacting with nondirs / etc

If top level recursive viewing is crashing, 
	speed up a tad by only rendering those in view (not all across scroll bar) and rechecking on scroll
*/

#include <iostream>
#include <SDL.h>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>


#define WIDTH 800
#define HEIGHT 600

#define BUFFER_HEIGHT 5
#define ROW_HEIGHT 20 // can fit 35 items per page
#define COL_WIDTH_ICON ROW_HEIGHT // up to 20
#define COL_WIDTH_NAME 280 // up to 300
#define COL_WIDTH_SIZE 100 // up to 400
#define COL_WIDTH_PERM 100 // up to 500
#define PAGINATION_BUTTON_WIDTH 80 // up to 580

/* TODO main list

- Add the page up/down buttons, and make them change data->current_page_start
- Add a function per filetype to handle clicks
- Add a location-setting loop to initialize() or render()
- Question: Does initialize() need to be re-called when the page is changed? 

Note: List-view rather than grid-view, include file size & permissions on side

*/

enum filetype{directory, executable, image, video, codefile, other};

typedef struct ScreenObject {
	filetype type; 
	std::string filename; 
	SDL_Rect icon_rect {0, 0, COL_WIDTH_ICON, ROW_HEIGHT}; 
	SDL_Rect name_rect {0, 0, COL_WIDTH_NAME, ROW_HEIGHT}; 
	SDL_Rect size_rect {0, 0, COL_WIDTH_SIZE, ROW_HEIGHT}; 
	SDL_Rect perm_rect {0, 0, COL_WIDTH_PERM, ROW_HEIGHT}; 
	SDL_Texture* icon_texture; 
	SDL_Texture* name_texture; 
	SDL_Texture* size_texture; 
	SDL_Texture* perm_texture; 
	int data_size; 
	std::string permissions; 
} ScreenObject; 

typedef struct AppData {
	TTF_Font *font; 
    std::vector<ScreenObject*> files; 
	// TODO update current_page_start somewhere (on pagination button click?)
	int current_page_start = 0; 
	int page_size = 20; 
} AppData; 

void initialize(SDL_Renderer *renderer, AppData* data);
void render(SDL_Renderer *renderer, AppData* data);

// directory, executable, image, video, codefile, other
void openDirectory(ScreenObject* file); 
void openExecutable(ScreenObject* file); 
void openImage(ScreenObject* file); 
void openVideo(ScreenObject* file); 
void openCodefile(ScreenObject* file); 
void openOther(ScreenObject* file); 

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
	
	// TODO initialize each of the files to go in data->files and add them
	ScreenObject* file = new ScreenObject(); 
	file->data_size = 19; 
	file->filename = "Test"; 
	file->permissions = "rwx"; 
	file->type = filetype::image; 
	
	data.files.push_back(file); 
	
	initialize(renderer, &data);
	render(renderer, &data);
	SDL_Event event;
	SDL_WaitEvent(&event);
	while (event.type != SDL_QUIT)
	{
		SDL_WaitEvent(&event);
		switch(event.type) {
			case SDL_MOUSEMOTION:
				//printf("Mouse: %d, %d\n", event.motion.x, event.motion.y); 
				break; 
			case SDL_MOUSEBUTTONDOWN:
				printf("Button down\n"); 
				break; 
			case SDL_MOUSEBUTTONUP:
				printf("Button up\n"); 
				if (event.button.x < WIDTH - PAGINATION_BUTTON_WIDTH) {
					// WARNING This assumes integer math drops the remainder
					int file_index = (event.button.y / (ROW_HEIGHT + BUFFER_HEIGHT)) + data.current_page_start; 
					printf("Clicked on file %i\n", file_index); 
					if (file_index < data.files.size()) {
						printf("Index found\n"); 
						ScreenObject* file = data.files.at(file_index); 
						switch (file->type) {
							case directory: 
								openDirectory(file); 
								break; 
							case executable: 
								openExecutable(file); 
								break; 
							case image: 
								openImage(file); 
								break; 
							case video: 
								openVideo(file); 
								break; 
							case codefile: 
								openCodefile(file); 
								break; 
							default: 
								openOther(file); 
								break; 
						}
					}
				}
				break; 
		}
		
		render(renderer, &data); // loops rendering
	}

	// clean up
	// TODO loop-destroy the textures
	//SDL_DestroyTexture(data.specific_texture); 
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
	SDL_Color text_color = {0, 0, 0}; 
	SDL_Surface *surf; 
	
	// Load font
	data->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", ROW_HEIGHT); 
	
	SDL_SetRenderDrawColor(renderer, 215, 215, 235, 255);
	// Load icons
	for (int i = data->current_page_start; i < data->current_page_start + data->page_size && i < data->files.size(); i++) {
		ScreenObject* file = data->files.at(i); 
		switch (file->type) {
			// directory, executable, image, video, codefile, other
			case directory: 
				surf = IMG_Load("resrc/images/directory.png"); 
				break; 
			case executable: 
				surf = IMG_Load("resrc/images/executable.png"); 
				break; 
			case image: 
				surf = IMG_Load("resrc/images/image.png"); 
				break; 
			case video: 
				surf = IMG_Load("resrc/images/video.png"); 
				break; 
			case codefile: 
				surf = IMG_Load("resrc/images/codefile->png"); 
				break; 
			default: 
				surf = IMG_Load("resrc/images/other.png"); 
				break; 
		}
		file->icon_texture = SDL_CreateTextureFromSurface(renderer, surf); 
		SDL_FreeSurface(surf); 
	}
	
	// Load filenames
	for (int i = data->current_page_start; i < data->current_page_start + data->page_size && i < data->files.size(); i++) {
		ScreenObject* file = data->files.at(i); 
		surf = TTF_RenderText_Solid(data->font, file->filename.c_str(), text_color); 
		file->name_texture = SDL_CreateTextureFromSurface(renderer, surf); 
		SDL_FreeSurface(surf); 
	}
	
	// Load file sizes
	for (int i = data->current_page_start; i < data->current_page_start + data->page_size && i < data->files.size(); i++) {
		ScreenObject* file = data->files.at(i); 
		surf = TTF_RenderText_Solid(data->font, std::to_string(file->data_size).c_str(), text_color); 
		file->size_texture = SDL_CreateTextureFromSurface(renderer, surf); 
		SDL_FreeSurface(surf); 
	}
	
	// Load file permissions
	for (int i = data->current_page_start; i < data->current_page_start + data->page_size && i < data->files.size(); i++) {
		ScreenObject* file = data->files.at(i); 
		surf = TTF_RenderText_Solid(data->font, file->permissions.c_str(), text_color); 
		file->perm_texture = SDL_CreateTextureFromSurface(renderer, surf); 
		SDL_FreeSurface(surf); 
	}
	
	// TODO Set item locations for icon, name, size, permissions
	// example: penguin_location.x = 200; file->icon_rect.x = 200; 
	for (int i = data->current_page_start; i < data->current_page_start + data->page_size && i < data->files.size(); i++) {
		ScreenObject* file = data->files.at(i); 
		file->icon_rect.x = COL_WIDTH_ICON; 
		file->name_rect.x = file->icon_rect.x + COL_WIDTH_NAME; 
		file->size_rect.x = file->name_rect.x + COL_WIDTH_SIZE; 
		file->perm_rect.x = file->size_rect.x + COL_WIDTH_PERM; 
		
		int y = (i - data->current_page_start) * ROW_HEIGHT + BUFFER_HEIGHT; 
		file->icon_rect.y = y; 
		file->name_rect.y = y; 
		file->size_rect.y = y; 
		file->perm_rect.y = y; 
	}
	
}

void render(SDL_Renderer *renderer, AppData* data)
{
	// erase renderer content
	SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
	SDL_RenderClear(renderer);
	
	// draw!
	
	// --- How to rectangle ---
	// coords are in pixels
	//SDL_Rect rect {200, 150, 300, 100}; // shorthand {x, y, w, h} for the values (in order)
	//SDL_SetRenderDrawColor(renderer, 182, 64, 64, 255);
	//SDL_RenderFillRect(renderer, &rect); 
	
	// --- How to sprite and text ---
	// 1. Download image, save in resrc/images/[filename]
	// Look in properties to learn exact image size if needed (calculator to scale)
	
	
	for (int i = data->current_page_start; i < data->current_page_start + data->page_size && i < data->files.size(); i++) {
		ScreenObject* file = data->files.at(i); 
		SDL_RenderCopy(renderer, file->icon_texture, NULL, &(file->icon_rect)); 
		SDL_QueryTexture(file->icon_texture, NULL, NULL, &(file->icon_rect.w), &(file->icon_rect.y)); 
		
		SDL_RenderCopy(renderer, file->name_texture, NULL, &(file->name_rect)); 
		SDL_QueryTexture(file->name_texture, NULL, NULL, &(file->name_rect.w), &(file->name_rect.y)); 
		
		SDL_RenderCopy(renderer, file->size_texture, NULL, &(file->size_rect)); 
		SDL_QueryTexture(file->size_texture, NULL, NULL, &(file->size_rect.w), &(file->size_rect.y)); 
		
		SDL_RenderCopy(renderer, file->perm_texture, NULL, &(file->perm_rect)); 
		SDL_QueryTexture(file->perm_texture, NULL, NULL, &(file->perm_rect.w), &(file->perm_rect.y)); 
	}
	
	
	// show rendered frame
	SDL_RenderPresent(renderer);
}



void openDirectory(ScreenObject* file) {
	// TODO method stub
}

void openExecutable(ScreenObject* file) {
	// TODO method stub
}
void openImage(ScreenObject* file) {
	// TODO method stub
}

void openVideo(ScreenObject* file) {
	// TODO method stub
}

void openCodefile(ScreenObject* file) {
	// TODO method stub
}

void openOther(ScreenObject* file) {
	// TODO method stub
}



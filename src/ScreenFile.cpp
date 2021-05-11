#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h> 

using namespace std;

enum filetype{directory, executable, image, video, codefile, other};

class ScreenObject {
    private:
        const int WIDTH = 100; 
        const int ICON_HEIGHT = 100; 
        const int TEXT_HEIGHT = 10; 
        filetype type = directory; 
        string filename; 
        SDL_Rect iconRect {0, 0, WIDTH, ICON_HEIGHT}; 
        SDL_Rect textRect {0, 0, WIDTH, TEXT_HEIGHT}; 
        SDL_Texture* iconTexture; 
        SDL_Texture* textTexture; 
    public:
        ScreenObject(filetype type, string filename, int x, int y, SDL_Texture* iconTexture, SDL_Texture* textTexture) {
            this.type = type; 
            this.filename = filename; 
            iconRect.x = x; 
            iconRect.y = y; 
            textRect.x = x; 
            textRect.y = y + ICON_HEIGHT; 
            this.iconTexture = iconTexture; 
            this.textTexture = textTexture; 
        }
        
        void init() {
            
        }
        
        // TODO Data access method per filetype? 
};
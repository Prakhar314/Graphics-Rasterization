#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class SoftwareRasterizer{
    // Framebuffer
    const int frameWidth;
    const int frameHeight;
    const int displayScale;
    // Output file
    const char* outputFile = "out.png";
    SDL_Surface* framebuffer = NULL;
    SDL_Window* window = NULL;
    SDL_Surface* windowSurface = NULL;
    
    public:
    bool quit = false;

    SoftwareRasterizer(int w, int h, int s):frameWidth(w),frameHeight(h),displayScale(s){
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            throw SDL_GetError(); 
        } else {
            int screenWidth = frameWidth * displayScale;
            int screenHeight = frameHeight * displayScale;
            window = SDL_CreateWindow("COL781", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
            if (window == NULL) {
                throw SDL_GetError(); 
            } else {
                windowSurface = SDL_GetWindowSurface(window);
                framebuffer = SDL_CreateRGBSurface(0, frameWidth, frameHeight, 32, 0, 0, 0, 0);
            }
        }
    }

    // Handle window exit 
    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }
        
    void saveFramebuffer() {
        // Save the image
        IMG_SavePNG(framebuffer, outputFile);
    }

    void render(){
        Uint32 *pixels = (Uint32*)framebuffer->pixels;
        SDL_PixelFormat *format = framebuffer->format;
        for (int i = 0; i < frameWidth; i++) {
            for (int j = 0; j < frameHeight; j++) {
                float x = i + 0.5;
                float y = j + 0.5;
                float r = 4;
                Uint32 color;
                if ((x-5)*(x-5) + (y-5)*(y-5) <= r*r) { // circle
                    color = SDL_MapRGBA(format, 0, 153, 0, 255); // green
                } else {
                    color = SDL_MapRGBA(format, 255, 255, 255, 255); // white
                }
                pixels[i + frameWidth*j] = color;
            }
        }

        // Update screen to apply the changes
        SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
        SDL_UpdateWindowSurface(window);
    }

    ~SoftwareRasterizer() {
        // Free resources and close SDL
        SDL_FreeSurface(framebuffer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char* args[]) {
    SoftwareRasterizer sR{10,10,40};
    // Display and interaction
    while (!sR.quit) {
        // Event handling
        sR.handleEvents();
        sR.render();
    }
	// Save image
    sR.saveFramebuffer();
    return 0;
}
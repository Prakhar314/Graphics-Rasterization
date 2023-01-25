#include <iostream>
#include <vector>

#include <glm/common.hpp>
#include <glm/integer.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x3.hpp>
#include <glm/geometric.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
using namespace std;
class SoftwareRasterizer{

    // Framebuffer
    const int frameWidth;
    const int frameHeight;
    const int displayScale;
    // Output file
    const char* outputFile = "out.png";

    SDL_Renderer* renderer = NULL;
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


    void clear(glm::vec4 color){
        // argument is normalized
        color *= 255;
        SDL_Rect framerect {0,0,frameWidth, frameHeight};
        SDL_PixelFormat *format = framebuffer->format;
        Uint32 bgColor = SDL_MapRGBA(format, color[0], color[1], color[2], color[3]);
        SDL_FillRect(framebuffer, &framerect, bgColor);
    }

    /**
     * Adds a triangle to framebuffer
    */
    void draw_triangle(glm::vec4 v4_1, glm::vec4 v4_2, glm::vec4 v4_3, glm::vec4 color){
        // transposed on multiplication with vector
        glm::mat4x3 scale{
            frameWidth/2.0,     0,              0,
            0,              frameHeight/2.0,    0,
            0,                  0,              1,
            frameWidth/2.0, frameHeight/2.0,    0
        };
        
        glm::vec3 v1{scale * v4_1}, v2{scale * v4_2}, v3{scale * v4_3};
        
        Uint32 *pixels = (Uint32*)framebuffer->pixels;
        SDL_PixelFormat *format = framebuffer->format;
        color *= 255;
        for (int i = 0; i < frameWidth; i++) {
            for (int j = 0; j < frameHeight; j++) {
                glm::vec3 pos{i+0.5, j+0.5, 0};
                if(glm::cross(v2-v1,pos-v1)[2]<0){
                    continue;
                }
                if(glm::cross(v3-v2,pos-v2)[2]<0){
                    continue;
                }
                if(glm::cross(v1-v3,pos-v3)[2]<0){
                    continue;
                }
                pixels[i + frameWidth*(frameHeight - 1 - j)] = SDL_MapRGBA(format, color[0], color[1], color[2], color[3]);
            }
        }
    }

    /**
     * Adds a shape to framebuffer
    */
    void draw(vector<glm::vec4> vertices, vector<glm::ivec3> indices){
        for(glm::ivec3  i :indices){
            draw_triangle(
                vertices[i[0]],
                vertices[i[1]],
                vertices[i[2]],
                glm::vec4{1,0,0,1}
            );
        }
    }

    /** 
     * Update screen to apply the changes
    */
    void render(){
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
    SoftwareRasterizer sR{50,50,4};
    
    sR.clear(glm::vec4(1,1,0,1));
    vector<glm::vec4> vertices = {
            glm::vec4(-0.8, 0.0, 0.0, 1.0),
            glm::vec4(-0.4, -0.8, 0.0, 1.0),
            glm::vec4(0.8, 0.8, 0.0, 1.0),
            glm::vec4(-0.4, -0.4, 0.0, 1.0)
        };
    vector<glm::ivec3> indices ={
            glm::ivec3{0,1,3},
            glm::ivec3{1,2,3}
        };
    sR.draw(vertices, indices);
    sR.render();
	// Save image
    sR.saveFramebuffer();
    return 0;
}
//
//  Window.hpp
//  ProgrammedParties
//
//  Created by Harry Culpan on 7/5/19.
//  Copyright © 2019 Harry Culpan. All rights reserved.
//

#ifndef Window_hpp
#define Window_hpp

#include <iostream>
#include <list>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

class Window {
protected:
    Window *parent = NULL;
    
    int x, y, w, h;
    
    std::list<Window *>children;
    
    SDL_Renderer *renderer = NULL;
    
    void logSDLError(const std::string &msg);
    
    Window *getTopParent();
    bool withinRectangle(int x, int y);
    
    SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);
    void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);
    void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, double angle);
    void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
    
    SDL_Texture* renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer);

    Window();
    Window(Window *parent);
public:
    Window *getParent() const { return parent; }
    virtual void cleanup() = 0;
    virtual void render(bool forceRedraw) = 0;
    virtual bool handleEvent(SDL_Event *e) = 0;
    
    virtual void render() { render(false); }
    
    void setPositions(int x, int y) { this->x = x; this->y = y; }

    SDL_Renderer *getRenderer();
};

#endif /* Window_hpp */

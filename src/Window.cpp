//
//  Window.cpp
//  ProgrammedParties
//
//  Created by Harry Culpan on 7/5/19.
//  Copyright © 2019 Harry Culpan. All rights reserved.
//

#include "Window.hpp"
#include "main.hpp"

#include <SDL_ttf.h>

Window::Window() : Window(NULL) {
}

Window::Window(Window *parent) {
    this->parent = parent;
    if (parent) {
        this->parent->children.push_back(this);
    }
}

/**
 * Loads a BMP image into a texture on the rendering device
 * @param file The BMP image file to load
 * @param ren The renderer to load the texture onto
 * @return the loaded texture, or nullptr if something went wrong.
 */
SDL_Texture* Window::loadTexture(const std::string &file, SDL_Renderer *ren) {
    UNUSED(file)
    UNUSED(ren)
/*    //Initialize to nullptr to avoid dangling pointer issues
    SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
    
    // Check if texture loaded
    if (texture == nullptr){
        logSDLError("LoadTexture");
    }
    return texture; */
    return nullptr;
}

void Window::logSDLError(const std::string &msg) {
    std::string errorMsg = msg + " error: " + SDL_GetError();
    SDL_Quit();
    throw errorMsg;
}

/**
 * Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
 * width and height
 * @param tex The source texture we want to draw
 * @param ren The renderer we want to draw to
 * @param x The x coordinate to draw to
 * @param y The y coordinate to draw to
 * @param w The width of the texture to draw
 * @param h The height of the texture to draw
 */
void Window::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h) {
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

/**
 * Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
 * width and height
 * @param tex The source texture we want to draw
 * @param ren The renderer we want to draw to
 * @param x The x coordinate to draw to
 * @param y The y coordinate to draw to
 * @param w The width of the texture to draw
 * @param h The height of the texture to draw
 * @param angle The angle of the texture to draw
 */
void Window::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, double angle) {
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    SDL_RenderCopyEx(ren, tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}

/**
 * Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
 * the texture's width and height
 * @param tex The source texture we want to draw
 * @param ren The renderer we want to draw to
 * @param x The x coordinate to draw to
 * @param y The y coordinate to draw to
 */
void Window::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    renderTexture(tex, ren, x, y, w, h);
}

SDL_Renderer *Window::getRenderer() {
    return getTopParent()->renderer;
};

Window *Window::getTopParent() {
    Window *current = this;
    while (true) {
        if (current->parent != nullptr) {
            current = current->parent;
        } else {
            break;
        }
    }
    return current;
};

bool Window::withinRectangle(int x, int y) {
    return (x >= this->x && x <= this->x + this->w && y >= this->y && y <= this->y + this->h);
}

/**
 * Render the message we want to display to a texture for drawing
 * @param message The message we want to display
 * @param font The font we want to use to render the text
 * @param color The color we want the text to be
 * @param renderer The renderer to load the texture in
 * @return An SDL_Texture containing the rendered message, or nullptr if something went wrong
 */
SDL_Texture* Window::renderText(const std::string &message, TTF_Font *font,
                        SDL_Color color, SDL_Renderer *renderer)
{
    //We need to first render to a surface as that's what TTF_RenderText
    //returns, then load that surface into a texture
    SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
    if (surf == nullptr){
        logSDLError("TTF_RenderText");
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
    if (texture == nullptr){
        logSDLError("CreateTexture");
    }
    //Clean up the surface and font
    SDL_FreeSurface(surf);
    return texture;
}

//
//  MainWindow.cpp
//  ProgrammedParties
//
//  Created by Harry Culpan on 7/5/19.
//  Copyright © 2019 Harry Culpan. All rights reserved.
//

#include "MainWindow.hpp"

#include "main.hpp"
#include "FontManager.hpp"
#include "System.hpp"

#include <SDL2/SDL.h>

#include <sstream>
#include <iterator>
#include <algorithm>

MainWindow *mainWindow;

#define MAP_START_X 405
#define MAP_START_Y 110

bool MainWindow::handleEvent(SDL_Event *e) {
    std::list<Window *>::iterator i;
    bool eventHandled = false;

    for(i = children.begin(); i != children.end(); ++i) {
        eventHandled = (*i)->handleEvent(e);
        if (eventHandled)
            break;
    }
    
    if (!eventHandled) {
        if (e->type == SDL_QUIT){
            loopResult = l_quitting;
            eventHandled = true;
        } else if (e->type == SDL_KEYDOWN && e->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            loopResult = l_escape;
            eventHandled = true;
        } else if (e->type == SDL_KEYDOWN) {
            addCharacter(translateKey(e->key.keysym));
            eventHandled = true;
        }
    }
    
    return eventHandled;
}

MainWindow::MainWindow(int lineSize, int lineCount, int fontSize) : Window() {
    this->m_fontSize = fontSize;
    this->m_lineSize = lineSize;
    this->m_lineCount = lineCount;

    SDL_Surface* text = TTF_RenderText_Solid(fontManager->getFont(fontManager->SOURCECODEPRO, m_fontSize), "g", {128, 128, 128, 255});
    textWidth = text->w;
    textHeight = text->h;
    SDL_FreeSurface(text);

    for (int i = 0; i < m_lineCount; i++) {
        textures[i] = nullptr;
    }

    int screenWidth = textWidth * m_lineSize + 10;
    int screenHeight = textHeight * m_lineCount + 10;

    //Create window
    window = SDL_CreateWindow(
                "KBasic",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                screenWidth,
                screenHeight,
                SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if( window == nullptr ) {
        logSDLError("CreateWindow");
        SDL_Quit();
    }
    
    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);

    dpiModifier = w / screenWidth;
    // dpiModifier = 1.0;
    // game->screenWidth = w;
    // game->screenHeight = h;
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        logSDLError("CreateRenderer");
        SDL_Quit();
    }

    mapKeys();

    addText("KBASIC v1.0");
    addText("Ready");
    addText("_");
}

void MainWindow::mapKeys() {
    keyMap[SDL_SCANCODE_0] = ")";
    keyMap[SDL_SCANCODE_1] = "!";
    keyMap[SDL_SCANCODE_2] = "@";
    keyMap[SDL_SCANCODE_3] = "#";
    keyMap[SDL_SCANCODE_4] = "$";
    keyMap[SDL_SCANCODE_5] = "%";
    keyMap[SDL_SCANCODE_6] = "^";
    keyMap[SDL_SCANCODE_7] = "&";
    keyMap[SDL_SCANCODE_8] = "*";
    keyMap[SDL_SCANCODE_9] = "(";
    keyMap[SDL_SCANCODE_APOSTROPHE] = "\"";
    keyMap[SDL_SCANCODE_COMMA] = "<";
    keyMap[SDL_SCANCODE_PERIOD] = ">";
    keyMap[SDL_SCANCODE_SLASH] = "?";
    keyMap[SDL_SCANCODE_SEMICOLON] = ":";
    keyMap[SDL_SCANCODE_LEFTBRACKET] = "{";
    keyMap[SDL_SCANCODE_RIGHTBRACKET] = "}";
    keyMap[SDL_SCANCODE_EQUALS] = "+";
    keyMap[SDL_SCANCODE_GRAVE] = "~";
    keyMap[SDL_SCANCODE_MINUS] = "_";
    keyMap[SDL_SCANCODE_BACKSLASH] = "|";
}

bool MainWindow::loop() 
{
    return mainLoop();
}

void MainWindow::cleanup() {
    std::list<Window *>::iterator i;
    for(i = children.begin(); i != children.end(); ++i) {
        (*i)->cleanup();
    }
    
    freeTextures();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void MainWindow::render(bool forceRedraw) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    
    renderOutput();
    
    std::list<Window *>::iterator i;
    for(i = children.begin(); i != children.end(); ++i) {
        (*i)->render(forceRedraw);
    }

    SDL_RenderPresent(renderer);
}

void MainWindow::createTextures() {
    freeTextures();

    int offset = (int(consoleText.size()) < m_lineCount ? 0 : consoleText.size() - m_lineCount);
    for (int i = 0; i < m_lineCount; i++) {
        if (i + offset >= int(consoleText.size())) {
            break;
        }

        if (consoleText[i + offset] != "") {
            textures[i] = renderText(
                consoleText[i + offset], 
                fontManager->getFont(FontManager::SOURCECODEPRO, m_fontSize),
                {255, 255, 255, 255},
                getRenderer()
            );
        }
    }
}

void MainWindow::freeTextures() {
    for (int i = 0; i < m_lineCount; i++) {
        if (textures[i]) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = nullptr;
        }
    }
}

void MainWindow::renderOutput() {
    if (consoleTextDirty)
    {
        createTextures();
        consoleTextDirty = false;
    }

    for (int i = 0; i < m_lineCount; i++) {
        if (textures[i]) {
            renderTexture(textures[i], getRenderer(), 5, i * textHeight + 5);
        }
    }
};

void MainWindow::addText(string s, bool append) {
    string t = s;
    if (appendNext)
    {
        t = consoleText.back() + s;
        consoleText.pop_back();
    }
    appendNext = append;
    consoleText.push_back(t);
    consoleTextDirty = true;
}

void MainWindow::addCharacter(string c) {
    if (consoleText.size() == 0) return;

    if (c.size() == 1) {
        string s = consoleText.back();
        consoleText.pop_back();
        if (s.size() > 0 && s.at(s.size() - 1) == '_') {
            s = s.substr(0, s.size() - 1);
        }
        addText(s + c + '_');
        if (loopResult == l_input) inputBuffer += c;
    } else if (c == "return" || c == "Return") {
        string s = consoleText.back();
        consoleText.pop_back();
        if (s.size() > 1 && s.at(s.size() - 1) == '_') {
            s = s.substr(0, s.size() - 1);
        } else if (s.size() == 1) {
            s = "";
        }
        addText(s);
        if (loopResult == l_input)
        {
            loopResult = l_endInput;
        } else 
        {
            core->command(s, this);
            addText("_");
        }
    } else if (c == "capslock" || c == "CapsLock") {
        capsLock = !capsLock;
        if (loopResult == l_input) loopResult = l_endInput;
    } else if (c == "backspace" || c == "Backspace") {
        string s = consoleText.back();
        consoleText.pop_back();
        if (s.size() > 1 && s.at(s.size() - 1) == '_') {
            s = s.substr(0, s.size() - 2);
        } else if (s.size() == 1) {
            s = "";
        }
        addText(s + "_");
        if (loopResult == l_input && inputBuffer.size() > 0) inputBuffer = inputBuffer.substr(0, inputBuffer.size() - 1);
    } else {
//        cout << "Unhandled key: " << c << endl;
    }
}

string MainWindow::translateKey(SDL_Keysym &keysym) const {
    if (keysym.scancode == SDL_SCANCODE_SPACE) {
        return " ";
    } else if (keysym.scancode >= SDL_SCANCODE_A && keysym.scancode <= SDL_SCANCODE_Z) {
        if ((!capsLock && (keysym.mod & KMOD_LSHIFT || keysym.mod & KMOD_RSHIFT))
                    || (capsLock && !(keysym.mod & KMOD_LSHIFT) && !(keysym.mod & KMOD_RSHIFT))) {
            auto search = keyMap.find(keysym.scancode);
            if (search != keyMap.end()) {
                return search->second;
            }
            return SDL_GetScancodeName(keysym.scancode);
        } else {
            string result(SDL_GetScancodeName(keysym.scancode));
            transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c){ return std::tolower(c); });
            return result;
        }
    } else if (keysym.mod & KMOD_LSHIFT || keysym.mod & KMOD_RSHIFT) {
        auto search = keyMap.find(keysym.scancode);
        if (search != keyMap.end()) {
            return search->second;
        }
        return SDL_GetScancodeName(keysym.scancode);
    } else {
        string result(SDL_GetScancodeName(keysym.scancode));
        transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c){ return std::tolower(c); });
        return result;
    }
}

void MainWindow::addTextAt(int location, string s)
{
    UNUSED(location)

    string t = s;
    appendNext = false;
    consoleText.push_back(t);
    consoleTextDirty = true;
}

void MainWindow::clearText() {
    consoleText.clear();
}

void MainWindow::terminate() {
    SDL_Event sdlevent;
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);
}

float MainWindow::inputNumber(string prompt)
{
    float result = 0.0;
    addText(prompt + "? ");    

    loopResult = l_input;
    inputBuffer = "";

    bool quitting = false;
    while (!quitting){
        quitting = (mainLoop() == l_quitting);
        if (loopResult == l_escape)
        {
            addText("Break");
            inputBuffer = "";
            break;
        } else if (loopResult == l_endInput && isFloat(inputBuffer)) 
        {
            result = stof(inputBuffer);
            break;
        } else if (loopResult == l_endInput && inputBuffer == "")
        {
            break;
        } else if (loopResult == l_endInput) 
        {
            addText("Type mismatch");
            addText(prompt + "? ");
            inputBuffer = "";
            loopResult = l_input;
        }
    }

    if (loopResult == l_escape) loopResult = l_end;
    else loopResult = l_running;
    return result;
}

string MainWindow::inputString(string prompt)
{
    addText(prompt + "?");    

    loopResult = l_input;
    inputBuffer = "";

    bool quitting = false;
    while (!quitting){
        quitting = (mainLoop() == l_quitting);
        if (loopResult == l_endInput) break;
        else if (loopResult == l_escape) 
        {
            addText("Break");
            break;
        }
    }

    if (loopResult == l_escape) loopResult = l_end;
    else loopResult = l_running;
    return inputBuffer;
}
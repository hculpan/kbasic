//
//  MainWindow.hpp
//  ProgrammedParties
//
//  Created by Harry Culpan on 7/5/19.
//  Copyright © 2019 Harry Culpan. All rights reserved.
//

#ifndef MainWindow_hpp
#define MainWindow_hpp

#include "Window.hpp"
#include "ConsoleOutput.hpp"

#include <vector>
#include <array>
#include <unordered_map>

using namespace std;

class MainWindow : public Window, ConsoleOutput {
private:
    SDL_Window *window;

    int textWidth = 0;
    int textHeight = 0;

    bool capsLock = false;

    void renderOutput();

    vector<string> consoleText;
    array<SDL_Texture *, 25> textures;
    unordered_map<int, string> keyMap;

    void mapKeys();
    string translateKey(SDL_Keysym &sym) const;

    void freeTextures();
    void createTextures();

public:
    MainWindow();
    
    virtual void render(bool forceRedraw);
    virtual void cleanup();
    virtual bool handleEvent(SDL_Event *e);

    void addCharacter(string c);

    // ConsoleOutput interface
    void addText(string s);
    void clearText();
    void terminate();
};

#endif /* MainWindow_hpp */

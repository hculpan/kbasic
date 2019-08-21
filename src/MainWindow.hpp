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
#include "Console.hpp"
#include "main.hpp"

#include <vector>
#include <array>
#include <unordered_map>

class MainWindow : public Window, Console {
public:
    MainWindow(int lineSize, int lineCount, int fontSize);
    
    virtual void render(bool forceRedraw);
    virtual void cleanup();
    virtual bool handleEvent(SDL_Event *e);

    void addCharacter(string c);

    int fontSize() { return m_fontSize; }

    // ConsoleOutput interface
    void addText(string s, bool append = false);
    void addTextAt(int location, string s);
    void clearText();
    void terminate();
    inline int lineSize() { return m_lineSize; };
    bool loop();
    float inputNumber(string prompt);
    string inputString(string prompt);

private:
    SDL_Window *window;

    int m_fontSize;
    int m_lineSize;
    int m_lineCount;

    int textWidth = 0;
    int textHeight = 0;

    bool capsLock = false;
    bool appendNext = false;

    string inputBuffer;

    void renderOutput();

    bool consoleTextDirty = false;

    vector<string> consoleText;
    array<SDL_Texture *, SCREEN_HEIGHT> textures;
    unordered_map<int, string> keyMap;

    void mapKeys();
    string translateKey(SDL_Keysym &sym) const;

    void freeTextures();
    void createTextures();

};

extern MainWindow *mainWindow;

#endif /* MainWindow_hpp */

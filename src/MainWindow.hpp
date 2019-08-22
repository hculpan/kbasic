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
#include <chrono>

using namespace std::chrono;

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
    void putTextAt(int location, string s, bool append = false);
    void clearText();
    void terminate();
    inline int lineSize() { return m_lineSize; };
    inline int lineCount() { return m_lineCount; }
    bool loop();
    float inputNumber(string prompt);
    string inputString(string prompt);

private:
    SDL_Window *window;

    int m_fontSize;
    int m_lineSize;
    int m_lineCount;

    int m_screenWidth;
    int m_screenHeight;

    int m_cursorPos = 0;
    int m_cursorLine = 0;
    
    int m_inputStartPos = 0;
    string m_inputBuffer = "";

    int textWidth = 0;
    int textHeight = 0;

    bool capsLock = false;

    void renderOutput();
    void renderCursor();
    time_point<high_resolution_clock> m_lastCursorUpdate = high_resolution_clock::now();
    bool m_cursorOn = true;

    void newLine();

    bool consoleTextDirty = false;

    vector<string> m_text;
    SDL_Texture *m_screenTexture = nullptr;
    SDL_Texture *m_cursorTexture = nullptr;
    unordered_map<int, string> keyMap;

    void mapKeys();
    string translateKey(SDL_Keysym &sym) const;

    void freeTextures();
    void createTextures();

};

extern MainWindow *mainWindow;

#endif /* MainWindow_hpp */

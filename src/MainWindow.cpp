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
#include <chrono>
#include <cwctype>

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

    m_text = vector<string>(m_lineCount);

    SDL_Surface* text = TTF_RenderText_Solid(fontManager->getFont(fontManager->SOURCECODEPRO, m_fontSize), "g", {128, 128, 128, 255});
    textWidth = text->w;
    textHeight = text->h;
    SDL_FreeSurface(text);

    for (int i = 0; i < m_lineCount; i++) {
        m_text[i] = string(m_lineSize, ' ');
    }

    m_screenWidth = textWidth * m_lineSize + 10;
    m_screenHeight = textHeight * m_lineCount + 10;

    //Create window
    window = SDL_CreateWindow(
                "KBasic",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                m_screenWidth,
                m_screenHeight,
                SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if( window == nullptr ) {
        logSDLError("CreateWindow");
        SDL_Quit();
    }
    
    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);

    dpiModifier = w / m_screenWidth;
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

    if (m_cursorTexture) SDL_DestroyTexture(m_cursorTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void MainWindow::render(bool forceRedraw) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    renderOutput();
    renderCursor();
    
    std::list<Window *>::iterator i;
    for(i = children.begin(); i != children.end(); ++i) {
        (*i)->render(forceRedraw);
    }

    SDL_RenderPresent(renderer);
}

void MainWindow::createTextures() {
    freeTextures();

//    SDL_Renderer *target = getRenderer();

    m_screenTexture = SDL_CreateTexture(getRenderer(), 
            SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_TARGET,
            m_screenWidth,
            m_screenHeight);

    SDL_SetRenderTarget(getRenderer(), m_screenTexture);
    SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 0);
    SDL_RenderClear(getRenderer());

    for (int i = 0; i < m_lineCount; i++) {
        SDL_Texture *t = renderText(m_text[i],
            fontManager->getFont(FontManager::SOURCECODEPRO, m_fontSize),
            {255, 255, 255, 255},
            getRenderer());
        
        renderTexture(t, getRenderer(), 5, i * textHeight + 5);
    }

    SDL_SetRenderTarget(getRenderer(), NULL);
    consoleTextDirty = false;
}

void MainWindow::freeTextures() {
    if (m_screenTexture) SDL_DestroyTexture(m_screenTexture);
}

void MainWindow::renderCursor() {
    if (!m_cursorTexture)
    {    
        m_cursorTexture = renderText("_", fontManager->getFont(fontManager->SOURCECODEPRO, m_fontSize), {255, 255, 255, 255}, getRenderer());
    }

    auto finish = high_resolution_clock::now();
    duration elapsed = duration_cast<milliseconds>(finish - m_lastCursorUpdate);
    if (elapsed.count() > 500)
    {
        m_lastCursorUpdate = finish;
        m_cursorOn = !m_cursorOn;
    } 

    
    if (executionStatus != ex_executing || loopResult == l_input)
    {
        if (m_cursorOn)
        {
            renderTexture(m_cursorTexture, getRenderer(), m_cursorPos * textWidth + 5, m_cursorLine * textHeight + 5);    
        }
    }
    
}

void MainWindow::renderOutput() {
    if (consoleTextDirty)
    {
        createTextures();
        consoleTextDirty = false;
    }

    renderTexture(m_screenTexture, getRenderer(), 0, 0);
};

void MainWindow::addText(string s, PrintAppendMode appendMode) {
    m_text[m_cursorLine].replace(m_cursorPos, s.size(), s);
    if (appendMode == pam_none) 
    {
        newLine();
        m_cursorPos = 0;
    } else if (appendMode == pam_tab) {
        m_cursorPos = (int(m_cursorPos/10) + 1) * 10;
        if (m_cursorPos > m_lineSize) 
        {
            newLine();
            m_cursorPos = 0;
        }
    } else 
    {
        m_cursorPos += s.size();
    }
    consoleTextDirty = true;
}

void MainWindow::newLine()
{
    if (m_cursorLine >= m_lineCount - 1)
    {
        for (int i = 0; i < m_lineCount - 1; i++)
        {
            m_text[i] = m_text[i+1];
        }
        m_text[m_lineCount - 1] = string(m_lineSize, ' ');
    } else 
    {
        m_cursorLine++;
    }
    consoleTextDirty = true;
}

void MainWindow::addCharacter(string c) {
    if (c == "capslock" || c == "CapsLock") {
        capsLock = !capsLock;
        return;
    } else if (loopResult != l_running && loopResult != l_input) return;

    if (c.size() == 1) {
        if (m_cursorPos >= m_lineSize)
        {
            m_cursorPos = 0;
        }

        m_text[m_cursorLine].replace(m_cursorPos++, 1, c);
        consoleTextDirty = true;
    } else if (c == "return" || c == "Return") {
        if (loopResult == l_input)
        {
            loopResult = l_endInput;
            m_inputBuffer = m_text[m_cursorLine].substr(m_inputStartPos, m_lineSize);
            rtrim(m_inputBuffer);
            m_cursorPos=0;
            newLine();
        } else 
        {
            // Just in case there happens to be anything after the 
            // cursor when you hit Return
            string s = m_text[m_cursorLine];
            m_cursorPos = 0;
            newLine();
            core->command(s, this);
            m_cursorPos = 0;
        }
    } else if (c == "backspace" || c == "Backspace") {
        if ((m_cursorPos > 0 && loopResult != l_input) || m_cursorPos > m_inputStartPos)
        {
            m_text[m_cursorLine].replace(m_cursorPos - 1, 1, " ");
            m_cursorPos--;
            consoleTextDirty = true;
        }
    } else if (c == "home")
    {
        if (loopResult != l_input) m_cursorPos = 0;
        else if (loopResult == l_input) m_cursorPos = m_inputStartPos;
    } else if (c == "end")
    {
        int i = m_lineSize - 1;
        for (; i >= 0; i--) if (!iswspace(m_text[m_cursorLine].at(i))) break;
        m_cursorPos = i + 1;
        if (loopResult == l_input && i < m_inputStartPos)
        {
            m_cursorPos = m_inputStartPos;
        } else if (loopResult != l_input && m_cursorPos >= m_lineSize - 1)
        {
            newLine();
            m_cursorPos = 0;
        } 
    } else if (c == "left")
    {
        if ((m_cursorPos > 0 && loopResult != l_input) || m_cursorPos > m_inputStartPos) m_cursorPos--;
    } else if (c == "right")
    {
        if (m_cursorPos< m_lineSize - 1) m_cursorPos++;
    } else if (c == "up")
    {
        if (m_cursorLine > 0 && loopResult != l_input) m_cursorLine--;
    } else if (c == "down")
    {
        if (m_cursorLine < m_lineCount - 1 && loopResult != l_input) m_cursorLine++;
        else if (loopResult != l_input) newLine();
    } else if (c == "delete")
    {
        m_text[m_cursorLine] = m_text[m_cursorLine].substr(0, m_cursorPos) + 
                               m_text[m_cursorLine].substr(m_cursorPos +1, m_lineSize) +
                               " ";
        consoleTextDirty = true;
    } else {
        cout << "Unhandled key: " << c << endl;
    }
}

string MainWindow::translateKey(SDL_Keysym &keysym) const {
    return translateKey(keysym.scancode, (keysym.mod & KMOD_LSHIFT || keysym.mod & KMOD_RSHIFT));
}

string MainWindow::translateKey(SDL_Scancode scancode, bool shifted) const {
    if (scancode == SDL_SCANCODE_SPACE) {
        return " ";
    } else if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
        if ((!capsLock && shifted) || (capsLock && !shifted)) {
            auto search = keyMap.find(scancode);
            if (search != keyMap.end()) {
                return search->second;
            }
            return SDL_GetScancodeName(scancode);
        } else {
            string result(SDL_GetScancodeName(scancode));
            transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c){ return std::tolower(c); });
            return result;
        }
    } else if (shifted) {
        auto search = keyMap.find(scancode);
        if (search != keyMap.end()) {
            return search->second;
        }
        return SDL_GetScancodeName(scancode);
    } else {
        string result(SDL_GetScancodeName(scancode));
        transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c){ return std::tolower(c); });
        return result;
    }
}

void MainWindow::putTextAt(int location, string s, PrintAppendMode pmode)
{
    if (location >= m_lineSize * m_lineCount) return;

    m_cursorLine = location / m_lineSize;
    m_cursorPos = location % m_lineSize;

    addText(s, pmode);
}

void MainWindow::clearText() {
    for (int i = 0; i < m_lineCount; i++)
    {
        m_text[i] = string(m_lineSize, ' ');
    }

    m_cursorPos = 0;
    m_cursorLine = 0;
    consoleTextDirty = true;
}

void MainWindow::terminate() {
    SDL_Event sdlevent;
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);
}

string MainWindow::getKey()
{
    int numKeys;
    const Uint8 *state = SDL_GetKeyboardState(&numKeys);
    bool shifted = (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]);

    for (int i = 0; i < numKeys; i++)
    {
        if (i == SDL_SCANCODE_LSHIFT || i == SDL_SCANCODE_RSHIFT) continue;

        if (state[i]) return translateKey(static_cast<SDL_Scancode>(i), shifted);
    }

    return "";
}

float MainWindow::inputNumber(string prompt)
{
    float result = 0.0;
    addText(prompt + "? ", pam_append);    

    LoopStatus oldLoopResult = loopResult;
    loopResult = l_input;
    m_inputStartPos = m_cursorPos;

    bool quitting = false;
    while (!quitting){
        quitting = (mainLoop() == l_quitting);
        if (loopResult == l_escape)
        {
            addText("Break");
            break;
        } else if (loopResult == l_endInput)
        {
            if (m_inputBuffer == "")
            {
                break;
            } else if  (isFloat(m_inputBuffer)) 
            {
                result = stof(m_inputBuffer);
                break;
            } else 
            {
                addText("Type mismatch");
                addText(prompt + "? ", pam_append);
                loopResult = l_input;
            }
        }
    }

    if (loopResult == l_escape) loopResult = l_end;
    else loopResult = oldLoopResult;
    return result;
}

string MainWindow::inputString(string prompt)
{
    addText(prompt + "? ", pam_append);    

    LoopStatus oldLoopResult = loopResult;
    loopResult = l_input;
    m_inputStartPos = m_cursorPos;

    bool quitting = false;
    while (!quitting){
        quitting = (mainLoop() == l_quitting);
        if (loopResult == l_endInput) break;
        else if (loopResult == l_escape) 
        {
            m_inputBuffer = "";
            addText("Break");
            break;
        }
    }

    if (loopResult == l_escape) loopResult = l_end;
    else loopResult = oldLoopResult;
    return m_inputBuffer;
}

CursorPos MainWindow::getCursorPos()
{
    return CursorPos(m_cursorPos, m_cursorLine);
}

void MainWindow::setCursorPos(const CursorPos &pos)
{
    m_cursorPos = pos.col;
    m_cursorLine = pos.row;
}

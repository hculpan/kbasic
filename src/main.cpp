//
//  main.cpp
//  ProgrammedParties
//
//  Created by Harry Culpan on 7/3/19.
//  Copyright © 2019 Harry Culpan. All rights reserved.
//

//Using SDL and standard IO
#include <iostream>
#include <sstream>
#include <algorithm>

#include <CoreFoundation/CFBundle.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include "main.hpp"
#include "MainWindow.hpp"

double dpiModifier = 1.0;

LoopStatus loopResult = l_running;
ExecutionStatus executionStatus = ex_done;

string resourcePath = "";

void logSDLError(const std::string &msg);
bool initGraphics();
string findResourcePath();

LoopStatus mainLoop()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        mainWindow->handleEvent(&e);
        if (loopResult == l_quitting) {
            break;
        }
    }
    
    mainWindow->render(false);

    return loopResult;
}

LoopStatus singleLoop()
{
    SDL_Event e;
    if (SDL_PollEvent(&e)) mainWindow->handleEvent(&e);
    
    mainWindow->render(false);

    return loopResult;
}

int main(int argc, const char * argv[]) {
    UNUSED(argc)
    UNUSED(argv)

    resourcePath = findResourcePath();

    if (!initGraphics()) {
        return 1;
    };
    
    fontManager->initialize();
    mainWindow = new MainWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 24);

    bool quitting = false;
    while (!quitting){
        quitting = (mainLoop() == l_quitting);
    }

    mainWindow->cleanup();
    return 0;
}

void logSDLError(const std::string &msg) {
    std::cout << msg << " error: " << SDL_GetError() << std::endl;
}

bool initGraphics() {
    //Initialize SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {
        logSDLError("SDL_Init");
        return false;
    }
    
    if (TTF_Init() != 0){
        logSDLError("TTF_Init");
        SDL_Quit();
        return false;
    }
    
    return true;
}

bool isFloat( string myString ) {
    std::istringstream iss(myString);
    float f;
    iss >> noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}

bool isInteger(string s)
{
    bool result = true;
    for (char const &c : s)
    {
        if (!isdigit(c))
        {
            result = false;
            break;
        }
    }
    return result;
}

string findResourcePath() {
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyBundleURL(bundle);
	CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
	CFRelease(resourcesURL);
	char path[PATH_MAX];
    CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
    return string(path) + "/Contents/Resources/";
}

void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](int ch) {
        return !isspace(ch);
    }).base(), s.end());
}



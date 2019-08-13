//
//  main.cpp
//  ProgrammedParties
//
//  Created by Harry Culpan on 7/3/19.
//  Copyright © 2019 Harry Culpan. All rights reserved.
//

//Using SDL and standard IO
#include <iostream>

#include <CoreFoundation/CFBundle.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include "main.hpp"

double dpiModifier = 1.0;

bool quitting = false;

string resourcePath = "";

MainWindow *mainWindow;

void logSDLError(const std::string &msg);
bool initGraphics();
string findResourcePath();

int main(int argc, const char * argv[]) {
    UNUSED(argc)
    UNUSED(argv)

    resourcePath = findResourcePath();

    if (!initGraphics()) {
        return 1;
    };
    
    fontManager->initialize();
    mainWindow = new MainWindow();

    SDL_Event e;
    while (!quitting){
        while (SDL_PollEvent(&e)){
            mainWindow->handleEvent(&e);
            if (quitting) {
                break;
            }
        }
        
        mainWindow->render(false);
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

string findResourcePath() {
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyBundleURL(bundle);
	CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
	CFRelease(resourcesURL);
	char path[PATH_MAX];
    CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
    return string(path) + "/Contents/Resources/";
}



//
//  FontManager.hpp
//  CityState
//
//  Created by Harry Culpan on 7/8/19.
//  Copyright © 2019 Harry Culpan. All rights reserved.
//

/*
 * To add new font family:
 *  1) Create static constant with next int value
 *  2) Add file name to fontFileNames vector in initialize()
 *  3) Use new font! :)
 *
 */

#ifndef FontManager_hpp
#define FontManager_hpp

#include <SDL_ttf.h>

#include <string>
#include <map>
#include <vector>

typedef std::map<int, TTF_Font *> FontMap;

class FontManager {
private:
    std::vector<FontMap *> fontFamiliesArray;
    
    std::vector<std::string> fontFileNames;
    
public:
    static const int SOURCECODEPRO = 0;
    static const int SOURCECODEPRO_BOLD = 1;

    void initialize();
    
    void cleanup();
    
    TTF_Font *getFont(int fontFamily, int fontSize);
};

extern FontManager *fontManager;

#endif /* FontManager_hpp */

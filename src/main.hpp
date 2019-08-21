#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include "MainWindow.hpp"
#include "FontManager.hpp"

#include <string>

using namespace std;

#define UNUSED(a)   (void)a;

#define PATH_MAX 1024

extern double dpiModifier;

extern MainWindow *mainWindow;

enum LoopStatus { l_running, l_quitting, l_escape, l_end, l_input, l_endInput };

extern LoopStatus loopResult;
extern string resourcePath;

const int SCREEN_WIDTH = 1240;
const int SCREEN_HEIGHT = 940;

extern LoopStatus mainLoop();

extern bool isFloat( string myString );

extern bool isInteger(string s);

#endif
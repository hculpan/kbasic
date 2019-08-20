#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include "MainWindow.hpp"
#include "FontManager.hpp"

#include <string>

using namespace std;

#define UNUSED(a)   (void)a;

extern double dpiModifier;

extern MainWindow *mainWindow;

enum LoopStatus { l_running, l_quitting, l_escape, l_end };

extern LoopStatus loopResult;
extern string resourcePath;

const int SCREEN_WIDTH = 1240;
const int SCREEN_HEIGHT = 940;

extern LoopStatus mainLoop();

#endif
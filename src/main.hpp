#ifndef _MAIN_HPP_
#define _MAIN_HPP_

#include "FontManager.hpp"

#include <string>

using namespace std;

#define UNUSED(a)   (void)a;

#define PATH_MAX 1024

extern double dpiModifier;

enum LoopStatus { l_running, l_quitting, l_escape, l_end, l_input, l_endInput };

enum ExecutionStatus { ex_executing, ex_done };

extern LoopStatus loopResult;
extern string resourcePath;
extern ExecutionStatus executionStatus;

#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT  25

extern LoopStatus mainLoop();

extern bool isFloat( string myString );

extern bool isInteger(string s);
extern void rtrim(string &s);

#endif
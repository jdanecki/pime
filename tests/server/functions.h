#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <ncurses.h>
#include <string.h>

#define MAX_OUTPUT_SIZE 189

extern WINDOW * out_w;
extern WINDOW * status;
extern int win_w, win_h;
extern int buf_pos;
extern bool scrolling;
extern int history_up;
extern char ** out_buf;
extern int history_size;
extern int col;

void print_status(const char * format, ...);
void ncurses_tick_test();
int add_to_output(const char * fmt, ...);
void shift_output();
#define MAX_OUTPUT_SIZE 189

#endif

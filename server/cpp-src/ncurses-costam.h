#ifndef NCURSES_costam
#define NCURSES_costam
#ifdef USE_ENET
extern void add_to_output(const char * fmt, ...);
#define printf add_to_output
#endif

#endif // !NCURSES

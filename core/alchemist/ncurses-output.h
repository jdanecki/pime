#ifndef NCURSES_output
#define NCURSES_output

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_NCURSES
extern int add_to_output(const char * fmt, ...);
#define CONSOLE_LOG add_to_output
#else
#define CONSOLE_LOG printf
#endif

#ifdef __cplusplus
}
#endif

#endif

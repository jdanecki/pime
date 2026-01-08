#ifndef KEY_HANDLERS_H
#define KEY_HANDLERS_H

extern void quit_program();
extern void handle_i();
extern void handle_a();
extern void handle_k();
extern void handle_escape();
#ifndef DISABLE_NPC
extern void handle_n();
#endif

void handle_f1();
void handle_f2();
void handle_f3();
void handle_f4();
void handle_f5();
void handle_f6();
void handle_f7();
void handle_f11();

void handle_enter();
void handle_c();
void handle_hotbar_0();
void handle_hotbar_1();
void handle_hotbar_2();
void handle_hotbar_3();
void handle_hotbar_4();
void handle_hotbar_5();
void handle_hotbar_6();
void handle_hotbar_7();
void handle_hotbar_8();
void handle_hotbar_9();
void handle_left();
void handle_right();
void handle_up();
void handle_down();
void handle_prev_hotbar();
void handle_next_hotbar();
void handle_minus();
void handle_equal();


extern bool finish_program;


#endif // KEY_HANDLERS_H

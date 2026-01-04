#include <curses.h>

int main()
{
	WINDOW * win;
	int y1,x1,x,y;
    short fg = COLOR_RED;
    short bg = COLOR_BLUE;
	short pair=1;
    int key;
    char str[30];

	initscr();
	cbreak();
	noecho();
	nonl();

	start_color();
	win=newwin(10,50,5,20);
	keypad(win,TRUE);
    init_pair(pair, fg, bg);
    //  wattron(win, (attr_t) COLOR_PAIR(pair));
	wcolor_set(win, pair, NULL);
	
	getmaxyx(win, y1, x1);
    mvwaddstr(win, 1, 5, "window");
  	x=10;y=10;
    do
    {
        switch(key=wgetch(win))
        {
                case KEY_UP:	y--; break;
                case KEY_DOWN:	y++; break;
                case KEY_LEFT:	x--; break;
                case KEY_RIGHT:	x++; break;
        }		
    	sprintf(str, "x=%u y=%u key=%d char=%c\n",x,y, key, key);
        addstr(str);
        refresh();
    
//        mvwin(win,y,x);
//        wrefresh(win);
        refresh();
//        touchwin(win);
//       wnoutrefresh(win);

        doupdate();
    } while(key!='q');

	delwin(win);
  endwin();

}

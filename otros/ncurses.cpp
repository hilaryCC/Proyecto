#include <ncurses.h>
#include <string.h>
#include <string>

using namespace std;

string getstring(){
    string input;

    // let the terminal do the line editing
    nocbreak();
    echo();
    // this reads from buffer after <ENTER>, not "raw"
    // so any backspacing etc. has already been taken care of
    int ch = getch();

    while ( ch != 10 )
    {
        input.push_back( ch );
        ch = getch();
    }

    // restore your cbreak / echo settings here

    return input;
}

void principal(WINDOW *menu, WINDOW *info){
  box(menu, 42, 0);
  box(info, 42, 0);

  mvwaddstr(menu, 0, 2, "Menu");
  mvwaddstr(info, 0, 2, "Info");

  wrefresh(menu);
  wrefresh(info);
  string opciones[7] = {"1. Cargar", "2. Ver rutas", "3. Comparar", "4. Generar", "5. Crear", "6. Inspeccionar", "7. Salir"};
  for (size_t i = 0; i < 7; i++) {
    mvwaddstr(menu, i+2, 1, opciones[i].c_str());
  }
  keypad(menu_win, true);
}

int main()
{
 int alto, ancho, y_inicio, x_inicio, filas, column;
 alto = 11;
 ancho = 30;
 initscr();
 getmaxyx(stdscr,filas,column);

 WINDOW * menu_win = newwin(alto, 20, filas-alto-4, column-22);
 WINDOW * info_win = newwin(alto, ancho, 1, column-ancho-2);
 refresh();

 bool running = true;
 int opcion;
 int select = 0;
 char input[80];

 while(running){
   werase(stdscr);
   principal(menu_win, info_win);
   wmove(menu_win, 9, 1);
   opcion = wgetch(menu_win);

   switch (opcion) {
         case 49:
              mvwaddstr(info_win, 1, 1, "Cargar xml");
              wrefresh(info_win);
              move(0,0);
              getstr(input);
              werase(stdscr);
              printw("Path: %s", input);
              refresh();
              break;
         case 50:
              mvwaddstr(info_win, 1, 1, "Ver rutas\n disponibles");
              break;
         case 51:
              mvwaddstr(info_win, 1, 1, "Comparar dos rutas");
              break;
         case 52:
              mvwaddstr(info_win, 1, 1, "Generar grafo");
              break;
         case 53:
              mvwaddstr(info_win, 1, 1, "Crear nueva ruta");
              break;
         case 54:
              mvwaddstr(info_win, 1, 1, "Modo inspeccionar");
              break;
         case 55:
              mvwaddstr(info_win, 1, 1, "Salir");
              running = false;
              break;
         default:
              mvwaddstr(info_win, 1, 1, "Opcion invalida");
       }
       wrefresh(info_win);
   }

 getch();
 endwin();

 return 0;
}

   /*if(opcion == 10){
     werase(info_win);
     box(info_win, 42, 0);
     mvwprintw(info_win, 0, 2, "Info");
     string input = "";
     switch (select) {
       case 0:
            mvwprintw(info_win, 1, 1, "Cargar xml");
            wrefresh(info_win);
            input = getstring();
            printw("Path: %s", input.c_str());
            refresh();
            break;
       case 1:
            mvwprintw(info_win, 1, 1, "Ver rutas\n disponibles");
            wrefresh(info_win);
            break;
       case 2:
            mvwprintw(info_win, 1, 1, "Comparar dos rutas");
            wrefresh(info_win);
            break;
       case 3:
            mvwprintw(info_win, 1, 1, "Generar grafo");
            wrefresh(info_win);
            break;
       case 4:
            mvwprintw(info_win, 1, 1, "Crear nueva ruta");
            wrefresh(info_win);
            break;
       case 5:
            mvwprintw(info_win, 1, 1, "Modo inspeccionar");
            wrefresh(info_win);
            break;
       case 6:
            mvwprintw(info_win, 1, 1, "Salir");
            wrefresh(info_win);
            running = false;
            break;
     }
   }*/

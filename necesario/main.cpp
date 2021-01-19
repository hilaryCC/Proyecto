#include "pugixml.hpp"
#include "pugiconfig.hpp"
#include <string.h>
#include <iostream>
#include <cstring>
#include "pathway.h"
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string>
#include <ctype.h>
using namespace std;

Pathway *primero, *ultimo;
WINDOW * menu_win, * info_win;

void esfinal2(){
  int y,x;
  int maxy, maxx;
  getyx(stdscr, y, x);
  getmaxyx(stdscr, maxy,maxx);
  if(y+3 >= maxy){
    move(maxy,0);
    printw(".:Continuar:.");
    refresh();
    getch();
    werase(stdscr);
    refresh();
  }
}

void principal(WINDOW *menu, WINDOW *info){
  wrefresh(menu);
  wrefresh(info);

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
  keypad(menu, true);
  keypad(stdscr, true);
  refresh();
  wrefresh(menu);
  wrefresh(info);
}

int largoPat(){
  int cont = 0;
  Pathway *aux = primero;
  while(aux!=NULL){
    cont++;
    aux=aux->getSig();
  }
  return cont;
}

bool digito(string str){
  for(int i = 0; i < (int)str.length();i++){
    if(!isdigit(str.at(i)))
      return false;
  }
  return true;
}

string getstring(){
    string input;
    nocbreak();
    echo();
    int ch = getch();

    while ( ch != '\n' )
    {
        input.push_back( ch );
        ch = getch();
    }
    cbreak();
    return input;
}

void addPathway(Pathway * nuevo){
  if(primero == NULL){
    primero = nuevo;
    ultimo = nuevo;
  }
  else{
    ultimo->setSig(nuevo);
    ultimo = nuevo;
  }
}

void printPathways(){
  Pathway * aux = primero;
  while(aux != NULL){
    aux->print();
    aux = aux->getSig();
  }
}

void verPathways(){
  Pathway *aux = primero;
  int cont = 1;
  std::string nuevo;
  if(aux == NULL){
    printw("No hay rutas disponibles\n");
    refresh();
  }
  while(aux != NULL){
    nuevo = std::to_string(cont)+". "+aux->getNombre()+"\n\t -> "+aux->getTitulo();
    if(esfinal()){
      principal(menu_win, info_win);
      getch();
      werase(stdscr);
      refresh();
      principal(menu_win, info_win);
    }
    printw(" %s\n", nuevo.c_str());
    refresh();
    principal(menu_win, info_win);
    aux = aux->getSig();
    cont++;
  }
}

void crearDot(int num, string out){
  Pathway *nuevo = primero;
  int cont = 1;
  while(nuevo != NULL && cont < num){
    nuevo = nuevo->getSig();
    cont++;
  }
  if(nuevo!=NULL && !nuevo->empty()){
    string nombre = nuevo->getTitulo();
    string line = " [style=dotted];\n";
    string shape = "[shape=box];\n";
    string label = " [label=\"";
    string flecha = " -> {";
    string filen = "grafos/"+out+".dot";
    Entry *actual = nuevo->getPrimero();
    ofstream file;
    file.open(filen);
    file << "digraph grafo{\n"
         << "a" << label << nombre << "\"] " << shape;
    while(actual != NULL){
      Reaction *aux = actual->getRPrimero();
      int id = actual->getId();
      file << id << label << actual->getNombre() << "\"];\n"
           << id << flecha;
      while(aux != NULL){
        int aid = aux->getId();
        file << aid << " ";
        aux = aux->getSig();
      }
      file << "};\n";
      actual = actual->getSig();
    }
    file << "}";
    file.close();
    string cg = "dot -Tpng "+filen+" -o grafos/"+out+".png";
    string og = "xdg-open grafos/"+out+".png";
    char cgraph[cg.size()];
    char ograph[og.size()];
    strcpy(cgraph, cg.c_str());
    strcpy(ograph, og.c_str());
    system (cgraph);
    system (ograph);
  }
}

bool createPathway(const char* path){
  pugi::xml_document doc;
  if (!doc.load_file(path)) return false; //hay un error en el archivo

  pugi::xml_node tools = doc.child("pathway");
  if (tools != NULL) {
        string nTitulo = tools.attribute("title").value();
        printw("\nNombre del pathway: %s\n", tools.attribute("name").value());
        printw("Titulo del pathway: %s\n", nTitulo.c_str());
        printw("\n¿Desea cambiar titulo? {S/N}");
        refresh();
        principal(menu_win, info_win);
        printw("");
        refresh();
        char cambiar = getch();
        if(cambiar == 83 || cambiar == 115 || cambiar == 10){
          nTitulo = "";
          while(strcmp(nTitulo.c_str(), "") == 0){
            if(esfinal()){
              principal(menu_win, info_win);
              getch();
              werase(stdscr);
              refresh();
              principal(menu_win, info_win);
            }
            printw("\nIngrese el nuevo titulo: ");
            refresh();
            principal(menu_win, info_win);
            printw("");
            refresh();
            nTitulo = getstring();
          }
          printw("Nuevo nombre: %s\n", nTitulo.c_str());
          esfinal();
          principal(menu_win, info_win);
        }

        Pathway * nuevo = new Pathway(tools.attribute("name").value(), nTitulo);
        int contSubs = 0;
        int contProd = 0;



        for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling())
        {
            if(strcmp(tool.name(), "entry")==0 &&
                    strcmp(tool.attribute("type").value(), "compound") == 0){ //Busca todos los compuestos del pathway
                Entry *newEntry = new Entry(tool.attribute("name").value(), tool.attribute("id").as_int());
                nuevo->addEntry(newEntry);
            }

            if(strcmp(tool.name(), "reaction")==0){//Busca las reacciones
                contSubs = 0;
                contProd = 0;
                int type = 0;
                if(strcmp(tool.attribute("type").value(), "reversible")==0)
                    type = 1;
                else
                    type = 2;
                //cuenta la cantidad de sustratos y productos
                for (pugi::xml_node to = tool.first_child(); to; to = to.next_sibling()){
                    if(strcmp(to.name(), "substrate") == 0)
                        contSubs++;
                    else if(strcmp(to.name(), "product") == 0)
                            contProd++;
                }
                //Si existe al menos uno de cada uno, crea las listas con los id de cada uno
                if(contProd != 0 && contSubs != 0){
                    vector<int> subs(contSubs);
                    vector<int> prods(contProd);
                    contSubs = 0;
                    contProd = 0;

                    for (pugi::xml_node to = tool.first_child(); to; to = to.next_sibling()){
                        if(strcmp(to.name(), "substrate") == 0){
                            subs[contSubs] = to.attribute("id").as_int();
                            contSubs++;
                        }
                        else if(strcmp(to.name(), "product") == 0){
                            prods[contProd] = to.attribute("id").as_int();
                            contProd++;
                        }
                    }
                    //a cada sustrato se le agregan todos los productos segun el tipo de relacion
                    for(int sub = 0; sub < (int)subs.size(); sub++){
                        Entry * subActual = nuevo->getEntry(subs[sub]);

                        for(int prod = 0; prod < (int)prods.size(); prod++){
                            Entry * prodActual = nuevo->getEntry(prods[prod]);

                            if(subActual != NULL && prodActual != NULL){
                                if(type == 1){
                                    Reaction * nuevaRs = new Reaction(type, prodActual);
                                    Reaction * nuevaRp = new Reaction(type, subActual);
                                    if(!subActual->hasReaction(nuevaRs))
                                        subActual->addReaction(nuevaRs);
                                    else delete(nuevaRs);
                                    if(!prodActual->hasReaction(nuevaRp))
                                        prodActual->addReaction(nuevaRp);
                                    else delete(nuevaRp);
                                }

                                else if(type == 2){
                                    Reaction * nuevaR = new Reaction(type, prodActual);
                                    if(!subActual->hasReaction(nuevaR))
                                        subActual->addReaction(nuevaR);
                                    else delete(nuevaR);
                                    break;
                                }
                            }
                        }
                    }
                }
            }

        }
        if(!nuevo->empty()){
          nuevo->delEmptyR();
          addPathway(nuevo);
          return true;
        }
        else{
          printw("Pathway vacio\n");
          refresh();
          delete(nuevo);
          return false;
        }
      }
  return false;
}

void memoria(){
  Entry *aux;
  Pathway *path;
  Reaction *react;
  esfinal2();
  printw("--- Memoria utilizada por cada ruta creada ---");
  refresh();
  esfinal2();
  printw("\n*Clase Pathway*\n");
  refresh();
  esfinal2();
  printw("\tTipo de dato\t\tNombre\t\tTamaño(Bytes)\n");
  refresh();
  esfinal2();
  printw("\t  -string\t\tnombre\t\t  %ld\n", sizeof(string));
  refresh();
  esfinal2();
  printw("\t  -string\t\ttitulo\t\t  %ld\n", sizeof(string));
  refresh();
  esfinal2();
  printw("\t  -*Entry\t\tprimero\t\t  %ld\n", sizeof(aux));
  refresh();
  esfinal2();
  printw("\t  -*Entry\t\tultimo\t\t  %ld\n", sizeof(aux));
  refresh();
  esfinal2();
  printw("\t  -*Pathway\t\tsig\t\t  %ld\n", sizeof(path));
  refresh();
  esfinal2();
  printw("\nTamaño total de la clase: %ld\n", sizeof(Pathway));
  refresh();

  esfinal2();
  printw("\n----------------------------------------------------------\n");
  refresh();
  esfinal2();
  printw("\n*Clase Entry*\n");
  refresh();
  esfinal2();
  printw("\tTipo de dato\t\tNombre\t\tTamaño(Bytes)\n");
  refresh();
  esfinal2();
  printw("\t  -string\t\tnombre\t\t  %ld\n", sizeof(string));
  refresh();
  esfinal2();
  printw("\t  -int\t\t\tid\t\t\t  %ld\n", sizeof(int));
  refresh();
  esfinal2();
  printw("\t  -*Reaction\t\tprimero\t\t  %ld\n", sizeof(react));
  refresh();
  esfinal2();
  printw("\t  -*Reaction\t\tultimo\t\t  %ld\n", sizeof(react));
  refresh();
  esfinal2();
  printw("\t  -*Entry\t\tsig\t\t  %ld\n", sizeof(aux));
  refresh();
  esfinal2();
  printw("\nTamaño total de la clase: %ld\n", sizeof(Entry));
  refresh();

  esfinal2();
  printw("\n----------------------------------------------------------\n");
  refresh();
  esfinal2();
  printw("\n*Clase Reaction*\n");
  refresh();
  esfinal2();
  printw("\tTipo de dato\t\tNombre\t\tTamaño(Bytes)\n");
  refresh();
  esfinal2();
  printw("\t  -int\t\t\tid\t\t\t  %ld\n", sizeof(int));
  refresh();
  esfinal2();
  printw("\t  -*Entry\t\treaction\t\t  %ld\n", sizeof(aux));
  refresh();
  esfinal2();
  printw("\t  -*Reaction\t\tsig\t\t  %ld\n", sizeof(react));
  refresh();
  esfinal2();
  printw("\nTamaño total de la clase: %ld\n", sizeof(Reaction));
  refresh();

  esfinal2();
  printw("\n- Direcciones de los objetos creados -\n");
  refresh();
  esfinal2();
  printw("\nVentana de informacion: %p\n", info_win);
  refresh();
  esfinal2();
  printw("Ventana de menu: %p\n", menu_win);
  refresh();
  if (primero == NULL) {
    esfinal2();
    printw("No se han creado rutas\n");
    refresh();
  }
  else{
    path = primero;
    esfinal2();
    printw("Rutas creadas, y reacciones\n");
    refresh();
    while(path != NULL){
      esfinal2();
      printw("\tRuta: %s -> %p\n", path->getTitulo().c_str(), path);
      refresh();
      if(!path->empty()){
        aux = path->getPrimero();
        while(aux != NULL){
          esfinal2();
          printw("\t\tEntry: %s -> %p\n", aux->getNombre().c_str(), aux);
          refresh();
          if(!aux->empty()){
            react = aux->getRPrimero();
            while(react != NULL){
              esfinal2();
              printw("\t\t\tReaction tipo: %d -> %p\n", react->getType(), react);
              refresh();
              react = react->getSig();
            }
          }
          aux = aux->getSig();
        }
      }
      path = path->getSig();
    }
  }

}

int main()
{
  int alto, ancho, filas, column;
  alto = 11;
  ancho = 20;
  initscr();
  getmaxyx(stdscr,filas,column);
  menu_win = newwin(alto, ancho, filas-alto-4, column-ancho-2);
  info_win = newwin(alto, ancho, 1, column-ancho-2);
  refresh();
  principal(menu_win, info_win);

  bool running = true;
  int opcion;
  string input = "";
  int select;

  while(running){
    principal(menu_win, info_win);
    refresh();
    wmove(menu_win, 9, 1);
    opcion = wgetch(menu_win);

    switch (opcion) {
          case 48:
                werase(stdscr);
                refresh();
                werase(info_win);
                principal(menu_win, info_win);
                mvwaddstr(info_win, 1, 1, "Todas las rutas");
                wrefresh(info_win);
                refresh();
                move(0,0);
                printPathways();
                final();
                getch();
                refresh();
                wrefresh(info_win);
                break;
          case 49:
               werase(stdscr);
               refresh();
               werase(info_win);
               principal(menu_win, info_win);
               mvwaddstr(info_win, 1, 1, "Cargar xml");
               wrefresh(info_win);
               refresh();
               move(0,0);
               printw("Ingrese la direccion del archivo: ");
               wrefresh(info_win);
               refresh();
               input = getstring();
               printw("Archivo: %s\n", input.c_str());
               wrefresh(info_win);
               refresh();
               if(createPathway(input.c_str())){
                  werase(info_win);
                  principal(menu_win, info_win);
                  mvwaddstr(info_win, 1, 1, "agregado \ncorrectamente");
                }
               else{
                  werase(info_win);
                  principal(menu_win, info_win);
                  mvwaddstr(info_win, 1, 1, "error al agregar");
                }
               refresh();
               wrefresh(info_win);
               break;
          case 50:
               werase(stdscr);
               refresh();
               werase(info_win);
               verPathways();
               principal(menu_win, info_win);
               mvwaddstr(info_win, 1, 1, "Ver rutas\n disponibles");
               wrefresh(info_win);
               break;
          case 51:
               mvwaddstr(info_win, 1, 1, "Comparar dos rutas");
               break;
          case 52:
               werase(stdscr);
               refresh();
               werase(info_win);
               principal(menu_win, info_win);
               mvwaddstr(info_win, 1, 1, "Generar grafo");
               wrefresh(info_win);
               move(0,0);
               printw("Ingrese el numero de la ruta a generar: ");
               refresh();
               input = getstring();
               if(strcmp(input.c_str(), "") && digito(input)){
                 select = stoi(input);
                 if(select!=0 && select <= largoPat()){
                    printw("Nombre del archivo de salida: ");
                    refresh();
                    input = getstring();
                    crearDot(select, input);
                  }
                  else{
                    printw("\nLa ruta no existe");
                    refresh();
                  }
               }
               else{
                 printw("\nDebe ingresar un numero");
                 refresh();
               }
               refresh();
               break;
          case 53:
               mvwaddstr(info_win, 1, 1, "Crear nueva ruta");
               break;
          case 54:
              werase(stdscr);
              refresh();
              werase(info_win);
              memoria();
               mvwaddstr(info_win, 1, 1, "Modo inspeccionar");
               final();
               getch();
               break;
          case 55:
               mvwaddstr(info_win, 1, 1, "Salir");
               running = false;
               break;
          default:
               werase(info_win);
               principal(menu_win, info_win);
               mvwaddstr(info_win, 1, 1, "Opcion invalida");
               wrefresh(info_win);
               break;
        }
    }

  getch();
  endwin();

  return 0;
}

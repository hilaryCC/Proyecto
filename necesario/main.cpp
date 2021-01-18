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
using namespace std;

Pathway *primero, *ultimo;

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

void verPathways(WINDOW *salida){
  Pathway *aux = primero;
  int cont = 1;
  std::string nuevo;
  if(aux == NULL){
    printw("No hay rutas disponibles\n");
    refresh();
  }
  while(aux != NULL){
    nuevo = std::to_string(cont)+". "+aux->getNombre()+"\n\t -> "+aux->getTitulo();
    printw(" %s\n", nuevo.c_str());
    refresh();
    aux = aux->getSig();
    cont++;
  }
}

void crearDot(Pathway *nuevo, string out){
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
         << "a" << label << nombre << "\"] " << shape
         << "a -> " << actual->getId() << line;
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
    string cg = "dot -Tpng "+filen+" -o "+out+".png";
    string og = "xdg-open "+out+".png";
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
  string nTitulo = tools.attribute("title").value();
  char opcion; 

  printw("Nombre del pathway: %s\n", nTitulo);

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
  //nuevo->print();
  if(!nuevo->empty()){
    addPathway(nuevo);
  }
  return true;
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
  keypad(menu, true);
}

int main()
{
  int alto, ancho, filas, column;
  alto = 11;
  ancho = 30;
  initscr();
  getmaxyx(stdscr,filas,column);

  WINDOW * menu_win = newwin(alto, 20, filas-alto-4, column-22);
  WINDOW * info_win = newwin(alto, ancho, 1, column-ancho-2);
  refresh();

  bool running = true;
  int opcion;
  char input[80];

  while(running){
    werase(stdscr);
    werase(info_win);
    principal(menu_win, info_win);
    wmove(menu_win, 9, 1);
    opcion = wgetch(menu_win);

    switch (opcion) {
          case 49:
               mvwaddstr(info_win, 1, 1, "Cargar xml");
               wrefresh(info_win);
               move(0,0);
               getstr(input);
               printw("Path: %s\n", input);
               refresh();
               if(createPathway(input))
                  mvwaddstr(info_win, 1, 1, "agregado correctamente");
               else
                  mvwaddstr(info_win, 1, 1, "error al agregar");

               werase(stdscr);
               break;
          case 50:
               werase(stdscr);
               principal(menu_win, info_win);
               mvwaddstr(info_win, 1, 1, "Ver rutas\n disponibles");
               wrefresh(info_win);
               verPathways(stdscr);
               getch();
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

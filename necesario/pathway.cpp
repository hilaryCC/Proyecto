#include "pathway.h"
#include <iostream>
#include <vector>
#include <ncurses.h>
#include <string.h>
#include <string>
using namespace std;

bool esfinal(){
  int y,x;
  int maxy, maxx;
  getyx(stdscr, y, x);
  getmaxyx(stdscr, maxy,maxx);
  if(y+3 >= maxy){
    move(maxy,0);
    printw(".:Continuar:.");
    refresh();
    return true;
  }
  return false;
}

void final(){
  printw("\n.:Salir:.");
  refresh();
}

//Funciones de la clase Pathway
Pathway::Pathway(string nombre, string titulo){
    this->nombre = nombre;
    this->titulo = titulo;
    this->primero = NULL;
    this->ultimo = NULL;
    this->sig = NULL;
}

bool Pathway::empty(){
    if(this->primero == NULL)
        return true;
    return false;
}

int Pathway::largo(){
    if(empty())
        return 0;
    Entry *actual = this->primero;
    int cont = 0;
    while(actual != NULL){
        cont++;
        actual = actual->sig;
    }
    return cont;
}

Entry * Pathway::getEntry(int id){
    if(this->primero == NULL)
        return NULL;
    Entry *actual = this->primero;
    while(actual != NULL){
        if(actual->id == id)
            return actual;
        actual = actual->sig;
    }
    return NULL;
}

void Pathway::print(){
    string salida = "Nombre del Pathway: "+this->nombre+" - Titulo: "+this->titulo;
    salida = salida+"\nCantidad de entradas: "+to_string(this->largo());
     if(esfinal()){
       getch();
       werase(stdscr);
       refresh();
     }
    printw("%s\n", salida.c_str());
    refresh();
    Entry *actual = this->primero;
    while(actual != NULL){
        actual->print();
        actual = actual->sig;
    }
}

void Pathway::addEntry(Entry *nuevo){
    if(this->primero == NULL){
        this->primero = nuevo;
        this->ultimo = nuevo;
        nuevo->sig = NULL;
    }
    else{
        this->ultimo->sig = nuevo;
        this->ultimo = nuevo;
        nuevo->sig = NULL;
    }
}

void Pathway::profundidad(Pathway *actual){

}

void Pathway::delEmptyR(){
  Entry *actual = this->primero;
  Entry *aux;
  bool band = false;
  Entry *del = NULL;
  while(actual != NULL){
    if(actual->empty()){
      band = false;
      aux = this->primero;
      while(aux != NULL){
        if(aux->isReaction(actual)){
          band = true;
          break;
        }
        aux = aux->sig;
      }
      if(!band){
        del = actual;
      }
    }
    actual = actual->sig;
    delEntry(del);
  }
}

void Pathway::delEntry(Entry *del){
  Entry *aux = this->primero;
  Entry *anterior;
  while(aux != NULL){
    if(aux == del && aux == this->primero){
        this->primero = aux->sig;
        free(aux);
        break;
    }
    else if(aux == del && aux == this->ultimo){
      this->ultimo = anterior;
      free(aux);
      break;
    }
    else if(aux == del){
      anterior->sig = aux->sig;
      free(aux);
      break;
    }
    anterior = aux;
    aux = aux->sig;
  }
}

//Funciones de la clase Entry

Entry::Entry(string nombre, int id){
    this->nombre = nombre;
    this->id = id;
    this->primero = NULL;
    this->ultimo = NULL;
    this->sig = NULL;
}

void Entry::print(){
    string salida = "\tNombre de la entrada: "+this->nombre+" - Id: "+to_string(this->id)+"\n\t\tId de las reacciones:";
    /*cout << "\tNombre de la entrada: " << this->nombre
         << " - Id: " << this->id
         << "\n\t\tId de las reacciones:";*/
     if(esfinal()){
       getch();
       werase(stdscr);
       refresh();
     }
    printw("%s", salida.c_str());
    refresh();
    Reaction *aux = this->primero;
    while(aux != NULL){
      salida = " "+to_string(aux->getId());
      printw("%s", salida.c_str());
      refresh();
        //cout << " " << aux->getId();
        aux = aux->sig;
    }
    printw("\n");
}

void Entry::addReaction(Reaction *nuevo){
    if(this->primero == NULL){
        this->primero = nuevo;
        this->ultimo = nuevo;
        nuevo->sig = NULL;
    }

    else{
        this->ultimo->sig = nuevo;
        this->ultimo = nuevo;
        nuevo->sig = NULL;
    }
}

bool Entry::hasReaction(Reaction *nuevo){
    Reaction * aux = this->primero;
    while(aux!=NULL){
        if(aux->reaction == nuevo->reaction)
            return true;
        aux = aux->sig;
    }
    return false;
}

bool Entry::empty(){
  if(this->primero == NULL)
    return true;
  return false;
}

bool Entry::isReaction(Entry *aux){
  Reaction *actual = this->primero;
  while(actual!=NULL){
    if(actual->reaction == aux)
      return true;
    actual = actual->sig;
  }
  return false;
}

#include "pathway.h"
#include <iostream>
#include <vector>
using namespace std;


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
    cout << "Nombre del Pathway: " << this->nombre
         << " - Titulo: " << this->titulo
         << "\nCantidad de entradas: " << this->largo() << endl;
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

//Funciones de la clase Entry
Entry::Entry(string nombre, int id){
    this->nombre = nombre;
    this->id = id;
    this->primero = NULL;
    this->ultimo = NULL;
    this->sig = NULL;
}

void Entry::print(){
    cout << "\tNombre de la entrada: " << this->nombre
         << " - Id: " << this->id
         << "\n\t\tId de las reacciones:";
    Reaction *aux = this->primero;
    while(aux != NULL){
        cout << " " << aux->getId();
        aux = aux->sig;
    }
    cout << "\n" << endl;
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
        if(aux == nuevo)
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

#ifndef PATHWAY_H
#define PATHWAY_H
#include <iostream>
#include <vector>
using namespace std;

class Reaction;

class Entry{
private:
    string nombre;
    int id;
    Reaction *primero;
    Reaction *ultimo;
    Entry *sig;

public:
    Entry(string nombre, int id);
    string getNombre() {return this->nombre;}
    int getId() {return this->id;}
    void addReaction(Reaction *nuevo);
    void print();
    Entry * getSig() {return this->sig;}
    bool hasReaction(Reaction *nuevo);
    friend class Pathway;
    friend class Reaction;
};

class Reaction{
private:
    int type;
    Entry *reaction;
    Reaction *sig;

public:
    Reaction(int type, Entry * reaction){
        this->type = type;
        this->reaction = reaction;
        this->sig = NULL;
    }
    int getId(){return this->reaction->id;}
    friend class Pathway;
    friend class Entry;
};

class Pathway{
private:
    string nombre;
    string titulo;
    Entry *primero;
    Entry *ultimo;
    Pathway *sig;

public:
    Pathway(string nombre, string titulo);
    string getNombre() {return this->nombre;}
    string getTitulo() {return this->titulo;}
    void addEntry(Entry *nuevo);
    void print();
    Entry * getEntry(int id);
    Entry * getPrimero() {return this->primero;}
    bool empty();
    int largo();
    Pathway * getSig() {return this->sig;}
};

#endif // PATHWAY_H

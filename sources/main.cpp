#include "pugixml.hpp"
#include "pugiconfig.hpp"
#include <string.h>
#include <iostream>
#include <cstring>
#include <pathway.h>
#include <vector>
using namespace std;

int main()
{
    pugi::xml_document doc;
    if (!doc.load_file("C:/Users/Administrador/Downloads/ejemplo1.xml")) return -1;

    pugi::xml_node tools = doc.child("pathway");
    Pathway nuevo = Pathway(tools.attribute("name").value(), tools.attribute("title").value());
    int contSubs = 0;
    int contProd = 0;
    nuevo.print();


    // tag::basic[]
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling())
    {
        if(strcmp(tool.name(), "entry")==0 &&
                strcmp(tool.attribute("type").value(), "compound") == 0){
            Entry *newEntry = new Entry(tool.attribute("name").value(), tool.attribute("id").as_int());
            nuevo.addEntry(newEntry);
        }

        if(strcmp(tool.name(), "reaction")==0){
            contSubs = 0;
            contProd = 0;
            int type = 0;
            if(strcmp(tool.attribute("type").value(), "reversible")==0)
                type = 1;
            else
                type = 2;

            for (pugi::xml_node to = tool.first_child(); to; to = to.next_sibling()){
                if(strcmp(to.name(), "substrate") == 0)
                    contSubs++;
                else if(strcmp(to.name(), "product") == 0)
                        contProd++;
            }
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

                for(int sub = 0; sub < (int)subs.size(); sub++){
                    Entry * subActual = nuevo.getEntry(subs[sub]);

                    for(int prod = 0; prod < (int)prods.size(); prod++){
                        Entry * prodActual = nuevo.getEntry(prods[prod]);

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
    nuevo.print();
}


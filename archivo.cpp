#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
int main()
{
  ofstream file;
  file.open("archivo.dot");
  file << "digraph hola{\n"
       << "16 -> {18 c} [style=dotted];\n"
       << "d -> 16;\n"
       << "16 [label=\"chs:0003\"] [shape=box];\n"
       << "}\n";
  file.close();
  system ("dot -Tpng archivo.dot -o archivo.png");
  system ("xdg-open archivo.png");
  return 0;
}

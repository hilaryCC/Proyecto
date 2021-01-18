
#include <stdio.h>
#include <stdlib.h>

int main(){

    printf("El contenido de la carpeta actual es:\n");
    system ("dot -Tpng ejemplo.dot -o ejemplo.png");
    system ("xdg-open ejemplo.png");
    return 0;
}

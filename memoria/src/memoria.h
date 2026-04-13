#ifndef MEMORIA_H
#define MEMORIA_H

#include <utils.h>

void ObtenerConfig ();
void EscribirMemoria (int direccion, char* contenido);
char* LeerMemoria (int direccion, int tamanio);

#endif
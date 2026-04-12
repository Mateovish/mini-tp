
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <commons/log.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>

void saludar(char* quien);
int IniciarServidor (int puerto);
int EsperarConexion (int socket_servidor);
int CrearConexion (char* ip, int puerto);
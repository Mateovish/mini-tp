
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <commons/log.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdlib.h>

extern t_log* logger;
void saludar(char* quien);
int IniciarServidor (int puerto);
int EsperarConexion (int socket_servidor);
int CrearConexion (char* ip, char* puerto);
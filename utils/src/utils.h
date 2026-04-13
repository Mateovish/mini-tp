
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
#include <commons/collections/dictionary.h>
#include <commons/bitarray.h>

typedef enum {
    MENSAJE,
    PAQUETE,
    INICIAR_PROCESO,
    LEER_MEMORIA
} op_code;

typedef struct {
    int size;
    void* stream;
} t_buffer;

typedef struct {
    op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete;

extern t_log* logger;
void saludar(char* quien);
void RecibirEntero (int socket_emisor);
void EnviarEntero (int socket_receptor, int ent);
int IniciarServidor (int puerto);
int EsperarConexion (int socket_servidor);
int CrearConexion (char* ip, char* puerto);
void EnviarMensaje(char* mensaje, int socket_cliente);
void* SerializarPaquete(t_paquete* paquete, int bytes);
void EnviarPaquete(t_paquete* paquete, int socket_cliente);
void EliminarPaquete(t_paquete* paquete);
void* RecibirBuffer(int* size, int socket_cliente);
t_list* RecibirPaquete(int socket_cliente);
void AgregarAPaquete(t_paquete* paquete, void* valor, int tamanio);
void CargarEnteroEnPaquete (t_paquete* paquete, int entero);
void CargarMensajeEnPaquete (t_paquete* paquete, char* mensaje, int largo);
t_paquete* CrearPaquete(op_code codigo_operacion);
void RecibirMensaje(int socket_cliente);
int RecibirOperacion (int socket_cliente);
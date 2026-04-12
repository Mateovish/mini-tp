#include <utils.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
t_log* logger;

int EsperarConexion (int socket_servidor) {


    return socket_cpu
}

int RecibirEntero (int socket_emisor) {


    return
}

int EnviarEntero (int socket_receptor, int entero) {


    return 
}

int CrearConexion (char *ip, int puerto) {



    return socket_memoria
}

int IniciarServidor (int puerto) {

    int socket_servidor;
    struct addrinfo hints, *servinfo;

    // 1. Preparativos (Las pistas)
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // 2. Traducir las pistas a configuración real
    getaddrinfo(NULL, puerto, &hints, &servinfo);

    // 3. Crear el socket (El enchufe)
    socket_servidor = socket(servinfo->ai_family,
                             servinfo->ai_socktype,
                             servinfo->ai_protocol);

    // 4. Asociar el socket al puerto (Enchufarlo)
    bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

    // 5. Poner el socket a escuchar
    listen(socket_servidor, SOMAXCONN);

    // 6. Limpiar y avisar
    freeaddrinfo(servinfo);
    log_trace(logger, "Listo para escuchar a mi cliente");

    return socket_servidor;
}

void saludar(char* quien) {
    printf("Hola desde %s!!\n", quien);
}
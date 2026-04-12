#include <utils.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

t_log* logger;

int EsperarConexion (int socket_servidor) {

// Aceptamos un nuevo cliente
	int socket_cpu;
	socket_cpu = accept (socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto la CPU");
    return socket_cpu;
}

int CrearConexion (char *ip, char* puerto) {

    struct addrinfo hints;
    struct addrinfo *server_info;

    // 1. Preparativos (Las pistas)
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    // ¡Ojo! Acá NO va AI_PASSIVE porque no somos el servidor. 
    // Nosotros vamos a conectarnos a una IP específica.

    // 2. Traducir las pistas y la IP/Puerto a configuración real
    getaddrinfo(ip, puerto, &hints, &server_info);

    // 3. Crear el socket (Comprar nuestro teléfono)
    int socket_memoria = socket(server_info->ai_family, 
                                server_info->ai_socktype, 
                                server_info->ai_protocol);

    // 4. Conectarnos al servidor (Marcar el número y llamar)
    connect(socket_memoria, server_info->ai_addr, server_info->ai_addrlen);

    // 5. Limpiar
    freeaddrinfo(server_info);

    return socket_memoria;
}

int IniciarServidor (int puerto) {

    int socket_servidor;
    struct addrinfo hints, *servinfo;

    // 1. Preparativos (Las pistas)
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char* puerto_string = string_itoa(puerto);

    // 2. Traducir las pistas a configuración real
    getaddrinfo(NULL, puerto_string, &hints, &servinfo);

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
    free(puerto_string); 
    log_trace(logger, "Listo para escuchar a mi cliente");

    return socket_servidor;
}

void saludar(char* quien) {
    printf("Hola desde %s!!\n", quien);
}
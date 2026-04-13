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

void RecibirEntero (int socket_emisor) {

    int numero;
    recv(socket_emisor, &numero, sizeof(int), 0);
    log_info(logger, "Recibi el numero %d", numero);
}

void EnviarEntero (int socket_receptor, int ent) {

    send(socket_receptor, &ent, sizeof(int), 0);
    log_info(logger, "Envié el numero %d", ent);
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

int RecibirOperacion (int socket_cliente) {

	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

t_paquete* CrearPaquete(op_code codigo_operacion) {
    // 1. Compramos la caja principal (reservamos memoria para la estructura externa)
    t_paquete* paquete = malloc(sizeof(t_paquete));

    // 2. Le pegamos la etiqueta indicando qué tipo de mensaje es
    paquete->codigo_operacion = codigo_operacion;

    // 3. Compramos la sub-caja interna para guardar los datos
    paquete->buffer = malloc(sizeof(t_buffer));

    // 4. Inicializamos la caja vacía (¡Súper importante!)
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    return paquete;
}

void* SerializarPaquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void EnviarMensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = SerializarPaquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	EliminarPaquete(paquete);
}

void EnviarPaquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = SerializarPaquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void* RecibirBuffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

t_list* RecibirPaquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = RecibirBuffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void RecibirMensaje(int socket_cliente) {
    int size;
    // 1. Recibimos el tamaño del texto (el segundo int que habíamos mandado)
    recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
    
    // 2. Pedimos memoria para guardar el texto
    char* buffer = malloc(size);
    
    // 3. Recibimos el texto real
    recv(socket_cliente, buffer, size, MSG_WAITALL);
    
    // 4. Lo mostramos en pantalla
    log_info(logger, "Me llego un mensaje: %s", buffer);
    
    // 5. Limpiamos la basura
    free(buffer);
}

void EliminarPaquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void AgregarAPaquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void CargarEnteroEnPaquete (t_paquete* paquete, int entero) {

    AgregarAPaquete (paquete, &entero, sizeof(int));
}

void CargarMensajeEnPaquete (t_paquete* paquete, char* mensaje, int largo) {

    int tam = strlen(mensaje) + 1;
    AgregarAPaquete (paquete, &mensaje, tam);
}

void saludar(char* quien) {
    printf("Hola desde %s!!\n", quien);
}
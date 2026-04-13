#include "memoria.h"
t_log_level log_level;
t_config* config;
int puerto_escucha;
int socket_servidor;
int socket_kernel;
int socket_cpu;
int tam_memo;
void* memoria_fisica;
int tam_pag;
t_bitarray* bitmap;
t_dictionary* de_pid_a_tabla;
int memoria_restante;

int main(int argc, char* argv[]) {

    // Estableciendo conexiones
    saludar("memoria");
    ObtenerConfig ();
    logger = log_create ("memoria.log", "MEMORIA", true, log_level);
    socket_cpu = EsperarConexion(socket_servidor);
    socket_kernel = EsperarConexion(socket_servidor);
    
    // Memory Allocate

    memoria_fisica = malloc(tam_memo);

    //Prueba de lectura y escritura
    EscribirMemoria(0, "Hola Mundo");
    LeerMemoria(0, 10);

    // Creación del diccionario para mapear PID a tabla de páginas
    de_pid_a_tabla = dictionary_create();

    return 0;
}

char* LeerMemoria (int direccion, int tamanio) {

    char* lectura = malloc(tamanio +1);
    memcpy(lectura, memoria_fisica + direccion, tamanio);
    lectura[tamanio] = '\0';
    log_info (logger, "Lectura Realizada. Direccion: %d, Contenido: %s", direccion, lectura);
    return lectura;
}

void EscribirMemoria (int direccion, char* contenido) {

    int tamanio = strlen(contenido);
    memcpy(memoria_fisica + direccion, contenido, tamanio);
    log_info (logger, "Escritura Realizada. Direccion: %d, Contenido: %s", direccion, contenido);
}

void ObtenerConfig () {

    config = config_create ("memoria.config");
    puerto_escucha = config_get_int_value (config, "PUERTO_ESCUCHA");
    log_level = log_level_from_string(config_get_string_value(config, "LOG_LEVEL"));
    tam_memo = config_get_int_value (config, "TAM_MEMO");
}

void CrearBitmap () {

    int cant_paginas = tam_memo / tam_pag;
    int cant_bytes_bitmap = (cant_paginas + 7) / 8; // Redondear hacia arriba
    char* contenido = malloc(cant_bytes_bitmap);
    memset(contenido, 0, cant_bytes_bitmap); // Inicializar el bitmap con ceros
    bitmap = bitarray_create_with_mode (contenido, cant_paginas, LSB_FIRST);
}

bool ReservarMemoriaParaProceso (int pid, int tam) {

    if () {
        memoria_restante -= tam;
        


        return true;
    } else () {


        return false
    }
}

int DeIntAString (int numero) {

    int tam = ContarDigitos(numero);
    char* str = malloc(tam + 1);
    sprintf(str,tam + 1 , "%d", numero);
    return str;
}

int ContarDigitos (int numero) {

    int contador = 0;
    if (numero == 0) return 1; // El número 0 tiene un dígito
    while (numero > 0) {
        numero /= 10;
        contador++;
    }
    return contador;
}

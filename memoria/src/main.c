#include "memoria.h"
t_log_level log_level;
t_config* config;
int puerto_escucha;
int socket_servidor;
int socket_kernel;
int socket_cpu;
int tam_memo;
void* memoria_fisica;

int main(int argc, char* argv[]) {

    // Estableciendo conexiones
    saludar("memoria");
    ObtenerConfig ();
    logger = log_create ("memoria.log", "MEMORIA", true, log_level);
    socket_cpu = EsperarConexion(socket_servidor);
    socket_kernel = EsperarConexion(socket_servidor);
    
    // Memory Allocate

    memoria_fisica = malloc(tam_memo);


    return 0;
}

void ObtenerConfig () {

    config = config_create ("memoria.config");
    puerto_escucha = config_get_int_value (config, "PUERTO_ESCUCHA");
    log_level = log_level_from_string(config_get_string_value(config, "LOG_LEVEL"));
    tam_memo = config_get_int_value (config, "TAM_MEMO");
}

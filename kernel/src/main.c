#include "kernel.h"

t_config* config;
int puerto_escucha;
int socket_servidor;
int socket_cpu;
t_log_level log_level;

int main(int argc, char* argv[]) {
    saludar("kernel");
    
    // 1. Primero cargamos la config (para saber el puerto y log_level)
    ObtenerConfig(); 
    
    // 2. Ahora sí podemos crear el logger
    logger = log_create("kernel.log", "KERNEL", true, log_level);
    
    // 3. Levantamos el servidor con el puerto correcto
    socket_servidor = IniciarServidor(puerto_escucha);
    socket_cpu = EsperarConexion(socket_servidor);
    
    log_info(logger, "Se conectó la CPU");
    return 0;
}

void ObtenerConfig () {

    config = config_create ("kernel.config");
    puerto_escucha = config_get_int_value (config, "PUERTO_ESCUCHA");
    log_level = log_level_from_string(config_get_string_value (config,"LOG_LEVEL"));
}


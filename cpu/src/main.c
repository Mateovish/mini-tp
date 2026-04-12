#include "cpu.h"

t_config* config;
char* ip_kernel;
int puerto_kernel;
t_log_level log_level;
int socket_kernel;


int main(int argc, char* argv[]) {
    saludar("cpu");
    
    // 1. Primero cargamos la config
    ObtenerConfig();
    
    // 2. Creamos el logger para poder registrar lo que pase en la conexión
    logger = log_create("cpu.log", "CPU", true, log_level);
    
    // 3. Nos conectamos
    char* puerto_kernel_string = string_itoa(puerto_kernel);
    socket_kernel = CrearConexion(ip_kernel, puerto_kernel_string);
    free(puerto_kernel_string);
    
    log_info(logger, "Me conecté al Kernel existosamente!");
    
    // IMPORTANTE: Un pequeño freno temporal
    sleep(10); 
    
    return 0;
}

void ObtenerConfig () {

    config = config_create ("cpu.config");
    ip_kernel= config_get_string_value (config, "IP_KERNEL");
    puerto_kernel= config_get_int_value (config, "PUERTO_KERNEL");
    log_level= log_level_from_string(config_get_string_value(config, "LOG_LEVEL"));

}

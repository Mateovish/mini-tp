#include "cpu.h"

t_config* config;
char* ip_kernel;
int puerto_kernel;
t_log_level log_level;
t_log* logger;
int socket_kernel;


int main(int argc, char* argv[]) {
    saludar("cpu");
    ObtenerConfig();
    socket_kernel = CrearConexion (ip_kernel, puerto_kernel);
    logger = log_create ("cpu.log", "CPU", true, log_level);
    return 0;
}

void ObtenerConfig () {

    config = config_create ("cpu.config");
    ip_kernel= config_get_string_value (config, "IP_KERNEL");
    puerto_kernel= config_get_int_value (config, "PUERTO_KERNEL");
    log_level= log_level_from_string(config_get_string_value(config, "LOG_LEVEL"));

}

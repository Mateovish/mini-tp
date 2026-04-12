#include "kernel.h"

t_config* config;
int puerto_escucha;
int socket_servidor;
int socket_cpu;
t_log_level log_level;
t_log* logger;

int main(int argc, char* argv[]) {
    saludar("kernel");
    logger = log_create ("kernel.log","KERNEL",true,log_level);
    socket_servidor = IniciarServidor(puerto_escucha);
    socket_cpu = EsperarConexion(socket_servidor);
    log_info (logger, "Se conectó la CPU");
    return 0;
}

void ObtenerConfig () {

    config = config_create ("kernel.config");
    puerto_escucha = config_get_int_value (config, "PUERTO_ESCUCHA");
    log_level = log_level_from_string(config_get_string_value (config,"LOG_LEVEL"));
}


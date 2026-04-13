#include "memoria.h"
t_log_level log_level;
t_config* config;
int puerto_escucha;
int socket_servidor;
int socket_kernel;
int socket_cpu;
int tam_memo;
void* memoria_fisica;
int tam_pag = 8;
t_bitarray* bitmap;
t_dictionary* de_pid_a_tabla;
int memoria_restante = 4096;

int main(int argc, char* argv[]) {

    saludar("memoria");
    
    // 1. CONFIGURACIÓN Y LOGGER
    ObtenerConfig();
    logger = log_create("memoria.log", "MEMORIA", true, log_level);
    
    // ========================================================
    // 2. PREPARAR LA CASA (Inicializar estructuras)
    // ========================================================
    
    // Memory Allocate (Idealmente usá calloc para que arranque en 0)
    memoria_fisica = malloc(tam_memo); 

    // Creación del diccionario y Bitmap
    de_pid_a_tabla = dictionary_create();
    CrearBitmap();

    // Pruebas locales (esto se ejecuta antes de que nadie se conecte)
    EscribirMemoria(0, "Hola Mundo");
    LeerMemoria(0, 10);
    ReservarMemoriaParaProceso(1, 67);
    LiberarProceso(1);

    // ========================================================
    // 3. ABRIR LAS PUERTAS (Red)
    // ========================================================

    socket_servidor = IniciarServidor(puerto_escucha); 
    
    // Ahora sí esperamos a los clientes
    log_info(logger, "Esperando conexiones...");
    socket_cpu = EsperarConexion(socket_servidor);
    log_info(logger, "Se conectó la CPU");
    
    socket_kernel = EsperarConexion(socket_servidor);
    log_info(logger, "Se conectó el Kernel");

    // ========================================================
    // 4. ATENDER A LOS CLIENTES (Secuencial por ahora)
    // ========================================================

    // Recibo el paquete (Alineado con el EnviarPaquete de CPU)
    t_list* paquete = RecibirPaquete(socket_cpu);

    // ========================================================
    // 5. APAGAR TODO ORDENADAMENTE
    // ========================================================
    
    free(memoria_fisica);
    dictionary_destroy(de_pid_a_tabla);
    // Destruir bitmap, logger, config, etc...

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

bool ReservarMemoriaParaProceso(int pid, int tamanio) {
    
    // PASO 1: Verificar si hay espacio suficiente
    if (tamanio > memoria_restante) {
        log_warning(logger, "No hay memoria suficiente para el PID %d", pid);
        return false; 
    }

    // PASO 2: Descontar la memoria
    memoria_restante -= tamanio;

    // PASO 3: Calcular cuántas páginas necesita
    // Truco matemático de C para redondear hacia arriba divisiones enteras
    int paginas_necesarias = (tamanio + tam_pag - 1) / tam_pag;

    // PASO 4: Crear la tabla de páginas para este proceso
    t_list* tabla_del_proceso = list_create();

    // Acá cargamos contenido en nuevas páginas y vamos añadíendolas a la lista
    for (int i = 0; i < paginas_necesarias; i++) {
        
        int* nueva_pagina = malloc(sizeof(int));
        *nueva_pagina = i; //acá agregamos el contenido en si, en este caso nada más es el número de página
        list_add(tabla_del_proceso, nueva_pagina);
        log_debug(logger, "Cree el marco %d del proceso %d", i, pid);
    }

    char* pid_str = string_itoa(pid);
    dictionary_put(de_pid_a_tabla, pid_str, tabla_del_proceso);
    free(pid_str);

    return true;
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

void LiberarProceso(int pid) {

    char* pid_str = string_itoa(pid);

    // 2. Sacamos directamente la t_list* del diccionario
    t_list* lista_de_frames = (t_list*) dictionary_remove(de_pid_a_tabla, pid_str);

    // 3. Verificamos que exista
    if (lista_de_frames == NULL) {
        printf("Error: No hay páginas asignadas al PID %d\n", pid);
        free(pid_str);
        return; 
    }

    int cant_paginas = list_size(lista_de_frames);

    // 5. Recorremos la lista y liberamos cada frame en el bitmap
    for (int i = 0; i < cant_paginas; i++) {
        
        int* frame_asignado = (int*) list_get(lista_de_frames, i);
        bitarray_clean_bit(bitmap, *frame_asignado);
        log_debug(logger, "Libere el marco %d del proceso %d", i, pid);
    }

    memoria_restante += cant_paginas * tam_pag;
    list_destroy_and_destroy_elements(lista_de_frames, free);
    free(pid_str);
}

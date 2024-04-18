#ifndef STRUCTS_H
	#define STRUCTS_H
	
	#define ADM 1
	#define USR 0
	#define NOMBRE 0
	#define AUTOR 1
	#define MOSTRAR 1
	#define DESCARGAR 0
	
	#define OK 0
	#define ERROR 1
	#define ERRPASS 2
	#define ERRUSR 3
	#define ERRMEM 4
	#define ERRFILE 5
        #define END 6


        #define NBACKLOG 10
        #define LARGO 100

	#define ARCH_USUARIO "cfg/usuarios.lst"
	#define ARCH_CLIENTE "cfg/cliente.cfg"
	#define ARCH_DOCUMENTO "cfg/documentos.lst"	

	typedef struct documento
	{
		char codigo[9];
		char* nombre;
		char* autor;
		int anio;
	}Documento_t;

	typedef struct usuario
	{
		char* nombre;
		char* contrasenia;
		int nivel;
	}Usuario_t;
	
	typedef struct nodo
	{
		void * dato;
		struct nodo * next;	
	}Nodo_t;
        typedef struct argumento
        {
                int socket;
                char puerto[10];
                pthread_mutex_t semaforo;
                Nodo_t ** lista_usuarios;    //para modificar en el main
                Nodo_t ** lista_documentos;
        }Argumento_t;
#endif

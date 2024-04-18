#ifndef STRUCTS_H
	#define STRUCTS_H
	
	#define MENU_0 0
	#define MENU_1 1
	#define MENU_2 2
	#define MENU_3 3
	#define MENU_ROOT 4
	
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

        #define USR 0
        #define ADM 1


        #define NBACKLOG 10
        #define LON_MAX 100

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

	typedef struct nodo
	{
		void * dato;
		struct nodo * next;	
	}Nodo_t;
#endif

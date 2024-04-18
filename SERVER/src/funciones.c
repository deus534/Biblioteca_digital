#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "structs.h"
#include "funciones.h"

int agregar( Nodo_t ** lista, void * dat )
{
	int error = ERRMEM;
	Nodo_t * aux_lista = *lista;
	Nodo_t * nuevo;

	nuevo = (Nodo_t *)malloc( sizeof(Nodo_t) );

	if( nuevo )
	{
		error = OK;
		nuevo->dato = dat;
		nuevo->next = NULL;
		if( aux_lista )
		{
			while( aux_lista->next ) aux_lista = aux_lista->next;
			aux_lista->next = nuevo;
		}
		else *lista = nuevo;
	}
	return error;
}
void * buscar( Nodo_t ** lista, void * elem, int(*crit_busqueda)(void *, void *),int * error )
{
	void * dato_aux = NULL;
	Nodo_t * aux_list = *lista;
        *error = ERROR;
	while( aux_list && (*error)!=OK )
	{
		dato_aux = aux_list->dato;
		(*error) = crit_busqueda(elem, dato_aux);
		aux_list = aux_list->next;
	}
        if( (*error)!=OK ) dato_aux = NULL;
        
	return dato_aux;
}
int eliminar(Nodo_t ** lista, void * elim, int(*cmp_elim)(void*,void*),void(*liberar_dato)(void *) )
{
	int err = ERROR;
	Nodo_t * aux = *lista;
	Nodo_t * ant = NULL;
	while( aux && cmp_elim(aux->dato, elim)!=OK )
	{
		ant = aux;
		aux = aux->next;
	}
	if( aux )
	{
		err = OK;
		if( ant ) ant->next = aux->next;
		else *lista = aux->next;
                liberar_dato(aux->dato);
		free(aux);
	}
	return err;
}
void ordenar(Nodo_t * lista,int(*fun_cmp)(void *,void *))
{
	void * aux = NULL;
	Nodo_t * pdato = lista;
	Nodo_t * actual = NULL;
	if( lista )
	{
		while( pdato->next )
		{
			actual = pdato->next;
			while( actual )
			{
				if( fun_cmp(pdato->dato,actual->dato)>0 )
				{
					aux = pdato->dato;
					pdato->dato = actual->dato;
					actual->dato = aux;
				}
				actual = actual->next;
			}
			pdato = pdato->next;
		}
	}
}
Nodo_t * leer_usuarios(int * error)
{
	FILE * file;
	int nivel;
	char * nombre, * contra, linea[LARGO];
	Nodo_t * lista = NULL;
	Usuario_t * nuevo;

	file = fopen(ARCH_USUARIO, "r");

	if( file )
	{
		while( fgets(linea, LARGO, file) && (*error)!=ERRMEM )
		{
			//obtengo los datos leidos
			nombre = strtok(linea, "|");
			
			contra = strtok(NULL, "|");

			nivel = atoi( strtok(NULL,"|") );
			//creo un nuevo usuario con los datos
			nuevo = nuevo_usuario( nombre, contra, nivel );
			
			if( nuevo ) *error = agregar(&lista, (void *)nuevo);
				
			else *error = ERRMEM;
		}
		fclose(file);
	}
        else *error = ERRFILE;

	return lista;
}
Nodo_t * leer_documentos(int * error)
{
	FILE * file;
	char * cod, * nom, * aut, linea[LARGO];
	Nodo_t * lista = NULL;
	Documento_t * nuevo;

	file = fopen(ARCH_DOCUMENTO, "r");

	if( file )
	{
		while( fgets(linea, LARGO, file) && (*error)!=ERRMEM )
		{
			//obtengo los datos leidos
			cod = strtok(linea, "|");
			
			nom = strtok(NULL, "|");
			
			aut = strtok(NULL, "|");

			//creo un nuevo usuario con los datos
			nuevo = nuevo_documento( cod, nom, aut, atoi( strtok(NULL, "|") ) );
			
			if( nuevo ) *error = agregar(&lista, (void *)nuevo);
				
			else *error = ERRMEM;
		}
		fclose(file);
	}
        else *error = ERRFILE;

	return lista;
}
int cargar_usuarios(Nodo_t * lista)
{
	FILE * file;
	int error = 0;
	Usuario_t * usuario;
	
	file = fopen(ARCH_USUARIO,"w+");	
	
	if( file )
	{
		while( lista && error!=-1 )
		{
			usuario = (Usuario_t*)lista->dato;

			error = fprintf(file, "%s|%s|%d\n",usuario->nombre,usuario->contrasenia,usuario->nivel);
			
			lista = lista->next;	
		}
		fclose(file);
	}
        else error = ERRFILE;

	return error;
}
int cargar_documentos(Nodo_t * lista)
{
	FILE* file;
	int error = 0;
	Documento_t * pdato = NULL;

	file = fopen(ARCH_DOCUMENTO, "w+");

	if( file )
	{
		while( lista && error!=-1 )
		{
			pdato = (Documento_t*)lista->dato;

                        //nose si poner la validacion de que pueda o no escribir
			error = fprintf(file, "%s|%s|%s|%d\n",pdato->codigo,pdato->nombre,pdato->autor,pdato->anio);
			
			lista = lista->next;
		}
                fclose(file);
	}
        else error = ERRFILE;

        return error;
}
Usuario_t * nuevo_usuario(char* nom, char* cont, int nivel)
{
	Usuario_t* us;

	//pido memoria
	us = (Usuario_t*)malloc( sizeof(Usuario_t) );

	//valido memoria
	if( us )
	{
		//pido memoria
		us->nombre = (char *)malloc( sizeof(nom) );
		//pido memoria
		us->contrasenia = (char *)malloc( sizeof(cont) );

		//valido la memoria
		if( us->nombre && us->contrasenia )
		{
			strcpy(us->nombre, nom);
			strcpy(us->contrasenia, cont);
			us->nivel = nivel;
		}
                //no me pudieron dar memoria para los char*
		else us = NULL;
	}
	return us;
}
Documento_t * nuevo_documento(char * cod, char * nom, char * aut, int an)
{
	Documento_t * nuevo;
	nuevo = (Documento_t*)malloc( sizeof(Documento_t) );
	if( nuevo )
	{
		//pido memoria
		nuevo->nombre = (char*)malloc( strlen(nom)+1 );
		nuevo->autor = (char*)malloc( strlen(aut)+1 );
		//valido los 2 pedidos de memoria
		
		if( nuevo->nombre && nuevo->autor )
		{
			strcpy(nuevo->codigo, cod);
			strcpy(nuevo->nombre, nom);
			strcpy(nuevo->autor, aut);
			nuevo->anio = an;
		}
                //no me pudieron dar memoria para los char*
		else nuevo = NULL;
	}
	return nuevo;
}
void liberar_usuario(void * dato)
{
	Usuario_t * pdato = (Usuario_t*)dato;
	free(pdato->nombre);
	free(pdato->contrasenia);
	free(pdato);
}
void liberar_documento(void * dato)
{
	Documento_t * doc = (Documento_t*)dato;
	free(doc->nombre);
	free(doc->autor);
	free(doc);
}
void liberar(Nodo_t * lista, void(*liberar_dato)(void*))
{
	Nodo_t * aux;
	while( lista )
	{
		aux = lista;
		lista = lista->next;	
		liberar_dato(aux->dato);
		free(aux);
	}
}
int iniciar_socket(char * puerto)
{
	int misocket, resu, on = 1;
	struct sockaddr_in midireccion;

	misocket = socket(AF_INET, SOCK_STREAM, 0);
	
        if (misocket != -1)
	{
		printf("Socket abierto\n");
		midireccion.sin_family = AF_INET;
		midireccion.sin_port = htons(atoi(puerto));
		midireccion.sin_addr.s_addr = INADDR_ANY;
		memset(midireccion.sin_zero, 0, 8);
		setsockopt(misocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		resu = bind(misocket, (struct sockaddr *) &midireccion, sizeof(midireccion));

                if( resu ==-1 )
                {
                        close(misocket);
                        exit(1);
                }
        }
        return misocket;
}
void transferir_documento(int socket, Nodo_t ** lista_documentos  )
{
        char codigo[LARGO], nom_arch[LARGO], buffer[LARGO];
        int error, fd;
        ssize_t bytes_leidos = 1;
        Documento_t * doc;

        recv(socket, codigo, LARGO, 0);
        doc = (Documento_t*)buscar(lista_documentos, codigo, cmp_codigo, &error);
        send(socket, &error, sizeof(int), 0);

        //encontro el documento
        if( error==OK )
        {
                strcpy(buffer, doc->nombre);
                send( socket, buffer, LARGO, 0 );
                strcpy(buffer, doc->autor);
                send( socket, buffer, LARGO, 0 );
                send( socket, &doc->anio, sizeof(int), 0 );

                strcpy(nom_arch, "biblioteca/");
                strcat(nom_arch, codigo);
                
                fd = open(nom_arch, O_RDONLY);

                if( fd!=-1 )
                {
                        while( bytes_leidos )
                        {
                                bytes_leidos = read( fd, buffer, (LARGO-1) );
                                buffer[LARGO-1] = '\0';

                                if( bytes_leidos<(LARGO-1) ) buffer[bytes_leidos] = '\0'; 
                                //envio el dato
                                if( bytes_leidos!=0 ) send( socket, buffer, LARGO, 0 ); 
                        }
                        //envio la condicion de finalizacion
                        strcpy(buffer, "TERMINAR");
                        send( socket, buffer, LARGO, 0 );
                        close(fd);
                }
                else error = ERRFILE;
        }
}
void eliminar_documento(Nodo_t ** lista_documentos, pthread_mutex_t * semaforo, int socket)
{
        char buffer_cod[9], nom_arch[LARGO] = "biblioteca/";
        int error;

        recv(socket, buffer_cod, 9, 0);

        pthread_mutex_lock(semaforo);
        error = eliminar(lista_documentos, buffer_cod, cmp_codigo, liberar_documento);
        pthread_mutex_unlock(semaforo);

        strcat(nom_arch, buffer_cod);
        //si lo elimina es OK y elimino el documento
        if( error==OK ) remove( nom_arch );

        send(socket, &error, sizeof(int), 0);
}
void * escuchar_conexiones(Argumento_t * argumentos)
{
        int misocket, nuevo_socket;
        pthread_t thread;
        Argumento_t arg_pth;
	struct sockaddr_in sudireccion;
	socklen_t leng = sizeof(sudireccion);

        misocket = iniciar_socket(argumentos->puerto);

        argumentos->socket = misocket;

        if( misocket!=-1 )
        {
                pthread_mutex_init( &(arg_pth.semaforo),NULL );
                while( 1 )
                {              
                        listen(misocket, NBACKLOG);

                        //funciona como lockeo
                        nuevo_socket = accept(misocket, (struct sockaddr *) &sudireccion, &leng);
                        
                        printf("Conexión establecida desde %s\n", inet_ntoa(sudireccion.sin_addr));

                        //lockeo para que no tenga problema con el socket de cada cliente
                        pthread_mutex_lock( &(arg_pth.semaforo) );

                        arg_pth.socket = nuevo_socket;
                        arg_pth.lista_usuarios = argumentos->lista_usuarios;
                        arg_pth.lista_documentos = argumentos->lista_documentos;

                        pthread_create( &thread, NULL, (void *)crear_thread_cliente, (void *)&arg_pth );
                        pthread_detach(thread);

                        usleep(5000);
                }
        }
        return NULL;
}
void * crear_thread_cliente(Argumento_t * dato)
{
        int nuevosocket, error, peticion, nro_menu;
        int terminar = ERROR;

        nuevosocket = dato->socket;
        pthread_mutex_unlock( &(dato->semaforo) );

        //tengo un error que si digamos lo pongo la condicion de salida en el while no me funciona
        while( terminar!=OK )
        {
                recv(nuevosocket, &nro_menu, sizeof(int), 0);
                recv(nuevosocket, &peticion, sizeof(int), 0);
                
                switch( nro_menu )
                {
                        case 1:
                                if( peticion==1 ) error = agregar_usuario(dato->lista_usuarios, nuevosocket, &(dato->semaforo));
                                else if( peticion==2 ) error = validar_usuario(dato->lista_usuarios, nuevosocket);
                                else if( peticion==3 ) error = eliminar_usuario(dato->lista_usuarios, nuevosocket, &(dato->semaforo));
                                break;
                        case 2:
                                if( peticion==1 ) enviar_datos_documentos(*dato->lista_documentos, nuevosocket);
                                else if( peticion==2 ) error = recibir_documento(dato->lista_documentos, nuevosocket);
                                break;
                        case 3:
                                if( peticion==3 || peticion==4 ) transferir_documento(nuevosocket, dato->lista_documentos);
                                else if( peticion==5 )  enviar_datos_documentos( *dato->lista_documentos, nuevosocket );
                                break;
                        case 4:
                                //MENU_ROOT
                                if( peticion==1 ) enviar_datos_documentos(*dato->lista_documentos, nuevosocket);
                                else if( peticion==2 ) error = recibir_documento(dato->lista_documentos, nuevosocket);
                                else if( peticion==3 ) eliminar_documento(dato->lista_documentos, &(dato->semaforo), nuevosocket);
                                break;
                }
                if( (nro_menu==1 && peticion==4) || error==ERRMEM || error==ERRFILE  ) terminar=OK;
                usleep(5000);
        }
        if( error==ERRFILE ) printf("ERROR DE MEMORIA\n");
        if( error==ERRMEM ) printf("ERROR DE ARCHIVOS\n");
        printf("Finalizo la sesion nro: %d\n", nuevosocket);
        close(nuevosocket);
        return NULL;
}
int cmp_codigo(void * cod, void * pdocumento)
{
        int error = ERROR;
        Documento_t * doc = (Documento_t*)pdocumento;

        if( strcmp((char *)cod,doc->codigo)==0 ) error = OK;

        return error;
}
void enviar_datos_documentos( Nodo_t * lista_doc, int socket)
{
        int finalizacion_cliente = OK;
        char buffer[LARGO];
        Documento_t * doc;

        while( lista_doc )
        {
                //Envio la condicion para que este en el while del cliente 
                send(socket, &finalizacion_cliente, sizeof(int), 0);

                doc = (Documento_t*)lista_doc->dato;
                //empiezo a enviar los datos del documento

                //envio el codigo
                strcpy(buffer, doc->codigo);
                send(socket, buffer, LARGO, 0);
                
                //envio el nombre
                strcpy(buffer, doc->nombre);
                send(socket, buffer, LARGO, 0);

                //envio el autor
                strcpy(buffer, doc->autor);
                send(socket, buffer, LARGO, 0);

                //envio el anio
                send(socket, &(doc->anio), sizeof(int), 0);

                lista_doc = lista_doc->next;
        }
        //envio la finalizacion del while del cliente
        finalizacion_cliente = ERROR;
        send(socket, &finalizacion_cliente, sizeof(int), 0);
}
int recibir_documento(Nodo_t ** lista_documentos, int socket)
{
        char codigo[LARGO], nombre[LARGO], autor[LARGO], buffer[LARGO], nom_arch[LARGO];
        int anio, fd, error = OK, finalizar=ERROR;
        Documento_t * doc;

        //recivo cod valido y envio error
        recv(socket, codigo, LARGO, 0);
        buscar(lista_documentos, codigo, cmp_codigo, &error);
        send(socket, &error, sizeof(int), 0);

        //si encuentro el codigo no recivo nada
        if( error!=OK && strlen(codigo)==8 )
        {
                recv(socket, nombre, LARGO, 0);
                recv(socket, autor, LARGO, 0);
                recv(socket, &anio, sizeof(int), 0);
                
                doc = nuevo_documento(codigo, nombre, autor, anio);
                if( doc )
                {
                        agregar(lista_documentos, (void *)doc);

                        strcpy( nom_arch, "biblioteca/" );
                        strcat( nom_arch, codigo);

                        //creo archivo, empiezo a recibir los datos y escribo en el archivo
                        fd = open( nom_arch, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

                        if( fd!=-1 )
                        {
                                while( finalizar!=OK )
                                {
                                        recv(socket, buffer, LARGO, 0);

                                        if( strcmp(buffer, "TERMINAR")==0 ) finalizar=OK;
                                        else dprintf(fd,"%s",buffer);
                                }
                                close(fd);
                        }
                        else error= ERRFILE;
                }
                else error = ERRMEM;
        }
        return error;
}
int __nombre(void * comp, void * dat_lista)
{
        int error = ERRUSR;
        Usuario_t * u1 = (Usuario_t*)comp;
        Usuario_t * u2 = (Usuario_t*)dat_lista;

        if( strcmp( u1->nombre, u2->nombre )==0 ) error = OK;

        return error;
}
int __nombre_contra(void * comp, void * dat_lista)
{
        int error = ERRUSR;
        Usuario_t * u1 = (Usuario_t*)comp;
        Usuario_t * u2 = (Usuario_t*)dat_lista;

        if( strcmp( u1->nombre, u2->nombre )==0 )
        {
                if( strcmp( u1->contrasenia, u2->contrasenia )==0 )
                {
                        error = OK;
                }
                else error = ERRPASS;
        }
        return error;
}
int agregar_usuario(Nodo_t ** lista_usuarios, int socket, pthread_mutex_t * semaforo)
{
        int error;
        char nombre[LARGO], contra[LARGO];
        Usuario_t * usuario;

        recv(socket, nombre, LARGO, 0);
        recv(socket, contra, LARGO, 0);

        usuario = nuevo_usuario(nombre, contra, 0);
        if( usuario )
        {
                buscar(lista_usuarios, (void *)usuario, __nombre, &error);
                send(socket, &error, sizeof(int), 0);
                //si es OK significa que encontro el usuario
                if( error!=OK )
                {
                        pthread_mutex_lock( semaforo );
                        error = agregar(lista_usuarios, usuario);
                        pthread_mutex_unlock( semaforo );
                }
        }
        else error = ERRMEM;
        return error;
}
int validar_usuario(Nodo_t ** lista_usuarios, int socket)
{
        int error;
        char nombre[LARGO], contra[LARGO];
        Usuario_t * usuario;
        Usuario_t * cliente;

        recv(socket, nombre, LARGO, 0);
        recv(socket, contra, LARGO, 0);

        usuario = nuevo_usuario(nombre, contra, 0);

        if( usuario )
        {
                cliente = buscar(lista_usuarios, (void *)usuario, __nombre_contra, &error);
                send(socket, &error, sizeof(int), 0);

                //envio el nivel si el usuario fue encontrado
                if( error==OK ) send(socket, &cliente->nivel, sizeof(int), 0);
                //libero el usuario que cree para hacer la verificacion
                liberar_usuario(usuario);
        }
        else error = ERRMEM;

        return error;
}
int eliminar_usuario(Nodo_t ** lista_usuarios, int socket, pthread_mutex_t * semaforo)
{
        int error;
        char nombre[LARGO], contra[LARGO];
        Usuario_t * usuario;

        recv(socket, nombre, LARGO, 0);
        recv(socket, contra, LARGO, 0);

        usuario = nuevo_usuario(nombre, contra, 0);

        if( usuario )
        {
                buscar(lista_usuarios, (void *)usuario, __nombre_contra, &error);
                send(socket, &error, sizeof(int), 0);

                //verifico la sesion
                if( error==OK )
                {
                        send(socket, &usuario->nivel, sizeof(int), 0);
                        //recibo una confirmacion de eliminar
                        recv(socket, &error, sizeof(int), 0);
                        if( error==1 )
                        {
                                pthread_mutex_lock( semaforo );

                                error = eliminar(lista_usuarios, (void *)usuario, __nombre, liberar_usuario);
                                //envio el error de la funcion eliminar
                                send(socket, &error, sizeof(int), 0);
                                
                                pthread_mutex_unlock( semaforo );
                        }
                }
                liberar_usuario(usuario);  //libero el usuario auxiliar
        }
        else error = ERRMEM;

        return error;
}

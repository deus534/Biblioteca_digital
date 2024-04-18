#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <signal.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "structs.h"
#include "funciones.h"

int establecer_conexion()
{
        FILE * file;
        char buffer[LON_MAX];
	int misocket, resu;
	struct sockaddr_in tudireccion;
	struct hostent * direccion;
       
        file = fopen(ARCH_CLIENTE, "r");
        if( file )
        {
                fgets(buffer, LON_MAX, file);
                misocket = socket(AF_INET, SOCK_STREAM, 0);

                if (misocket != -1)
                {
                        printf("Socket abierto\n");

                        direccion = gethostbyname(strtok(buffer,"|"));

                        tudireccion.sin_family = AF_INET;
                        tudireccion.sin_port = htons(atoi( strtok(NULL,"|") ));
                        memcpy(&(tudireccion.sin_addr), direccion->h_addr, 4);
                        memset(tudireccion.sin_zero, 0, 8);

                        resu = connect(misocket, (struct sockaddr *) &tudireccion, sizeof(tudireccion));
                        if(resu)
                        {
	                        misocket = -1;
                        }
                }
                fclose(file);
        }
        return misocket;
}
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

void mostrar_menu(int nro_menu)
{
	switch( nro_menu )
	{
		case MENU_1:
			printf("1) Agregar usuario\n2) Iniciar sesion\n3) Borrar usuario\n4) Salir\n");
			break;
		case MENU_2:
			printf("1) Buscar documento\n2) Subir documento\n3) Volver\n");
			break;
		case MENU_3:
			printf("1) Mostrar documentos ordenados por nombre\n");
			printf("2) Mostrar documentos ordenados por autor\n");
			printf("3) Mostrar un documento\n");
			printf("4) Descargar un documento\n");
			printf("5) Actualizar lista de documentos\n");
			printf("6) Volver\n");
			break;
		case MENU_ROOT:
			printf("1) Buscar documento\n2) Subir documento\n3) Eliminar documento\n4) Volver\n");
			break;
	}
}
int leer_cadena( char* cad, int lon_max  )
{
        int len;
	
        fgets(cad, lon_max, stdin);
        len = strlen(cad);

	if( cad[len-1]=='\n' ) cad[len-1] = '\0';
        else
        {
		//si leo demas, limpio mi buffer de teclado
                __fpurge(stdin);
                //len = -1; //con esto aviso que llegue a la lon max de leer
        }
		
	return len;
}
int leer_peticion()
{
	char cad[10];
	int peticion = -1;

	leer_cadena(cad, 10);
	
	if( cad[0]>='0' && cad[0]<='9' )
	{
		peticion = cad[0] - '0' ;
	}
	return peticion;
}
char root13(char c)
{
	char ret;

	if( c>='A' && c>='N') ret = c - 13;
	else ret = c + 13;
		
	if( c>='a' && c>='n' ) ret = c - 13;
	else ret = c + 13;

	return ret;
}
void encriptar_contrasenia(char* org,char* encriptado)
{
	int lon = strlen(org), i;
	char aux;

	for (i = 0; i < lon; ++i)
	{
		aux = *(org+i);
		if( (aux>='A' && aux<='Z') || (aux>='a' && aux<='z') ) *(encriptado+i) = root13(aux);
		else
		{
			if( aux>='5' ) *(encriptado+i) = aux-5;
			else
			{
				if( aux<='9' ) *(encriptado+i) = aux+5;
				else *(encriptado+i) = aux;
			}
		}
	}
	*(encriptado+i) = '\0';
}
int agregar_usuario( int socket, int * nivel)
{
	int menu = MENU_1, error = OK;
        
        //recibo del servidor si pudo o no agregar al usuario
        recv(socket, &error, sizeof(int), 0);

	if( error!=OK )
	{
                (*nivel) = USR;
                printf("Usuario agregado correctamente\n");
                menu = MENU_2;
	}
	else printf("Usuario existente\n");

	return menu;
}
int iniciar_sesion( int socket, int *nivel )
{
	int nro_menu = MENU_1, error;

        //recibo del servidor si pudo o no confirmar la existencia del usuario
        recv(socket, &error, sizeof(int), 0);

	if( error==OK )
	{
                recv(socket, nivel, sizeof(int), 0);

                if( *nivel==USR ) nro_menu = MENU_2;
                else nro_menu = MENU_ROOT;
	}
	else
	{
		if( error==ERRPASS ) printf("ERROR contrasenia incorrecta\n");
		else printf("ERROR no existe usuario\n");
	}
	return nro_menu;
}
int borrar_usuario( int socket, int * nivel )
{
	int nro_menu, error = OK, elim;
	
	nro_menu = iniciar_sesion( socket, nivel );

	//pudo validar y inicio sesion
	if( nro_menu == MENU_2 )
	{
		printf("Confirma que deseas eliminar tu usuario\n");
		printf("1) SI\n");
		printf("2) NO\n");
		//lee por consola la baja de usuario
		elim = leer_peticion();

                //SERVIDOR: envio al servidor la baja de usuario
                send(socket, &elim, sizeof(int), 0);
		if( elim == 1 )
		{

                        //SERVIDOR: recibo la confirmacion de la eliminacion
                        recv(socket, &error, sizeof(int), 0);

                        if( error==OK ) printf("Usuario borrado\n");
			else printf("ERROR no se pudo borrar el usuario\n");
				
			nro_menu = MENU_1;
		}
		else
		{
			if( elim==-1 ) printf("Introduce una opcion correcta\n");
		}
	}
	return nro_menu;
}
int realizar_menu_1( int peticion, int * nro_menu, int socket, int * nivel)
{
        char usuario[LON_MAX], contrasenia[LON_MAX], encriptado[LON_MAX];
        int error = OK;
        
        *nro_menu = MENU_0;
        
        if( peticion!= 4)
        {
                printf("Introduce tu usuario: ");
                leer_cadena(usuario, LON_MAX);

                printf("Introduce tu contrasenia: ");
                leer_cadena(contrasenia, LON_MAX);
                
                encriptar_contrasenia( contrasenia, encriptado );

                //envio el usuario y contrasenia
                send(socket, usuario, LON_MAX, 0);
                send(socket, encriptado, LON_MAX, 0);
                
                switch( peticion )
                {
                        case 1:
                                *nro_menu = agregar_usuario( socket, nivel );
                                break;
                        case 2:
                                *nro_menu = iniciar_sesion( socket, nivel );
                                break;
                        case 3:
                                *nro_menu = borrar_usuario( socket, nivel );
                                break;
                }
        }
	return error;
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
		else nuevo = NULL;  //no me dio memoria para nombre, autor
	}
	return nuevo;
}
void liberar_documento(void * dato)
{
        Documento_t * aux = (Documento_t*)dato;
        free(aux->nombre);
        free(aux->autor);
        free(aux);
}
void liberar(Nodo_t * lista, void (*liberar_dato)(void *))
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
int cmp_codigo(void * d1, void * d2)
{
        int error = ERROR;
        Documento_t * aux_d1 = (Documento_t*)d1;
        Documento_t * aux_d2 = (Documento_t*)d2;

        if( strcmp(aux_d1->codigo,aux_d2->codigo)==0 ) error = OK;
                
        return error;
}
int buscar_agregar( Nodo_t ** lista, void * dato, int (*cmp_fun)(void *,void *) )
{
        Nodo_t * aux = *lista;
        Nodo_t * nuevo = NULL;
        Nodo_t * ant = NULL;
        int find = ERROR;

        if( aux )
        {
                while( aux )
                {
                        if( cmp_fun(dato,aux->dato)==OK ) find=OK;
                        ant = aux;
                        aux = aux->next;
                }
                if( find==ERROR )
                {
                        nuevo = (Nodo_t *)malloc( sizeof(Nodo_t) );
                        if( nuevo )
                        {
                                nuevo->dato = dato;
                                nuevo->next = NULL;
                                ant->next = nuevo;
                        }
                }
        }
        else
        {
                nuevo = (Nodo_t *)malloc( sizeof(Nodo_t) );
                if( nuevo )
                {
                        nuevo->dato = dato;
                        nuevo->next = NULL;
                        *lista = nuevo;
                }
        }
        return find;
}
int buscar_documentos(Nodo_t ** lista, int * error, int socket)
{
	char cod[LON_MAX], nom[LON_MAX], aut[LON_MAX];
	int anio, condicion = OK;

	Documento_t * nuevo;
        
        while( condicion==OK )
        {
                //recibo la condicion de salida
                recv(socket, &condicion, sizeof(int), 0);

                if( condicion==OK )
                {
                        //empiezo a recibir los datos
                        //recivo el codigo
                        recv(socket, cod, LON_MAX, 0);
                        //recibo el nombre
                        recv(socket, nom, LON_MAX, 0);
                        //recibo el autor
                        recv(socket, aut, LON_MAX, 0);
                        //recibo el anio
                        recv(socket, &anio, sizeof(int), 0);

                        nuevo = nuevo_documento(cod, nom, aut, anio);
                        if( nuevo )
                        {
                                *error = buscar_agregar(lista, nuevo, cmp_codigo);
                                if( (*error)==OK ) liberar_documento((void*)nuevo);
                        }
                        else *error = ERRMEM;
                }
        }
	return MENU_3;
}
void subir_documento(Nodo_t ** lista, int * error, int socket)
{
	char codigo[LON_MAX], nombre[LON_MAX], autor[LON_MAX], buffer[LON_MAX], anio[10];
        int an, fd;
        ssize_t bytes_leidos = 1;
	Documento_t * nuevo;

	printf("Introduce el codigo: ");
	leer_cadena(codigo, LON_MAX);
        //envio el codigo al servidor y recibo la validacion en error
        send(socket, codigo, LON_MAX, 0);
        recv(socket, error, sizeof(int), 0);

        if( *error != OK && strlen(codigo)==8 )
        {
                printf("Introduce el nombre: ");
                leer_cadena(nombre, LON_MAX);
                printf("Introduce el autor: ");
                leer_cadena(autor, LON_MAX);
                printf("Introduce el anio: ");
                leer_cadena(anio, 10);

                an = atoi(anio);
                nuevo = nuevo_documento(codigo,nombre,autor,an);

                if( nuevo )
                {
                        *error = agregar(lista,(void*)nuevo);
                        
                        //envio el nombre
                        send(socket, nombre, LON_MAX, 0);
                        //envio el autor
                        send(socket, autor, LON_MAX, 0);
                        //envio el anio
                        send(socket, &an, sizeof(int), 0);

                        //SERVIDOR: solicito el nombre del archivo local para subir el documento
                        printf("Introduce el nombre del archivo local: ");
                        leer_cadena(nombre, LON_MAX);
                        
                        fd = open(nombre, O_RDONLY);

                        if( fd!=-1 )
                        {
                                while( bytes_leidos )
                                {
                                        bytes_leidos = read(fd, buffer, (LON_MAX-1));
                                        buffer[LON_MAX-1]='\0';

                                        if( bytes_leidos<(LON_MAX-1) ) buffer[bytes_leidos]='\0';

                                        if( bytes_leidos!=0 ) send(socket, buffer, LON_MAX, 0);
                                }
                                strcpy(buffer, "TERMINAR");
                                send(socket, buffer, LON_MAX, 0);
                                close(fd);
                        }
                        else printf("No pude abir archivo\n");
                }
        }
        else
        {
                if( strlen(codigo)!=8 ) printf("Introduce un codigo valido\n");
                else printf("Codigo existente\n");
        }
}

int realizar_menu_2( Nodo_t ** lista_documentos, int peticion, int * nro_menu, int socket,int nivel )
{
	int error = OK;
	*nro_menu = MENU_1;
	
	switch( peticion )
	{
		case 1:
			*nro_menu = buscar_documentos(lista_documentos, &error, socket);
			break;
		case 2:
			subir_documento(lista_documentos,&error, socket);
                        if( nivel==USR ) *nro_menu = MENU_2;
                        else *nro_menu = MENU_ROOT;
			break;
	}
	return error;
}
int cmp_nombre( void * dat1, void * dat2 )
{
	int err;
	Documento_t * d_dat1 = (Documento_t*)dat1;
	Documento_t * d_dat2 = (Documento_t*)dat2;

	err = strcmp(d_dat1->nombre,d_dat2->nombre);
        
	return err;
}
int cmp_autor( void * dat1, void * dat2 )
{
	int err;
	Documento_t * d_dat1 = (Documento_t*)dat1;
	Documento_t * d_dat2 = (Documento_t*)dat2;

	err = strcmp(d_dat2->autor, d_dat1->autor);

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
				if( fun_cmp(pdato->dato,actual->dato)<0 )
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

void mostrar_documentos(Nodo_t * lista, int criterio)
{
	int i;
	int (*fun_cmp)(void *,void *);
	char msg[30];
	Nodo_t * aux_lista = lista;
	Documento_t * dato_aux;
	//veo que criterio para ordenar
	if( criterio==NOMBRE )
	{
		fun_cmp = cmp_nombre;
		strcpy(msg,"ORDENADO POR NOMBRE");
	}
	else
	{
		fun_cmp = cmp_autor;
		strcpy(msg,"ORDENADO POR AUTOR");
	}
	//ordeno el documento
	ordenar(lista,fun_cmp);
	
	//muestro los documentos
	printf("+++++++++++++++++++++++++++\n\n");
	printf("%s\n",msg);
	printf("\t\t%-13s\t%s\t%13s\t\t%s\n","NOMBRE","CODIGO", "AUTOR","ANIO");
	for(i=1; aux_lista;i++ )
	{
		dato_aux = (Documento_t *)aux_lista->dato;

		printf("\t%d) %-20s\t",i, dato_aux->nombre);
		printf("(%s)\t", dato_aux->codigo);
		printf("%-10s\t", dato_aux->autor);
		printf("%d\n", dato_aux->anio);

		aux_lista = aux_lista->next;
	}
	printf("\n\n+++++++++++++++++++++++++++\n");
}
void visualizar_documento(int socket)
{
        char buffer[LON_MAX];
        int finalizar;
        recv( socket, buffer, LON_MAX, 0 );
        recv( socket, buffer, LON_MAX, 0 );
        recv( socket, &finalizar, sizeof(int), 0 );

        finalizar = ERROR;
        printf("++++++++++++++++++++++++++++++++++++\n");
        while( finalizar!=OK )
        {
                recv( socket, buffer, LON_MAX, 0 );

                if( strcmp(buffer, "TERMINAR")==0 ) finalizar=OK;
                else  printf("%s",buffer);
        }
        printf("++++++++++++++++++++++++++++++++++++\n");
}
void descargar_documento(char * codigo, int socket)
{
        char buffer[LON_MAX], nom_arch[LON_MAX];
        char nombre[LON_MAX];
        char autor[LON_MAX];
        int anio;
        int error;
        time_t now;
        struct tm * local;
        int finalizar = ERROR;
        int fd;
        
        //envio el codigo y recibo la validacoion
        strcpy(buffer, codigo);
        send(socket, buffer, LON_MAX, 0);
        recv(socket, &error, sizeof(int), 0);

        if( error==OK )
        {
                recv(socket, nombre, LON_MAX, 0);
                recv(socket, autor, LON_MAX, 0);
                recv(socket, &anio, sizeof(int), 0);

                strcpy(nom_arch, "cfg/");
                strcat(nom_arch, codigo);
                strcat(nom_arch, ".txt");
                fd = open(nom_arch, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR );

                if(fd!=-1)
                {
                        while( finalizar!=OK )
                        {
                                recv(socket, buffer, LON_MAX, 0);
                                if( strcmp(buffer,"TERMINAR")==0 ) finalizar=OK;
                                else dprintf(fd, "%s", buffer);
                        }
                        close(fd);
                }
                strcpy(nom_arch, "cfg/");
                strcat(nom_arch, codigo);
                strcat(nom_arch, "-meta.txt");
                fd = open(nom_arch, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR );
                if( fd!=-1 )
                {
                        time(&now);
                        local = localtime(&now);
                        
                        //escribo los datos del documento
                        dprintf(fd, "Nombre: %s\nAutor: %s\nAnio: %d\n",nombre, autor, anio);
                        //escribo la fecha
                        dprintf(fd, "Fecha: %d/%d/%d\n", local->tm_mday, local->tm_mon, local->tm_year+1900);
                        //escribo la hora
                        dprintf(fd, "Hora: %d:%d:%d", local->tm_hour, local->tm_min, local->tm_sec);

                        close(fd);
                }
        }
        else printf("Codigo no encontrado\n");
}
int realizar_menu_3( Nodo_t ** lista_documentos, int peticion, int * nro_menu,int socket, int nivel )
{
	int error = OK;
	char buffer_cod[9];

	*nro_menu = MENU_3;
	
	switch( peticion )
	{
		case 1:
			mostrar_documentos(*lista_documentos, NOMBRE);
			break;
		case 2:
			mostrar_documentos(*lista_documentos, AUTOR);
			break;
		case 3:
                        printf("Introduce el codigo: ");
                        leer_cadena(buffer_cod, 9);
                        
                        //envio el codigo y recibo la validacion
                        send(socket, buffer_cod, 9, 0);
                        recv(socket, &error, sizeof(int), 0);

                        //podria ordenarlo un poquito mejor mostrando al principio los datos del documento
                        if( error==OK ) visualizar_documento(socket);
                        else printf("Codigo no encontrado\n");
			break;
		case 4:
                        printf("Introduce el codigo: ");
                        leer_cadena(buffer_cod, 9);
                        descargar_documento(buffer_cod, socket);
			break;
		case 5:
			buscar_documentos(lista_documentos,&error, socket);
			break;
		case 6:
                        if( nivel==USR ) *nro_menu = MENU_2;
                        else *nro_menu = MENU_ROOT; 
			break;
	}
	return error;
}
void eliminar_documento(Nodo_t ** lista_documentos, int misocket)
{
        char buffer_cod[9];
        int error;

        printf("Introduce el codigo: ");
        leer_cadena(buffer_cod, 9);

        send(misocket, buffer_cod, 9, 0);
        recv(misocket, &error, sizeof(int), 0);

        if( error==OK ) printf("Se elimino el documento\n");
        else printf("El documento no existe\n");
}
void realizar_menu_root(Nodo_t ** lista_documentos, int peticion, int * nro_menu, int misocket)
{
        if( peticion==1 || peticion ==2 ) realizar_menu_2(lista_documentos, peticion, nro_menu, misocket, ADM);
                
        else if( peticion==3 ) eliminar_documento(lista_documentos, misocket);
                
        else if( peticion==4 ) (*nro_menu)=MENU_1;
}

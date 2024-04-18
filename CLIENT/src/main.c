#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "structs.h"
#include "funciones.h"


int main(int argc, char * argv[])
{
        int nro_menu = MENU_1;
        int error = OK;
        int peticion;
        int nivel;
        //condiciones de salida de los menus, el NRO_MENU corresponde a la posicion del vector es decir : MENU_1 en pos 1 MENU_2 en pos 2 y asi
        int cod_salida[5] = {0, 4, 3, 6, 4};
        Nodo_t * lista_documentos = NULL;

        ///////////////
        //para la conexion
	int misocket;

        misocket = establecer_conexion();
        if( misocket!=-1 )
        {
                if( argc==2 )
                {
                        nro_menu = MENU_3;
                        peticion = 4;
                        send(misocket, &nro_menu,sizeof(int), 0);
                        send(misocket, &peticion,sizeof(int), 0);
                        descargar_documento(argv[1], misocket);
                        nro_menu = MENU_1;
                        peticion = 4;
                        send(misocket, &nro_menu,sizeof(int), 0);
                        send(misocket, &peticion,sizeof(int), 0);
                }
                else
                {
                        printf("CONEXION REALIZADA\n\n");
                        printf("Socket conectado\n");

                        while( nro_menu!=MENU_0 && error==OK  )
                        {
                                mostrar_menu(nro_menu);
                                peticion = leer_peticion();
                                
                                if( peticion > 0 && peticion <= cod_salida[nro_menu] )
                                {
                                        //envio el nro_menu y peticion
                                        send(misocket, &nro_menu,sizeof(int), 0);
                                        send(misocket, &peticion,sizeof(int), 0);

                                        switch( nro_menu )
                                        {
                                                case MENU_1:
                                                        realizar_menu_1(peticion, &nro_menu, misocket, &nivel);
                                                        break;
                                                case MENU_2:
                                                        realizar_menu_2(&lista_documentos, peticion, &nro_menu, misocket,nivel);
                                                        break;
                                                case MENU_3:
                                                        realizar_menu_3(&lista_documentos, peticion, &nro_menu, misocket,nivel );
                                                        break;
                                                case MENU_ROOT:
                                                        realizar_menu_root(&lista_documentos, peticion, &nro_menu, misocket);
                                                        break;
                                        }
                                }
                                else printf("++++++++INTRODUCE UNA OPCION CORRECTA++++++++\n");
                        }
                }
                liberar(lista_documentos, liberar_documento);
                close(misocket);
        }
        else printf("No se pudo establecer la conexion\n");
        return 0;
}

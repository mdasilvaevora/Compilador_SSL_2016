#define NUMESTADOS 15
#define NUMCOLS 13
#define TAMLEX 32+1
#define TAMNOM 20+1

#include <ctype.h>
#include <stdio.h>
#include <string.h>


FILE * in;
typedef enum{
	INICIO, FIN, LEER, ESCRIBIR, ID, CONSTANTE, PARENIZQUIERDO, PARENDERECHO, PUNTOYCOMA,
	COMA, ASIGNACION, SUMA, RESTA, FDT, ERRORLEXICO
	} TOKEN;

typedef struct{
    char identificador [TAMLEX];
    TOKEN tok;
    }RegTS;
RegTS TS[1000]={{"inicio",INICIO},{"fin",FIN},{"leer",LEER},{"escribir",ESCRIBIR},{"$",99}};
                                                                                  /*Centinela de fin de TS*/

char buffer[TAMLEX];


TOKEN scanner();
int columna(int);
int estadoFinal(int);
TOKEN esReservada();

/*Para la Tabla de Simbolos*/
int buscar(char*,RegTS*,TOKEN*);
void colocar(char*,RegTS*);


int main(int argc, char * argv[])
{

    char nombreArchivo[TAMNOM];
    int lenNombreArchivo;

    /*Validaciones*/

	if ( argc == 1 ){
		printf("Debe ingresar el nombre del archivo fuente en la linea de comandos\n");
		 return -1;
	}

	if ( argc != 2 ){
		printf("Numero incorrecto de argumentos\n");
		return -2;
	}

    	strcpy(nombreArchivo, argv[1]);
    	lenNombreArchivo = strlen(nombreArchivo);

	if ( lenNombreArchivo > TAMNOM ){
		printf("Nombre incorrecto del Archivo Fuente\n");
		return -3;
	}

	if ( nombreArchivo[lenNombreArchivo-1] != 'm' || nombreArchivo[lenNombreArchivo-2] != '.' ){
		printf("El nombre del Archivo Fuente debe contener una extension .m \n");
		return -4;
	}

	if ( (in = fopen(nombreArchivo, "r") ) == NULL ){
		printf("No se pudo abrir archivo fuente\n");
		return -5;
	}
	char c;
	while(!feof(in))scanner();
	return 0;
}

int columna(int c){
	if(isalpha(c)) return 0; /* Si es una letra*/
	if(isdigit(c)) return 1; /* Si es un digito*/
	if(c == '+') return 2;
	if(c == '-') return 3;
	if(c == '(') return 4;
	if(c == ')') return 5;
	if(c == ',') return 6;
	if(c == ';') return 7;
	if(c == ':') return 8;
	if(c == '=') return 9;
	if(c == EOF) return 10;
	if(isspace(c)) return 11; /*Si es un espacio*/
	return 12;
}
int estadoFinal(int e){
	if(e == 0 || e == 1 || e == 3 || e == 11 || e == 14) return 0;
	return 1;
}


TOKEN scanner(){
	static int tabla[NUMESTADOS][NUMCOLS] =
{   { 1,  3,  5,  6,  7,  8,  9, 10, 11, 14, 13,  0, 14 },
    {  1,  1,  2,  2,  2,  2,2,  2,  2,  2,  2,  2,  2 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    {  4,  3,  4,  4,  4,  4,  4,  4,  4, 4, 4, 4, 4 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 12, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 } };

	TOKEN t[20];
	int caracter,col,estado,i;
	i=estado=0;

	do{
		caracter = fgetc(in);
		col = columna(caracter);
		estado = tabla[estado][col];
		if(col != 11){
 			buffer[i] = caracter;
			i++;
		}
	}
	while(!(estadoFinal(estado)) && !(estado == 14));
	buffer[i] = '\0';
	

	switch(estado){
		case 2: if(col != 11){
			ungetc(caracter,in);
			buffer[i-1] = '\0';
			}
			if(esReservada()== ID)printf("Identificador = %s \n",buffer);
			else printf("Palabra Reservada = %s \n",buffer);
			return esReservada();
		case 4: if(col != 11){
			ungetc(caracter,in);
			buffer[i-1] = '\0';
			}
			colocar(buffer,TS);
			printf("Constante = %s \n",buffer);return CONSTANTE;
		case 5: printf("Suma\n");return SUMA;
		case 6: printf("Resta\n");return RESTA;
		case 7: printf("ParenIzq \n");return PARENIZQUIERDO;
		case 8: printf("ParenDer \n");return PARENDERECHO;
		case 9: printf("Coma \n");return COMA;
		case 10: printf("PuntoYComa \n");return PUNTOYCOMA;
		case 12: printf("Asignacion \n");return ASIGNACION;
		case 13: printf("Fin de Texto\n");return FDT;
		case 14: printf("Error Lexico\n");return ERRORLEXICO;
	}
	return 0;
}

void colocar(char * id, RegTS * TS);
void colocar(char * id, RegTS * TS)
{
    /* Agrega un ID a la TS */
    int i = 4;
    while ( strcmp("$", TS[i].identificador) ) i++; /*Mientras no encuentre la centinela avanza en el array*/
    if ( i < 999 )                                  /* Controla que no se exceda del tamaño maximo del array*/
    {
    strcpy(TS[i].identificador, id );               /*Actualiza el nombre del identificador y el token*/
    TS[i].tok= ID;
    strcpy(TS[++i].identificador, "$" );             /*Avanza y coloca el centinela*/
    }
}

TOKEN esReservada();
TOKEN esReservada(){
    if(strcmp(buffer,"inicio")==0){ return INICIO;}
    if(strcmp(buffer,"fin")==0) { return FIN;}
    if(strcmp(buffer,"leer")==0) { return LEER;}
    if(strcmp(buffer,"escribir")==0) { return ESCRIBIR;}
    return ID;
}


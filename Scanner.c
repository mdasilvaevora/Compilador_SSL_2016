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

char buffer[TAMLEX];

TOKEN scanner();
int columna(int c);
int estadoFinal(int e);

int main(int argc, char * argv[])
{

    char nombreArchivo[TAMNOM];
    int lenNombreArchivo;

    //Validaciones

if ( argc == 1 )
{
printf("Debe ingresar el nombre del archivo fuente en la linea de comandos\n");
 return -1;
}

if ( argc != 2 )
{
printf("Numero incorrecto de argumentos\n");
return -2;
}
    strcpy(nombreArchivo, argv[1]);
    lenNombreArchivo = strlen(nombreArchivo);
if ( lenNombreArchivo > TAMNOM )
{
printf("Nombre incorrecto del Archivo Fuente\n");
return -3;
}

if ( nombreArchivo[lenNombreArchivo-1] != 'm' || nombreArchivo[lenNombreArchivo-2] != '.' )
{
printf("El nombre del Archivo Fuente debe contener una extension .m \n");
return -4;
}
if ( (in = fopen(nombreArchivo, "r") ) == NULL )
{
printf("No se pudo abrir archivo fuente\n");
return -5;
}

return 0;
}



int columna(int c){
	if(isalpha(c)) return 0; // Si es una letra
	if(isdigit(c)) return 1; // Si es un digito
	if(c == '+') return 2;
	if(c == '-') return 3;
	if(c == '(') return 4;
	if(c == ')') return 5;
	if(c == ',') return 6;
	if(c == ';') return 7;
	if(c == ':') return 8;
	if(c == '=') return 9;
	if(c == EOF) return 10;
	if(isspace(c)) return 11; //Si es un espacio
	return 12;
}
int estadoFinal(int e){
	if(e == 0 || e == 1 || e == 3 || e == 11 || e == 14) return 0;
	return 1;
}
TOKEN scanner(){
	static int tabla[NUMESTADOS][NUMCOLS] =
            //L D + - ( ) , ; : = EOF sp OTRO
			{
			    {1,3,5,6,7,8,9,10,11,14,13,0,14},
			     {1,1,2,2,2,2,2,2,2,2,2,2,2},
/*ID*/	         {14,14,14,14,14,14,14,14,14,14,14,14,14},
		     	 {4, 3,4,4,4,4,4,4,4,4,4,4,4},
/*ID*/           {14,14,14,14,14,14,14,14,14,14,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,14,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,14,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,14,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,14,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,14,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,12,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,14,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,14,14,14,14},
/*ID*/			 {14,14,14,14,14,14,14,14,14,14,14,14,14}
            };

	int caracter,columna,estado,i;
	i=estado=0;

	do{
		caracter = fgetc(in);
		columna = columna(caracter);
		estado = tabla[estado][columna];
		if(columna != 11){
 			buffer[i] = caracter;
			i++;
		}
	}
	while(!(estadoFinal(estado)) && !(estado == 14));
	buffer[i] = '\0';

	switch(estado){
		case 2: if(columna != 11){
			ungetc(caracter,in);
			buffer[i-1] = '\0';
			}
			return ID;
		case 4: if(columna != 11){
			ungetc(caracter,in);
			buffer[i-1] = '\0';
			}
			return  CONSTANTE;
		case 5: return SUMA;
		case 6: return RESTA;
		case 7: return PARENIZQUIERDO;
		case 8: return PARENDERECHO;
		case 9: return COMA;
		case 10: return PUNTOYCOMA;
		case 12: return ASIGNACION;
		case 13: return FDT;
		case 14: return ERRORLEXICO;
	}
	return 0;
}

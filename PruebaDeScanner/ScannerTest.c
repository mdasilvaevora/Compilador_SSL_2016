#define NUMESTADOS 15
#define NUMCOLS 13
#define TAMLEX 32+1
#define TAMNOM 20+1

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef enum{
	INICIO, FIN, LEER, ESCRIBIR, ID, CONSTANTE, PARENIZQUIERDO, PARENDERECHO, PUNTOYCOMA,COMA, ASIGNACION, SUMA, RESTA, FDT, ERRORLEXICO
	} TOKEN;

char buffer[TAMLEX];


TOKEN scanner(char * s);
int columna(char);
int estadoFinal(int);
TOKEN esReservada();

int main(int argc, char * argv[])
{


    /*Validaciones*/

	if ( argc == 1 ){
		printf("Debe ingresar la palabra a reconocer por el Scanner\n");
		 return -1;
	}

	if ( argc != 2 ){
		printf("Numero incorrecto de argumentos\n");
		return -2;
	}
	
	scanner(argv[1]);
	return 0;
}

int columna(char c){
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


TOKEN scanner(char * s){
	static int tabla[NUMESTADOS][NUMCOLS] =
{   { 1,  3,  5,  6,  7,  8,  9, 10, 11, 14, 13,  0, 14},
    { 1,  1,  2,  2,  2,  2, 2,  2,  2,  2,  2,  2,  2},
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
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 }};

	char caracter = *s;
	int col;
	int estado,i,n,k;
	estado=i=0;

	n = strlen(s);

	/*printf("%s %s %d\n",s,"s antes del while",n);*/

	do{	
		col=columna(caracter);	
		estado = tabla[estado][col];
		
		/*el problema era que cuando leia la letra siguiente al numero, el estado quedaba igualado
		a 4 entonces por eso devolvia constante, el if siguiente soluciona eso comparando las
		longitudes de la palabra que le pasas por linea de comando, y la longitud del buffer,
		siempre y cuando el estado sea un estado final*/

		if(estadoFinal(estado) && n > k){ 
			estado = 14;		
			break;
		}		
			
		
		if(col != 11){
 			buffer[i] = caracter;
			i++;
		}

		k = strlen(buffer);
		
		s++;
		caracter=*s;
	
	}
	while(!(estadoFinal(estado)) && !(estado == 14));
	buffer[i] = '\0';
	
	/*k = strlen(buffer);*/
	/*l = strlen(s);*/

	/*printf("%s %s %d\n", s,"s despues del while",l);*/
	/*printf("%s %s %d\n",buffer, "buffer despues del while",k);*/
		
	
	switch(estado){
		case 2: if(col != 11){
			buffer[i-1] = '\0';
			}
			if(esReservada()== ID)printf("Identificador = %s \n",buffer);
			else printf("Palabra Reservada = %s \n",buffer);
			return esReservada();
		case 4: if(col != 11){
			buffer[i-1] = '\0';
			}
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


TOKEN esReservada(){
    if(strcmp(buffer,"inicio")==0){ return INICIO;}
    if(strcmp(buffer,"fin")==0) { return FIN;}
    if(strcmp(buffer,"leer")==0) { return LEER;}
    if(strcmp(buffer,"escribir")==0) { return ESCRIBIR;}
    return ID;
}


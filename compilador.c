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

typedef struct{
	char identificador[TAMLEX];
	TOKEN t; /* t=0,1,2,3 ->Palabra Reservada. Y si t=4->Identificador*/
	} RegTS;

RegTS TS[1000] = { {"inicio", INICIO}, {"fin", FIN}, {"leer", LEER}, {"escribir", ESCRIBIR}, {"$", 99} };

typedef struct{
	TOKEN clase;
	char nombre[TAMLEX];
	int valor;
} REG_EXPRESION


TOKEN tokenActual;
int flagToken;
char buffer[TAMLEX];
FILE* in;

/*PAS*/

/* GRAMATICA SEMANTICA
<objetivo> -> <programa> FDT #terminar
<programa> -> #comenzar INICIO <listaSentencias> FIN
<listaSentencias> -> <sentencia> {<sentencia>}
<sentencia> -> <identificador> := <expresion> #asignar ; |
			   LEER ( <listaIdentificadores> ) ; |
			   ESCRIBIR ( <listaExpresiones> ) ;
<listaIdentificadores> -> <identificador> #leer_id {, <identificador> #leer_id}
<listaExpresiones> -> <expresion> #escribir_exp {, <expresion> #escribir_exp}
<expresion> -> <primaria> {<operadorAditivo> <primaria> #gen_infijo}
<primaria> -> <identificador> |
              CONSTANTE #procesar_cte |
			  ( <expresion> )
<operadorAditivo> -> SUMA #procesar_op | RESTA #procesar_op
<identificador> -> ID #procesar_id */

void Objetivo();
void Programa();
void ListaSentencias();
void Sentencia();
void ListaIdentificadores();
void ListaExpresiones();

void Expresion(REG_EXPRESION *resultado);
void Primaria (REG_EXPRESION *reg);
void OperadorAditivo(REG_OPERACION *op);
void Identificador(REG_EXPRESION *reg);

/*Rutinas semanticas*/
REG_EXPRESION ProcesarCte(void);
REG_EXPRESION ProcesarId(void);
void Leer(REG_EXPRESION in);
void Escribir(REG_EXPRESION out);

/*Funciones auxiliares */
void Match(TOKEN t);
TOKEN ProximoToken();
void ErrorLexico();
void ErrorSintactico();
int Buscar(char * id, RegTS * TS, TOKEN * t);
void Colocar(char * id, RegTS * TS);
void Chequear(char * s);
void Comenzar(void);
void Terminar(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der)

void Generar();/* Falta desarrollar- pagina 52*/
void Extraer(); /*Falta desarrollar pagina 52 */

/*Scanner*/
TOKEN scanner();
int columna(char);
int estadoFinal(int);
TOKEN esReservada();



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

	Objetivo();
	fclose(in);
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


TOKEN scanner(){
	static int tabla[NUMESTADOS][NUMCOLS] =
{   { 1,  3,  5,  6,  7,  8,  9, 10, 11, 14, 13,  0, 14},
    { 1,  1,  2,  2,  2,  2, 2,  2,  2,  2,  2,  2,  2},
    { 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
    { 14,  3,  4,  4,  4,  4,  4,  4,  4, 4, 4, 4, 4 }, /**/
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
			return esReservada();
		case 4: if(col != 11){
			ungetc(caracter,in);
			buffer[i-1] = '\0';
			}
			return CONSTANTE;
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


TOKEN esReservada(){
    if(strcmp(buffer,"inicio")==0){ return INICIO;}
    if(strcmp(buffer,"fin")==0) { return FIN;}
    if(strcmp(buffer,"leer")==0) { return LEER;}
    if(strcmp(buffer,"escribir")==0) { return ESCRIBIR;}
    return ID;
}

									/* PAS */
void Objetivo(){
	/* <objetivo> -> <programa> FDT */
	Programa();
	Match(FDT);
}

void Programa(){
	/* #comenzar inicio <listaSentencias> fin */
	Comenzar();
	Match(INICIO);
	ListaSentencias();
	Match(FIN);
}


void ListaSentencias(){
	/* <listaSentencias> -> <sentencia> {<sentencia>} */
	Sentencia()
	while(1) {
		switch (ProximoToken()) {
			case ID: case LEER: case ESCRIBIR:
				Sentencia();
				break;
			default:
				return;
					}
		}
}

void Sentencia(){
	TOKEN tok = ProximoToken();
	switch(tok) {
		case ID : /* <sentencia> -> ID := <expresion>; */
		     Match(ID); Match(ASIGNACION); Expresion(); Match (PUNTOYCOMA);
			break;
		case LEER: /* <sentencia> -> LEER ( <listaIdentificadores>); */
			Match(LEER); Match(PARENIZQUIERDO); ListaIdentificadores(); Match(PARENDERECHO); Match(PUNTOYCOMA);
			break;
		case ESCRIBIR: /* <sentencia> -> ESCRIBIR (<listaExpresiones>); */
			Match(ESCRIBIR); Match(PARENIZQUIERDO); ListaExpresiones(); Match(PARENDERECHO); Match(PUNTOYCOMA);
			break;
		default:
			ErrorSintatico(tok); break;
		}
}

void ListaIdentificadores(){ /*Falta agregar procesamiento semantico */
	/* <listaIdentificadores> -> <identificador> #leer_id { COMA <identificador> #leer_id} */
	Match(ID);
	while(ProximoToken() == COMA) {
		Match(COMA); Match(ID);
		}
}

void ListaExpresiones(){ /*Falta agregar procesamiento semantico */
	/* <listaExpresiones> -> <expresion> #escribir_exp { COMA <expresion> #escribir_exp} */
	Expresion();
	while(1) {
		switch(ProximoToken()){
			case COMA: Match(COMA); Expresion();
			default: return;
			}
		}
}

void Expresion(REG_EXPRESION *resultado){
	/* <expresion> -> <primaria> {<operadorAditivo> <primaria>} */
	REG_EXPRESION operandoIzq, operandoDer;
	REG_OPERACION op;	
	TOKEN t;
	Primaria(&operandoIzq)
	for(t = ProximoToken() ; t==SUMA || t==RESTA ; t = ProximoToken())
		{
			OperadorAditivo(&op); Primaria(&operandoDet);
			operandoIzq = GenInfijo (operandoIzq, op, operandoDer);
		}
	*resultado = operandoIzq;
}

void Primaria(){ /*Falta agregar procesamiento semantico */
	/* <primaria> -> ID | CONSTANTE | PARENIZQUIERDO <expresion> PARENDERECHO */
	TOKEN tok= ProximoToken();
	switch(tok){
		case ID: Match(ID); break;
		case CONSTANTE: Match(CONSTANTE); break;
		case PARENIZQUIERDO: Match(PARENIZQUIERDO); Expresion(); Match(PARENDERECHO); break;
		default:
			ErrorSintactico(tok); break;
	}
}

void OperadorAditivo(){ /* Sin procesamiento semantico */
	/* <operadorAditivo> -> uno de SUMA RESTA */
	TOKEN t= ProximoToken();
	if(t == SUMA || t== RESTA) Match(t);
	else ErrorSintactico(t);
}


									/* RUTINAS SEMANTICAS */
void Leer(REG_EXPRESION in){
	/* genera la instruccion para leer*/
	Generar("Read",in.nombre,"Entera", "");
}

void Escribir(REG_EXPRESION out){
	Generar("Write",extraer(out),"Entera","");
}

REG_EXPRESION ProcesarID(){
	/* Declara un ID y construye el correspondiente registro semantico */
	REG_EXPRESION t;
	Chequear(buffer);
	t.clase = ID;
	strcpy(t.nombre,buffer);
	return t;
}

REG_EXPRESION ProcesarCte(){
	/* convierte una cadena que representa un numero, a un numero entero y construye un registro semantico" */
	REG_EXPRESION t;
	t.clase = CONSTANTE;
	sscanf(buffer,"%d", &t.valor);
	return t;
}
								/* FUNCIONES AUXILIARES */

void Match(TOKEN t){
	if(t!=ProximoToken()) ErrorSintactico();
	flagToken=0;
}

void ProximoToken(){
	if ( !flagToken )
		{
			tokenActual = scanner();
			if ( tokenActual == ERRORLEXICO ) ErrorLexico();
			flagToken = 1;
			if ( tokenActual == ID )
				{
				Buscar(buffer, TS, &tokenActual);
				}
		}
	return tokenActual;
}

void ErrorSintactico(){
	printf("Error Sintactico");
}
void ErrorLexico(){
	printf("Error Lexico");
}

int Buscar(char *id, RegTS *TS, TOKEN *t){
	/*Busca un identificador en la TS*/
	int i=0;
	while(strcpm("$",TS[i].identificador)){
		if(!strcmp(id,TS[i].identificador)) {
			*t=TS[i].t;
			return 1;
		}
	i++;
	}
	return 0;
}

void Colocar(char * id, RegTS * TS)
{
	/* Agrega un identificador a la TS */
	int i = 4;
	while ( strcmp("$", TS[i].identificador) ) i++;
	if ( i < 999 )
	{
		strcpy(TS[i].identificador, id );
		TS[i].t = ID;
		strcpy(TS[++i].identificador, "$" );
	}
}

void Generar();
void Extraer();

void Chequear(string s){
	if(!Buscar(s)) { /* Si la cadena no esta en la TS */
		Colocar(s);	/* Almacena */
		Generar("Declara",s,"Entera",""); /*Genera la instruccion*/
	}
}
void Comenzar(); /*Inicializaciones semanticas*/
void Terminar(){
	/* Genera la instruccion para terminar la ejecucion del programa*/
	Generar("Detiene","","","");
}
void Asignar(REG_EXPRESION izquierda,REG_EXPRESION derecha){
	/* genera la instruccion para la asignacion */
	Generar("Almacena",Extraer(derecha),izquierda.nombre,"");
}

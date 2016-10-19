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


void objetivo (void) {
	programa();
	match(FDT);
	terminar();
}

void programa (void) {
	comenzar();
	match(INICIO);
	listaSentencias();
	match(FIN);
}

void listaSentencias (void) {
	sentencia();
	while(1)
	{
		switch(proximoToken())
		{
			case ID: case LEER: case ESCRIBIR:
				sentencia();
				break;
			default:
				return;
		}
	}
}

void sentencia (void) {
	Token tok = proximoToken();
	REG_EXPRESION izq, der;
	switch(tok)
	{
		case ID:
			identificador(&izq);
			match(ASIGNACION);
			expresion(&der);
			asignar(izq, der);
			match(PUNTOYCOMA);
			break;
		case LEER:
			match(LEER);
			match(PARENIZQUIERDO);
			listaIdentificadores();
			match(PARENDERECHO);
			match(PUNTOYCOMA);
			break;
		case ESCRIBIR: 
			match(ESCRIBIR);
			match(PARENIZQUIERDO);
			listaExpresiones();
			match(PARENDERECHO);
			match(PUNTOYCOMA);
			break;
		default:
			errorSintactico(tok);
			break;	
	}
}

void listaIdentificadores (void) {
	TOKEN t;
	REG_EXPRESION reg;
	identificador(&reg);
	leer(reg);
	t = proximoToken();
	while(t == COMA)
	{
		match(COMA);
		identificador(&reg);
		leer(reg);
		t = proximoToken();
	}
}

void listaExpresiones (void) {
	TOKEN t;
	REG_EXPRESION reg;
	expresion(&reg);
	escribir(reg);
	t = proximoToken();
	while(t == COMA)
	{
		match(COMA);
		expresion(&reg);
		escribir(reg);
		t = proximoToken();		
	}
}

void expresion (REG_EXPRESION *resultado)
{
	TOKEN t;
	REG_EXPRESION izq, der;
	REG_OPERACION op;
	primaria(&izq);
	t = proximoToken();
	while(t == SUMA || t == RESTA)
	{
		operadorAditivo(&op);
		primaria(&der);
		izq = genInfijo(izq, op, der);
		t = proximoToken();		
	}
	*resultado = izq;
}

void primaria (REG_EXPRESION *reg)
{
	TOKEN t;
	t = proximoToken();
	switch(t)
	{
		case ID:
			identificador(reg)
			break;
		case CONSTANTE:
			match(CONSTANTE);
			*reg = procesarCTE();
			break;
		case PARENIZQUIERDO:
			match(PARENIZQUIERDO);
			expresion(reg);
			match(PARENDERECHO);
			break;
		default :
			return;
	}
}

void operadorAditivo(REG_OPERACION *op)
{
	TOKEN t;
	t = proximoToken();
	if(t == SUMA || t == RESTA)
	{
		match(t);
		*op = t;
	}
}

void identificador(REG_EXPRESION *reg)
{
	match(ID)
	*reg = procesarID()
}

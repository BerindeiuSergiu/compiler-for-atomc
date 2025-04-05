#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"
#include "utils.h"
#include "ad.h"

Token *iTk;		   // the iterator in the tokens list
Token *consumedTk; // the last consumed token

Symbol *owner; // owner-ul simbolului curent (NULL pentru simboluri globale -> init cu NULL)


void tkerr(const char *fmt, ...)
{
	fprintf(stderr, "error in line %d: ", iTk->line);
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

const char *tkCodeName(int code)
{
	return printAtom(code);
}

bool consume(int code)
{
	printf("consume(%s)", tkCodeName(code));
	if (iTk->code == code)
	{
		switch (code)
		{
		case ID:
			printf(" ID: %s", iTk->text);
			break;
		case INT:
			printf(" INT: %d", iTk->i);
			break;
		case DOUBLE:
			printf(" DOUBLE: %g", iTk->d);
			break;
		case CHAR:
			printf(" CHAR: %c", iTk->c);
			break;
		case STRING:
			printf(" STRING: %s", iTk->text);
			break;
		default:
			break;
		}
		consumedTk = iTk;
		iTk = iTk->next;
		printf(" => consumed\n");
		return true;
	}
	printf(" => found %s\n", tkCodeName(iTk->code));
	return false;
}

// typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase(Type *t) {
    printf("# typeBase\n");
    Token *start = iTk;
    t->n = -1; // initializare dimensiune cu -1

    if (consume(TYPE_INT)) {
        t->tb = TB_INT; // setam tipul de baza ca INT
        return true;
    }
    if (consume(TYPE_DOUBLE)) {
        t->tb = TB_DOUBLE; // setam tipul de baza ca DOUBLE
        return true;
    }
    if (consume(TYPE_CHAR)) {
        t->tb = TB_CHAR; // setam tipul de baza ca CHAR
        return true;
    }
    if (consume(STRUCT)) {
        Token *tkName = consumedTk;// se salveaza numele structurii
        if (consume(ID)) {
            t->tb = TB_STRUCT; // setam tipul de baza ca STRUCT
            t->s = findSymbol(tkName->text); //cautam structura in tabela de simboluri
            if (!t->s) {
				///// daca structura nu este definita aruncam eroare /////
                tkerr("structura nedefinita: %s", tkName->text); 
            }
            return true;
        } else {
            tkerr("eroare de sintaxa, lipseste numele structurii dupa struct\n");
        }
    }
    iTk = start;
    return false;
}

// unit: ( structDef | fnDef | varDef )* END
bool unit()
{
	printf("# unit\n");
	for (;;)
	{
		if (structDef())
		{
		}
		else if (fnDef())
		{
		}
		else if (varDef())
		{
		}
		else
			break;
	}
	if (consume(END))
	{
		return true;
	}else tkerr("eroare de sintaxa, programul nu poate fi incheiat corect\n");
	return false;
}

// structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef()
{
	printf("# structDef\n");
	Token *start = iTk;
	if (consume(STRUCT))
	{
		Token *tkName = consumedTk;
		if (consume(ID))
		{
			Symbol *s = findSymbolInDomain(symTable, tkName->text);
            if (s)
                tkerr("symbol redefinition: %s", tkName->text);
            s = addSymbolToDomain(symTable, newSymbol(tkName->text, SK_STRUCT));
            s->type.tb = TB_STRUCT;
            s->type.s = s;
            s->type.n = -1;
            pushDomain();
            owner = s;
			if (consume(LACC))
			{
				for (;;)
				{
					if (varDef())
					{
					}
					else
					{
						break;
					}
				}
				if (consume(RACC))
				{
					owner = NULL;
                    dropDomain();
					if (consume(SEMICOLON))
					{
						return true;
					}else tkerr("lipseste ; la final\n");
				}else tkerr("eroare de sintaxa, lipseste }\n");
			}
		}else tkerr("lipseste numele var dupa struct\n");
	}
	iTk = start;
	return false;
}

// varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef() {
    printf("# varDef\n");
    Token *start = iTk;
    Type t; // timpul variabilei

    if (typeBase(&t)) { 
        Token *tkName = consumedTk; // se salveaza numele variabilei
        if (consume(ID)) { 
            if (arrayDecl(&t)) { // se proceseaza un array
                if (t.n == 0) {
                    tkerr("a vector variable must have a specified dimension");
                }
            }
            if (consume(SEMICOLON)) {
                Symbol *var = findSymbolInDomain(symTable, tkName->text);
                if (var) {
                    tkerr("symbol redefinition: %s", tkName->text);
                }
                var = newSymbol(tkName->text, SK_VAR);
                var->type = t;
                var->owner = owner;
                addSymbolToDomain(symTable, var);

                if (owner) { // verificam daca variabila aparine unei functii sau unei structuri
                    switch (owner->kind) {
                        case SK_FN: // variabilă locală într-o funcție
                            var->varIdx = symbolsLen(owner->fn.locals);
                            addSymbolToList(&owner->fn.locals, dupSymbol(var));
                            break;
                        case SK_STRUCT: // membru al unei structuri
                            var->varIdx = typeSize(&owner->type);
                            addSymbolToList(&owner->structMembers, dupSymbol(var));
                            break;
                    }
                } else { // cazul in care variabila este locala
                    var->varMem = safeAlloc(typeSize(&t));
                }
                return true;
            } else {
                tkerr("lipseste ; la final\n");
            }
        } else {
            tkerr("lipseste numele variabilei dupa declaratia tipului de date\n");
        }
    }
    iTk = start;
    return false;
}

// arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl(Type *t)
{
	printf("# arrayDecl\n");
	Token *start = iTk;
	if (consume(LBRACKET))
	{
		if (consume(INT))
		{
			Token *tkSize = consumedTk;
			t->n = tkSize->i; // pentru cazul in care se specifica dimensiunea array-ului: int v[10]
		}else{
			t->n = 0; //pentru cazul in care nu se specifica dimensiunea array-ului: int v[]
		}
		if (consume(RBRACKET))
		{
			return true;
		}else tkerr("lipseste ], declarare incompleta\n");
	}
	iTk = start;
	return false;
}

// fnDef: ( typeBase | VOID ) ID
// LPAR ( fnParam ( COMMA fnParam )* )? RPAR
// stmCompound
bool fnDef()
{
	printf("# fnDef\n");
	Token *start = iTk;
	if (typeBase() || consume(VOID))
	{
		if (consume(ID))
		{
			if (consume(LPAR))
			{
				if (fnParam())
				{
					for (;;)
					{
						if (consume(COMMA))
						{
							if (fnParam())
							{
							}else tkerr("lipseste parametru dupa ,\n");
						}
						else
						{
							break;
						}
					}
				}
				if (consume(RPAR))
				{
					if (stmCompound())
					{
						return true;
					}else tkerr("sintaxa incorecta, lipseste corpul\n");
				}tkerr("conditie invalida pentru declararea functiei sau lipseste ) dupa (");
			}
		}
	}
	iTk = start;
	return false;
}

// fnParam: typeBase ID arrayDecl?
bool fnParam()
{
	printf("# fnParam\n");
	Token *start = iTk;
	if (typeBase())
	{
		if (consume(ID))
		{
			if (arrayDecl())
			{
			}
			return true;
		}else tkerr("lipseste identificator dupa declaratia tipului de date\n");
		
	}
	iTk = start;
	return false;
}

// stm: stmCompound
// | IF LPAR expr RPAR stm ( ELSE stm )?
// | WHILE LPAR expr RPAR stm
// | RETURN expr? SEMICOLON
// | expr? SEMICOLON
bool stm()
{
	printf("# stm\n");
	Token *start = iTk;
	if (stmCompound())
	{
		return true;
	}
	if (consume(IF))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (consume(RPAR))
				{
					if (stm())
					{
						if (consume(ELSE))
						{
							if (stm())
							{
								return true;
							}else tkerr("lipseste corpul pentru else\n");
						}
					}else tkerr("lipseste corpul pentru if\n");
					return true;
				}else tkerr("conditie invaldia: in if sau lipsa de )");
			}else tkerr("lipseste expresia dupa (\n");
		}else tkerr("lipseste ( dupa if\n");
	}
	iTk = start; //aici am adaugat
	if (consume(WHILE))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (consume(RPAR))
				{
					if (stm())
					{
						return true;
					}else tkerr("lipseste corpul lui while\n");
				}else tkerr("conditie invalida pentru while sau lipseste ) dupa expresia din while\n");
			}else tkerr("lipseste expresia dupa ( din while\n");
		}else tkerr("lipseste ( dupa while\n");
	}
	iTk = start; //aici am adaugat
	if (consume(RETURN))
	{
		if (expr())
		{
		}
		if (consume(SEMICOLON))
		{
			return true;
		}else tkerr("lipseste ; dupa return sau expresie return\n");
	}
	iTk = start; //aici am adaugat
	if (expr())
		{
		}
		if (consume(SEMICOLON))
		{
			return true;
		}
	iTk = start;
	return false;
}

// stmCompound: LACC ( varDef | stm )* RACC
bool stmCompound()
{
	printf("# stmCompound\n");
	Token *start = iTk;
	if (consume(LACC))
	{
		for (;;)
		{
			if (varDef() || stm())
			{
			}
			else
			{
				break;
			}
		}
		if (consume(RACC))
		{
			return true;
		}else tkerr("lipseste } dupa { \n");
	}
	iTk = start;
	return false;
}

// expr: exprAssign
bool expr()
{
	printf("# expr\n");
	Token *start = iTk;
	if (exprAssign())
	{
		return true;
	}
	iTk = start;
	return false;
}

// exprAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign()
{
	printf("# exprAssign\n");
	Token *start = iTk;
	if (exprUnary())
	{
		if (consume(ASSIGN))
		{
			if (exprAssign())
			{
				return true;
			}else tkerr("lipseste expresia dupa = \n");
		}
	}
	iTk = start; //aici am adaugat
	if (exprOr())
	{
		return true;
	}
	iTk = start;
	return false;
}

// exprOr: exprOr OR exprAnd | exprAnd

// exprOr: exprAnd exprOrPrim
bool exprOr()
{
	printf("# exprOr\n");
	Token *start = iTk;
	if (exprAnd())
	{
		if (exprOrPrim())
		{
			return true;
		}
	}
	iTk = start;
	return false;
}
// exprOrPrim : OR exprAnd exprOrPrim
bool exprOrPrim()
{
	printf("# exprOrPrim\n");
	Token *start = iTk;
	if (consume(OR))
	{
		if (exprAnd())
		{
			if (exprOrPrim())
			{
				return true;
			}
		}
		tkerr("sintaxa gresita, lipseste expresie dupa ||\n");
	}
	iTk = start;
	return true;
}

// exprAnd: exprAnd AND exprEq | exprEq
// exprAnd: exprEq exprAndPrim
bool exprAnd()
{
	printf("# exprAnd\n");
	Token *start = iTk;
	if (exprEq())
	{
		if (exprAndPrim())
		{
			return true;
		}
	}
	iTk = start;
	return false;
}
// exprAndPrim: AND exprEq exprAndPrim
bool exprAndPrim()
{
	printf("# exprAndPrim\n");
	Token *start = iTk;
	if (consume(AND))
	{
		if (exprEq())
		{
			if (exprAndPrim())
			{
				return true;
			}
		}
		tkerr("sintaxa gresita, lipseste expresie dupa &&\n");
	}
	iTk = start;
	return true;
}

// exprEq: exprEq ( EQUAL | NOTEQ ) exprRel | exprRel
// exprEq: exprRel exprEqPrim
bool exprEq()
{
	printf("# exprEq\n");
	Token *start = iTk;
	if (exprRel())
	{
		if (exprEqPrim())
		{
			return true;
		}
	}
	iTk = start;
	return false;
}
// exprEqPrim: ( EQUAL | NOTEQ ) exprRel exprEqPrim
bool exprEqPrim()
{
	printf("# exprEqPrim\n");
	Token *start = iTk;
	if (consume(EQUAL) || consume(NOTEQ))
	{
		if (exprRel())
		{
			if (exprEqPrim())
			{
				return true;
			}
		}
		tkerr("sintaxa gresita, lipseste expresie dupa == sau !=\n");
	}
	iTk = start;
	return true;
}

// exprRel: exprRel ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd | exprAdd
// exprRel: exprAdd exprRelPrim
bool exprRel()
{
	printf("# exprRel\n");
	Token *start = iTk;
	if (exprAdd())
	{
		if (exprRelPrim())
		{
			return true;
		}
	}
	iTk = start;
	return false;
}
// exprRelPrim: ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd exprRelPrim
bool exprRelPrim()
{
	printf("# exprRelPrim\n");
	Token *start = iTk;
	if (consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
	{
		if (exprAdd())
		{
			if (exprRelPrim())
			{
				return true;
			}
		}
		tkerr("sitaxa gresita, lipseste expresie dupa > sau < sau >= sau <=\n");
	}
	iTk = start;
	return true;
}

// exprAdd: exprAdd ( ADD | SUB ) exprMul | exprMul

// exprAdd: exprMul exprAddPrim
bool exprAdd()
{
	printf("# exprAdd\n");
	Token *start = iTk;
	if (exprMul())
	{
		if (exprAddPrim())
		{
			return true;
		}
	}
	iTk = start;
	return false;
}
// exprAddPrim: ( ADD | SUB ) exprMul exprAddPrim
bool exprAddPrim()
{
	printf("# exprAddPrim\n");
	Token *start = iTk;
	if (consume(ADD) || consume(SUB))
	{
		if (exprMul())
		{
			if (exprAddPrim())
			{
				return true;
			}
		}
		tkerr("sintaxa gresita, lipseste expresie dupa + sau -\n");
	}
	iTk = start;
	return true;
}

// exprMul: exprMul ( MUL | DIV ) exprCast | exprCast
// exprMul: exprCast exprMulPrim
bool exprMul()
{
	printf("# exprMul\n");
	Token *start = iTk;
	if (exprCast())
	{
		if (exprMulPrim())
		{
			return true;
		}
	}
	iTk = start;
	return false;
}

bool exprMulPrim()
{
	printf("# exprMulPrim\n");
	Token *start = iTk;
	if (consume(MUL) || consume(DIV))
	{
		if (exprCast())
		{
			if (exprMulPrim())
			{
				return true;
			}
		}
		tkerr("sintaxa gresita, lipseste expresie dupa * sau /\n");
	}
	iTk = start;
	return true;
}

// exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary

bool exprCast()
{
	printf("# exprCast\n");
	Token *start = iTk;
	if (consume(LPAR))
	{
		if (typeBase())
		{
			if (arrayDecl())
			{
			}
			if (consume(RPAR))
			{
				if (exprCast())
				{
					return true;
				}
			}else tkerr("lipseste ) dupa tipul de date in care se va face conversia\n");
		}
	}
	if (exprUnary())
	{
		return true;
	}
	iTk = start;
	return false;
}

// exprUnary: ( SUB | NOT ) exprUnary | exprPostfix
bool exprUnary()
{
	printf("# exprUnary\n");
	Token *start = iTk;
	if (consume(SUB) || consume(NOT))
	{
		if (exprUnary())
		{
			return true;
		}else tkerr("expresie invalida, lipseste expresie dupa - / !\n");
	}
	if (exprPostfix())
	{
		return true;
	}
	iTk = start;
	return false;
}

// exprPostfix: exprPrimary exprPostFixPrim
bool exprPostfix()
{
	printf("# exprPostfix\n");
	Token *start = iTk;
	if (exprPrimary())
	{
		if (exprPostFixPrim())
		{
			return true;
		}
	}
	iTk = start;
	return false;
}

// exprPostFixPrim: exprPrimary (DOT ID exprPostFixPrim | LBRACKET expr RBRACKET exprPostFixPrim)

bool exprPostFixPrim()
{
	printf("# exprPostFixPrim\n");
	Token *start = iTk;
	if (consume(LBRACKET))
	{
		if (expr())
		{
			if (consume(RBRACKET))
			{
				if (exprPostFixPrim())
				{
					return true;
				}
			}else tkerr("lipseste ] dupa expresie\n");
		}else tkerr("lipseste expresie dupa [\n");
	}
	iTk = start;
	if (consume(DOT))
	{
		if (consume(ID))
		{
			if (exprPostFixPrim())
			{
				return true;
			}
		}else tkerr("lipseste un nume variabila dupa .\n");
	}

	iTk = start;
	return true;
}

// exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
// | INT | DOUBLE | CHAR | STRING | LPAR expr RPAR
bool exprPrimary()
{
	printf("# exprPrimary\n");
	Token *start = iTk;
	if (consume(ID))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				for (;;)
				{
					if (consume(COMMA))
					{
						if (expr())
						{
						}else tkerr("lipseste expresie dupa ,\n");
					}
					else
					{
						break;
					}
				}
			}
			if (consume(RPAR))
			{
				return true;
			} else tkerr("sintaxa incorecta sau lipseste ) dupa (\n");
		}
		return true;
	}
	iTk = start;
    if (consume(INT))
    {
        return true;
    }
    if (consume(DOUBLE))
    {
        return true;
    }
    if (consume(CHAR))
    {
        return true;
    }
    if (consume(STRING))
    {
        return true;
    }
	iTk = start;
	if (consume(LPAR))
	{
		if (expr())
		{
			if (consume(RPAR))
			{
				return true;
			}else tkerr("lipseste ) dupa expresie\n");
		}else tkerr("eroare de sintaxa, lipseste expresie dupa (\n");
	}
	iTk = start;
	return false;
}

void parse(Token *tokens)
{
	iTk = tokens;
	if (!unit())
		tkerr("syntax error");
}

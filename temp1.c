#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"
#include "utils.h"

Token *iTk;		   // the iterator in the tokens list
Token *consumedTk; // the last consumed token

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

// bool consume(int code)
// {
// 	if (iTk->code == code)
// 	{
// 		consumedTk = iTk;
// 		iTk = iTk->next;
// 		return true;
// 	}
// 	return false;
// }

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
bool typeBase()
{
	if (consume(TYPE_INT))
	{
		return true;
	}
	if (consume(TYPE_DOUBLE))
	{
		return true;
	}
	if (consume(TYPE_CHAR))
	{
		return true;
	}
	if (consume(STRUCT))
	{
		if (consume(ID))
		{
			return true;
		}
	}
	return false;
}

// unit: ( structDef | fnDef | varDef )* END
bool unit()
{
	for (;;)
	{
		if (structDef())
		{
			printf("structDef\n");
		}
		else if (fnDef())
		{
			printf("fnDef\n");
		}
		else if (varDef())
		{
			printf("varDef\n");
		}
		else
			break;
	}
	if (consume(END))
	{
		printf("unit\n");
		return true;
	}
	printf("unit UNIT UNIT UNTI\n");
	return false;
}

// structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef()
{
	Token *start = iTk;
	if (consume(STRUCT))
	{
		if (consume(ID))
		{
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
					if (consume(SEMICOLON))
					{
						return true;
					}
				}
			}
		}
	}
	iTk = start;
	return false;
}

// varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef()
{
	Token *start = iTk;
	if (typeBase())
	{
		if (consume(ID))
		{
			if (arrayDecl())
			{
			}
			if (consume(SEMICOLON))
			{
				return true;
			}
		}
	}
	iTk = start;
	return false;
}

// arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl()
{
	Token *start = iTk;
	// printf("arrayDecl\n");
	if (consume(LBRACKET))
	{
		if (consume(INT))
		{
		}
		if (consume(RBRACKET))
		{
			printf("LAST TEST\n");
			return true;
		}
	}
	iTk = start;
	return false;
}

// fnDef: ( typeBase | VOID ) ID
// LPAR ( fnParam ( COMMA fnParam )* )? RPAR
// stmCompound
bool fnDef()
{
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
							}
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
					}
				}
			}
		}
	}
	iTk = start;
	return false;
}

// fnParam: typeBase ID arrayDecl?
bool fnParam()
{
	Token *start = iTk;
	if (typeBase())
	{
		if (consume(ID))
		{
			if (arrayDecl())
			{
			}
			return true;
		}
		
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
	Token *start = iTk;
	if (stmCompound())
	{
		return true;
	}
	else if (consume(IF))
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
							}
						}
					}
					return true;
				}
			}
		}
	}
	else if (consume(WHILE))
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
					}
				}
			}
		}
	}
	else if (consume(RETURN))
	{
		if (expr())
		{
		}
		if (consume(SEMICOLON))
		{
			return true;
		}
	}
	else
	{
		if (expr())
		{
		}
		if (consume(SEMICOLON))
		{
			return true;
		}
	}
	iTk = start;
	return false;
}

// stmCompound: LACC ( varDef | stm )* RACC
bool stmCompound()
{
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
		}
	}
	iTk = start;
	return false;
}

// expr: exprAssign
bool expr()
{
	printf("expr\n");
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
	printf("exprAssign\n");
	Token *start = iTk;
	if (exprUnary())
	{
		if (consume(ASSIGN))
		{
			if (exprAssign())
			{
				return true;
			}
		}
	}
	else if (exprOr())
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
	printf("exprOr\n");
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
	printf("exprOrPrim\n");
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
	}
	iTk = start;
	return true;
}

// exprAnd: exprAnd AND exprEq | exprEq
// exprAnd: exprEq exprAndPrim
bool exprAnd()
{
	printf("exprAnd\n");
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
	printf("exprAndPrim\n");
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
	}
	iTk = start;
	return true;
}

// exprEq: exprEq ( EQUAL | NOTEQ ) exprRel | exprRel
// exprEq: exprRel exprEqPrim
bool exprEq()
{
	printf("exprEq\n");
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
	printf("exprEqPrim\n");
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
	}
	iTk = start;
	return true;
}

// exprRel: exprRel ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd | exprAdd
// exprRel: exprAdd exprRelPrim
bool exprRel()
{
	printf("exprRel\n");
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
	}
	iTk = start;
	return true;
}

// exprAdd: exprAdd ( ADD | SUB ) exprMul | exprMul

// exprAdd: exprMul exprAddPrim
bool exprAdd()
{
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
	}
	iTk = start;
	return true;
}

// exprMul: exprMul ( MUL | DIV ) exprCast | exprCast
// exprMul: exprCast exprMulPrim
bool exprMul()
{
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
	}
	iTk = start;
	return true;
}

// exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary

bool exprCast()
{
	printf("exprCast\n");
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
			}
		}
	}
	else if (exprUnary())
	{
		return true;
	}
	iTk = start;
	return false;
}

// exprUnary: ( SUB | NOT ) exprUnary | exprPostfix
bool exprUnary()
{
	printf("exprUnary\n");
	Token *start = iTk;
	if (consume(SUB) || consume(NOT))
	{
		if (exprUnary())
		{
			return true;
		}
	}
	else if (exprPostfix())
	{
		return true;
	}
	iTk = start;
	return false;
}

// exprPostfix: exprPrimary exprPostFixPrim
bool exprPostfix()
{
	printf("exprPostfix\n");
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
	printf("exprPostFixPrim\n");
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
			}
		}
	}
	else if (consume(DOT))
	{
		if (consume(ID))
		{
			if (exprPostFixPrim())
			{
				return true;
			}
		}
	}

	iTk = start;
	return true;
}

// exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
// | INT | DOUBLE | CHAR | STRING | LPAR expr RPAR
bool exprPrimary()
{
	printf("exprPrimary\n");
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
						}
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
			}
		}
		return true;
	}
	else if(consume(INT) || consume(DOUBLE) || consume(CHAR) || consume(STRING))
	{
		return true;
	}
	else if (consume(LPAR))
	{
		if (expr())
		{
			if (consume(RPAR))
			{
				return true;
			}
		}
	}
	iTk = start;
	printf("A RETURNAT FALSE\n");
	return false;
}

void parse(Token *tokens)
{
	iTk = tokens;
	if (!unit())
		tkerr("syntax error");
}

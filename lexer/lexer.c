#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "utils.h"

Token *tokens; // single linked list of tokens
Token *lastTk; // the last token in list

int line = 1; // the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code)
{
	Token *tk = safeAlloc(sizeof(Token));
	tk->code = code;
	tk->line = line;
	tk->next = NULL;
	if (lastTk)
	{
		lastTk->next = tk;
	}
	else
	{
		tokens = tk;
	}
	lastTk = tk;
	return tk;
}

char *extract(const char *begin, const char *end)
{
	int size = end - begin + 1;
	char *text = safeAlloc(size);
	strncpy(text, begin, size - 1);
	text[size - 1] = '\0';
	return text;
}

Token *tokenize(const char *pch)
{
	const char *start;
	Token *tk;
	for (;;)
	{
		switch (*pch)
		{
		case ' ':
		case '\t':
			pch++;
			break;
		case '\r': // handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
			if (pch[1] == '\n')
				pch++;
			// fallthrough to \n
		case '\n':
			line++;
			pch++;
			break;

		case '\0':
			addTk(END);
			return tokens;

		case ',':
			addTk(COMMA);
			pch++;
			break;

		case '+':
			addTk(ADD);
			pch++;
			break;
		case '-':
			addTk(SUB);
			pch++;
			break;

		case '*':
			addTk(MUL);
			pch++;
			break;

		case '/':
			if(pch[1] == '/')
			{
				start = pch;
				while(*pch != '\0' && *pch != '\r' && *pch != '\n' && *pch != '\t')
				{
					pch++;
				}
				break;
			}else{
				addTk(DIV);
				pch++;
				break;
			}

		case '.':
			addTk(DOT);
			pch++;
			break;

		case '{':
			addTk(LACC);
			pch++;
			break;

		case '}':
			addTk(RACC);
			pch++;
			break;

		case '[':
			addTk(LBRACKET);
			pch++;
			break;

		case ']':
			addTk(RBRACKET);
			pch++;
			break;

		case '(':
			addTk(LPAR);
			pch++;
			break;

		case ')':
			addTk(RPAR);
			pch++;
			break;

		case ';':
			addTk(SEMICOLON);
			pch++;
			break;

		case '&':
			if (pch[1] == '&')
			{
				addTk(AND);
				pch += 2;
			}
			else
			{
				err("Error, invalid AND operand, missing &");
				pch++;
			}
			break;

		case '|':
			if (pch[1] == '|')
			{
				addTk(OR);
				pch += 2;
			}
			else
			{
				err("Error, invalid OR operand, missing |");
				pch++;
			}
			break;

		case '=':
			if (pch[1] == '=')
			{
				addTk(EQUAL);
				pch += 2;
			}
			else
			{
				addTk(ASSIGN);
				pch++;
			}
			break;

		case '!':
			if (pch[1] == '=')
			{
				addTk(NOTEQ);
				pch += 2;
			}
			else
			{
				addTk(NOT);
				pch++;
			}
			break;

		case '<':
			if (pch[1] == '=')
			{
				addTk(LESSEQ);
				pch += 2;
			}
			else
			{
				addTk(LESS);
				pch++;
			}
			break;

		case '>':
			if (pch[1] == '=')
			{
				addTk(GREATEREQ);
				pch += 2;
			}
			else
			{
				addTk(GREATER);
				pch++;
			}
			break;

		default:
			if (isalpha(*pch) || *pch == '_') /* verificare ID si variable_name */
			{
				for (start = pch++; isalnum(*pch) || *pch == '_'; pch++)
				{
				}
				/* start = aici am facut; pch = am facut             */
				char *text = extract(start, pch); /* pch e textul taiat */


				if (strcmp(text, "char") == 0)
					{addTk(TYPE_CHAR); break;}
				if (strcmp(text, "double") == 0)
					{addTk(TYPE_DOUBLE); break;}
				if (strcmp(text, "else") == 0)
					{addTk(ELSE); break;}
				if (strcmp(text, "if") == 0)
					{addTk(IF); break;}
				if (strcmp(text, "int") == 0)
					{addTk(TYPE_INT); break;}
				if (strcmp(text, "return") == 0)
					{addTk(RETURN); break;}
				if (strcmp(text, "struct") == 0)
					{addTk(STRUCT); break;}
				if (strcmp(text, "void") == 0)
					{addTk(VOID); break;}
				if (strcmp(text, "while") == 0)
					{addTk(WHILE); break;}	
				else
				{
					tk = addTk(ID);
					tk->text = text;
					break;
				}
				
			}
			if(*pch == '\'') /* verificare caracter */
			{
				pch++;
				if(pch[1] != '\'')
				{
					err("Not a correct character!\n");
				}
				tk = addTk(CHAR);
				tk -> c = pch[0];
				pch += 2;
				break;

			}

			if(*pch == '"') 
			{
				/* verificare string atata timp -> 
				parsare pana cand dam de pch = " sau pch = terminator de sir
				*/
				start = ++pch; // uitasem ++ la pch :)
				//printf("AFist: %c\n", pch[0]);
				while(*pch != '"' && *pch != '\0')
				{
					if(*pch == '\n')
					{
						line++;
					}
					pch++;
				}

				

				if(*pch == '\0')
				{
					err("Invalid string, terminator not found!");
				}
				
				//printf("%s", start);
				char *text = extract(start, pch);
				tk = addTk(STRING);
				tk -> text = text;
				pch++;
				
				break;
			}
			if(isdigit(*pch))
			{
				start = pch++;
				while(isdigit(*pch)) /* parse number */
				{
					pch++;
				}
				if(*pch == '.') /*verificam daca am dat de un punct*/
				{
					/* verificam corectitudinea numerelor */
					pch++;
					if(!isdigit(*pch))
					{
						err("Invalid double, missing number after . !");
					}
					while(isdigit(*pch)) // while folosit pentru a parsa peste eventualele cifre
					{
						pch++;
					}

					if(*pch == 'e' || *pch == 'E')
					{
						pch++;
						if(*pch == '-' || *pch == '+')
						{
							pch++;
						}

						if(!isdigit(*pch))
						{
							err("Invalid double, missing number after expr !");
						}

						while(isdigit(*pch)) // while folosit pentru a parsa peste eventualele cifre
						{
							pch++;
						}
					}

					char *text = extract(start, pch);
					tk = addTk(DOUBLE);
					tk -> d = atof(text);
					break;				
				}
				if(*pch == 'e' || *pch == 'E') // verificare al doilea caz cand primul caracter poate fi e sau E
				{
					pch++;
					if(*pch == '-' || *pch == '+')
					{
						pch++;
					}

					if(!isdigit(*pch))
					{
						err("Invalid double, missing number after +\\-");
					}

					while(isdigit(*pch))
					{
						pch++;
					}

					char *text = extract(start, pch);
					//printf("DADADADADAADADADA\n\n\n");
					tk = addTk(DOUBLE);
					tk -> d = atof(text);
					break;
				}else
				{
					char *text = extract(start, pch);
					tk = addTk(INT);
					tk -> i = atoi(text);
					break;
				}
			}else
				{
					err("invalid char: %c (%d)", *pch, *pch);
				}
		}
	}
}

void showTokens(const Token *tokens)
{
    FILE *file = fopen("tokenized_code.txt", "w");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    for (const Token *tk = tokens; tk; tk = tk->next)
    {
        printf("%d ", tk->line);
        fprintf(file, "%d ", tk->line);
        
        switch (tk->code)
        {
        case ID:
            printf("ID:%s\n", tk->text);
            fprintf(file, "ID:%s\n", tk->text);
            break;
        case TYPE_CHAR:
            printf("TYPE_CHAR\n");
            fprintf(file, "TYPE_CHAR\n");
            break;
        case TYPE_DOUBLE:
            printf("TYPE_DOUBLE\n");
            fprintf(file, "TYPE_DOUBLE\n");
            break;
        case ELSE:
            printf("ELSE\n");
            fprintf(file, "ELSE\n");
            break;
        case IF:
            printf("IF\n");
            fprintf(file, "IF\n");
            break;
        case TYPE_INT:
            printf("TYPE_INT\n");
            fprintf(file, "TYPE_INT\n");
            break;
        case RETURN:
            printf("RETURN\n");
            fprintf(file, "RETURN\n");
            break;
        case STRUCT:
            printf("STRUCT\n");
            fprintf(file, "STRUCT\n");
            break;
        case VOID:
            printf("VOID\n");
            fprintf(file, "VOID\n");
            break;
        case WHILE:
            printf("WHILE\n");
            fprintf(file, "WHILE\n");
            break;
        case INT:
            printf("INT:%d\n", tk->i);
            fprintf(file, "INT:%d\n", tk->i);
            break;
        case DOUBLE:
            printf("DOUBLE:%g\n", tk->d);
            fprintf(file, "DOUBLE:%g\n", tk->d);
            break;
        case CHAR:
            printf("CHAR:%c\n", tk->c);
            fprintf(file, "CHAR:%c\n", tk->c);
            break;
        case STRING:
            printf("STRING:%s\n", tk->text);
            fprintf(file, "STRING:%s\n", tk->text);
            break;
        case COMMA:
            printf("COMMA\n");
            fprintf(file, "COMMA\n");
            break;
        case SEMICOLON:
            printf("SEMICOLON\n");
            fprintf(file, "SEMICOLON\n");
            break;
        case LPAR:
            printf("LPAR\n");
            fprintf(file, "LPAR\n");
            break;
        case RPAR:
            printf("RPAR\n");
            fprintf(file, "RPAR\n");
            break;
        case LBRACKET:
            printf("LBRACKET\n");
            fprintf(file, "LBRACKET\n");
            break;
        case RBRACKET:
            printf("RBRACKET\n");
            fprintf(file, "RBRACKET\n");
            break;
        case LACC:
            printf("LACC\n");
            fprintf(file, "LACC\n");
            break;
        case RACC:
            printf("RACC\n");
            fprintf(file, "RACC\n");
            break;
        case END:
            printf("END\n");
            fprintf(file, "END\n");
            break;
        case ADD:
            printf("ADD\n");
            fprintf(file, "ADD\n");
            break;
        case SUB:
            printf("SUB\n");
            fprintf(file, "SUB\n");
            break;
        case MUL:
            printf("MUL\n");
            fprintf(file, "MUL\n");
            break;
        case DIV:
            printf("DIV\n");
            fprintf(file, "DIV\n");
            break;
        case DOT:
            printf("DOT\n");
            fprintf(file, "DOT\n");
            break;
        case AND:
            printf("AND\n");
            fprintf(file, "AND\n");
            break;
        case OR:
            printf("OR\n");
            fprintf(file, "OR\n");
            break;
        case NOT:
            printf("NOT\n");
            fprintf(file, "NOT\n");
            break;
        case ASSIGN:
            printf("ASSIGN\n");
            fprintf(file, "ASSIGN\n");
            break;
        case EQUAL:
            printf("EQUAL\n");
            fprintf(file, "EQUAL\n");
            break;
        case NOTEQ:
            printf("NOTEQ\n");
            fprintf(file, "NOTEQ\n");
            break;
        case LESS:
            printf("LESS\n");
            fprintf(file, "LESS\n");
            break;
        case LESSEQ:
            printf("LESSEQ\n");
            fprintf(file, "LESSEQ\n");
            break;
        case GREATER:
            printf("GREATER\n");
            fprintf(file, "GREATER\n");
            break;
        case GREATEREQ:
            printf("GREATEREQ\n");
            fprintf(file, "GREATEREQ\n");
            break;
        default:
            err("invalid code for token");
            fprintf(file, "invalid code for token\n");
            break;
        }
    }
    fclose(file);
}


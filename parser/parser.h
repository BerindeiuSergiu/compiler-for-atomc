#include <stdbool.h>
#include "lexer.h"

bool unit();
bool structDef();
bool varDef();
bool typeBase();
bool arrayDecl(Type *t);
bool fnDef();
bool fnParam();
bool stm();
bool stmCompound();
bool expr();
bool exprAssign();
bool exprOr();
bool exprAnd();
bool exprEq();
bool exprRel();
bool exprAdd();
bool exprMul();
bool exprCast();
bool exprUnary();
bool exprPostfix();
bool exprPrimary();

bool exprOrPrim();
bool exprAndPrim();
bool exprEqPrim();
bool exprRelPrim();
bool exprAddPrim();
bool exprMulPrim();
bool exprPostFixPrim();


void parse(Token *tokens);


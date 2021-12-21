/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode* stmt_sequence(void);
static TreeNode* statement(void);
static TreeNode* if_stmt(void);
static TreeNode* repeat_stmt(void);
static TreeNode* doWhile_stmt(void);  // do while循环
static TreeNode* for_stmt(void);  // for循环
static TreeNode* to_stmt(void);  // to/downto
static TreeNode* assign_stmt(void);
static TreeNode* read_stmt(void);
static TreeNode* write_stmt(void);
static TreeNode* exp(void);
static TreeNode* exp2(void);  // 实现逻辑表达式and, or
static TreeNode* minunseq_exp(char*);  // -=运算
static TreeNode* simple_exp(void);
static TreeNode* term(void);
static TreeNode* term2(void);  // 乘方运算
static TreeNode* factor(void);

static void syntaxError(string message)
{
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message.c_str());
    Error = TRUE;
}

static void match(TokenType expected)
{
    if(token == expected) {
        token = getToken();
    } else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        fprintf(listing, "      ");
    }
}

TreeNode* stmt_sequence(void)
{
    TreeNode* t = statement();
    TreeNode* p = t;
    while((token != ENDFILE) && (token != END) &&
          (token != ELSE) && (token != UNTIL) &&
          (token != WHILE) && (token != ENDDO)) {
        TreeNode* q;
        match(SEMI);
        q = statement();
        if(q != NULL) {
            if(t == NULL) {
                t = p = q;
            } else { /* now p cannot be NULL either */
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

TreeNode* statement(void)
{
    TreeNode* t = NULL;
    switch(token) {
        case IF :
            t = if_stmt();
            break;
        case REPEAT :
            t = repeat_stmt();
            break;
        case ID :
            t = assign_stmt();
            break;
        case READ :
            t = read_stmt();
            break;
        case WRITE :
            t = write_stmt();
            break;
        case DO:
            t = doWhile_stmt();
            break;
        case FOR:
            t = for_stmt();
            break;
        default :
            syntaxError("unexpected token -> ");
            printToken(token, tokenString);
            token = getToken();
            break;
    } /* end case */
    return t;
}

TreeNode* if_stmt(void)
{
    TreeNode* t = newStmtNode(IfK);
    match(IF);
    match(LPAREN);
    if(t != NULL) {
        t->child[0] = exp2();
    }
    match(RPAREN);
    //    match(THEN);
    if(t != NULL) {
        t->child[1] = stmt_sequence();
    }
    if(token == ELSE) {
        match(ELSE);
        if(t != NULL) {
            t->child[2] = stmt_sequence();
        }
    }
    match(END);
    return t;
}

// 实现do while循环
treeNode* doWhile_stmt(void)
{
    TreeNode* t = newStmtNode(DoWhileK);
    match(DO);
    if(t != NULL) {
        t->child[0] = stmt_sequence();
    }
    match(SEMI);
    match(WHILE);
    match(LPAREN);
    if(t != NULL) {
        t->child[1] = exp2();
    }
    match(RPAREN);
    return t;
}

// 实现for循环
TreeNode* for_stmt(void)
{
    TreeNode* t = newStmtNode(ForK);
    match(FOR);
    if(t != NULL) {
        t->child[0] = assign_stmt();
    }
    if(t != NULL) {
        t->child[1] = to_stmt();
    }
    match(DO);
    if(t != NULL) {
        t->child[2] = stmt_sequence();
    }
    match(ENDDO);
    return t;
}
TreeNode* to_stmt(void)
{
    TreeNode* t = NULL;
    if(token == TO) {
        t = newStmtNode(ToK);
        match(TO);
    } else if(token == DOWNTO) {
        t = newStmtNode(DowntoK);
        match(DOWNTO);
    }
    if(t != NULL) {
        t->child[0] = simple_exp();
    }
    return t;
}

TreeNode* repeat_stmt(void)
{
    TreeNode* t = newStmtNode(RepeatK);
    match(REPEAT);
    if(t != NULL) {
        t->child[0] = stmt_sequence();
    }
    match(UNTIL);
    if(t != NULL) {
        t->child[1] = exp2();
    }
    return t;
}

TreeNode* assign_stmt(void)
{
    TreeNode* t = newStmtNode(AssignK);
    char* varname = NULL;
    if((t != NULL) && (token == ID)) {
        t->attr.name = copyString(tokenString);
        varname = t->attr.name;
    }
    match(ID);
    if(token == ASSIGN) {
        match(ASSIGN);
        if(t != NULL) {
            t->child[0] = exp2();
        }
    } else if(token == MINUSEQ) {
        match(MINUSEQ);
        if(t != NULL) {
            t->child[0] = minunseq_exp(varname);
        }
    }
    if(token == SEMI) {
        match(SEMI);
    }
    return t;
}

TreeNode* read_stmt(void)
{
    TreeNode* t = newStmtNode(ReadK);
    match(READ);
    if((t != NULL) && (token == ID)) {
        t->attr.name = copyString(tokenString);
    }
    match(ID);
    return t;
}

TreeNode* write_stmt(void)
{
    TreeNode* t = newStmtNode(WriteK);
    match(WRITE);
    if(t != NULL) {
        t->child[0] = exp2();
    }
    return t;
}

TreeNode* exp(void)
{
    bool hasNot = false;
    if(token == NOT) {
        match(token);
        hasNot = true;
    }

    TreeNode* t = simple_exp();
    if(token == LT || token == LTE
       || token == GT || token == GTE
       || token == EQ || token == NE) {
        TreeNode* p = newExpNode(OpK);
        if(p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
        }
        match(token);
        if(t != NULL) {
            t->child[1] = simple_exp();
        }
    }
    // 实现not
    if(hasNot) {
        switch(t->attr.op) {
            case LT:
                t->attr.op = GTE;
                break;
            case LTE:
                t->attr.op = GT;
                break;
            case GT:
                t->attr.op = LTE;
                break;
            case GTE:
                t->attr.op = LT;
                break;
            case EQ:
                t->attr.op = NE;
                break;
            case NE:
                t->attr.op = EQ;
                break;
            default:
                break;
        }
    }
    return t;
}

// 实现逻辑表达式and, or
TreeNode* exp2(void)
{
    TreeNode* t = exp();
    while(token == AND || token == OR) {
        TreeNode* p = NULL;
        if(token == AND) {
            p = newStmtNode(AndK);
        } else if(token == OR) {
            p = newStmtNode(OrK);
        }
        match(token);
        if(p != NULL) {
            p->child[0] = t;
            p->child[1] = exp();
            t = p;
        }
    }
    return t;
}

// 实现-=赋值号
TreeNode* minunseq_exp(char* varname)
{
    TreeNode* t = newExpNode(OpK);
    TreeNode* p = newExpNode(IdK);
    if(t != NULL && p != NULL) {
        p->attr.name = varname;
        t->child[0] = p;
        t->child[1] = simple_exp();
        t->attr.op = MINUS;
    }
    return t;
}

TreeNode* simple_exp(void)
{
    TreeNode* t = term();
    while(token == PLUS || token == MINUS
          || token == LINK || token == LOR) { // 连接&和或|
        TreeNode* p = NULL;
        if(token == LINK || token == LOR) {
            p = newExpNode(LopK);
        } else {
            p = newExpNode(OpK);
        }
        if(p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = term();
        }
    }
    return t;
}

TreeNode* term(void)
{
    TreeNode* t = term2();
    while(token == TIMES || token == OVER || token == MOD) {
        TreeNode* p = newExpNode(OpK);
        if(p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = term2();
        }
    }
    while(token == CLOSURE) { // 闭包#
        TreeNode* p = newExpNode(LopK);
        if(p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
        }
    }
    return t;
}

// 乘方运算，优先级最高
TreeNode* term2(void)
{
    TreeNode* t = factor();
    while(token == POWER) {
        TreeNode* p = newExpNode(OpK);
        if(p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = factor();
        }
    }
    return t;
}

TreeNode* factor(void)
{
    TreeNode* t = NULL;
    switch(token) {
        case NUM :
            t = newExpNode(ConstK);
            if((t != NULL) && (token == NUM)) {
                t->attr.val = atoi(tokenString);
            }
            match(NUM);
            break;
        case ID :
            t = newExpNode(IdK);
            if((t != NULL) && (token == ID)) {
                t->attr.name = copyString(tokenString);
            }
            match(ID);
            break;
        case LPAREN :
            match(LPAREN);
            t = exp2();
            match(RPAREN);
            break;
        default:
            syntaxError("unexpected token -> ");
            printToken(token, tokenString);
            token = getToken();
            break;
    }
    return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode* parse(void)
{
    TreeNode* t;
    token = getToken();
    t = stmt_sequence();
    if(token != ENDFILE) {
        syntaxError("Code ends before file\n");
    }
    return t;
}

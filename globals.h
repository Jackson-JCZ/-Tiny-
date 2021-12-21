/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef TRUE
    #define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 17

typedef enum
/* book-keeping tokens */
{
    ENDFILE, ERROR,
    /* reserved words */
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE, WHILE, DO, FOR, ENDDO, TO, DOWNTO,
    AND, OR, NOT,
    /* multicharacter tokens */
    ID, NUM,
    /* special symbols */
    ASSIGN, EQ, LT, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI, MINUSEQ, MOD, POWER,
    LTE, GT, GTE, NE, LINK, LOR, CLOSURE
} TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum { StmtK, ExpK } NodeKind;
typedef enum { IfK, RepeatK, AssignK, ReadK, WriteK,
               DoWhileK, ForK, ToK, DowntoK,
               AndK, OrK
             } StmtKind;  // 语句
typedef enum { OpK, ConstK, IdK, LopK } ExpKind;

/* ExpType is used for type checking */
//typedef enum { Void, Integer, Boolean } ExpType;

#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode* child[MAXCHILDREN];  // 最多有3个孩子
    struct treeNode* sibling;
    NodeKind nodekind;
    union {
        StmtKind stmt;
        ExpKind exp;
    } kind;
    union {
        TokenType op;
        int val;
        char* name;
    } attr;
    //    ExpType type; /* for type checking of exps */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;
#endif

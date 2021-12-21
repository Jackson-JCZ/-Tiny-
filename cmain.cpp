/****************************************************/
/* File: cmain.cpp                                   */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "parse.h"

/* allocate global variables */
int lineno = 0;
FILE* source;
FILE* listing;

extern int linepos;
extern int bufsize;
extern int EOF_flag;

/* allocate and set tracing flags */
int Error = FALSE;

string fun(char* filename)
{
    string s = "";
    TreeNode* syntaxTree;
    char pgm[120]; /* source code file name */
    strcpy_s(pgm, 120, filename);
    if(strchr(pgm, '.') == NULL) {
        strcat_s(pgm, ".tny");
    }
    source = fopen(pgm, "r");
    listing = stdout; /* send listing to screen */

    syntaxTree = parse();
    s += printTree(syntaxTree, s, 0);

    fclose(source);

    delete source;
    delete listing;
    lineno = 0;
    linepos = 0;
    bufsize = 0;
    Error = FALSE;
    EOF_flag = FALSE;
    return s;
}

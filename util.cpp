/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
string printToken(TokenType token, string tokenString)
{
    string s = "";
    switch(token) {
        case IF:
        case THEN:
        case ELSE:
        case END:
        case REPEAT:
        case UNTIL:
        case READ:
        case WRITE:
        case WHILE:
        case DO:
        case FOR:
        case ENDDO:
        case TO:
        case DOWNTO:
        case AND:
        case OR:
        case NOT:
            s += "reserved word: " + tokenString;
            break;
        case ASSIGN:
            s += '=';
            break;
        case LT:
            s += '<';
            break;
        case LTE:
            s += "<=";
            break;
        case GT:
            s += '>';
            break;
        case GTE:
            s += ">=";
            break;
        case EQ:
            s += "==";
            break;
        case NE:
            s += "<>";
            break;
        case LPAREN:
            s += '(';
            break;
        case RPAREN:
            s += ')';
            break;
        case SEMI:
            s += ';';
            break;
        case PLUS:
            s += '+';
            break;
        case MINUS:
            s += '-';
            break;
        case TIMES:
            s += '*';
            break;
        case MOD:
            s += '%';
            break;
        case POWER:
            s += '^';
            break;
        case LINK:
            s += '&';
            break;
        case LOR:
            s += '|';
            break;
        case CLOSURE:
            s += '#';
            break;
        case OVER:
            s += '/';
            break;
        case ENDFILE:
            s += "EOF";
            break;
        case NUM:
            s += "NUM, val= " + tokenString;
            break;
        case ID:
            s += "ID, name= " + tokenString;
            break;
        case ERROR:
            s += "ERROR: " + tokenString;
            break;
        default: /* should never happen */
            s += "Unknown token: " + to_string(token);
    }
    s += "\n";
    return s;
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode* newStmtNode(StmtKind kind)
{
    TreeNode* t = (TreeNode*) malloc(sizeof(TreeNode));
    int i;
    if(t == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    } else {
        for(i = 0; i < MAXCHILDREN; i++) {
            t->child[i] = NULL;
        }
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
    }
    return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode* newExpNode(ExpKind kind)
{
    TreeNode* t = (TreeNode*) malloc(sizeof(TreeNode));
    int i;
    if(t == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    } else {
        for(i = 0; i < MAXCHILDREN; i++) {
            t->child[i] = NULL;
        }
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
    }
    return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char* copyString(char* s)
{
    int n;
    char* t;
    if(s == NULL) {
        return NULL;
    }
    n = strlen(s) + 1;
    t = new char[n];
    if(t == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    } else {
        strcpy_s(t, n, s);
    }
    return t;
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
string printTree(TreeNode* tree, string s, int indentCount)
{
    while(tree != NULL) {
        for(int i = 0; i < indentCount; i++) {
            s += "  ";
        }
        if(tree->nodekind == StmtK) {
            switch(tree->kind.stmt) {
                case IfK:
                    s += "If";
                    break;
                case RepeatK:
                    s += "Repeat";
                    break;
                case AssignK: {
                    s += "Assign to: ";
                    s += tree->attr.name;
                    break;
                }
                case ReadK: {
                    s += "Read: ";
                    s += tree->attr.name;
                    break;
                }
                case WriteK:
                    s += "Write";
                    break;
                case DoWhileK:
                    s += "Do";
                    break;
                case ForK:
                    s += "For";
                    break;
                case DowntoK:
                    s += "downto";
                    break;
                case ToK:
                    s += "to";
                    break;
                case AndK:
                    s += "And";
                    break;
                case OrK:
                    s += "Or";
                    break;
                default:
                    s += "Unknown ExpNode kind";
                    break;
            }
            s += "\n";
        } else if(tree->nodekind == ExpK) {
            switch(tree->kind.exp) {
                case OpK:
                    s += "Op: ";
                    s += printToken(tree->attr.op, "\0");
                    break;
                case ConstK:
                    s += "Const: " + to_string(tree->attr.val) + "\n";
                    break;
                case IdK:
                    s += "Id: ";
                    s += tree->attr.name;
                    s += "\n";
                    break;
                case LopK:
                    s += "Lop: ";
                    s += printToken(tree->attr.op, "\0");
                    break;
                default:
                    s += "Unknown ExpNode kind\n";
                    break;
            }
        } else {
            s += "Unknown node kind\n";
        }
        for(int i = 0; i < MAXCHILDREN; i++) {
            s = printTree(tree->child[i], s, indentCount + 1);
        }
        tree = tree->sibling;
    }
    return s;
}

/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "semantics.h"
#include "error.h"
#include <stdio.h>
#include "token.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
  // TODO
}

void checkFreshIdent(char *name) {
    ObjectNode *list = symtab->currentScope->objList;
    Object *owner = symtab->currentScope->owner;

    // search for object having name in the object list
    while(list != NULL) {
        if (strcmp(name, list->object->name) == 0) {
            // if we find an object having the same name
            error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
        }

        list = list->next;
    }

    // get the params list
    ObjectNode *params = NULL;
    if (owner->kind == OBJ_FUNCTION) {
        params = owner->funcAttrs->paramList;
    } else if (owner->kind == OBJ_PROCEDURE) {
        params = owner->procAttrs->paramList;
    }

    // search for object having the same name in the param list
    while(params != NULL) {
        if (strcmp(name, params->object->name) == 0) {
            // if we find an object having the same name
            error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
        }

        params = params->next;
    }

}


Object* checkDelcaredParam(Scope *scope, char *name) {
    if (scope == NULL || name == NULL) return NULL;
    Object *owner = scope->owner;
    ObjectNode *params = NULL;

    // get param list
    if (owner->kind == OBJ_FUNCTION) {
        params = owner->funcAttrs->paramList;
    } else if (owner->kind == OBJ_PROCEDURE) {
        params = owner->procAttrs->paramList;
    } else {
        return NULL;
    }

    while (params != NULL) {
        if (strcmp(params->object->name, name) == 0) {
            return params->object;
        }

        params = params->next;
    }

    return NULL;
}


Object* checkDeclaredIdent(char* name) {
    Scope *currentScope = symtab->currentScope;

    while(currentScope != NULL) {
        ObjectNode *list = currentScope->objList;
        // search through the object list of current scope
        while(list != NULL) {
            if (strcmp(name, list->object->name) == 0) {
                // if we find an object having the same name
                return list->object;
            }

            // move to the next object
            list = list->next;
        }

        // search for object in the parameter list of current scope
        Object *result = checkDelcaredParam(currentScope, name);
        if (result != NULL) return result;
        // search in outer scope
        currentScope = currentScope->outer;
    }

    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

}

Object* checkDeclaredConstant(char* name) {
    Scope *currentScope = symtab->currentScope;

    while(currentScope != NULL) {
        ObjectNode *list = currentScope->objList;
        // search through the object list of current scope
        while(list != NULL) {
            if (list->object->kind == OBJ_CONSTANT) {
                // if the object is a constant
                if (strcmp(name, list->object->name) == 0) {
                    // if we find an object having the same name
                    return list->object;
                }
            }

            // move to the next object
            list = list->next;
        }

        // search in outer scope
        currentScope = currentScope->outer;
    }

    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

}

Object* checkDeclaredType(char* name) {
    Scope *currentScope = symtab->currentScope;

    while(currentScope != NULL) {
        ObjectNode *list = currentScope->objList;
        // search through the object list of current scope
        while(list != NULL) {
            if (list->object->kind == OBJ_TYPE) {
                // if the object is a constant
                if (strcmp(name, list->object->name) == 0) {
                    // if we find an object having the same name
                    return list->object;
                }
            }

            // move to the next object
            list = list->next;
        }

        // search in outer scope
        currentScope = currentScope->outer;
    }

    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

}

Object* checkDeclaredVariable(char* name) {
    Scope *currentScope = symtab->currentScope;

    while(currentScope != NULL) {
        ObjectNode *list = currentScope->objList;
        // search through the object list of current scope
        while(list != NULL) {
            if (strcmp(name, list->object->name) == 0 && list->object->kind == OBJ_VARIABLE) {
                // if we find an object having the same name
                // and it is a variable
                return list->object;
            }

            // move to the next object
            list = list->next;
        }

        // search in outer scope
        currentScope = currentScope->outer;
    }

    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
}

Object* checkDeclaredFunction(char* name) {
  // TODO
}

/**
 * Check if a function or procedure name is global functions or procedures
 * @param name
 * @return
 */
int isGlobalPFs(char *name) {
    if (name == NULL) return 0;
    int pfNum = 5;
    char globalPFs[5][20] = {"WRITELN", "WRITEI", "WRITEC", "READC", "READI"};
    int i = 0;
    int n = strlen(name);
    char cname[n];

    // convert to upper case
    for (i = 0; i < n; i++) {
        cname[i] = toupper(name[i]);
    }

    // compare the name with all global procedures and functions
    for (i = 0; i < pfNum; i++) {
        if (strcmp(cname, globalPFs[i]) == 0) {
            // printf("call %s\n", globalPFs[i]);
            return 1;
        }
    }

    return 0;
}

Object* checkDeclaredProcedure(char* name) {
    if (isGlobalPFs(name)) {
        // if it is a global function
        // then it passes
        return NULL;
    }

    Scope *currentScope = symtab->currentScope;

    while(currentScope != NULL) {
        ObjectNode *list = currentScope->objList;
        // search through the object list of current scope
        while(list != NULL) {
            if (list->object->kind == OBJ_PROCEDURE) {
                // if the object is a constant
                if (strcmp(name, list->object->name) == 0) {
                    // if we find an object having the same name
                    return list->object;
                }
            }

            // move to the next object
            list = list->next;
        }

        // search in outer scope
        currentScope = currentScope->outer;
    }

    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);

}

Object* checkDeclaredLValueIdent(char* name) {
    Object *ident = checkDeclaredIdent(name);

    if (ident->kind == OBJ_FUNCTION || ident->kind == OBJ_VARIABLE || ident->kind == OBJ_PARAMETER) {
        return ident;
    } else {
        return NULL;
    }
}


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

#define PROCEDURE_SUBROUTINE 1
#define FUNCTION_SUBROUTINE 2

extern SymTab* symtab;
extern Token* currentToken;

char globalPs[3][20] = {"WRITELN", "WRITEI", "WRITEC"};
char globalFs[2][20] = {"READC", "READI"};


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


Object* lookupObject(char *name) {
    if (name == NULL) return NULL;
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

    return NULL;
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


Object* checkDeclaredIdent(char* name) {
    Object *found = lookupObject(name);

    if(found) return found;

    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

}

Object* checkDeclaredConstant(char* name) {
    Object *found = lookupObject(name);

    if(found){
        if (found->kind == OBJ_CONSTANT)
            return found;
    }

    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

}

Object* checkDeclaredType(char* name) {
    Object *found = lookupObject(name);

    if(found) {
        if (found->kind == OBJ_TYPE) {
            return found;
        }
    }

    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

}

Object* checkDeclaredVariable(char* name) {
    Object *found = lookupObject(name);

    if(found) {
        if (found->kind == OBJ_VARIABLE)
            return found;
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
int isGlobalPs(char *name) {
    if (name == NULL) return 0;
    char globalPs[3][20] = {"WRITELN", "WRITEI", "WRITEC"};
    int i = 0;
    int n = strlen(name);
    char cname[n];

    // convert to upper case
    for (i = 0; i < n; i++) {
        cname[i] = toupper(name[i]);
    }
    cname[i] = '\0';
    // compare the name with all global procedures and functions
    for (i = 0; i < 3; i++) {
        if (strcmp(cname, globalPs[i]) == 0) {
            // printf("call %s\n", globalPFs[i]);
            return 1;
        }
    }

    return 0;
}

int isGlobalFs(char *name) {
    if (name == NULL) return 0;
    char globalFs[2][20] = {"READC", "READI"};
    int i = 0;
    int n = strlen(name);
    char cname[n];

    // convert to upper case
    for (i = 0; i < n; i++) {
        cname[i] = toupper(name[i]);
    }
    cname[i] = '\0';

    // compare the name with all global procedures and functions
    for (i = 0; i < 2; i++) {
        if (strcmp(cname, globalFs[i]) == 0) {
            // printf("call %s\n", globalPFs[i]);
            return 1;
        }
    }

    return 0;
}

Object* checkDeclaredProcedure(char* name) {
    if (isGlobalPs(name)) {
        // if it is a global function
        // then it passes
        return NULL;
    }

    Object *found = lookupObject(name);

    if(found){
        if(found->kind == OBJ_PROCEDURE) {
            return found;
        }
    }

    //printf("%s\n", name);
    // if we cannot find any object having the same name
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);

}

Object* checkDeclaredLValueIdent(char* name) {
    if(isGlobalFs(name)) {
        return NULL;
    }
    Object *ident = lookupObject(name);

    if (ident->kind == OBJ_FUNCTION || ident->kind == OBJ_VARIABLE || ident->kind == OBJ_PARAMETER) {
        return ident;
    }

    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
}


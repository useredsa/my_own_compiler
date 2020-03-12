#ifndef LEXICON_H
#define LEXICON_H

#include <assert.h>

#define             MAX_ID_LENGTH   16
// Data Types
#define             INTEGER         1
#define             STRING          2
// Keywords
#define             PROGRAM         3
#define             FUNCTION        4
#define             CONST           5
#define             VAR             6
#define             INTLIT          7
#define             BEGINN          8
#define             END             9
#define             IF              10
#define             THEN            11
#define             ELSE            12
#define             WHILE           13
#define             DO              14
#define             FOR             15
#define             TO              16
#define             WRITE           17
#define             READ            18
// Operators
#define             ID              19
#define             SEMICOL         20
#define             COLON           21
#define             DOT             22
#define             COMMA           23
#define             PLUSOP          24
#define             MINUSOP         25
#define             MULTOP          26
#define             DIVOP           27
#define             LBRACKET        28
#define             RBRACKET        29
#define             ASSIGNOP        30

const char* lexeme[] = {
    "NULL",
    "INT",
    "STRING",
    "PROGRAM",
    "FUNCTION",
    "CONST",
    "VAR",
    "INTEGER",
    "BEGINN",
    "END",
    "IF",
    "THEN",
    "ELSE",
    "WHILE",
    "DO",
    "FOR",
    "TO",
    "WRITE",
    "READ",
    "ID",
    "SEMICOL",
    "COLON",
    "DOT",
    "COMMA",
    "PLUSOP",
    "MINUSOP",
    "MULTOP",
    "DIVOP",
    "LBRACKET",
    "RBRACKET",
    "ASSIGNOP"
};

static const char* int_to_lexeme(const int id) {
    assert(id < sizeof(lexeme) / sizeof(char*));
    return lexeme[id];
}

#endif // LEXICON_H

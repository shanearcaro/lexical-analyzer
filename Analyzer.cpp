/*
 * File: Analyzer.cpp
 * Project: Assignment02
 * File Created: Tuesday, 13th October 2020 8:46:47 pm
 * Author: Shane Arcaro (sma237@njit.edu)
 */

#include <iostream>
#include <regex>
#include <map>
#include "lex.h"

static std::map<Token, std::string> tokenPrint {
    {PRINT, "PRINT"}, {IF, "IF"},
    {BEGIN, "BEGIN"}, {END, "END"},
    {THEN, "THEN"}, {IDENT, "IDENT"}, 
    {ICONST, "ICONST"}, {SCONST, "SCONST"}, 
    {RCONST, "RCONST"}, {PLUS, "PLUS"}, 
    {MINUS, "MINUS"}, {MULT, "MULT"}, 
    {DIV, "DIV"}, {EQ, "EQ"}, 
    {LPAREN, "LPAREN"}, {RPAREN, "RPAREN"}, 
    {SCOMA, "SCOMA"}, {COMA, "COMA"}, 
    {ERR, "ERR"}, {DONE, "DONE"}
};

ostream& operator<<(ostream& out, const LexItem& tok) {
    std::string *token = &tokenPrint[tok.GetToken()];
    std::cout << *token;

    bool eval = (tok.GetToken() == SCONST) || (tok.GetToken() == RCONST) ||
                (tok.GetToken() == ICONST) || (tok.GetToken() == IDENT)  ||
                (tok.GetToken() == ERR);

    if (eval)
        std::cout << " (" << tok.GetLexeme() << ")";
    return out;
}

LexItem currentToken;
LexItem previousToken;
int characterCount = 0;

LexItem getNextToken(istream& in, int& linenum) {
    enum TokenState { START, INID, INSTRING, ININT, INREAL, INCOMMENT, SIGN} lexstate = START;
    std::string lexeme;
    char character;
    char nextCharacter;

    while (in.get(character)) {
        switch (lexstate) {
            case START:
                if (character == '\n')
                    linenum++;
                if (in.peek() == -1) {
                    if (previousToken.GetToken() != END)
                        return LexItem(ERR, "No END Token", previousToken.GetLinenum());
                    return LexItem(DONE, lexeme, linenum);
                }
                if (std::isspace(character))
                    continue;
                
                lexeme = character;
                nextCharacter = char(in.peek());

                if (character == '/' && nextCharacter == '/') {
                    lexstate = INCOMMENT;
                    continue;
                }

                if (character == '+' || character == '-' || character == '*' ||
                    character == '/' || character == '(' || character == ')' ||
                    character == '=' || character == ',' || character == ';') {
                        lexstate = SIGN;
                        continue;
                    }

                if (character == '\"') {
                    lexstate = INSTRING;
                    continue;
                }

                if (std::isdigit(character)) {
                    lexstate = ININT;
                    continue;
                }

                if (character == '.') {
                    lexstate = INREAL;
                    continue;
                }

                if (std::isalpha(character)) {
                    lexstate = INID;
                    continue;
                }
                return LexItem(ERR, lexeme, linenum);

            case INID:
                if (std::regex_match(lexeme + character, std::regex("[a-zA-Z][a-zA-Z0-9]*")))
                    lexeme += character;
                if (in.peek() == -1 || !std::regex_match(lexeme + character, std::regex("[a-zA-Z][a-zA-Z0-9]*"))) {
                    lexstate = START;
                    in.putback(character);
                    if (lexeme == "begin") {
                        if (previousToken.GetToken() != ERR)
                            return LexItem(ERR, lexeme, linenum);
                        currentToken = LexItem(BEGIN, lexeme, linenum);
                    }
                    else if (lexeme == "print")
                        currentToken = LexItem(PRINT, lexeme, linenum);
                    else if (lexeme == "end") {
                        if (previousToken.GetToken() != SCOMA)
                            return LexItem(ERR, previousToken.GetLexeme(), linenum);
                        currentToken = LexItem(END, lexeme, linenum);
                    }
                    else if (lexeme == "if")
                        currentToken = LexItem(IF, lexeme, linenum);
                    else if (lexeme == "then")
                        currentToken = LexItem(THEN, lexeme, linenum);
                    else {
                        if (previousToken.GetToken() == IDENT)
                            return LexItem(ERR, lexeme, linenum);
                        currentToken = LexItem(IDENT, lexeme, linenum);
                    }

                    if (currentToken != BEGIN && previousToken == ERR)
                        return LexItem(ERR, "No BEGIN Token", currentToken.GetLinenum());
                    previousToken = currentToken;
                    return currentToken;
                }
                break;

            case INSTRING:
                if (previousToken == ERR)
                    return LexItem(ERR, "No Begin Token", linenum);
                if (character == 10)
                    return LexItem(ERR, lexeme, linenum);

                if (std::regex_match(lexeme + character, std::regex("\"[ -~]*"))) {
                    if (character == '\\' && in.peek() == '\"') {
                        lexeme += character;
                        in.get(character);
                        lexeme += character;
                        continue;
                    }
                    else 
                        lexeme += character;
                }
                if (std::regex_match(lexeme + character, std::regex("\"[ -~]*\""))) {
                    lexstate = START;
                    currentToken = LexItem(SCONST, lexeme, linenum);
                    previousToken = currentToken;
                    return currentToken;
                }
                break;

            case ININT:
                if (previousToken == ERR)
                    return LexItem(ERR, "No Begin Token", linenum);
                // Checks if an alpha character is next to an integer number
                if (std::isalpha(character))
                    return LexItem(ERR, lexeme + character, linenum);
                if (std::regex_match(lexeme + character, std::regex("[0-9]+"))) {
                    lexeme += character;
                }
                else if(character == '.') {
                    lexstate = INREAL;
                    in.putback(character);
                    continue;
                }
                else {
                    lexstate = START;
                    in.putback(character);
                    currentToken = LexItem(ICONST, lexeme, linenum);
                    previousToken = currentToken;
                    return currentToken;
                }
                break;

            case INREAL:
                if (previousToken == ERR)
                    return LexItem(ERR, "No Begin Token", linenum);
                // Checks if an alpha character is next to a real number
                if (std::isalpha(character))
                    return LexItem(ERR, lexeme + character, linenum);
                if (std::regex_match(lexeme + character, std::regex("[0-9]*.[0-9]+"))) {
                    lexeme += character;
                }
                else if (std::regex_match(lexeme + character, std::regex("[0-9]*.[0-9]*"))) {
                    lexeme += character;
                }
                else {
                    if (lexeme[lexeme.length() - 1] == '.')
                        return LexItem(ERR, lexeme, linenum);
                    lexstate = START;
                    in.putback(character);
                    currentToken = LexItem(RCONST, lexeme, linenum);
                    previousToken = currentToken;
                    return currentToken;
                }
                break;

            case INCOMMENT:
                if (character == '\n') {
                    linenum++;
                    lexstate = START;
                }
                continue;
            /** 
             * TODO: Right now the code is just being copied and pasted with different
             *       values for testing. Code has to be refactored to not be repetitive.
             */
            case SIGN:
                if (previousToken == ERR)
                    return LexItem(ERR, "No Begin Token", linenum);
                if (lexeme == "+" || lexeme == "*" || lexeme == "/") {
                    Token token = previousToken.GetToken();
                    if (token == IDENT || token == ICONST || token == RCONST) {
                        lexstate = START;
                        in.putback(character);
                        if (lexeme == "+")
                            currentToken = LexItem(PLUS, lexeme, linenum);
                        else if (lexeme == "*")
                            currentToken = LexItem(MULT, lexeme, linenum);
                        else
                            currentToken = LexItem(DIV, lexeme, linenum);
                        previousToken = currentToken;
                        return currentToken;
                    }
                    else
                        return LexItem(ERR, lexeme + character, linenum);
                }
                if (lexeme == "-") {
                    Token token = previousToken.GetToken();
                    if (token == IDENT || token == ICONST || token == RCONST || token == EQ) {
                        lexstate = START;
                        in.putback(character);
                        currentToken = LexItem(MINUS, lexeme, linenum);
                        previousToken = currentToken;
                        return currentToken;
                    }
                    else
                        return LexItem(ERR, lexeme + character, linenum);
                }
                if (lexeme == "(") {
                    Token token = previousToken.GetToken();
                    if (token == IF || token == EQ || token == PLUS || token == MINUS ||
                        token == MULT || token == DIV) {
                        lexstate = START;
                        in.putback(character);
                        currentToken = LexItem(LPAREN, lexeme, linenum);
                        previousToken = currentToken;
                        return currentToken;
                    }
                    else
                        return LexItem(ERR, lexeme + character, linenum);
                }
                if (lexeme == ")") {
                    Token token = previousToken.GetToken();
                    if (token == ICONST || token == RCONST || token == IDENT) {
                        lexstate = START;
                        in.putback(character);
                        currentToken = LexItem(RPAREN, lexeme, linenum);
                        previousToken = currentToken;
                        return currentToken;
                    }
                    else
                        return LexItem(ERR, lexeme + character, linenum);
                }
                if (lexeme == "=") {
                    Token token = previousToken.GetToken();
                    if (token == IDENT) {
                        lexstate = START;
                        in.putback(character);
                        currentToken = LexItem(EQ, lexeme, linenum);
                        previousToken = currentToken;
                        return currentToken;
                    }
                    else
                        return LexItem(ERR, lexeme + character, linenum);
                }
                if (lexeme == ",") {
                    Token token = previousToken.GetToken();
                    if (token == SCONST) {
                        lexstate = START;
                        in.putback(character);
                        currentToken = LexItem(COMA, lexeme, linenum);
                        previousToken = currentToken;
                        return currentToken;
                    }
                    else
                        return LexItem(ERR, lexeme + character, linenum);
                }
                if (lexeme == ";") {
                    Token token = previousToken.GetToken();
                    if (token == SCONST || token == ICONST || token == RCONST || token == IDENT) {
                        lexstate = START;
                        in.putback(character);
                        currentToken = LexItem(SCOMA, lexeme, linenum);
                        previousToken = currentToken;
                        return currentToken;
                    }
                    else
                        return LexItem(ERR, lexeme + character, linenum);
                }
                break;
        }     
    }
    return LexItem(DONE, "", linenum);
}
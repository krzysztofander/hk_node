/*
 * MiniInParser.h
 *
 *  Created on: Mar 22, 2017
 *      Author: Zarnowski
 */

#ifndef MiniInParser_hpp
#define MiniInParser_hpp

#include <stdint.h>

typedef bool(*ParserDataFeeder)(char*);

typedef enum {
    OutParamType_NONE,
    OutParamType_INT_DIGIT,     //signed!
    //beware! Fixed in format 24.8 U2 encoded = 32bits! -> 0x00 00 00 00  XX XX XX XX
    OutParamType_FIXED_DIGIT,
    OutParamType_STRING,
} OutParamType;

typedef struct {
    uint32_t        cmd;
    OutParamType    outParamType;
    union
    {
        uint64_t        numericValue;
        struct
        {
            char*           stringValue;
            uint8_t         stringValueMaxLen;
        };
    };
} Command;

typedef enum {
    ParseResult_WILL_CONTINUE,
    ParseResult_ERROR_NO_CMD,   //Syntax error -> there was no command in stream
    ParseResult_ERROR_INVALID_CMD,    //Syntax error -> command is not made by [A-Z] symbols
    ParseResult_ERROR_STRING_OVERFLOW,    //to long string in argument
    ParseResult_ERROR_MALFORMED,   //General error in syntax
    ParseResult_ERROR_NEED_RESET_PARSER, //last command was malformed/errored call miniInParserReset()
    ParseResult_SUCCESS,     //Successfully parsed, logic can interpret result
} ParseResult;

bool miniInParse(ParserDataFeeder feederFunction, Command* outCmd);

ParseResult miniInParse(char nextChar, Command* outCmd);
void miniInParserReset();

#endif /* MiniInParser_hpp */

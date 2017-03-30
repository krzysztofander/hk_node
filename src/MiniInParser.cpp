/*
 * Parser.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: Zarnowski
 */

#include "MiniInParser.h"


static const char endLineCharacter = 13;

typedef enum {
    MiniInParserMode_EXPECT_COMMAND,
    MiniInParserMode_CMD_PARSED,
    MiniInParserMode_DIGIT,
    MiniInParserMode_DIGIT_FIXED,
    MiniInParserMode_DIGIT_SWALLOW,
    MiniInParserMode_STRING,
    MiniInParserMode_NEED_RESET,
} MiniInParserMode;

typedef enum {
    ParserHelperState_START_OF_MODE,
    ParserHelperState_IN_PROGRESS,
    ParserHelperState_EXPECT_EOL,
    ParserHelperState_SWALLOW_TO_EOL
} ParserHelperState;

static MiniInParserMode mode = MiniInParserMode_EXPECT_COMMAND;
static ParserHelperState parserHelper = ParserHelperState_START_OF_MODE;
static int8_t parserIndex;
static bool negative;
static uint16_t fracPart;

void parseCmd(char nextChar, Command* outCmd, ParseResult* result) {
    if (parserHelper == ParserHelperState_START_OF_MODE) {
        outCmd->cmd = 0;
        parserHelper = ParserHelperState_IN_PROGRESS;
    }
    if (nextChar == endLineCharacter) {
        *result = ParseResult_ERROR_NO_CMD;
        mode = MiniInParserMode_NEED_RESET;
        return;
    }

    if (nextChar < 'A' || nextChar > 'Z') {
        *result = ParseResult_ERROR_INVALID_CMD;
        mode = MiniInParserMode_NEED_RESET;
        return;
    }

    outCmd->cmd |= (uint8_t)nextChar;
    outCmd->cmd <<= 8;
    parserIndex++;

    mode = parserIndex == 3 ? MiniInParserMode_CMD_PARSED : MiniInParserMode_EXPECT_COMMAND;
    *result = ParseResult_WILL_CONTINUE;
}

void finalizeParseWithSuccess(ParseResult* result) {
    *result = ParseResult_SUCCESS;
    miniInParserReset();
}

void handleStringArgument(char nextChar, Command* outCmd, ParseResult* result) {

    if (nextChar == endLineCharacter) {
        if (parserHelper != ParserHelperState_EXPECT_EOL) {
            *result = ParseResult_ERROR_MALFORMED;
            mode = MiniInParserMode_NEED_RESET;
            return;

        } else {
            //end of string
            finalizeParseWithSuccess(result);
            return;
        }
    }

    if (parserIndex >= outCmd->stringValueMaxLen - 1) {
        if (nextChar == '"') {
            outCmd->stringValue[parserIndex] = 0;
            parserHelper = ParserHelperState_EXPECT_EOL;
            *result = ParseResult_WILL_CONTINUE;
            mode = MiniInParserMode_STRING;

        } else {
            outCmd->stringValue[outCmd->stringValueMaxLen - 1] = 0;
            *result = ParseResult_ERROR_STRING_OVERFLOW;
            mode = MiniInParserMode_NEED_RESET;
        }
        return;
    }

    if (nextChar == '"') {
      if (parserHelper == ParserHelperState_EXPECT_EOL) {
          *result = ParseResult_ERROR_MALFORMED;
          mode = MiniInParserMode_NEED_RESET;
        
      } else {
          outCmd->stringValue[parserIndex] = 0;
          parserHelper = ParserHelperState_EXPECT_EOL;
          *result = ParseResult_WILL_CONTINUE;
          mode = MiniInParserMode_STRING;
      }
      return;
    }
  
    if (nextChar <' ' || nextChar > '~') {
        *result = ParseResult_ERROR_MALFORMED;
        mode = MiniInParserMode_NEED_RESET;
        return;
    }

    outCmd->stringValue[parserIndex] = nextChar;
    parserIndex++;
    *result = ParseResult_WILL_CONTINUE;
    mode = MiniInParserMode_STRING;
}

void handleDigitArgument(char nextChar, Command* outCmd, ParseResult* result) {
    if (parserHelper == ParserHelperState_START_OF_MODE) {
        outCmd->numericValue = 0;
        negative = nextChar == '-';
        parserHelper = ParserHelperState_IN_PROGRESS;
        if (negative == true) {
            *result = ParseResult_WILL_CONTINUE;
            mode = MiniInParserMode_DIGIT;
            return;
        }
    }
    if (nextChar == endLineCharacter) {
        if (negative) {
            outCmd->numericValue = -outCmd->numericValue;
        }
        finalizeParseWithSuccess(result);
        return;
    }
    if (nextChar == '.') {
        *result = ParseResult_WILL_CONTINUE;
        mode = MiniInParserMode_DIGIT_FIXED;
        parserIndex = 0;
        fracPart = 0;
        outCmd->outParamType = OutParamType_FIXED_DIGIT;
        return;
    }
    if (nextChar < '0' || nextChar > '9') {
        *result = ParseResult_ERROR_MALFORMED;
        mode = MiniInParserMode_NEED_RESET;
        return;
    }
    outCmd->numericValue *= 10;
    outCmd->numericValue += nextChar - '0';
    *result = ParseResult_WILL_CONTINUE;
    mode = MiniInParserMode_DIGIT;
}

void finalizeFixedDigit(Command* outCmd, ParseResult* result) {
    //in outCmd->numericValue is integral part, it need to be truncated to 23b (1 bit for sign)
    if (outCmd->numericValue > 0x7FFFFF) {
        outCmd->numericValue = 0x7FFFFF;
    }
    outCmd->numericValue <<= 8; //make place for fract part
    uint16_t base = 1;
    for(;parserIndex != 0; parserIndex --) {
        base *= 10;
    }
    uint16_t scalled = (fracPart << 8) / base;
    outCmd->numericValue |= scalled & 0xFF;
    if (negative) {
        outCmd->numericValue = (~outCmd->numericValue) & 0xFFFFFFFF;
    }
    finalizeParseWithSuccess(result);
}

void handleFracDigitArgument(char nextChar, Command* outCmd, ParseResult* result) {
    if (nextChar == endLineCharacter) {
        finalizeFixedDigit(outCmd, result);
        return;
    }

    if (nextChar < '0' || nextChar > '9') {
        *result = ParseResult_ERROR_MALFORMED;
        mode = MiniInParserMode_NEED_RESET;
        return;
    }
    if (parserHelper == ParserHelperState_IN_PROGRESS) {
        fracPart *= 10;
        fracPart += nextChar - '0';
        parserIndex++;
        parserHelper = parserIndex < 3 ? ParserHelperState_IN_PROGRESS : ParserHelperState_SWALLOW_TO_EOL;
    }
    *result = ParseResult_WILL_CONTINUE;
    mode = MiniInParserMode_DIGIT_FIXED;
}

void discoverParam(char nextChar, Command* outCmd, ParseResult* result) {
    if (nextChar == '"') {
        *result = ParseResult_WILL_CONTINUE;
        mode = MiniInParserMode_STRING;
        outCmd->outParamType = OutParamType_STRING;
        parserHelper = ParserHelperState_START_OF_MODE;
        parserIndex = 0;

    } else if (nextChar == '-' || (nextChar >= '0' && nextChar <= '9')) {
        *result = ParseResult_WILL_CONTINUE;
        mode = MiniInParserMode_DIGIT;
        outCmd->outParamType = OutParamType_INT_DIGIT;
        parserHelper = ParserHelperState_START_OF_MODE;
        parserIndex = 0;
        handleDigitArgument(nextChar, outCmd, result);

    } else if (nextChar == endLineCharacter) {
        finalizeParseWithSuccess(result);
        outCmd->outParamType = OutParamType_NONE;

    } else {
        *result = ParseResult_ERROR_MALFORMED;
        mode = MiniInParserMode_NEED_RESET;
    }
}

ParseResult miniInParse(char nextChar, Command* outCmd) {
    ParseResult result = ParseResult_ERROR_MALFORMED;
    switch(mode) {
        default:
        case MiniInParserMode_EXPECT_COMMAND:
            parseCmd(nextChar, outCmd, &result);
            break;

        case MiniInParserMode_CMD_PARSED:
            discoverParam(nextChar, outCmd, &result);
            break;

        case MiniInParserMode_STRING:
            handleStringArgument(nextChar, outCmd, &result);
            break;

        case MiniInParserMode_DIGIT:
            handleDigitArgument(nextChar, outCmd, &result);
            break;

        case MiniInParserMode_DIGIT_FIXED:
            handleFracDigitArgument(nextChar, outCmd, &result);
            break;
        
        case MiniInParserMode_NEED_RESET:
            result = ParseResult_ERROR_NEED_RESET_PARSER;
            break;
    }

    return result;
}

void miniInParserReset() {
    mode = MiniInParserMode_EXPECT_COMMAND;
    parserIndex = 0;
    negative = false;
    fracPart = 0;
    parserHelper = ParserHelperState_START_OF_MODE;
}

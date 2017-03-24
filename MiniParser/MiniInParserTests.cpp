/*
 * MiniInParserTests.cpp
 *
 *  Created on: Mar 23, 2017
 *      Author: Zarnowski
 */

#include "MiniInParserTests.h"
#include "MiniInParser.h"
#include <stdio.h>
#include <string.h>

ParseResult executeParse(const char* strCmd, Command* cmd) {
    ParseResult result = ParseResult_ERROR_NO_CMD;
    for(int t = 0; t < strlen(strCmd); t++) {
        result = miniInParse(strCmd[t], cmd);
        if (result != ParseResult_WILL_CONTINUE) {
            break;
        }
    }
    return result;
}

bool testNoParamCmd() {
    Command cmd;
    bool testResult;
    miniInParserReset();

    ParseResult result = executeParse("CMD\r", &cmd);
    testResult = result == ParseResult_SUCCESS;
    testResult &= cmd.cmd == 0x434d4400;   //cmd

    //invalid
    //no " starting string
    result = executeParse("CMDinv\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;

    //no capital letters used
    miniInParserReset();
    result = executeParse("CmD\r", &cmd);
    testResult &= result == ParseResult_ERROR_INVALID_CMD;

    //numbers not allowed here
    miniInParserReset();
    result = executeParse("C1D\r", &cmd);
    testResult &= result == ParseResult_ERROR_INVALID_CMD;

    //to short command
    miniInParserReset();
    result = executeParse("C\r", &cmd);
    testResult &= result == ParseResult_ERROR_NO_CMD;

    //too short command
    miniInParserReset();
    result = executeParse("CM\r", &cmd);
    testResult &= result == ParseResult_ERROR_NO_CMD;

    //no command
    miniInParserReset();
    result = executeParse("\r", &cmd);
    testResult &= result == ParseResult_ERROR_NO_CMD;

    return testResult;
}

bool testIntDigitParamCmd() {
    Command cmd;
    bool testResult;
    miniInParserReset();

    ParseResult result = executeParse("CMD123\r", &cmd);
    testResult = result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_INT_DIGIT;
    testResult &= cmd.numericValue == 123;
    testResult &= cmd.cmd == 0x434d4400;   //cmd

    //invalid character 'x'
    miniInParserReset();
    result = executeParse("CMD12x2\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;

    //no hex is supported
    miniInParserReset();
    result = executeParse("CMD12a2\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;

    //chack max 32 bits value
    miniInParserReset();
    result = executeParse("CMD4294967295\r", &cmd);
    testResult &= result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_INT_DIGIT;
    testResult &= cmd.numericValue == 0xFFFFFFFF;
    testResult &= cmd.cmd == 0x434d4400;   //cmd

    //chack max 64 bits value
    result = executeParse("CMD18446744073709551615\r", &cmd);
    testResult &= result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_INT_DIGIT;
    testResult &= cmd.numericValue == 0xffffffffffffffff;
    testResult &= cmd.cmd == 0x434d4400;   //cmd
  
    //check -98 bits value
    result = executeParse("CMD-98\r", &cmd);
    testResult &= result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_INT_DIGIT;
    testResult &= ((int64_t)cmd.numericValue) == -98;
    testResult &= cmd.cmd == 0x434d4400;   //cmd

    //'-' in wrong place
    miniInParserReset();
    result = executeParse("CMD11-22\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;

    //too many '-'
    miniInParserReset();
    result = executeParse("CMD--1122\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;

    return testResult;
}

bool testStringParamCmd() {
    char strBuf[20];
    Command cmd;
    cmd.stringValue = strBuf;
    cmd.stringValueMaxLen = 10;   //we spare some space for overflow test
    bool testResult;
    miniInParserReset();

    ParseResult result = executeParse("CMD\"test\"\r", &cmd);
    testResult = result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_STRING;
    testResult &= strcmp("test", cmd.stringValue) == 0;

    //even if buffer is 10, we can put only 9 chars there, it's must be Null-termineted!
    result = executeParse("CMD\"012345678\"\r", &cmd);
    testResult &= result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_STRING;
    testResult &= strcmp("012345678", cmd.stringValue) == 0;

    //invalid -> overflow
    memset(strBuf, 0, 20);
    result = executeParse("CMD\"01234567891\"\r", &cmd);
    testResult &= result == ParseResult_ERROR_STRING_OVERFLOW;
    testResult &= strBuf[9] == 0;
    testResult &= strBuf[10] == 0;

    //invalid character
    miniInParserReset();
    result = executeParse("CMD\"\x10\"\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;

    //invalid character 3 x "
    miniInParserReset();
    result = executeParse("CMD\"\"\"\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;

    return testResult;
}

bool testIntFixedParamCmd() {
    Command cmd;
    bool testResult;
    miniInParserReset();
    
    ParseResult result = executeParse("CMD123.00\r", &cmd);
    testResult = result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_FIXED_DIGIT;
    testResult &= cmd.numericValue == (123 << 8);
    
    result = executeParse("CMD123.12\r", &cmd);
    testResult &= result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_FIXED_DIGIT;
    testResult &= cmd.numericValue == ((123 << 8) | (12 * 256 / 100));

    //double '-' is failure
    result = executeParse("CMD--123.12\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;
  
    // '-' in wrong place
    miniInParserReset();
    result = executeParse("CMD123.-12\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;

    // illegal character
    miniInParserReset();
    result = executeParse("CMD123.1d2\r", &cmd);
    testResult &= result == ParseResult_ERROR_MALFORMED;
  
    //overflow of frac part, however it's legal will be truncated
    miniInParserReset();
    result = executeParse("CMD0.12324234234234\r", &cmd);
    testResult &= result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_FIXED_DIGIT;
    testResult &= cmd.numericValue == 31;
  
    //overflow of int part, however it's legal should be truncated to 0x7fff
    miniInParserReset();
    result = executeParse("CMD9437183.0\r", &cmd);
    testResult &= result == ParseResult_SUCCESS;
    testResult &= cmd.outParamType == OutParamType_FIXED_DIGIT;
    testResult &= cmd.numericValue == 0x7fffff00;
  
  //TODO: No power to do negative values check, feel free to finish
    return testResult;
}

bool testMiniInParser() {
    bool result = true;
  
    result &= testNoParamCmd();
    result &= testIntDigitParamCmd();
    result &= testIntFixedParamCmd();
    result &= testStringParamCmd();

    return result;
}

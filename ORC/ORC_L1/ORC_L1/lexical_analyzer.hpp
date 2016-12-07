//
//  lexical_analyzer.hpp
//  ORC_L1
//
//  Created by Aleksandr Borzikh on 07.12.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#ifndef lexical_analyzer_hpp
#define lexical_analyzer_hpp

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdint.h>

class Lexeme
{
public:
    enum class Type : char
    {
        VAR = 0x00,
        CONST,
        ID,
        BRACKET_OPEN,
        BRACKET_CLOSE,
        SEMICOLON,
        COMMA,
        ASSIGNMENT,
        WHILE,
        DO,
        UNAR_OPER_NOT,
        OPERATOR_EQUAL,
        OPERATOR_LESS,
        OPERATOR_GREATER,
        OPERATOR_ADD,
        OPERATOR_SUB,
        OPERATOR_DIV,
        OPERATOR_MUL,
        
    };
    
public:
    Lexeme() = delete;
    Lexeme(enum Type,
           int32_t,
           size_t,
           size_t);
    
    Type        GetType() const;
    int32_t     GetValue() const;
    size_t      GetLineNumber() const;
    size_t      GetColumnNumber() const;
    
private:
    enum Type   m_eType;
    int32_t     m_dValue;
    size_t      m_nLine;
    size_t      m_nColumn;
};

class LexicalAnalyzer
{
public:
    LexicalAnalyzer(const std::string&);
    
    void Analyze();
    void PrintContent();
    
private:
    std::vector<Lexeme> m_aLexemes;
    std::map<std::string, uint32_t > m_mIDTable;
    
    std::ifstream m_oFileStream;
};

#endif /* lexical_analyzer_hpp */

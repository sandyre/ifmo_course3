//
//  lexical_analyzer.cpp
//  ORC_L1
//
//  Created by Aleksandr Borzikh on 07.12.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#include <iostream>
#include <cctype>

#include "lexical_analyzer.hpp"

const std::array<std::string, (size_t)Lexeme::Type::TypeEnumSize>
Lexeme::TypeToString
{
    "VAR",
    "CONST",
    "IDENTIF",
    "(", ")", ";", ",", ":=", "WHILE", "DO", "NOT",
    "==", "<", ">", "+", "-", "/", "*"
};

Lexeme::Lexeme(enum Lexeme::Type eType,
               int32_t dValue,
               size_t nLine,
               size_t nColumn) :
m_eType(eType),
m_dValue(dValue),
m_nLine(nLine),
m_nColumn(nColumn)
{
}

Lexeme::Type
Lexeme::GetType() const
{
    return m_eType;
}

int32_t
Lexeme::GetValue() const
{
    return m_dValue;
}

size_t
Lexeme::GetLineNumber() const
{
    return m_nLine;
}

size_t
Lexeme::GetColumnNumber() const
{
    return m_nColumn;
}


LexicalAnalyzer::LexicalAnalyzer(const std::string& sFilename) :
m_oFileStream(std::ifstream(sFilename))
{
    if(!m_oFileStream.is_open())
    {
        std::cerr << "ERROR OPENING FILE: " << sFilename << std::endl;
    }
}

void
LexicalAnalyzer::Analyze()
{
        // let the game begin!
    if(!m_oFileStream.is_open())
        return;
    
    char Letter;
    size_t nCurrentLine = 0, nCurrentColumn = 0;
    while(!m_oFileStream.eof())
    {
        m_oFileStream.get(Letter);
        ++nCurrentColumn;
        
        switch(Letter)
        {
            case ' ' :
                ++nCurrentColumn;
                break;
                
            case '\n' :
                ++nCurrentLine;
                nCurrentColumn = 0;
                break;
            
            case ';' :
                m_aLexemes.emplace_back(Lexeme(Lexeme::Type::SEMICOLON, 0,
                                               nCurrentLine, nCurrentColumn));
                break;
            
            case ',' :
                m_aLexemes.emplace_back(Lexeme(Lexeme::Type::COMMA, 0,
                                               nCurrentLine, nCurrentColumn));
                break;
                
            case '(' :
                m_aLexemes.emplace_back(Lexeme(Lexeme::Type::BRACKET_OPEN, 0,
                                               nCurrentLine, nCurrentColumn));
                break;
            
            case ')' :
                m_aLexemes.emplace_back(Lexeme(Lexeme::Type::BRACKET_CLOSE, 0,
                                               nCurrentLine, nCurrentColumn));
                break;
                
            case '*' :
                m_aLexemes.emplace_back(Lexeme(Lexeme::Type::OPERATOR_MUL, 0,
                                               nCurrentLine, nCurrentColumn));
                break;
                
            case '+' :
                m_aLexemes.emplace_back(Lexeme(Lexeme::Type::OPERATOR_ADD, 0,
                                               nCurrentLine, nCurrentColumn));
                break;
                
            case '-' :
                    // two possibilites - binary or unary minus
                if(!m_aLexemes.size())
                {
                    m_aLexemes.emplace_back(Lexeme(Lexeme::Type::UNAR_OPER_NOT, 0,
                                                   nCurrentLine, nCurrentColumn));
                    break;
                }
                else
                {
                    if(m_aLexemes.back().GetType() == Lexeme::Type::CONST ||
                       m_aLexemes.back().GetType() == Lexeme::Type::ID ||
                       m_aLexemes.back().GetType() == Lexeme::Type::BRACKET_CLOSE)
                    {
                        m_aLexemes.emplace_back(Lexeme(Lexeme::Type::OPERATOR_SUB, 0,
                                                       nCurrentLine, nCurrentColumn));
                        break;
                    }
                    else
                    {
                        m_aLexemes.emplace_back(Lexeme(Lexeme::Type::UNAR_OPER_NOT, 0,
                                                       nCurrentLine, nCurrentColumn));
                        break;
                    }
                }
                break;
            
            case ':' :
                if(m_oFileStream.peek() == '=')
                {
                    m_aLexemes.emplace_back(Lexeme(Lexeme::Type::ASSIGNMENT, 0,
                                                   nCurrentLine, nCurrentColumn));
                    m_oFileStream.get();
                }
                else
                {
                    std::cerr << "UNDEFINED LEXEME, ABORT\n";
                    return;
                }
                
                break;
                
            case '=' :
                if(m_oFileStream.peek() == '=')
                {
                    m_aLexemes.emplace_back(Lexeme(Lexeme::Type::OPERATOR_EQUAL, 0,
                                                   nCurrentLine, nCurrentColumn));
                    m_oFileStream.get();
                }
                else
                {
                    std::cerr << "UNDEFINED LEXEME, ABORT\n";
                    return;
                }
                break;
                
            case '>' :
                m_aLexemes.emplace_back(Lexeme(Lexeme::Type::OPERATOR_GREATER, 0,
                                               nCurrentLine, nCurrentColumn));
                break;
                
            case '<' :
                m_aLexemes.emplace_back(Lexeme(Lexeme::Type::OPERATOR_LESS, 0,
                                               nCurrentLine, nCurrentColumn));
                break;
            
            default:
                if(!(std::isalpha(Letter) || std::isdigit(Letter)))
                {
                    std::cerr << "Found unrecognizable char in input stream.\n";
                    break;
                }
                
                size_t nCharsReaded = 1;
                std::string Keyword;
                Keyword += Letter;
                if(std::isalpha(Letter))
                {
                    while(std::isalpha(m_oFileStream.peek()))
                    {
                        m_oFileStream.get(Letter);
                        Keyword += Letter;
                        ++nCharsReaded;
                    }
                    
                    if(!strcmp(Keyword.c_str(), "Var"))
                    {
                        m_aLexemes.emplace_back(Lexeme(Lexeme::Type::VAR, 0,
                                                       nCurrentLine, nCurrentColumn));
                        nCurrentColumn += nCharsReaded;
                        break;
                    }
                    else if(!strcmp(Keyword.c_str(), "WHILE"))
                    {
                        m_aLexemes.emplace_back(Lexeme(Lexeme::Type::WHILE, 0,
                                                     nCurrentColumn, nCurrentColumn));
                        nCurrentColumn += nCharsReaded;
                        break;
                    }
                    else if(!strcmp(Keyword.c_str(), "DO"))
                    {
                        m_aLexemes.emplace_back(Lexeme(Lexeme::Type::DO, 0,
                                                      nCurrentColumn, nCurrentColumn));
                        nCurrentColumn += nCharsReaded;
                        break;
                    }
                    else
                    {
                        auto IDIter = m_mIDTable.find(Keyword);
                        if(IDIter != m_mIDTable.end())
                        {
                            m_aLexemes.emplace_back(Lexeme(Lexeme::Type::ID, IDIter->second,
                                                           nCurrentLine, nCurrentColumn));
                        }
                        else
                        {
                            m_mIDTable.insert(std::make_pair(Keyword, m_mIDTable.size()));
                            m_aLexemes.emplace_back(Lexeme(Lexeme::Type::ID, m_mIDTable.size()-1,
                                                           nCurrentLine, nCurrentColumn));
                        }
                        
                        nCurrentColumn += nCharsReaded;
                    }
                }
                
                if(std::isdigit(Letter))
                {
                    while(std::isdigit(m_oFileStream.peek()))
                    {
                        m_oFileStream.get(Letter);
                        Keyword += Letter;
                        ++nCharsReaded;
                    }
                    
                    m_aLexemes.emplace_back(Lexeme(Lexeme::Type::CONST, std::stoi(Keyword),
                                                   nCurrentLine, nCurrentColumn));
                    nCurrentColumn += nCharsReaded;
                }
                
                break;
        }
    }
}

void
LexicalAnalyzer::PrintContent()
{
    std::cout << "LEXEMES:\n";
    for(auto& lexem : m_aLexemes)
    {
        std::cout << lexem.TypeToString[(int)lexem.GetType()] << " "
                  << lexem.GetValue() << " "
                  << lexem.GetLineNumber() << " "
                  << lexem.GetColumnNumber() << "\n";
    }
    std::cout << "IDS TABLE:\n";
    for(auto iter = m_mIDTable.begin(); iter != m_mIDTable.end(); ++iter)
    {
        std::cout << iter->first << " " << iter->second << "\n";
    }
}

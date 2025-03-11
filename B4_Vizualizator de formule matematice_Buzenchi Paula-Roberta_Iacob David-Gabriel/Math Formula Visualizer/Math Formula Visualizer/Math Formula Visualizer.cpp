//Warning! When typing a formula, some text might not render on the screen. This problem is out of the scope of our project.

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS

#endif //This declaration is here because Visual Studio gives out constant warnings when using functions from "cstring". Without this, the project will not compile if Visual Studio is used as an IDE.

#include "graphics.h"
#pragma comment(lib,"graphics.lib")

#include <iostream>
#include <cstring>
#include <queue>
#include <stack>
#include <vector>
#include <cstdlib>

using namespace std;

#define MAX_CFormLen 200 //The maximum length of a formula in number of characters.
#define MAX_ArrayCFormLen MAX_CFormLen + 1 //The maximum length of a character array that represents a formula.
#define MAX_OprdOrOprtOrFunLen 25 //The maximum length of an operand or an operation or a function.
#define MAX_ArrayOOFLen MAX_OprdOrOprtOrFunLen + 1 //The maximum length of a character array that represents an operand or an operation or a function.

#define Max_ButtonTextLen 50 //The maximum length of the text that is displayed inside a button.
#define Max_ArrayButtonTextLen Max_ButtonTextLen + 1 //The maximum length of a character array that represents the text that is displayed inside a button.

#define pi 3.14159265
#define e 2.71828182

#define NODE_INFO_MAX_LENGTH MAX_OprdOrOprtOrFunLen

struct TokenizedFormula
{
    char tokens[MAX_CFormLen][MAX_ArrayOOFLen] = {};
    int numberOfTokens = 0;
};

struct Node
{
    char information[MAX_ArrayOOFLen] = "";
    Node* left = NULL;
    Node* right = NULL;
};

enum TokenNature
{
    OPERAND,
    OPERATION,
    FUNCTION,
    OPEN_BRACKET,
    CLOSED_BRACKET
};

enum ApplicationStatuses
{
    MAIN_MENU,
    TYPE_AN_EXPRESSION,
    DISPLAY_EXPRESSION,
    DISPLAY_EXPRESSION_TREE,
};

enum ButtonsTypes
{
    EXIT,
    SWITCH_TO_TYPE_AN_EXPRESSION,
    SWITCH_TO_DISPLAY_EXPRESSION,
    SWITCH_TO_DISPLAY_EXPRESSION_TREE,
    SUBMIT_AN_EXPRESSION,
    TYPE_A_CHARACTER,
    REMOVE_A_CHARACTER,
    SWITCH_LETTERS_CASE,
    MOVE_LEFT,
    MOVE_UP,
    MOVE_RIGHT,
    MOVE_DOWN
};

struct ButtonPosition
{
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
};

struct Button
{
    ButtonsTypes type;
    ButtonPosition position;
    char text[Max_ArrayButtonTextLen] = "";
};

const char validCharactersForAFormula_1[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char validCharactersForAFormula_2[] = "0123456789";
const char validCharactersForAFormula_3[] = "+-*/^%()";
const char validCharactersForAFormula_4[] = ".";
const char validCharactersForAFormula_5[] = "_";

//In the following constant arrays of arrays of characters, every array of characters must have the same length!
const char brackets[][MAX_ArrayOOFLen] = { "(", ")" };
const char firstDegreeOperations[][MAX_ArrayOOFLen] = { "+", "-" };
const char secondDegreeOperations[][MAX_ArrayOOFLen] = { "*", "/" };
const char thirdDegreeOperations[][MAX_ArrayOOFLen] = { "^" , "%" };
const char basicMathematicalFunctions_1[][MAX_ArrayOOFLen] = { "abs" };
const char basicMathematicalFunctions_2[][MAX_ArrayOOFLen] = { "lg", "ln" };
const char basicMathematicalFunctions_3[][MAX_ArrayOOFLen] = { "sqrt" };
const char basicTrigonometricFunctions[][MAX_ArrayOOFLen] = { "sin", "cos", "tan" };
const char inverseBasicTrigonometricFunctions[][MAX_ArrayOOFLen] = { "asin", "acos", "atan" };
const char hyperbolicTrigonometricFunctions[][MAX_ArrayOOFLen] = { "sinh", "cosh", "tanh" };
const char inverseHyperbolicTrigonometricFunctions[][MAX_ArrayOOFLen] = { "asinh", "acosh", "atanh" };
const char diverseMathematicalFunctions[][MAX_ArrayOOFLen] = { "log", "rad", "sum", "mul", "lim", "der", "int" };

char twovariablesoperator[7][2] = { "+","-","*", "/","^", "%" }; //the operators who requires two variables.
char onevariableoperator[23][7] = { "-", "abs", "lg", "ln", "sqrt", "sin", "cos", "tan", "cot", "asin", "acos", "atan", "acot", "sinh", "cosh", "tanh", "coth", "asinh", "acosh", "atanh", "acoth" };//the operators who requires only one variable
char specialoperator[8][4] = { "log", "rad","sum","mul", "lim", "der", "int" };

ApplicationStatuses applicationStatus = MAIN_MENU;
vector<Button> mainMenuButtons, typeAnExpressionButtons, displayExpressionButtons, displayExpressionTreeButtons;
bool typeLowercaseLetters = true;
char Formula[MAX_ArrayCFormLen] = "";
bool showEmptyFormulaMessage = false;
bool showSyntacticallyIncorrectFormulaMessage = false;
bool showToLongOperandsMessage = false;
bool showBasicMathematicalPrinciplesBrokenMessage = false;
Node* Root = new Node;
int xDrawingExpressionStartPosition = 0, yDrawingExpressionStartPosition = 0;
int xDrawingExpressionTreeStartPosition = 0, yDrawingExpressionTreeStartPosition = 0;
int xDrawingDelta = 100, yDrawingDelta = 50;

bool DoesItContainsASpecificArray(const char arrayOfArrays[][MAX_ArrayOOFLen], const int arrayLength, char array[MAX_ArrayOOFLen]) //Does an array of arrays of characters contain a specific array of characters.
{
    for (int ind = 0; ind < arrayLength; ind++)
        if (!strcmp(arrayOfArrays[ind], array))
            return true;

    return false;
}

bool DoesItContainsASpecificArrayWithNumberOfCharacters(const char arrayOfArrays[][MAX_ArrayOOFLen], const int arrayLength, char array[MAX_ArrayOOFLen], int number) //Does an array of arrays of characters contain a specific array of characters.
{
    for (int ind = 0; ind < arrayLength; ind++)
        if (!strncmp(arrayOfArrays[ind], array, number))
            return true;

    return false;
}

TokenNature GetTokenNature(char token[MAX_ArrayOOFLen])
{
    if (!strcmp(token, "("))
        return OPEN_BRACKET;

    if (!strcmp(token, ")"))
        return CLOSED_BRACKET;

    if (DoesItContainsASpecificArray(basicMathematicalFunctions_1, 1, token) || DoesItContainsASpecificArray(basicMathematicalFunctions_2, 2, token) || DoesItContainsASpecificArray(basicMathematicalFunctions_3, 1, token) || DoesItContainsASpecificArray(basicTrigonometricFunctions, 3, token) || DoesItContainsASpecificArray(inverseBasicTrigonometricFunctions, 3, token) || DoesItContainsASpecificArray(hyperbolicTrigonometricFunctions, 3, token) || DoesItContainsASpecificArray(inverseHyperbolicTrigonometricFunctions, 3, token) || DoesItContainsASpecificArrayWithNumberOfCharacters(diverseMathematicalFunctions, 7, token, 3))
        return FUNCTION;

    if (DoesItContainsASpecificArray(firstDegreeOperations, 2, token) || DoesItContainsASpecificArray(secondDegreeOperations, 2, token) || DoesItContainsASpecificArray(thirdDegreeOperations, 2, token))
        return OPERATION;

    return OPERAND;
}

//void RemoveBlankSpacesFromAFormula(char formula[MAX_ArrayCFormLen])
//{
//    for (int ind = 0; ind < strlen(formula); ind++)
//        if (formula[ind] == ' ')
//            strcpy(formula + ind, formula + ind + 1), ind--;
//}

void PartiallySeparateAFormulaInTokens(char formula[MAX_ArrayCFormLen], const char oprtOrFun[][MAX_ArrayOOFLen], const int numberOfOprtOrFun, int& ind, bool& tokenize)
{
    int oprtOrFunLength = strlen(oprtOrFun[0]);
    for (int point = 0; tokenize && point < numberOfOprtOrFun; point++)
        if (!strncmp(formula + ind, oprtOrFun[point], oprtOrFunLength))
        {
            char temp[MAX_ArrayCFormLen] = "";
            bool test = false;

            if (formula[ind + oprtOrFunLength] != '\0')
            {
                strcpy(temp, formula + ind + oprtOrFunLength);
                formula[ind + oprtOrFunLength] = '|';
                strcpy(formula + ind + oprtOrFunLength + 1, temp);
            }

            if (ind > 0)
                if (formula[ind - 1] != '|')
                {
                    strcpy(temp, formula + ind);
                    formula[ind] = '|';
                    strcpy(formula + ind + 1, temp);
                    test = true;
                }

            ind += oprtOrFunLength;

            if (test)
                ind++;

            tokenize = false;
        }
}

void SeparateAFormulaInTokens(char formula[MAX_ArrayCFormLen])
{
    for (int ind = 0; ind < strlen(formula); ind++)
        if (formula[ind] == '_' && formula[ind + 1] == '-')
            ind++;
        else
        {
            bool tokenize = true;

            PartiallySeparateAFormulaInTokens(formula, inverseHyperbolicTrigonometricFunctions, 3, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, hyperbolicTrigonometricFunctions, 3, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, inverseBasicTrigonometricFunctions, 3, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, basicTrigonometricFunctions, 3, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, basicMathematicalFunctions_3, 1, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, basicMathematicalFunctions_2, 2, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, basicMathematicalFunctions_1, 1, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, thirdDegreeOperations, 2, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, secondDegreeOperations, 2, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, firstDegreeOperations, 2, ind, tokenize);
            PartiallySeparateAFormulaInTokens(formula, brackets, 2, ind, tokenize);
        }
}

void TokenizeAFormula(char formula[MAX_ArrayCFormLen], TokenizedFormula& tokenizedFormula)
{
    char formulaCopy[MAX_ArrayCFormLen];
    strcpy(formulaCopy, formula);
    SeparateAFormulaInTokens(formulaCopy);

    char* token = strtok(formulaCopy, "|");
    while (token)
    {
        if (!strcmp(token, "-"))
        {
            if (!tokenizedFormula.numberOfTokens)
                strcpy(tokenizedFormula.tokens[tokenizedFormula.numberOfTokens++], "NULL");
            else if (!strcmp(tokenizedFormula.tokens[tokenizedFormula.numberOfTokens - 1], "("))
                strcpy(tokenizedFormula.tokens[tokenizedFormula.numberOfTokens++], "NULL");
        }

        strcpy(tokenizedFormula.tokens[tokenizedFormula.numberOfTokens++], token);
        token = strtok(NULL, "|");
    }
}

bool IsAFormulaThatOnlyContainsValidCharacters(char formula[MAX_ArrayCFormLen])
{
    for (int ind = 0; ind < strlen(formula); ind++)
        if (!(strchr(validCharactersForAFormula_1, formula[ind]) || strchr(validCharactersForAFormula_2, formula[ind]) || strchr(validCharactersForAFormula_3, formula[ind]) || strchr(validCharactersForAFormula_4, formula[ind]) || strchr(validCharactersForAFormula_5, formula[ind])))
            return false;

    return true;
}

bool IsACorrectlyBracketedFormula(char formula[MAX_ArrayCFormLen])
{
    int bracketsCheck = 0;
    for (int ind = 0; ind < strlen(formula); ind++)
    {
        if (formula[ind] == '(')
            bracketsCheck++;
        else if (formula[ind] == ')')
            bracketsCheck--;

        if (bracketsCheck < 0)
            return false;
    }

    if (bracketsCheck)
        return false;

    return true;
}

bool IsAFormulaThatRespectsTheLimitationsInOperandsLength(char formula[MAX_ArrayCFormLen])
{
    char formulaCopy[MAX_ArrayCFormLen];
    strcpy(formulaCopy, formula);
    SeparateAFormulaInTokens(formulaCopy);

    char* token = strtok(formulaCopy, "|");
    while (token)
    {
        if (strlen(token) > MAX_OprdOrOprtOrFunLen)
            return false;

        token = strtok(NULL, "|");
    }

    return true;
}

bool IsASyntacticallyCorrectOperand(char operand[MAX_ArrayOOFLen])
{
    if (!strlen(operand) || (strlen(operand) == 1 && operand[0] == '-'))
        return false;

    int dotCounter = 0;
    int lettersCounter = 0;
    int digitCounter = 0;
    int zeroCounter = 0;

    char operandCopy[MAX_ArrayOOFLen] = "";
    (operand[0] == '-') ? strcpy(operandCopy, operand + 1) : strcpy(operandCopy, operand);

    for (int ind = 0; ind < strlen(operandCopy); ind++)
    {
        if (!strchr(validCharactersForAFormula_1, operandCopy[ind]) && !strchr(validCharactersForAFormula_2, operandCopy[ind]) && !strchr(validCharactersForAFormula_4, operandCopy[ind]))
            return false;

        if (operandCopy[ind] == '.')
        {
            if (ind == 0 || ind == strlen(operandCopy) - 1)
                return false;

            dotCounter++;

            if (dotCounter > 1)
                return 0;
        }

        if (strchr(validCharactersForAFormula_1, operandCopy[ind]))
            lettersCounter++;

        if (strchr(validCharactersForAFormula_2, operandCopy[ind]))
        {
            if (ind == 0 && operandCopy[ind] == '0' && strlen(operandCopy) > 1)
                if (operandCopy[1] != '.')
                    return false;

            digitCounter++;

            if (operandCopy[ind] == '0')
                zeroCounter++;
        }
    }

    if (dotCounter && digitCounter != strlen(operandCopy) - 1)
        return false;

    if (digitCounter && lettersCounter)
        return false;

    if (operand[0] == '-' && (zeroCounter >= strlen(operand) - 2 && zeroCounter))
        return false;

    return true;
}

bool IsAFormulaThatOnlyContainsValidOperands(char formula[MAX_ArrayCFormLen])
{
    TokenizedFormula tokenizedFormula;
    TokenizeAFormula(formula, tokenizedFormula);

    for (int ind = 0; ind < tokenizedFormula.numberOfTokens; ind++)
        if (GetTokenNature(tokenizedFormula.tokens[ind]) == OPERAND && !IsASyntacticallyCorrectOperand(tokenizedFormula.tokens[ind]))
            return false;

    return true;
}

bool IsAFormulaThatRespectsTheOperandsAndOperationsPrinciples(char formula[MAX_ArrayCFormLen])
{
    TokenizedFormula tokenizedFormula;
    TokenizeAFormula(formula, tokenizedFormula);

    if (!tokenizedFormula.numberOfTokens)
    {
        return false;
    }
    else if (tokenizedFormula.numberOfTokens == 1)
    {
        if (GetTokenNature(tokenizedFormula.tokens[0]) != OPERAND)
        {
            return false;
        }
        return true;
    }

    for (int ind = 0; ind < tokenizedFormula.numberOfTokens; ind++)
    {
        TokenNature tokenNature = GetTokenNature(tokenizedFormula.tokens[ind]);

        if (!ind)
        {
            TokenNature rightTokenNature = GetTokenNature(tokenizedFormula.tokens[ind + 1]);

            switch (tokenNature)
            {
            case OPERAND:
                if (rightTokenNature == OPERAND || rightTokenNature == FUNCTION || rightTokenNature == OPEN_BRACKET)
                {
                    return false;
                }
                break;
            case OPERATION:
                if (strcmp(tokenizedFormula.tokens[ind], "-"))
                {
                    return false;
                }
                if (rightTokenNature == OPERATION || rightTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                break;
            case FUNCTION:
                if (rightTokenNature == OPERAND || rightTokenNature == OPERATION || rightTokenNature == FUNCTION || rightTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                break;
            case OPEN_BRACKET:
                if ((rightTokenNature == OPERATION && strcmp(tokenizedFormula.tokens[ind + 1], "-")) || rightTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                break;
            default:
                return false;
                break;
            }
        }
        else if (ind == tokenizedFormula.numberOfTokens - 1)
        {
            TokenNature leftTokenNature = GetTokenNature(tokenizedFormula.tokens[ind - 1]);

            switch (tokenNature)
            {
            case OPERAND:
                if (leftTokenNature == OPERAND || leftTokenNature == FUNCTION || leftTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                break;
            case CLOSED_BRACKET:
                if (leftTokenNature == OPERATION || leftTokenNature == FUNCTION || leftTokenNature == OPEN_BRACKET)
                {
                    return false;
                }
                break;
            default:
                return false;
                break;
            }
        }
        else
        {
            TokenNature leftTokenNature = GetTokenNature(tokenizedFormula.tokens[ind - 1]);
            TokenNature rightTokenNature = GetTokenNature(tokenizedFormula.tokens[ind + 1]);

            switch (tokenNature)
            {
            case OPERAND:
                if (leftTokenNature == OPERAND || leftTokenNature == FUNCTION || leftTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                if (rightTokenNature == OPERAND || rightTokenNature == FUNCTION || rightTokenNature == OPEN_BRACKET)
                {
                    return false;
                }
                break;
            case OPERATION:
                if (leftTokenNature == OPERATION || leftTokenNature == FUNCTION || (leftTokenNature == OPEN_BRACKET && strcmp(tokenizedFormula.tokens[ind], "-")))
                {
                    return false;
                }
                if (rightTokenNature == OPERATION || rightTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                break;
            case FUNCTION:
                if (leftTokenNature == OPERAND || leftTokenNature == FUNCTION || leftTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                if (rightTokenNature == OPERAND || rightTokenNature == OPERATION || rightTokenNature == FUNCTION || rightTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                break;
            case OPEN_BRACKET:
                if (leftTokenNature == OPERAND || leftTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                if ((rightTokenNature == OPERATION && strcmp(tokenizedFormula.tokens[ind + 1], "-")) || rightTokenNature == CLOSED_BRACKET)
                {
                    return false;
                }
                break;
            case CLOSED_BRACKET:
                if (leftTokenNature == OPERATION || leftTokenNature == FUNCTION || leftTokenNature == OPEN_BRACKET)
                {
                    return false;
                }
                if (tokenNature == OPERAND || rightTokenNature == FUNCTION || rightTokenNature == OPEN_BRACKET)
                {
                    return false;
                }
                break;
            default:
                break;
            }
        }
    }

    return true;
}

int GetTheNumberOfTokensFromAnExpression(char expression[MAX_ArrayOOFLen], char delimiters[MAX_ArrayOOFLen])
{
    char expressionCopy[MAX_ArrayOOFLen];
    strcpy(expressionCopy, expression);
    int number = 0;
    char* p = strtok(expressionCopy, delimiters);
    while (p)
    {
        number++;
        p = strtok(NULL, delimiters);
    }

    return number;
}

int GetTheNumberOfApparitionsOfACharacterInAToken(char token[MAX_ArrayOOFLen], char character)
{
    int number = 0;
    for (int ind = 0; ind < strlen(token); ind++)
        if (token[ind] == character)
            number++;

    return number;
}

void GetASpecificTokenFromAnExpression(char expression[MAX_ArrayOOFLen], char delimiters[MAX_ArrayOOFLen], int index, char specificToken[MAX_ArrayOOFLen])
{
    char expressionCopy[MAX_ArrayOOFLen];
    strcpy(expressionCopy, expression);
    int number = 0;
    char* p = strtok(expressionCopy, delimiters);
    while (p)
    {
        number++;

        if (number == index)
        {
            strcpy(specificToken, p);
            return;
        }

        p = strtok(NULL, delimiters);
    }
}

bool IsTokenANumber(char token[MAX_ArrayOOFLen])
{
    if (strlen(token))
        if (IsASyntacticallyCorrectOperand(token) && strchr(validCharactersForAFormula_2, token[strlen(token) - 1]))
            return true;

    return false;
}

bool IsTokenANaturalNumber(char token[MAX_ArrayOOFLen])
{
    if (!IsTokenANumber(token))
        return false;

    if (token[0] == '-')
        return false;

    bool test = true;
    for (int ind = 0; ind < strlen(token); ind++)
        if (token[ind] == '.')
            test = false;
        else if (!test && token[ind] != '0')
            return false;

    return true;
}

bool IsTokenANegativeNumber(char token[MAX_ArrayOOFLen])
{
    if (IsTokenANumber(token))
        if (token[0] == '-')
            return true;

    return false;
}

bool IsTokenADigit(char token[MAX_ArrayOOFLen], char digit)
{
    if (!strlen(token))
        return false;

    if (token[0] != digit)
        return false;

    if (strlen(token) > 1)
        if (token[1] != '.' || (token[0] == '0' && GetTheNumberOfApparitionsOfACharacterInAToken(token, '0') != strlen(token) - 1) || (token[0] != '0' && GetTheNumberOfApparitionsOfACharacterInAToken(token, '0') != strlen(token) - 2))
            return false;

    return true;
}

bool IsAFormulaThatCorrectlyUsesTheDiverseMathematicalFunctions(char formula[MAX_ArrayCFormLen])
{
    TokenizedFormula tokenizedFormula;
    TokenizeAFormula(formula, tokenizedFormula);

    for (int ind = 0; ind < tokenizedFormula.numberOfTokens; ind++)
        if (DoesItContainsASpecificArrayWithNumberOfCharacters(diverseMathematicalFunctions, 7, tokenizedFormula.tokens[ind], 3))
        {
            if (!strncmp(tokenizedFormula.tokens[ind], "log", 3) || !strncmp(tokenizedFormula.tokens[ind], "rad", 3))
            {
                char delimiters[MAX_ArrayOOFLen] = "_";
                if (GetTheNumberOfTokensFromAnExpression(tokenizedFormula.tokens[ind], delimiters) != 2 || GetTheNumberOfApparitionsOfACharacterInAToken(tokenizedFormula.tokens[ind], '_') != 1)
                    return false;

                char specificToken[MAX_ArrayOOFLen];
                GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 2, specificToken);
                if (!IsASyntacticallyCorrectOperand(specificToken) || IsTokenANegativeNumber(specificToken))
                    return false;

                if (IsTokenADigit(specificToken, '0') || IsTokenADigit(specificToken, '1'))
                    return false;

                if (!strncmp(tokenizedFormula.tokens[ind], "rad", 3))
                    if (!IsTokenANaturalNumber(specificToken))
                        return false;
            }
            else if (!strncmp(tokenizedFormula.tokens[ind], "sum", 3) || !strncmp(tokenizedFormula.tokens[ind], "mul", 3))
            {
                char delimiters[MAX_ArrayOOFLen] = "_";
                if (GetTheNumberOfTokensFromAnExpression(tokenizedFormula.tokens[ind], delimiters) != 4 || GetTheNumberOfApparitionsOfACharacterInAToken(tokenizedFormula.tokens[ind], '_') != 3)
                    return false;

                char specificToken[MAX_ArrayOOFLen];
                GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 2, specificToken);
                if (!IsASyntacticallyCorrectOperand(specificToken) || IsTokenANumber(specificToken))
                    return false;
                if (specificToken[0] == '-')
                    return false;

                GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 3, specificToken);
                if (!IsASyntacticallyCorrectOperand(specificToken))
                    return false;

                GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 4, specificToken);
                if (!IsASyntacticallyCorrectOperand(specificToken))
                    return false;
            }
            else if (!strncmp(tokenizedFormula.tokens[ind], "lim", 3))
            {
                char delimiters[MAX_ArrayOOFLen] = "_";
                if (GetTheNumberOfTokensFromAnExpression(tokenizedFormula.tokens[ind], delimiters) != 3 || GetTheNumberOfApparitionsOfACharacterInAToken(tokenizedFormula.tokens[ind], '_') != 2)
                    return false;

                char specificToken[MAX_ArrayOOFLen];
                GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 2, specificToken);
                if (!IsASyntacticallyCorrectOperand(specificToken) || IsTokenANumber(specificToken))
                    return false;
                if (specificToken[0] == '-')
                    return false;

                GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 3, specificToken);
                if (!IsASyntacticallyCorrectOperand(specificToken))
                    return false;

            }
            else if (!strncmp(tokenizedFormula.tokens[ind], "der", 3))
            {
                char delimiters[MAX_ArrayOOFLen] = "_";
                if (GetTheNumberOfTokensFromAnExpression(tokenizedFormula.tokens[ind], delimiters) != 2 || GetTheNumberOfApparitionsOfACharacterInAToken(tokenizedFormula.tokens[ind], '_') != 1)
                    return false;

                char specificToken[MAX_ArrayOOFLen];
                GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 2, specificToken);
                if (!IsASyntacticallyCorrectOperand(specificToken) || IsTokenANumber(specificToken))
                    return false;
                if (specificToken[0] == '-')
                    return false;
            }
            else
            {
                char delimiters[MAX_ArrayOOFLen] = "_";
                char specificToken[MAX_ArrayOOFLen];
                if (GetTheNumberOfTokensFromAnExpression(tokenizedFormula.tokens[ind], delimiters) == 2 && GetTheNumberOfApparitionsOfACharacterInAToken(tokenizedFormula.tokens[ind], '_') == 1)
                {
                    GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 2, specificToken);
                    if (!IsASyntacticallyCorrectOperand(specificToken) || IsTokenANumber(specificToken))
                        return false;
                    if (specificToken[0] == '-')
                        return false;
                }
                else if (GetTheNumberOfTokensFromAnExpression(tokenizedFormula.tokens[ind], delimiters) == 4 && GetTheNumberOfApparitionsOfACharacterInAToken(tokenizedFormula.tokens[ind], '_') == 3)
                {
                    GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 2, specificToken);
                    if (!IsASyntacticallyCorrectOperand(specificToken))
                        return false;

                    GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 3, specificToken);
                    if (!IsASyntacticallyCorrectOperand(specificToken))
                        return false;

                    GetASpecificTokenFromAnExpression(tokenizedFormula.tokens[ind], delimiters, 4, specificToken);
                    if (!IsASyntacticallyCorrectOperand(specificToken) || IsTokenANumber(specificToken))
                        return false;
                    if (specificToken[0] == '-')
                        return false;
                }
                else
                    return false;
            }
        }

    return true;
}

bool IsASyntacticallyCorrectFormula_1(char formula[MAX_ArrayCFormLen])
{
    if (!IsAFormulaThatRespectsTheLimitationsInOperandsLength(formula))
        return false;

    return true;
}

bool IsASyntacticallyCorrectFormula_2(char formula[MAX_ArrayCFormLen])
{
    if (!IsAFormulaThatOnlyContainsValidCharacters(formula) || !IsACorrectlyBracketedFormula(formula) || !IsAFormulaThatOnlyContainsValidOperands(formula) || !IsAFormulaThatRespectsTheOperandsAndOperationsPrinciples(formula) || !IsAFormulaThatCorrectlyUsesTheDiverseMathematicalFunctions(formula))
        return false;

    return true;
}

int GetTokenPriority(char token[MAX_ArrayOOFLen])
{
    if (DoesItContainsASpecificArray(firstDegreeOperations, 2, token))
        return 1;

    if (DoesItContainsASpecificArray(secondDegreeOperations, 2, token))
        return 2;

    if (DoesItContainsASpecificArray(thirdDegreeOperations, 2, token))
        return 3;

    if (GetTokenNature(token) == FUNCTION)
        return 4;

    if (DoesItContainsASpecificArray(brackets, 2, token))
        return 99;

    return -1;
}

void CreateThePostfixedFormOfAFormulaFromTheUnfixedForm(char formula[MAX_ArrayCFormLen], TokenizedFormula& postfixedForm)
{
    TokenizedFormula tokenizedFormula;
    TokenizeAFormula(formula, tokenizedFormula);

    queue<char*> unfixed;
    for (int ind = 0; ind < tokenizedFormula.numberOfTokens; ind++)
        unfixed.push((char*)tokenizedFormula.tokens[ind]);

    queue<char*> postfixed;
    stack<char*> S;

    while (!unfixed.empty())
    {
        char* token = new char[MAX_ArrayOOFLen];
        strcpy(token, (const char*)unfixed.front());
        unfixed.pop();
        if (GetTokenNature(token) == OPERAND)
        {
            postfixed.push(token);
        }
        else
        {
            if (GetTokenNature(token) == CLOSED_BRACKET)
            {
                while (GetTokenNature(S.top()) != OPEN_BRACKET)
                {
                    postfixed.push(S.top());
                    S.pop();
                }
                S.pop();
            }
            else
            {
                bool test = true;
                while (!S.empty() && test)
                {
                    if (GetTokenNature(S.top()) != OPEN_BRACKET && GetTokenPriority(S.top()) >= GetTokenPriority(token))
                    {
                        postfixed.push(S.top());
                        S.pop();
                    }
                    else
                        test = false;
                }
                S.push(token);
            }
        }
    }

    while (!S.empty())
    {
        postfixed.push(S.top());
        S.pop();
    }

    while (!postfixed.empty())
    {
        strcpy(postfixedForm.tokens[postfixedForm.numberOfTokens++], postfixed.front());
        postfixed.pop();
    }
}

void RemoveUnnecessaryZerosFromANumber(char token[MAX_ArrayOOFLen])
{
    if (!IsTokenANumber(token))
        return;

    if (!strchr(token, '.'))
        return;

    int length = strlen(token);
    for (int ind = length - 1; ind >= 0; ind--)
        if (token[ind] == '0')
            token[ind] = '\0';
        else if (token[ind] == '.')
        {
            token[ind] = '\0';
            return;
        }
        else
            return;
}

void _CreateTheExpressionTreeOfAFormula(TokenizedFormula postfixedForm, int& ind, Node*& node)
{
    if (ind >= 0)
        if (strcmp(postfixedForm.tokens[ind], "NULL"))
        {
            node = new Node;

            if (DoesItContainsASpecificArrayWithNumberOfCharacters(diverseMathematicalFunctions, 7, postfixedForm.tokens[ind], 3))
            {
                strncpy(node->information, postfixedForm.tokens[ind], 3);
                node->information[3] = '\0';

                char information[MAX_ArrayOOFLen] = "";

                char delimiters[MAX_ArrayOOFLen] = "_";
                int numberOfTokens = GetTheNumberOfTokensFromAnExpression(postfixedForm.tokens[ind], delimiters);
                char specificToken[MAX_ArrayOOFLen];
                for (int index = 2; index <= numberOfTokens; index++)
                {
                    GetASpecificTokenFromAnExpression(postfixedForm.tokens[ind], delimiters, index, specificToken);
                    RemoveUnnecessaryZerosFromANumber(specificToken);
                    strcat(information, specificToken);
                    strcat(information, "_");
                }
                information[strlen(information) - 1] = '\0';

                Node* left = new Node;
                strcpy(left->information, information);
                node->left = left;
            }
            else
            {
                strcpy(node->information, postfixedForm.tokens[ind]);
                RemoveUnnecessaryZerosFromANumber(node->information);
            }

            TokenNature tokenNature = GetTokenNature(postfixedForm.tokens[ind]);
            ind--;
            if (tokenNature == OPERATION || tokenNature == FUNCTION)
            {
                _CreateTheExpressionTreeOfAFormula(postfixedForm, ind, node->right);
                if (tokenNature == OPERATION)
                    _CreateTheExpressionTreeOfAFormula(postfixedForm, ind, node->left);
            }
        }
        else
            ind--;
}

void CreateTheExpressionTreeOfAFormula(char formula[MAX_ArrayCFormLen], Node*& node)
{
    TokenizedFormula postfixedForm;
    CreateThePostfixedFormOfAFormulaFromTheUnfixedForm(formula, postfixedForm);

    int ind = postfixedForm.numberOfTokens - 1;
    _CreateTheExpressionTreeOfAFormula(postfixedForm, ind, node);
}

void Preordine(Node* R)
{
    if (R)
    {
        cout << R->information << ' ';
        Preordine(R->left);
        Preordine(R->right);
    }
}

double GetNumberFromToken(char token[MAX_ArrayOOFLen])
{
    if (token == NULL)
        return 0;

    double number = 0;

    int indStart = 0;
    if (IsTokenANegativeNumber(token))
        indStart++;

    int ind = indStart;
    for (; ind < strlen(token) && token[ind] != '.'; ind++)
        number = number * 10 + (token[ind] - '0');

    if (ind < strlen(token))
    {
        ind++;
        double power = 10;
        for (; ind < strlen(token); ind++)
        {
            number = number + (double)(token[ind] - '0') / power;
            power *= 10;
        }
    }

    if (indStart)
        number *= -1;

    return number;
}

double CalculateExpressionTreeValue(Node* R)
{
    if (R == NULL)
        return 0;

    if (!strcmp(R->information, "+"))
        return CalculateExpressionTreeValue(R->left) + CalculateExpressionTreeValue(R->right);
    else if (!strcmp(R->information, "-"))
        return CalculateExpressionTreeValue(R->left) - CalculateExpressionTreeValue(R->right);
    else if (!strcmp(R->information, "*"))
        return CalculateExpressionTreeValue(R->left) * CalculateExpressionTreeValue(R->right);
    else if (!strcmp(R->information, "/"))
        return CalculateExpressionTreeValue(R->left) / CalculateExpressionTreeValue(R->right);
    else if (!strcmp(R->information, "^"))
        return pow(CalculateExpressionTreeValue(R->left), CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "%"))
        return CalculateExpressionTreeValue(R->left) / 100. * CalculateExpressionTreeValue(R->right);
    else if (!strcmp(R->information, "abs"))
        return abs(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "lg"))
        return log10(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "ln"))
        return log(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "sqrt"))
        return sqrt(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "sin"))
        return sin(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "cos"))
        return cos(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "tan"))
        return tan(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "asin"))
        return asin(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "acos"))
        return acos(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "atan"))
        return atan(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "sinh"))
        return sinh(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "cosh"))
        return cosh(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "tanh"))
        return tanh(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "asinh"))
        return asinh(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "acosh"))
        return acosh(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "atanh"))
        return atanh(CalculateExpressionTreeValue(R->right));
    else if (!strcmp(R->information, "pi"))
        return pi;
    else if (!strcmp(R->information, "e"))
        return e;
    else
        return GetNumberFromToken(R->information);
}

void ExpressionTreeContainsOnlyEligibleFunctionsAndConstants(Node* R, bool& test)
{
    if (R == NULL)
        return;

    if (!(DoesItContainsASpecificArray(firstDegreeOperations, 2, R->information) || DoesItContainsASpecificArray(secondDegreeOperations, 2, R->information) || DoesItContainsASpecificArray(thirdDegreeOperations, 2, R->information) || DoesItContainsASpecificArray(basicMathematicalFunctions_1, 1, R->information) || DoesItContainsASpecificArray(basicMathematicalFunctions_2, 2, R->information) || DoesItContainsASpecificArray(basicMathematicalFunctions_3, 1, R->information) || DoesItContainsASpecificArray(basicTrigonometricFunctions, 3, R->information) || DoesItContainsASpecificArray(inverseBasicTrigonometricFunctions, 3, R->information) || DoesItContainsASpecificArray(hyperbolicTrigonometricFunctions, 3, R->information) || DoesItContainsASpecificArray(inverseHyperbolicTrigonometricFunctions, 3, R->information) || IsTokenANumber(R->information) || !strcmp(R->information, "pi") || !strcmp(R->information, "e")))
        test = false;

    ExpressionTreeContainsOnlyEligibleFunctionsAndConstants(R->left, test);
    ExpressionTreeContainsOnlyEligibleFunctionsAndConstants(R->right, test);
}

void IsAnExpressionTreeThatRespectsTheBasicMathematicalPrinciples(Node* R, bool& test)
{
    if (R == NULL)
        return;

    IsAnExpressionTreeThatRespectsTheBasicMathematicalPrinciples(R->left, test);
    IsAnExpressionTreeThatRespectsTheBasicMathematicalPrinciples(R->right, test);

    if (!test)
        return;

    bool verify = true;
    ExpressionTreeContainsOnlyEligibleFunctionsAndConstants(R->right, verify);
    if (!verify)
        return;

    if (!strcmp(R->information, "/"))
    {
        if (CalculateExpressionTreeValue(R->right) == 0)
            test = false;
    }
    else if (!strcmp(R->information, "ln"))
    {
        if (CalculateExpressionTreeValue(R->right) <= 0)
            test = false;
    }
    else if (!strcmp(R->information, "lg"))
    {
        if (CalculateExpressionTreeValue(R->right) <= 0)
            test = false;
    }
    else if (!strcmp(R->information, "log"))
    {
        if (CalculateExpressionTreeValue(R->right) <= 0)
            test = false;
    }
    else if (!strcmp(R->information, "tan"))
    {
        if (cos(CalculateExpressionTreeValue(R->right)) == 0)
            test = false;
    }
    else if (!strcmp(R->information, "asin"))
    {
        if (CalculateExpressionTreeValue(R->right) < -1. || CalculateExpressionTreeValue(R->right) > 1)
            test = false;
    }
    else if (!strcmp(R->information, "acos"))
    {
        if (CalculateExpressionTreeValue(R->right) < -1 || CalculateExpressionTreeValue(R->right) > 1)
            test = false;
    }
    else if (!strcmp(R->information, "acosh"))
    {
        if (CalculateExpressionTreeValue(R->right) < 1)
            test = false;
    }
    else if (!strcmp(R->information, "atanh"))
    {
        if (CalculateExpressionTreeValue(R->right) <= -1 || CalculateExpressionTreeValue(R->right) >= 1)
            test = false;
    }
}

void IsAFormulaThatRespectsTheBasicMathematicalPrinciples(char formula[MAX_ArrayCFormLen], bool& test)
{
    Node* R = new Node;
    CreateTheExpressionTreeOfAFormula(formula, R);
    IsAnExpressionTreeThatRespectsTheBasicMathematicalPrinciples(R, test);
}

bool IsASyntacticallyCorrectFormula_3(char formula[MAX_ArrayCFormLen])
{
    bool test = true;
    IsAFormulaThatRespectsTheBasicMathematicalPrinciples(formula, test);
    return test;
}

/////////////////////////////////////////////////////////////////////

//this function verify if an operator need only one variable
bool has_one_variable(char x[NODE_INFO_MAX_LENGTH])
{
    for (int i = 0; i < 7; ++i)   //we verify if the root has only one child, therefore it will be one of onevariableoperator's string
    {
        if (!(strcmp(onevariableoperator[i], x))) //if it has one son
            return true;
    }
    return false;
}

//this function verify if an operator needs two variables
bool has_two_variables(char x[NODE_INFO_MAX_LENGTH])
{
    for (int i = 0; i < 29; ++i)   //we verify if the root has only one child, therefore it will be one of onevariableoperator's string
    {
        if (!(strcmp(twovariablesoperator[i], x))) //if it has one son
            return true;
    }
    return false;
}

//this function verify if an operator is radical of an order>2,logarithm with a base different than 10 or e, a limit,a derivate or an integral  
bool its_special_operator(char x[NODE_INFO_MAX_LENGTH])
{
    for (int i = 0; i < 8; ++i)
    {
        if (!strcmp(specialoperator[i], x))
            return true;
    }
    return false;
}

//the maximum between two values
int maxim(int a, int b)
{
    int maxi;
    ((a > b) ? maxi = a : maxi = b);
    return maxi;
}

//it counts the levels of the tree
int count_levels(Node* root)
{
    if (root == NULL)
        return 0;

    int left = count_levels(root->left);
    int right = count_levels(root->right);
    return 1 + maxim(left, right);//1 + the maximum between the left and right subtree

}

//a function for drawing the information of a node in his visual representation
void print_text_inside_node(int x, int y, char exp[NODE_INFO_MAX_LENGTH])
{
    x -= textwidth(exp) / 2;
    y -= textheight(exp) / 2;

    outtextxy(x, y, (char*)exp);
}

//a function for the drawing of a node
void draw_node(int x, int y, Node* root)
{
    char exp[NODE_INFO_MAX_LENGTH];
    strcpy(exp, root->information);

    setcolor(WHITE);//the circle's color

    setfillstyle(SOLID_FILL, BLACK);//the fill color of the elipse

    //float dimensionModifier = 1.5;
    int textWidth = textwidth((char*)exp);//the width of the expression that will be in the node
    int textHeight = textheight((char*)exp);//the height of the expression

    float dimensionModifier = 1.2;
    textWidth /= dimensionModifier;
    textHeight *= dimensionModifier;

    if (textWidth < textHeight)
        textWidth = textHeight;
   

    fillellipse(x, y, textWidth, textHeight);

    print_text_inside_node(x, y, exp);
}

//a function that draws the entire tree
//basically we make a preorder traversal
void draw_tree(Node* root, int x, int y, int tree_width_between_nodes, int tree_height_between_nodes)
{
    if (!root)
        return;

    draw_node(x, y, root);

    int node_text_height = textheight(root->information);
    float dimensionModifier = 1.2;
    node_text_height *= dimensionModifier;

    if (root->left == NULL && root->right != NULL)
    {
        line(x, y + node_text_height, x, y + tree_height_between_nodes);
        draw_tree(root->right, x, y + tree_height_between_nodes, tree_width_between_nodes / 2, tree_height_between_nodes);
    }
    else if (root->left != NULL && root->right != NULL)
    {
        line(x, y + node_text_height, x - tree_width_between_nodes / 2, y + tree_height_between_nodes);
        line(x, y + node_text_height, x + tree_width_between_nodes / 2, y + tree_height_between_nodes);
        draw_tree(root->left, x - tree_width_between_nodes / 2, y + tree_height_between_nodes, tree_width_between_nodes / 2, tree_height_between_nodes);
        draw_tree(root->right, x + tree_width_between_nodes / 2, y + tree_height_between_nodes, tree_width_between_nodes / 2, tree_height_between_nodes);
    }
}

//the width and height of the tree, and also the dimensions of the window
void calculate_width_height_of_the_tree(Node* root, int& windowWidth, int& windowHeight, int& treeWidth, int& treeHeight)//treewidth and treeheight are the width and the height between nodes
{
    if (!root)
    {
        windowWidth = 0;
        windowHeight = 0;
        treeWidth = 0;
        treeHeight = 0;
        return;
    }

    int number_of_levels = count_levels(root);
    int last_level = pow(2, number_of_levels - 1);//the number of nodes on the last level in a complete binary tree
    treeWidth = last_level * 50+5;//we choose the width between the nodes reporting at the last level
    treeHeight = number_of_levels * 50+5;//this is the height of letter "M",which is one of the largest
    windowWidth = last_level * 100+5;//we consider the last_level
    windowHeight = number_of_levels * 200+5;
}

//it verifies if an expression needs brackets
void DoesAnExpressionNeedToBeBracketed(Node* R, Node* r, bool& test)
{
    if (r != NULL)
        if (GetTokenNature(r->information) == OPERATION)
            if (GetTokenPriority(r->information) < GetTokenPriority(R->information))
                test = true;
}


//it calculates the width of an expression, considering the brackets, which we need in order to calculate the sons of an operator or a function
int GetExpressionWidth(Node* R)
{
    if (R == NULL)
        return 0;

    int bracketsWidth = 0;

    if (GetTokenNature(R->information) == FUNCTION)  //for functions we need brackets
        bracketsWidth = textwidth((char*)"(") + textwidth((char*)")");
    else if (!strcmp(R->information, "-"))
    {
        if (R->left == NULL)//the case of unary minus
        {
            bool test = false;
            DoesAnExpressionNeedToBeBracketed(R, R->right, test);//it verifies if the expression needs brackets
            if (test)
                bracketsWidth = textwidth((char*)"(") + textwidth((char*)")");
        }
    }
    else if (GetTokenNature(R->information) == OPERATION)//in this case we need to verify if each of the sons needs brackets
    {
        bool test = false;
        DoesAnExpressionNeedToBeBracketed(R, R->left, test);
        if (test)
            bracketsWidth = bracketsWidth + textwidth((char*)"(") + textwidth((char*)")");

        test = false;
        DoesAnExpressionNeedToBeBracketed(R, R->right, test);
        if (test)
            bracketsWidth = bracketsWidth + textwidth((char*)"(") + textwidth((char*)")");
    }

    if (!strcmp(R->information, "sqrt"))
        return textwidth((char*)"I") * 2 + GetExpressionWidth(R->left) + GetExpressionWidth(R->right);//I is the tallest letter

    if (!strcmp(R->information, "/"))
        return max(GetExpressionWidth(R->left), GetExpressionWidth(R->right));

    if (!strcmp(R->information, "^"))
        return bracketsWidth + GetExpressionWidth(R->left) + GetExpressionWidth(R->right);

    return textwidth(R->information) + bracketsWidth + GetExpressionWidth(R->left) + GetExpressionWidth(R->right);
}

//it calculates the number of operations that will displace the height of the expression
int GetNumberOfHeightDisplacementOperations(Node* R)
{
    if (R == NULL)
        return 0;

    if (!strcmp(R->information, "/") || !strcmp(R->information, "^"))
        return 1 + GetNumberOfHeightDisplacementOperations(R->left) + GetNumberOfHeightDisplacementOperations(R->right);

    return GetNumberOfHeightDisplacementOperations(R->left) + GetNumberOfHeightDisplacementOperations(R->right);
}

//the visual height of an expression
int GetExpressionHeight(Node* R)
{
    if (R == NULL)
        return 0;

    return (GetNumberOfHeightDisplacementOperations(R) + 1) * textheight((char*)"M");
}


//the sigma symbol for sums
void draw_sigma(int x_start, int y_start, int width, int height)
{
    line(x_start, y_start, x_start + width, y_start);
    line(x_start, y_start, x_start + width / 2, y_start - height / 2);
    line(x_start, y_start - height, x_start + width / 2, y_start - height / 2);
    line(x_start, y_start - height, x_start + width, y_start - height);
}

//the symbol for multiplication
void draw_phi(int x_start, int y_start, int width, int height)
{
    line(x_start, y_start, x_start, y_start - height);
    line(x_start, y_start - height, x_start + width, y_start - height);
    line(x_start + width, y_start - height, x_start + width, y_start);
}

//the symbol of arrow from the limit
void draw_arrow(int x_start, int y_start, int width)
{
    line(x_start, y_start, x_start + width, y_start);
    line(x_start + width, y_start, x_start + width / 2, y_start - 5);
    line(x_start + width, y_start, x_start + width / 2, y_start + 5);
}

//the symbol for integral
void draw_integral(int x_start, int y_start, int height)
{
    line(x_start + 5, y_start - height / 2, x_start + 10, y_start - height / 2);
    line(x_start - 5, y_start + height, x_start + 5, y_start - height / 2);
    line(x_start - 5, y_start + height, x_start - 10, y_start + height);
}

//it draws an expression
void DrawAnExpression(Node* R, int x_position, int y_position)
{
    if (R == NULL)
        return;

    if (!strcmp(R->information, "sqrt"))
    {
        int squareRootDrawingDimensionReference = textwidth((char*)"I");
        int squareRootHeight = textheight((char*)"I");
        DrawAnExpression(R->right, x_position + squareRootDrawingDimensionReference * 2 + GetExpressionWidth(R->right->left), y_position);//it draws the right son, under the radical

        line(x_position, y_position + GetExpressionHeight(R->right) / 2, x_position + squareRootDrawingDimensionReference, y_position + GetExpressionHeight(R->right));//draws the short vertical segment at the left-bottom of the square root symbol
        line(x_position + squareRootDrawingDimensionReference, y_position + GetExpressionHeight(R->right), x_position + squareRootDrawingDimensionReference * 2, y_position - GetExpressionHeight(R->right) / 2);//draws the slanted line connecting the vertical line to the horizontal line of the radical
        line(x_position + squareRootDrawingDimensionReference * 2, y_position - GetExpressionHeight(R->right) / 2, x_position + squareRootDrawingDimensionReference * 3 + GetExpressionWidth(R->right), y_position - GetExpressionHeight(R->right) / 2);//the line from above
    }
    else if (!strcmp(R->information, "rad"))
    {
        int squareRootDrawingDimensionReference = textwidth((char*)"I");
        int squareRootHeight = textheight((char*)"I");
        int textmaxwidth = textwidth((char*)"M");
        int textmaxheight = textheight((char*)"M");
        int scaledTextHeight = textmaxheight * 0.7;
        int scaledTextWidth = textmaxwidth * 0.7;
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, scaledTextHeight / scaledTextWidth);//the order of the radical should be smaller than the base
        DrawAnExpression(R->left, x_position - GetExpressionWidth(R->left) + GetExpressionWidth(R->left->left), y_position - textmaxheight / 2);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
        DrawAnExpression(R->right, x_position + squareRootDrawingDimensionReference * 2 + GetExpressionWidth(R->right->left), y_position);//it draws the right son, under the radical
        line(x_position, y_position + GetExpressionHeight(R->right) / 2, x_position + squareRootDrawingDimensionReference, y_position + GetExpressionHeight(R->right));//draws the short vertical segment at the left-bottom of the square root symbol
        line(x_position + squareRootDrawingDimensionReference, y_position + GetExpressionHeight(R->right), x_position + squareRootDrawingDimensionReference * 2, y_position - GetExpressionHeight(R->right) / 2);//draws the slanted line connecting the vertical line to the horizontal line of the radical
        line(x_position + squareRootDrawingDimensionReference * 2, y_position - GetExpressionHeight(R->right) / 2, x_position + squareRootDrawingDimensionReference * 3 + GetExpressionWidth(R->right), y_position - GetExpressionHeight(R->right) / 2);//the line from above
    }
    else if (!strcmp(R->information, "abs"))
    {
        int expressionheight = GetExpressionHeight(R->right);
        int expressionwidth = GetExpressionWidth(R->right);
        line(x_position, y_position, x_position, y_position + expressionheight);
        DrawAnExpression(R->right, x_position + GetExpressionWidth(R->right->left) + textwidth((char*)"|"), y_position);
        line(x_position + expressionwidth + 2 * textwidth((char*)"|"), y_position, x_position + expressionwidth + 2 * textwidth((char*)"|"), y_position + expressionheight);

    }
    else if (!strcmp(R->information, "/"))
    {
        x_position -= GetExpressionWidth(R->left);
        int maxwidth = max(GetExpressionWidth(R->left), GetExpressionWidth(R->right));
        int minwidth = min(GetExpressionWidth(R->left), GetExpressionWidth(R->right));
        int difference = maxwidth - minwidth;
        difference /= 2;//the distance of the margins of the expression to look simetrical
        if (GetExpressionWidth(R->left) == maxwidth)
        {
            DrawAnExpression(R->left, x_position + GetExpressionWidth(R->left->left), y_position);
            DrawAnExpression(R->right, x_position + GetExpressionWidth(R->right->left) + difference, y_position + GetExpressionHeight(R->left));
        }
        else
        {
            DrawAnExpression(R->left, x_position + GetExpressionWidth(R->left->left) + difference, y_position);
            DrawAnExpression(R->right, x_position + GetExpressionWidth(R->right->left), y_position + GetExpressionHeight(R->left));
        }

        line(x_position, y_position + GetExpressionHeight(R->left), x_position + maxwidth, y_position + GetExpressionHeight(R->left));
    }
    else if (!strcmp(R->information, "^"))
    {
        int textmaxwidth = textwidth((char*)"M");
        int textmaxheight = textheight((char*)"M");
        int scaledTextHeight = textmaxheight * 0.7;
        int scaledTextWidth = textmaxwidth * 0.7;
        DrawAnExpression(R->left, x_position - GetExpressionWidth(R->left) + GetExpressionWidth(R->left->left), y_position);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, scaledTextHeight / scaledTextWidth);//the exponent should be smaller than the base
        DrawAnExpression(R->right, x_position + GetExpressionWidth(R->right->left), y_position - GetExpressionHeight(R->right) / 2);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
    }
    else if (!strcmp((R->information), "log"))
    {
        int textmaxwidth = textwidth((char*)"M");
        int textmaxheight = textheight((char*)"M");
        int scaledTextHeight = textmaxheight * 0.7;
        int scaledTextWidth = textmaxwidth * 0.7;
        outtextxy(x_position, y_position, R->information);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, scaledTextHeight / scaledTextWidth);//the base should be smaller than the argument
        DrawAnExpression(R->left, x_position + GetExpressionWidth(R->left->left) + textwidth((char*)"log") + 5, y_position + textheight((char*)"log"));
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
        outtextxy(x_position + textwidth((char*)"log") + 5, y_position, (char*)"(");
        DrawAnExpression(R->right, x_position + textwidth((char*)"log") + textwidth((char*)"(") + GetExpressionWidth(R->right->left) + 5, y_position);
        outtextxy(x_position + textwidth((char*)"log") + GetExpressionWidth(R->right) + textwidth((char*)"(") + 5, y_position, (char*)")");
    }
    else if (!strcmp((R->information), "sum"))
    {
        //we will split every component of the left son
        char var1[NODE_INFO_MAX_LENGTH], var2[NODE_INFO_MAX_LENGTH], var3[NODE_INFO_MAX_LENGTH];
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 1, var1);
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 2, var2);
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 3, var3);
        int height = GetExpressionHeight(R->right);
        height /= 2;
        int length1 = textwidth((char*)var1);
        int lenght2 = textwidth((char*)var2);
        int length3 = textwidth((char*)var3);
        int lengthequal = textwidth((char*)"=");
        //we will concatenate var1, = and var2
        char subscript[3 * NODE_INFO_MAX_LENGTH];
        strcpy(subscript, var1);
        strcat(subscript, (char*)"=");
        strcat(subscript, var2);
        subscript[strlen(subscript)] = NULL;
        int maxwidth = maxim(length3, length1 + lenght2 + lengthequal);
        int minwidth = min(length3, length1 + lenght2 + lengthequal);
        int difference = maxwidth - minwidth;
        difference /= 2;//the indexes will look simetrical
        draw_sigma(x_position, y_position + height, maxwidth, height);
        if (length3 == maxwidth) {
            outtextxy(x_position + difference, y_position + 1.5 * height, subscript);//the subscript
            outtextxy(x_position, y_position - 2.5 * height, var3);//the superscript
        }
        else if (length3 == minwidth)
        {
            outtextxy(x_position, y_position + 1.5 * height, subscript);//the subscript
            outtextxy(x_position + difference, y_position - 2.5 * height, var3);//the superscript
        }
        outtextxy(x_position + maxwidth, y_position - height / 2, (char*)"(");
        DrawAnExpression(R->right, x_position + maxwidth + textwidth((char*)"(") + GetExpressionWidth(R->right->left), y_position - height / 2);
        outtextxy(x_position + maxwidth + GetExpressionWidth(R->right) + textwidth((char*)"("), y_position - height / 2, (char*)")");
    }
    else if (!strcmp((R->information), "mul"))
    {
        //we will split every component of the left son
        char var1[NODE_INFO_MAX_LENGTH], var2[NODE_INFO_MAX_LENGTH], var3[NODE_INFO_MAX_LENGTH];
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 1, var1);
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 2, var2);
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 3, var3);
        int height = GetExpressionHeight(R->right);
        height /= 2;
        int length1 = textwidth((char*)var1);
        int lenght2 = textwidth((char*)var2);
        int length3 = textwidth((char*)var3);
        int lengthequal = textwidth((char*)"=");
        //we will concatenate var1, = and var2
        char subscript[3 * NODE_INFO_MAX_LENGTH];
        strcpy(subscript, var1);
        strcat(subscript, (char*)"=");
        strcat(subscript, var2);
        subscript[strlen(subscript)] = NULL;
        int maxwidth = maxim(length3, length1 + lenght2 + lengthequal);
        int minwidth = min(length3, length1 + lenght2 + lengthequal);
        int difference = maxwidth - minwidth;
        difference /= 2;//the indexes will look simetrical
        draw_phi(x_position, y_position, maxwidth, 2 * height);
        if (length3 == maxwidth) {
            outtextxy(x_position + difference, y_position + height / 2, subscript);//the subscript
            outtextxy(x_position, y_position - 4 * height - 5, var3);//the superscript
        }
        else if (length3 == minwidth)
        {
            outtextxy(x_position, y_position + height / 2, subscript);//the subscript
            outtextxy(x_position + difference, y_position - 4 * height - 5, var3);//the superscript
        }
        outtextxy(x_position + maxwidth + 5, y_position - 2 * height, (char*)"(");
        DrawAnExpression(R->right, x_position + maxwidth + textwidth((char*)"(") + GetExpressionWidth(R->right->left) + 5, y_position - 2 * height);
        outtextxy(x_position + maxwidth + GetExpressionWidth(R->right) + textwidth((char*)"(") + 5, y_position - 2 * height, (char*)")");
    }
    else if (!strcmp((R->information), "lim"))
    {
        //we will split every component of the left son
        char var1[NODE_INFO_MAX_LENGTH], var2[NODE_INFO_MAX_LENGTH];
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 1, var1);
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 2, var2);
        int height = textwidth((char*)"lim");
        int length1 = textwidth((char*)var1);
        int lenght2 = textwidth((char*)var2);
        int limlength = textwidth((char*)"lim");
        //20px will be the length of the arrow
        int maxwidth = maxim(limlength, length1 + lenght2 + 20);
        int minwidth = min(limlength, length1 + lenght2 + 20);
        int difference = maxwidth - minwidth;
        difference /= 2;
        if (limlength == maxwidth) {
            outtextxy(x_position, y_position, (char*)"lim");
            outtextxy(x_position + difference, y_position + height, var1);
            draw_arrow(x_position + length1 + difference, y_position + 1.5 * height, 20);
            outtextxy(x_position + length1 + difference + 20, y_position + height, var2);
        }
        else if (limlength == minwidth)
        {
            outtextxy(x_position + difference, y_position, (char*)"lim");
            outtextxy(x_position, y_position + height, var1);
            draw_arrow(x_position + length1, y_position + 1.5 * height, 20);
            outtextxy(x_position + length1 + 20, y_position + height, var2);
        }
        outtextxy(x_position + maxwidth, y_position, (char*)"(");
        DrawAnExpression(R->right, x_position + maxwidth + textwidth((char*)"(") + GetExpressionWidth(R->right->left), y_position);
        outtextxy(x_position + maxwidth + GetExpressionWidth(R->right) + textwidth((char*)"("), y_position, (char*)")");
    }
    else if (!strcmp(R->information, "der"))
    {
        int height = GetExpressionHeight(R->right);
        height /= 2;
        int length1 = textwidth((char*)R->left->information);
        int lengthd = textwidth((char*)"d");
        line(x_position, y_position + height / 2 + 7, x_position + length1 + lengthd, y_position + height / 2 + 7);
        outtextxy(x_position + length1 / 2, y_position - 20, (char*)"d");
        char d[NODE_INFO_MAX_LENGTH];
        strcpy(d, "d");
        strcat(d, R->left->information);
        d[strlen(d)] = NULL;
        outtextxy(x_position, y_position + 20, (char*)d);
        outtextxy(x_position + length1 + lengthd, y_position, (char*)"(");
        DrawAnExpression(R->right, x_position + length1 + lengthd + textwidth((char*)"(") + GetExpressionWidth(R->right->left), y_position);
        outtextxy(x_position + length1 + lengthd + GetExpressionWidth(R->right) + textwidth((char*)"("), y_position, (char*)")");
    }
    else if (!strcmp(R->information, "int"))
    {
        char var1[NODE_INFO_MAX_LENGTH], var2[NODE_INFO_MAX_LENGTH], var3[NODE_INFO_MAX_LENGTH];
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 1, var1);
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 2, var2);
        GetASpecificTokenFromAnExpression(R->left->information, (char*)"_", 3, var3);
        if (!strcmp(var1, R->left->information))
        {
            char d[NODE_INFO_MAX_LENGTH];
            strcpy(d, "d");
            strcat(d, R->left->information);
            d[strlen(d)] = NULL;
            draw_integral(x_position, y_position, 40);
            DrawAnExpression(R->right, x_position + textwidth((char*)"(") + GetExpressionWidth(R->right->left), y_position);
            outtextxy(x_position + GetExpressionWidth(R->right) + 2 * textwidth((char*)"("), y_position, ((char*)d));
        }
        else
        {
            int length1 = textwidth((char*)var1);
            int lenght2 = textwidth((char*)var2);
            char d[NODE_INFO_MAX_LENGTH];
            strcpy(d, "d");
            strcat(d, var3);
            d[strlen(d)] = NULL;
            draw_integral(x_position, y_position, 40);
            outtextxy(x_position, y_position + 40, var1);//the subscript
            outtextxy(x_position + 15, y_position - 40, var2);//the superscript
            DrawAnExpression(R->right, x_position + textwidth((char*)"(") + GetExpressionWidth(R->right->left) + maxim(length1, lenght2), y_position);
            outtextxy(x_position + GetExpressionWidth(R->right) + 2 * textwidth((char*)"(") + maxim(length1, lenght2), y_position, ((char*)d));
        }
    }
    else
    {
        if (GetTokenNature(R->information) == FUNCTION)
        {
            int functionWidth = textwidth(R->information);
            int expressionWidth = GetExpressionWidth(R->right);
            int openBracketWidth = textwidth((char*)"(");
            outtextxy(x_position, y_position, R->information);
            outtextxy(x_position + functionWidth, y_position, (char*)"(");
            outtextxy(x_position + functionWidth + openBracketWidth + expressionWidth, y_position, (char*)")");

            DrawAnExpression(R->right, x_position + functionWidth + openBracketWidth + GetExpressionWidth(R->right->left), y_position);
        }
        else if (!strcmp(R->information, "-") && R->left == NULL)
        {
            int unaryMinusWidth = textwidth(R->information);
            int expressionWidth = GetExpressionWidth(R->right);

            outtextxy(x_position, y_position, R->information);

            int openBracketWidth = textwidth((char*)"(");
            outtextxy(x_position + unaryMinusWidth, y_position, (char*)"(");
            outtextxy(x_position + unaryMinusWidth + openBracketWidth + expressionWidth, y_position, (char*)")");

            DrawAnExpression(R->right, x_position + unaryMinusWidth + openBracketWidth + GetExpressionWidth(R->right->left), y_position);
        }
        else if (GetTokenNature(R->information) == OPERATION)
        {
            int operationWidth = textwidth(R->information);
            int leftExpressionWidth = GetExpressionWidth(R->left);
            int rightExpressionWidth = GetExpressionWidth(R->right);
            int openBracketWidth = textwidth((char*)"(");
            int closedBracketWidth = textwidth((char*)")");
            int textmaxheight = textheight((char*)"M");
            int textmaxwidth = textwidth((char*)"M");
            int fractionh = 0;
            int fractionw = 0;
            if (!strcmp((R->left->information), "/") || !strcmp((R->right->information), "/"))
            {
                fractionh = textmaxheight / 2;
                fractionw = textmaxwidth / 2;
            }
            outtextxy(x_position, y_position + fractionh, R->information);

            bool test = false;
            DoesAnExpressionNeedToBeBracketed(R, R->left, test);
            if (test)
            {
                outtextxy(x_position - openBracketWidth - leftExpressionWidth - closedBracketWidth, y_position, (char*)"(");
                outtextxy(x_position - closedBracketWidth, y_position, (char*)")");
                DrawAnExpression(R->left, x_position - closedBracketWidth - leftExpressionWidth + GetExpressionWidth(R->left->left), y_position);
            }
            else
                DrawAnExpression(R->left, x_position - leftExpressionWidth + GetExpressionWidth(R->left->left), y_position);

            test = false;
            DoesAnExpressionNeedToBeBracketed(R, R->right, test);
            if (test)
            {
                outtextxy(x_position + operationWidth, y_position, (char*)"(");
                outtextxy(x_position + operationWidth + openBracketWidth + rightExpressionWidth, y_position, (char*)")");
                DrawAnExpression(R->right, x_position + operationWidth + openBracketWidth + GetExpressionWidth(R->right->left), y_position);
            }
            else
                DrawAnExpression(R->right, x_position + operationWidth + GetExpressionWidth(R->right->left), y_position);
        }
        else if (!has_one_variable(R->information) && !has_two_variables(R->information))
        {
            outtextxy(x_position, y_position, R->information);
        }
    }
}

////////////////////////////////////////////////////////////////////

void InitializeAButton(Button& button, ButtonsTypes type, int left, int top, int right, int bottom, char text[Max_ArrayButtonTextLen])
{
    button.type = type;
    button.position.left = left;
    button.position.top = top;
    button.position.right = right;
    button.position.bottom = bottom;
    strcpy(button.text, text);
}

void DrawAButton(Button button)
{
    rectangle(button.position.left, button.position.top, button.position.right, button.position.bottom);

    int textStartPositionX = button.position.left, textStartPositionY = button.position.top;
    textStartPositionX += (button.position.right - button.position.left - textwidth(button.text)) / 2;
    textStartPositionY += (button.position.bottom - button.position.top - textheight(button.text)) / 2;
    outtextxy(textStartPositionX, textStartPositionY, button.text);
}

void DrawApplicationStatusButtons()
{
    vector<Button> buttons;

    switch (applicationStatus)
    {
    case MAIN_MENU:
        buttons = mainMenuButtons;
        settextstyle(DEFAULT_FONT, 0, 3);
        break;
    case TYPE_AN_EXPRESSION:
        buttons = typeAnExpressionButtons;
        settextstyle(SANS_SERIF_FONT, 0, 1);
        break;
    case DISPLAY_EXPRESSION:
        buttons = displayExpressionButtons;
        settextstyle(SANS_SERIF_FONT, 0, 1);
        break;
    case DISPLAY_EXPRESSION_TREE:
        buttons = displayExpressionTreeButtons;
        settextstyle(SANS_SERIF_FONT, 0, 1);
        break;
    default:
        break;
    }

    for (int ind = 0; ind < buttons.size(); ind++)
        DrawAButton(buttons[ind]);

    settextstyle(DEFAULT_FONT, 0, 2);
}

void PrintCenteredText(char* text, int y_position, int widthDisplacement)
{
    int width = GetSystemMetrics(SM_CXSCREEN) - widthDisplacement;
    outtextxy((width - textwidth(text)) / 2, y_position, text);
}

void DrawApplicationStatusExtraUI()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    int textWidth = 0;

    int boxWidth = textwidth((char*)"M") * 50 + 20;
    int boxHeight = textheight((char*)"M") * 4 + 20;
    int boxTopY = 50 + 75 + 50;

    switch (applicationStatus)
    {
    case MAIN_MENU:
        outtextxy(25, height - 100, (char*)"Project: Formulas visualiser");
        outtextxy(25, height - 75, (char*)"Team: Iacob David-Gabriel & Buzenchi Paula-Roberta (IB4)");
        outtextxy(25, height - 50, (char*)"All rights reserved");

        textWidth = textwidth((char*)"Project: Formulas visualiser");
        settextstyle(DEFAULT_FONT, 0, 1);
        outtextxy(25 + textWidth, height - 100, (char*)"TM");
        settextstyle(DEFAULT_FONT, 0, 2);
        break;
    case TYPE_AN_EXPRESSION:
        if (showEmptyFormulaMessage)
        {
            PrintCenteredText((char*)"The submitted formula cannot be empty!", 50, 300);
        }
        else if (showSyntacticallyIncorrectFormulaMessage)
        {
            PrintCenteredText((char*)"The submitted formula is syntactically incorrect.", 50, 300);
            PrintCenteredText((char*)"Correct the formula to be eligible and retry submitting.", 75, 300);
        }
        else if (showToLongOperandsMessage)
        {
            PrintCenteredText((char*)"In the submitted formula, some operands are to long.", 50, 300);
            PrintCenteredText((char*)"Modify the operands to be shorter and retry submitting.", 75, 300);
            char text[MAX_ArrayCFormLen] = "The maximum length for an operand: ";

            char numberText[12] = "";
            _itoa(MAX_OprdOrOprtOrFunLen, numberText, 10);
            strcat(text, numberText);

            PrintCenteredText(text, 100, 300);
        }
        else if (showBasicMathematicalPrinciplesBrokenMessage)
        {
            PrintCenteredText((char*)"The submitted formula is syntactically correct.", 50, 300);
            PrintCenteredText((char*)"However, some basic mathematical principles have been broken.", 75, 300);
            PrintCenteredText((char*)"Correct the formula to be eligible and retry submitting.", 100, 300);
        }
        else
        {
            PrintCenteredText((char*)"Use the buttons with letters and symbols on them", 50, 300);
            PrintCenteredText((char*)"to type out a mathematical formula.", 75, 300);
            PrintCenteredText((char*)"When you're done, click the \"SUBMIT\" button.", 100, 300);
        }

        setlinestyle(DASHED_LINE, 0, NORM_WIDTH);
        width -= 300;
        rectangle(width / 2 - boxWidth / 2, boxTopY, width / 2 + boxWidth / 2, boxTopY + boxHeight);
        width += 300;
        line(width - 300, 0, width - 300, height);
        setlinestyle(SOLID_LINE, 0, NORM_WIDTH);

        settextstyle(SANS_SERIF_FONT, 0, 1);

        outtextxy(width - 275, 50, (char*)"Predefined functions:");

        outtextxy(width - 250, 75, (char*)"- abs()");
        outtextxy(width - 250, 100, (char*)"- sqrt()");
        outtextxy(width - 150, 75, (char*)"- lg()");
        outtextxy(width - 150, 100, (char*)"- ln()");

        outtextxy(width - 250, 150, (char*)"- sin()");
        outtextxy(width - 250, 175, (char*)"- cos()");
        outtextxy(width - 250, 200, (char*)"- tan()");

        outtextxy(width - 150, 150, (char*)"- (a)sin(h)()");
        outtextxy(width - 150, 175, (char*)"- (a)cos(h)()");
        outtextxy(width - 150, 200, (char*)"- (a)tan(h)()");

        outtextxy(width - 250, 225, (char*)"* a = arc");
        outtextxy(width - 250, 250, (char*)"* h = hyperbolic");

        outtextxy(width - 275, 300, (char*)"Diverse functions:");

        outtextxy(width - 250, 325, (char*)"- log_con()");
        outtextxy(width - 250, 350, (char*)"- rad_con()");
        outtextxy(width - 150, 325, (char*)"- der_unvar()");
        outtextxy(width - 150, 350, (char*)"- int_unvar()");
        outtextxy(width - 250, 375, (char*)"- sum_unvar_con_con()");
        outtextxy(width - 250, 400, (char*)"- mul_unvar_con_con()");
        outtextxy(width - 250, 425, (char*)"- lim_unvar_con()");
        outtextxy(width - 250, 450, (char*)"- int_con_con_unvar()");
        outtextxy(width - 250, 475, (char*)"* con = proper constant");
        outtextxy(width - 250, 500, (char*)"* var = unnegated variable");


        settextstyle(DEFAULT_FONT, 0, 2);
        break;
    case DISPLAY_EXPRESSION:
        setlinestyle(DASHED_LINE, 0, NORM_WIDTH);
        line(width - 300, 0, width - 300, height);
        setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
        break;
    case DISPLAY_EXPRESSION_TREE:
        setlinestyle(DASHED_LINE, 0, NORM_WIDTH);
        line(width - 300, 0, width - 300, height);
        setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
        break;
    default:
        break;
    }
}

void InitializeMainMenuButtons(vector<Button>& _mainMenuButtons)
{
    int width = GetSystemMetrics(SM_CXSCREEN);

    Button startButton;
    InitializeAButton(startButton, SWITCH_TO_TYPE_AN_EXPRESSION, width / 2 - 175, 150, width / 2 + 175, 225, (char*)"START");
    _mainMenuButtons.push_back(startButton);

    Button exitButton;
    InitializeAButton(exitButton, EXIT, width / 2 - 175, 250, width / 2 + 175, 325, (char*)"EXIT");
    _mainMenuButtons.push_back(exitButton);
}

void InitializeTypeAnExpressionButtons(vector<Button>& _typeAnExpressionButtons)
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    int boxWidth = textwidth((char*)"M") * 50 + 20;
    int boxHeight = textheight((char*)"M") * 4 + 20;

    char firstBlockSymbols[] = "1234567890.+-*/^%()B";
    width -= 300;
    int buttonsStartPositionX = width / 2 - boxWidth / 2;
    width += 300;
    int buttonsStartPositionY = height - 50 - 4 * 50;
    int numberOfColumns = 5;
    for (int ind = 0; ind < strlen(firstBlockSymbols); ind++)
    {
        Button button;
        char text[5] = "";
        ButtonsTypes buttonType = TYPE_A_CHARACTER;
        if (firstBlockSymbols[ind] == 'B')
        {
            strcpy(text, "<---");
            buttonType = REMOVE_A_CHARACTER;
        }
        else
        {
            text[0] = firstBlockSymbols[ind];
            text[1] = '\0';
        }
        InitializeAButton(button, buttonType, buttonsStartPositionX + 50 * (ind % numberOfColumns), buttonsStartPositionY + 50 * (ind / numberOfColumns), buttonsStartPositionX + 50 * (ind % numberOfColumns) + 49, buttonsStartPositionY + 50 * (ind / numberOfColumns) + 49, text);
        _typeAnExpressionButtons.push_back(button);
    }

    char secondBlockSymbols[] = "abcdefghijklmnopqrstuvwxyz_C";
    width -= 300;
    buttonsStartPositionX = width - (width - boxWidth) / 2 - 50 * 7;
    width += 300;
    buttonsStartPositionY = height - 50 - 4 * 50;
    numberOfColumns = 7;
    for (int ind = 0; ind < strlen(secondBlockSymbols); ind++)
    {
        Button button;
        char text[5] = "";
        ButtonsTypes buttonType = TYPE_A_CHARACTER;
        if (secondBlockSymbols[ind] == 'C')
        {
            strcpy(text, "CAPS");
            buttonType = SWITCH_LETTERS_CASE;
        }
        else
        {
            text[0] = secondBlockSymbols[ind];
            text[1] = '\0';
        }
        InitializeAButton(button, buttonType, buttonsStartPositionX + 50 * (ind % numberOfColumns), buttonsStartPositionY + 50 * (ind / numberOfColumns), buttonsStartPositionX + 50 * (ind % numberOfColumns) + 49, buttonsStartPositionY + 50 * (ind / numberOfColumns) + 49, text);
        _typeAnExpressionButtons.push_back(button);
    }

    Button submitButton;
    InitializeAButton(submitButton, SUBMIT_AN_EXPRESSION, width - 275, height - 175, width - 25, height - 125, (char*)"SUBMIT");
    _typeAnExpressionButtons.push_back(submitButton);

    Button exitButton;
    InitializeAButton(exitButton, EXIT, width - 275, height - 100, width - 25, height - 50, (char*)"EXIT");
    _typeAnExpressionButtons.push_back(exitButton);
}

void InitializeDisplayExpressionButtons(vector<Button>& _displayExpressionButtons)
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    Button switchToDisplayExpressinTreeButton;
    InitializeAButton(switchToDisplayExpressinTreeButton, SWITCH_TO_DISPLAY_EXPRESSION_TREE, width - 275, 25, width - 25, 75, (char*)"DISPLAY EXPRESSION TREE");
    _displayExpressionButtons.push_back(switchToDisplayExpressinTreeButton);

    Button switchToTypeAnExpressinButton;
    InitializeAButton(switchToTypeAnExpressinButton, SWITCH_TO_TYPE_AN_EXPRESSION, width - 275, 100, width - 25, 150, (char*)"TYPE AN EXPRESSION");
    _displayExpressionButtons.push_back(switchToTypeAnExpressinButton);

    Button moveLeftButton;
    InitializeAButton(moveLeftButton, MOVE_LEFT, width - 225, height - 250, width - 175, height - 200, (char*)"L");
    _displayExpressionButtons.push_back(moveLeftButton);

    Button moveUpButton;
    InitializeAButton(moveUpButton, MOVE_UP, width - 175, height - 300, width - 125, height - 250, (char*)"U");
    _displayExpressionButtons.push_back(moveUpButton);

    Button moveRightButton;
    InitializeAButton(moveRightButton, MOVE_RIGHT, width - 125, height - 250, width - 75, height - 200, (char*)"R");
    _displayExpressionButtons.push_back(moveRightButton);

    Button moveDownButton;
    InitializeAButton(moveDownButton, MOVE_DOWN, width - 175, height - 200, width - 125, height - 150, (char*)"D");
    _displayExpressionButtons.push_back(moveDownButton);

    Button exitButton;
    InitializeAButton(exitButton, EXIT, width - 275, height - 100, width - 25, height - 50, (char*)"EXIT");
    _displayExpressionButtons.push_back(exitButton);
}

void InitializeDisplayExpressionTreeButtons(vector<Button>& _displayExpressionTreeButtons)
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    Button switchToDisplayExpressinButton;
    InitializeAButton(switchToDisplayExpressinButton, SWITCH_TO_DISPLAY_EXPRESSION, width - 275, 25, width - 25, 75, (char*)"DISPLAY EXPRESSION");
    _displayExpressionTreeButtons.push_back(switchToDisplayExpressinButton);

    Button switchToTypeAnExpressinButton;
    InitializeAButton(switchToTypeAnExpressinButton, SWITCH_TO_TYPE_AN_EXPRESSION, width - 275, 100, width - 25, 150, (char*)"TYPE AN EXPRESSION");
    _displayExpressionTreeButtons.push_back(switchToTypeAnExpressinButton);

    Button moveLeftButton;
    InitializeAButton(moveLeftButton, MOVE_LEFT, width - 225, height - 250, width - 175, height - 200, (char*)"L");
    _displayExpressionTreeButtons.push_back(moveLeftButton);

    Button moveUpButton;
    InitializeAButton(moveUpButton, MOVE_UP, width - 175, height - 300, width - 125, height - 250, (char*)"U");
    _displayExpressionTreeButtons.push_back(moveUpButton);

    Button moveRightButton;
    InitializeAButton(moveRightButton, MOVE_RIGHT, width - 125, height - 250, width - 75, height - 200, (char*)"R");
    _displayExpressionTreeButtons.push_back(moveRightButton);

    Button moveDownButton;
    InitializeAButton(moveDownButton, MOVE_DOWN, width - 175, height - 200, width - 125, height - 150, (char*)"D");
    _displayExpressionTreeButtons.push_back(moveDownButton);

    Button exitButton;
    InitializeAButton(exitButton, EXIT, width - 275, height - 100, width - 25, height - 50, (char*)"EXIT");
    _displayExpressionTreeButtons.push_back(exitButton);
}

void InitializeAllButtons(vector<Button>& _mainMenuButtons, vector<Button>& _typeAnExpressionButtons, vector<Button>& _displayExpressionButtons, vector<Button>& _displayExpressionTreeButtons)
{
    InitializeMainMenuButtons(_mainMenuButtons);
    InitializeTypeAnExpressionButtons(_typeAnExpressionButtons);
    InitializeDisplayExpressionButtons(_displayExpressionButtons);
    InitializeDisplayExpressionTreeButtons(_displayExpressionTreeButtons);
}

bool IsCursorPositionInsideAButton(POINT cursorPosition, Button button)
{
    if (cursorPosition.x >= button.position.left && cursorPosition.x <= button.position.right && cursorPosition.y >= button.position.top && cursorPosition.y <= button.position.bottom)
        return true;

    return false;
}

void ExitButtonBehaviour()
{
    exit(0);
}

void DeleteTree(Node*& R)
{
    if (R == NULL)
        return;

    DeleteTree(R->left);
    DeleteTree(R->right);
    delete(R);
}

void SwitchToTypeAnExpressionButtonBehaviour()
{
    applicationStatus = TYPE_AN_EXPRESSION;
    strcpy(Formula, "");
    DeleteTree(Root);
    Root = new Node;
    cleardevice();
    DrawApplicationStatusButtons();
    DrawApplicationStatusExtraUI();
}

void SetStartingPositionsForDrawingAnExpressionOnTheScreen(char token[MAX_ArrayOOFLen], int& xStart, int& yStart)
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    settextstyle(SANS_SERIF_FONT, 0, 3);
    xStart = (width - 300) / 2;
    int leftExpressionWidth = 0, rightExpressionWidth = 0;
    if (!strcmp(token, "/"))
    {
        rightExpressionWidth = max(GetExpressionWidth(Root->left), GetExpressionWidth(Root->right));
    }
    else if (!strcmp(token, "^"))
    {
        leftExpressionWidth = GetExpressionWidth(Root->left);
        rightExpressionWidth = GetExpressionWidth(Root->right);
    }
    else if (!strcmp(token, "sqrt"))
    {
        rightExpressionWidth = GetExpressionWidth(Root->right) + textwidth((char*)"II");
    }
    else if (!strcmp(token, "rad"))
    {
        leftExpressionWidth = GetExpressionWidth(Root->left) / 2;
        rightExpressionWidth = GetExpressionWidth(Root->right) + textwidth((char*)"II");
    }
    else if (!strcmp(token, "der"))
    {
        rightExpressionWidth = GetExpressionWidth(Root->left) + GetExpressionWidth(Root->right) + textwidth((char*)"d()");
    }
    else if (!strcmp(token, "int"))
    {
        char token1[MAX_ArrayOOFLen];
        if (GetTheNumberOfApparitionsOfACharacterInAToken(token, '_') > 1)
            GetASpecificTokenFromAnExpression(Root->left->information, (char*)"_", 3, token1);
        else
            strcpy(token1, Root->left->information);

        rightExpressionWidth = GetExpressionWidth(Root->right) + textwidth((char*)"()d") + textwidth(token1);
    }
    else if (!strcmp(token, "lim"))
    {
        char token1[MAX_ArrayOOFLen], token2[MAX_ArrayOOFLen];
        GetASpecificTokenFromAnExpression(Root->left->information, (char*)"_", 1, token1);
        GetASpecificTokenFromAnExpression(Root->left->information, (char*)"_", 2, token2);
        int length1 = textwidth(token1);
        int lenght2 = textwidth(token2);

        rightExpressionWidth = GetExpressionWidth(Root->right) + textwidth((char*)"()") + textwidth((char*)token1) + textwidth((char*)token2) + 20;
    }
    else if (!strcmp(token, "sum") || !strcmp(token, "mul"))
    {
        char token1[MAX_ArrayOOFLen], token2[MAX_ArrayOOFLen], token3[MAX_ArrayOOFLen];
        GetASpecificTokenFromAnExpression(Root->left->information, (char*)"_", 1, token1);
        GetASpecificTokenFromAnExpression(Root->left->information, (char*)"_", 2, token2);
        GetASpecificTokenFromAnExpression(Root->left->information, (char*)"_", 3, token3);
        int length1 = textwidth(token1);
        int lenght2 = textwidth(token2);
        int lenght3 = textwidth(token3);

        rightExpressionWidth = GetExpressionWidth(Root->right) + textwidth((char*)"()") + max(length1 + lenght2 + textwidth((char*)"="), lenght3);
    }
    else if (GetTokenNature(token) == FUNCTION)
    {
        rightExpressionWidth = GetExpressionWidth(Root);
    }
    else
    {
        leftExpressionWidth = GetExpressionWidth(Root->left);
        rightExpressionWidth = GetExpressionWidth(Root->right) + textwidth(Root->information);
    }

    int xDelta = abs(leftExpressionWidth - rightExpressionWidth) / 2;
    if (leftExpressionWidth > rightExpressionWidth)
        xStart += xDelta;
    else
        xStart -= xDelta;

    yStart = height / 2;
    int leftExpressionHeight = 0;
    int rightExpressionHeight = 0;
    int yDelta = 0;
    if (!strcmp(token, "/"))
    {
        yStart -= leftExpressionHeight = GetExpressionHeight(Root->left);
    }
    else
    {
        rightExpressionHeight = textheight((char*)"M");
        yDelta = rightExpressionHeight / 2;

        if (leftExpressionHeight > rightExpressionHeight)
            yStart += yDelta;
        else
            yStart -= yDelta;
    }

    settextstyle(DEFAULT_FONT, 0, 2);
}

void CleanRightButtonsSection()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    readimagefile("Black_Background.jpg", width - 300, 0, width, height);
}

void DrawExpressionOnTheScreen()
{
    settextstyle(SANS_SERIF_FONT, 0, 3);
    DrawAnExpression(Root, xDrawingExpressionStartPosition, yDrawingExpressionStartPosition);
    settextstyle(DEFAULT_FONT, 0, 2);

    CleanRightButtonsSection();
}

void SetPositionsForExpressionDrawing()
{
    xDrawingExpressionStartPosition = yDrawingExpressionStartPosition = 0;
    SetStartingPositionsForDrawingAnExpressionOnTheScreen(Root->information, xDrawingExpressionStartPosition, yDrawingExpressionStartPosition);
}

void SwitchToDisplayExpressionButtonBehaviour()
{
    applicationStatus = DISPLAY_EXPRESSION;
    cleardevice();

    DrawExpressionOnTheScreen();

    DrawApplicationStatusButtons();
    DrawApplicationStatusExtraUI();
}

void CalculateTheWidthForAnExpressionTree(Node* R, int numberOfLevels, int level, int& treeWidth)
{
    if (R == NULL)
        return;

    if (level == numberOfLevels)
    {
        int textWidth = textwidth((char*)R->information);
        if (textWidth > treeWidth)
            treeWidth = textWidth;
    }
    else
    {
        CalculateTheWidthForAnExpressionTree(R->left, numberOfLevels, level + 1, treeWidth);
        CalculateTheWidthForAnExpressionTree(R->right, numberOfLevels, level + 1, treeWidth);
    }
}

void NormalizeExpressionTreeDimensions(int windowWidth, int windowHeight, int& treeWidth, int& treeHeight)
{
    treeWidth = 0;
    int numberOfLevels = count_levels(Root);

    CalculateTheWidthForAnExpressionTree(Root, numberOfLevels, 1, treeWidth);
    if (treeWidth < 30)
        treeWidth = 30;
    for (int ind = 1; ind < numberOfLevels; ind++)
        treeWidth *= 2;

    if (windowHeight > 900)
    {
        treeHeight = 100;
    }
}

void CalculateExpressionTreeLeftWidth(Node* R, int treeWidth, int& treeLeftWidth)
{
    if (R == NULL)
        return;

    if (R->left != NULL)
    {
        treeLeftWidth = treeLeftWidth + treeWidth / 2;
        CalculateExpressionTreeLeftWidth(R->left, treeWidth / 2, treeLeftWidth);
    }
    else if (R->right != NULL)
        CalculateExpressionTreeLeftWidth(R->right, treeWidth / 2, treeLeftWidth);
}

void CalculateExpressionTreeRightWidth(Node* R, int treeWidth, int& treeRightWidth)
{
    if (R == NULL)
        return;

    if (R->left != NULL)
        treeRightWidth = treeRightWidth + treeWidth / 2;

    if (R->right != NULL)
        CalculateExpressionTreeRightWidth(R->right, treeWidth / 2, treeRightWidth);
}

void CalculateExpressionTreeTotalHeight(Node* R, int treeHeight, int& treeTotalHeight)
{
    treeTotalHeight = (count_levels(R) - 1) * treeHeight;
}

void DrawExpressionTreeOnTheScreen()
{
    int windowWidth = 0, windowHeight = 0, treeWidth = 0, treeHeight = 0;
    calculate_width_height_of_the_tree(Root, windowWidth, windowHeight, treeWidth, treeHeight);

    settextstyle(SANS_SERIF_FONT, 0, 3);
    NormalizeExpressionTreeDimensions(windowWidth, windowHeight, treeWidth, treeHeight);

    draw_tree(Root, xDrawingExpressionTreeStartPosition, yDrawingExpressionTreeStartPosition, treeWidth, treeHeight);
    settextstyle(DEFAULT_FONT, 0, 2);

    CleanRightButtonsSection();
}

void SetPositionsForExpressionTreeDrawing()
{
    int windowWidth = 0, windowHeight = 0, treeWidth = 0, treeHeight = 0;
    calculate_width_height_of_the_tree(Root, windowWidth, windowHeight, treeWidth, treeHeight);

    settextstyle(SANS_SERIF_FONT, 0, 3);
    NormalizeExpressionTreeDimensions(windowWidth, windowHeight, treeWidth, treeHeight);
    settextstyle(DEFAULT_FONT, 0, 2);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    xDrawingExpressionTreeStartPosition = (width - 300) / 2;
    int treeLeftWidth = 0, treeRightWidth = 0;
    CalculateExpressionTreeLeftWidth(Root, treeWidth, treeLeftWidth);
    CalculateExpressionTreeRightWidth(Root, treeWidth, treeRightWidth);

    int xDelta = abs(treeLeftWidth - treeRightWidth) / 2;
    if (treeLeftWidth > treeRightWidth)
        xDrawingExpressionTreeStartPosition += xDelta;
    else
        xDrawingExpressionTreeStartPosition -= xDelta;

    int treeTotalHeight = 0;
    CalculateExpressionTreeTotalHeight(Root, treeHeight, treeTotalHeight);
    yDrawingExpressionTreeStartPosition = (height - treeTotalHeight) / 2;
    if (yDrawingExpressionTreeStartPosition < 0)
        yDrawingExpressionTreeStartPosition = 100;
}

void SwitchToDisplayExpressionTreeButtonBehaviour()
{
    applicationStatus = DISPLAY_EXPRESSION_TREE;
    cleardevice();

    DrawExpressionTreeOnTheScreen();

    DrawApplicationStatusButtons();
    DrawApplicationStatusExtraUI();
}

void PrintTypedFormulaOnTheScreen()
{
    cleardevice();
    DrawApplicationStatusButtons();
    DrawApplicationStatusExtraUI();

    int numberOfCharactersInARow = 50;
    char formulaPart[MAX_ArrayCFormLen] = "";
    formulaPart[numberOfCharactersInARow] = '\0';
    int boxTopY = 50 + 75 + 50;
    int textHeight = textheight((char*)"M");
    int ind = 0;
    for (; ind < strlen(Formula); ind++)
    {
        formulaPart[ind % numberOfCharactersInARow] = Formula[ind];
        if (!((ind + 1) % numberOfCharactersInARow))
            PrintCenteredText(formulaPart, boxTopY + 10 + ind / numberOfCharactersInARow * textHeight, 300);
    }

    if (ind % numberOfCharactersInARow)
    {
        formulaPart[ind % numberOfCharactersInARow] = '\0';
        PrintCenteredText(formulaPart, boxTopY + 10 + ind / numberOfCharactersInARow * textHeight, 300);
    }
}

void SubmitAnExpressionButtonBehaviour()
{
    showEmptyFormulaMessage = showToLongOperandsMessage = showSyntacticallyIncorrectFormulaMessage = showBasicMathematicalPrinciplesBrokenMessage = false;

    if (!strlen(Formula))
    {
        showEmptyFormulaMessage = true;

        cleardevice();
        DrawApplicationStatusButtons();
        DrawApplicationStatusExtraUI();

        return;
    }

    if (IsASyntacticallyCorrectFormula_1(Formula))
        if (IsASyntacticallyCorrectFormula_2(Formula))
            if (IsASyntacticallyCorrectFormula_3(Formula))
            {
                applicationStatus = DISPLAY_EXPRESSION;

                cleardevice();

                CreateTheExpressionTreeOfAFormula(Formula, Root);

                SetPositionsForExpressionDrawing();
                DrawExpressionOnTheScreen();

                SetPositionsForExpressionTreeDrawing();

                DrawApplicationStatusButtons();
                DrawApplicationStatusExtraUI();

                return;
            }

    if (!IsASyntacticallyCorrectFormula_1(Formula))
        showToLongOperandsMessage = true;
    else if (!IsASyntacticallyCorrectFormula_2(Formula))
        showSyntacticallyIncorrectFormulaMessage = true;
    else
        showBasicMathematicalPrinciplesBrokenMessage = true;

    PrintTypedFormulaOnTheScreen();
}

void TypeACharacterButtonBehaviour()
{
    if (strlen(Formula) >= MAX_CFormLen)
        return;

    POINT cursorPosition;
    GetCursorPos(&cursorPosition);
    ScreenToClient(GetForegroundWindow(), &cursorPosition);
    for (int ind = 0; ind < typeAnExpressionButtons.size(); ind++)
        if (IsCursorPositionInsideAButton(cursorPosition, typeAnExpressionButtons[ind]))
        {
            strcat(Formula, typeAnExpressionButtons[ind].text);
            PrintTypedFormulaOnTheScreen();
            return;
        }
}

void RemoveACharacterButtonBehaviour()
{
    if (!strlen(Formula))
        return;

    Formula[strlen(Formula) - 1] = '\0';
    PrintTypedFormulaOnTheScreen();
}

void SwitchLettersCaseButtonBehaviour()
{
    typeLowercaseLetters = !typeLowercaseLetters;

    for (int ind = 0; ind < typeAnExpressionButtons.size(); ind++)
    {
        char character = typeAnExpressionButtons[ind].text[0];
        if (((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z')) && strlen(typeAnExpressionButtons[ind].text) == 1)
            if (typeLowercaseLetters)
                typeAnExpressionButtons[ind].text[0] += 32;
            else
                typeAnExpressionButtons[ind].text[0] -= 32;
    }

    PrintTypedFormulaOnTheScreen();

    //cleardevice();
    //DrawApplicationStatusButtons();
    //DrawApplicationStatusExtraUI();
}

void MoveLeftButtonBehaviour()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    for (int row = 0; row <= height; row++)
        if (getpixel(width - 301, row) == WHITE)
        {
            cleardevice();
            if (applicationStatus == DISPLAY_EXPRESSION)
            {
                xDrawingExpressionStartPosition -= xDrawingDelta;
                DrawExpressionOnTheScreen();
            }
            else
            {
                xDrawingExpressionTreeStartPosition -= xDrawingDelta;
                DrawExpressionTreeOnTheScreen();
            }
            DrawApplicationStatusButtons();
            DrawApplicationStatusExtraUI();

            return;
        }
}

void MoveUpButtonBehaviour()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    for (int column = 0; column < width - 300; column++)
        if (getpixel(column, height - 5) == WHITE)
        {
            cleardevice();
            if (applicationStatus == DISPLAY_EXPRESSION)
            {
                yDrawingExpressionStartPosition -= yDrawingDelta;
                DrawExpressionOnTheScreen();
            }
            else
            {
                yDrawingExpressionTreeStartPosition -= yDrawingDelta;
                DrawExpressionTreeOnTheScreen();
            }
            DrawApplicationStatusButtons();
            DrawApplicationStatusExtraUI();

            return;
        }
}

void MoveRightButtonBehaviour()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    for (int row = 0; row <= height; row++)
        if (getpixel(0, row) == WHITE)
        {
            cleardevice();
            if (applicationStatus == DISPLAY_EXPRESSION)
            {
                xDrawingExpressionStartPosition += xDrawingDelta;
                DrawExpressionOnTheScreen();
            }
            else
            {
                xDrawingExpressionTreeStartPosition += xDrawingDelta;
                DrawExpressionTreeOnTheScreen();
            }
            DrawApplicationStatusButtons();
            DrawApplicationStatusExtraUI();

            return;
        }
}

void MoveDownButtonBehaviour()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    for (int column = 0; column < width - 300; column++)
        if (getpixel(column, 0) == WHITE)
        {
            cleardevice();
            if (applicationStatus == DISPLAY_EXPRESSION)
            {
                yDrawingExpressionStartPosition += yDrawingDelta;
                DrawExpressionOnTheScreen();
            }
            else
            {
                yDrawingExpressionTreeStartPosition += yDrawingDelta;
                DrawExpressionTreeOnTheScreen();
            }
            DrawApplicationStatusButtons();
            DrawApplicationStatusExtraUI();

            return;
        }
}

void TryToExecuteClicOperation(vector<Button> buttons, POINT cursorPosition)
{
    for (int ind = 0; ind < buttons.size(); ind++)
        if (IsCursorPositionInsideAButton(cursorPosition, buttons[ind]))
        {
            switch (buttons[ind].type)
            {
            case EXIT:
                ExitButtonBehaviour();
                break;
            case SWITCH_TO_TYPE_AN_EXPRESSION:
                SwitchToTypeAnExpressionButtonBehaviour();
                break;
            case SWITCH_TO_DISPLAY_EXPRESSION:
                SwitchToDisplayExpressionButtonBehaviour();
                break;
            case SWITCH_TO_DISPLAY_EXPRESSION_TREE:
                SwitchToDisplayExpressionTreeButtonBehaviour();
                break;
            case SUBMIT_AN_EXPRESSION:
                SubmitAnExpressionButtonBehaviour();
                break;
            case TYPE_A_CHARACTER:
                TypeACharacterButtonBehaviour();
                break;
            case REMOVE_A_CHARACTER:
                RemoveACharacterButtonBehaviour();
                break;
            case SWITCH_LETTERS_CASE:
                SwitchLettersCaseButtonBehaviour();
                break;
            case MOVE_LEFT:
                MoveLeftButtonBehaviour();
                break;
            case MOVE_UP:
                MoveUpButtonBehaviour();
                break;
            case MOVE_RIGHT:
                MoveRightButtonBehaviour();
                break;
            case MOVE_DOWN:
                MoveDownButtonBehaviour();
                break;
            default:
                break;
            }

            return;
        }
}

int main()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    initwindow(width, height, "Visualize_A_Mathematical_Expression.exe");

    settextstyle(DEFAULT_FONT, 0, 2); //This will be the default font, orientation and size of the text in this project.

    InitializeAllButtons(mainMenuButtons, typeAnExpressionButtons, displayExpressionButtons, displayExpressionTreeButtons);

    DrawApplicationStatusButtons();
    DrawApplicationStatusExtraUI();

    bool canClickOnAButton = true;
    while (true)
    {
        if (ismouseclick(WM_LBUTTONDOWN))
        {
            if (canClickOnAButton)
            {
                POINT cursorPosition = { 0 };
                GetCursorPos(&cursorPosition);
                ScreenToClient(GetForegroundWindow(), &cursorPosition);
                switch (applicationStatus)
                {
                case MAIN_MENU:
                    TryToExecuteClicOperation(mainMenuButtons, cursorPosition);
                    break;
                case TYPE_AN_EXPRESSION:
                    TryToExecuteClicOperation(typeAnExpressionButtons, cursorPosition);
                    break;
                case DISPLAY_EXPRESSION:
                    TryToExecuteClicOperation(displayExpressionButtons, cursorPosition);
                    break;
                case DISPLAY_EXPRESSION_TREE:
                    TryToExecuteClicOperation(displayExpressionTreeButtons, cursorPosition);
                    break;
                default:
                    break;
                }
            }

            canClickOnAButton = false;
        }
        else
            canClickOnAButton = true;
    }

    //getch();
    //closegraph();

    return 0;
}
#ifndef INCLUDED
#define INCLUDED
#include <stdio.h> 
#include <stdlib.h> 
//运算符栈的长度 
#define OPSTACK_LENGTH 5 
//操作数栈的长度 
#define NUMSTACK_LENGTH 100 
//输入串的最大长度 
#define MAX_STRING_LENGTH 100 
 
//运算符结构体

struct operatorStruct 
{ 
    //运算符名称 
    char name; 
    //优先级 
    int priority; 
    //目数，即操作数个数，例如单目运算符为1，双目运算符2 
    int opnum; 
}; 
 
typedef struct operatorStruct OPERATOR; 
 
//运算符栈 
OPERATOR opStack[OPSTACK_LENGTH]; 
//运算符栈顶指针 
int opStackTop = -1; 
//操作数栈 
double numStack[NUMSTACK_LENGTH]; 
//操作数栈顶指针 
int numStackTop = -1; 
 
//获取一个字符所代表的运算符的优先级 
int getPriority(char name) 
{ 
    if (name == '(' || name == ')') 
    { 
        return 0; 
    } 
    if (name == '!') 
    { 
        return 3; 
    } 
    if (name == '*' || name == '/') 
    { 
        return 2; 
    } 
    if (name == '+' || name == '-') 
    { 
        return 1; 
    } 
	MessageBox(NULL,TEXT("错误的运算符或操作数！"),NULL,MB_OK);
} 
//获取一个字符所代表的运算符的目数 
int getOpNum(char name) 
{ 
    if (name == '*' || name == '/' || name == '+' || name == '-') 
    { 
        return 2; 
    } 
    if (name == '!') 
    { 
        return 1; 
    } 
    if (name == '(' || name == ')') 
    { 
        return 0; 
    } 
    MessageBox(NULL,TEXT("错误的运算符或操作数！"),NULL,MB_OK);

} 
 
//运算符压栈 
void pushOperator(OPERATOR op) 
{ 
    if (opStackTop < OPSTACK_LENGTH - 1) 
    { 
        opStack[++opStackTop] = op; 
    } 
    else 
    { 
        MessageBox(NULL,TEXT("错误的运算符或操作数！"),NULL,MB_OK);
    } 
} 
//运算符出栈 
OPERATOR popOperator() 
{ 
    if (opStackTop >= 0) 
    { 
        return opStack[opStackTop--]; 
    } 
    else 
    { 
        MessageBox(NULL,TEXT("错误的运算符或操作数！"),NULL,MB_OK);
    } 
} 
//操作数压栈 
void pushNumber(double num) 
{ 
    if (numStackTop < NUMSTACK_LENGTH - 1) 
    { 
        numStack[++numStackTop] = num; 
    } 
    else 
    { 
        MessageBox(NULL,TEXT("错误的运算符或操作数！"),NULL,MB_OK); 
    } 
} 
//操作数出栈 
double popNumber() 
{ 
    if (numStackTop >= 0) 
    { 
        return numStack[numStackTop--]; 
    } 
    else 
    { 
        MessageBox(NULL,TEXT("错误的运算符或操作数！"),NULL,MB_OK);
    } 
} 
//将输入字符串中的以0-9开头、到下一个运算符结束的一段转化为浮点型 
//i加上浮点型对应的字符串的长度 
double getNumFromString(char *s, int *i) 
{ 
    int j = 0; 
    static char numstr[MAX_STRING_LENGTH]; 
    while ((*s) >= '0' && *s <= '9') 
    { 
        numstr[j++] = (*s); 
        s++; 
    } 
    if ((*s) == '.') 
    { 
        numstr[j++] = (*s); 
        s++; 
        while ((*s) >= '0' && *s <= '9') 
        { 
            numstr[j++] = (*s); 
            s++; 
        } 
    } 
    (*i) = (*i) + j; 
    numstr[j] = '\0'; 
    return atof(numstr); 
} 
 
//从操作数栈中弹出两个操作数，完成一次双目运算 
double opertate2Num(OPERATOR op) 
{ 
    double num2 = popNumber(); 
    double num1 = popNumber(); 
    if (op.name == '+') 
    { 
        return num1 + num2; 
    } 
    if (op.name == '-') 
    { 
        return num1 - num2; 
    } 
    if (op.name == '*') 
    { 
        return num1 * num2; 
    } 
    if (op.name == '/') 
    { 
        return num1 / num2; 
    } 
    MessageBox(NULL,TEXT("错误的运算符或操作数！"),NULL,MB_OK);
} 
//从操作数栈中弹出一个操作数，完成一次单目运算 
double opertate1Num(OPERATOR op) 
{ 
    double num = popNumber(); 
    if (op.name == '!') 
    { 
        double result = 1; 
        while (num > 1) 
        { 
            result *= num; 
            num--; 
        } 
        return result; 
    } 
} 
//完成一次运算 www.2cto.com
double operate(OPERATOR op) 
{ 
    if (op.opnum == 1) 
    { 
        return opertate1Num(op); 
    } 
    else if (op.opnum == 2) 
    { 
        return opertate2Num(op); 
    } 
} 
 
double jisuan(char string[MAX_STRING_LENGTH])
{ 
    int i; 
    OPERATOR op, topOp;//op为从当前输入串中提取的一个运算符，topOp为运算符栈栈顶的运算符 
    double end; 
    topOp.name = '#'; 
    topOp.priority = 0; 
    topOp.opnum = 0; 
    pushOperator(topOp);//压入#作为初始运算符 
     
    for (i = 0; string[i] != '\0' && string[i] != '=';) 
    { 
        //从输入串中取出一个字符作为开始，进行处理，直到表达式结束 
        if (string[i] >= '0' && string[i] <= '9') 
        { 
            //如果是操作数，将整个操作数提取出来，压入操作数栈 
            pushNumber(getNumFromString(&string[i], &i)); 
        } 
        else 
        { 
            op.name = string[i]; 
            op.priority = getPriority(string[i]); 
            op.opnum = getOpNum(string[i]); 
            topOp = popOperator(); 
            if (op.name == '(') 
            { 
                //如果是'('，将从栈顶弹出的运算符压回栈内，并将当前运算符则压栈 
                pushOperator(topOp); 
                pushOperator(op); 
            } 
            else if (op.name == ')') 
            { 
                //如果是')'，则进行运算，每次运算结果作为一个操作数压入操作数栈，直到将'('弹出运算符栈 
                while (topOp.name != '(') 
                { 
                    pushNumber(operate(topOp)); 
                    topOp = popOperator(); 
                } 
            } 
            else 
            { 
                //如果是普通运算符 
                if (topOp.name != '#' && op.priority <= topOp.priority) 
                { 
                    //如果运算符栈非空，且当前运算符的优先级大于栈顶运算符，则进行一次运算，将结果压入操作数栈 
                    pushNumber(operate(topOp)); 
                } 
                else 
                { 
                    //否则将从栈顶弹出的运算符压回 
                    pushOperator(topOp); 
                } 
                //将当前运算符压栈 
                pushOperator(op); 
            } 
            i++; 
        } 
    } 
    //完成栈内剩余的运算 
    while ((topOp = popOperator()).name != '#') 
    { 
        pushNumber(operate(topOp)); 
    } 
	end=popNumber();
	TCHAR* te=(TCHAR*)malloc(100);
    //操作数栈中剩下的最后一个数即为结果 
    return end;
} 
#endif
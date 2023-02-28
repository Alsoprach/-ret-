#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdarg.h>

__int64_t *csp;
__int64_t main_canary;
__int64_t tmpebp;
__int64_t tmpret;

#define cpush(x) csp++;asm ("":"=r"(*csp) : "a"(x): );
#define cpop(x)  asm ("":"=r"(x) : "a"(*csp): );csp--;

#define Function_Stack_Init \
    csp=(int*)calloc(0x10000,sizeof(__int64_t));\
    srand(time((time_t *)NULL));\
    __int64_t main_canary_A=(rand()*0x100000000+rand())&0xffffffffffffff00;\
    main_canary=main_canary_A;

#define Function_Stack_End \
    if(main_canary_A != main_canary)exit(0);

void retget( ){
    __asm__("pop %0":"=r"(tmpebp)::"memory");
    __asm__("pop %0":"=r"(tmpret)::"memory");
    __asm__("push %0"::"r"(tmpret):"memory");
    __asm__("push %0"::"r"(tmpebp):"memory");
}

void value( int num, ... ){
    va_list valist;
    va_start(valist, num);
    for(int i=1;i<=num;i++){
        int value=va_arg(valist, int);
        if(i<=6){
            if(i==1){
                asm ("mov %%rax, %%rdi;": : "a"(value): );
            }
            else if(i==2){
                asm ("mov %%rax, %%rsi;": : "a"(value): );
            }
            else if(i==3){
                asm ("mov %%rax, %%rdx;": : "a"(value): );
            }
            else if(i==4){
                asm ("mov %%rax, %%rcx;": : "a"(value): );
            }
            else if(i==5){
                asm ("mov %%rax, %%r8;" : : "a"(value): );
            }
            else if(i==6){
                asm ("mov %%rax, %%r9;" : : "a"(value): );
            }
        }else{
                asm ("push %%rax": : "a"(value): );
        }
    }
    asm (""::"a"(0):);
}

#define Function_Scheduler(function,num,...) \
    value( num , ##__VA_ARGS__ );\
    retget();\
    tmpret+=49;\
    cpush(tmpret);\
    __asm__("push %%rbx;jmp %0"::"r"(function):"memory");

#define ret \
    cpop(tmpret);\
    __asm__("leave":::);\
    __asm__("pop %%rbx":::"rcx");\
    __asm__("jmp %0": :"r"(tmpret):"memory");
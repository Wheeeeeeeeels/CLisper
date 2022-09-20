#include "mpc.h"
#include "stdio.h"
#include "stdlib.h"

#ifndef __WIN32

#include <string.h>

static char buffer[2048];

char *readline(char *prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    // 字符串最后的'\0'
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}


long eval_op(long x,char *op,long y){
    if(strcmp(op,"+")==0) {return x+y;}
    if(strcmp(op,"-")==0){return x-y;}
    if(strcmp(op,"*")==0){return x*y;}
    if(strcmp(op,"/")==0){return x/y;}
    return 0;
}

long eval(mpc_ast_t* t){
    // tag表示节点内容之前的信息,表示了解析某个节点所用到的规则.例如:expr|number|regex
    // strstr(): 接收两个char* 如果第一个字符串包含第二个，返回其在第一个首次出现的位置的指针，否则为0
    if(strstr(t->tag,"number")){
        // contents:包括节点中具体的内容，对于表示分支的非叶子节点，该字段为空
        // 对于叶子节点，则包含了操作数或操作符的字符串形式
        return atoi(t->contents);
    }
    // 操作符永远是第二个孩节点
    char* op = t->children[1]->contents;
    // 存储第三个孩子在x中
    long x = eval(t->children[2]);
    int i = 3;
    while(strstr(t->children[i]->tag,"expr")){
        x = eval_op(x, op,eval(t->children[i]));
        i++;
    }
    return x;
}

void add_history(char *unused) {}

#else
#ifdef __linux__
#include "editline/history.h"
#include "editline/readline.h"
#endif

#ifdef __MACH__
#include "editline/readline.h"
#endif
#endif

enum{LERR_DIV_ERROR,LERR_BAD_OP,LERR_BAD_NUM};
enum{LVAL_NUM,LVAL_ERR};



int main() {
    // create some prasers
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Clisp = mpc_new("clisp");

    mpca_lang(MPCA_LANG_DEFAULT,
                   "number   : /-?[0-9]+/ ;"
                   "operator : '+' | '-' | '*' | '/' ;"
                   "expr     : <number> | '(' <operator> <expr>+ ')' ;"
                   "clisp    : /^/ <operator> <expr>+ /$/ ;"
            ,Number, Operator, Expr, Clisp);
    puts("CLisp Version 0.0.0.1");
    puts("Press Ctrl+c to Exit\n");
    while (1) {
        char *input = readline("clisp> ");
        add_history(input);
        // printf("No you're a %s\n", input);
        mpc_result_t res;
        if (mpc_parse("<stdin>", input, Clisp, &res)) {
           //  mpc_ast_print(res.output);
            // mpc_ast_delete(res.output);
            long result = eval(res.output);
            printf("%li\n",result);
            mpc_ast_delete(res.output);
        } else {
             mpc_err_print(res.error);
             mpc_err_delete(res.error);
        }
        free(input);
    }
    mpc_cleanup(4, Number, Operator, Expr, Clisp);
    return 0;
}

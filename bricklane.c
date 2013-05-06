#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define IMAGE_SIZE 65536
#define STACK_SIZE 64
#define RETURN_STACK_SIZE 8192
#define WORD_SIZE 32
#define BUFFER_SIZE 4096
#define NEXT goto *ip++->p
#define CELL_SIZE (sizeof(void*))
#define HEADER(N,L,I,H) dp->p = link; link = dp++; \
  dp->p = (word_metadata *)malloc(sizeof(word_metadata)); \
  ((word_metadata*)dp->p)->length = L; \
  ((word_metadata*)dp->p)->immediate = I; \
  ((word_metadata*)dp->p)->hidden = H; \
  strcpy(((word_metadata*)dp->p)->name, N); \
  dp++

#define VERSION_MAJOR 0 /* not backward compatible */
#define VERSION_MINOR 0 /* backward compatible */

typedef union {
  intptr_t i;
  void *p;
} cell_t;

typedef struct {
  unsigned int immediate : 1;
  unsigned int hidden : 1;
  int length;
  char name[WORD_SIZE];
} word_metadata;

void die(const char *message)
{
  fprintf(stderr, "FATAL: %s\n", message);
  exit(EXIT_FAILURE);
}

void clean_metadata(cell_t * begining, cell_t * top) {
  do {
    free((top+1)->p); 
    top = top->p;
  } while (top!=begining);
}

int main(int argc, const char *argv[])
{
  cell_t stack[STACK_SIZE], *sp = stack;
  cell_t return_stack[RETURN_STACK_SIZE], *rp = return_stack;
  cell_t dictionary[IMAGE_SIZE], *dp = dictionary, *link = NULL, *ip;
  char base = 10, state = 0;
  char word_buffer[WORD_SIZE], *word_p = word_buffer;
  cell_t temp, *temp_p;
  char temp_char, *end;
  div_t divmod_result;
  
  HEADER("VERSION_MAJOR",13,0,0); dp++->p = &&LIT; dp++->i = VERSION_MAJOR;
  HEADER("VERSION_MINOR",13,0,0); dp++->p = &&LIT; dp++->i = VERSION_MINOR;
  HEADER("YES",3,0,0); dp++->p = &&LIT; dp++->i = -1;
  HEADER("NO",2,0,0); dp++->p = &&LIT; dp++->i = 0;

  HEADER("state",5,0,0); dp++->p = &&LIT; dp++->p = &state;
  HEADER("base",4,0,0); dp++->p = &&LIT; dp++->p = &base;

  HEADER("show-stack",10,0,0); dp++->p = &&SHOW_STACK;
  HEADER("drop",4,0,0); dp++->p = &&DROP;
  HEADER("swap",4,0,0); dp++->p = &&SWAP;
  HEADER("dup",3,0,0); dp++->p = &&DUP;
  HEADER("over",4,0,0); dp++->p = &&OVER;
  HEADER("dig",3,0,0); dp++->p = &&DIG;
  HEADER("bury",4,0,0); dp++->p = &&BURY;
  /* 2drop 2dup 2swap ?dup */
  HEADER("1+",2,0,0); dp++->p = &&INCREMENT;
  HEADER("1-",2,0,0); dp++->p = &&DECREMENT;
  HEADER("+",1,0,0); dp++->p = &&PLUS;
  HEADER("-",1,0,0); dp++->p = &&MINUS;
  HEADER("*",1,0,0); dp++->p = &&MULTIPLY;
  HEADER("/mod",4,0,0); dp++->p = &&DIVMOD;
  /* u/mod */
  HEADER("nest",4,0,0); dp++->p = &&NEST;
  HEADER("unnest",6,0,0); dp++->p = &&UNNEST;
  HEADER("lit:",4,0,0); dp++->p = &&LIT;
  HEADER("@",1,0,0); dp++->p = &&FETCH;
  HEADER("!",1,0,0); dp++->p = &&STORE;
  /* +! -! c@ c! c@c! cmove */
  HEADER("key",3,0,0); dp++->p = &&KEY;
  HEADER("emit",4,0,0); dp++->p = &&EMIT;
  HEADER("word:",5,0,0); dp++->p = &&WORD;
  HEADER("number",6,0,0); dp++->p = &&NUMBER;
  HEADER("find",4,0,0); dp++->p = &&FIND;
  HEADER("code-field",10,0,0); dp++->p = &&CODE_FIELD;
  HEADER("create",6,0,0); dp++->p = &&CREATE;
  HEADER(",",1,0,0); dp++->p = &&COMMA;
  HEADER("[",1,1,0); dp++->p = &&STOP_COMPILER;
  HEADER("]",1,0,0); dp++->p = &&START_COMPILER;
  HEADER("jump",4,0,0); dp++->p = &&JUMP;
  HEADER("bye",3,0,0); dp++->p = &&BYE;
  HEADER("quit",4,0,0); dp++->p = &&QUIT;
  
  /* >r r> rsp@ rsp! rdrop */
  /* dsp@ dsp! */ 

  HEADER("interpret",9,0,0); 
  dp++->p = &&NEST;
  dp++->p = &&WORD; dp++->p = &&SHOW_STACK; dp++->p = &&UNNEST;
  //dp++->p = &&WORD; dp++->p = &&FIND;
  //dp++->p = &&CODE_FIELD; dp++->p = &&EXECUTE;

  HEADER("reset",5,0,0); dp++->p = &&NEST;
  dp++->p = (dp-8); dp++->p = &&BYE; //dp++->p = &&JUMP; dp++->i = -2;

  ip = (dp-2);
  NEXT;
DEBUG: puts("y"); NEXT;
NEST: rp++->p = ip++; NEXT;
UNNEST: ip = --rp->p; NEXT;
LIT: *sp++ = *ip++; NEXT;
FETCH: *(sp-1) = *(cell_t*)(sp-1)->p; NEXT;
STORE: *(cell_t*)(sp-1)->p = *(sp-2); sp -= 2; NEXT;
DOVAR: sp++->p = ip++; NEXT;
DROP: sp--; NEXT;
SWAP:
  temp = *(sp-1);
  *(sp-1) = *(sp-2);
  *(sp-2) = temp;
  NEXT;
DUP: *sp++ = *(sp-1); NEXT;
OVER: *sp++ = *(sp-2); NEXT;
DIG:
  temp = *(sp-3);
  *(sp-3) = *(sp-2);
  *(sp-2) = *(sp-1);
  *(sp-1) = temp;
  NEXT;
BURY:
  temp = *(sp-1);
  *(sp-1) = *(sp-2);
  *(sp-2) = *(sp-3);
  *(sp-3) = temp;
  NEXT;
INCREMENT: (sp-1)->i += 1; NEXT;
DECREMENT: (sp-1)->i -= 1; NEXT;
PLUS: ((sp--)-2)->i += (sp-1)->i; NEXT;
MINUS: ((sp--)-2)->i -= (sp-1)->i; NEXT;
MULTIPLY: ((sp--)-2)->i *= (sp-1)->i; NEXT;
DIVMOD:
  divmod_result = div((sp-2)->i, (sp-1)->i);
  (sp-2)->i = divmod_result.quot;
  (sp-1)->i = divmod_result.rem;
  NEXT;
KEY: sp++->i = getchar(); NEXT;
EMIT: putchar(((sp--)-1)->i); NEXT;
WORD:
  word_p = word_buffer;
  temp_char = getchar();
  while(isspace(temp_char)) {
    temp_char = getchar();
  }
  while(!isspace(temp_char)) {
    *word_p++ = temp_char;
    temp_char = getchar();
  }
  *word_p++ = '\0'; /* only while I use std lib */
  sp++->p = word_buffer;
  sp++->i = word_p - word_buffer;
  NEXT;
NUMBER:
  sp--;
  sp++->i = strtol((--sp)->p, &end, base);
  sp++->i = (!*end) ? -1 : 0;
  NEXT;
FIND:
  temp_p = link;
  sp--; /* length not used while using std lib */
  do {
    if (strcmp((sp-1)->p, ((word_metadata*)(temp_p+1))->name)==0) {
      (sp-1)->p = temp_p;
      break;
    }
    temp_p = temp_p->p;
  } while (temp_p!=dictionary);
  NEXT;
CODE_FIELD: (sp-1)->p += 2; NEXT;
CREATE:
  HEADER((char*)((sp-2)->p),(sp-1)->i,0,0);
  sp -= 2;
  NEXT;
COMMA: *dp++ = *--sp; NEXT;
START_COMPILER: state = 1; NEXT;
STOP_COMPILER: state = 0; NEXT;
JUMP: ip += ip->i; NEXT;
EXECUTE: goto *--sp->p;
SHOW_STACK:
  fprintf(stdout, "( ");
  for (temp_p = stack; temp_p < sp; temp_p++) {
    fprintf(stdout, "%ld ", temp_p->i);
  }
  fprintf(stdout, ")\n");
  NEXT;
BYE: puts("bye");
QUIT: clean_metadata(dictionary, link);
  
  return 0;
}


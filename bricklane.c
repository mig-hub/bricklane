#include <stdio.h>
#include <stdlib.h>
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
  sprintf(((word_metadata*)dp->p)->name, N); \
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
  char temp_char;
  div_t divmod_result;
  
  HEADER("show-stack",10,0,0); dp++->p = &&SHOW_STACK;
  HEADER("drop",4,0,0); dp++->p = &&DROP;
  HEADER("swap",4,0,0); dp++->p = &&SWAP;
  HEADER("dup",3,0,0); dp++->p = &&DUP;
  HEADER("over",4,0,0); dp++->p = &&OVER;
  HEADER("dig",3,0,0); dp++->p = &&DIG;
  HEADER("bury",4,0,0); dp++->p = &&BURY;
  HEADER("quit",4,0,0); dp++->p = &&QUIT;
  /* 2drop 2dup 2swap ?dup */
  HEADER("1+",2,0,0); dp++->p = &&INCREMENT;
  HEADER("1-",2,0,0); dp++->p = &&DECREMENT;
  HEADER("+",1,0,0); dp++->p = &&PLUS;
  HEADER("-",1,0,0); dp++->p = &&MINUS;
  HEADER("*",1,0,0); dp++->p = &&MULTIPLY;
  HEADER("/mod",4,0,0); dp++->p = &&DIVMOD;
  /* u/mod */
  HEADER("end",3,0,0); dp++->p = &&END;
  HEADER("lit:",4,0,0); dp++->p = &&LIT;
  HEADER("@",1,0,0); dp++->p = &&FETCH;
  HEADER("!",1,0,0); dp++->p = &&STORE;
  /* +! -! c@ c! c@c! cmove */
  HEADER("key",3,0,0); dp++->p = &&KEY;
  HEADER("emit",4,0,0); dp++->p = &&EMIT;
  
  HEADER("state",5,0,0); dp++->p = &&LIT; dp++->p = &state;
  HEADER("base",4,0,0); dp++->p = &&LIT; dp++->p = &base;

  HEADER("VERSION_MAJOR",13,0,0); dp++->p = &&LIT; dp++->i = VERSION_MAJOR;
  HEADER("VERSION_MINOR",13,0,0); dp++->p = &&LIT; dp++->i = VERSION_MINOR;
  HEADER("DOCOL",5,0,0); dp++->p = &&LIT; dp++->p = &&DOCOL;

  /* >r r> rsp@ rsp! rdrop */
  /* dsp@ dsp! */ 
  sp++->i = 3; sp++->i = 7; sp++->i = 2;
  dp++->p = &&WORD;
  dp++->p = &&SHOW_STACK; 
  dp++->p = &&QUIT;
  ip = (dp-3);
  NEXT;

DOCOL: rp++->p = ip++; NEXT;
END: ip = --rp->p; NEXT;
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
  sp++->p = word_buffer;
  sp++->i = word_p - word_buffer;
  NEXT;
SHOW_STACK:
  fprintf(stdout, "( ");
  for (temp_p = stack; temp_p < sp; temp_p++) {
    fprintf(stdout, "%ld ", temp_p->i);
  }
  fprintf(stdout, ")\n");
  NEXT;
QUIT:
  puts("bye");
  clean_metadata(dictionary, link);
  
  return 0;
}


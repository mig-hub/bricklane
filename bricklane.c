#include <stdio.h>
#include <stdlib.h>

#define IMAGE_SIZE 65536
#define STACK_SIZE 64
#define RETURN_STACK_SIZE 8192
#define WORD_SIZE 32
#define BUFFER_SIZE 4096
#define NEXT goto **ip++
#define CELL_SIZE (sizeof(void*))
#define HEADER(N,L,I,H) *dp = link; link = dp++; \
  *dp = (word_metadata *)malloc(sizeof(word_metadata)); \
  ((word_metadata*)*dp)->length = L; \
  ((word_metadata*)*dp)->immediate = I; \
  ((word_metadata*)*dp)->hidden = H; \
  sprintf(((word_metadata*)*dp)->name, N); \
  dp++

#define VERSION_MAJOR 0 /* not backward compatible */
#define VERSION_MINOR 0 /* backward compatible */

typedef void *cell_t;

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

void clean_metadata(cell_t dictionary, cell_t * top) {
  do {
    free(*(top+1)); 
    top = *top;
  } while (top!=dictionary);
}

int main(int argc, const char *argv[])
{
  cell_t stack[STACK_SIZE];
  cell_t *sp = stack;
  cell_t return_stack[RETURN_STACK_SIZE];
  cell_t *rp = return_stack;
  cell_t *ip;
  cell_t dictionary[IMAGE_SIZE], *dp = dictionary;
  cell_t *link = NULL;
  cell_t temp; cell_t *temp_p;
  div_t divmod_result;
  
  HEADER("show-stack",10,0,0); *dp++ = &&SHOW_STACK;
  HEADER("drop",4,0,0); *dp++ = &&DROP;
  HEADER("swap",4,0,0); *dp++ = &&SWAP;
  HEADER("dup",3,0,0); *dp++ = &&DUP;
  HEADER("over",4,0,0); *dp++ = &&OVER;
  HEADER("dig",3,0,0); *dp++ = &&DIG;
  HEADER("bury",4,0,0); *dp++ = &&BURY;
  HEADER("quit",4,0,0); *dp++ = &&QUIT;
  /* 2drop 2dup 2swap ?dup */
  HEADER("1+",2,0,0); *dp++ = &&INCREMENT;
  HEADER("1-",2,0,0); *dp++ = &&DECREMENT;
  HEADER("+",1,0,0); *dp++ = &&PLUS;
  HEADER("-",1,0,0); *dp++ = &&MINUS;
  HEADER("*",1,0,0); *dp++ = &&MULTIPLY;
  HEADER("/mod",4,0,0); *dp++ = &&DIVMOD;
  /* u/mod */
  HEADER("end",3,0,0); *dp++ = &&END;
  HEADER("lit:",4,0,0); *dp++ = &&LIT;
  HEADER("@",1,0,0); *dp++ = &&FETCH;
  HEADER("!",1,0,0); *dp++ = &&STORE;
  /* +! -! c@ c! c@c! cmove */
  
  HEADER("state",5,0,0); *dp++ = &&DOVAR; *dp++ = 0;

  HEADER("VERSION_MAJOR",13,0,0); *dp++ = &&LIT; *dp++ = VERSION_MAJOR;
  HEADER("VERSION_MINOR",13,0,0); *dp++ = &&LIT; *dp++ = VERSION_MINOR;
  HEADER("DOCOL",5,0,0); *dp++ = &&LIT; *dp++ = &&DOCOL;

  /* >r r> rsp@ rsp! rdrop */
  /* dsp@ dsp! */
  
  *sp++ = (cell_t)3; *sp++ = (cell_t)7; *sp++ = (cell_t)2;
  *dp++ = &&DIVMOD;
  *dp++ = &&SHOW_STACK; 
  *dp++ = &&QUIT;
  ip = (dp-3);
  NEXT;

DOCOL:
  *rp++ = ip++; NEXT;
END:
  ip = *--rp; NEXT;
DOVAR:
  *sp++ = ip++; NEXT;
DROP:
  sp--; NEXT;
SWAP:
  temp = *(sp-1);
  *(sp-1) = *(sp-2);
  *(sp-2) = temp;
  NEXT;
DUP:
  *sp++ = *(sp-1); NEXT;
OVER:
  *sp++ = *(sp-2); NEXT;
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
INCREMENT:
  *(sp-1) += 1; NEXT;
DECREMENT:
  *(sp-1) -= 1; NEXT;
PLUS:
  *(sp-2) += (intptr_t)*(sp-1); sp--; NEXT; /* test *((sp--)-1) */
MINUS:
  *(sp-2) -= (intptr_t)*(sp-1); sp--; NEXT; /* test *((sp--)-1) */
MULTIPLY:
  *(sp-2) = (cell_t)((intptr_t)*(sp-1) * (intptr_t)*(sp-2)); sp--; NEXT; /* test *((sp--)-1) */
DIVMOD:
  divmod_result = div((intptr_t)*(sp-2), (intptr_t)*(sp-1));
  *(sp-2) = (cell_t)divmod_result.quot;
  *(sp-1) = (cell_t)divmod_result.rem;
  NEXT;
LIT:
  *sp++ = *ip++; NEXT;
FETCH:
  /* *(sp-1) = (cell_t)*(*(sp-1)); NEXT; */
STORE:
  /* (cell_t)**(sp-1) = *((sp--)-2); NEXT; */
SHOW_STACK:
  fprintf(stdout, "[ ");
  for (temp_p = stack; temp_p < sp; temp_p++) {
    fprintf(stdout, "%ld ", (intptr_t)*temp_p);
  }
  fprintf(stdout, "]\n");
  NEXT;
QUIT:
  puts("bye");
  clean_metadata(dictionary, link);
  
  return 0;
}


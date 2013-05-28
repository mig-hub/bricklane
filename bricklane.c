#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define IMAGE_SIZE 65536
#define STACK_SIZE 64
#define RETURN_STACK_SIZE 8192
#define WORD_SIZE 32
#define BUFFER_SIZE 4096
#define NEXT w = *ip++; goto **w++
#define CELL_SIZE (sizeof(cell_t))
#define HEADER(N,L,I,H) *dp = link; link = dp++; \
  *dp = (word_metadata *)malloc(sizeof(word_metadata)); \
  ((word_metadata*)*dp)->length = L; \
  ((word_metadata*)*dp)->immediate = I; \
  ((word_metadata*)*dp)->hidden = H; \
  strcpy(((word_metadata*)*dp)->name, N); \
  dp++
#define DICT(V) *dp++ = V
#define PRIMITIVE(N,L,I,H,A) HEADER(N,L,I,H); DICT(A)
#define COMPILE(N) DICT(compile(N, link))

typedef void* cell_t;

typedef struct {
  unsigned int immediate : 1;
  unsigned int hidden : 1;
  int length;
  char name[WORD_SIZE];
} word_metadata;

void clean_metadata(cell_t *top) {
  while (top!=NULL) { free(*(top+1)); top = *top; };
}

void show_stack(cell_t *stack, cell_t *sp) {
  cell_t *temp_p;
  fprintf(stdout, "( ");
  for (temp_p = stack; temp_p < sp; temp_p++) {
    fprintf(stdout, "%ld ", *(intptr_t*)temp_p);
  }
  fprintf(stdout, ")\n");
}

cell_t* compile(char *word, cell_t *link) {
  cell_t *p = link;
  while (p!=NULL) {
    if (strcmp(word, ((word_metadata*)*(p+1))->name)==0) {
      return p+2;
    }
    p = *p;
  }
  return p;
}

int main(int argc, const char *argv[])
{
  cell_t stack[STACK_SIZE], *sp = stack;
  cell_t return_stack[RETURN_STACK_SIZE], *rp = return_stack;
  cell_t dictionary[IMAGE_SIZE], *dp = dictionary, *link = NULL, *ip, *w;
  intptr_t base = 10, state = 0;
  char word_buffer[WORD_SIZE], *word_p = word_buffer;
  cell_t *temp_p;
  char temp_char, *end;
  div_t divmod_result;

  PRIMITIVE("debug",5,0,0,&&DEBUG);
  PRIMITIVE("show-stack",10,0,0,&&SHOW_STACK);
  PRIMITIVE("quit",4,0,0,&&QUIT);
  PRIMITIVE("number",6,0,0,&&NUMBER);
  PRIMITIVE("header",6,0,0,&&CREATE_HEADER);
  PRIMITIVE(",",1,0,0,&&COMMA);
  PRIMITIVE("push[]",9,0,0,&&PUSH_LITERAL);
  PRIMITIVE("@",1,0,0,&&FETCH);
  PRIMITIVE("!",1,0,0,&&STORE);
  PRIMITIVE("?",1,0,0,&&CHOICE);
  PRIMITIVE("drop",4,0,0,&&DROP);
  PRIMITIVE("swap",4,0,0,&&SWAP);
  PRIMITIVE("dup",3,0,0,&&DUP);
  PRIMITIVE("over",4,0,0,&&OVER);
  PRIMITIVE("dig",3,0,0,&&DIG);
  PRIMITIVE("bury",4,0,0,&&BURY);
  /* 2drop 2dup 2swap ?dup */
  PRIMITIVE("1+",2,0,0,&&INCREMENT);
  PRIMITIVE("1-",2,0,0,&&DECREMENT);
  PRIMITIVE("+",1,0,0,&&PLUS);
  PRIMITIVE("-",1,0,0,&&MINUS);
  PRIMITIVE("*",1,0,0,&&TIMES);
  PRIMITIVE("/mod",4,0,0,&&DIVMOD);
  /* u/mod */
  PRIMITIVE("and",3,0,0,&&AND);
  PRIMITIVE("or",2,0,0,&&OR);
  PRIMITIVE("xor",3,0,0,&&XOR);
  PRIMITIVE("not",3,0,0,&&NOT);
  PRIMITIVE("=",1,0,0,&&EQ);
  PRIMITIVE("~=",2,0,0,&&NEQ);
  PRIMITIVE("<",1,0,0,&&LT);
  PRIMITIVE(">",1,0,0,&&GT);
  PRIMITIVE("<=",2,0,0,&&LTE);
  PRIMITIVE(">=",2,0,0,&&GTE);

  /* variables */
  HEADER("base",4,0,0); DICT(&&DEBUG);
  /* COMPILE("push[]"); DICT(&base); COMPILE("unnest"); */

  /* constants */
  PRIMITIVE("nest-token",10,0,0,&&NEST_TOKEN);
  /* DICT(&&DODOES); DICT(dp+2); DICT(&&NEST); */
  /* COMPILE("@"); COMPILE("unnest"); */

  PRIMITIVE("word:",5,0,0,&&WORD);
  PRIMITIVE("find",4,0,0,&&FIND);
  PRIMITIVE("token",5,0,0,&&TOKEN);
  PRIMITIVE("execute",7,0,0,&&EXECUTE);
  PRIMITIVE("unnest",6,0,0,&&UNNEST);
  PRIMITIVE("jump[]",6,0,0,&&JUMP);

  HEADER("number:",7,0,0);
  DICT(&&NEST); COMPILE("word:");
  COMPILE("number"); COMPILE("unnest");

  HEADER("header:",7,0,0);
  DICT(&&NEST); COMPILE("word:");
  COMPILE("header"); COMPILE("unnest");

  HEADER("token:",6,0,0);
  DICT(&&NEST); COMPILE("word:"); COMPILE("find");
  COMPILE("token"); COMPILE("unnest");

  HEADER("interpret:",10,0,0);
  DICT(&&NEST); COMPILE("token:"); 
  COMPILE("execute"); COMPILE("unnest");
  
  HEADER("reset",5,0,0);
  DICT(&&NEST); COMPILE("interpret:"); COMPILE("jump[]");
  DICT((cell_t)-2);

  ip = dp-3;
  NEXT;

DEBUG: *sp++ = &base; NEXT;

NEST: *rp++ = ip; ip = w; NEXT;
UNNEST: ip = *--rp; NEXT;

DODOES:
  *sp++ = w+1;
  if(*w) { *rp++ = ip; ip = *w; }
  NEXT;

NEST_TOKEN: *sp++ = &&NEST; NEXT;

WORD:
  word_p = word_buffer;
  temp_char = getchar();
  while(isspace(temp_char)) {
    temp_char = getchar();
  }
  while(!isspace(temp_char)) {
    if (temp_char==EOF) goto QUIT;
    *word_p++ = temp_char;
    temp_char = getchar();
  }
  *word_p = '\0';
  *sp++ = word_buffer;
  *sp++ = (cell_t)(word_p - word_buffer);
  NEXT;
FIND:
  temp_p = link;
  --sp;
  while (temp_p!=NULL) {
    /* printf("%s\n", ((word_metadata*)*(temp_p+1))->name); */
    if (strcmp(*(sp-1), ((word_metadata*)*(temp_p+1))->name)==0) {
      *(sp-1) = temp_p;
      break;
    }
    temp_p = *temp_p;
  };
  if (temp_p==NULL) *(sp-1) = 0;
  NEXT;
TOKEN: *(sp-1) += (CELL_SIZE+CELL_SIZE); NEXT;
EXECUTE: w = *--sp; goto **w++;
JUMP: ip += (intptr_t)*ip; NEXT;

NUMBER:
  --sp;
  *(sp-1) = (cell_t)strtol(word_buffer, &end, base);
  /* *sp++ = (cell_t)((!*end) ? -1 : 0); */
  NEXT;

SHOW_STACK: show_stack(stack,sp); NEXT;
CREATE_HEADER:
  w = *--sp;
  HEADER((char*)*--sp,(intptr_t)w,0,0); NEXT;
COMMA: DICT(*--sp); NEXT;
PUSH_LITERAL: *sp++ = *ip++; NEXT;
FETCH: w = *--sp; *sp++ = *w; NEXT;
STORE: w = *--sp; *w = *--sp; NEXT;
CHOICE:
  *(sp-3) = *(sp-3) ? *(sp-2) : *(sp-1);
  sp -= 2; NEXT;
DROP: sp--; NEXT;
SWAP:
  w = *(sp-1); *(sp-1) = *(sp-2);
  *(sp-2) = w; NEXT;
DUP: *sp++ = *(sp-1); NEXT;
OVER: *sp++ = *(sp-2); NEXT;
DIG:
  w = *(sp-3); *(sp-3) = *(sp-2);
  *(sp-2) = *(sp-1); *(sp-1) = w;
  NEXT;
BURY:
  w = *(sp-1); *(sp-1) = *(sp-2);
  *(sp-2) = *(sp-3); *(sp-3) = w;
  NEXT;
INCREMENT: *(sp-1) += 1; NEXT;
DECREMENT: *(sp-1) -= 1; NEXT;
PLUS: *((sp--)-2) += (intptr_t)*(sp-1); NEXT;
MINUS: *((sp--)-2) -= (intptr_t)*(sp-1); NEXT;
TIMES: *((sp--)-2) = (cell_t)((intptr_t)*(sp-1) * (intptr_t)*(sp-2)); NEXT;
DIVMOD:
  divmod_result = div((intptr_t)*(sp-2), (intptr_t)*(sp-1));
  *(sp-2) = (cell_t)divmod_result.quot;
  *(sp-1) = (cell_t)divmod_result.rem;
  NEXT;
AND: *((sp--)-2) = (cell_t)((intptr_t)*(sp-1) & (intptr_t)*(sp-2)); NEXT;
OR: *((sp--)-2) = (cell_t)((intptr_t)*(sp-1) | (intptr_t)*(sp-2)); NEXT;
XOR: *((sp--)-2) = (cell_t)((intptr_t)*(sp-1) ^ (intptr_t)*(sp-2)); NEXT;
NOT: *(sp-1) = (cell_t)~(intptr_t)*(sp-1); NEXT;
EQ: *sp++ = (cell_t)-(*--sp == *--sp); NEXT;
NEQ: *sp++ = (cell_t)-(*--sp != *--sp); NEXT;
LT: *sp++ = (cell_t)-(*--sp < *--sp); NEXT;
GT: *sp++ = (cell_t)-(*--sp > *--sp); NEXT;
LTE: *sp++ = (cell_t)-(*--sp <= *--sp); NEXT;
GTE: *sp++ = (cell_t)-(*--sp >= *--sp); NEXT;

QUIT: clean_metadata(link);
  return 0;
}


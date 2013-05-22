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
  char base = 10, state = 0;
  char word_buffer[WORD_SIZE], *word_p = word_buffer;
  cell_t temp, *temp_p;
  char temp_char, *end;
  div_t divmod_result;

  PRIMITIVE("debug",5,0,0,&&DEBUG);
  PRIMITIVE("show-stack",10,0,0,&&SHOW_STACK);
  PRIMITIVE("quit",4,0,0,&&QUIT);
  PRIMITIVE("number",6,0,0,&&NUMBER);

  PRIMITIVE("word:",5,0,0,&&WORD);
  PRIMITIVE("find",4,0,0,&&FIND);
  PRIMITIVE("token",5,0,0,&&TOKEN);
  PRIMITIVE("execute",7,0,0,&&EXECUTE);
  PRIMITIVE("unnest",6,0,0,&&UNNEST);
  
  HEADER("interpret:",10,0,0);
  DICT(&&NEST); COMPILE("word:"); COMPILE("find");
  COMPILE("token"); COMPILE("execute"); COMPILE("unnest");
  
  PRIMITIVE("jump",4,0,0,&&JUMP);
  
  HEADER("reset",5,0,0);
  DICT(&&NEST); COMPILE("interpret:"); COMPILE("jump");
  DICT((cell_t)-2);

  ip = dp-3;
  NEXT;

DEBUG: puts("debug"); NEXT;

NEST: *rp++ = ip; ip = w; NEXT;
UNNEST: ip = *--rp; NEXT;

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
  show_stack(stack,sp);
  printf("%s\n", *(sp-1));
  *(sp-1) = (cell_t)strtol(word_buffer, &end, base);
  /* *sp++ = (cell_t)((!*end) ? -1 : 0); */
  NEXT;

SHOW_STACK: show_stack(stack,sp); NEXT;
QUIT: clean_metadata(link);
  return 0;
}


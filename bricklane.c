#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef void* cell_t;

typedef struct {
  unsigned int immediate : 1;
  unsigned int hidden : 1;
  int length;
  char name[WORD_SIZE];
} word_metadata;

void clean_metadata(cell_t * begining, cell_t * top) {
  do {
    free(*(top+1)); 
    top = *top;
  } while (top!=begining);
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

  PRIMITIVE("unnest",6,0,0,&&UNNEST);
  PRIMITIVE("bye",3,0,0,&&BYE);
  PRIMITIVE("debug",5,0,0,&&DEBUG);

  // Fake compiled word which does debug twice
  DICT(&&NEST); DICT(dp-2); DICT(dp-3); DICT(dp-10);

  DICT(dp-4); DICT(dp-9); // double-debug bye
  ip = dp-2;
  NEXT;

DEBUG: puts("debug"); NEXT;
NEST: *rp++ = ip; ip = w; NEXT;
UNNEST: ip = *--rp; NEXT;
BYE: puts("bye");
/* QUIT: clean_metadata(dictionary, link); */
  return 0;
}


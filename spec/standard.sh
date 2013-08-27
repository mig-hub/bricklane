describe "Bricklane Standard Library"

script_is() {
  R=$(echo "$*" | cat standard.bl - | bricklane)
}

stdout_is() {
  test "$R" = "$1"
}

stack_is() {
  stdout_is "( $* )"
}

it_has_shortcuts_for_compiling() {
  script_is create-word: double compile: dup compile: + compile: unnest number: 16 double show-stack
  stack_is 32
}

it_has_shortcuts_for_compiling_literals() {
  script_is create-word: 2+ compile-number: 2 compile: + compile: unnest number: 4 2+ show-stack
  stack_is 6
}


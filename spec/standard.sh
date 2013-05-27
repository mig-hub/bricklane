describe "Bricklane Standard Library"

it_has_shortcut_for_compiling() {
  R=$(echo create-word: double compile: dup compile: + compile: unnest number: 16 double show-stack | cat standard.bl - | bricklane)
  test "$R" = "( 32 )"
}

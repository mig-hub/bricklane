#! /usr/bin/env roundup

describe "Bricklane Core"

it_shows_the_stack() {
  R=$(echo number: 2066 number: 42 show-stack| bricklane)
  test "$R" = "( 2066 42 )"
}

it_shows_an_empty_stack() {
  R=$(echo show-stack | bricklane)
  test "$R" = "( )"
}

it_drops() {
  R=$(echo number: 355 number: 113 drop show-stack | bricklane)
  test "$R" = "( 355 )"
}

it_swaps() {
  R=$(echo number: 113 number: 355 swap show-stack | bricklane)
  test "$R" = "( 355 113 )"
}

it_duplicates() {
  R=$(echo number: 5 number: 3 number: 2 number: 1 dup show-stack | bricklane)
  test "$R" = "( 5 3 2 1 1 )"
}

it_duplicates_over() {
  R=$(echo number: 0 number: -3 over show-stack | bricklane)
  test "$R" = "( 0 -3 0 )"
}

it_digs() {
  R=$(echo number: 1 number: 2 number: 3 dig show-stack | bricklane)
  test "$R" = "( 2 3 1 )"
}

it_buries() {
  R=$(echo number: 1 number: 2 number: 3 bury show-stack | bricklane)
  test "$R" = "( 3 1 2 )"
}

it_increments_and_decrements() {
  R=$(echo number: 354 1+ number: 114 1- show-stack | bricklane)
  test "$R" = "( 355 113 )"
}

it_adds() {
  R=$(echo number: -20 number: 62 + show-stack | bricklane)
  test "$R" = "( 42 )"
}

it_substracts() {
  R=$(echo number: 20 number: -22 - show-stack | bricklane)
  test "$R" = "( 42 )"
}

it_multiplies() {
  R=$(echo number: -157 number: -2 \* show-stack | bricklane)
  test "$R" = "( 314 )"
}

it_divides_with_remain() {
  R=$(echo number: 355 number: 113 /mod show-stack | bricklane)
  test "$R" = "( 3 16 )"
}

it_compiles_in_interpreted_mode() {
  R=$(echo header: double nest-token , token: dup , token: + , token: unnest , number: 16 double show-stack | bricklane)
  test "$R" = "( 32 )"
}

it_makes_choices() {
  R=$(echo number: -1 number: 355 number: 113 \? number: 0 number: 34 number: 21 \? show-stack | bricklane )
  test "$R" = "( 355 21 )"
}


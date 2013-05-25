#! /usr/bin/env roundup

describe "Bricklane Core"

it_shows_the_stack() {
  R=$(echo number: 2066 number: 42 show-stack quit | bricklane)
  test "$R" = "( 2066 42 )"
}

it_shows_an_empty_stack() {
  R=$(echo show-stack quit | bricklane)
  test "$R" = "( )"
}

it_drops() {
  R=$(echo number: 255 number: 113 drop show-stack quit | bricklane)
  test "$R" = "( 255 )"
}

it_swaps() {
  R=$(echo number: 113 number: 255 swap show-stack quit | bricklane)
  test "$R" = "( 255 113 )"
}

it_duplicates() {
  R=$(echo number: 5 number: 3 number: 2 number: 1 dup show-stack quit | bricklane)
  test "$R" = "( 5 3 2 1 1 )"
}

it_duplicates_over() {
  R=$(echo number: 0 number: -3 over show-stack quit | bricklane)
  test "$R" = "( 0 -3 0 )"
}

it_digs() {
  R=$(echo number: 1 number: 2 number: 3 dig show-stack quit | bricklane)
  test "$R" = "( 2 3 1 )"
}

it_buries() {
  R=$(echo number: 1 number: 2 number: 3 bury show-stack quit | bricklane)
  test "$R" = "( 3 1 2 )"
}


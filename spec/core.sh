#! /usr/bin/env roundup

describe "Bricklane Core"

script_is() {
  R=$(echo "$*" | bricklane)
}

stdout_is() {
  test "$R" = "$1"
}

stack_is() {
  stdout_is "( $* )"
}

it_shows_the_stack() {
  script_is number: 2066 number: 42 show-stack
  stack_is  2066 42
}

it_shows_an_empty_stack() {
  script_is show-stack
  stdout_is "( )"
}

it_drops() {
  script_is number: 355 number: 113 drop show-stack
  stack_is  355
}

it_swaps() {
  script_is number: 113 number: 355 swap show-stack
  stack_is  355 113
}

it_duplicates() {
  script_is number: 5 number: 3 number: 2 number: 1 dup show-stack
  stack_is  5 3 2 1 1
}

it_duplicates_over() {
  script_is number: 0 number: -3 over show-stack
  stack_is  0 -3 0
}

it_digs() {
  script_is number: 1 number: 2 number: 3 dig show-stack
  stack_is  2 3 1
}

it_buries() {
  script_is number: 1 number: 2 number: 3 bury show-stack
  stack_is  3 1 2
}

it_increments_and_decrements() {
  script_is number: 354 1+ number: 114 1- show-stack
  stack_is  355 113
}

it_adds() {
  script_is number: -20 number: 62 + show-stack
  stack_is  42
}

it_substracts() {
  script_is number: 20 number: -22 - show-stack
  stack_is  42
}

it_multiplies() {
  script_is number: -157 number: -2 \* show-stack
  stack_is  314
}

it_divides_with_remain() {
  script_is number: 355 number: 113 /mod show-stack
  stack_is  3 16
}

it_compiles_in_interpreted_mode() {
  script_is header: double nest-token , token: dup , token: + , token: unnest , number: 16 double show-stack
  stack_is  32
}

it_makes_choices() {
  script_is number: -1 number: 355 number: 113 \? number: 0 number: 34 number: 21 \? show-stack
  stack_is  355 21
}

it_fetches_and_stores() {
  script_is base @ number: 2 base ! base @ show-stack
  stack_is 10 2
  script_is number: 8 base -! base @ show-stack
  stack_is 2
  script_is number: -8 base +! base @ show-stack
  stack_is 2
}

logical_case() {
  script_is number: $1 number: $2 $3 show-stack
  stack_is $4
}

logical_cases() {
  logical_case 0 0 $1 $2
  logical_case 0 -1 $1 $3
  logical_case -1 0 $1 $4
  logical_case -1 -1 $1 $5
}

it_is_logical() {
  logical_cases and 0 0 0 -1
  logical_cases or 0 -1 -1 -1
  logical_cases xor 0 -1 -1 0
  script_is number: 0 not show-stack
  stack_is -1
  script_is number: -1 not show-stack
  stack_is 0
}

compare_case() {
  script_is number: $1 number: $2 $3 show-stack
  stack_is $4
}

compare_cases() {
  compare_case 42 42 $1 $2
  compare_case -42 -42 $1 $3
  compare_case 42 -42 $1 $4
  compare_case -42 42 $1 $5
}

it_compares() {
  compare_cases "=" -1 -1 0 0
  compare_cases "~=" 0 0 -1 -1
  compare_cases "<" 0 0 0 -1
  compare_cases ">" 0 0 -1 0
  compare_cases "<=" -1 -1 0 -1
  compare_cases ">=" -1 -1 -1 0
}


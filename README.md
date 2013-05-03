In Progress
===========

Roadmap
=======

- Get a working/testable version asap
  - Sacrifice speed at first
  - Use libraries if needed at first
  - And then refactor when I have automated tests
  - Maybe use roundup for testing the primitives
- Do not do error checking like stack overflow until I have a satisfactory decision (or solution)
- When I get a working interpreter, make `key` and `emit` buffered
- Make sure I have the most appropriate name for everything
- Make a choice regarding strings
  - Null terminated strings?
  - Address+count stack strings?
  - Something else?
  - Some forths have a null AND the count
  - Stack strings make substrings simple but if I go for null+count, it makes this less obvious
  - Some forths have both (reva)
- Work on the dictionary
  - Flexible length?
  - Char based?

Features wishlist
=================

- Implement quotes
- Study `hype` and other OOP to see if I can make a very simple object system like Io (differential inheritance, prototype)
  - Namespaces could be based around this
- Coroutines or something approaching


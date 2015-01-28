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
- Try to replace jump[] =0jump[] with something based on tokens and execute
  - This is how I would like to do it on high level anyway
  - It might need recursion
- Recursion has 2 possible implementations
  - The name reference itself and old version is called with a special word (like super in oop)
  - The name refers to the old version and current word is called with `recurse` (prefered)
  - It all depends on hidden words
  - End of definition can make a work un-hidden but it needs to work on low level

Features wishlist
=================

- Implement quotes
- Study `hype` and other OOP to see if I can make a very simple object system like Io (differential inheritance, prototype)
  - Namespaces could be based around this
- Coroutines or something approaching


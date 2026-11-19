Checkpoint 6 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about 4 hours to do. I did attend the lab session.

Program Structure and Design of the Router [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]:

Routing Table Design:
The general idea is to keep the routes in reverse sorted order based on prefix 
length so that the first match is the longest match. A vector seemed good enough, 
with O(n) insert and O(n) matching, with matching being slightly more efficient 
due to the fact that it quits the moment a match is found.

The insert being O(n) should be okay in this situation since I imagine adding a 
new rule is not too common.


Matching:
The match algorithm itself treats the two addresses as bits and creates a mask 
of 1s for the bits we want to match. Then, it ands the mask and the addresses 
and checks whether they are equivalent. Since shifting by 32 bits is undefined 
for uint32, if the prefix length is 0 we say it always matches.

Implementation Challenges:
[]
I was getting an error that said my datagram was "bad" even though I decremented 
the ttl. The bug was that I did not recompute the checksum, which upon fixing, 
the code passed all tests.

Remaining Bugs:
If two rules have the same prefix and prefix length but route to different 
next hops, the current router code does not handle that and simply
chooses the rule first inserted. This may be an issue if a rule were to be 
updated, but since removing and updating rules was not part of the checkpoint's 
scope it remains unhandled.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

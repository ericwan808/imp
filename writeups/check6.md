Checkpoint 6 Writeup
====================

My name: [your name here]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about [n] hours to do. I [did/did not] attend the lab session.

Program Structure and Design of the Router [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: []

save forwarding rules in a sorted order from highest prefix length (most specific) 
to lowest. When checking, simply return the first match which will have the 
highest prefix length.

Implementation Challenges:
[]
decrement ttl and compute checksum,
iterating through c++ vectors is a pain when I'm used to 
using normal indexing

Remaining Bugs:
If two rules have the same prefix and prefix length but route to different 
next hops, the current router code does not handle that and simply
chooses the rule first inserted. This may be an issue if a rule were to be 
updated, but since removing and updating rules was not part of the checkpoint 
scope it remains unhandled.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

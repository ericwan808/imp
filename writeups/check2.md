Checkpoint 2 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about 5 hours to do. I did attend the lab session.

Describe Wrap32 and TCPReceiver structure and design. [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

New variables:
There was not much to add on the data structure. For the receiver, all I added 
was a private variable to keep track of the ISN. I made it optional so that we 
can also tell if the connection has even been fully established yet. 

Errors and RST:
I decided to connect the RST and byte stream error functions directly. That is 
to say that one will trigger the other. In checkpoint 0, I did not implement 
any other way for the byte stream to receive an error internally, so this error 
would have to originate from either user calling set_error.


Implementation Challenges:

unwrap:
I solved this little puzzle by drawing out a number line spaced by 2^32 byte 
chunks. The first possible absolute seqno is easy to calculate since unsigned 
int wraps its subtraction on its own. From there it just came down to imagining 
the possible places the checkpoint could be relative to the possible absolute 
seqnos. Figuring out whether to take the lower or higher possible choice is 
done by comparing their difference with 2^31, or half of a 2^32 byte chunk.
Remaining Bugs:
[]

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [submit as GitHub PR
  and include URL here]

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

There was not much to add on the data structure. For the receiver, all I added 
was a private variable to keep track of the ISN. The recieve function simply 
checks for some errors, gets the right stream index using unwrap (plus some 
extra additions to account for SYN), and inserts into the reassembler which 
does the hard work. The send function is similarly mostly error checking and 
preparing the required message.


Implementation Challenges:
Converting a raw seqno to the absolute seqno and then to the stream index was 
definitely the most challenging part of the checkpoint. The first thing I did 
zero-out the ISN to get the first possible absolute seqno. From there, it helped 
to draw a diagram on paper of repeating chunks of 2^32 and see where the 
possible absolute seqnos could be relative to the checkpoint. Conceptually it 
was somewhat difficult to imagine when we should take the seqno above or below 
the checkpoint, but again the paper diagram helped a lot.

Converting between absolute seqno and the stream index was much easier. In the 
recieve function we don't need to account for the position FIN would take, so we 
only need to subtract out the one position SYN takes before passing things to 
the reassembler. In the send function if we are sending an ackno then we must 
have recieved a SYN flag, so we can always include the +1 required for the 
absolute seqno. Also if the stream is closed we must have recieved a FIN flag, 
so we can include that +1 as well.

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

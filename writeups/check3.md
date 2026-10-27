Checkpoint 3 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: khliu

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about 8 hours to do. I did attend the lab session.

Program Structure and Design of the TCPSender [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]:

next_seqno_ and acked_:
These two variables keep track of what the sender knows about the state of the 
sequence numbers. In between are all the sequence numbers in flight, which makes 
the corresponding function very simple. I had a thought of not keeping track of 
acked_, and instead calculating it from the oustanding messages. In the cases 
where there was no outstanding message the last ackno received should be the 
same as next_seqno_, but in terms of implementation and ease of understanding 
the code it was better to just keep track of the last ackno as acked_ in my opinion.

SYN and FIN in push:
I went through multiple iterations of push. The difficulty (at least 
in my mind) is how know when to send SYN and FIN, while not repeating them. 
Originally I tried to make SYN is own thing by sending it before everything else 
and only moving on after it was acked. However, this felt clumsy and I was told 
this would not be an allowed design choice. Instead, I just checked if next_seqno_ 
was 0, which was a much simpler solution.

For FIN, I originally had another boolean state to determine if FIN was sent or 
not. We cannot tell if FIN has been sent or not using the outstanding 
messages. But while writing this writeup, I realized you could determine whether 
FIN was sent using bytes_popped and is_finished by comparing them to next_seqno_.
If FIN was sent, then next_seqno_ should exceed bytes_popped by 2 due to SYN and 
FIN.


Implementation Challenges:
Window size:
An interesting requirement was that we should have the option to bypass the 
window size if its set to 0, but cannot outright set window size to 1 due to 
needing it for backoff. But if the check was placed in push, we need a way to 
ensure we cannot just keep pushing 1 seqno every time we call it. The way I 
solved this was to include seqnos in flight in my calculation of availible space 
push could send in. Thus, if window size is set to 0 we can get +1, but that 
seqno takes a space in flight so we cannot send another one until it is acked.


Results from section 4:
Kenneth (khliu) sent me a large file of 30 megabytes:
eric@eric-laptop:~/Desktop/cs144/imp$ ls -l /tmp/big-received.txt
-rw-rw-r-- 1 eric eric 30000000 Oct 21 20:34 /tmp/big-received.txt
eric@eric-laptop:~/Desktop/cs144/imp$ sha256sum /tmp/big-received.txt
736e57035a54f61a7b320b4b471d2783f53c975f4e6cf324739a192689a9c3c6  /tmp/big-received.txt

from Kenneth:
ls -l /tmp/big.txt
-rw-rw-r-- 1 cs144 cs144 30000000 Oct 22 03:34 /tmp/big.txt
sha256sum /tmp/big.txt
736e57035a54f61a7b320b4b471d2783f53c975f4e6cf324739a192689a9c3c6  /tmp/big.txt

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

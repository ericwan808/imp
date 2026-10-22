Checkpoint 3 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: khliu

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about [n] hours to do. I [did/did not] attend the lab session.

Program Structure and Design of the TCPSender [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: []

How to determine whether or not to send SYN:
1. send SYN until ackno, then send rest of byte stream
2. send SYN once and only resend if timeout

Choose 1 so I don't have to add another state to keep track of and also makes it
easier to calculate the current abs_seqno

Cannot do that, so instead have a choice between:
1. store next abs sequence number to send for the first time
2. store booleans for SYN and FIN

1 feels more elegant

I use a list instead of a queue because they have the same functionality and I 
read more of the cppreference on list compared to queue.


fin_sent used since after we send it and everything is acked, we cannot tell 
what has been sent previously so we might resend fin

what occurs when receive gets the same seqno

Implementation Challenges:
when can win_size be artificially set to 1?
Initially interpreted as "at most once per received request", which made
implementation difficult since repeated pushes could send 1 new seqno total before 
having to wait for an ackno. However, making it per-push was much easier.


eric@eric-laptop:~/Desktop/cs144/imp$ </dev/null ./build/apps/tcp_native -l 0 9090 > /tmp/big-received.txt
DEBUG: Listening for incoming connection...
DEBUG: New connection from 10.144.0.86:16471.
DEBUG: Outbound stream to 10.144.0.86:16471 finished.
DEBUG: Inbound stream from 10.144.0.86:16471 finished.
eric@eric-laptop:~/Desktop/cs144/imp$ ls -l /tmp/big-received.txt
-rw-rw-r-- 1 eric eric 30000000 Oct 21 20:34 /tmp/big-received.txt
eric@eric-laptop:~/Desktop/cs144/imp$ sha256sum /tmp/big-received.txt
736e57035a54f61a7b320b4b471d2783f53c975f4e6cf324739a192689a9c3c6  /tmp/big-received.txt


from Kenneth:
sha256sum /tmp/big.txt
736e57035a54f61a7b320b4b471d2783f53c975f4e6cf324739a192689a9c3c6  /tmp/big.txt

ls -l /tmp/big.txt
-rw-rw-r-- 1 cs144 cs144 30000000 Oct 22 03:34 /tmp/big.txt


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

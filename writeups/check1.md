Checkpoint 1 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: Kenneth Liu, Erica Dong, Harrison

I would like to thank/reward these classmates for their help: merrittb

This lab took me about [n] hours to do. I did attend the lab session.

I was surprised by or edified to learn that: [describe]

Report from the hands-on component of the lab checkpoint: [include
information from 2.1(4), and report on your experience in 2.2]
2.1
a. Average delay: 41.627 ms
b. Delivery rate: 100%, Loss rate: 0%
c. No
d. "tcpdump: listening on wg0, link-type RAW (Raw IP), snapshot length 262144 bytes
20:16:30.052696 IP 10.144.0.10 > 10.144.0.42: ICMP echo request, id 27243, seq 1, length 64
20:16:30.073061 IP 10.144.0.42 > 10.144.0.10: ICMP echo reply, id 27243, seq 1, length 64"
e. Yes, the version is 4, header length is 20 bytes, etc.
f. ttl is decremented by one on the reciever compared to sender, header checksum is different

2.2
We encountered some issues with sending the datagram at first, but this was fixed once we changed the Address field of the send function to include the full ip address of the destination.

Describe Reassembler structure and design. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

Implementation Challenges:
[]

Remaining Bugs:
[]

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

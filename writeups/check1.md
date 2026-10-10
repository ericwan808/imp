Checkpoint 1 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: Kenneth Liu, Erica Dong, Harrison Chen

I would like to thank/reward these classmates for their help: merrittb

This lab took me about 6 hours to do. I did attend the lab session.

I was surprised by or edified to learn that: you can simply write a c++ string
in a certain formatting to create an acceptable IP datagram and that there is no
special data structure in c++ for it.

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
We encountered some issues with sending the datagram at first, but this was 
fixed once we changed the Address field of the send function to include the full 
ip address of the destination.

Describe Reassembler structure and design. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

The simplest approach I could think of was to simply store the segments without
any processing in a list. Then, I would greedily push as many segments as I 
could into the Writer. The trigger for pushing a segment would be that its 
first_index was less than or equal to the next needed byte by the Writer. Thus, 
I decided that the list should be sorted by first_index so we only need to check 
whether the first element of the list could be pushed.

I had a little trouble deciding between using a vector or a (linked) list in my 
implementation. List is linear time to insert a sorted element since we need to 
iterate through each link to get there, but it is constant in removing. Vector 
can use binary search to find the place to insert in log time, but could still 
take linear time to insert due to shifting the other elements in the array. 
Removing an element can still be constant if we sort in reverse and pop the last
 element. For simplicity of implementation and the slight possibility of a 
speedup on insert, I chose the list.

The reassembler is quite slow with an overlap. Its speeds are 42 Gbit/s without 
overlap, and 4 Gbit/s with a 10x overlap. I believe this comes from the fact 
that I need to sort each new arriving segment while also processing them for 
pushing even if the amount of new data they contribute is minimal.

To try and improve the throughput I decided to trim incoming segments before 
inserting them into the list. This would reduce the amount of data stored and 
also simplify the push_until_gap and count_pending_bytes functions at the 
expense of a more complex insert function.



Implementation Challenges:
The largest implementation bug was closing the stream when any byte of the 
last substring was pushed, rather than waiting until all bytes of the last 
substring were pushed. This alone was solved by checking that the bytes pushed 
(aka the next needed byte) was after the last byte of the last substring. 
However, getting the code to look clean while also checking for other things like 
whether to skip a segment entirely took some intermediate variables.

Remaining Bugs:
[]

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

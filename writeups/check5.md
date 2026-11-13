Checkpoint 5 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about 4 hours to do. I did not attend the lab session.

Program Structure and Design of the NetworkInterface:
The primary data structures are two maps: one for translating ip to ethernet 
addressess, and the other for keeping track of ARP information. Whenever an ARP 
message is sent or received, the second map is updated with the newest information. 
Furthermore, all timeout updates to the ARP map are done only when the relevent 
ip address is about to be used. This keeps the tick function very simple and 
allows stale information to hang around if it is not causing harm.

Prior Design Considerations:
Originally, I had a seperate list for ARP requests waiting, but this required 
searching through the list upon getting an ARP response. I would've also had to 
search through it on every tick to see which ARP requests are stale. There was 
also once a queue for ip timeout. However, receiving any ARP should refresh the 
timeout, which is difficult to do on a queue without popping and re-pushing the 
entire thing just to update one element in the middle.

Another design consideration was to merge everything into a single map containing 
ARP information and ethernet addresses for each ip address. I decided against 
this mainly for the sake of understandability of the design. I like how each 
map has its own segmented responsibility.

ARPRequest object:
The "sent_time" and "received_time" fields are used to keep track of whether this 
ARP request or ethernet address is stale. The boolean "sent" in the object was 
necessary because if the current time was less than 5 seconds, every ARP would 
assume it had been sent less than 5 seconds ago since sent_time is an unsigned 
int and cannot go below 0. I did not make sent_time a signed int because I 
wanted to keep it consistent with the data type that keeps track of the current 
time. "waiting_datagrams" is a list instead of a queue mainly for the clear 
function so I could remove all stale datagrams easily if the ARP request timed out.

Implementation Challenges:
I had a small hiccup in getting a queue in a map to update. Turns out I just 
needed to change the variable on line 92 in network_interface.cc to a reference.

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

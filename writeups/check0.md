Checkpoint 0 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: Calista Woo, Kenneth Liu, Logan Wang, Ethan Lai, Greg Li

I would like to credit/thank these classmates for their help: hchen0

This lab took me about 10 hours to do. I did attend the lab session.

I was surprised by or edified to learn that: you have to cast a string to string_view before taking a substr if you want to return that substring.

Describe ByteStream implementation, focusing on the most salient
design choices. [Describe data structures and approach taken. Describe
alternative designs considered or tested.  Describe benefits and
weaknesses of your design compared with alternatives -- perhaps in
terms of simplicity/complexity, risk of bugs, asymptotic performance,
empirical performance, required implementation time and difficulty,
and other factors. Include any measurements if applicable.]

I initially chose to implement the buffer with a string and its builtin functions to implement push, peek, and pop.
Size enforcement had to be done manually. The ability to specify a certain range on append and erase was useful to handle this enforcement.
It was rather simple to implement, but the throughput suffers with many pops due to the inefficiency of erasing from the start of a string.
When we do many small erases to the start of a string, we need to shift all the remaining characters forward.
Thus, the complexity of push is O(n) due to append and the complexity of pop is O(n) due to erase.
Peeking is very simple since we can just return a string_view to the entire buffer in O(1).

To make pop more efficient, I opted to change the implementation to a ring buffer queue, where the allocated string acts as a storage space for a moving window to be the actual buffer.
The primary concern is the cases where the tail is "before" the head.
In push and pop, we must check for size constraints in the same way as the simpler impelementation, but also wrap around the ring when the tail exceeds the capacity. There should be no cases where the tail overtakes the head due to the size checking.
Pop is now much simpler and involves moving the head to "deallocate" part of the buffer in O(1).
Peek is slightly more complex (with challenges described below), but remains O(1) due to the fact that substr is O(1) on string_view objects since we don't copy the string itself.
Push uses replace as a sort of memcpy function to move chunks of data into the buffer. The only real difference is that we may have to cut the data into two pieces when it loops around the ring. Also, to make sure replace does not throw an error, buffer is initialized to be the full capacity using resize. It is still O(n) to move all the data into the buffer.

The (rounded) throughput in speed test for the simpler implementation was: (pop lenght 4096) 13 Gbit/s, (pop lenght 128) 3 Gbit/s, (pop lenght 32) 1 Gbit/s
In the ring buffer queue implementation: 13 Gbit/s, 10 Gbit/s, 8 Gbit/s
This should match our expectations, since using erase was slowest when we did many small pops.

Implementation Challenges:
In the peek function, I encountered a stack-use-after-return error when trying to implement the ring buffer queue. Using gdb, it seemed that the initial return value was correct, but then the value would get corrupted when the function returned.
I thought it might have something to do with substr since I know that function creates a temporary copy of a string. I tried storing the substr in a string and then returning it, but the problem persisted.
I realized later that peek returned a string_view and not a string.
I checked stack overflow (https://stackoverflow.com/questions/46032307/how-to-efficiently-get-a-string-view-for-a-substring-of-stdstring) and fixed the problem by casting the buffer to a string_view before taking the substr.
The unfortunate downside to this is that we cannot use append on a string_view, so sometimes we must use 2 peeks to get the entire buffer when it is wrapped around the ring.
Afterwards, I checked the cppreference pages for string_view function's complexity to verify the speed improvement and to write this writeup.

Remaining Bugs:
None that I know of.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list prompts/questions]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I contributed a new test case that catches a plausible bug
  not otherwise caught: [provide Pull Request URL]


Checkpoint 0 Writeup
====================

My name: Eric Wan

My SUNet ID: ehwan

I collaborated with: Calista Woo, Kenneth Liu, Logan Wang, Ethan Lai, Greg Li

I would like to credit/thank these classmates for their help: [list sunetids here]

This lab took me about [n] hours to do. I did attend the lab session.

I was surprised by or edified to learn that: [describe]

Describe ByteStream implementation, focusing on the most salient
design choices. [Describe data structures and approach taken. Describe
alternative designs considered or tested.  Describe benefits and
weaknesses of your design compared with alternatives -- perhaps in
terms of simplicity/complexity, risk of bugs, asymptotic performance,
empirical performance, required implementation time and difficulty,
and other factors. Include any measurements if applicable.]

I initially chose to implement the buffer with a string, making use of its builtin functions to implement push, peek, and pop.
Size enforcement had to be done manually. The ability to specify a certain range on append and erase was useful to handle this enforcement.
It was rather simple to implement since much of the complexity was handled by builtin functions, but the throughput suffers with many pops due to the inefficiency of erasing from the start of a string.
Peeking is very simple since we can just return the entire buffer.

We need to make sure in the case that tail is "before" head the capacity does not go over or under

pop is now O(1), while peek is O(n) and push remains O(n)

Implementation Challenges:

substr error, stack-use-after-return, I thought it might have something to do with the interaction between substr which creates a temporary copy and string_view.
I checked stack overflow: https://stackoverflow.com/questions/46032307/how-to-efficiently-get-a-string-view-for-a-substring-of-stdstring
I also referrenced cppreference.com a lot for string_view and substr.
I fooled around with casting this->buffer to either string or string_view, and having the return value be string or string_view until it worked
Cannot use append, so we must use 2 peeks sometimes to get the wrapped around part

Remaining Bugs:
[]

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list prompts/questions]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I contributed a new test case that catches a plausible bug
  not otherwise caught: [provide Pull Request URL]


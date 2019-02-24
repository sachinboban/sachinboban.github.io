---
layout: post
title: "Buffer Overflow"
---

We all have heard this term over and over again. But most of us would have
never tried (or even understood) how exaclty a buffer overflow can be used to
run a completely different (and often malicious) code. Before we jump to
performing such an attack, let us spend some time to understand some basics.

> Before we begin, this article assumes that you are familiar with stack,
> stack frame and function calls conventions on an x86-64 machine. If not I
> recommend taking a look at
> [Stack frame layout on x86-64][stk-eli] by Eli Bendersky.

# What is a buffer Overflow?
[Wikipedia][wiki] defines Buffer overflow as follows:
> ... buffer overflow, or buffer overrun, is an anomaly where a program, while
> writing data to a buffer, overruns the buffer's boundary and overwrites
> adjacent memory locations.

In other words, a buffer overflows when we (the programmer) writes more data
than what the buffer can hold. This can be easily happen in a `C` program as it
does not perform any check for bounds. In fact the very `strcpy()` function
(`char *strcpy(char *dest, const char *src)`) provided as part of `string.h`
does not check whether all the bytes in srouce would fit into the destination.
If `src` is bigger than `dest` then the extra bytes in the `src` will result in
an overflow.
> `strncpy()` should be used instead of `strcpy()` as the later is considered
> unsafe due to chances of an overflow.

# Ok. So what if a buffer overflows?
When a buffer overflows, the extra byte(s) that do not fit into the buffer,
overwrites memory locations adjacent to the buffer. Depending on where the
buffer is actually placed, the buffer overflow can spill into memory locations
containing executable code, or any other important information such as return
address (which instruction to execute upon returning from a function call).

There are different types of buffer overflow attacks. In our example we try to
modify the return address in a stack frame. By overwritting the return address
with another address, we can make the execution to resume at any other memory
location (and hence execute any code we want).

Now that we have set the background, lets get cracking!

# Memory Layout
Programs make use of different regions of memory such as stack, heap, code etc.
As you would already know by now, stack is where the variables and frames are
stored. The actual location of these memory regions depends the system. They
may vary from time the same program is run or it might be fixed.

Stack is where the return address (the address of the instruction to be executed
upon returning from a function call) is stored. Pushing the return address to
the stack is done by the caller, and is done as part of the `callq` instruction.
So very first entry in the stack frame of function, shall be this return
address. In addition, the stack is also used to store some varibles.

# Manipulating the stack
As mentioned before the stack is used for storing varibles and also
(temporarily) store address of certain instructions as well. The very fact that
these two pieces sometimes are dangerously close to each other in a stack frame
is used to orchestrate such an attack in a poorly written `C` program.

Before we begin, let us have look at the _poorly written `C` program_ that we
are going to use.

<script src="http://gist-it.appspot.com/github/sachinboban/buffer-overflow-demo/blob/master/overflow.c"></script>

`main()` blindly calls the `serial_mult()` function and prints the value
returned by `serial_mult()`. `serial_mult()` asks the user for the number of
integers to multiply. These numbers are then read from the shell using `scanf()`
and is stored into `a[]`. Though `a[]` by definition can only hold upto 5
elements, `serial_mult()` does not do perform any check on how many integers the
the user can input. The very fact that there is no such check is what makes this
program _poorly written_.

The program also contains another function `not_used()` and is no where called
in the program. In the normal flow of the program we shall never see this
function getting called.

Compile the program using the command below. Do not use the `-g`, as we only
want to look at the assembly in the debugger.
```sh
$ gcc -Og overflow.c -o overflow
```

If interested, we can also generate the disassemble the binary using objdump
utility

```sh
$ objdump -D overflow > disassembly.txt
```

First let us try to run the program with 5 numbers. It should print the
product of all of them.
```sh
$ ./overflow
Enter array size
5
Enter array the array
1
2
3
4
5
Product: 120
```
Now let us try with 6 numbers. This shouldn't work, since a can only hold upto
5 integers. But still it does work (magic??).
```sh
$ ./overflow
Enter array size
6
Enter array the array
1
2
3
4
5
1
Product: 120
```
What if we try 10 numbers?
```sh
$ ./overflow
Enter array size
10
Enter array the array
1
1
1
1
1
1
1
1
1
1
*** stack smashing detected ***: <unknown> terminated
Aborted (core dumped)
```

# An example
First we try to find the location of the return address on the stack. Then we
use the error prone array access to overwrite the return address and try to
return from `serial_mult()` to an address where should't be.

Since the stack _grows down_, `a[]` is located at a lower address than the
location of the return address. If the `a[]` overflows, we might end up writting
all the way upto the return address. Now suppose that our stack frame looks
something like this:

```
        ----    ---------------------
         |      |   return address  | 0x7bcdffffffff0038
         |      ---------------------
         |      |                   | 0x7bcdffffffff0030
         |      ---------------------
         |      |                   | 0x7bcdffffffff0028
         |      ---------------------
         |      |       a[4]        | 0x7bcdffffffff0020
   stack |      ---------------------
   frame |      |       a[3]        | 0x7bcdffffffff0018
         |      ---------------------
         |      |       a[2]        | 0x7bcdffffffff0010
         |      ---------------------
         |      |       a[1]        | 0x7bcdffffffff0008
         |      ---------------------
         |      |       a[0]        | 0x7bcdffffffff0000
        ----    ---------------------
                |                   |
```
If this is case, by writting 8 numbers, we can easily overwrite the return
address, with the last number essentially being the new return address.

# Let the attack begin!
The stack we saw is just an example explaining our approach. To find the actual
location of `a[]` and the return address in out `overflow.c` program we make
use of `gdb`.

Run the overflow debugger in gdb.
```sh
$ gdb overflow
```

At first we look at the disassembly of `main()` function to get the actual
return address.
```
(gdb) disassem main
Dump of assembler code for function main:
   0x0000000000000889 <+0>:	sub    $0x8,%rsp
   0x000000000000088d <+4>:	callq  0x7e4 <serial_mult>
   0x0000000000000892 <+9>:	mov    %rax,%rdx
   0x0000000000000895 <+12>:	lea    0xe7(%rip),%rsi        # 0x983
   0x000000000000089c <+19>:	mov    $0x1,%edi
   0x00000000000008a1 <+24>:	mov    $0x0,%eax
   0x00000000000008a6 <+29>:	callq  0x680 <__printf_chk@plt>
   0x00000000000008ab <+34>:	mov    $0x0,%eax
   0x00000000000008b0 <+39>:	add    $0x8,%rsp
   0x00000000000008b4 <+43>:	retq
End of assembler dump.
```
In the above disassembly of `main()`, `callq  0x7e4 <serial_mult>` is the
call to the `serial_mult()` function (which is located at `0x7e4`). Upon
returning from `serial_mult()` the instruction at address `0x892` shall execute
the next instruction.

Immediately upon entering any function (before any instruction of that
function is excuted), the stack pointer shall be pointing to the location
of the return address in the stack. For this we set a break point at the very
beginning of the `serial_mult()` function.
```
(gdb) break serial_mult
```
After we set the breakpoint as shown above we let the program run untill we
hit the breakpoint.
```
(gdb) run
```
Upon hitting the break point we set previously we can print the value of the
stack register `%rsp` to get the location of the return address in the stack
```
(gdb) print/x $rsp
```
> You may also look into the location pointed to by `%rsp` to get the return
> address using `print/x *<location>`, where `<location>` is the value of the
> stack pointer.

Now that we have got the location of the return address in the stack frame of
`serial_mult()`, the next step would be get the base address of `a[]`. This is
nothing but the address of `a[0]`. Recall that we get the each number from
the user using `scanf()` function. The second argument to `scanf()` is the
address of the location where the number has to be stored. In fact, the very
first call to `scanf()` within the for loop has the address of `a[0]` as second
argument. From ABI, we know the very first argument is passed to callee using
`%rdi` and the second argument is passed using `%rsi`. For this we can
disassemble `serial_mult` and put a break point at the very first call to
`scanf()`.
```
(gdb) disassem serial_mult
```
and
```
break *<address_of_call_to_scanf>
```
Once we reach this breakpoint we can print the value of `%rsi` to get base
address of `a[]`. Using this address of `a[]` and the location of the return
address, we can figure out how many numbers we need to input inorder to
overwrite the return address.

Suppose its the 8th number overwrites the return address. Lets try to
give `0` as the 8th number. The program would try to fertch instruction at
address `0x0` and shall crash.

Now what if instead of inputing `0`, we give a valid address as the 8th number?
For example the address of the function `not_used()`?
> You can again use `disassem not_used` within gdb to get the address of our
> `not_used` function. Remember to convert the address to a decimal value
> before feeding it as the 8th number to the program.

Upon successfully overwritting the return address with the address of `not_used`
function, you can see the print within the `not_used` function even though the
function was never called anywhere in our program.
```sh
I am never here!
```
> Since  `not_used()` does exits using the `exit()` function instead of
> returning back to its caller, we would never see the part of `main()` after
> the call to `serial_mult()` getting exectuted.

# Summarizing
There are some limitations to the attack we did: we can only execute an existing
function. But by being even more clever, we can feed even more numbers,
overwriting beyond the return address, into `main()` function's stack frame.
Note that we can write any number we wish to. This essentially changes the
behaviour of `main()` and/or other functions. What if we encode actual machine
instructions into data using `scanf()`, set the return address to those
_injected_ instructions? If so, we are essentially executing the _injected code_
within the program.

# References
1. [Wikipedia page on bufffer overflow][wiki]
2. [Stack frame layout on x86-64][stk-eli]

[wiki]: https://en.wikipedia.org/wiki/Buffer_overflow
[stk-eli]: https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64/
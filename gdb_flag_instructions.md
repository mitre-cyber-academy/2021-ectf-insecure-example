# Instructions for Getting the Debugger Flag

This tutorial will show you how to use the GDB debugger to walk through and
interact with a running SCEWL Bus Controller.

## 1. Launch Controller in Debug Mode
Instead of the typical `launch_sed` command, we will instead use `launch_sed_gdb`,
which attaches a GDB debugger to the SCEWL Bus Controller.

For this tutorial, you may simply launch the following script from the root
directory of this repository (it will pull from pre-built containers we have
pushed to Docker Hub):
```
./tools/deploy_gdb.sh
```

## 2. Getting Your Bearings
When the deployment finishes spinning up, you should see output along the lines of:
```
0x000003cc in Reset_Handler ()
(gdb) 
```

You are now using GDB. QEMU has started emulating the SCEWL Bus Controller, but
stopped it at the first instruction, which is the reset handler at address 0x438.

Let's view the current register values with `info`:
```
(gdb) info registers
r0             0x0                 0
r1             0x0                 0
r2             0x0                 0
r3             0x0                 0
r4             0x0                 0
r5             0x0                 0
r6             0x0                 0
r7             0x0                 0
r8             0x0                 0
r9             0x0                 0
r10            0x0                 0
r11            0x0                 0
r12            0x0                 0
sp             0x20000100          0x20000100
lr             0xffffffff          -1
pc             0x3cc               0x3cc <Reset_Handler>
cpsr           0x400001f3          1073742323
```

We can also view values in memory with `x` specified by address or symbol:
```
(gdb) x 0x3cc
0x3cc <Reset_Handler>:	0xb084b580
(gdb) x Reset_Handler
0x3cc <Reset_Handler>:	0xb084b580
```

**Write down the raw value of the instruction in memory at intf_init for later
as value1**

## 3. Setting a Breakpoint
Now that we have control of the system, let's continue to main. To do that,
we must first set a breakpoint at main using `break` or `b` for shorthand:
```
(gdb) b main
Breakpoint 1 at 0x568
```

Now continue running the process with `continue` or `c` for shorthand:
```
(gdb) c
Continuing.

Breakpoint 1, 0x00000568 in main ()
=> 0x00000568 <main+8>:	ff f7 c2 fd	bl	0xf0 <intf_init>
```

Note, GDB did not actually break at main, but rather at an adddress 8 bytes
into main (seen by `<main+8>`. This can be confirmed by printing the address
of main with `print` or `p` for shorthand:
```
(gdb) p main
$1 = {<text variable, no debug info>} 0x560 <main>
```

Why is that? It turns out GDB is trying to be smart under the covers. It skips
over the function prologue that mainly handles cleaning up the stack before the
main body of the function. If you did not want that and instead want to break at
the exact start of a function, we can either manually break at an address by
calling `break` with a number preceded by `*`:
```
(gdb) b *0xdeadbeef
Breakpoint X at 0xdeadbeef
```

Or by dereferencing the address and using the variable to break:
```
(gdb) print func_at_deadbeef
$2 = {<text variable, no debug info>} 0xdeadbeef <func_at_deadbeef>
(gdb) b *$2
Breakpoint X at 0xdeadbeef
```

Let's try that with `intf_init`:
```
(gdb) p intf_init
$2 = {<text variable, no debug info>} 0xf0 <intf_init>
(gdb) b *$2
Breakpoint 2 at 0xf0
(gdb) c
Continuing.

Breakpoint 2, 0x000000f0 in intf_init ()
=> 0x000000f0 <intf_init+0>:	80 b4	push	{r7}
```

**Inspect the registers and write down the value in `sp` as value2**


## 4. Stepping Through Code
Let's skip ahead to a function called `do_dumb_math` (without skipping the
function prologue):
```
(gdb) p do_dumb_math
$3 = {<text variable, no debug info>} 0x1fe <do_dumb_math>
(gdb) b *$3
Breakpoint 1 at 0x1fe
(gdb) c
Continuing.

Breakpoint 1, 0x000001fe in do_dumb_math ()
=> 0x000001fe <do_dumb_math+0>:	80 b4	push	{r7}
```

Now, let's inspect the disassembly of this function with `disas`:
```
(gdb) disas do_dumb_math
Dump of assembler code for function do_dumb_math:
=> 0x000001fe <+0>:	push	{r7}
   0x00000200 <+2>:	sub	sp, #20
   0x00000202 <+4>:	add	r7, sp, #0
   0x00000204 <+6>:	str	r0, [r7, #12]
   0x00000206 <+8>:	str	r1, [r7, #8]
   0x00000208 <+10>:	str	r2, [r7, #4]
   0x0000020a <+12>:	str	r3, [r7, #0]
   0x0000020c <+14>:	ldr	r2, [r7, #12]
   0x0000020e <+16>:	ldr	r3, [r7, #8]
   0x00000210 <+18>:	add	r3, r2
   0x00000212 <+20>:	ldr	r1, [r7, #8]
   0x00000214 <+22>:	ldr	r2, [r7, #4]
   0x00000216 <+24>:	sdiv	r2, r1, r2
   0x0000021a <+28>:	mul.w	r2, r2, r3
   0x0000021e <+32>:	ldr	r3, [r7, #0]
   0x00000220 <+34>:	ldr	r1, [r7, #12]
   0x00000222 <+36>:	sdiv	r1, r3, r1
   0x00000226 <+40>:	ldr	r0, [r7, #12]
   0x00000228 <+42>:	mul.w	r1, r0, r1
   0x0000022c <+46>:	subs	r3, r3, r1
   0x0000022e <+48>:	mul.w	r3, r3, r2
   0x00000232 <+52>:	ldr	r1, [r7, #12]
   0x00000234 <+54>:	ldr	r2, [r7, #8]
   0x00000236 <+56>:	eors	r1, r2
   0x00000238 <+58>:	ldr	r2, [r7, #0]
   0x0000023a <+60>:	add	r2, r1
   0x0000023c <+62>:	sdiv	r1, r3, r2
   0x00000240 <+66>:	mul.w	r2, r2, r1
   0x00000244 <+70>:	subs	r3, r3, r2
   0x00000246 <+72>:	mov	r0, r3
   0x00000248 <+74>:	adds	r7, #20
   0x0000024a <+76>:	mov	sp, r7
   0x0000024c <+78>:	pop	{r7}
   0x0000024e <+80>:	bx	lr
End of assembler dump.
```

Instead of using breakpoints, we can instead step through this function
instruction by instruction using `si` for step instruction:
```
(gdb) si
0x00000200 in do_dumb_math ()
=> 0x00000200 <do_dumb_math+2>:	85 b0	sub	sp, #20
(gdb) si
0x00000202 in do_dumb_math ()
=> 0x00000202 <do_dumb_math+4>:	00 af	add	r7, sp, #0
(gdb) si
0x00000204 in do_dumb_math ()
=> 0x00000204 <do_dumb_math+6>:	f8 60	str	r0, [r7, #12]
```

You can see that we are stepping through the instructions of the function.
If you run `disas` again, you will see our position has changed:
```
(gdb) disas do_dumb_math
Dump of assembler code for function do_dumb_math:
   0x000001fe <+0>:	push	{r7}
   0x00000200 <+2>:	sub	sp, #20
   0x00000202 <+4>:	add	r7, sp, #0
=> 0x00000204 <+6>:	str	r0, [r7, #12]
   0x00000206 <+8>:	str	r1, [r7, #8]
   0x00000208 <+10>:	str	r2, [r7, #4]
   0x0000020a <+12>:	str	r3, [r7, #0]
   0x0000020c <+14>:	ldr	r2, [r7, #12]
   0x0000020e <+16>:	ldr	r3, [r7, #8]
   0x00000210 <+18>:	add	r3, r2
   0x00000212 <+20>:	ldr	r1, [r7, #8]
   0x00000214 <+22>:	ldr	r2, [r7, #4]
   0x00000216 <+24>:	sdiv	r2, r1, r2
   0x0000021a <+28>:	mul.w	r2, r2, r3
   0x0000021e <+32>:	ldr	r3, [r7, #0]
   0x00000220 <+34>:	ldr	r1, [r7, #12]
   0x00000222 <+36>:	sdiv	r1, r3, r1
   0x00000226 <+40>:	ldr	r0, [r7, #12]
   0x00000228 <+42>:	mul.w	r1, r0, r1
   0x0000022c <+46>:	subs	r3, r3, r1
   0x0000022e <+48>:	mul.w	r3, r3, r2
   0x00000232 <+52>:	ldr	r1, [r7, #12]
   0x00000234 <+54>:	ldr	r2, [r7, #8]
   0x00000236 <+56>:	eors	r1, r2
   0x00000238 <+58>:	ldr	r2, [r7, #0]
   0x0000023a <+60>:	add	r2, r1
   0x0000023c <+62>:	sdiv	r1, r3, r2
   0x00000240 <+66>:	mul.w	r2, r2, r1
   0x00000244 <+70>:	subs	r3, r3, r2
   0x00000246 <+72>:	mov	r0, r3
   0x00000248 <+74>:	adds	r7, #20
   0x0000024a <+76>:	mov	sp, r7
   0x0000024c <+78>:	pop	{r7}
   0x0000024e <+80>:	bx	lr
End of assembler dump.
```

**Step through the instructions until the value of r3 starts with 0xe and
record that value as value3**

## 5. Writing to Registers and Memory
Set a breakpoint at 0x24c (the end of `do_dumb_math` and continue there:

```
(gdb) b *0x24c
Breakpoint 2 at 0x24c
(gdb) c
Continuing.

Breakpoint 2, 0x0000024c in do_dumb_math ()
=> 0x0000024c <do_dumb_math+78>:	80 bc	pop	{r7}
```

With the `set` command we can now modify registers (make sure to reset them):
```
(gdb) info registers r0
r0             0x0                 0
(gdb) set $r0=111
(gdb) info registers r0
r0             0x6f                111
(gdb) set $r0=0
```

And memory:
```
(gdb) x 0x20000000
0x20000000 <pulStack>:	0x00000000
(gdb) set *0x20000000=0x111
(gdb) x 0x20000000
0x20000000 <pulStack>:	0x00000111
(gdb) set *0x20000000=0
```

## 6. Capturing the flag
With what you've learned, set a breakpoint at the first instruction of the
`check_flag` function and continue up to there.

`check_flag` has five arguments; let's check them out. The ARM calling convention
is to place the first four arguments in registers and further arguments are
pushed to the stack.

Print the registers and then the top value on the stack to view the arguments:
```
(gdb) info registers
r0             0x11111111          286331153
r1             0x22222222          572662306
r2             0x33333333          858993459
r3             0x44444444          1145324612
r4             0x0                 0
r5             0x0                 0
r6             0x0                 0
r7             0x200090f8          536908024
r8             0x0                 0
r9             0x0                 0
r10            0x0                 0
r11            0x0                 0
r12            0x0                 0
sp             0x200090f0          0x200090f0
lr             0x5a3               1443
pc             0x420               0x420 <check_flag>
cpsr           0x1f3               499
(gdb) x $sp
0x200090f0:	0x55555555
```

We can see that arguments 1-4 (0x11111111, 0x22222222, 0x33333333, and
0x44444444) are in registers r0 through r3, and the top value of the stack
hold the fifth argument (0x55555555).
Now, using what you have learned, change the values of the function so that the
first argument is set to `value1`, the third argument is set to `value2`, and
the fifth argument is set to `value3`.

Next, set a breakpoint at 0x5a2, which is just after the return of the flag-
dispensing function, and continue to it. Type `q` to quit GDB; **don't hit
<ctrl+c> or else it will also kill the FAA receiver**:
```
(gdb) q
(gdb) q
A debugging session is active.

	Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/ubuntu/2021-ectf/controller.elf.deleteme, process 1
Ending remote debugging.
```

Press `<enter>` to show the buffered messages in the FAA receiver. If you did
everything correctly, you should see a flag, and if not, you should see an
explanation of which argument was incorrect.


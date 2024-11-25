# Syscall Fun

This repository contains the code, test scripts, and results from my work on bundling system calls. This work was carried out as a part of the Boise State University class CS 554 - Advanced Operating Systems.

It contains two implementations of a program to send the contents of a file over the network. One is the typical Unix implementation. The other is a new system call implmented as a kernel module that does the same.

## Environment

This code was written on and for FreeBSD 14.1 on AMD64. It is very very unlikely to run elsewhere.

Both of the programs have hardcoded behaviors. Both look for a file (relative to their sources) ../text/moby-dick.txt. This is the full text of Moby Dick from Project Gutenburg. Both also send the file out in 512 byte UDP packets to 127.0.0.1:1234.

## Directories

Classic - A "classic" implementation of sending a file over the network. Uses open(2), read(2), sendto(2). The DTrace script records the time spent in each read->sendto section. The Perl script runs the program via DTrace and calculates the average time elapsed. 

Module - A kernel module that implements a syscall. This new syscall takes as its argument a file descriptor returned by open(2). It then sends the contents of that file over the network, effectively reimplementing the "classic" program within a single syscall.

Runner - A small wrapper program that calls the syscall added by the module using syscall(2). It also contains the same DTrace and Perl scripts as the classic directory. The DTrace script uses low overhead syscalls (getpid(2) and getpgrp(2)) to start and stop the clock.

## Running

The classic implemetation can be run via DTrace using the Perl script.

The module can be loaded with kldload(8). The runner program can then be run via DTrace using the Perl script.

### Pitfalls

There are lots of ways this can go wrong. Here are the ones I've encountered so far.

 - The runner program assumes that the new syscall will be number 210. This is hardcoded.
 
 - Running the classic program after loading the module can cause a kernel panic.

 - Running the runner program more than once can cause a kernel panic.

 - Unloading the module can cause a kernel panic.
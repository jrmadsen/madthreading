#!/usr/bin/env python

import os
import sys
import time
import async
import pythreading

waitForFutures = False
pyfib = 27

pythreading.thread_manager(4)

def fibonacci(n):
    if n < 2:
        return n 
    return fibonacci(n-1) + fibonacci(n-2)

c_start = time.clock()
w_start = time.time()

async.write("(Python) Starting loop calling C++ function scheduling "
            "asynchronous calculations...")

futures = []
for i in range(0, 100):
    async.write("(Python) Running async iteration {}...".format(i))
    if i < 50:
        futures.append(async.run())
    else:
        futures.append(async.work(i))

async.write("(Python) Finished any calls to C++ code... Any future C++ output "
            "demonstrates C++ running in the background alongside Python...")

if waitForFutures == True:
    for n in range(0, len(futures)):
        async.write("(Python) Waiting on future {}...".format(n))
        futures[n].wait()

sum = 0
for n in range(0, len(futures)):
    # do work in Python to try to exceed the ~400% utilization of C++
    sum += fibonacci(pyfib)
    async.write("(Python) Getting future for {}...".format(n))
    sum += futures[n].get()

c_end = time.clock()
w_end = time.time()

c_diff = c_end - c_start 
w_diff = w_end - w_start
cpu_usage = (c_diff / w_diff) * 100.

async.write("")
async.write("(Python) Sum = {}".format(sum))
async.write("")
async.write("CPU time : {} seconds".format(c_diff))
async.write("Wall time : {} seconds".format(w_diff))
async.write("")
async.write("% CPU : {:6.2f}%".format(cpu_usage))

ans = (sum == 10253057300)
if ans == True:
    sys.exit(0)
else:
    sys.exit(1)

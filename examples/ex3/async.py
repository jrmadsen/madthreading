#!/usr/bin/env python3

import os
import sys
import time
import async

c_start = time.clock()
w_start = time.time()

async.write("(Python) Starting loop calling C++ function scheduling asynchronous calculations...")

futures = []
for i in range(0, 100):
    async.write("(Python) Running async iteration %i..." % i)
    futures.append(async.run())

async.write("(Python) Finished any calls to C++ code... Any future C++ output demonstrates C++ running in the background alongside Python...")

sum = 0
for n in range(0, len(futures)):
    async.write("(Python) Getting future for %i..." % n)
    sum += futures[n].get()

c_end = time.clock()
w_end = time.time()

c_diff = c_end - c_start 
w_diff = w_end - w_start
cpu_usage = (c_diff / w_diff) * 100.

async.write("")
async.write("(Python) Sum = %i" % sum)
async.write("")
async.write("CPU time : %f seconds" % c_diff)
async.write("Wall time : %f seconds" % w_diff)
async.write("")
async.write("%s CPU : %6.2f%s" % ('%', cpu_usage, '%'))

ans = (sum == 2269806570)
if ans == True:
    sys.exit(0)
else:
    sys.exit(1)

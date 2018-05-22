#!/usr/bin/env python

import os
import sys
import time
import numpy as np
import traceback
import timemory
import madthreading.async
import madthreading.threading

print("async file: {}".format(madthreading.async.__file__))

#timanager = timemory.manager()
thmanager = madthreading.threading.thread_manager(4)
timemory.enable_signal_detection()

waitForFutures = False
pyfib = 27
nitr = 50
futures = []
sum = 0


#------------------------------------------------------------------------------#
def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n-1) + fibonacci(n-2)


#------------------------------------------------------------------------------#
#@timemory.util.auto_timer()
def run(_sum, _pyfib, _n, _futures):
    global futures
    global sum
    print("(Python) Calculting fibonacci({})...".format(_pyfib))
    _sum += fibonacci(pyfib)
    print("(Python) Getting future for {}...".format(_n))
    _sum += _futures[_n].get()


#------------------------------------------------------------------------------#
#@timemory.util.auto_timer()
def main():
    global futures
    global sum

    #timer = timemory.timer("Total execution time")
    #timer.start()

    print("{} {}".format("(Python) Starting loop calling C++ function ",
                               "scheduling asynchronous calculations..."))

    for i in range(0, nitr):
        print("(Python) Running async iteration {}...".format(i))
        if i < nitr/2:
            f = madthreading.async.run()
            futures.append(f)
        else:
            f = madthreading.async.work(i)
            print('Adding async.work({})'.format(i))
            futures.append(f)

    print("{} {} {}".format("(Python) Finished any calls to C++ code...",
                                  "Any future C++ output demonstrates C++ running",
                                  "in the background alongside Python..."))

    if waitForFutures == True:
        for n in range(0, len(futures)):
            print("(Python) Waiting on future {}...".format(n))
            futures[n].wait()


    for n in range(0, len(futures)):
        # do work in Python to try to exceed the ~400% utilization of C++
        run(sum, pyfib, n, futures)

    #timer.stop()

    print("")
    print("(Python) Sum = {}".format(sum))
    print("")
    #print("{}".format(timer))

    madthreading.async.report()

    return (sum == 10253057300)


#------------------------------------------------------------------------------#
if __name__ == "__main__":
    ans = False
    try:
        ans = main()
        print('{}'.format(timanager))
    except Exception as e:
        exc_type, exc_value, exc_traceback = sys.exc_info()
        traceback.print_exception(exc_type, exc_value, exc_traceback, limit=100)
        print ('Exception - {}'.format(e))
        ans = False

    if ans == True:
        sys.exit(0)
    else:
        sys.exit(1)


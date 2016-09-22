from subprocess import run
from time import time
from matplotlib import pyplot

testFile = r"SubmissionStuff/KingJamesBible"
byteSize = [2 ** x for x in range(8,32)]
incrementalSize = [10 * x for x in range(25,25+len(byteSize))]
testTime = []
testTime_incremental = []
sampleSize = 30
for size, size_2 in zip(byteSize, incrementalSize):
    average = []
    for i in range(0,sampleSize):
        timeStart = time()
        run(['./copycat', '-b', str(size), '-o', 'testfile', testFile]) 
        timeEnd = time()
        average.append(timeEnd-timeStart)
    testTime.append(sum(average)/sampleSize)

    average = []
    for i in range(0,sampleSize):
        timeStart - time()
        run(['./copycat', '-b', str(size_2), '-o', 'testfile', testFile]) 
        timeEnd = time()
        average.append(timeEnd-timeStart)
    testTime_incremental.append(sum(average)/sampleSize)

pyplot.subplot(211)
pyplot.plot(byteSize, testTime) 
pyplot.ylabel("Time (s)")
pyplot.xlabel("Size of buffer (bytes) Log Scale")
pyplot.title("Size of Buffer (Power of 2) vs Time")
pyplot.subplot(212)
pyplot.plot(incrementalSize, testTime_incremental)
pyplot.ylabel("Time (s)")
pyplot.xlabel("Size of buffer (bytes) Linear Scale")
pyplot.title("Size of Buffer (Increments of 10 bytes) vs Time")
pyplot.show()

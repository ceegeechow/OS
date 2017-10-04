import os
#import subprocess

sizes = ['1', '2', '4', '8', '16', '32', '64', '128', '256', '512', '1024', '2048', '4096', '8192', '16384', '32768', '65536', '131072', '262144']

os.system("gcc minicat.c")

for buff_size in sizes:
    print("\nbuffer size: " + buff_size)
    os.system("time ./a.out -b %s -o output T1.txt input1.txt input2.txt" %(buff_size))
    #p = subprocess.Popen("time ./a.out -b %s -o output T1.txt input1.txt input2.txt" %(buff_size), stderr = times.txt)
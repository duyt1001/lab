import os

for filename in os.listdir('.'):
    info = os.stat(filename)
    print filename, info.st_mtime, info.st_ctime


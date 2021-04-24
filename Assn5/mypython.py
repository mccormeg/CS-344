# Name: Megan McCormick
# Program name: mypython.py
# Decsription: Makes 3 files with 10 random letters and prints them to the screen
# Takes two random integers from 1-42 and adds them together

import string
import random
from random import randrange

#Creation of file one
f1 = open("File1.txt","w")
string.letters = 'abcdefghijklmnopqrstuvwxyz'
word = ''.join(random.choice(string.letters) for _ in range(10))
print("File1: %s" % (word))
f1.write("%s\n" % (word))
f1.close()

#Creation of file two
f2 = open("File2.txt","w")
string.letters = 'abcdefghijklmnopqrstuvwxyz'
word = ''.join(random.choice(string.letters) for _ in range(10))
print("File2: %s" % (word))
f2.write("%s\n" % (word))
f2.close()

#Creationof file three
f3 = open("File3.txt","w")
string.letters = 'abcdefghijklmnopqrstuvwxyz'
word = ''.join(random.choice(string.letters) for _ in range(10))
print("File3: %s" % (word))
f3.write("%s\n" % (word))
f3.close()

#Integer printing
i1 = randrange(1,43)
print("Integer 1: %s" % (i1))
i2 = randrange(1,43)
print("Integer 2: %s" % (i2))
print("Addition: %s" % (i1+i2))
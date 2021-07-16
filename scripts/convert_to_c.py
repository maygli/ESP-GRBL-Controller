#!/usr/bin/python

import sys

def print_usage():
  print("Convert html input file to C string.\nUsage python convert_to_c.py <input html file name> <c file name>")

def convert(theInFileName, theOutFileName):
  anInFile = open(theInFileName, "r")
  aLines = anInFile.readlines()
  anInFile.close()
  anOutFile = open(theOutFileName, "w")
  anOutFile.write('"')
  for aLine in aLines:
    aLine = aLine.replace("\n","")
    aLine = aLine.replace('"','\\"')
    aLine = aLine + "\\n\\\n"	
    anOutFile.write(aLine)
  anOutFile.write('"')
     

if __name__ == '__main__':
  if len(sys.argv) < 3:
    print_usage()
    sys.exit(1)
  anInputFileName = sys.argv[1]
  anOutputFileName = sys.argv[2]
  convert(anInputFileName, anOutputFileName)  
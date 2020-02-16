#!/usr/bin/python3.7
import sys
#Jesus Alejandro Gonzalez Sanchez
#A00820225
def main():
	print("Lab 01 Compilers Lecture")
	print("Code Analysis:")
	scriptPath =  sys.argv[1]
	listOfFunctions = []
	dictCommands = {}
	with open(scriptPath) as file:
		for line in file:
			if line[0]=='0':
				words =  line.split(" ")
				listOfFunctions.append(words)
			else:
				words = line.split("\t")
				if len(words)>2:
					commands = words[2].split(" ")
					if len(commands) >0: 
						if commands[0] in dictCommands:
							dictCommands[commands[0]] = dictCommands[commands[0]]+1
						else:
							dictCommands[commands[0]] = 1

	print("You have {0} kind of instructions in this obj. file:".format(len(dictCommands)))
	for comm in dictCommands:
		print("  {0}\t: Executed {1} times ".format(comm, dictCommands[comm]))
	print("You have {0} functions:".format(len(listOfFunctions)))
	for x in  listOfFunctions:
		print("  {0}".format( x[1][1:-3] ), end='' )
		space =(24-len(x[1][1:-3])) * ' '
		print(space, end='')
		print(": Located at {0} addr".format(x[0][-4:] ))

if __name__ == '__main__':
	main()

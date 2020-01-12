import sys
import os

cmp = "./build/cmp"
path = sys.argv[1]

print()
print("#########################################")
print()
print()

failures = 0
tests = 0

for file in os.listdir(path):
	if file.endswith(".imp"):
		print("-----------------------------------------")
		print(file)
		tests += 1
		print("---")
		if os.system(cmp + " " + path + "/" + file + " /dev/null") != 0:
			failures += 1

print("####### ", failures, " failures out of ", tests, " tests")

import sys
import os

cmp = "./build/cmp"
path = sys.argv[1]

print()
print("#########################################")
print()
print()

for file in os.listdir(path):
	if file.endswith(".imp"):
		print("-----------------------------------------")
		print(file)
		print("---")
		os.system(cmp + " < " + path + "/" + file)

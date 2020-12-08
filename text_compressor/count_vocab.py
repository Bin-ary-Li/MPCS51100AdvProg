import sys

assert(len(sys.argv) == 2), "Please pass in the filename to count its unique words."
originalfile = sys.argv[1]
with open(originalfile, 'r') as file:
    data = file.read().replace('\n', ' ') 
og = data.split(" ")
og = [word.lower() for word in og if word != ""]
og_set = set(og)

print(f"{originalfile} has {len(og_set)} unique words.")
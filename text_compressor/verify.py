import sys

assert(len(sys.argv) == 2), "Please pass in the filename of the original file to verify the compression."
originalfile = sys.argv[1]

with open(originalfile, 'r') as file:
    data = file.read().replace('\n', ' ') 
og = data.split(" ")
og = [word.lower() for word in og if word != ""]
with open('coded_files/slow_decoded.txt', 'r') as file:
    data = file.read().replace('\n', ' ')    
de_slow = data.split(" ")
de_slow = [word for word in de_slow if word != ""]
with open('coded_files/tree_decoded.txt', 'r') as file:
    data = file.read().replace('\n', ' ')    
de_tree = data.split(" ")
de_tree = [word for word in de_tree if word != ""]
with open('coded_files/hash_decoded_naive.txt', 'r') as file:
    data = file.read().replace('\n', ' ')    
de_hash_n = data.split(" ")
de_hash_n = [word for word in de_hash_n if word != ""]
with open('coded_files/hash_decoded_bernstein.txt', 'r') as file:
    data = file.read().replace('\n', ' ')    
de_hash_b = data.split(" ")
de_hash_b = [word for word in de_hash_b if word != ""]
with open('coded_files/hash_decoded_FNV.txt', 'r') as file:
    data = file.read().replace('\n', ' ')    
de_hash_fnv = data.split(" ")
de_hash_fnv = [word for word in de_hash_fnv if word != ""]



assert len(og)==len(de_slow)==len(de_tree)==len(de_hash_n)==len(de_hash_b)==len(de_hash_fnv), "# of words after decoded doesn't match the # of word in the original file."

for i in range(len(og)):
    assert (og[i] == de_slow[i]), f"At index {i}, og={og[i]} but decoded = {de_slow[i]} in slow_decoded.txt"
    assert (og[i] == de_tree[i]), f"At index {i}, og={og[i]} but decoded = {de_tree[i]} in tree_decoded.txt"
    assert (og[i] == de_hash_n[i]), f"At index {i}, og={og[i]} but decoded = {de_hash_n[i]} in hash_decoded_naive.txt"
    assert (og[i] == de_hash_b[i]), f"At index {i}, og={og[i]} but decoded = {de_hash_b[i]} in hash_decoded_bernstein.txt"
    assert (og[i] == de_hash_fnv[i]), f"At index {i}, og={og[i]} but decoded = {de_hash_fnv[i]} in hash_decoded_FNV.txt"
    
    

print("Success! All output decoded files in the coded_files directory match the original.")


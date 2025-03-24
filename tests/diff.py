import difflib

def compare_files(file1, file2):
    with open(file1, 'r', encoding='utf-8') as f1, open(file2, 'r', encoding='utf-8') as f2:
        lines1 = f1.readlines()
        lines2 = f2.readlines()
    
    diff = difflib.ndiff(lines1, lines2)
    
    print("Differences:")
    for line in diff:
        if line.startswith("+"):
            print(f"Added: {line[2:].strip()}")
        elif line.startswith("-"):
            print(f"Removed: {line[2:].strip()}")
        elif line.startswith("?"):
            print(f"Change indicator: {line.strip()}")

# Example usage
file1 = "lista-de-atomi.txt"
file2 = "tokenized_code.txt"
compare_files(file1, file2)
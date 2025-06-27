input_file = "intercept_results.txt"
output_file = "intercept_unique.txt"

seen = set()
with open(input_file, 'r') as fin, open(output_file, 'w') as fout:
    for line in fin:
        if not line.startswith("Intercepted execution: "):
            continue
        func = line.strip().split("Intercepted execution: ")[1]
        if func not in seen:
            seen.add(func)
            fout.write(line)
def extract_function_names(file_path):
    """Extract function names from lines like 'Intercepted execution: cuFunctionName'."""
    function_names = set()
    with open(file_path, 'r') as f:
        for line in f:
            if ':' in line:
                parts = line.strip().split(':', 1)
                if len(parts) > 1:
                    func_name = parts[1].strip()
                    if func_name:
                        function_names.add(func_name)
    return function_names

def extract_matching_prototypes(intercepted_names, prototype_file_path):
    """Return prototypes that match intercepted function names."""
    matching_prototypes = []
    with open(prototype_file_path, 'r') as f:
        for line in f:
            for name in intercepted_names:
                if f' {name}(' in line or line.strip().startswith(name + '('):
                    matching_prototypes.append(line.strip())
                    break
    return matching_prototypes

def main():
    intercepted_file = '/home/ubuntu/fan_thesis/fan_master_thesis/tests/test_basic_pytorch/intercept_unique.txt'  # Replace with your file path
    prototypes_file = './filtered_prototypes.txt'    # Replace with your file path

    intercepted_names = extract_function_names(intercepted_file)
    matching_prototypes = extract_matching_prototypes(intercepted_names, prototypes_file)

    print("Matched Prototypes:")
    for prototype in matching_prototypes:
        print(prototype)

if __name__ == "__main__":
    main()

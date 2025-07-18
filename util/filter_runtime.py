import re

def extract_cuda_host_functions(file_path, output_path):
    pattern = re.compile(r'^extern\s+__host__.*?;\s*$')
    seen = set()
    results = []

    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.strip()
            if pattern.match(line) and line not in seen:
                seen.add(line)
                results.append(line)

    with open(output_path, 'w', encoding='utf-8') as out:
        for line in results:
            out.write(line + '\n')

if __name__ == "__main__":
    input_path = "/usr/local/cuda-12.6/include/cuda_runtime_api.h"                 # 修改为实际头文件路径
    output_path = "filtered_declarations_runtime.txt"       # 输出路径
    extract_cuda_host_functions(input_path, output_path)

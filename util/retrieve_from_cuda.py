import re

def filter_prototypes(prototype_file, name_file, output_file):
    # 读取函数名（保持顺序）
    with open(name_file, 'r') as f:
        needed = [line.strip() for line in f if line.strip()]

    # 读取所有原型
    with open(prototype_file, 'r') as f:
        prototype_lines = f.readlines()

    # 构建函数名 → 原型的映射
    func_map = {}
    for line in prototype_lines:
        match = re.search(r'\b(cu\w+)\b', line)
        if match:
            func_name = match.group(1)
            func_map[func_name] = line.strip()

    # 处理输出
    filtered = []
    for name in needed:
        if name in func_map:
            filtered.append(func_map[name])
        else:
            print(f"[WARNING] Function '{name}' not found in prototypes.")

    # 写入结果
    with open(output_file, 'w') as f:
        f.write("\n".join(filtered) + "\n")

def deduplicate_needed_file(path):
    seen = set()
    unique_lines = []
    with open(path, 'r') as f:
        for line in f:
            name = line.strip()
            if name and name not in seen:
                unique_lines.append(name)
                seen.add(name)

    with open(path, 'w') as f:
        for name in unique_lines:
            f.write(name + '\n')

# 用法
deduplicate_needed_file('util.txt')

# 用法
filter_prototypes(
    prototype_file='cuda_prototype.txt',
    name_file='util.txt',
    output_file='filtered_prototypes.txt'
)

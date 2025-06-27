import re

def parse_prototype_line(line):
    # 匹配 CUresult CUDAAPI cuFunctionName(...)；
    match = re.match(r'CUresult\s+CUDAAPI\s+(cu\w+)\s*\((.*)\);', line.strip())
    if not match:
        return None

    func_name = match.group(1)
    param_block = match.group(2)

    # 拆分参数，处理逗号+括号匹配
    param_block = param_block.strip()
    if not param_block or param_block == 'void':
        param_list = []
    else:
        param_list = [p.strip() for p in param_block.split(',')]

    # 从每个参数提取变量名作为 __VA_ARGS__
    arg_names = []
    for p in param_list:
        tokens = p.split()
        if '*' in tokens[-1]:
            # 例如：const CUdeviceptr* dptr
            arg_names.append(tokens[-1].split('*')[-1])
        else:
            arg_names.append(tokens[-1])

    return func_name, param_block, arg_names

def generate_hook_code(prototype_file, output_file):
    with open(prototype_file, 'r') as f:
        lines = f.readlines()

    hook_lines = []
    for line in lines:
        parsed = parse_prototype_line(line)
        if not parsed:
            print(f"[SKIP] Unrecognized line: {line.strip()}")
            continue

        func_name, param_block, arg_names = parsed
        hook_line = f"CU_HOOK_DRIVER_FUNC({func_name}_intercepted, {func_name}, ({param_block}), {', '.join(arg_names)})"
        hook_lines.append(hook_line)

    with open(output_file, 'w') as f:
        f.write("\n\n".join(hook_lines) + "\n")

def generate_dispatch_code(prototype_file, output_file):
    with open(prototype_file, 'r') as f:
        lines = f.readlines()

    hook_lines = []
    for line in lines:
        parsed = parse_prototype_line(line)
        if not parsed:
            print(f"[SKIP] Unrecognized line: {line.strip()}")
            continue

        func_name, param_block, arg_names = parsed
        hook_line = f"TRY_INTERCEPT(\"{func_name}\", {func_name}_intercepted)"
        hook_lines.append(hook_line)

    with open(output_file, 'w') as f:
        f.write("\n\n".join(hook_lines) + "\n")


# 用法
generate_hook_code(
    prototype_file='filtered_prototypes.txt',
    output_file='generated_hooks.cpp'
)

generate_dispatch_code(
    prototype_file='filtered_prototypes.txt',
    output_file='generated_dispatch.cpp'
)
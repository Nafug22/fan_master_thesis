import re

# 输入输出文件路径
input_file = "filtered_declarations_runtime.txt"
output_file = "intercepts_runtime_output.txt"

# 正则提取函数名和参数列表
pattern = re.compile(
    r'^extern\s+(?:__host__\s+)?(?:__cudart_builtin__\s+)?'
    r'[\w:<>]+\s+CUDARTAPI\s+(\w+)\s*\(([^)]*)\);'
)

def extract_param_names(param_list):
    if not param_list.strip() or param_list.strip() == "void":
        return ""
    # 按逗号分割参数
    params = param_list.split(',')
    param_names = []
    for p in params:
        p = p.strip()
        if p.endswith('[]'):
            p = p[:-2].strip()
        tokens = p.split()
        if not tokens:
            continue
        # 取最后一个标识符作为变量名（处理 * 和 &）
        var = tokens[-1]
        # 处理形如 `*devPtr`, `**ptr`, `&x`
        while var.startswith('*') or var.startswith('&'):
            var = var[1:]
        param_names.append(var)
    return ', '.join(param_names)

# 执行转换
with open(input_file, 'r', encoding='utf-8') as infile, \
     open(output_file, 'w', encoding='utf-8') as outfile:

    for line in infile:
        line = line.strip()
        match = pattern.match(line)
        if match:
            func_name = match.group(1)
            param_list = match.group(2).strip()
            arg_names = extract_param_names(param_list)
            outfile.write(f"INTERCEPT_RT({func_name}, ({param_list}), {arg_names})\n")

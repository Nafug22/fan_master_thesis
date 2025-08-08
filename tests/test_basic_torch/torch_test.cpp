#include <torch/torch.h>
#include <iostream>

int main() {
    // Create tensors directly on GPU
    torch::Tensor a = torch::tensor({1.0, 2.0, 3.0}, torch::device(torch::kCUDA));
    torch::Tensor b = torch::tensor({4.0, 5.0, 6.0}, torch::device(torch::kCUDA));

    // Add on GPU
    torch::Tensor c = a + b;

    // Print (move to CPU for readable output)
    std::cout << "a: " << a.cpu() << "\n";
    std::cout << "b: " << b.cpu() << "\n";
    std::cout << "a + b = " << c.cpu() << "\n";

    return 0;
}
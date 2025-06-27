import torch
from torchvision.models import alexnet, AlexNet_Weights

# Load model with modern weight API
weights = AlexNet_Weights.DEFAULT
model = alexnet(weights=weights)
model.eval()

# Move to CUDA if available
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.to(device)

# Create synthetic input
input_tensor = torch.randn(1, 3, 224, 224).to(device)

# Inference
with torch.no_grad():
    output = model(input_tensor)

print("Output shape:", output.shape)
print("Model is on:", next(model.parameters()).device)
print("Input is on:", input_tensor.device)
print("CUDA available:", torch.cuda.is_available())

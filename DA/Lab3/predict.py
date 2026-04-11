import argparse
import torch
from torchvision import transforms
from PIL import Image
from model import get_model


parser = argparse.ArgumentParser(description="Predict image class: person or dog")
parser.add_argument("--image_path", type=str, required=True, help="Path to input image")
parser.add_argument("--model_path", type=str, default="resnet18_binary.pth", help="Path to saved model weights")
args = parser.parse_args()


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")


model = get_model(pretrained=False, num_classes=2)  
model.load_state_dict(torch.load(args.model_path, map_location=device))
model = model.to(device)
model.eval()

transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.485, 0.456, 0.406],
                         std=[0.229, 0.224, 0.225])
])

image = Image.open(args.image_path).convert("RGB")
image = transform(image).unsqueeze(0).to(device)


with torch.no_grad():
    outputs = model(image)
    _, pred = torch.max(outputs, 1)

classes = ["Person", "Dog"]
print(f"Predicted class: {classes[pred.item()]}")
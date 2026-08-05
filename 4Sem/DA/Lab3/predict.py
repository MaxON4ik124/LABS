import argparse
import torch
import torch.nn.functional as F
from torchvision import transforms
from PIL import Image
from pathlib import Path
from model import get_model


def load_image(image_path, transform, device):
    try:
        image = Image.open(image_path).convert("RGB")
        image = transform(image).unsqueeze(0).to(device)
        return image
    except Exception as e:
        print(f"Error loading image {image_path}: {e}")
        return None


def predict_image(model, image_tensor):
    with torch.no_grad():
        outputs = model(image_tensor)
        probs = F.softmax(outputs, dim=1)
        conf, pred = torch.max(probs, 1)
    return pred.item(), conf.item()


def main():
    parser = argparse.ArgumentParser(description="Predict: person or dog")
    parser.add_argument("--image_path", type=str, required=True,
                        help="Path to image OR folder with images")
    parser.add_argument("--model_path", type=str, default="resnet18_binary.pth")
    args = parser.parse_args()

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

    model = torch.load(args.model_path, map_location=device, weights_only=False)
    model.to(device)
    model.eval()

    transform = transforms.Compose([
        transforms.Resize((224, 224)),
        transforms.ToTensor(),
        transforms.Normalize([0.485,0.456,0.406],
                             [0.229,0.224,0.225])
    ])

    classes = ["Person", "Dog"]

    path = Path(args.image_path)

    if path.is_dir():
        images = list(path.glob("*"))
    else:
        images = [path]

    for img_path in images:
        image_tensor = load_image(img_path, transform, device)
        if image_tensor is None:
            continue

        pred, conf = predict_image(model, image_tensor)

        print(f"{img_path.name:20} -> {classes[pred]} ({conf*100:.1f}%)")


if __name__ == "__main__":
    main()
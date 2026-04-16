import argparse
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, Dataset, random_split
from torchvision import transforms
from PIL import Image
from model import get_model
from datasets import load_dataset


class HuggingFaceImageDataset(Dataset):
    def __init__(self, dataset, label, transform=None):
        self.dataset = dataset
        self.label = label
        self.transform = transform

    def __len__(self):
        return len(self.dataset)

    def __getitem__(self, idx):

        item = self.dataset[idx]
        image = item['image'].convert("RGB")
        if self.transform:
            image = self.transform(image)
        return image, self.label


parser = argparse.ArgumentParser(description="Train ResNet-18 binary classifier")
parser.add_argument("--epochs", type=int, default=5, help="Number of training epochs")
parser.add_argument("--batch_size", type=int, default=32, help="Batch size")
parser.add_argument("--lr", type=float, default=1e-3, help="Learning rate")
parser.add_argument("--save_path", type=str, default="resnet18_binary.pth", help="Path to save model weights")
args = parser.parse_args()


print("Loading datasets...")
ds_peop = load_dataset("HK83/real_people_3000")['train']
ds_dogs = load_dataset("nasserCha/dog_dataset")['train']


transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.485, 0.456, 0.406],
                         std=[0.229, 0.224, 0.225])
])


people_dataset = HuggingFaceImageDataset(ds_peop, label=0, transform=transform)
dogs_dataset = HuggingFaceImageDataset(ds_dogs, label=1, transform=transform)

full_dataset = people_dataset + dogs_dataset


train_size = int(0.9 * len(full_dataset))
val_size = len(full_dataset) - train_size
train_dataset, val_dataset = random_split(full_dataset, [train_size, val_size])

train_loader = DataLoader(train_dataset, batch_size=args.batch_size, shuffle=True)
val_loader = DataLoader(val_dataset, batch_size=args.batch_size, shuffle=False)


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model = get_model(pretrained=True, num_classes=2).to(device)

criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=args.lr)


def train_one_epoch(model, loader, optimizer, criterion, device):
    model.train()
    running_loss = 0.0
    for images, labels in loader:
        images, labels = images.to(device), labels.to(device)
        optimizer.zero_grad()
        outputs = model(images)
        loss = criterion(outputs, labels)
        loss.backward()
        optimizer.step()
        running_loss += loss.item() * images.size(0)
    return running_loss / len(loader.dataset)

def validate(model, loader, device):
    model.eval()
    correct = 0
    total = 0
    with torch.no_grad():
        for images, labels in loader:
            images, labels = images.to(device), labels.to(device)
            outputs = model(images)
            _, preds = torch.max(outputs, 1)
            correct += (preds == labels).sum().item()
            total += labels.size(0)
    return correct / total
for epoch in range(args.epochs):
    train_loss = train_one_epoch(model, train_loader, optimizer, criterion, device)
    val_acc = validate(model, val_loader, device)
    print(f"Epoch {epoch+1}/{args.epochs}, Train Loss: {train_loss:.4f}, Val Acc: {val_acc:.4f}")
torch.save(model.state_dict(), args.save_path)
print(f"Model weights saved to {args.save_path}")
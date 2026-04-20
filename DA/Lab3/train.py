import argparse
import torch
import torch.nn as nn
import random
import numpy as np
import torch.optim as optim
from torch.utils.data import DataLoader, Dataset, random_split
from torchvision import transforms
from PIL import Image
from model import get_model
from datasets import load_dataset
from datasets import ClassLabel
import os

N = 1000


def set_seed(seed=42):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    torch.cuda.manual_seed_all(seed)

set_seed(42)
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

def main():
    parser = argparse.ArgumentParser(description="Train ResNet-18 binary classifier")
    parser.add_argument("--epochs", type=int, default=50, help="Number of training epochs")
    parser.add_argument("--batch_size", type=int, default=16, help="Batch size")
    parser.add_argument("--lr", type=float, default=1e-3, help="Learning rate")
    parser.add_argument("--save_path", type=str, default="resnet18_binary.pth", help="Path to save model weights")
    args = parser.parse_args()


    print("Loading datasets...")
    ds_peop = load_dataset("beurkinger/autotrain-data-human-action-recognition")['train'].shuffle(seed=42)
    ds_dogs = load_dataset("chandocchi/dog-dataset")['train'].shuffle(seed=42) 
    
    save_dir = 'predict'
    for i in range(int(N*0.2)):
        img = ds_peop[i]["image"]
        img.save(f"{save_dir}\\image_{i}P.jpg")
    for i in range(int(N*0.2)):
        img = ds_dogs[i]["image"]
        img.save(f"{save_dir}\\image_{i}D.jpg")

    ds_dogs = ds_dogs.select(range(N))
    ds_peop = ds_peop.select(range(N))


    split_people = ds_peop.train_test_split(test_size=0.2, seed=42)
    split_dogs   = ds_dogs.train_test_split(test_size=0.2, seed=42)

    train_people = split_people['train']
    val_people   = split_people['test']

    train_dogs = split_dogs['train']
    val_dogs   = split_dogs['test']

    train_transform = transforms.Compose([
        transforms.Resize((256, 256)),
        transforms.RandomResizedCrop(224),
        transforms.RandomHorizontalFlip(),
        transforms.RandomRotation(15),
        transforms.ColorJitter(brightness=0.2, contrast=0.2, saturation=0.2),
        transforms.ToTensor(),
        transforms.Normalize([0.485,0.456,0.406],
                            [0.229,0.224,0.225])
    ])

    val_transform = transforms.Compose([
        transforms.Resize((224, 224)),
        transforms.ToTensor(),
        transforms.Normalize([0.485,0.456,0.406],
                            [0.229,0.224,0.225])
    ])


    people_dataset = HuggingFaceImageDataset(ds_peop, label=0)
    dogs_dataset   = HuggingFaceImageDataset(ds_dogs, label=1)

    full_dataset = people_dataset + dogs_dataset

    train_size = int(0.8 * len(full_dataset))
    val_size = len(full_dataset) - train_size
    train_dataset = torch.utils.data.ConcatDataset([
        HuggingFaceImageDataset(train_people, label=0, transform=train_transform),
        HuggingFaceImageDataset(train_dogs,   label=1, transform=train_transform)
    ])

    val_dataset = torch.utils.data.ConcatDataset([
        HuggingFaceImageDataset(val_people, label=0, transform=val_transform),
        HuggingFaceImageDataset(val_dogs,   label=1, transform=val_transform)
    ])


    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = get_model(pretrained=True, num_classes=2)

    for param in model.parameters():
        param.requires_grad = False
    for param in model.fc.parameters():
        param.requires_grad = True

    model = model.to(device)

    criterion = nn.CrossEntropyLoss()

    optimizer = optim.Adam(model.fc.parameters(), lr=3e-4)

    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
        optimizer, mode='max', patience=3, factor=0.3
    )

    pin_memory = torch.cuda.is_available()
    train_loader = DataLoader(train_dataset,
                            batch_size=args.batch_size,
                            shuffle=True,
                            num_workers=2,
                            pin_memory=pin_memory)

    val_loader = DataLoader(val_dataset,
                            batch_size=args.batch_size,
                            shuffle=False,
                            num_workers=2,
                            pin_memory=pin_memory)



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

    best_acc = 0
    patience = 5
    epochs_no_improve = 0

    for epoch in range(args.epochs):

        train_loss = train_one_epoch(model, train_loader, optimizer, criterion, device)
        val_acc = validate(model, val_loader, device)

        scheduler.step(val_acc)

        print(f"Epoch {epoch+1}/{args.epochs} | Loss: {train_loss:.4f} | Val Acc: {val_acc:.4f}")

        if val_acc > best_acc:
            best_acc = val_acc
            torch.save(model.state_dict(), args.save_path)
            print("Best model saved!")
            epochs_no_improve = 0
        else:
            epochs_no_improve += 1

        if epochs_no_improve >= patience:
            print("Early stopping triggered")
            break
    print(f"Model weights saved to {args.save_path}")

if __name__ == '__main__':
    main()
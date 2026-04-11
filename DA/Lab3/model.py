from datasets import load_dataset
import torchvision.models as models
import torch.nn as nn
import torch

def get_model():
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = models.resnet18(pretrained=True)
    model.fc = nn.Linear(model.fc.in_features, 2)
    model = model.to(device)
    return model
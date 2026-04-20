from torchvision.models import resnet18, ResNet18_Weights
import torch.nn as nn

def get_model(pretrained, num_classes):
    weights = ResNet18_Weights.DEFAULT if pretrained else None
    model = resnet18(weights=weights)
    model.fc = nn.Linear(model.fc.in_features, num_classes)
    return model

import torch
import torch.nn as nn
import torch.nn.functional as F

class ActivityClassifierNet(nn.Module):
    def __init__(self, num_classes=3):
        super(ActivityClassifierNet, self).__init__()

        self.conv1 = nn.Conv2d(27, 12, kernel_size=(5, 1), padding='same')
        self.bn1 = nn.BatchNorm2d(12)
        self.pool1 = nn.AdaptiveAvgPool2d((80, 12))

        self.conv2 = nn.Conv2d(12, 20, kernel_size=(5, 1), padding='same')
        self.bn2 = nn.BatchNorm2d(20)
        self.pool2 = nn.AdaptiveAvgPool2d((60, 20))

        self.conv3 = nn.Conv2d(20, 30, kernel_size=(5, 1), padding='same')
        self.bn3 = nn.BatchNorm2d(30)
        self.pool3 = nn.AdaptiveAvgPool2d((40, 30))

        self.conv4 = nn.Conv2d(30, 40, kernel_size=(5, 1), padding='same')
        self.bn4 = nn.BatchNorm2d(40)
        self.pool4 = nn.AdaptiveAvgPool2d((20, 40))

        self.flatten = nn.Flatten()
        self.dropout1 = nn.Dropout(0.5)
        self.fc1 = nn.Linear(800, 512)
        self.dropout2 = nn.Dropout(0.5)
        self.fc2 = nn.Linear(512, 512)
        self.output_layer = nn.Linear(512, num_classes)

    def forward(self, x):

        x = self.pool1(F.relu(self.bn1(self.conv1(x))))
        x = self.pool2(F.relu(self.bn2(self.conv2(x))))
        x = self.pool3(F.relu(self.bn3(self.conv3(x))))
        x = self.pool4(F.relu(self.bn4(self.conv4(x))))

        x = self.flatten(x)
        x = self.dropout1(x)
        x = F.relu(self.fc1(x))
        x = self.dropout2(x)
        x = F.relu(self.fc2(x))
        output = self.output_layer(x)
        return F.softmax(output, dim=1)
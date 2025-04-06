import cv2
import torch
import time
import numpy as np
from torchvision import transforms
from PIL import Image
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision.datasets import CIFAR10

# Global Variables
MODEL_PATH = "cifar10_trained_model.pth"
DATA_PATH = "./data"
IMG_WIDTH, IMG_HEIGHT = 32, 32  # Based on training dataset
CONFIDENCE_THRESHOLD = 0.5  # Minimum confidence for valid detections
FILTER_WIDTH = 3
FILTER_HEIGHT = 3
BATCH_SIZE = 64
EPOCHS = 10
LEARNING_RATE = 0.001

# Neural Network Constants
OUT_CHANNELS1 = 16
OUT_CHANNELS2 = 32
FC1_INPUT = OUT_CHANNELS2 * 8 * 8  # 32 filters * 8x8 after pooling
FC1_OUTPUT = 128
NUM_CLASSES = 10
POOL_KERNEL = 2
POOL_STRIDE = 2

# Get max CPU and GPU count
MAX_CPU = torch.get_num_threads()
MAX_GPU = torch.cuda.device_count()

# Define CNN Model
class ObjectDetectionCNN(nn.Module):
    def __init__(self, num_classes=NUM_CLASSES):
        super(ObjectDetectionCNN, self).__init__()
        self.conv1 = nn.Conv2d(in_channels=3, out_channels=OUT_CHANNELS1, kernel_size=(FILTER_WIDTH, FILTER_HEIGHT), stride=1, padding=1)
        self.relu1 = nn.ReLU()
        self.pool1 = nn.MaxPool2d(kernel_size=POOL_KERNEL, stride=POOL_STRIDE)
        
        self.conv2 = nn.Conv2d(in_channels=OUT_CHANNELS1, out_channels=OUT_CHANNELS2, kernel_size=(FILTER_WIDTH, FILTER_HEIGHT), stride=1, padding=1)
        self.relu2 = nn.ReLU()
        self.pool2 = nn.MaxPool2d(kernel_size=POOL_KERNEL, stride=POOL_STRIDE)

        self.fc1 = nn.Linear(FC1_INPUT, FC1_OUTPUT)
        self.relu3 = nn.ReLU()
        self.fc2 = nn.Linear(FC1_OUTPUT, num_classes)
        self.softmax = nn.Softmax(dim=1)

    def forward(self, x):
        x = self.pool1(self.relu1(self.conv1(x)))
        x = self.pool2(self.relu2(self.conv2(x)))
        x = x.view(x.size(0), -1)  # Flatten
        x = self.relu3(self.fc1(x))
        x = self.softmax(self.fc2(x))
        return x

# Load model
def load_model():
    print("Loading model...")
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = ObjectDetectionCNN(num_classes=NUM_CLASSES).to(device)
    model.load_state_dict(torch.load(MODEL_PATH, map_location=device))
    model.eval()
    print("Model loaded successfully.")
    return model, device

# Train model
def train_model():
    print("Starting training...")
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    
    # Data transformations
    transform = transforms.Compose([
        transforms.Resize((IMG_WIDTH, IMG_HEIGHT)),
        transforms.ToTensor()
    ])
    # Load dataset
    train_dataset = CIFAR10(root=DATA_PATH, train=True, transform=transform, download=True)
    train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True, num_workers=MAX_CPU)

    model = ObjectDetectionCNN(num_classes=NUM_CLASSES).to(device)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=LEARNING_RATE)

    # Training loop
    for epoch in range(EPOCHS):
        start_time = time.time()
        running_loss = 0.0

        for images, labels in train_loader:
            images, labels = images.to(device), labels.to(device)

            optimizer.zero_grad()
            outputs = model(images)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()

            running_loss += loss.item()

        end_time = time.time()
        print(f"Epoch {epoch + 1}/{EPOCHS}, Loss: {running_loss:.4f}, Time: {end_time - start_time:.4f} sec")

    # Save trained model
    torch.save(model.state_dict(), MODEL_PATH)
    print("Training completed and model saved.")

# Preprocess a frame before passing it to the model
def preprocess_frame(frame):
    print("Preprocessing frame...")
    transform = transforms.Compose([
        transforms.Resize((IMG_WIDTH, IMG_HEIGHT)),
        transforms.ToTensor()
    ])
    frame = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))  # Convert OpenCV frame to PIL Image
    return transform(frame).unsqueeze(0)  # Add batch dimension

# Capture image from webcam
def capture_image():
    print("Opening webcam...")
    cap = cv2.VideoCapture(0)  # Open webcam
    if not cap.isOpened():
        print("Error: Could not open webcam.")
        return None

    ret, frame = cap.read()  # Capture a single frame
    cap.release()  # Release camera resource

    if not ret:
        print("Error: Could not capture frame.")
        return None

    print("Captured image successfully.")
    return frame

# Detect objects in the image
def detect_objects(model, device, frame):
    print("Detecting objects...")
    start_time = time.time()  # Start timing

    input_tensor = preprocess_frame(frame).to(device)
    with torch.no_grad():
        outputs = model(input_tensor)

    # Post-process outputs (simulated for example)
    detected_objects = []
    for idx, confidence in enumerate(outputs[0]):
        if confidence.item() > CONFIDENCE_THRESHOLD:  # If confidence is high enough
            label = f"Object_{idx}"  # Example label
            box = (10, 10, 50, 50)  # Example bounding box (x1, y1, x2, y2)
            detected_objects.append((label, confidence.item(), box))

    end_time = time.time()  # End timing
    print(f"Detection completed in {end_time - start_time:.4f} seconds.")
    
    return detected_objects

# Main function to run everything
if __name__ == "__main__":
    train_model()  # Train model first
    model, device = load_model()
    frame = capture_image()
    if frame is not None:
        detected_objects = detect_objects(model, device, frame)
        print("Detected Objects:", detected_objects)


import cv2

def main():
    # Open the video capture device (camera with index 0)
    cap = cv2.VideoCapture(2)

    # Check if the camera was opened successfully
    if not cap.isOpened():
        print("Error: Could not open camera")
        return

    # Loop to continuously capture frames
    while True:
        # Read a frame from the camera
        ret, frame = cap.read()
        print("read frame")



        # Check if frame was successfully captured
        if not ret:
            print("Error: Failed to capture frame")
            break


    # Release the camera and close any OpenCV windows
    cap.release()

if __name__ == "__main__":
    main()


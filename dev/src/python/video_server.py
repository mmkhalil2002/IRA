

import socket
import threading
import cv2
import struct
import fcntl
import sys
import getopt
import signal
import cv2
from io import BytesIO
import numpy as np

# Global variables to hold configuration options
cap_dev = 0          # Default video device
video_port = 0       # Video server port
ctr_port = 0         # Control server port

# Placeholder for the video capture object
# Global settings
video_port = 8080  # Replace with the desired port number
client_status = {}
color_frame = True



# Camera setup
cap = None

debug_level = 1  # Example debug level for output




def zoom_in(frame, zoom_factor):
    """
    Zooms into the frame by the specified zoom factor.
    Args:
        frame: Input frame.
        zoom_factor: Factor to zoom in (e.g., 2.0 for 2x zoom).
    Returns:
        Zoomed-in frame.
    """
    h, w, _ = frame.shape
    center_x, center_y = w // 2, h // 2

    # Calculate cropping box
    radius_x, radius_y = int(w / (2 * zoom_factor)), int(h / (2 * zoom_factor))

    # Crop the image
    cropped_frame = frame[center_y - radius_y:center_y + radius_y, center_x - radius_x:center_x + radius_x]

    # Resize back to original size
    zoomed_frame = cv2.resize(cropped_frame, (w, h), interpolation=cv2.INTER_LINEAR)
    return zoomed_frame


def zoom_out(frame, zoom_factor):
    """
    Zooms out of the frame by the specified zoom factor.
    Args:
        frame: Input frame.
        zoom_factor: Factor to zoom out (e.g., 2.0 for 0.5x zoom).
    Returns:
        Zoomed-out frame.
    """
    h, w, channels  = frame.shape
    print(f"Height: {h}, Width: {w}, Channels: {channels}")
    new_w, new_h = int(w * zoom_factor), int(h * zoom_factor)

    # Resize the frame to smaller dimensions
    resized_frame = cv2.resize(frame, (new_w, new_h), interpolation=cv2.INTER_LINEAR)

    # Pad the resized frame to fit original size
    top = (h - new_h) // 2
    bottom = h - new_h - top
    left = (w - new_w) // 2
    right = w - new_w - left

    padded_frame = cv2.copyMakeBorder(resized_frame, top, bottom, left, right, cv2.BORDER_CONSTANT, value=(0, 0, 0))
    return padded_frame




def initialize_camera(cap_dev):
    global cap, img_height, img_width, Brightness, Contrast, Saturation, Gain
    if cap is not None:
        cap.release()  # Release the previous source if it exists

    cap = cv2.VideoCapture(cap_dev)  # Open the video device

    # Check if the camera opened successfully
    if not cap.isOpened():
        print(f"Error: Could not open camera with device index {cap_dev}")
        return None
# Set video properties
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

    # Retrieve video properties
    img_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    img_width  = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    Brightness = int(cap.get(cv2.CAP_PROP_BRIGHTNESS))
    Contrast   = int(cap.get(cv2.CAP_PROP_CONTRAST))
    Saturation = int(cap.get(cv2.CAP_PROP_SATURATION))
    Gain       = int(cap.get(cv2.CAP_PROP_GAIN))

    # Debug output
    if debug_level == 1:
        print(f"Height: {img_height}")
        print(f"Width: {img_width}")
        print(f"Brightness: {Brightness}")
        print(f"Contrast: {Contrast}")
        print(f"Saturation: {Saturation}")
        print(f"Gain: {Gain}")



def get_ip_address(interface_name: str) -> str:
    """
    Get the IP address of a network interface on a Linux system.

    :param interface_name: Name of the interface (e.g., 'eth0', 'wlan0')
    :return: IP address as a string
    """
    try:
        # Create a socket to interface with the network stack
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Use the ioctl system call to get interface details
        iface_info = fcntl.ioctl(
            sock.fileno(),
            0x8915,  # SIOCGIFADDR
            struct.pack('256s', interface_name[:15].encode('utf-8'))
        )

        # Extract and return the IP address
        ip_address = socket.inet_ntoa(iface_info[20:24])
        print(f"System IP Address is: {ip_address}")
        return ip_address

    except OSError as e:
        print(f"Error accessing interface {interface_name}: {e}")
        return None




# Global variables
ctr_port = 5001  # Replace with your desired control port


def recv_ctr_data(client_socket):
    """
    Function to handle receiving control data from a connected client.
    """
    try:
        while True:
            data = client_socket.recv(1024)  # Adjust buffer size as needed
            if not data:
                print("Client disconnected.")
                break
            print(f"Received control data: {data.decode('utf-8')}")
    except ConnectionResetError:
        print("Connection reset by client.")
    finally:
        client_socket.close()




def ctr_server():
    """
    Control server function. Listens for incoming connections on a control port
    and spawns a thread to handle each connection.
    """
    global ctr_port
    local_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print(f"Local socket created: {local_socket}")

    try:
        ip_address = get_ip_address("eth0")  # Replace "eth0" with your network interface
        if not ip_address:
            print("Failed to retrieve IP address.")
            return

        local_socket.bind((ip_address, ctr_port))
        local_socket.listen(3)
        print(f"Waiting for control connections...\nServer Port: {ctr_port}")

        while True:
            print("Before accept...")
            client_socket, client_address = local_socket.accept()
            print(f"Connection accepted from {client_address}")
            client_status[client_address] = True

            # Create a thread to handle the control connection
            thread = threading.Thread(target=recv_ctr_data, args=(client_socket,))
            thread.start()

    except socket.error as e:
        print(f"Socket error: {e}")
    except KeyboardInterrupt:
        print("Server shutting down...")
    finally:
        local_socket.close()


# Function to send data to the client
def send_gray_data(client_socket):
    try:
        while True:
            # Capture video frame
            ret, frame = cap.read()
            if not ret:
                print("Failed to read frame from webcam.")
                break
            # Assuming the frame is already in RGB (for demonstration)
            rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert to RGB for simulation

            # Now convert the RGB frame to grayscale
            gray_frame = cv2.cvtColor(rgb_frame, cv2.COLOR_RGB2GRAY) 
            # Flatten the frame into a raw byte sequence
            start = bytearray([0xaf, 0xbf, 0xcf, 0xdf])
            end   = bytearray([0xfa, 0xfb, 0xfc, 0xfd])
            raw_data =gray_frame.tobytes()
            # Use bytearray to concatenate start, frame, and end
            concatenated_bytes = bytearray()
            concatenated_bytes.extend(start)  # Add prefix
            concatenated_bytes.extend(raw_data)  # Add frame bytes
            concatenated_bytes.extend(end)  # Add suffix
            # Send frame size first (packed as unsigned int)
            #frame_size = len(raw_data)
            #client_socket.sendall(struct.pack('!I', frame_size))

            # Send the frame data
            client_socket.sendall(concatenated_bytes)

    except Exception as e:
        print(f"Error in send_grey_data: {e}")
    finally:
        client_socket.close()
        print(f"Closed connection with client {client_socket}")



# Function to send data to the client
def send_color_data(client_socket):
    try:
        while True:
            # Capture video frame
            ret, frame = cap.read()
            if not ret:
                print("Failed to read frame from webcam.")
                break
            start = bytearray([0xaf, 0xbf, 0xcf, 0xdf])
            end = bytearray([0xfa, 0xfb, 0xfc, 0xfd])
            raw_data =frame.tobytes()
            # Use bytearray to concatenate start, frame, and end
            concatenated_bytes = bytearray()
            concatenated_bytes.extend(start)  # Add prefix
            concatenated_bytes.extend(raw_data)  # Add frame bytes
            concatenated_bytes.extend(end)  # Add suffix
            # Send frame size first (packed as unsigned int)
            #frame_size = len(raw_data)
            #client_socket.sendall(struct.pack('!I', frame_size))

            # Send the frame data
            client_socket.sendall(concatenated_bytes)

    except Exception as e:
        print(f"Error in send_color_data: {e}")
    finally:
        client_socket.close()
        print(f"Closed connection with client {client_socket}")




# Main video server function
def video_server():
    print("START VIDEO SERVER")

    try:
        # Create a socket
        local_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print(f"Socket created: {local_socket}")

        # Get IP address
        addr = get_ip_address("eth0")
        if not addr:
            print("Failed to get IP address.")
            return

        # Bind the socket to the address and port
        local_socket.bind((addr, video_port))
        print(f"Socket bound to {addr}:{video_port}")

        # Listen for incoming connections
        local_socket.listen(3)
        print(f"Listening for connections on port {video_port}...")

        while True:
            print("Before accept...")
            client_socket, client_address = local_socket.accept()
            print(f"Accepted connection from {client_address}")


            # Start a thread to handle this client
            if (color_frame):
                client_thread = threading.Thread(target=send_color_data, args=(client_socket,))
                client_thread.start()
            else:
                client_thread = threading.Thread(target=send_gray_data, args=(client_socket,))
                client_thread.start()
    except Exception as e:
        print(f"Error in send_color_data: {e}")
    finally:
        client_socket.close()
        print(f"Closed connection with client {client_socket}")





def sig_handler(signum, frame):
    print(f"Signal {signum} received. Terminating...")
    sys.exit(0)



def record_video():
    print("Starting video recording thread...")
    # Placeholder for video recording functionality
    # e.g., save video frames to a file
    pass


#  python3 server.py -v 0 -p 5000 -c 5001


def main(argv):
    global cap_dev, video_port, ctr_port
    print("Starting server...")

    # Setup signal handlers
    signal.signal(signal.SIGPIPE, sig_handler)
    signal.signal(signal.SIGINT, sig_handler)

    # Parse command-line arguments
    try:
        opts, args = getopt.getopt(argv, "v:p:c:")
    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    for opt, arg in opts:
        if opt == "-v":
            cap_dev = int(arg)
            print(f"Video device: {cap_dev}")
        elif opt == "-p":
            video_port = int(arg)
            print(f"Video port: {video_port}")
        elif opt == "-c":
            ctr_port = int(arg)
            print(f"Control port: {ctr_port}")
        else:
            print(f"Unrecognized option: {opt}")
            sys.exit(2)

        if args:
           print("Non-option arguments:", args)

    # Initialize video capture and other resources
    initialize_camera(cap_dev)

    # Start threads
    video_thread = threading.Thread(target=video_server)
    ctr_thread = threading.Thread(target=ctr_server)
    record_thread = threading.Thread(target=record_video)

    video_thread.start()
    ctr_thread.start()
    record_thread.start()

    # Wait for threads to complete
    video_thread.join()
    ctr_thread.join()
    record_thread.join()

if __name__ == "__main__":
    main(sys.argv[1:])


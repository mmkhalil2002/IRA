sudo apt update
sudo apt install -y build-essential cmake git pkg-config libjpeg-dev libpng-dev libtiff-dev \
libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libgtk-3-dev libatlas-base-dev \
gfortran python3-dev python3-numpy libeigen3-dev libxslt1-dev \
libopencv-dev
sudo apt install openssl libssl-dev
sudo apt install build-essential libncurses-dev flex bison
sudo apt install python3-pip -y
sudo pip install torch torchvision opencv-python --break-system-packages

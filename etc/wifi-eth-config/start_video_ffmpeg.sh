#! /bin/bash
#
# Diffusion youtube avec ffmpeg

# Configurer youtube avec une résolution 720p. La vidéo n'est pas scalée.

VBR="2500k"                                    # Bitrate de la vidéo en sortie
FPS="30"                                       # FPS de la vidéo en sortie
QUAL="medium"                                  # Preset de qualité FFMPEG
TUNE="zerolatency"                                  # Preset de qualité FFMPEG
FS=640x480
SOURCE="/dev/video0"                           # Source UDP (voir les annonces SAP)
WIDTH=1280
HEIGHT=720

YOUTUBE_URL="rtmp://a.rtmp.youtube.com/live2"
KEY="4kvg-b4q3-bta3-g9h7-5j5k"
ffmpeg -thread_queue_size 512 -f v4l2 -i /dev/video0 \
  -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero -acodec aac -ab 128k -strict experimental -fflags nobuffer  \
  -aspect 16:9 -vcodec h264  -preset ultrafast -tune zerolatency   -crf 25 -pix_fmt yuv420p -g 60 -vb 820k -maxrate 820k -bufsize 820k -profile:v baseline \
  -r 10 -f flv $YOUTUBE_URL/$KEY  

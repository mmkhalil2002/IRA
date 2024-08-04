x=$(ls -al /dev/serial/by-id | grep Controller | awk '{print $11}' | tr . ' ' | tr / ' ')
y=/dev/$x
echo device=$y
devise=$(echo $y | tr -d ' ')
DAEMON_ARGS="--master=$devise --out=udp:25.6.38.89:14552  --aircraft=copter --mission=flyby --state-basedir=logs --daemon"
echo $DAEMON_ARGS
#mavproxy.py  --master=$y --out=udp:25.6.38.89:14552  --aircraft=copter --mission=flyby --state-basedir=logs 


mkdir /sys/fs/cgroup/memory/kv256
echo 128G > /sys/fs/cgroup/memory/kv256/memory.limit_in_bytes
echo 0 > /sys/fs/cgroup/memory/kv256/memory.swappiness

chown -R root:root /sys/fs/cgroup/memory/kv*

# set CPU in performance mode
cmd='-g performance'
MAX_CPU=$((`nproc --all` - 1))
for i in $(seq 0 $MAX_CPU); do
    echo "Changing CPU " $i " with parameter "$cmd;
    cpufreq-set -c $i $cmd ;
done
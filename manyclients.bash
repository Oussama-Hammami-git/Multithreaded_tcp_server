for N in {1..50}
    do
        echo "multithreaded_tcp.c" | nc 127.0.0.1 12345 > /dev/null &
    done

    wait
./build/main
trap 'if [[ $? -eq 139 ]]; then echo "segfault !"; fi' CHLD

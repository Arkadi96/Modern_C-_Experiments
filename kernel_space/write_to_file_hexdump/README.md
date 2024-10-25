## File hierarchy
```
write_to_file_hexdump/
├── hexdump.c
├── hex_results
│   ├── output_1
│   ├── output_2
│   └── output_3
├── Makefile
├── README.md
├── test.c
├── test_files
│   ├── test.mp3
│   ├── test.mp4
│   └── test.txt
└── test.sh
```

# How to run the hexdump program

## Step 1: Build the project
```
$ make clean
$ make
```
## Step 2: Load the kernel
```
$ sudo insmod hexdump.ko
$ sudo chmod 666 /dev/hexdump
```
## Step 2: Run the test
```
$ chmod u+x test.sh
$ ./test.sh
```
## Step 3: Unload the LKM
```
$ sudo rmmod hexdump
```


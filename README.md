# Transferring-file-with-tcp

## Compile
1. compile them directly  
    $ gcc server.c -o server  
    $ gcc client.c -o client

2. use makefile  
    $ make

## Transfer

Server side `$ ./server [port] [file-name]`  
Client side `$ ./client [port] [server-IP]`  
Execute server first or client first.  

### Example
```shell=
$ ./server 9999 test.jpg
Waiting for receiver...
Connect successfully. Ready to transfer 'test.jpg'[52.61 KB]...
Transfer completed.
```

```shell=
$ ./client 9999 127.0.0.1
trying connect to server...
Connected successfully.
Ready to download 'test.jpg'[52.61 KB]...
Compeleted.
```

## Select 

select is a system call that allows waiting on multiple fds

#### Select Server

This is a TCP Server that implements select to connect and communicate with mulitple clients at once. 

#### Build

``` sh
mkdir build 
cd build
cmake ..
make 
```

#### Run Server

``` sh
./server
```

#### Run Client

``` sh
./client
```

#### More on select

**USAGE**
``` c++
int select(int nfds, fd_set *readfds,
                  fd_set *writefds,
                  fd_set *exceptfds,
                  timeval *timeout
                  );

void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, const fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
```

[MAN page link](https://www.man7.org/linux/man-pages/man2/select.2.html)

***WARNING:*** *select() can monitor only file descriptors numbers that are less than FD_SETSIZE (1024)*
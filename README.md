# thallium-rpc-ordering

This example has an RPC client and server using thallium. The client invokes `hello(data)` RPC
with increasing integer values.  The server performs a monotonic check to verify the RPC was invoked in order.
This check fails when the buffer size varies for each even and odd integer.

## Build
Make sure thallium can be discovered by CMake.

```sh
cmake -GNinja -S . -B build
cmake --build build
```

## Run the server
### Linux
```sh
$ ./tl-server --url tcp://localhost:1234
Server running at address ofi+tcp;ofi_rxm://xx.xx.xx.xx:1234
```
### MacOS
```sh
$ ./tl-server --url sockets
Server running at address ofi+sockets://xx.xx.xx.xx:xxxx
```

## Run the client
### Linux
```sh
$ ./tl-client --url tcp://xx.xx.xx.xx:1234
```
### MacOS
```sh
$ ./tl-client --url sockets://xx.xx.xx.xx:xxxx
```

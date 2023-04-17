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
```sh
$ ./tl-server
Server running at address ofi+tcp;ofi_rxm://xx.xx.xx.xx:35187
Data[0] 1
Data[0] 3
terminate called after throwing an instance of 'std::runtime_error'
  what():  Unexpected head! [3-1 != 1]
Aborted (core dumped)
```

## Run the client
```sh
$ ./tl-client tcp://xx.xx.xx.xx:35187
Invoking hello(1)
Invoking hello(2)
Invoking hello(3)
```

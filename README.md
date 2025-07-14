- sudo apt update first
- sudo install these packages for cmake:

protobuf-compiler 
libprotobuf-dev
protobuf-c-compiler
libgrpc++-dev
grpc-protoc-plugin
nlohmann-json3-dev
protobuf-compiler-grpc

- make python 3.10 .venv with requirements.txt
- make .py files from .proto using : python -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. echo.proto
- use cmake to build c++ grpc server and run it using build file
- client python script has functions to get protobuf definition of request and response messages with its types.

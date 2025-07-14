#!/bin/bash

# Generate C++ files
protoc -I=. --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` json_reflection.proto

# Generate Python files
python -m grpc_tools.protoc -I=. --python_out=../client/ --grpc_python_out=../client/ json_reflection.proto
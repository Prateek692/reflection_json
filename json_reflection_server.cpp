#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
// #include <grpcpp/reflection.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <nlohmann/json.hpp>
#include "proto/json_reflection.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using json_reflection::JsonReflectionService;
using json_reflection::JsonFieldRequest;
using json_reflection::JsonFieldResponse;
using json_reflection::FieldList;
using json_reflection::FieldModification;
using json_reflection::OperationStatus;
using json_reflection::Empty;
using json = nlohmann::json;

class JsonReflectionServiceImpl final : public JsonReflectionService::Service {
public:
    JsonReflectionServiceImpl(const std::string& json_path) {
        std::ifstream f(json_path);
        json_data = json::parse(f);
    }

    Status GetJsonField(ServerContext* context, const JsonFieldRequest* request,
        JsonFieldResponse* response) override {
        try {
            // Convert field_path to JSON pointer (replace . with / and add / prefix)
            std::string pointer = "/" + request->field_path();
            std::replace(pointer.begin(), pointer.end(), '.', '/');

            // Use JSON pointer to access nested fields
            auto& value = json_data[json::json_pointer(pointer)];
            response->set_value(value.dump());
            return Status::OK;
        } 
        catch (const std::exception& e) {
            response->set_value("Error: " + std::string(e.what()));
            return Status::OK;
        }
    }

    
    void list_fields_recursive(const json& j, const std::string& prefix, FieldList* response) {
        for (auto& el : j.items()) {
            std::string full_path = prefix.empty() ? el.key() : prefix + "." + el.key();
            response->add_fields(full_path);
            
            if (el.value().is_object()) {
                list_fields_recursive(el.value(), full_path, response);
            }
        }
    }

    Status ListFields(ServerContext* context, const Empty* request,
                    FieldList* response) override {
        list_fields_recursive(json_data, "", response);
        return Status::OK;
    }

    Status ModifyField(ServerContext* context, const FieldModification* request,
        OperationStatus* response) override {
        try {
            std::string pointer = "/" + request->field_path();
            std::replace(pointer.begin(), pointer.end(), '.', '/');

            json_data[json::json_pointer(pointer)] = json::parse(request->new_value());
            response->set_success(true);
            response->set_message("Field updated successfully");
        } catch (const std::exception& e) {
            response->set_success(false);
            response->set_message("Error: " + std::string(e.what()));
        }
        return Status::OK;
    }

private:
    json json_data;
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    std::string json_file("sample.json");
    
    JsonReflectionServiceImpl service(json_file);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    // Enable server reflection
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    std::unique_ptr<Server> server(builder.BuildAndStart());
    
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
import grpc
import json
import json_reflection_pb2
import json_reflection_pb2_grpc

def run():
    channel = grpc.insecure_channel('localhost:50051')
    stub = json_reflection_pb2_grpc.JsonReflectionServiceStub(channel)
    
    # List all fields (including nested)
    print("Listing all fields (including nested):")
    response = stub.ListFields(json_reflection_pb2.Empty())
    for field in response.fields:
        print(f" - {field}")
    
    # Access nested field
    print("\nGetting nested field 'address.city':")
    response = stub.GetJsonField(json_reflection_pb2.JsonFieldRequest(
        field_path="address.city"))
    print(f"Value: {response.value}")
    
    # Modify nested field
    print("\nModifying 'contacts.0.value':")
    response = stub.ModifyField(json_reflection_pb2.FieldModification(
        field_path="contacts.0.value",
        new_value='"john.doe@example.com"'
    ))
    print(f"Success: {response.success}, Message: {response.message}")
    
    # Verify the change
    response = stub.GetJsonField(json_reflection_pb2.JsonFieldRequest(
        field_path="contacts.0.value"))
    print(f"New value: {response.value}")

if __name__ == '__main__':
    run()
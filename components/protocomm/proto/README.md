# Protobuf files for implementing protocol communication packets

Protocomm uses Google Protobuf for language, transport and architecture agnostic protocol communication. These proto files define the protocomm packet structure, separated across multiple files:
* contants.proto - Defines the "Status" structure for conveying the success or failure of a single protocomm transaction
* sec0.proto - Defines the Security0 Command and Response packet structures
* sec1.proto - Defines the Security1 Command and Response packet structures
* session.proto - Defines the protocomm transacion packets for session establishment which internally has a Security packet as payload

Note : These proto files are not automatically compiled during the build process.

Run "make" (Optional) to generate the respective C and Python files. The generated C files are used by protocomm itself to create, delete and manipulate transaction packets. The generated Python files can be used by python based applications for implementing client side interface to protocomm layer.

Compilation requires protoc (Protobuf Compiler) and protoc-c (Protobuf C Compiler) installed. Since the generated files are to remain the same, as long as the proto files are not modified, therefore the generated files are already available under "protocomm/proto-c" and "protocomm/python" directories, and thus running make (and installing the Protobuf compilers) is optional.

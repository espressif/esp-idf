all: c_proto python_proto

c_proto: *.proto
	@protoc-c --c_out=../proto-c/ *.proto

python_proto: *.proto
	@protoc --python_out=../python/ *.proto

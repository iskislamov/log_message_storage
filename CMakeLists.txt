cmake_minimum_required(VERSION 3.16)

project("Log Message Serializarion")

add_definitions(-DUNICODE -D_UNICODE)

set(sources
	main.cpp
	log_messages_storage.h
	log_messages_storage.cpp)
	
add_executable(serializer ${sources})

set_property(TARGET serializer PROPERTY CXX_STANDARD 17)

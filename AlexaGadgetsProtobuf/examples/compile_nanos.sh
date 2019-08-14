#
# Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
# These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
# The Agreement is available at https://aws.amazon.com/asl/.
# See the Agreement for the specific terms and conditions of the Agreement.
# Capitalized terms not defined in this file have the meanings given to them in the Agreement.
#
#!/bin/bash

echo 'compiling proto files'

echo $(pwd)
echo $USER
PROTO_COMPILE_PATH='/Users/'$USER'/Downloads/nanopb-0.3.9.1-macosx-x86/generator-bin'
PROTO_COMMAND_NAME='protoc'

PROTO_COMMAND=$PROTO_COMPILE_PATH/$PROTO_COMMAND_NAME
INPUT_COMMON='-I=../common/ '
INPUT_TAG='-I='
PROTO_EXT='*.proto'
FORWARD_SLASH_PATH='/'
SPACE=' '
NANOPB_OUT='--nanopb_out=-I'
COLON_SEP=':'
CURRENT_FOLDER='.'

all_proto_files=$(find ../. -name "*.proto")
#echo $all_proto_files

for file in $all_proto_files; do
        file_path=${file%/*}
        echo 'filepath:' $file_path

	#Filter out the duplicate paths here TODO
	input_path=$INPUT_TAG$file_path$FORWARD_SLASH_PATH$SPACE
        #Use the below for output files in the same folders as input folders
	#output_path=$NANOPB_OUT$file_path$FORWARD_SLASH_PATH$COLON_SEP$file_path$FORWARD_SLASH_PATH$SPACE
	output_path=$NANOPB_OUT$file_path$FORWARD_SLASH_PATH$COLON_SEP$CURRENT_FOLDER
        #echo $output_path
	proto_path=$file_path$FORWARD_SLASH_PATH$PROTO_EXT

	#Run the proto compilation
	$PROTO_COMMAND \
	$INPUT_COMMON \
	$input_path \
	$output_path \
	$proto_path
done

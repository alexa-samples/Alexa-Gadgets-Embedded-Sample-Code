@echo off

REM ///////////////
REM Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
REM
REM Licensed under the Amazon Software License (the "License"). You may not use this file except in
REM compliance with the License. A copy of the License is located at
REM
REM    http://aws.amazon.com/asl/
REM
REM or in the "license" file accompanying this file. This file is distributed on an "AS IS" BASIS,
REM WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express or implied. See the License for the specific
REM language governing permissions and limitations under the License.
REM ///////////////


REM INSTRUCTIONS
REM Run this script from the \examples folder
REM change the PROTO_COMPILE_PATH variable below to point to the install location of Nanopb that contains protoc.exe
setlocal
set PROTO_COMPILE_PATH=C:\nanopb\nanopb-0.3.9.1-windows-x86\generator-bin\


set PROTO_COMMAND_NAME=protoc.exe
set PROTO_COMMAND=%PROTO_COMPILE_PATH%%PROTO_COMMAND_NAME%
set INPUT_ROOT=%cd%\..\..\..
set OUTPUT_PATH=%cd%


if exist %PROTO_COMMAND% (
    echo protoc compiler found..
) else (
    echo protoc not found, exiting.
	goto :eof
)

for /R  %INPUT_ROOT%\ConnectionHelpers\BLE\Proto %%G in (*.proto) do (
	cd %%~pG
	echo Compiling %%G
	%PROTO_COMMAND% -I%INPUT_ROOT%\AlexaGadgetsProtobuf\common;. --nanopb_out=%OUTPUT_PATH% %%~nG%%~xG
)

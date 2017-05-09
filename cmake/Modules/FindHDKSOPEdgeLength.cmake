#[[
	IMPORTANT! ------------------------------------------
	-----------------------------------------------------

	Author: 	SNWOFLAKE
	Email:		snowflake@outlook.com

	LICENSE ------------------------------------------

	Copyright (c) 2017 SNOWFLAKE
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]

#[[-----------------------------------------------------------------
INCLUDES                                                           |
------------------------------------------------------------------]]

Include(FindPackageHandleStandardArgs)
Include(HDKUtils)

#[[-----------------------------------------------------------------
DEFAULTS                                                           |
------------------------------------------------------------------]]

# specify module name
Set(____module_name____ "hou-hdk-sop-setedgelength")

# specify source directory
Set(____module_source_dir____ "${CMAKE_CURRENT_SOURCE_DIR}/../source")

#[[-----------------------------------------------------------------
MAIN                                                               |
------------------------------------------------------------------]]
	
HDK_MODULE_NAME_TOUPPER()
if(EXISTS ${____module_source_dir____})

	# find all header files
	HDK_MODULE_GET_INCLUDE_FILES(${____module_source_dir____})
	
	# find all source files
	HDK_MODULE_GET_SOURCE_FILES(${____module_source_dir____})		
	
	# report back
	Find_Package_Handle_Standard_Args(${____module_name_toupper____} 
		DEFAULT_MSG 
		____module_source_dir____
	)
		
	# output data
	Set(HDK_SOP_EDGELENGTH_INCLUDE_DIR 	"${____module_source_dir____}/include")
	Set(HDK_SOP_EDGELENGTH_INCLUDE_FILES ${____module_include_files____})
	Set(HDK_SOP_EDGELENGTH_SOURCE_FILES 	${____module_source_files____})
else()
	Message(STATUS "Didn't found ${____module_name_toupper____}: ${____module_source_dir____}")	
endif()
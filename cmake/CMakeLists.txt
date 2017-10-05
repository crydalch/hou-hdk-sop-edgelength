#[[
	IMPORTANT! ------------------------------------------
	-----------------------------------------------------

	Author: 	SWANN
	Email:		sebastianswann@outlook.com

	LICENSE ------------------------------------------

	Copyright (c) 2017 SWANN
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]

#[[-----------------------------------------------------------------
DEFAULTS                                                           |
------------------------------------------------------------------]]

Project(SOP_EdgeLength)

# set available compile configuration types
#set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo")
set(CMAKE_CONFIGURATION_TYPES "Release")

# get custom modules
Set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/Modules")

#[[-----------------------------------------------------------------
MAIN                                                               |
------------------------------------------------------------------]]

if(NOT DEFINED ENV{GIT_PATH})
	Message(FATAL_ERROR "GIT_PATH environment variable is not defined.")
else()	
	# find all "hou-hdk-sop-template" files
	Find_Package(HDKSOPEdgeLength REQUIRED)

	# TODO: setup directories and definitions for this project	
	Add_Library(${PROJECT_NAME} 
		SHARED
		${HDK_SOP_EDGELENGTH_INCLUDE_FILES} 
		${HDK_SOP_EDGELENGTH_SOURCE_FILES}
	)
	
	# pack operator files into its own filter
	Source_Group("Operators\\Headers" FILES ${HDK_SOP_EDGELENGTH_INCLUDE_FILES})
	Source_Group("Operators\\Source" FILES ${HDK_SOP_EDGELENGTH_SOURCE_FILES})		
	
	Target_Include_Directories(${PROJECT_NAME} 
		PUBLIC "${HDK_SOP_EDGELENGTH_INCLUDE_DIR}" 
		PRIVATE ${HDK_COMMON_INCLUDE_DIR} ${HOUDINI_INCLUDE_DIRS})
		
	Target_Link_Libraries(${PROJECT_NAME} ${HOUDINI_LIBRARIES})
	
	# specify destination path	
	HOU_SETUP_DSO_OUTPUTS(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME})
endif()
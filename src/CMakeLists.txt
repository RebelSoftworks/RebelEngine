##========= Copyright (c) 2026 Mohamed Ashraf, All rights reserved. ============//
##
## This software is proprietary and confidential.
##
## You are granted a limited, non-exclusive, non-transferable license to use and
## modify this software for personal or internal purposes only. You may distribute
## compiled binary builds of the software provided that such distribution does not
## expose, include, or facilitate access to the source code.
##
## You may not distribute, publish, disclose, or otherwise make available the
## source code, in whole or in part, whether original or modified. All rights
## not expressly granted herein are reserved by the copyright holder.
##
## This software is provided "as is", without warranty of any kind, express or
## implied.
##
##=============================================================================//
cmake_minimum_required(VERSION 3.15)
project(Dispersion)

set(SRCDIR ${CMAKE_SOURCE_DIR})

## From https://pastebin.com/FEFypFX4

set(Games
    "STEAMPAL"	
##	"CSTRIKE"
##	"CSTRIKE2"
##	"DOD"
##  "DINODDAY"
##	"HL2"
##	"PORTAL"
##	"PORTALMP"
##	"SDK"
##	"EP3"
##	"INFESTED"
##	"NIMBUS"
##	"SOB"
##	"PORTAL2"
##	"LOOP"
	"TF"
	"LEFT4DEAD2"   
	"EPISODIC"
	"VR"
	"HOTDOG"
	"TCG"
	"TCG2"
	"DAC"
	"CSGO"
	"HLVR"
	"CITADEL" 
	"BENDY"
	"MANN"
)

set(GAME "" CACHE STRING "Game to build")

if(NOT GAME)
    message(FATAL_ERROR "GAME not specified. Use -DGAME=<name>")
endif()

string(TOUPPER "${GAME}" GAME)

list(FIND Games "${GAME}" GAME_INDEX)

if(GAME_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid GAME: ${GAME}")
endif()

message(STATUS "Selected GAME: ${GAME}")

if(LINUX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -fPIC")
endif()

add_subdirectory(interfaces) # most important one
add_subdirectory(game/client)
add_subdirectory(game/server)
add_subdirectory(engine2)
add_subdirectory(launcher_main)
add_subdirectory(hammer) # hammer 3 confirmed
add_subdirectory(gameui)
add_subdirectory(vphysics2)

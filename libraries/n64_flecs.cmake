# Specify the C and C++ compilers
set(CMAKE_C_COMPILER ${N64_INST}/bin/mips64-elf-gcc)
set(CMAKE_CXX_COMPILER ${N64_INST}/bin/mips64-elf-g++)

# Specify the linker
set(CMAKE_LINKER ${N64_INST}/bin/mips64-elf-ld)

# Separate C and C++ flags
set(CMAKE_C_FLAGS   "-march=vr4300 -mtune=vr4300 -mips3 -I${N64_INST}/mips64-elf/include -DN64 -O2 -G0 -falign-functions=32 -ffunction-sections -fdata-sections -g -ffast-math -ftrapping-math -fno-associative-math -std=gnu99")
set(CMAKE_CXX_FLAGS "-march=vr4300 -mtune=vr4300 -mips3 -I${N64_INST}/mips64-elf/include -DN64 -O2 -G0 -falign-functions=32 -ffunction-sections -fdata-sections -g -ffast-math -ftrapping-math -fno-associative-math -std=c++17 -fno-exceptions -fno-rtti")

# Verify and adjust linker flags
# set(CMAKE_EXE_LINKER_FLAGS "-L${N64_INST}/mips64-elf/lib -ldragon -lm -ldragonsys -Tn64.ld")

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-Map,output.map")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${N64_INST}/mips64-elf/lib/ -lc -lm -T ${N64_INST}/mips64-elf/lib/n64.ld")


# Disable certain features not needed or supported on the N64
add_definitions(-D_DISABLE_THREADS -D_DISABLE_LIBSSP)

# Specify include and library directories
set(CMAKE_INCLUDE_PATH ${N64_INST}/include)
set(CMAKE_LIBRARY_PATH ${N64_INST}/lib)

# Specify the target architecture
set(CMAKE_SYSTEM_NAME Nintendo64)

# Set the default output paths for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories and link directories
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

# Additional options (set according to your project requirements)
option(FLECS_STATIC "Build static flecs lib" ON)
option(FLECS_SHARED "Build shared flecs lib" ON)
option(FLECS_PIC "Compile static flecs lib with position independent code (PIC)" ON)
option(FLECS_TESTS "Build flecs tests" OFF)

set(FLECS_STATIC ON)
set(FLECS_SHARED OFF)
set(FLECS_PIC    OFF)

# Configure flecs build
add_definitions(-DFLECS_CUSTOM_BUILD -DECS_TARGET_POSIX -DECS_TARGET_LINUX)

# modules to enable
# modules to disable
add_definitions(-DFLECS_CPP)
add_definitions(-DFLECS_MODULE)
add_definitions(-DFLECS_SYSTEM)
add_definitions(-DFLECS_PIPELINE)
add_definitions(-DFLECS_TIMER)
add_definitions(-DFLECS_META)
add_definitions(-DFLECS_NO_UNITS)
add_definitions(-DFLECS_NO_JSON)
add_definitions(-DFLECS_NO_DOC)
add_definitions(-DFLECS_NO_HTTP)
add_definitions(-DFLECS_NO_REST)
add_definitions(-DFLECS_NO_PARSER)
add_definitions(-DFLECS_NO_QUERY_DSL)
add_definitions(-DFLECS_NO_SCRIPT)
add_definitions(-DFLECS_NO_STATS)
add_definitions(-DFLECS_NO_METRICS)
add_definitions(-DFLECS_NO_ALERTS)
add_definitions(-DFLECS_NO_LOG)
add_definitions(-DFLECS_NO_JOURNAL)
add_definitions(-DFLECS_APP)
add_definitions(-DFLECS_NO_OS_API_IMPL)

# FLECS_CPP         "C++17 API"                                      
# FLECS_MODULE      "Organize game logic into reusable modules"      
# FLECS_SYSTEM      "Create & run systems"                           
# FLECS_PIPELINE    "Automatically schedule & multithread systems"   
# FLECS_TIMER       "Run systems at time intervals or at a rate"     
# FLECS_META        "Flecs reflection system"                        
# FLECS_UNITS       "Builtin unit types"                             
# FLECS_JSON        "JSON format"                                    
# FLECS_DOC         "Add documentation to components, systems & more"
# FLECS_HTTP        "Tiny HTTP server for processing simple requests"
# FLECS_REST        "REST API for showing entities in the browser"   
# FLECS_PARSER      "Parser utilities used by script & query DSL"    
# FLECS_QUERY_DSL   "Query DSL parser"                               
# FLECS_SCRIPT      "DSL for scenes, assets and configuration"       
# FLECS_STATS       "Functions for collecting statistics"            
# FLECS_METRICS     "Create metrics from user-defined components"    
# FLECS_ALERTS      "Create alerts from user-defined queries"        
# FLECS_LOG         "Extended tracing and error logging"             
# FLECS_JOURNAL     "Journaling of API functions"                    
# FLECS_APP         "Flecs application framework"                    
# FLECS_OS_API_IMPL "Default OS API implementation for Posix/Win32"  
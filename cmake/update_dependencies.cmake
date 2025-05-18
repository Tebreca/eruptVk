# This CMake script updates git dependencies using sparse-checkout
# To be run before the main CMake configuration

message(STATUS "Updating git dependencies...")

# Set paths relative to project root
set(DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies")
set(DEPS_CONFIG_FILE "${CMAKE_SOURCE_DIR}/cmake/dependencies.json")

# Read dependencies configuration
file(READ "${DEPS_CONFIG_FILE}" DEPS_CONFIG_CONTENT)
string(JSON DEPS_COUNT LENGTH "${DEPS_CONFIG_CONTENT}" "libraries")

# Process each dependency
math(EXPR DEPS_COUNT_MINUS_ONE "${DEPS_COUNT} - 1")
foreach(IDX RANGE ${DEPS_COUNT_MINUS_ONE})
    # Extract library information from JSON
    string(JSON LIB_NAME GET "${DEPS_CONFIG_CONTENT}" "libraries" ${IDX} "name")
    string(JSON LIB_URL GET "${DEPS_CONFIG_CONTENT}" "libraries" ${IDX} "url")
    string(JSON LIB_BRANCH GET "${DEPS_CONFIG_CONTENT}" "libraries" ${IDX} "branch")
    
    string(JSON SPARSE_PATHS_COUNT LENGTH "${DEPS_CONFIG_CONTENT}" "libraries" ${IDX} "sparse_paths")
    
    set(LIB_DIR "${DEPENDENCIES_DIR}/${LIB_NAME}")
    
    # Check if library directory exists
    if(NOT EXISTS "${LIB_DIR}/.git")
        # Clone repository with minimal setup
        message(STATUS "Initializing ${LIB_NAME} from ${LIB_URL}")
        file(MAKE_DIRECTORY "${LIB_DIR}")
        
        execute_process(
            COMMAND git init
            WORKING_DIRECTORY "${LIB_DIR}"
        )
        
        execute_process(
            COMMAND git remote add origin "${LIB_URL}"
            WORKING_DIRECTORY "${LIB_DIR}"
        )
        
        # Enable sparse checkout
        execute_process(
            COMMAND git config core.sparseCheckout true
            WORKING_DIRECTORY "${LIB_DIR}"
        )
        
        # Create sparse-checkout file
        file(MAKE_DIRECTORY "${LIB_DIR}/.git/info")
        set(SPARSE_CHECKOUT_FILE "${LIB_DIR}/.git/info/sparse-checkout")
        file(WRITE "${SPARSE_CHECKOUT_FILE}" "")
        
        # Add sparse paths
        math(EXPR SPARSE_PATHS_COUNT_MINUS_ONE "${SPARSE_PATHS_COUNT} - 1")
        foreach(PATH_IDX RANGE ${SPARSE_PATHS_COUNT_MINUS_ONE})
            string(JSON SPARSE_PATH GET "${DEPS_CONFIG_CONTENT}" "libraries" ${IDX} "sparse_paths" ${PATH_IDX})
            file(APPEND "${SPARSE_CHECKOUT_FILE}" "${SPARSE_PATH}\n")
        endforeach()
        
        # Pull the specified branch
        execute_process(
            COMMAND git pull origin "${LIB_BRANCH}"
            WORKING_DIRECTORY "${LIB_DIR}"
        )
    else()
        # Update existing repositories
        message(STATUS "Updating ${LIB_NAME} from ${LIB_URL}")
        
        # Fetch updates
        execute_process(
            COMMAND git fetch origin
            WORKING_DIRECTORY "${LIB_DIR}"
        )
        
        # Update to latest on branch
        execute_process(
            COMMAND git reset --hard "origin/${LIB_BRANCH}"
            WORKING_DIRECTORY "${LIB_DIR}"
        )
        
        # Update sparse-checkout if needed
        set(SPARSE_CHECKOUT_FILE "${LIB_DIR}/.git/info/sparse-checkout")
        file(WRITE "${SPARSE_CHECKOUT_FILE}" "")
        
        # Add sparse paths
        math(EXPR SPARSE_PATHS_COUNT_MINUS_ONE "${SPARSE_PATHS_COUNT} - 1")
        foreach(PATH_IDX RANGE ${SPARSE_PATHS_COUNT_MINUS_ONE})
            string(JSON SPARSE_PATH GET "${DEPS_CONFIG_CONTENT}" "libraries" ${IDX} "sparse_paths" ${PATH_IDX})
            file(APPEND "${SPARSE_CHECKOUT_FILE}" "${SPARSE_PATH}\n")
        endforeach()
        
        # Apply sparse-checkout
        execute_process(
            COMMAND git read-tree -mu HEAD
            WORKING_DIRECTORY "${LIB_DIR}"
        )
    endif()
endforeach()

message(STATUS "Git dependencies updated successfully")

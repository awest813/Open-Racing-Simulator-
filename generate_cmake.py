import os

base_dir = r"d:\Games\Open-Racing-Simulator-\src\libs"
root_cmake = r"d:\Games\Open-Racing-Simulator-\CMakeLists.txt"

cmake_dirs = []

for lib_name in os.listdir(base_dir):
    lib_path = os.path.join(base_dir, lib_name)
    if os.path.isdir(lib_path) and lib_name not in ["portability"]:
        # Find all cpp and c files
        sources = [f for f in os.listdir(lib_path) if f.endswith(".cpp") or f.endswith(".c")]
        if not sources:
            continue
        
        cmake_content = f"set({lib_name.upper()}_SOURCES\n"
        for src in sources:
            cmake_content += f"    {src}\n"
        cmake_content += ")\n\n"
        
        # All static, except perhaps some if they must be shared, but static is easier for linking
        cmake_content += f"add_library({lib_name} STATIC ${{{lib_name.upper()}_SOURCES}})\n\n"
        
        cmake_content += f"target_include_directories({lib_name} PUBLIC \n"
        cmake_content += f"    ${{CMAKE_CURRENT_SOURCE_DIR}}\n"
        cmake_content += f"    ${{PROJECT_SOURCE_DIR}}/src/interfaces\n"
        cmake_content += f"    ${{PROJECT_SOURCE_DIR}}/src/libs\n"
        cmake_content += ")\n"
        
        # Exception for tgf which was already handled but we might overwrite, let's just make it if it doesnt exist
        cmake_path = os.path.join(lib_path, "CMakeLists.txt")
        if not os.path.exists(cmake_path):
            with open(cmake_path, "w") as f:
                f.write(cmake_content)
            print(f"Created {cmake_path}")
        
        cmake_dirs.append(lib_name)

print("Libraries found:", cmake_dirs)

# We should also generate for src/modules and src/windows etc, but let's see how libs compile first.

build_dir := "build"
bin_dir   := "build/bin"
bench_dir := "build_bench"

# show these info 
default:
	@just --list

setup compiler="" type="Debug":
    #!/usr/bin/env bash
    set -e
    c_val="{{compiler}}"
    if [ -n "$c_val" ]; then
        if [ "$c_val" = "clang" ]; then
            c_comp="clang"; cxx_comp="clang++"
        elif [ "$c_val" = "gcc" ]; then
            c_comp="gcc"; cxx_comp="g++"
        else
            c_comp="$c_val"; cxx_comp="$c_val"
        fi
    fi
    current_type_file="{{build_dir}}/.build_type"
    last_type=""
    [ -f "$current_type_file" ] && last_type=$(cat "$current_type_file")

    if [ ! -d "{{build_dir}}" ] || [ -n "$c_val" ] || [ "{{type}}" != "$last_type" ]; then
        echo "--- Configuring CMake (Type: {{type}}) ---"
        args=(-B "{{build_dir}}" -DCMAKE_BUILD_TYPE={{type}})
        [ -n "$c_comp" ] && args+=("-DCMAKE_C_COMPILER=$c_comp")
        [ -n "$cxx_comp" ] && args+=("-DCMAKE_CXX_COMPILER=$cxx_comp")
        
        cmake "${args[@]}"
        mkdir -p "{{build_dir}}"
        echo "{{type}}" > "$current_type_file"
    else
        echo "Build directory exists with same config. Skipping reconfiguration."
    fi

build:
    #!/usr/bin/env bash
    set -e
    if [ ! -d "{{build_dir}}" ]; then
        echo "Error: Build directory not found. Please run 'just setup' first."
        exit 1
    fi
    threads=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    cmake --build {{build_dir}} -j "$threads"

run:
    #!/usr/bin/env bash
    just build

    selected_path=$(find {{bin_dir}} -executable -type f | fzf --prompt="Select target > ")

    if [ -n "$selected_path" ]; then
        target=$(basename "$selected_path")
        echo "--- Building $target ---"
        cmake --build {{build_dir}} --target "$target"
        echo "--- Running $target ---"
        "$selected_path"
    fi

# --- 测试与性能 ---
test :
	#!/usr/bin/env bash
	just build
	cd {{build_dir}} && ctest --output-on-failure

bench compiler="" asan_toggle="no-asan":
    #!/usr/bin/env bash
    set -e
    
    if [ "{{asan_toggle}}" = "asan" ]; then
        asan_flag="ON"
        mode_msg="ASan: ON (Debug Mode)"
    else
        asan_flag="OFF"
        mode_msg="ASan: OFF (Performance Mode)"
    fi

    c_val="{{compiler}}"
    if [ ! -d "{{bench_dir}}" ] || [ -n "$c_val" ] || [ -n "{{asan_toggle}}" ]; then
        echo "--- Configuring Benchmark Build ($mode_msg) ---"
        args=(-B "{{bench_dir}}" \
          -DCMAKE_BUILD_TYPE=Release \
          -DUSE_ASAN=$asan_flag \
          -DBUILD_BENCHMARKS=ON \
          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
        
        if [ "$c_val" = "clang" ]; then
            args+=("-DCMAKE_C_COMPILER=clang" "-DCMAKE_CXX_COMPILER=clang++")
        elif [ "$c_val" = "gcc" ]; then
            args+=("-DCMAKE_C_COMPILER=gcc" "-DCMAKE_CXX_COMPILER=g++")
        fi
        cmake "${args[@]}"
    fi

    # 交互式选择源码
    selected_cpp=$(find benchmarks -name "bench_*.cpp" ! -name "bench_main.cpp" | fzf --prompt="Select Benchmark Source ($mode_msg) > ")

    if [ -n "$selected_cpp" ]; then
        target=$(basename "$selected_cpp" .cpp)
        threads=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

        echo "--- Building $target ---"
        cmake --build {{bench_dir}} --target "$target" -j "$threads"

        echo "------------------------------------------------"
        echo "Running: $target"
        ./{{bench_dir}}/bin/benchmarks/"$target" --benchmark-samples 100
    else
        echo "No target selected."
    fi
clean:
    #!/usr/bin/env bash
    rm -rf {{build_dir}}
    rm -rf {{bench_dir}}
    echo "Cleanup complete: '{{build_dir}}' and '{{bench_dir}}' removed."

stats:
    #!/usr/bin/env bash
    set -e
    echo "================ Code Statistics Overview ================"
    
    # include 中的各个子模块 (Header files)
    echo "(include/):"
    for dir in include/*/; do
        module=$(basename "$dir")
        count=$(find "$dir" -type f \( -name "*.hpp" -o -name "*.h" \) | wc -l)
        printf "  %-12s: %2d files\n" "$module" "$count"
    done

    # src 中的实现文件 (Source files)
    echo -e "\n(src/):"
    for dir in src/*/; do
        module=$(basename "$dir")
        count=$(find "$dir" -type f \( -name "*.cpp" -o -name "*.c" \) | wc -l)
        printf "  %-12s: %2d files\n" "$module" "$count"
    done

    # 统计应用层代码
    echo -e "\nOther files:"
    examples_count=$(find examples -type f \( -name "*.cpp" -o -name "*.c" \) | wc -l)
    tests_count=$(find tests -type f \( -name "*.cpp" -o -name "*.c" \) | wc -l)
    
    printf "  %-12s: %2d files\n" "Examples" "$examples_count"
    printf "  %-12s: %2d files\n" "Tests" "$tests_count"
    
    # 总计行数统计
    if command -v cloc >/dev/null 2>&1; then
        echo -e "\n========= Lines of code =========:"
        cloc include src examples tests --quiet --not-match-d=third_party
    fi

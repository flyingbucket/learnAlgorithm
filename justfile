# 变量定义
build_dir := "build"
bin_dir   := "build/bin"

# 默认指令：显示所有可用指令
default:
    @just --list

# ------------------------------------------------------------------------------
# 基础构建指令
# ------------------------------------------------------------------------------

# 配置 CMake (生成 build 目录)
setup:
    cmake -B {{build_dir}} -DCMAKE_BUILD_TYPE=RelWithDebInfo

# 编译所有目标 (Examples 和 Tests)
build: setup
    cmake --build {{build_dir}} -j $(nproc)

# 清理门户
clean:
    rm -rf {{build_dir}}
    @echo "Build directory removed."

# ------------------------------------------------------------------------------
# 运行与调试 (类 Cargo 体验)
# ------------------------------------------------------------------------------

# 精准打击：编译并运行特定的 target (如: just p test_DLinkList)
p target:
    @cmake --build {{build_dir}} --target {{target}}
    @echo "----------------------------------------"
    @./{{bin_dir}}/{{target}}

# 交互式起飞：列出所有可执行文件并选择运行 (需要安装 fzf)
run:
    @if [ ! -d "{{bin_dir}}" ]; then just build; fi
    @target=$(ls {{bin_dir}} | fzf --prompt="🚀 Select Target > ") && \
    cmake --build {{build_dir}} --target $target && \
    ./{{bin_dir}}/$target

# ------------------------------------------------------------------------------
# 测试与性能
# ------------------------------------------------------------------------------

# 运行所有自动化测试 (CTest)
test: build
    cd {{build_dir}} && ctest --output-on-failure

# 专门跑 Benchmark：只运行带有 [benchmark] 标签的测试用例
# 用法: just bench <target_name> (如: just bench test_DLinkList)
bench target:
    @cmake --build {{build_dir}} --target {{target}}
    @echo "📊 Running Benchmarks for {{target}}..."
    @./{{bin_dir}}/{{target}} "[benchmark]" --benchmark-samples 100

# ------------------------------------------------------------------------------
# 辅助功能
# ------------------------------------------------------------------------------

# 快速检查代码统计
stats:
    @echo "HPP files: $(find include -name "*.hpp" | wc -l)"
    @echo "CPP files: $(find examples tests -name "*.cpp" | wc -l)"

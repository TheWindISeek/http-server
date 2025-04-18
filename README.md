# Simple HTTP/1.1 server

Implementation of a simple HTTP server in C++

## Features

- Can handle multiple concurrent connections, tested up to 10k.
- Support basic HTTP request and response. Provide an extensible framework to implement other HTTP features.
- HTTP/1.1: Persistent connection is enabled by default.

## Quick start

```bash
mkdir build && cd build
cmake ..
make
./test_SimpleHttpServer # Run unit tests
./SimpleHttpServer      # Start the HTTP server on port 8080
```

- There are two endpoints available at `/` and `/hello.html` which are created for demo purpose.
- In order to have multiple concurrent connections, make sure to raise the resource limit (with `ulimit`) before running the server. A non-root user by default can have about 1000 file descriptors opened, which corresponds to 1000 active clients.

## Design

The server program consists of:

- 1 main thread for user interaction.
- 1 listener thread to accept incoming clients.
- 5 worker threads to process HTTP requests and sends response back to client.
- Utility functions to parse and manipulate HTTP requests and repsonses conveniently.

## Benchmark

I used a tool called [wrk](https://github.com/wg/wrk) to benchmark this HTTP server. The tests were performed on my laptop with the following specs:

```bash
Model: Thinkpad T480
OS: Ubuntu 18.04 TLS x84_64
Kernel: 4.18.0-24-generic
CPU: Intel i7-8550 (8) @ 4.000 GHz
GPU: Intel UHD Graphics 620
Memory: 6010 MiB / 15803 MiB
```

Here are the results for two test runs. Each test ran for 1 minute, with 10 client threads. The first test had only 500 concurrent connections, while the second test had 10000.

```bash
$ ./wrk -t10 -c500 -d60s http://0.0.0.0:8080/
Running 1m test @ http://0.0.0.0:8080/
  10 threads and 500 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     5.01ms    1.31ms  57.86ms   86.35%
    Req/Sec     9.94k     0.99k   36.28k    76.69%
  5933266 requests in 1.00m, 441.36MB read
Requests/sec:  98760.82
Transfer/sec:      7.35MB
```

```bash
$ ./wrk -t10 -c10000 -d60s http://0.0.0.0:8080/
Running 1m test @ http://0.0.0.0:8080/
  10 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   111.78ms   21.38ms 403.80ms   76.79%
    Req/Sec     8.73k     1.42k   18.77k    75.62%
  5174508 requests in 1.00m, 384.91MB read
Requests/sec:  86123.84
Transfer/sec:      6.41MB

```

# how to use joern
```shell
/opt/joern/joern-cli/c2cpg.sh -J-Xmx3956m /home/lucifer/Codes/test/http-server/src --output /home/lucifer/Codes/test/http-server/workspace/http-server/cpg.bin.zip
/opt/joern/joern-cli/joern --script analyze_main.sc
dot -Tpng main_cfg.dot -o main_cfg.png && dot -Tpng main_pdg.dot -o main_pdg.png
```

# contact with claude3.5
根据错误信息，这是一个Java版本不兼容的问题。错误显示您正在使用的Java运行时环境只支持到class文件版本55.0，但是joern需要更新的Java版本（class文件版本61.0）。

让我们来检查您当前的Java版本并安装所需的版本。

```bash
java -version
```
您当前使用的是Java 11，而Joern需要Java 17或更高版本。让我们安装OpenJDK 17：

```bash
sudo apt update && sudo apt install openjdk-17-jdk -y
```
现在让我们将系统默认的Java版本切换到Java 17：

```bash
sudo update-alternatives --set java /usr/lib/jvm/java-17-openjdk-amd64/bin/java
```
让我们验证Java版本是否已经更新：

```bash
java -version
```
现在Java 17已经成功安装并设置为默认版本。让我们重新运行joern命令：

```bash
/opt/joern/joern-cli/c2cpg.sh -J-Xmx3956m /home/lucifer/Codes/test/http-server/src --output /home/lucifer/Codes/test/http-server/workspace/http-server/cpg.bin.zip
```
命令似乎已经成功执行。让我们检查一下输出文件是否已经生成：

```bash
ls -l workspace/http-server/cpg.bin.zip
```
太好了！问题已经解决。让我总结一下我们做了什么：

1. 发现原始错误是由于Java版本不兼容导致的（需要Java 17，但系统使用的是Java 11）
2. 安装了OpenJDK 17
3. 将系统默认Java版本切换到Java 17
4. 重新运行joern命令
5. 验证输出文件已经成功生成

现在您可以继续使用生成的`cpg.bin.zip`文件进行后续的分析工作了。如果您需要进一步的帮助，请随时告诉我。

我来帮您使用joern分析main函数的AST图。我们需要使用joern-cli工具来进行分析。

首先，让我们启动joern交互式shell：

```bash
/opt/joern/joern-cli/joern --script-mode
```
让我们创建一个joern脚本来分析main函数的AST。我们需要创建一个脚本文件：

```plain text:analyze_main.sc
@main def main() = {
    importCpg("/home/lucifer/Codes/test/http-server/workspace/http-server/cpg.bin.zip")
    
    println("Analyzing main function AST...")
    
    // 查找main函数
    val mainMethod = cpg.method.name("main").l
    
    // 打印main函数的AST
    mainMethod.foreach { method =>
        println(s"AST for method: ${method.name}")
        method.ast.foreach { node =>
            println(s"${"\t" * node.depth}${node.label}: ${node.code}")
        }
    }
    
    // 导出AST为DOT格式
    val dotAst = mainMethod.head.dotAst
    os.write(os.pwd / "main_ast.dot", dotAst)
    println("AST has been exported to main_ast.dot")
}
```
现在让我们运行这个脚本：

```bash
/opt/joern/joern-cli/joern --script analyze_main.sc
```
我们已经成功生成了main函数的AST（抽象语法树）。AST已经被导出到`main_ast.dot`文件中。让我们将这个dot文件转换为可视化的图片格式：

```bash
dot -Tpng main_ast.dot -o main_ast.png
```
现在我们来分析一下main函数的AST结构：

1. 主要结构：
   - 函数声明：`METHOD: int main(void)`
   - 参数：`METHOD_PARAMETER_IN: void`
   - 函数体：一个大的`BLOCK`节点

2. 主要变量声明：
   - `std::string host = "0.0.0.0"`
   - `int port = 8080`
   - `const std::string default_page = "/main.html"`
   - `HttpServer server(host, port)`

3. 主要控制流程：
   - try-catch 块
   - while 循环：`while (std::cin >> command, command != "quit")`
   - 条件语句和异常处理

4. 主要操作：
   - 服务器初始化和配置
   - 服务器启动：`server.Start()`
   - 命令行交互
   - 服务器停止：`server.Stop()`

5. 错误处理：
   - catch 块捕获 `std::exception`
   - 错误输出和返回值处理

AST图已经生成为`main_ast.png`文件，您可以在文件系统中查看完整的可视化图形。这个图会显示代码的层次结构，包括所有的节点和它们之间的关系。

如果您想了解更具体的某个部分，或者想分析其他函数的AST，请告诉我。


我来帮您生成main函数的控制流图(CFG)和程序依赖图(PDG)。我们需要修改joern脚本：

```plain text:analyze_main.sc
@main def main() = {
    importCpg("/home/lucifer/Codes/test/http-server/workspace/http-server/cpg.bin.zip")
    
    println("Analyzing main function graphs...")
    
    // 查找main函数
    val mainMethod = cpg.method.name("main").l
    
    // 导出CFG为DOT格式
    println("Generating CFG...")
    val cfgDot = mainMethod.head.dotCfg
    os.write(os.pwd / "main_cfg.dot", cfgDot)
    println("CFG has been exported to main_cfg.dot")
    
    // 导出PDG为DOT格式
    println("Generating PDG...")
    val pdgDot = mainMethod.head.dotPdg
    os.write(os.pwd / "main_pdg.dot", pdgDot)
    println("PDG has been exported to main_pdg.dot")
}
```
现在让我们运行修改后的脚本：

```bash
/opt/joern/joern-cli/joern --script analyze_main.sc
```
让我们将生成的dot文件转换为PNG格式以便查看：

```bash
dot -Tpng main_cfg.dot -o main_cfg.png && dot -Tpng main_pdg.dot -o main_pdg.png
```
现在我们已经生成了main函数的三种图：

1. 控制流图(CFG) - `main_cfg.png`：
   - 显示了程序的执行流程
   - 包含了条件分支、循环等控制结构
   - 可以看到从程序入口到退出的所有可能路径

2. 程序依赖图(PDG) - `main_pdg.png`：
   - 显示了程序中数据和控制依赖关系
   - 包含了变量之间的数据流
   - 展示了不同语句之间的依赖关系

main函数的主要控制流程如下：

1. 初始化阶段：
   - 设置主机地址(`host = "0.0.0.0"`)
   - 设置端口号(`port = 8080`)
   - 设置默认页面(`default_page = "/main.html"`)
   - 创建HTTP服务器实例(`HttpServer server(host, port)`)

2. 服务器运行阶段：
   - 启动服务器(`server.Start()`)
   - 进入命令监听循环
   - 等待"quit"命令

3. 退出阶段：
   - 停止服务器(`server.Stop()`)
   - 程序结束

4. 错误处理：
   - try-catch块处理可能的异常
   - 如果发生异常，返回-1

这些图可以帮助我们理解代码的：
- 执行顺序
- 数据流动
- 依赖关系
- 可能的执行路径
- 错误处理流程

您可以在生成的PNG文件中查看完整的图形表示。如果您想了解某个特定部分的更多细节，请告诉我。

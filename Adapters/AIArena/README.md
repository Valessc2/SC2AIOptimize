# AI Arena adapter matrix

SC2AIOptimize keeps one neutral core. AI Arena language/framework integration lives at the edge and must not introduce bot/framework types into Registry, Kernel or Tuner.

## Supported edges

| AI Arena consumer | SC2AIOptimize edge | Core dependency |
|---|---|---|
| C++ / cpp-sc2 / CommandCenter-style bots | Direct generic C++ headers | none beyond C++20/STL |
| Python / python-sc2 / Sharpy / PySC2-style bots | `Adapters/Python` pybind11 buffer adapter | pybind11 at binding build only |
| C | optional `sc2opt_c` stable C ABI | none |
| Rust | thin `extern "C"` wrapper over `sc2opt_c` | none in core |
| Go | thin cgo wrapper over `sc2opt_c` | none in core |
| .NET | thin P/Invoke wrapper over `sc2opt_c` | none in core |
| Java 17 | thin JNI module over `sc2opt_c`, using direct buffers | JDK/JNI only when that edge is built |
| Node.js | thin N-API module over `sc2opt_c`, using typed arrays | Node headers only when that edge is built |

## Runtime law

Adapters translate. They do not own gameplay policy, tuning semantics or SC2 framework state. Consumers pack their own objects however is cheapest and should prefer coarse batched calls.

The C ABI is deliberately tiny, versioned, optional and caller-buffer based. It is **not linked by `sc2opt::sc2opt`**. New non-C++ language edges should bind that ABI where practical rather than duplicate Kernel logic.

## Direct C++ consumers

Native C++ bots should use the generic C++ surface directly. They do not build or pay for the C ABI, Python, JNI, N-API or any other language edge unless explicitly requested.

In particular, MMEvo remains a normal generic C++ consumer: its SC2API/MME types are translated on the MMEvo side into SC2AIOptimize neutral spans/views. There is no MMEvo-specific adapter in this repository.
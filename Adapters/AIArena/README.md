# AI Arena adapter matrix

SC2AIOptimize keeps one neutral core. AI Arena language/framework integration lives at the edge and must not introduce bot/framework types into Registry, Kernel or Tuner.

## Supported edges

| AI Arena consumer | SC2AIOptimize edge | Core dependency |
|---|---|---|
| C++ / cpp-sc2 / CommandCenter-style bots | Direct generic C++ headers | none beyond C++20/STL |
| Python / python-sc2 / Sharpy / PySC2-style bots | `Adapters/Python` pybind11 buffer adapter | pybind11 at binding build only |
| C | `sc2opt_c` stable C ABI | none |
| Rust | thin `extern "C"` wrapper over `sc2opt_c` | none in SC2AIOptimize core |
| Go | thin cgo wrapper over `sc2opt_c` | none in SC2AIOptimize core |
| .NET | thin P/Invoke wrapper over `sc2opt_c` | none in SC2AIOptimize core |
| Java 17 | JNI shim over `sc2opt_c` | JDK/JNI only when building Java shim |
| Node.js | N-API shim over `sc2opt_c` | Node headers only when building Node shim |

## Law

Adapters translate. They do not own gameplay policy, tuning semantics or SC2 framework state. Consumers may pack their own objects however is cheapest, and should prefer coarse batched calls.

The C ABI is deliberately tiny and versioned. New language edges should bind that ABI where practical rather than duplicate Kernel logic.

## MMEvo

MMEvo should use the direct generic C++ surface, not the C ABI. Its SC2API/MME types are translated on the MMEvo side into SC2AIOptimize neutral spans/views. That preserves the cheapest possible native path.

# Generic Python adapter

The Python adapter will bind the same neutral packed contracts used by the C++ interface. It must not depend on `python-sc2` or any specific bot.

Consumer responsibilities:

- translate framework objects into SC2AIOptimize packed/native views;
- batch crossings so Python/native transition overhead is amortised;
- map consumer unit/type identifiers to `SC2Registry` during startup/validation;
- keep policy/final decisions in the consuming bot unless a generic Kernel operation explicitly owns the computation.

`pybind11` is the intended optional binding mechanism unless measurement later proves a different boundary materially better. It is not a core/runtime dependency when Python support is disabled.

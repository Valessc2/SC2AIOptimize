# Generic Python adapter

The Python adapter will expose coarse, batched operations over SC2AIOptimize's neutral contracts.

It will **not** depend on python-sc2. A Python consumer translates framework objects into packed arrays/views at its own boundary.

Target crossing:

```text
Python consumer
  -> packed neutral snapshot / arrays
  -> one coarse native call
  -> compact result
  -> Python policy
```

`pybind11` is the current likely binding mechanism, but it is intentionally not a mandatory dependency until the binding slice is implemented and measured.

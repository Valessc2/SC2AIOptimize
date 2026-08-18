# Kernel

Framework-independent compute lives here. `Hot` is reserved for tiny, repeatedly-called operations that have profiling evidence and an equivalence-tested baseline. Larger native jobs belong in Compute only when the larger boundary produces positive measured net benefit.

Hot admission is earned, not guessed.

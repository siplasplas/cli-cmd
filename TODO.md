# TODO

## Parser Core

- [ ] Support **compact flag groups**:
    - Parsing and recognition of `-a -v` as `-av`
    - Ensure correctness of order-insensitive grouping: `-va`, `-av`, etc.
    - Prevent collisions with flags that expect values

## Parameter Handling

- [ ] Finalize support for parameters:
    - `addParameter`, `addReqParameter`, `addDefParameter`
    - Integration with `--help`, JSON output, and error codes
    - Command-local and app-global parameter shadowing
- [ ] Support `--param=value` and `--param value`
- [x] Consistent behavior for required vs optional vs defaulted parameters
- [ ] Support override global options

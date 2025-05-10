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
- [ ] Consistent behavior for required vs optional vs defaulted parameters

## Validation System

- [ ] Implement built-in validators: `string`, `url`, `path`, `auto-path`, etc.
- [ ] Enable chaining and composing validators
- [ ] Allow custom user-defined validators with registration
- [ ] Cycle detection during validator dependency resolution
- [ ] JSON-compatible validator descriptions for documentation/help

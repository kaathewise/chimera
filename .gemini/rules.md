## Sync Policy

The user changes the files inbetween requests to GEMINI, so don't assume their contents stay the same.
Before using any file-editing tool (write_file, replace), you MUST first use read_file on the target file to ensure your context matches the current disk state.

## After each C++ change

1. Run `cpplint --recursive --exclude=third_party .` to check for lint errors.

2. Use `clang-format` to format the changed files.

3. Always check that the changed module builds.

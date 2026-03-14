## Sync Policy

Before using any file-editing tool (write_file, replace), you MUST first use read_file on the target file to ensure your context matches the current disk state. Do not assume your cached memory of the file is up to date.
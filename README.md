# HolyPackager

A packaging tool for HolyC code and resources that creates self-extracting packages compatible with TempleOS.

## Overview

HolyPackager allows you to bundle HolyC source code along with resources (files, images, data) into a single package file (`.hcpk`). These packages can be extracted standalone or turned into self-extracting executables for easy distribution.

## Features

- **Package HolyC Code**: Bundle HolyC source files with any number of resource files
- **Self-Extracting Packages**: Create executable files that extract themselves when run
- **Cross-Platform**: Works on Windows, Linux, and other Unix-like systems
- **Efficient Format**: Binary format with magic headers and metadata
- **Fast Operations**: Timing information for all operations
- **TempleOS Compatible**: Extracted HolyC code ready for use in TempleOS

## Requirements

- C compiler (GCC, Clang, MSVC)
- Standard C library
- POSIX environment (for Unix-like systems)

## Building

### Linux/Unix/macOS

```bash
# Clone the repository
git clone https://github.com/TheusHen/HolyPackager.git
cd HolyPackager

# Compile the packager
gcc -o packager packager.c

# Compile the self-extracting stub
gcc -o hcpk_stub hcpk_stub.c
```

### Windows

```cmd
# Using MSVC
cl packager.c /Fe:packager.exe
cl hcpk_stub.c /Fe:hcpk_stub.exe

# Using MinGW
gcc -o packager.exe packager.c
gcc -o hcpk_stub.exe hcpk_stub.c
```

## Usage

### Creating a Package

```bash
./packager code.hc output.hcpk [resources...]
```

**Parameters:**
- `code.hc` - Path to your HolyC source file
- `output.hcpk` - Output package filename
- `[resources...]` - Optional list of resource files to include

**Examples:**

```bash
# Package just HolyC code
./packager hello.hc hello.hcpk

# Package HolyC code with resources
./packager game.hc game.hcpk sprites.bmp sounds.wav data.txt

# Package with multiple resources
./packager app.hc myapp.hcpk images/logo.png docs/readme.txt config.ini
```

### Extracting a Package

You can extract packages in two ways:

#### Method 1: Direct Extraction (if you have an extractor)

```bash
# Note: This requires implementing an extractor tool
# The current codebase includes the extraction logic in hcpk_stub.c
```

#### Method 2: Self-Extracting Executable

Create a self-extracting executable:

```bash
# Create the marker file
echo -n "HCPK_EOF:" > marker.tmp

# Combine stub + marker + package into executable
cat hcpk_stub marker.tmp mypackage.hcpk > mypackage_extractor

# Make it executable (Unix-like systems)
chmod +x mypackage_extractor

# Run to extract
./mypackage_extractor [output_directory]
```

**Windows:**
```cmd
copy /b hcpk_stub.exe + marker.tmp + mypackage.hcpk mypackage_extractor.exe
mypackage_extractor.exe [output_directory]
```

### Extraction Output

When extracted, packages create:
- `extracted.hc` - The original HolyC source code
- Original resource files with their paths preserved

### Example Workflow

Here's a complete example:

```bash
# 1. Create a simple HolyC program
cat > hello.hc << 'EOF'
U0 Main()
{
    "Hello from HolyC!\n";
    "Package extracted successfully!\n";
}
EOF

# 2. Create some resources
echo "Configuration data" > config.txt
echo "Hello World" > message.txt

# 3. Build the tools
gcc -o packager packager.c
gcc -o hcpk_stub hcpk_stub.c

# 4. Create package
./packager hello.hc hello.hcpk config.txt message.txt

# 5. Create self-extracting executable
echo -n "HCPK_EOF:" > marker.tmp
cat hcpk_stub marker.tmp hello.hcpk > hello_extractor
chmod +x hello_extractor

# 6. Extract package
./hello_extractor my_extracted_files

# 7. Check results
ls my_extracted_files/
cat my_extracted_files/extracted.hc
```

## File Format Specification

The `.hcpk` file format is a binary format with the following structure:

```
Header:
- Magic: "HCPK" (4 bytes)
- Version: uint32_t (4 bytes) 
- HolyC Size: uint32_t (4 bytes)
- HolyC Code: variable length (HolyC Size bytes)

Resources Section:
- Resource Count: uint32_t (4 bytes)
- For each resource:
  - Name Length: uint32_t (4 bytes)
  - Name: variable length (Name Length bytes)
  - Data Size: uint32_t (4 bytes)  
  - Data: variable length (Data Size bytes)
```

**Notes:**
- All multi-byte integers are stored in little-endian format
- Resource names include the original path/filename
- Maximum practical limits depend on available memory

## Self-Extracting Format

Self-extracting executables use this structure:

```
[Stub Executable] + "HCPK_EOF:" + [HCPK Package Data]
```

The stub scans the last 64KB of the file for the "HCPK_EOF:" marker to locate the package data.

## Compatibility

- **Source**: Works with any HolyC source files
- **Resources**: Any file type can be included as resources
- **Platforms**: Cross-platform C code (tested on Linux, should work on Windows/macOS)
- **TempleOS**: Extracted HolyC code is ready for use in TempleOS

## Error Handling

The tools provide clear error messages for:
- File not found errors
- Memory allocation failures  
- Invalid package formats
- Write permission issues

## Performance

- Package creation is very fast (typically under 1 second)
- Extraction is optimized for speed
- Memory usage scales with largest individual file
- File I/O is performed in large chunks for efficiency

## Limitations

- The stub extractor cannot execute HolyC code natively (TempleOS required)
- Resource file paths are stored as-is (subdirectory creation is basic)
- No compression (packages store files as-is)
- No encryption or security features

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for details on how to contribute to this project.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Created by TheusHen (I'm Matheus)

## See Also

- [TempleOS](https://templeos.org/) - The operating system HolyC was designed for
- [HolyC Documentation](https://templeos.org/Wb/Doc/HolyC.html) - Learn more about the HolyC language
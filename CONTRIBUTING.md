# Contributing to HolyPackager

Thank you for your interest in contributing to HolyPackager! This document provides guidelines and information for contributors.

## Table of Contents

- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Pull Request Process](#pull-request-process)
- [Issue Reporting](#issue-reporting)
- [Code of Conduct](#code-of-conduct)

## Getting Started

### Prerequisites

Before contributing, make sure you have:

- A C compiler (GCC, Clang, or MSVC)
- Git for version control
- A text editor or IDE
- Basic knowledge of C programming
- Understanding of file I/O and binary formats (helpful but not required)

### Fork and Clone

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/HolyPackager.git
   cd HolyPackager
   ```
3. Add the upstream repository:
   ```bash
   git remote add upstream https://github.com/TheusHen/HolyPackager.git
   ```

## Development Setup

### Building the Project

```bash
# Compile packager
gcc -o packager packager.c

# Compile stub extractor  
gcc -o hcpk_stub hcpk_stub.c

# For debugging, add debug flags
gcc -g -Wall -Wextra -o packager packager.c
gcc -g -Wall -Wextra -o hcpk_stub hcpk_stub.c
```

### Testing Your Build

```bash
# Create a test HolyC file
cat > test.hc << 'EOF'
U0 Main()
{
    "Hello from test!\n";
}
EOF

# Create a test resource
echo "Test resource content" > test_resource.txt

# Test packaging
./packager test.hc test.hcpk test_resource.txt

# Test self-extraction
echo -n "HCPK_EOF:" > marker.tmp
cat hcpk_stub marker.tmp test.hcpk > test_extractor
chmod +x test_extractor
./test_extractor test_output

# Verify extraction worked
ls test_output/
cat test_output/extracted.hc
```

## Coding Standards

### C Code Style

- **Indentation**: 4 spaces (no tabs)
- **Braces**: Opening brace on same line for functions, new line for control structures
- **Naming**: 
  - Functions: `snake_case`
  - Variables: `snake_case`
  - Constants: `UPPER_CASE`
  - Structs: `PascalCase`

### Example Style

```c
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILENAME 256

typedef struct {
    char *name;
    uint32_t size;
    unsigned char *data;
} Resource;

unsigned char* read_file(const char* filename, uint32_t* out_size) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Error opening file");
        return NULL;
    }
    
    // File reading logic here
    fclose(f);
    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    
    // Main logic here
    return 0;
}
```

### Code Quality

- **Error Handling**: Always check return values from system calls
- **Memory Management**: Free all allocated memory, check for allocation failures
- **Input Validation**: Validate all user inputs and file data
- **Comments**: Document complex logic and file format details
- **Portability**: Use portable C code, avoid platform-specific features when possible

### Platform Considerations

When adding platform-specific code:

```c
#ifdef _WIN32
    // Windows-specific code
    #include <direct.h>
    #define mkdir_p(p) _mkdir(p)
#else
    // Unix-like systems
    #include <sys/stat.h>
    #define mkdir_p(p) mkdir(p, 0755)
#endif
```

## Testing

### Manual Testing

Always test your changes with:

1. **Basic functionality**:
   - Package creation with various file sizes
   - Package extraction
   - Self-extracting executable creation

2. **Edge cases**:
   - Empty files
   - Large files (>1MB)
   - Files with special characters in names
   - Missing input files
   - Invalid package files

3. **Error conditions**:
   - Insufficient disk space
   - Permission denied scenarios
   - Corrupted package files
   - Invalid command line arguments

### Test Cases to Verify

```bash
# Test 1: Basic packaging
./packager example/test.HC basic.hcpk

# Test 2: Multiple resources
./packager example/test.HC multi.hcpk file1.txt file2.txt file3.txt

# Test 3: Large files (create a large test file)
dd if=/dev/zero of=large.dat bs=1M count=10
./packager example/test.HC large.hcpk large.dat

# Test 4: Self-extraction
echo -n "HCPK_EOF:" > marker.tmp
cat hcpk_stub marker.tmp basic.hcpk > basic_extractor
chmod +x basic_extractor
./basic_extractor extracted_basic

# Test 5: Error handling
./packager nonexistent.hc test.hcpk  # Should fail gracefully
./packager example/test.HC /root/denied.hcpk  # Should handle permission errors
```

### Cross-Platform Testing

If possible, test on:
- Linux (GCC)
- macOS (Clang)  
- Windows (MSVC or MinGW)

## Pull Request Process

### Before Submitting

1. **Update your fork**:
   ```bash
   git fetch upstream
   git checkout main
   git merge upstream/main
   ```

2. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make your changes** following the coding standards

4. **Test thoroughly** on your platform

5. **Commit with clear messages**:
   ```bash
   git add .
   git commit -m "Add feature: brief description of what you added"
   ```

### Pull Request Guidelines

- **Title**: Clear, concise description of the change
- **Description**: Explain what you changed and why
- **Testing**: Describe how you tested the changes
- **Breaking Changes**: Clearly mark any breaking changes

### PR Template

```markdown
## Description
Brief description of changes made.

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Code refactoring

## Testing
- [ ] Tested on Linux
- [ ] Tested on Windows
- [ ] Tested on macOS
- [ ] Added test cases
- [ ] Verified existing functionality still works

## Checklist
- [ ] Code follows project style guidelines
- [ ] Code is well-commented
- [ ] No memory leaks introduced
- [ ] Error handling is appropriate
- [ ] Documentation updated if needed
```

## Issue Reporting

### Bug Reports

When reporting bugs, include:

- **Environment**: OS, compiler, version
- **Steps to reproduce**: Exact commands used
- **Expected behavior**: What should happen
- **Actual behavior**: What actually happened
- **Files**: Sample files that reproduce the issue (if applicable)
- **Error messages**: Complete error output

### Feature Requests

For new features:

- **Use case**: Describe why this feature would be useful
- **Proposed solution**: How you think it should work
- **Alternatives**: Other ways to solve the same problem
- **Impact**: How this affects existing functionality

### Security Issues

For security-related issues:
- **Do not** open a public issue
- Email the maintainer directly
- Provide detailed steps to reproduce
- Wait for acknowledgment before public disclosure

## Code of Conduct

### Our Standards

- Use welcoming and inclusive language
- Be respectful of differing viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community
- Show empathy towards other community members

### Unacceptable Behavior

- Harassment of any kind
- Discriminatory language or behavior
- Personal attacks or trolling
- Publishing private information without permission
- Other conduct that would be inappropriate in a professional setting

## Getting Help

- **Documentation**: Check the README.md first
- **Issues**: Search existing issues before creating new ones
- **Discussions**: Use GitHub Discussions for questions
- **Code Review**: Maintainers will review PRs and provide feedback

## Recognition

Contributors are recognized in:
- Git commit history
- Release notes for significant contributions
- README.md contributors section (for major contributions)

Thank you for contributing to HolyPackager!
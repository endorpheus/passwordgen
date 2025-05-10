# CLI Password Generator (pwgen)

A lightweight, secure command-line password generator with clipboard integration.

## Features

- Generate cryptographically secure random passwords
- Customize password length and character sets
- Enforce minimum character types for stronger passwords
- Avoid similar-looking characters (optional)
- Copy generated passwords to clipboard with automatic clearing
- Password strength evaluation

## Requirements

### Linux
- GCC or Clang compiler with C++11 support
- `xclip` for clipboard functionality
- pthread library

### macOS
- Xcode Command Line Tools or GCC/Clang
- C++11 support
- Built-in `pbcopy` for clipboard functionality

### Windows
- MinGW-w64 or Visual Studio with C++11 support
- Windows API for clipboard functionality

## Compilation Guide

### Linux

1. Install required dependencies:
   ```bash
   # Debian/Ubuntu
   sudo apt update
   sudo apt install build-essential xclip
   
   # Fedora/RHEL
   sudo dnf install gcc-c++ xclip
   
   # Arch Linux
   sudo pacman -S gcc xclip
   ```

2. Compile the program:
   ```bash
   g++ -o pwgen pwgen.cpp -std=c++11 -pthread
   ```

3. (Optional) Install system-wide:
   ```bash
   sudo cp pwgen /usr/local/bin/
   ```

### macOS

1. Install Xcode Command Line Tools (if not already installed):
   ```bash
   xcode-select --install
   ```

2. Compile the program:
   ```bash
   g++ -o pwgen pwgen.cpp -std=c++11 -pthread
   ```

3. (Optional) Install system-wide:
   ```bash
   sudo cp pwgen /usr/local/bin/
   ```

### Windows (using MinGW-w64)

1. Install MinGW-w64 from [MinGW-w64 website](https://www.mingw-w64.org/) or using a package manager like [MSYS2](https://www.msys2.org/)

2. Open Command Prompt or MinGW shell and navigate to your source code directory

3. Compile the program:
   ```bash
   g++ -o pwgen.exe pwgen.cpp -std=c++11 -static
   ```

Note: The `-static` flag creates a standalone executable without additional DLL dependencies.

### Windows (using Visual Studio)

1. Open Visual Studio Developer Command Prompt
2. Navigate to your source code directory
3. Compile:
   ```bash
   cl /EHsc /std:c++14 pwgen.cpp /Fe:pwgen.exe
   ```

## Usage

```
Secure Password Generator - Usage:
  pwgen [options]

Options:
  -l <length>  Set password length (default: 16, minimum: 8, sweet-spot: 17+)
  -p <seconds> Copy to clipboard and clear after timeout
  -u           Uppercase letters only
  -d           Digits only
  -s           No special characters
  -S           Avoid similar characters (I, l, 1, O, 0)
  -m           Don't enforce minimum character types
  -a           Alphanumeric only (same as -s)
  -h           Show this help message
  -n           No password strength quality meter (this can be helpful for external scripting)
```

## Examples

### Basic Password Generation

```bash
# Generate default password (16 chars, all character types)
pwgen

# Generate 20-character password
pwgen -l 20

# Generate digits-only PIN
pwgen -d -l 8

# Generate password without special characters
pwgen -s

# Generate password avoiding similar-looking characters
pwgen -S
```

### Clipboard Integration

```bash
# Generate password and copy to clipboard, clear after 30 seconds
pwgen -p 30

# Generate 24-character password and copy to clipboard for 60 seconds
pwgen -l 24 -p 60

# Generate alphanumeric password and copy to clipboard for 15 seconds
pwgen -a -p 15
```

## Security Features

1. **Cryptographically Secure Random Generation**:
   - Uses hardware-based entropy source via `std::random_device`
   - Properly seeds a Mersenne Twister engine (mt19937_64)
   
2. **Proper Character Distribution**:
   - Enforces minimum character set requirements when enabled
   - Shuffles the final password to avoid predictable patterns
   
3. **Clipboard Security**:
   - Automatically clears the clipboard after the specified timeout
   - Signal handling ensures cleanup even on program termination
   
4. **Password Strength Evaluation**:
   - Evaluates and reports password strength based on:
     - Length
     - Character variety
     - Estimated entropy

## Password Strength Ratings

The password strength is rated from 0-100:

- **0-29**: Very Weak
- **30-49**: Weak
- **50-69**: Moderate
- **70-89**: Strong
- **90-100**: Very Strong

## Best Practices

1. Use longer passwords (16+ characters) for important accounts
2. Enable all character types for maximum strength
3. Use the clipboard timeout feature in public environments
4. For passwords you need to memorize, consider using the `-S` option to avoid similar characters

## Integration Tips

This tool can be easily incorporated into scripts and other automation:

```bash
# Save password to a variable in bash
PASSWORD=$(pwgen)

# Generate multiple passwords
for i in {1..5}; do
  pwgen -l 20
done > passwords.txt

# Use with other tools (e.g., pass password manager)
pwgen | pass insert myaccount
```

## Troubleshooting

### Linux Clipboard Issues
- Ensure xclip is installed
- For Wayland systems, you might need to use wl-clipboard instead

### Windows Clipboard Issues
- Running in certain terminal environments might cause issues
- Try running the program with administrator privileges

### macOS Clipboard Issues
- If terminal doesn't have proper permissions, clipboard access might fail
- Check terminal permissions in System Preferences > Security & Privacy

## License

This tool is open source and free to use, modify, and distribute.

## Credits

Based on secure password generation best practices and modern C++ standards.


# TL;DR 

Compile CLI (pwgen)

Change to the cli directory, then do:
```
g++ -o pwgen pwgen.cpp -std=c++11
```

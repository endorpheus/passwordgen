#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <functional>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>  // Added missing header for std::atomic

// Cross-platform clipboard support
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#include <cstring>
#endif

// Cross-platform clipboard functions
void setClipboardText(const std::string& text) {
#ifdef _WIN32
    // Windows implementation
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hg) {
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg), text.c_str(), text.size() + 1);
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
#else
    // Unix/Linux/macOS implementation using xclip or pbcopy
    FILE* pipe;
#ifdef __APPLE__
    pipe = popen("pbcopy", "w");
#else
    pipe = popen("xclip -selection clipboard", "w");
#endif
    if (pipe) {
        fwrite(text.c_str(), sizeof(char), text.size(), pipe);
        pclose(pipe);
    }
#endif
}

void clearClipboard() {
    setClipboardText("");
}

// Signal handler for cleanup
std::atomic<bool> g_running{true};  // Changed initialization syntax for better compatibility
void signalHandler(int signal) {
    g_running = false;
}

class PasswordGenerator {
private:
    // Cryptographically secure random number generator
    std::random_device rd;
    std::mt19937_64 secureGenerator;
    
    // Character sets
    const std::string UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
    const std::string DIGITS = "0123456789";
    const std::string SPECIAL = "!@#$%^&*()-_=+[]{};:,.<>?/";
    const std::string SIMILAR = "Il1O0";
    
    // Default settings
    int length = 16;
    bool useUpper = true;
    bool useLower = true;
    bool useDigits = true;
    bool useSpecial = true;
    bool enforceMinimum = true;
    bool avoidSimilar = false;
    int clipboardTimeout = 0; // seconds, 0 = disabled
    bool showStrengthMeter = true; // Show strength meter by default
    
    // Initialize random generator with strong entropy
    void initSecureRandom() {
        std::array<unsigned int, std::mt19937_64::state_size> seedData;
        std::generate(seedData.begin(), seedData.end(), std::ref(rd));
        std::seed_seq seq(seedData.begin(), seedData.end());
        secureGenerator.seed(seq);
    }
    
    // Calculate password strength score (0-100)
    int calculateStrength(const std::string& password) {
        if (password.empty()) return 0;
        
        int score = 0;
        
        // Length contribution (up to 40 points)
        score += std::min(40, static_cast<int>(password.length() * 2));
        
        // Character variety (up to 30 points)
        bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
        for (char ch : password) {
            if (islower(ch)) hasLower = true;
            else if (isupper(ch)) hasUpper = true;
            else if (isdigit(ch)) hasDigit = true;
            else hasSpecial = true;
        }
        
        int charTypes = (hasLower ? 1 : 0) + (hasUpper ? 1 : 0) + 
                        (hasDigit ? 1 : 0) + (hasSpecial ? 1 : 0);
        score += charTypes * 7.5;
        
        // Entropy approximation (up to 30 points)
        double entropy = 0;
        if (hasLower) entropy += 26;
        if (hasUpper) entropy += 26;
        if (hasDigit) entropy += 10;
        if (hasSpecial) entropy += 33;
        
        entropy = log2(entropy) * password.length();
        score += std::min(30.0, entropy / 4.0);
        
        return std::min(100, score);
    }
    
    // Get strength description based on score
    std::string getStrengthDescription(int score) {
        if (score < 30) return "Very Weak";
        if (score < 50) return "Weak";
        if (score < 70) return "Moderate";
        if (score < 90) return "Strong";
        return "Very Strong";
    }

public:
    PasswordGenerator() {
        initSecureRandom();
    }
    
    // Setters for configuration
    void setLength(int value) { 
        length = value; 
    }
    
    void setClipboardTimeout(int value) { 
        clipboardTimeout = value; 
    }
    
    void setCharSets(bool upper, bool lower, bool digits, bool special) {
        useUpper = upper;
        useLower = lower;
        useDigits = digits;
        useSpecial = special;
        
        // Ensure at least one character set is enabled
        if (!useUpper && !useLower && !useDigits && !useSpecial) {
            useLower = true;
        }
    }
    
    void setSpecialChars(bool enabled) { 
        useSpecial = enabled; 
    }
    
    void setAvoidSimilar(bool enabled) { 
        avoidSimilar = enabled; 
    }
    
    void setEnforceMinimum(bool enabled) { 
        enforceMinimum = enabled; 
    }
    
    void setShowStrengthMeter(bool enabled) { 
        showStrengthMeter = enabled; 
    }
    
    // Generate a secure password based on current settings
    std::string generate() {
        // Filter character sets based on settings
        std::string chars;
        
        // Apply the "avoid similar" filter if needed
        std::string upperChars = UPPERCASE;
        std::string lowerChars = LOWERCASE;
        std::string digitChars = DIGITS;
        
        if (avoidSimilar) {
            for (char c : SIMILAR) {
                upperChars.erase(std::remove(upperChars.begin(), upperChars.end(), c), upperChars.end());
                lowerChars.erase(std::remove(lowerChars.begin(), lowerChars.end(), c), lowerChars.end());
                digitChars.erase(std::remove(digitChars.begin(), digitChars.end(), c), digitChars.end());
            }
        }
        
        if (useUpper) chars += upperChars;
        if (useLower) chars += lowerChars;
        if (useDigits) chars += digitChars;
        if (useSpecial) chars += SPECIAL;
        
        std::string password;
        
        if (enforceMinimum) {
            // Calculate minimum length needed
            int minLength = 0;
            if (useUpper) minLength++;
            if (useLower) minLength++;
            if (useDigits) minLength++;
            if (useSpecial) minLength++;
            
            if (length < minLength) {
                length = minLength;
                std::cerr << "Password length increased to " << length 
                          << " to accommodate minimum character requirements." << std::endl;
            }
            
            // Add one of each required type
            if (useUpper) {
                std::uniform_int_distribution<int> dist(0, upperChars.length() - 1);
                password += upperChars[dist(secureGenerator)];
            }
            
            if (useLower) {
                std::uniform_int_distribution<int> dist(0, lowerChars.length() - 1);
                password += lowerChars[dist(secureGenerator)];
            }
            
            if (useDigits) {
                std::uniform_int_distribution<int> dist(0, digitChars.length() - 1);
                password += digitChars[dist(secureGenerator)];
            }
            
            if (useSpecial) {
                std::uniform_int_distribution<int> dist(0, SPECIAL.length() - 1);
                password += SPECIAL[dist(secureGenerator)];
            }
        }
        
        // Fill the rest randomly
        std::uniform_int_distribution<int> dist(0, chars.length() - 1);
        while (password.length() < length) {
            password += chars[dist(secureGenerator)];
        }
        
        // Shuffle the result to avoid predictable positions
        std::shuffle(password.begin(), password.end(), secureGenerator);
        
        return password;
    }
    
    // Display help text
    void showHelp() {
        std::cout << "Secure Password Generator - Usage:" << std::endl
                  << "  pwgen [options]" << std::endl
                  << std::endl
                  << "Options:" << std::endl
                  << "  -l <length>  Set password length (default: 16)" << std::endl
                  << "  -p <seconds> Copy to clipboard and clear after timeout" << std::endl
                  << "  -u           Uppercase letters only" << std::endl
                  << "  -d           Digits only" << std::endl
                  << "  -s           No special characters" << std::endl
                  << "  -S           Avoid similar characters (I, l, 1, O, 0)" << std::endl
                  << "  -m           Don't enforce minimum character types" << std::endl
                  << "  -n           Disable password strength meter" << std::endl
                  << "  -a           Alphanumeric only (same as -s)" << std::endl
                  << "  -h           Show this help message" << std::endl;
    }
    
    // Handle clipboard functionality with timeout
    void handleClipboard(const std::string& password) {
        if (clipboardTimeout <= 0) {
            return; // Clipboard feature disabled
        }
        
        // Set up signal handlers for clean exit
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        // Copy to clipboard
        setClipboardText(password);
        std::cout << "Password copied to clipboard. Will clear in " 
                  << clipboardTimeout << " seconds." << std::endl;
        
        // Wait for timeout
        auto startTime = std::chrono::steady_clock::now();
        while (g_running) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            
            if (elapsed >= clipboardTimeout) {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Clear clipboard
        if (g_running) {
            clearClipboard();
            std::cout << "Clipboard cleared." << std::endl;
        }
    }
    
    // Display password with strength info
    void displayPassword(const std::string& password) {
        // Always show the password
        std::cout << password << std::endl;
        
        // Show strength meter if enabled
        if (showStrengthMeter) {
            int strength = calculateStrength(password);
            std::string rating = getStrengthDescription(strength);
            std::cout << "Strength: " << strength << "/100 (" << rating << ")" << std::endl;
        }
        
        // Handle clipboard if timeout is set
        if (clipboardTimeout > 0) {
            handleClipboard(password);
        }
    }
};

// Custom command-line argument parser to handle errors better than getopt
void parseCommandLine(int argc, char* argv[], PasswordGenerator& generator) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        // Check if it's an option (starts with -)
        if (arg[0] == '-') {
            // Single character options
            if (arg.length() == 2) {
                char option = arg[1];
                
                switch (option) {
                    case 'l': // length
                        if (i + 1 < argc && argv[i+1][0] != '-') {
                            try {
                                int length = std::stoi(argv[++i]);
                                if (length < 8) {
                                    std::cerr << "Warning: Password length less than 8 is not recommended." << std::endl;
                                    length = 8;
                                }
                                generator.setLength(length);
                            } catch (const std::exception& e) {
                                std::cerr << "Error: Invalid length parameter. Using default length." << std::endl;
                            }
                        } else {
                            std::cerr << "Error: -l option requires a numeric argument. Using default length." << std::endl;
                        }
                        break;
                        
                    case 'p': // clipboard timeout
                        if (i + 1 < argc && argv[i+1][0] != '-') {
                            try {
                                int timeout = std::stoi(argv[++i]);
                                if (timeout < 0) timeout = 0;
                                generator.setClipboardTimeout(timeout);
                            } catch (const std::exception& e) {
                                std::cerr << "Error: Invalid clipboard timeout. Clipboard feature disabled." << std::endl;
                            }
                        } else {
                            std::cerr << "Error: -p option requires a numeric argument. Clipboard feature disabled." << std::endl;
                        }
                        break;
                        
                    case 'u': // uppercase only
                        generator.setCharSets(true, false, false, false);
                        break;
                        
                    case 'd': // digits only
                        generator.setCharSets(false, false, true, false);
                        break;
                        
                    case 's': // no special chars
                        generator.setSpecialChars(false);
                        break;
                        
                    case 'S': // avoid similar chars
                        generator.setAvoidSimilar(true);
                        break;
                        
                    case 'm': // don't enforce minimum chars
                        generator.setEnforceMinimum(false);
                        break;
                        
                    case 'n': // disable strength meter
                        generator.setShowStrengthMeter(false);
                        break;
                        
                    case 'a': // alphanumeric only (no special)
                        generator.setSpecialChars(false);
                        break;
                        
                    case 'h': // help
                        generator.showHelp();
                        exit(0);
                        break;
                        
                    default:
                        std::cerr << "Warning: Unknown option -" << option << " ignored." << std::endl;
                        break;
                }
            }
            // Handle multiple options in a single argument (e.g., -aS)
            else if (arg.length() > 2) {
                for (size_t j = 1; j < arg.length(); j++) {
                    char option = arg[j];
                    
                    switch (option) {
                        case 'u': 
                            generator.setCharSets(true, false, false, false);
                            break;
                        case 'd': 
                            generator.setCharSets(false, false, true, false);
                            break;
                        case 's': 
                            generator.setSpecialChars(false);
                            break;
                        case 'S': 
                            generator.setAvoidSimilar(true);
                            break;
                        case 'm': 
                            generator.setEnforceMinimum(false);
                            break;
                        case 'n': 
                            generator.setShowStrengthMeter(false);
                            break;
                        case 'a': 
                            generator.setSpecialChars(false);
                            break;
                        case 'h': 
                            generator.showHelp();
                            exit(0);
                            break;
                        case 'l': 
                        case 'p': 
                            std::cerr << "Warning: Options -l and -p require values and cannot be grouped." << std::endl;
                            break;
                        default:
                            std::cerr << "Warning: Unknown option -" << option << " ignored." << std::endl;
                            break;
                    }
                }
            }
        } else {
            // Non-option argument - not supported
            std::cerr << "Warning: Unexpected argument '" << arg << "' ignored." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        PasswordGenerator generator;
        
        if (argc > 1) {
            // Use custom argument parser for better error handling
            parseCommandLine(argc, argv, generator);
        }
        
        std::string password = generator.generate();
        generator.displayPassword(password);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Error: An unknown error occurred." << std::endl;
        return 1;
    }
}
# Smart Stock Tracker - Makefile
# Author: [Your Name]
# Date: October 2025

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LIBS = -lcurl -ljson-c -lm

# Directories
SRCDIR = .
WEBDIR = web
DATADIR = data

# Source files
SOURCES = main.c stock_fetcher.c analyzer.c file_handler.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = stock_tracker

# Default target
all: $(TARGET) setup

# Build the main executable
$(TARGET): $(OBJECTS)
	@echo "🔗 Linking $(TARGET)..."
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)
	@echo "✅ Build successful! Run with: ./$(TARGET)"

# Compile source files
%.o: %.c stock_tracker.h
	@echo "🔨 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Setup directories and initial files
setup:
	@echo "📁 Setting up directories..."
	@mkdir -p $(WEBDIR)
	@mkdir -p $(DATADIR)
	@echo "📂 Directories created successfully!"

# Clean build files
clean:
	@echo "🧹 Cleaning build files..."
	@rm -f $(OBJECTS)
	@rm -f $(TARGET)
	@echo "✅ Clean complete!"

# Clean everything including generated files
cleanall: clean
	@echo "🧹 Cleaning all generated files..."
	@rm -rf $(WEBDIR)
	@rm -rf $(DATADIR)
	@rm -f *.txt *.log *.json
	@echo "✅ Full cleanup complete!"

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "📦 Installing dependencies..."
	@sudo apt-get update
	@sudo apt-get install -y libcurl4-openssl-dev libjson-c-dev build-essential
	@echo "✅ Dependencies installed!"

# Install dependencies (macOS with Homebrew)
install-deps-mac:
	@echo "📦 Installing dependencies for macOS..."
	@brew install curl json-c
	@echo "✅ Dependencies installed!"

# Run the program
run: $(TARGET)
	@echo "🚀 Running Smart Stock Tracker..."
	@./$(TARGET)

# Create a demo with sample data
demo: $(TARGET)
	@echo "🎬 Running demo mode..."
	@echo "This will show the program with sample data"
	@./$(TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)
	@echo "🐛 Debug build complete!"

# Release build (optimized)
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)
	@echo "🚀 Release build complete!"

# Package the project
package: cleanall
	@echo "📦 Creating project package..."
	@tar -czf smart_stock_tracker.tar.gz *.c *.h Makefile README.md
	@echo "✅ Package created: smart_stock_tracker.tar.gz"

# Check for memory leaks (requires valgrind)
check-memory: $(TARGET)
	@echo "🔍 Checking for memory leaks..."
	@valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Format code (requires clang-format)
format:
	@echo "✨ Formatting code..."
	@clang-format -i *.c *.h
	@echo "✅ Code formatted!"

# Static analysis (requires cppcheck)
analyze:
	@echo "🔍 Running static analysis..."
	@cppcheck --enable=all --std=c99 *.c *.h
	@echo "✅ Analysis complete!"

# Show help
help:
	@echo "📊 Smart Stock Tracker - Available Make Targets:"
	@echo ""
	@echo "Building:"
	@echo "  all           - Build the complete project (default)"
	@echo "  debug         - Build with debug symbols"
	@echo "  release       - Build optimized release version"
	@echo "  clean         - Remove build files"
	@echo "  cleanall      - Remove all generated files"
	@echo ""
	@echo "Dependencies:"
	@echo "  install-deps     - Install dependencies (Ubuntu/Debian)"
	@echo "  install-deps-mac - Install dependencies (macOS)"
	@echo ""
	@echo "Running:"
	@echo "  run           - Build and run the program"
	@echo "  demo          - Run with demo data"
	@echo ""
	@echo "Development:"
	@echo "  format        - Format source code"
	@echo "  analyze       - Run static analysis"
	@echo "  check-memory  - Check for memory leaks"
	@echo "  package       - Create distribution package"
	@echo ""
	@echo "  help          - Show this help message"

# API key setup helper
setup-api:
	@echo "🔑 API Key Setup Instructions:"
	@echo ""
	@echo "1. Visit: https://www.alphavantage.co/support/#api-key"
	@echo "2. Sign up for a free account"
	@echo "3. Copy your API key"
	@echo "4. Edit stock_tracker.h and replace 'YOUR_API_KEY_HERE' with your key"
	@echo ""
	@echo "Example:"
	@echo '#define API_KEY "ABCD1234EFGH5678"'
	@echo ""
	@echo "Note: Free tier allows 5 API calls per minute"

# Test build on different systems
test-build:
	@echo "🧪 Testing build on current system..."
	@echo "System: $$(uname -s)"
	@echo "Architecture: $$(uname -m)"
	@echo "Compiler: $$($(CC) --version | head -n1)"
	@$(MAKE) clean
	@$(MAKE) all
	@echo "✅ Build test passed!"

# Show project statistics
stats:
	@echo "📊 Project Statistics:"
	@echo "Source files: $$(find . -name '*.c' | wc -l)"
	@echo "Header files: $$(find . -name '*.h' | wc -l)"
	@echo "Total lines of code: $$(cat *.c *.h | wc -l)"
	@echo "Functions: $$(grep -c '^[a-zA-Z_][a-zA-Z0-9_]*(' *.c)"

# Backup project
backup:
	@echo "💾 Creating backup..."
	@DATE=$$(date +%Y%m%d_%H%M%S) && \
	tar -czf "backup_stock_tracker_$$DATE.tar.gz" *.c *.h Makefile README.md && \
	echo "✅ Backup created: backup_stock_tracker_$$DATE.tar.gz"

# Quick start for new users
quickstart: install-deps all setup-api
	@echo ""
	@echo "🎉 Quick Start Complete!"
	@echo ""
	@echo "Next steps:"
	@echo "1. Get your API key (see instructions above)"
	@echo "2. Edit stock_tracker.h with your API key"
	@echo "3. Run: make run"
	@echo ""
	@echo "Enjoy your Smart Stock Tracker! 📊"

# Special targets that don't represent files
.PHONY: all clean cleanall install-deps install-deps-mac run demo debug release package check-memory format analyze help setup-api test-build stats backup quickstart setup

# Default shell
SHELL := /bin/bash
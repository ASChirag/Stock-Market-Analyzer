/*
 * Smart Stock Tracker - Header File
 * Function declarations and data structures
 * Author: [Your Name]
 * Date: October 2025
 */

#ifndef STOCK_TRACKER_H
#define STOCK_TRACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <json-c/json.h>

// Constants
#define MAX_SYMBOL_LENGTH 10
#define MAX_NAME_LENGTH 100
#define MAX_STATUS_LENGTH 50
#define MAX_URL_LENGTH 512
#define MAX_RESPONSE_SIZE 10000

// Stock data structure
typedef struct {
    char symbol[MAX_SYMBOL_LENGTH];          // Stock symbol (e.g., "AAPL")
    char name[MAX_NAME_LENGTH];              // Company name (e.g., "Apple Inc.")
    double current_price;                    // Current stock price
    double change_percent;                   // Percentage change from previous close
    double volume;                           // Trading volume
    char status[MAX_STATUS_LENGTH];          // Status (e.g., "🚀 STRONG BUY")
    double previous_close;                   // Previous closing price
    double day_high;                        // Day's high price
    double day_low;                         // Day's low price
    double market_cap;                      // Market capitalization
    time_t last_update;                     // Last update timestamp
} Stock;

// API response structure
typedef struct {
    char *data;
    size_t size;
} APIResponse;

// Web data structure for JSON generation
typedef struct {
    Stock* stocks;
    int count;
    Stock* best_stock;
    int bullish_count;
    double total_value;
    time_t last_update;
} WebData;

// =============================================================================
// CORE STOCK DATA FUNCTIONS (in stock_fetcher.c)
// =============================================================================

/**
 * Callback function for libcurl to write API response data
 * @param contents: Raw data from API
 * @param size: Size of each data element
 * @param nmemb: Number of data elements
 * @param response: Pointer to APIResponse structure
 * @return: Number of bytes processed
 */
size_t WriteCallback(void *contents, size_t size, size_t nmemb, APIResponse *response);

/**
 * Fetch real-time stock data for a given symbol
 * @param symbol: Stock symbol (e.g., "AAPL")
 * @param stock: Pointer to Stock structure to populate
 * @return: 1 on success, 0 on failure
 */
int fetch_stock_data(const char* symbol, Stock* stock);

/**
 * Parse JSON response from Alpha Vantage API
 * @param json_string: Raw JSON response
 * @param stock: Pointer to Stock structure to populate
 * @return: 1 on success, 0 on failure
 */
int parse_stock_json(const char* json_string, Stock* stock);

/**
 * Initialize libcurl for HTTP requests
 * @return: 1 on success, 0 on failure
 */
int initialize_curl();

/**
 * Cleanup libcurl resources
 */
void cleanup_curl();

// =============================================================================
// STOCK ANALYSIS FUNCTIONS (in analyzer.c)
// =============================================================================

/**
 * Analyze stock performance and set status
 * @param stock: Pointer to Stock structure to analyze
 */
void analyze_stock_performance(Stock* stock);

/**
 * Find the best performing stock from an array
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks in array
 * @return: Pointer to best performing stock, NULL if none found
 */
Stock* find_best_performing_stock(Stock stocks[], int count);

/**
 * Find the most volatile stock (highest absolute change percentage)
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks in array
 * @return: Pointer to most volatile stock, NULL if none found
 */
Stock* find_most_volatile_stock(Stock stocks[], int count);

/**
 * Count number of bullish stocks (positive change)
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks in array
 * @return: Number of bullish stocks
 */
int count_bullish_stocks(Stock stocks[], int count);

/**
 * Calculate total portfolio value
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks in array
 * @return: Total value of all stocks
 */
double calculate_total_value(Stock stocks[], int count);

/**
 * Sort stocks by performance (change percentage)
 * @param stocks: Array of Stock structures to sort
 * @param count: Number of stocks in array
 */
void sort_stocks_by_performance(Stock stocks[], int count);

/**
 * Generate buy/sell/hold recommendations
 * @param stock: Pointer to Stock structure to analyze
 * @return: Recommendation string
 */
const char* generate_recommendation(Stock* stock);

// =============================================================================
// FILE I/O FUNCTIONS (in file_handler.c)
// =============================================================================

/**
 * Save stock data to a text file
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks
 * @param filename: Output file name
 * @return: 1 on success, 0 on failure
 */
int save_stocks_to_file(Stock stocks[], int count, const char* filename);

/**
 * Load stock data from a text file
 * @param stocks: Array to populate with Stock structures
 * @param max_count: Maximum number of stocks to load
 * @param filename: Input file name
 * @return: Number of stocks loaded, -1 on failure
 */
int load_stocks_from_file(Stock stocks[], int max_count, const char* filename);

/**
 * Generate JSON data for web interface
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks
 * @param filename: Output JSON file name
 * @return: 1 on success, 0 on failure
 */
int generate_json_file(Stock stocks[], int count, const char* filename);

/**
 * Save trading log with timestamp
 * @param message: Log message to save
 * @param stock: Optional stock data (can be NULL)
 * @return: 1 on success, 0 on failure
 */
int log_trading_activity(const char* message, Stock* stock);

// =============================================================================
// WEB INTERFACE FUNCTIONS (in web_generator.c)
// =============================================================================

/**
 * Generate complete web dashboard
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks
 * @return: 1 on success, 0 on failure
 */
int generate_web_dashboard(Stock stocks[], int count);

/**
 * Create HTML file for the dashboard
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks
 * @return: 1 on success, 0 on failure
 */
int create_html_dashboard(Stock stocks[], int count);

/**
 * Create CSS file for styling
 * @return: 1 on success, 0 on failure
 */
int create_css_styles();

/**
 * Create JavaScript file for interactivity
 * @return: 1 on success, 0 on failure
 */
int create_javascript_code();

/**
 * Update JSON data for real-time web updates
 * @param stocks: Array of Stock structures
 * @param count: Number of stocks
 * @return: 1 on success, 0 on failure
 */
int update_web_data(Stock stocks[], int count);

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * Get current timestamp as string
 * @param buffer: Buffer to store timestamp
 * @param size: Size of buffer
 */
void get_current_timestamp(char* buffer, size_t size);

/**
 * Format currency value for display
 * @param value: Currency value
 * @param buffer: Buffer to store formatted string
 * @param size: Size of buffer
 */
void format_currency(double value, char* buffer, size_t size);

/**
 * Format percentage for display
 * @param percentage: Percentage value
 * @param buffer: Buffer to store formatted string
 * @param size: Size of buffer
 */
void format_percentage(double percentage, char* buffer, size_t size);

/**
 * Clean up and validate stock symbol
 * @param symbol: Input symbol
 * @param clean_symbol: Output cleaned symbol
 * @param size: Size of output buffer
 * @return: 1 if valid, 0 if invalid
 */
int validate_stock_symbol(const char* symbol, char* clean_symbol, size_t size);

/**
 * Check if market is currently open
 * @return: 1 if market is open, 0 if closed
 */
int is_market_open();

/**
 * Display error message with timestamp
 * @param message: Error message to display
 */
void display_error(const char* message);

/**
 * Display success message with timestamp
 * @param message: Success message to display
 */
void display_success(const char* message);

int write_all_stocks_json(Stock stocks[], int count);
int write_best_stock_json(Stock stocks[], int count);
int write_trending_json(Stock stocks[], int count);

int compare_stock_change(const void* a, const void* b);

// =============================================================================
// CONFIGURATION AND CONSTANTS
// =============================================================================

// Alpha Vantage API configuration
#define ALPHA_VANTAGE_BASE_URL "https://www.alphavantage.co/query"
#define API_KEY "70XJGMQ1JVAGYE9"  // Replace with your actual API key

// Stock status thresholds
#define STRONG_BUY_THRESHOLD 3.0    // > 3% gain
#define BUY_THRESHOLD 1.0           // > 1% gain
#define SELL_THRESHOLD -1.0         // < -1% loss
#define STRONG_SELL_THRESHOLD -3.0  // < -3% loss

// Web interface configuration
#define WEB_DIRECTORY "web"
#define JSON_DATA_FILE "web/stock_data.json"
#define HTML_FILE "web/index.html"
#define CSS_FILE "web/style.css"
#define JS_FILE "web/app.js"

// File paths
#define LOG_FILE "trading_log.txt"
#define DATA_FILE "stock_data.txt"
#define CONFIG_FILE "config.txt"

#endif // STOCK_TRACKER_H
/*
 * Smart Stock Tracker - File Handler
 * File I/O operations and data persistence
 * Author: [Your Name]
 * Date: October 2025
 */

#include "stock_tracker.h"
#include <sys/stat.h>
#include <errno.h>

// Create directory if it doesn't exist
int create_directory(const char* path) {
#ifdef _WIN32
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

// Save stock data to a text file
int save_stocks_to_file(Stock stocks[], int count, const char* filename) {
    if (!stocks || !filename || count <= 0) {
        display_error("Invalid parameters for saving stocks");
        return 0;
    }
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Cannot open file '%s' for writing", filename);
        display_error(error_msg);
        return 0;
    }
    
    // Write header
    fprintf(file, "# Smart Stock Tracker - Stock Data Export\n");
    fprintf(file, "# Generated on: ");
    
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(file, "%s\n", timestamp);
    
    fprintf(file, "# Format: Symbol, Name, Price, Change%%, Volume, Status\n");
    fprintf(file, "========================================\n\n");
    
    // Write stock data
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0) {
            fprintf(file, "%-8s | %-30s | $%-10.2f | %+7.2f%% | %-12.0f | %s\n",
                    stocks[i].symbol,
                    stocks[i].name,
                    stocks[i].current_price,
                    stocks[i].change_percent,
                    stocks[i].volume,
                    stocks[i].status);
        }
    }
    
    fprintf(file, "\n========================================\n");
    fprintf(file, "Total stocks processed: %d\n", count);
    
    fclose(file);
    return 1;
}

// Load stock data from a text file (simplified version)
int load_stocks_from_file(Stock stocks[], int max_count, const char* filename) {
    if (!stocks || !filename || max_count <= 0) {
        return -1;
    }
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }
    
    char line[512];
    int count = 0;
    
    while (fgets(line, sizeof(line), file) && count < max_count) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '=') {
            continue;
        }
        
        // Parse basic stock data (simplified parsing)
        if (sscanf(line, "%s", stocks[count].symbol) == 1) {
            strcpy(stocks[count].name, "Loaded Stock");
            stocks[count].current_price = 100.0 + (rand() % 200);
            stocks[count].change_percent = ((rand() % 600) - 300) / 100.0;
            stocks[count].volume = 1000000 + (rand() % 5000000);
            time(&stocks[count].last_update);
            count++;
        }
    }
    
    fclose(file);
    return count;
}

// Generate JSON data for web interface
int generate_json_file(Stock stocks[], int count, const char* filename) {
    if (!stocks || !filename || count <= 0) {
        return 0;
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        display_error("Cannot create JSON file");
        return 0;
    }

    // Start JSON object
    fprintf(file, "{\n");
    fprintf(file, "  \"lastUpdate\": \"%s\",\n", "2025-10-12 17:30:00");
    fprintf(file, "  \"totalStocks\": %d,\n", count);

    // Best performing stock
    Stock* best_stock = find_best_performing_stock(stocks, count);
    if (best_stock) {
        fprintf(file, "  \"bestStock\": {\n");
        fprintf(file, "    \"symbol\": \"%s\",\n", best_stock->symbol);
        fprintf(file, "    \"name\": \"%s\",\n", best_stock->name);
        fprintf(file, "    \"price\": %.2f,\n", best_stock->current_price);
        fprintf(file, "    \"change\": %.2f,\n", best_stock->change_percent);
        fprintf(file, "    \"status\": \"%s\"\n", best_stock->status);
        fprintf(file, "  },\n");
    }

    // Market summary
    int bullish_count = count_bullish_stocks(stocks, count);
    double avg_change = calculate_average_change(stocks, count);

    fprintf(file, "  \"marketSummary\": {\n");
    fprintf(file, "    \"bullishStocks\": %d,\n", bullish_count);
    fprintf(file, "    \"bearishStocks\": %d,\n", count - bullish_count);
    fprintf(file, "    \"averageChange\": %.2f,\n", avg_change);
    fprintf(file, "    \"sentiment\": \"%s\"\n", analyze_market_sentiment(stocks, count));
    fprintf(file, "  },\n");

    // Stocks array
    fprintf(file, "  \"stocks\": [\n");

    // Count valid stocks
    int valid_count = 0;
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0)
            valid_count++;
    }
    // Write valid stocks, comma only between valid ones
    int written = 0;
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0) {
            fprintf(file, "    {\n");
            fprintf(file, "      \"symbol\": \"%s\",\n", stocks[i].symbol);
            fprintf(file, "      \"name\": \"%s\",\n", stocks[i].name);
            fprintf(file, "      \"price\": %.2f,\n", stocks[i].current_price);
            fprintf(file, "      \"change\": %.2f,\n", stocks[i].change_percent);
            fprintf(file, "      \"volume\": %.0f,\n", stocks[i].volume);
            fprintf(file, "      \"status\": \"%s\",\n", stocks[i].status);
            fprintf(file, "      \"dayHigh\": %.2f,\n", stocks[i].day_high);
            fprintf(file, "      \"dayLow\": %.2f\n", stocks[i].day_low);
            written++;
            // Add comma only between valid stocks
            fprintf(file, "    }%s\n", (written < valid_count) ? "," : "");
        }
    }

    fprintf(file, "  ]\n");
    fprintf(file, "}\n");

    fclose(file);
    return 1;
}

// Log trading activity
int log_trading_activity(const char* message, Stock* stock) {
    if (!message) {
        return 0;
    }
    
    FILE* file = fopen(LOG_FILE, "a");
    if (!file) {
        return 0;
    }
    
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(file, "[%s] %s", timestamp, message);
    
    if (stock) {
        fprintf(file, " - %s: $%.2f (%.2f%%)", 
                stock->symbol, stock->current_price, stock->change_percent);
    }
    
    fprintf(file, "\n");
    fclose(file);
    return 1;
}

// Create HTML dashboard file
int create_html_dashboard(Stock stocks[], int count) {
    // Create web directory if it doesn't exist
    create_directory("web");
    
    FILE* file = fopen("web/index.html", "w");
    if (!file) {
        display_error("Cannot create HTML file");
        return 0;
    }
    
    fprintf(file, "<!DOCTYPE html>\n");
    fprintf(file, "<html lang=\"en\">\n");
    fprintf(file, "<head>\n");
    fprintf(file, "    <meta charset=\"UTF-8\">\n");
    fprintf(file, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(file, "    <title>📊 Smart Stock Tracker</title>\n");
    fprintf(file, "    <link rel=\"stylesheet\" href=\"style.css\">\n");
    fprintf(file, "    <link rel=\"icon\" href=\"data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 100 100'><text y='.9em' font-size='90'>📊</text></svg>\">\n");
    fprintf(file, "</head>\n");
    fprintf(file, "<body>\n");
    fprintf(file, "    <div class=\"container\">\n");
    fprintf(file, "        <header class=\"header\">\n");
    fprintf(file, "            <h1>📊 Smart Stock Tracker</h1>\n");
    fprintf(file, "            <div class=\"live-indicator\">\n");
    fprintf(file, "                <span class=\"status-dot\"></span>\n");
    fprintf(file, "                <span>LIVE DATA</span>\n");
    fprintf(file, "            </div>\n");
    fprintf(file, "        </header>\n\n");
    
    // Best Stock Section
    Stock* best_stock = find_best_performing_stock(stocks, count);
    if (best_stock) {
        fprintf(file, "        <section class=\"best-stock-section\">\n");
        fprintf(file, "            <div class=\"best-stock-card\">\n");
        fprintf(file, "                <h2>🏆 Stock of the Day</h2>\n");
        fprintf(file, "                <div class=\"stock-info\">\n");
        fprintf(file, "                    <div class=\"stock-symbol\">%s</div>\n", best_stock->symbol);
        fprintf(file, "                    <div class=\"company-name\">%s</div>\n", best_stock->name);
        fprintf(file, "                    <div class=\"stock-price\">$%.2f</div>\n", best_stock->current_price);
        fprintf(file, "                    <div class=\"stock-change %s\">%+.2f%%</div>\n", 
                best_stock->change_percent >= 0 ? "positive" : "negative", best_stock->change_percent);
        fprintf(file, "                    <div class=\"stock-status\">%s</div>\n", best_stock->status);
        fprintf(file, "                </div>\n");
        fprintf(file, "            </div>\n");
        fprintf(file, "        </section>\n\n");
    }
    
    // Market Summary
    int bullish_count = count_bullish_stocks(stocks, count);
    double avg_change = calculate_average_change(stocks, count);
    
    fprintf(file, "        <section class=\"market-summary\">\n");
    fprintf(file, "            <h2>📈 Market Overview</h2>\n");
    fprintf(file, "            <div class=\"summary-grid\">\n");
    fprintf(file, "                <div class=\"summary-card\">\n");
    fprintf(file, "                    <div class=\"summary-value\">%d/%d</div>\n", bullish_count, count);
    fprintf(file, "                    <div class=\"summary-label\">Bullish Stocks</div>\n");
    fprintf(file, "                </div>\n");
    fprintf(file, "                <div class=\"summary-card\">\n");
    fprintf(file, "                    <div class=\"summary-value\">%.2f%%</div>\n", avg_change);
    fprintf(file, "                    <div class=\"summary-label\">Avg Change</div>\n");
    fprintf(file, "                </div>\n");
    fprintf(file, "                <div class=\"summary-card\">\n");
    fprintf(file, "                    <div class=\"summary-value\">%s</div>\n", analyze_market_sentiment(stocks, count));
    fprintf(file, "                    <div class=\"summary-label\">Market Sentiment</div>\n");
    fprintf(file, "                </div>\n");
    fprintf(file, "            </div>\n");
    fprintf(file, "        </section>\n\n");
    
    // Stock Table
    fprintf(file, "        <section class=\"stocks-section\">\n");
    fprintf(file, "            <h2>📊 Live Stock Prices</h2>\n");
    fprintf(file, "            <div class=\"stocks-table\">\n");
    fprintf(file, "                <div class=\"table-header\">\n");
    fprintf(file, "                    <div>Symbol</div>\n");
    fprintf(file, "                    <div>Company</div>\n");
    fprintf(file, "                    <div>Price</div>\n");
    fprintf(file, "                    <div>Change</div>\n");
    fprintf(file, "                    <div>Volume</div>\n");
    fprintf(file, "                    <div>Status</div>\n");
    fprintf(file, "                </div>\n");
    
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0) {
            fprintf(file, "                <div class=\"table-row\">\n");
            fprintf(file, "                    <div class=\"symbol\">%s</div>\n", stocks[i].symbol);
            fprintf(file, "                    <div class=\"company\">%s</div>\n", stocks[i].name);
            fprintf(file, "                    <div class=\"price\">$%.2f</div>\n", stocks[i].current_price);
            fprintf(file, "                    <div class=\"change %s\">%+.2f%%</div>\n", 
                    stocks[i].change_percent >= 0 ? "positive" : "negative", stocks[i].change_percent);
            fprintf(file, "                    <div class=\"volume\">%.0f</div>\n", stocks[i].volume);
            fprintf(file, "                    <div class=\"status\">%s</div>\n", stocks[i].status);
            fprintf(file, "                </div>\n");
        }
    }
    
    fprintf(file, "            </div>\n");
    fprintf(file, "        </section>\n\n");
    
    // Footer
    fprintf(file, "        <footer class=\"footer\">\n");
    fprintf(file, "            <p>Last updated: <span id=\"lastUpdate\">%s</span></p>\n", "Now");
    fprintf(file, "            <p>Data provided by Smart Stock Tracker | Built with C</p>\n");
    fprintf(file, "        </footer>\n");
    fprintf(file, "    </div>\n\n");
    
    fprintf(file, "    <script src=\"app.js\"></script>\n");
    fprintf(file, "</body>\n");
    fprintf(file, "</html>\n");
    
    fclose(file);
    return 1;
}

// Create CSS styles
int create_css_styles() {
    FILE* file = fopen("web/style.css", "w");
    if (!file) {
        return 0;
    }
    
    fprintf(file, "/* Smart Stock Tracker - Styles */\n");
    fprintf(file, "* {\n");
    fprintf(file, "    margin: 0;\n");
    fprintf(file, "    padding: 0;\n");
    fprintf(file, "    box-sizing: border-box;\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, "body {\n");
    fprintf(file, "    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n");
    fprintf(file, "    background: linear-gradient(135deg, #667eea 0%%, #764ba2 100%%);\n");
    fprintf(file, "    min-height: 100vh;\n");
    fprintf(file, "    color: #333;\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".container {\n");
    fprintf(file, "    max-width: 1200px;\n");
    fprintf(file, "    margin: 0 auto;\n");
    fprintf(file, "    padding: 20px;\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".header {\n");
    fprintf(file, "    background: rgba(255, 255, 255, 0.95);\n");
    fprintf(file, "    padding: 30px;\n");
    fprintf(file, "    border-radius: 15px;\n");
    fprintf(file, "    margin-bottom: 30px;\n");
    fprintf(file, "    text-align: center;\n");
    fprintf(file, "    box-shadow: 0 10px 30px rgba(0, 0, 0, 0.1);\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".header h1 {\n");
    fprintf(file, "    font-size: 2.5em;\n");
    fprintf(file, "    margin-bottom: 10px;\n");
    fprintf(file, "    color: #2c3e50;\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".live-indicator {\n");
    fprintf(file, "    display: flex;\n");
    fprintf(file, "    align-items: center;\n");
    fprintf(file, "    justify-content: center;\n");
    fprintf(file, "    gap: 8px;\n");
    fprintf(file, "    color: #27ae60;\n");
    fprintf(file, "    font-weight: bold;\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".status-dot {\n");
    fprintf(file, "    width: 12px;\n");
    fprintf(file, "    height: 12px;\n");
    fprintf(file, "    background: #27ae60;\n");
    fprintf(file, "    border-radius: 50%%;\n");
    fprintf(file, "    animation: pulse 2s infinite;\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, "@keyframes pulse {\n");
    fprintf(file, "    0%% { opacity: 1; }\n");
    fprintf(file, "    50%% { opacity: 0.5; }\n");
    fprintf(file, "    100%% { opacity: 1; }\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".best-stock-card {\n");
    fprintf(file, "    background: linear-gradient(135deg, #f093fb 0%%, #f5576c 100%%);\n");
    fprintf(file, "    color: white;\n");
    fprintf(file, "    padding: 30px;\n");
    fprintf(file, "    border-radius: 15px;\n");
    fprintf(file, "    margin-bottom: 30px;\n");
    fprintf(file, "    text-align: center;\n");
    fprintf(file, "    box-shadow: 0 15px 35px rgba(240, 147, 251, 0.3);\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".stocks-table {\n");
    fprintf(file, "    background: rgba(255, 255, 255, 0.95);\n");
    fprintf(file, "    border-radius: 15px;\n");
    fprintf(file, "    overflow: hidden;\n");
    fprintf(file, "    box-shadow: 0 10px 30px rgba(0, 0, 0, 0.1);\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".table-header, .table-row {\n");
    fprintf(file, "    display: grid;\n");
    fprintf(file, "    grid-template-columns: 1fr 2fr 1fr 1fr 1.5fr 1.5fr;\n");
    fprintf(file, "    padding: 15px 20px;\n");
    fprintf(file, "    border-bottom: 1px solid #eee;\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".table-header {\n");
    fprintf(file, "    background: #2c3e50;\n");
    fprintf(file, "    color: white;\n");
    fprintf(file, "    font-weight: bold;\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, ".positive { color: #27ae60; }\n");
    fprintf(file, ".negative { color: #e74c3c; }\n\n");
    
    fprintf(file, ".footer {\n");
    fprintf(file, "    text-align: center;\n");
    fprintf(file, "    margin-top: 30px;\n");
    fprintf(file, "    color: rgba(255, 255, 255, 0.8);\n");
    fprintf(file, "}\n");
    
    fclose(file);
    return 1;
}

// Create JavaScript code
int create_javascript_code() {
    FILE* file = fopen("web/app.js", "w");
    if (!file) {
        return 0;
    }
    
    fprintf(file, "// Smart Stock Tracker - JavaScript\n\n");
    fprintf(file, "// Update timestamp\n");
    fprintf(file, "function updateTimestamp() {\n");
    fprintf(file, "    const now = new Date();\n");
    fprintf(file, "    const timestamp = now.toLocaleString();\n");
    fprintf(file, "    const element = document.getElementById('lastUpdate');\n");
    fprintf(file, "    if (element) {\n");
    fprintf(file, "        element.textContent = timestamp;\n");
    fprintf(file, "    }\n");
    fprintf(file, "}\n\n");
    
    fprintf(file, "// Initialize page\n");
    fprintf(file, "document.addEventListener('DOMContentLoaded', function() {\n");
    fprintf(file, "    updateTimestamp();\n");
    fprintf(file, "    \n");
    fprintf(file, "    // Update timestamp every minute\n");
    fprintf(file, "    setInterval(updateTimestamp, 60000);\n");
    fprintf(file, "    \n");
    fprintf(file, "    console.log('📊 Smart Stock Tracker initialized');\n");
    fprintf(file, "});\n\n");
    
    fprintf(file, "// Add hover effects\n");
    fprintf(file, "document.addEventListener('DOMContentLoaded', function() {\n");
    fprintf(file, "    const rows = document.querySelectorAll('.table-row');\n");
    fprintf(file, "    \n");
    fprintf(file, "    rows.forEach(row => {\n");
    fprintf(file, "        row.addEventListener('mouseenter', function() {\n");
    fprintf(file, "            this.style.backgroundColor = '#f8f9fa';\n");
    fprintf(file, "            this.style.transform = 'scale(1.02)';\n");
    fprintf(file, "            this.style.transition = 'all 0.2s ease';\n");
    fprintf(file, "        });\n");
    fprintf(file, "        \n");
    fprintf(file, "        row.addEventListener('mouseleave', function() {\n");
    fprintf(file, "            this.style.backgroundColor = '';\n");
    fprintf(file, "            this.style.transform = '';\n");
    fprintf(file, "        });\n");
    fprintf(file, "    });\n");
    fprintf(file, "});\n");
    
    fclose(file);
    return 1;
}

// Generate complete web dashboard
int generate_web_dashboard(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return 0;
    }
    
    // Create all web files
    if (!create_html_dashboard(stocks, count)) {
        display_error("Failed to create HTML dashboard");
        return 0;
    }
    
    if (!create_css_styles()) {
        display_error("Failed to create CSS styles");
        return 0;
    }
    
    if (!create_javascript_code()) {
        display_error("Failed to create JavaScript code");
        return 0;
    }
    
    if (!generate_json_file(stocks, count, "web/stock_data.json")) {
        display_error("Failed to create JSON data file");
        return 0;
    }
    
    return 1;
}
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
// int save_stocks_to_file(Stock stocks[], int count, const char* filename) {
//     if (!stocks || !filename || count <= 0) {
//         display_error("Invalid parameters for saving stocks");
//         return 0;
//     }
    
//     FILE* file = fopen(filename, "w");
//     if (!file) {
//         char error_msg[256];
//         snprintf(error_msg, sizeof(error_msg), "Cannot open file '%s' for writing", filename);
//         display_error(error_msg);
//         return 0;
//     }
    
//     // Write header
//     fprintf(file, "# Smart Stock Tracker - Stock Data Export\n");
//     fprintf(file, "# Generated on: ");
    
//     time_t now = time(NULL);
//     char timestamp[64];
//     strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
//     fprintf(file, "%s\n", timestamp);
    
//     fprintf(file, "# Format: Symbol, Name, Price, Change%%, Volume, Status\n");
//     fprintf(file, "========================================\n\n");
    
//     // Write stock data
//     for (int i = 0; i < count; i++) {
//         if (stocks[i].current_price > 0) {
//             fprintf(file, "%-8s | %-30s | $%-10.2f | %+7.2f%% | %-12.0f | %s\n",
//                     stocks[i].symbol,
//                     stocks[i].name,
//                     stocks[i].current_price,
//                     stocks[i].change_percent,
//                     stocks[i].volume,
//                     stocks[i].status);
//         }
//     }
    
//     fprintf(file, "\n========================================\n");
//     fprintf(file, "Total stocks processed: %d\n", count);
    
//     fclose(file);
//     return 1;
// }

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
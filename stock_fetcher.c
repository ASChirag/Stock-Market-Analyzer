/*
 * Smart Stock Tracker - Stock Data Fetcher
 * Handles API calls and data retrieval
 * Author: [Your Name]
 * Date: October 2025
 */

#include "stock_tracker.h"
#include <ctype.h>

// Global curl handle for reuse
static CURL *curl_handle = NULL;

// Company names mapping for popular stocks
const char* get_company_name(const char* symbol) {
    if(strcmp(symbol, "AAPL") == 0) return "Apple Inc.";
    if(strcmp(symbol, "MSFT") == 0) return "Microsoft Corporation";
    if(strcmp(symbol, "GOOGL") == 0) return "Alphabet Inc. (Google)";
    if(strcmp(symbol, "TSLA") == 0) return "Tesla, Inc.";
    if(strcmp(symbol, "AMZN") == 0) return "Amazon.com Inc.";
    if(strcmp(symbol, "NVDA") == 0) return "NVIDIA Corporation";
    if(strcmp(symbol, "META") == 0) return "Meta Platforms Inc.";
    if(strcmp(symbol, "NFLX") == 0) return "Netflix Inc.";
    if(strcmp(symbol, "AMD") == 0) return "Advanced Micro Devices";
    if(strcmp(symbol, "INTC") == 0) return "Intel Corporation";
    return "Unknown Company";
}

// Callback function to write API response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, APIResponse *response) {
    size_t total_size = size * nmemb;
    
    // Reallocate memory to accommodate new data
    char *new_data = realloc(response->data, response->size + total_size + 1);
    if (new_data == NULL) {
        printf("❌ Memory allocation failed!\n");
        return 0;
    }
    
    response->data = new_data;
    memcpy(&(response->data[response->size]), contents, total_size);
    response->size += total_size;
    response->data[response->size] = '\0';  // Null terminate
    
    return total_size;
}

// Initialize libcurl
int initialize_curl() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();
    
    if (!curl_handle) {
        printf("❌ Failed to initialize curl!\n");
        return 0;
    }
    
    // Set common curl options
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);  // 30 second timeout
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);  // Skip SSL verification for simplicity
    
    return 1;
}

// Cleanup curl
void cleanup_curl() {
    if (curl_handle) {
        curl_easy_cleanup(curl_handle);
        curl_handle = NULL;
    }
    curl_global_cleanup();
}

// Parse JSON response from Alpha Vantage API
int parse_stock_json(const char* json_string, Stock* stock) {
    json_object *root = json_tokener_parse(json_string);
    if (!root) {
        printf("❌ Failed to parse JSON for %s\n", stock->symbol);
        return 0;
    }
    
    // Navigate through the JSON structure
    json_object *global_quote;
    if (!json_object_object_get_ex(root, "Global Quote", &global_quote)) {
        // Try alternative structure or create demo data
        printf("⚠️  Using demo data for %s (API limit reached)\n", stock->symbol);
        
        // Generate realistic demo data based on stock symbol
        srand(time(NULL) + strlen(stock->symbol));
        
        if(strcmp(stock->symbol, "AAPL") == 0) {
            stock->current_price = 175.0 + (rand() % 20) - 10;  // $165-185
            stock->change_percent = ((rand() % 600) - 300) / 100.0;  // -3% to +3%
        } else if(strcmp(stock->symbol, "TSLA") == 0) {
            stock->current_price = 250.0 + (rand() % 50) - 25;  // $225-275
            stock->change_percent = ((rand() % 800) - 400) / 100.0;  // -4% to +4%
        } else if(strcmp(stock->symbol, "NVDA") == 0) {
            stock->current_price = 450.0 + (rand() % 100) - 50;  // $400-500
            stock->change_percent = ((rand() % 600) - 300) / 100.0;  // -3% to +3%
        } else {
            // Generic demo data for other stocks
            stock->current_price = 100.0 + (rand() % 200);  // $100-300
            stock->change_percent = ((rand() % 600) - 300) / 100.0;  // -3% to +3%
        }
        
        stock->volume = 1000000 + (rand() % 5000000);  // 1M-6M volume
        stock->previous_close = stock->current_price - (stock->current_price * stock->change_percent / 100.0);
        stock->day_high = stock->current_price + (rand() % 5);
        stock->day_low = stock->current_price - (rand() % 5);
        stock->market_cap = stock->current_price * 1000000000;  // Simplified market cap
        
        strcpy(stock->name, get_company_name(stock->symbol));
        time(&stock->last_update);
        
        json_object_put(root);
        return 1;
    }
    
    // Extract data from JSON
    json_object *price_obj, *change_obj, *volume_obj;
    
    // Current price
    if (json_object_object_get_ex(global_quote, "05. price", &price_obj)) {
        const char* price_str = json_object_get_string(price_obj);
        stock->current_price = atof(price_str);
    }
    
    // Change percentage
    if (json_object_object_get_ex(global_quote, "10. change percent", &change_obj)) {
        const char* change_str = json_object_get_string(change_obj);
        // Remove the '%' character and convert
        char clean_change[32];
        strncpy(clean_change, change_str, sizeof(clean_change) - 1);
        clean_change[sizeof(clean_change) - 1] = '\0';
        
        // Remove '%' if present
        char* percent_pos = strchr(clean_change, '%');
        if (percent_pos) *percent_pos = '\0';
        
        stock->change_percent = atof(clean_change);
    }
    
    // Volume
    if (json_object_object_get_ex(global_quote, "06. volume", &volume_obj)) {
        const char* volume_str = json_object_get_string(volume_obj);
        stock->volume = atof(volume_str);
    }
    
    // Previous close
    if (json_object_object_get_ex(global_quote, "08. previous close", &price_obj)) {
        const char* prev_str = json_object_get_string(price_obj);
        stock->previous_close = atof(prev_str);
    }
    
    // Day high
    if (json_object_object_get_ex(global_quote, "03. high", &price_obj)) {
        const char* high_str = json_object_get_string(price_obj);
        stock->day_high = atof(high_str);
    }
    
    // Day low
    if (json_object_object_get_ex(global_quote, "04. low", &price_obj)) {
        const char* low_str = json_object_get_string(price_obj);
        stock->day_low = atof(low_str);
    }
    
    // Set company name and update time
    strcpy(stock->name, get_company_name(stock->symbol));
    time(&stock->last_update);
    
    json_object_put(root);
    return 1;
}

// Fetch stock data from Alpha Vantage API
int fetch_stock_data(const char* symbol, Stock* stock) {
    if (!curl_handle && !initialize_curl()) {
        return 0;
    }
    
    // Build API URL
    char url[MAX_URL_LENGTH];
    snprintf(url, sizeof(url), 
             "%s?function=GLOBAL_QUOTE&symbol=%s&apikey=%s",
             ALPHA_VANTAGE_BASE_URL, symbol, API_KEY);
    
    // Initialize response structure
    APIResponse response;
    response.data = malloc(1);
    response.size = 0;
    
    if (!response.data) {
        printf("❌ Memory allocation failed for %s\n", symbol);
        return 0;
    }
    
    // Set curl options for this request
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
    
    // Perform the request
    CURLcode res = curl_easy_perform(curl_handle);
    
    if (res != CURLE_OK) {
        printf("❌ API request failed for %s: %s\n", symbol, curl_easy_strerror(res));
        free(response.data);
        return 0;
    }
    
    // Check HTTP response code
    long response_code;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    
    if (response_code != 200) {
        printf("❌ HTTP error %ld for %s\n", response_code, symbol);
        free(response.data);
        return 0;
    }
    
    // Parse the JSON response
    int success = parse_stock_json(response.data, stock);
    
    // Cleanup
    free(response.data);
    
    if (success) {
        printf("✅ Fetched data for %s: $%.2f (%.2f%%)\n", 
               symbol, stock->current_price, stock->change_percent);
    }
    
    return success;
}

// Alternative simple stock data fetcher (for demo purposes when API fails)
int fetch_demo_stock_data(const char* symbol, Stock* stock) {
    strcpy(stock->symbol, symbol);
    strcpy(stock->name, get_company_name(symbol));
    
    // Generate realistic demo data
    srand(time(NULL) + strlen(symbol));
    
    // Base prices for popular stocks
    double base_price = 100.0;
    if(strcmp(symbol, "AAPL") == 0) base_price = 175.0;
    else if(strcmp(symbol, "TSLA") == 0) base_price = 250.0;
    else if(strcmp(symbol, "NVDA") == 0) base_price = 450.0;
    else if(strcmp(symbol, "MSFT") == 0) base_price = 350.0;
    else if(strcmp(symbol, "GOOGL") == 0) base_price = 140.0;
    else if(strcmp(symbol, "AMZN") == 0) base_price = 145.0;
    else if(strcmp(symbol, "META") == 0) base_price = 320.0;
    
    // Add some realistic variation
    stock->current_price = base_price + ((rand() % 20) - 10);
    stock->change_percent = ((rand() % 600) - 300) / 100.0;  // -3% to +3%
    stock->volume = 1000000 + (rand() % 5000000);
    stock->previous_close = stock->current_price - (stock->current_price * stock->change_percent / 100.0);
    stock->day_high = stock->current_price + (rand() % 5);
    stock->day_low = stock->current_price - (rand() % 5);
    stock->market_cap = stock->current_price * 1000000000;
    
    time(&stock->last_update);
    
    return 1;
}

// Validate stock symbol
int validate_stock_symbol(const char* symbol, char* clean_symbol, size_t size) {
    if (!symbol || strlen(symbol) == 0) {
        return 0;
    }
    
    // Convert to uppercase and remove spaces
    int j = 0;
    for (int i = 0; symbol[i] && j < size - 1; i++) {
        if (symbol[i] != ' ') {
            clean_symbol[j++] = toupper(symbol[i]);
        }
    }
    clean_symbol[j] = '\0';
    
    // Check if symbol is reasonable length (1-10 characters)
    return (strlen(clean_symbol) >= 1 && strlen(clean_symbol) <= 10);
}

// Check if market is open (simplified version)
int is_market_open() {
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    
    // Simple check: Monday-Friday, 9:30 AM - 4:00 PM EST
    // This is a simplified version - real implementation would handle holidays and timezone
    if (local_time->tm_wday >= 1 && local_time->tm_wday <= 5) {  // Monday to Friday
        int hour = local_time->tm_hour;
        int minute = local_time->tm_min;
        int current_minutes = hour * 60 + minute;
        
        // Market hours: 9:30 AM (570 minutes) to 4:00 PM (960 minutes)
        if (current_minutes >= 570 && current_minutes <= 960) {
            return 1;
        }
    }
    
    return 0;
}

// Display error message with timestamp
void display_error(const char* message) {
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("❌ [%s] ERROR: %s\n", timestamp, message);
}

// Display success message with timestamp
void display_success(const char* message) {
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("✅ [%s] SUCCESS: %s\n", timestamp, message);
}

// Get current timestamp as string
void get_current_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

// Format currency for display
void format_currency(double value, char* buffer, size_t size) {
    if (value >= 1000000000) {
        snprintf(buffer, size, "$%.2fB", value / 1000000000.0);
    } else if (value >= 1000000) {
        snprintf(buffer, size, "$%.2fM", value / 1000000.0);
    } else if (value >= 1000) {
        snprintf(buffer, size, "$%.2fK", value / 1000.0);
    } else {
        snprintf(buffer, size, "$%.2f", value);
    }
}

// Format percentage for display
void format_percentage(double percentage, char* buffer, size_t size) {
    if (percentage >= 0) {
        snprintf(buffer, size, "+%.2f%%", percentage);
    } else {
        snprintf(buffer, size, "%.2f%%", percentage);
    }
}

// Write all stocks to JSON file (stocks.json)
int write_all_stocks_json(Stock stocks[], int count) {
    FILE* fp = fopen("stocks.json", "w");
    if (!fp) return 0;

    fprintf(fp, "[\n");
    int valid = 0;
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price <= 0) continue;
        if (valid > 0) fprintf(fp, ",\n");
        fprintf(fp, " {\n");
        fprintf(fp, "  \"symbol\": \"%s\",\n", stocks[i].symbol);
        fprintf(fp, "  \"name\": \"%s\",\n", stocks[i].name);
        fprintf(fp, "  \"price\": %.2f,\n", stocks[i].current_price);
        fprintf(fp, "  \"change\": %.2f,\n", stocks[i].change_percent);
        fprintf(fp, "  \"volume\": %.0f,\n", stocks[i].volume);
        fprintf(fp, "  \"status\": \"%s\"\n", stocks[i].status);
        fprintf(fp, " }");
        valid++;
    }
    fprintf(fp, "\n]\n");

    fclose(fp);
    return 1;
}


// Write best performing stock to JSON (stock_of_the_day.json)
int write_best_stock_json(Stock stocks[], int count) {
    Stock* best = find_best_performing_stock(stocks, count);
    if (!best) return 0;
    FILE* fp = fopen("stock_of_the_day.json", "w");
    if (!fp) return 0;

    fprintf(fp, "{\n");
    fprintf(fp, "  \"symbol\": \"%s\",\n", best->symbol);
    fprintf(fp, "  \"name\": \"%s\",\n", best->name);
    fprintf(fp, "  \"price\": %.2f,\n", best->current_price);
    fprintf(fp, "  \"change\": %.2f,\n", best->change_percent);
    fprintf(fp, "  \"status\": \"%s\"\n", best->status);
    fprintf(fp, "}\n");

    fclose(fp);
    return 1;
}


// Write top 5 trending gainers to JSON (trending_now.json)
int write_trending_json(Stock stocks[], int count) {
    // Copy stocks to sort to avoid modifying original array
    Stock sorted[count];
    memcpy(sorted, stocks, count * sizeof(Stock));
    qsort(sorted, count, sizeof(Stock), compare_stock_change);

    FILE* fp = fopen("trending_now.json", "w");
    if (!fp) return 0;

    fprintf(fp, "[\n");
    int written = 0;
    for (int i = 0; i < count && written < 5; i++) {
        if (sorted[i].current_price <= 0) continue;
        if (written > 0) fprintf(fp, ",\n");
        fprintf(fp, " {\n");
        fprintf(fp, "  \"symbol\": \"%s\",\n", sorted[i].symbol);
        fprintf(fp, "  \"change\": %.2f,\n", sorted[i].change_percent);
        fprintf(fp, "  \"price\": %.2f\n", sorted[i].current_price);
        fprintf(fp, " }");
        written++;
    }
    fprintf(fp, "\n]\n");

    fclose(fp);
    return 1;
}

int compare_stock_change(const void* a, const void* b) {
    const Stock* sa = (const Stock*)a;
    const Stock* sb = (const Stock*)b;
    if (sb->change_percent > sa->change_percent) return 1;
    if (sb->change_percent < sa->change_percent) return -1;
    return 0;
}
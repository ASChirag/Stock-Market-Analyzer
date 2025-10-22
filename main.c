/*
 * Smart Stock Tracker - Main Program
 * Real-time Stock Market Analysis System
 * Author: [Your Name]
 * Date: October 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "stock_tracker.h"

// Popular stocks to track
const char* DEFAULT_STOCKS[] = {
    "AAPL",  // Apple
    "MSFT",  // Microsoft  
    "GOOGL", // Google
    "TSLA",  // Tesla
    "AMZN",  // Amazon
    "NVDA",  // NVIDIA
    "META",  // Meta (Facebook)
    "NFLX",  // Netflix
    "AMD",   // AMD
    "INTC"   // Intel
};

const int STOCK_COUNT = 10;

void print_header() {
    system("clear"); // Clear screen (use "cls" on Windows)
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                 📊 SMART STOCK TRACKER 📊                ║\n");
    printf("║                Real-Time Market Analysis                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Display current time
    time_t now = time(0);
    char* dt = ctime(&now);
    printf("🕒 Last Updated: %s", dt);
    printf("🌐 Status: 🟢 LIVE DATA\n\n");
}

void print_loading_animation(const char* message) {
    printf("%s", message);
    fflush(stdout);
    
    for(int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
        sleep(1);
    }
    printf(" ✅\n");
}

void display_best_stock(Stock* best_stock) {
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    🏆 STOCK OF THE DAY 🏆                ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ Symbol: %-10s                                        ║\n", best_stock->symbol);
    printf("║ Company: %-45s        ║\n", best_stock->name);
    printf("║ Price: $%-8.2f                                       ║\n", best_stock->current_price);
    printf("║ Change: %s%-6.2f%%                                    ║\n", 
           best_stock->change_percent >= 0 ? "📈 +" : "📉 ", 
           best_stock->change_percent);
    printf("║ Status: %-45s        ║\n", best_stock->status);
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
}

void display_stock_table(Stock stocks[], int count) {
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                            📈 LIVE STOCK PRICES 📈                      ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ %-6s │ %-12s │ %-10s │ %-8s │ %-15s ║\n", 
           "SYMBOL", "PRICE", "CHANGE %", "VOLUME", "STATUS");
    printf("╠══════════════════════════════════════════════════════════════════════════╣\n");
    
    for(int i = 0; i < count; i++) {
        if(stocks[i].current_price > 0) { // Only display valid data
            printf("║ %-6s │ $%-11.2f │ %s%-7.2f%% │ %-8.0f │ %-15s ║\n",
                   stocks[i].symbol,
                   stocks[i].current_price,
                   stocks[i].change_percent >= 0 ? "+" : "",
                   stocks[i].change_percent,
                   stocks[i].volume,
                   stocks[i].status);
        }
    }
    
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n\n");
}

void display_trending_stocks(Stock stocks[], int count) {
    printf("🔥 TRENDING NOW (Top Gainers):\n");
    printf("═══════════════════════════════\n");
    
    // Sort stocks by change percentage (simple bubble sort)
    for(int i = 0; i < count - 1; i++) {
        for(int j = 0; j < count - i - 1; j++) {
            if(stocks[j].change_percent < stocks[j + 1].change_percent) {
                Stock temp = stocks[j];
                stocks[j] = stocks[j + 1];
                stocks[j + 1] = temp;
            }
        }
    }
    
    for(int i = 0; i < 5 && i < count; i++) {
        if(stocks[i].current_price > 0) {
            printf("%d. %s %s %.2f%% ($%.2f)\n", 
                   i + 1, 
                   stocks[i].symbol,
                   stocks[i].change_percent >= 0 ? "📈" : "📉",
                   stocks[i].change_percent,
                   stocks[i].current_price);
        }
    }
    printf("\n");
}

void show_menu() {
    printf("┌─────────────────────────────────────────┐\n");
    printf("│               MAIN MENU                 │\n");
    printf("├─────────────────────────────────────────┤\n");
    printf("│ 1. 🔄 Refresh Stock Data                │\n");
    printf("│ 2. 📊 View Detailed Analysis            │\n");
    printf("│ 5. ❌ Exit                              │\n");
    printf("└─────────────────────────────────────────┘\n");
    printf("Enter your choice (1-5): ");
}

int main() {
    Stock stocks[STOCK_COUNT];
    int choice;
    int data_loaded = 0;
    
    print_header();
    
    printf("🚀 Initializing Smart Stock Tracker...\n\n");
    
    // Initialize stock array
    for(int i = 0; i < STOCK_COUNT; i++) {
        strcpy(stocks[i].symbol, DEFAULT_STOCKS[i]);
        stocks[i].current_price = 0.0;
        stocks[i].change_percent = 0.0;
        stocks[i].volume = 0.0;
        strcpy(stocks[i].name, "Loading...");
        strcpy(stocks[i].status, "FETCHING");
    }
    
    do {
        show_menu();
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                print_header();
                print_loading_animation("🔄 Fetching real-time stock data");
                print_loading_animation("📡 Connecting to market APIs");
                print_loading_animation("📊 Processing market data");
                
                // Fetch stock data
                int successful_fetches = 0;
                for(int i = 0; i < STOCK_COUNT; i++) {
                    if(fetch_stock_data(stocks[i].symbol, &stocks[i])) {
                        analyze_stock_performance(&stocks[i]);
                        successful_fetches++;
                    }
                }
                
                if(successful_fetches > 0) {
                    data_loaded = 1;
                    print_header();
                    
                    // Find and display best stock
                    Stock* best_stock = find_best_performing_stock(stocks, STOCK_COUNT);
                    if(best_stock != NULL) {
                        display_best_stock(best_stock);
                    }
                    
                    // Display all stocks
                    display_stock_table(stocks, STOCK_COUNT);
                    
                    // Show trending stocks
                    display_trending_stocks(stocks, STOCK_COUNT);

                    write_all_stocks_json(stocks, STOCK_COUNT);
                    write_best_stock_json(stocks, STOCK_COUNT);
                    write_trending_json(stocks, STOCK_COUNT);
                    
                    printf("✅ Successfully loaded %d stocks!\n\n", successful_fetches);
                } else {
                    printf("❌ Failed to fetch stock data. Please check your internet connection.\n\n");
                }
                break;
                
            case 2:
                if(!data_loaded) {
                    printf("⚠️  Please fetch stock data first (Option 1).\n\n");
                } else {
                    print_header();
                    printf("📊 DETAILED MARKET ANALYSIS\n");
                    printf("══════════════════════════════\n\n");
                    
                    double total_market_cap = calculate_total_value(stocks, STOCK_COUNT);
                    printf("💰 Total Portfolio Value: $%.2f\n", total_market_cap);
                    
                    int bullish_count = count_bullish_stocks(stocks, STOCK_COUNT);
                    printf("📈 Bullish Stocks: %d/%d\n", bullish_count, STOCK_COUNT);
                    
                    Stock* most_volatile = find_most_volatile_stock(stocks, STOCK_COUNT);
                    if(most_volatile != NULL) {
                        printf("⚡ Most Volatile: %s (%.2f%%)\n", 
                               most_volatile->symbol, most_volatile->change_percent);
                    }
                    printf("\n");
                }
                break;
                
            // case 3:
            //     if(!data_loaded) {
            //         printf("⚠️  Please fetch stock data first (Option 1).\n\n");
            //     } else {
            //         print_loading_animation("💾 Saving stock data to file");
            //         if(save_stocks_to_file(stocks, STOCK_COUNT, "stock_data.txt")) {
            //             printf("✅ Data saved successfully to 'stock_data.txt'\n\n");
            //         } else {
            //             printf("❌ Failed to save data to file.\n\n");
            //         }
            //     }
            //     break;
                
            // case 4:
            //     if(!data_loaded) {
            //         printf("⚠️  Please fetch stock data first (Option 1).\n\n");
            //     } else {
            //         print_loading_animation("🌐 Generating web dashboard");
            //         if(generate_web_dashboard(stocks, STOCK_COUNT)) {
            //             printf("✅ Web dashboard generated successfully!\n");
            //             printf("🌐 Open 'web/index.html' in your browser to view.\n\n");
            //         } else {
            //             printf("❌ Failed to generate web dashboard.\n\n");
            //         }
            //     }
            //     break;
                
            case 5:
                printf("\n👋 Thank you for using Smart Stock Tracker!\n");
                printf("📊 Stay informed, invest wisely! 💰\n\n");
                break;
                
            default:
                printf("❌ Invalid choice. Please select 1-5.\n\n");
        }
        
        if(choice != 5) {
            printf("Press Enter to continue...");
            getchar(); // Clear buffer
            getchar(); // Wait for Enter
        }
        
    } while(choice != 5);
    
    return 0;
}
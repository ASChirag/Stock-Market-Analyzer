/*
 * Smart Stock Tracker - Stock Analyzer
 * Analysis and recommendation algorithms
 * Author: [Your Name]
 * Date: October 2025
 */

#include "stock_tracker.h"

// Analyze individual stock performance and set status
void analyze_stock_performance(Stock* stock) {
    if (!stock || stock->current_price <= 0) {
        strcpy(stock->status, "❌ INVALID");
        return;
    }
    
    double change = stock->change_percent;
    
    // Categorize based on performance thresholds
    if (change >= STRONG_BUY_THRESHOLD) {
        strcpy(stock->status, "🚀 STRONG BUY");
    } else if (change >= BUY_THRESHOLD) {
        strcpy(stock->status, "📈 BULLISH");
    } else if (change > 0) {
        strcpy(stock->status, "🟢 POSITIVE");
    } else if (change == 0) {
        strcpy(stock->status, "⚪ NEUTRAL");
    } else if (change > SELL_THRESHOLD) {
        strcpy(stock->status, "🟡 WATCH");
    } else if (change > STRONG_SELL_THRESHOLD) {
        strcpy(stock->status, "📉 BEARISH");
    } else {
        strcpy(stock->status, "🔴 AVOID");
    }
}

// Find the best performing stock
Stock* find_best_performing_stock(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return NULL;
    }
    
    Stock* best = NULL;
    double best_performance = -1000.0;  // Start with very low number
    
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0 && stocks[i].change_percent > best_performance) {
            best = &stocks[i];
            best_performance = stocks[i].change_percent;
        }
    }
    
    return best;
}

// Find the most volatile stock (highest absolute change)
Stock* find_most_volatile_stock(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return NULL;
    }
    
    Stock* most_volatile = NULL;
    double highest_volatility = 0.0;
    
    for (int i = 0; i < count; i++) {
        double abs_change = fabs(stocks[i].change_percent);
        if (stocks[i].current_price > 0 && abs_change > highest_volatility) {
            most_volatile = &stocks[i];
            highest_volatility = abs_change;
        }
    }
    
    return most_volatile;
}

// Count bullish stocks (positive change)
int count_bullish_stocks(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return 0;
    }
    
    int bullish_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0 && stocks[i].change_percent > 0) {
            bullish_count++;
        }
    }
    
    return bullish_count;
}

// Calculate total portfolio value
double calculate_total_value(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return 0.0;
    }
    
    double total = 0.0;
    
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0) {
            // Assuming 1 share of each stock for simplicity
            total += stocks[i].current_price;
        }
    }
    
    return total;
}

// Sort stocks by performance (bubble sort for simplicity)
void sort_stocks_by_performance(Stock stocks[], int count) {
    if (!stocks || count <= 1) {
        return;
    }
    
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (stocks[j].change_percent < stocks[j + 1].change_percent) {
                // Swap stocks
                Stock temp = stocks[j];
                stocks[j] = stocks[j + 1];
                stocks[j + 1] = temp;
            }
        }
    }
}

// Generate detailed recommendation
const char* generate_recommendation(Stock* stock) {
    if (!stock || stock->current_price <= 0) {
        return "INVALID DATA";
    }
    
    double change = stock->change_percent;
    double price = stock->current_price;
    double volume = stock->volume;
    
    // Multi-factor analysis
    static char recommendation[200];
    
    if (change >= 3.0 && volume > 1000000) {
        strcpy(recommendation, "STRONG BUY - High momentum with strong volume");
    } else if (change >= 1.0 && volume > 500000) {
        strcpy(recommendation, "BUY - Positive trend with good volume");
    } else if (change >= 0.5) {
        strcpy(recommendation, "HOLD - Slight upward movement");
    } else if (change >= -0.5) {
        strcpy(recommendation, "HOLD - Minimal movement, watch closely");
    } else if (change >= -2.0) {
        strcpy(recommendation, "WATCH - Declining, consider exit strategy");
    } else if (change >= -5.0) {
        strcpy(recommendation, "SELL - Significant decline, limit losses");
    } else {
        strcpy(recommendation, "STRONG SELL - Major decline, exit immediately");
    }
    
    return recommendation;
}

// Calculate moving average (simplified)
double calculate_simple_moving_average(double prices[], int count, int period) {
    if (!prices || count < period || period <= 0) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (int i = count - period; i < count; i++) {
        sum += prices[i];
    }
    
    return sum / period;
}

// Calculate relative strength index (RSI) - simplified version
double calculate_rsi(Stock* stock) {
    if (!stock || stock->current_price <= 0) {
        return 50.0;  // Neutral RSI
    }
    
    // Simplified RSI calculation based on current change
    double change = stock->change_percent;
    
    if (change > 0) {
        return 50.0 + (change * 5.0);  // Scale positive changes
    } else {
        return 50.0 + (change * 5.0);  // Scale negative changes
    }
}

// Detect price patterns (simplified)
const char* detect_price_pattern(Stock* stock) {
    if (!stock || stock->current_price <= 0) {
        return "UNKNOWN";
    }
    
    double current = stock->current_price;
    double high = stock->day_high;
    double low = stock->day_low;
    double change = stock->change_percent;
    
    // Simple pattern detection
    if (change > 2.0 && current > (high + low) / 2) {
        return "BULLISH BREAKOUT";
    } else if (change < -2.0 && current < (high + low) / 2) {
        return "BEARISH BREAKDOWN";
    } else if (fabs(change) < 0.5) {
        return "SIDEWAYS TREND";
    } else if (change > 0) {
        return "UPWARD TREND";
    } else {
        return "DOWNWARD TREND";
    }
}

// Calculate support and resistance levels
void calculate_support_resistance(Stock* stock, double* support, double* resistance) {
    if (!stock || !support || !resistance) {
        return;
    }
    
    // Simplified calculation based on day high/low
    *support = stock->day_low * 0.95;    // 5% below day low
    *resistance = stock->day_high * 1.05; // 5% above day high
}

// Market sentiment analysis
const char* analyze_market_sentiment(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return "UNKNOWN";
    }
    
    int bullish = 0, bearish = 0, neutral = 0;
    
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0) {
            if (stocks[i].change_percent > 1.0) {
                bullish++;
            } else if (stocks[i].change_percent < -1.0) {
                bearish++;
            } else {
                neutral++;
            }
        }
    }
    
    if (bullish > bearish && bullish > neutral) {
        return "🟢 BULLISH MARKET";
    } else if (bearish > bullish && bearish > neutral) {
        return "🔴 BEARISH MARKET";
    } else {
        return "🟡 NEUTRAL MARKET";
    }
}

// Risk assessment
const char* assess_risk_level(Stock* stock) {
    if (!stock || stock->current_price <= 0) {
        return "UNKNOWN";
    }
    
    double abs_change = fabs(stock->change_percent);
    
    if (abs_change >= 5.0) {
        return "🔴 HIGH RISK";
    } else if (abs_change >= 2.0) {
        return "🟡 MEDIUM RISK";
    } else {
        return "🟢 LOW RISK";
    }
}

// Portfolio diversification analysis
double calculate_portfolio_diversity(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return 0.0;
    }
    
    // Simple diversity metric based on correlation of price movements
    double positive_count = 0, negative_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0) {
            if (stocks[i].change_percent > 0) {
                positive_count++;
            } else {
                negative_count++;
            }
        }
    }
    
    double total = positive_count + negative_count;
    if (total == 0) return 0.0;
    
    // Higher diversity when stocks are moving in different directions
    double ratio = (positive_count < negative_count) ? 
                   positive_count / total : negative_count / total;
    
    return ratio * 100.0;  // Return as percentage
}

// Find stocks with unusual volume
Stock* find_unusual_volume_stock(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return NULL;
    }
    
    Stock* unusual_stock = NULL;
    double highest_volume = 0.0;
    
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0 && stocks[i].volume > highest_volume) {
            unusual_stock = &stocks[i];
            highest_volume = stocks[i].volume;
        }
    }
    
    return unusual_stock;
}

// Calculate average price change
double calculate_average_change(Stock stocks[], int count) {
    if (!stocks || count <= 0) {
        return 0.0;
    }
    
    double total_change = 0.0;
    int valid_stocks = 0;
    
    for (int i = 0; i < count; i++) {
        if (stocks[i].current_price > 0) {
            total_change += stocks[i].change_percent;
            valid_stocks++;
        }
    }
    
    return (valid_stocks > 0) ? total_change / valid_stocks : 0.0;
}

// Generate market summary
void generate_market_summary(Stock stocks[], int count, char* summary, size_t size) {
    if (!stocks || !summary || count <= 0) {
        return;
    }
    
    int bullish_count = count_bullish_stocks(stocks, count);
    double avg_change = calculate_average_change(stocks, count);
    Stock* best_stock = find_best_performing_stock(stocks, count);
    Stock* most_volatile = find_most_volatile_stock(stocks, count);
    const char* sentiment = analyze_market_sentiment(stocks, count);
    
    snprintf(summary, size,
        "📊 MARKET SUMMARY\n"
        "═══════════════════\n"
        "• Bullish Stocks: %d/%d (%.1f%%)\n"
        "• Average Change: %.2f%%\n"
        "• Market Sentiment: %s\n"
        "• Best Performer: %s (%.2f%%)\n"
        "• Most Volatile: %s (%.2f%%)\n",
        bullish_count, count, (bullish_count * 100.0) / count,
        avg_change,
        sentiment,
        best_stock ? best_stock->symbol : "N/A",
        best_stock ? best_stock->change_percent : 0.0,
        most_volatile ? most_volatile->symbol : "N/A",
        most_volatile ? most_volatile->change_percent : 0.0
    );
}
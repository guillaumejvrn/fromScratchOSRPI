void uart_init(); 
void uart_puts(const char* s); 

void main() {     
    // Initialize the PL011 UART peripheral     
    uart_init();     
    
    // Print the custom system boot banner     
    uart_puts("\n\n");     
    uart_puts("======================================\n");     
    uart_puts("   Welcome to PepitOS (RPi 4)     \n");     
    uart_puts("   Pure ARM64 Bare-Metal Booting      \n");     
    uart_puts("======================================\n\n");     
    
    // Keep the CPU execution alive indefinitely     
    while(1) {         
        // Infinite loop to prevent CPU from running off into random memory     
    } 
} 
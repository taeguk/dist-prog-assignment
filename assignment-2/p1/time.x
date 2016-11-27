program TIME_PROG {
    version TIME_VERS {
        int get_time(void) = 1;
        int delay(int) = 2;
    } = 1;
} = 0x31111111;
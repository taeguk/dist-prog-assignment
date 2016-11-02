int collapse(int num)
{
    int res = 0;
    while (num) {
        res += num % 10;
        num /= 10;
    }
    return res;
}

int ultimate_collapse(int num)
{
    while (num / 10 > 0)
        num = collapse(num);
    return num;
}
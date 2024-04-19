#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Token.h"
#include <vector>
#pragma once



class Helper
{
private:
    /* data */
public:
    bool is_digit(char c) noexcept;
    bool is_identifier(char c) noexcept;
};
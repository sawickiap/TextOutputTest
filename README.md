# TextOutputTest

A simple "Test" application that prints messages to the output using various methods:

- Standard output, the C way: `printf` / `wprintf`
- Standard output, the C++ way: `std::cout`, `std::wcout`
- WinAPI console: `WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), …`
- Standard error: `fprintf(sderr, …` / `fwprintf(stderr, …`
- Debugger message: `OutputDebugString`

Plus a "Host" application that captures the output of a subprocess spawned using `CreateProcess` sent to standard output, standard error, and `OutputDebugString`.

Visual Studio 2022 project using C++20 language.

This code accompanies an article on my blog: **["Ways to Print and Capture Text Output of a Process" @ asawicki.info](https://asawicki.info/news_1768_ways_to_print_and_capture_text_output_of_a_process)**

Published in July 2023 
License: Public Domain

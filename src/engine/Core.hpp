#pragma once

// Определяем макросы для экспорта/импорта функций
#if defined(_MSC_VER)
    #if defined(SIMCOLONY_EXPORT)
        #define SIMCOLONY_API __declspec(dllexport)
    #else
        #define SIMCOLONY_API __declspec(dllimport)
    #endif
#else
    #define SIMCOLONY_API
#endif
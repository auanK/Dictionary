#pragma once
#include <unicode/coll.h>
#include <unicode/unistr.h>

// Functor padrão para comparar tipos genéricos onde suportam operador '<'
template <typename type>
struct default_compare {
    bool operator()(const type &lhs, const type &rhs) const {
        return lhs < rhs;
    }
};

// Functor para comparar strings Unicode usando ICU Collator
struct unicode_compare {
    icu::Collator *collator;

    unicode_compare() {
        UErrorCode status = U_ZERO_ERROR;
        collator = icu::Collator::createInstance(status);
    }

    bool operator()(const icu::UnicodeString &lhs,
                    const icu::UnicodeString &rhs) const {
        UErrorCode status = U_ZERO_ERROR;
        return collator->compare(lhs, rhs, status) < 0;
    }
};

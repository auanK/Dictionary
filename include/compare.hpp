#pragma once
#include <unicode/coll.h>
#include <unicode/unistr.h>

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

    ~unicode_compare() { delete collator; }
};

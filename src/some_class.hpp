#pragma once

namespace jam
{
    class SomeClass
    {
    private:


    public:
        SomeClass();
        inline ~SomeClass() {}

        SomeClass(const SomeClass&) = delete;
        SomeClass(SomeClass&&) = delete;
    };
}
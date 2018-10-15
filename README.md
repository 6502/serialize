# serialize

C++ objects serialization

A tiny serialization library for storing C++ types to and retrieve them from  byte arrays.

Support for bool, char, short, int, long long (signed/unsigned), strings, vectors, maps
and user defined classes with automatic code generation (for simple cases).

Example:

    struct P2d {
        double x, y;
    };

    DefSerializer(P2d, x, y)

    ...

    std::vector<P2d> pv{{12.3, 45.6}, {-1.1, -2.2}};
    std::vector<unsigned char> buf = Serialize::serialize(pv);

    ...

    auto pv2 = Serialize::deserialize<P2d>(buf);

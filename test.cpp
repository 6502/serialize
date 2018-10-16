#include "serialize.h"

struct P2d {
    double x, y;
};

DefSerializer(P2d, x, y)

typedef std::vector<P2d> Path2d;

struct Area2d {
    std::vector<Path2d> paths;
    std::string color;
    bool valid;
};

DefSerializer(Area2d, paths, color, valid)

/////////////////////////////////////////////////////////////

struct Base {
    int x = -1;
    Base(){}
    Base(int x) : x(x) {}
    virtual ~Base(){}
};

DefSerializer(Base, x)

struct Der1 : Base {
    int y = -1;
    Der1(){}
    Der1(int x, int y) : Base{x}, y(y) {}
};

DefSerializer(Der1, y)

struct Der2 : Base {
    std::string y;
    Der2(){}
    Der2(int x, const std::string& y) : Base{x}, y(y) {}
};

DefSerializer(Der2, y)

template<>
struct Serialize::Serializer<Base*> {
    static void save(WriteChannel& write, const Base * x) {
        if (auto p = dynamic_cast<const Der1 *>(x)) {
            write("1", 1);
            Serialize::save(write, *x);
            Serialize::save(write, *p);
        } else if (auto p = dynamic_cast<const Der2 *>(x)) {
            write("2", 1);
            Serialize::save(write, *x);
            Serialize::save(write, *p);
        } else {
            throw std::runtime_error("Unknown derived class");
        }
    }
    static void load(ReadChannel& read, Base*& x) {
        char t; read(t);
        if (t == '1') {
            x = new Der1; Serialize::load(read, *x); Serialize::load(read, *(Der1 *)x);
        } else if (t == '2') {
            x = new Der2; Serialize::load(read, *x); Serialize::load(read, *(Der2 *)x);
        } else {
            throw std::runtime_error("Data stream error");
        }
    }
    static std::string type() {
        return "*|" + Serializer<Base>::type() + Serializer<Der1>::type() + Serializer<Der2>::type() + "|";
    }
};

int main(int argc, const char *argv[]) {
    std::map<int, Area2d> areas;
    for (int j=0; j<100; j++) {
        Area2d& a = areas[j*3];
        a.color = "#AABBCC";
        a.valid = j % 4 == 0;
        for (int i = 0; i<2; i++) {
            a.paths.push_back({});
            for (int j=0; j<1000; j++) {
                a.paths.back().push_back({i/10.0, j/10.0});
            }
        }
    }

    clock_t start = clock();
    std::vector<unsigned char> buf = Serialize::serialize(areas);
    clock_t stop = clock();
    printf("Serialization time = %.3fms (%i bytes)\n",
           1000.0*(stop - start)/CLOCKS_PER_SEC,
           int(buf.size()));
    FILE *f = fopen("test.dat", "wb");
    if (f) {
        fwrite(&buf[0], 1, buf.size(), f);
        fclose(f);
    } else {
        fprintf(stderr, "Error creating binary dump... skipping\n");
    }
    clock_t start2 = clock();
    auto res = Serialize::deserialize<std::map<int, Area2d>>(buf);
    clock_t stop2 = clock();
    printf("Deserialization time = %.3fms\n",
           1000.0*(stop2 - start2)/CLOCKS_PER_SEC);

    printf("Data descriptor: %s\n", Serialize::Serializer<std::vector<Area2d>>::type().c_str());

    std::vector<Base *> bpv;
    bpv.push_back(new Der1(10, 99));
    bpv.push_back(new Der2(20, "Hey"));

    auto buf2 = Serialize::serialize(bpv);

    auto bpv2 = Serialize::deserialize<std::vector<Base *>>(buf2);
    for (auto& bp : bpv2) {
        if (Der1 *p = dynamic_cast<Der1 *>(bp)) {
            printf("Der1(%i, %i)\n", p->x, p->y);
        } else if (Der2 *p = dynamic_cast<Der2 *>(bp)) {
            printf("Der2(%i, \"%s\")\n", p->x, p->y.c_str());
        } else {
            printf("Unknown class\n");
        }
    }
    
    return 0;
}

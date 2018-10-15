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
    return 0;
}

{
    files = {
        "src/headline.c"
    },
    depfiles_gcc = "build/.objs/console/macosx/x86_64/release/src/headline.c.o:   src/headline.c src/./headline.h\
",
    values = {
        "/usr/bin/xcrun -sdk macosx clang",
        {
            "-Qunused-arguments",
            "-arch",
            "x86_64",
            "-fvisibility=hidden",
            "-O3",
            "-DNDEBUG"
        }
    }
}
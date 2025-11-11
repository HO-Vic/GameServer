
#define STRINGIFY(x) #x
#define CON_CAT(x, y) x##y
#define BUILD_MESSAGE(file, line, msg) __pragma(message( \
    file##"("##STRINGIFY(line)##")> "##msg))

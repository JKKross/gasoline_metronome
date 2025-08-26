if [ ! -d "./build/debug/mac_os" ]
then
	mkdir ./build/debug/mac_os
fi

rm -rf ./build/debug/mac_os/*

# @TODO: This is most likely broken (I don't own a mac anymore, so I can't try)
#
clang -std=c99 --debug -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL ./src/lib/raylib/raylib-5.5_macos/lib/libraylib.a ./src/gasoline_metronome.c -o ./build/debug/mac_os/gasoline_metronome

# If there's a NO_COMMIT tag anywhere in the code, we want to know
rg "@NO_COMMIT" --ignore-case -g "!win_debug_build.bat"

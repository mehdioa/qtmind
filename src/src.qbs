import qbs

Product {
    type: "application"
    name: qbs.targetOS.contains("osx") ? "Qt Mind" : "qtmind"
    version: "0.7.6"
    files:["*.cpp", "*.h", "*.ui", "../resource.qrc"]

    cpp.cxxFlags:{
            var flags = base
            if(cpp.compilerName.contains("g++"))
                flags = flags.concat(["-std=gnu++11"])
            return flags
        }
//    cpp.cxxFlags: ["-std=c++0x"]
    cpp.linkerFlags: {
        if (qbs.buildVariant == "release" && (qbs.toolchain.contains("gcc") || qbs.toolchain.contains("mingw")))
            return ["-Wl,-s"]
    }

    Depends { name: "cpp"}

    Depends{name:"Qt"; submodules:["widgets", "multimedia"]}

    Group {
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        qbs.install: true
        qbs.installDir: project.app_path
        fileTagsFilter: product.type
    }

    Group {
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        qbs.install: true
        qbs.installDir: "share/qtmind/translations"
        files: ["../translations/*.qm"]
    }

    Group {
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        qbs.install: true
        qbs.installDir: "share/applications"
        files: ["../qtmind.desktop"]
    }

    Group {
        condition: qbs.targetOS.contains("unix") && !qbs.targetOS.contains("osx")
        qbs.install: true
        qbs.installDir: "share/share/pixmaps"
		files: ["../resources/icons/hicolor/128x128/qtmind.png"]
    }

}


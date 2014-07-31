import qbs
Project {
	property string project_version_major: '0'
	property string project_version_minor: '7'
	property string project_version_release: '7'
	property string project_version: project_version_major + '.' + project_version_minor + '.' + project_version_release
	property string project_app_path: qbs.targetOS.contains("osx") ? "" : "bin"
	property string project_app_target: qbs.targetOS.contains("osx") ? "Qt Mind" : "qtmind"

//	if (qbs.targetOS.contains("osx")){
//		property string project_translations_path: project_app_target + ".app/Contents/Resources/translations"
//		property string project_desktop_path: project_app_target + ".app/Contents/Resources"
//		property string project_desktop_path: project_app_target + ".app/Contents/Resources"
//	} else if (qbs.targetOS.contains("windows")) {

//	}

//	property string project_translations_path: {
//		if (qbs.targetOS.contains("osx"))
//			return ide_app_target + ".app/Contents/Resources"
//		else if (qbs.targetOS.contains("windows"))
//			return project_app_path
//		else
//			return "share/qtmind/translations"
//	}
//	property string project_desktop_path: {
//		if (qbs.targetOS.contains("osx"))
//		return ide_app_target + ".app/Contents/Resources"
//		else if (qbs.targetOS.contains("windows"))
//		return project_app_path
//		else
//		return "share/qtmind/translations"
//	}

	references: [
		"src/src.qbs"
	]
}

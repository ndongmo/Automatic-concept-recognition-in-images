QT += core
QT += widgets

SOURCES += \
    main.cpp \
    interfaceui.cpp \
    bagOfWords.cpp \
    classifier.cpp \
    resultDialog.cpp \
    processing.cpp

HEADERS += \
    interfaceui.h \
    classifier.h \
    resultDialog.h \
    bagOfWords.h \
    processing.h

FORMS += \
    interfaceui.ui


INCLUDEPATH += $$(C_INCLUDES)

win32:CONFIG(release, debug|release): {
    LIBS += -L$$(C_LIBS)/RELEASE \
    -lopencv_core2411 \
    -lopencv_highgui2411 \
    -lopencv_imgproc2411 \
    -lopencv_features2d2411 \
    -lopencv_calib3d2411 \
    -lopencv_nonfree2411 \
    -lopencv_ml2411 \
    -lopencv_flann2411
}
else:win32:CONFIG(debug, debug|release): {
    LIBS += -L$$(C_LIBS)/DEBUG \
    -lopencv_core2411d \
    -lopencv_highgui2411d \
    -lopencv_imgproc2411d \
    -lopencv_features2d2411d \
    -lopencv_calib3d2411d \
    -lopencv_nonfree2411d \
    -lopencv_ml2411d \
    -lopencv_flann2411d
}

RESOURCES += \
    ressources.qrc

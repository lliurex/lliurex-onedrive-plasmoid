
#######################################################################################
# Package
plasma_install_package(package org.kde.plasma.lliurexonedrive)


#######################################################################################
# Notes Library
add_definitions(-DTRANSLATION_DOMAIN="plasma_applet_org.kde.plasma.lliurexonedrive")

set(llxonedrivewidget_SRCS
    plugin/plugin.cpp
    plugin/LliurexOneDriveWidget.cpp
    plugin/LliurexOneDriveWidgetSpaceItem.cpp
    plugin/LliurexOneDriveWidgetSpacesModel.cpp
    plugin/LliurexOneDriveWidgetModel.cpp
    plugin/LliurexOneDriveWidgetItem.cpp
    plugin/LliurexOneDriveWidgetUtils.cpp
   
)

add_library(lliurexonedriveplugin SHARED ${llxonedrivewidget_SRCS})

target_link_libraries(lliurexonedriveplugin
                      Qt5::Quick
                      Qt5::Core
                      Qt5::Widgets
                      KF5::CoreAddons
                      KF5::I18n
                      KF5::Notifications
                      KF5::KIOWidgets
                      )

install(FILES plugin/qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/lliurexonedrive)
install(TARGETS lliurexonedriveplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/lliurexonedrive)

install(FILES "icons/onedrive-running.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/icons/hicolor/scalable/apps/)
install(FILES "icons/onedrive-pause.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/icons/hicolor/scalable/apps/)
install(FILES "icons/onedrive-waiting.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/icons/hicolor/scalable/apps/)
install(FILES "icons/onedrive-error.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/icons/hicolor/scalable/apps/)
install(FILES "icons/onedrive-starting.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/icons/hicolor/scalable/apps/)
install(FILES "icons/onedrive-widget.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/icons/hicolor/scalable/apps/)
install(FILES "knotifications5/llxonedrive.notifyrc"
        DESTINATION ${DATAROOTDIR}/usr/share/knotifications5/)        

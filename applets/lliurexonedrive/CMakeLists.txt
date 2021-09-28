
#######################################################################################
# Package
plasma_install_package(package org.kde.plasma.lliurexonedrive)


#######################################################################################
# Notes Library
add_definitions(-DTRANSLATION_DOMAIN="plasma_applet_org.kde.plasma.lliurexonedrive")

set(upnotifier_SRCS
    plugin/plugin.cpp
    plugin/LliurexOneDriveWidget.cpp
    plugin/LliurexOneDriveWidgetUtils.cpp
   
)

add_library(lliurexonedriveplugin SHARED ${upnotifier_SRCS})

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

install(FILES "icons/onedrive.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/plasma/desktoptheme/default/icons/)
install(FILES "knotifications5/llxonedrive.notifyrc"
        DESTINATION ${DATAROOTDIR}/usr/share/knotifications5/)        


#######################################################################################
# Package
plasma_install_package(package org.kde.plasma.lliurexonedrive)


#######################################################################################
# Notes Library
add_definitions(-DTRANSLATION_DOMAIN="plasma_applet_org.kde.plasma.lliurexonedrive")
ecm_add_qml_module(lliurexonedriveplugin URI org.kde.plasma.private.lliurexonedrive)

target_sources(
lliurexonedriveplugin PRIVATE
    plugin/plugin.cpp
    plugin/LliurexOneDriveWidget.cpp
    plugin/LliurexOneDriveWidgetSpaceItem.cpp
    plugin/LliurexOneDriveWidgetSpacesModel.cpp
    plugin/LliurexOneDriveWidgetFilesModel.cpp
    plugin/LliurexOneDriveWidgetFileItem.cpp
    plugin/LliurexOneDriveWidgetUtils.cpp
   
)

target_link_libraries(lliurexonedriveplugin PRIVATE
                      Qt6::Quick
                      Qt6::Core
                      Qt6::Widgets
                      KF6::CoreAddons
                      KF6::I18n
                      KF6::Notifications
                      KF6::KIOWidgets
                      )

ecm_finalize_qml_module(lliurexonedriveplugin)

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
install(FILES "knotifications6/llxonedrive.notifyrc"
        DESTINATION ${DATAROOTDIR}/usr/share/knotifications6/)        

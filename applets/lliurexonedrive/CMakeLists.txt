
#######################################################################################
# Package
plasma_install_package(package org.kde.plasma.lliurexonedrive)


#######################################################################################
# Notes Library
set(PLUGIN_TARGET "lliurexonedriveplugin")
add_definitions(-DTRANSLATION_DOMAIN="plasma_applet_org.kde.plasma.lliurexonedrive")
ecm_add_qml_module(${PLUGIN_TARGET} URI org.kde.plasma.private.lliurexonedrive VERSION 2.0)

target_sources(
${PLUGIN_TARGET} PRIVATE
    plugin/plugin.cpp
    plugin/LliurexOneDriveWidget.cpp
    plugin/LliurexOneDriveWidgetSpaceItem.cpp
    plugin/LliurexOneDriveWidgetSpacesModel.cpp
    plugin/LliurexOneDriveWidgetFilesModel.cpp
    plugin/LliurexOneDriveWidgetFileItem.cpp
    plugin/LliurexOneDriveWidgetUtils.cpp
    plugin/plugin.h
    plugin/LliurexOneDriveWidget.h
    plugin/LliurexOneDriveWidgetSpaceItem.h
    plugin/LliurexOneDriveWidgetSpacesModel.h
    plugin/LliurexOneDriveWidgetFilesModel.h
    plugin/LliurexOneDriveWidgetFileItem.h
    plugin/LliurexOneDriveWidgetUtils.h
   
)

target_link_libraries(lliurexonedriveplugin PRIVATE
                      Qt6::Quick
                      Qt6::Core
                      Qt6::Widgets
                      Qt6::Concurrent
                      KF6::CoreAddons
                      KF6::I18n
                      KF6::Notifications
                      KF6::KIOWidgets
                      )

target_include_directories(${PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/plugin)  
ecm_finalize_qml_module(${PLUGIN_TARGET})

install(TARGETS ${PLUGIN_TARGET} DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/lliurexonedrive)

install(FILES "icons/onedrive-running.svg"
        DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps/)
install(FILES "icons/onedrive-pause.svg"
        DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps/)
install(FILES "icons/onedrive-waiting.svg"
        DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps/)
install(FILES "icons/onedrive-error.svg"
        DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps/)
install(FILES "icons/onedrive-starting.svg"
        DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps/)
install(FILES "icons/onedrive-widget.svg"
        DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps/)
install(FILES "knotifications6/llxonedrive.notifyrc"
       DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})

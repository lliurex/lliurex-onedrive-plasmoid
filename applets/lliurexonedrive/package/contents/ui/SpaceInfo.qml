import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.components 3.0 as PC3

Rectangle{
	color:"transparent"

    GridLayout{
	    id: optionsAccount
	    rows: 8
	    flow: GridLayout.TopToBottom
	    rowSpacing:12
	    width:parent.width

	    RowLayout{
	        id:head
	        Layout.fillWidth:true
	        PC3.ToolButton {
	            height:35
	            width:355
	            Layout.rightMargin:optionsAccount.width/3-35-headText.text.length
	            icon.name:"arrow-left"
	            PC3.ToolTip{
	            	id:backTP
	                text:i18n("Back to main view")
	            }
	            onClicked:{
	            	backTP.hide()
	            	lliurexOneDriveWidget.manageNavigation(0)
	            }
	        } 

	        PC3.Label{
	            id:headText
	            text:i18n("Space details")
	            font.italic:true
	            font.pointSize:11
	            Layout.fillWidth:true
	            Layout.alignment:Qt.AlignHCenter
	        }
	        PC3.ToolButton {
	            id:configureBtn
	            height:35
	            width:35
	            Layout.rightMargin:5
	            icon.name:"configure"
	            PC3.ToolTip{
	            	id:configureTP
	                text:i18n("Manage space")
	            }
	            onClicked:{
	            	configureTP.hide()
	            	lliurexOneDriveWidget.launchOneDrive(true)
	            }
	        } 
	    } 
     	RowLayout{
	        id:spaceMailRow
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        Layout.bottomMargin:5

	        PC3.Label{
	            id:spaceMail
	            text:i18n("Email associated:")
	            Layout.rightMargin:5
	        }
	        PC3.Label{
	            id:spaceMailVa5lue
	            text:lliurexOneDriveWidget.spaceMail
	            Layout.fillWidth:true
	        }
	    }
	    RowLayout{
	        id:spaceTypeRow
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5

	        PC3.Label{
	            id:spaceType
	            text:i18n("Type:")
	            Layout.rightMargin:5

	        }
	        PC3.Label{
	            id:spaceTypeValue
	            text:{
	            	switch (lliurexOneDriveWidget.spaceType){
	            		case "onedrive":
	            			var msg=i18n("OneDrive")
	            			break;
	            		case "onedriveBackup":
	            			var msg=i18n("Onedrive-Backup")
	            			break;
	            		case "sharepoint":
	            			var msg="SharePoint"
	            			break;
	            	}
	            	return msg
	            }
	            Layout.fillWidth:true
	        }
	    }
	    RowLayout {
	        id:folderRow
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        Layout.fillWidth:true

	        PC3.Label{
	            id:oneDriveFolder
	            text:i18n("Local folder:")
	            Layout.rightMargin:5
	        }
	        PC3.Label{
	            id:oneDriveFolderValue
	            text:lliurexOneDriveWidget.oneDriveFolder
	            Layout.fillWidth:true
	            elide:Text.ElideMiddle
	        }
	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:"document-open-folder"
	            PC3.ToolTip{
	            	id:folderTP
	                text:i18n("Click to open folder")
	            }
	            onClicked:{
	            	folderTP.hide()
	            	lliurexOneDriveWidget.openFolder()
	            }
	        } 
	    }

	    RowLayout{
	        id:freeSpaceRow
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5

	        PC3.Label{
	            id:freeSpace
	            text:i18n("Free Space:")
	            Layout.rightMargin:5
	        }
	        PC3.Label{
	            id:freeSpaceValue
	            text:{
	                if (lliurexOneDriveWidget.freeSpace!=""){
	                    lliurexOneDriveWidget.freeSpace
	                }else{
	                    i18n("Information not available")
	                }
	            }
	            Layout.fillWidth:true
	        }
	    }

	    RowLayout{
	        id:syncRow
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5

	        PC3.Label{
	            id:syncText
	            text:i18n("Synchronization:")
	            Layout.rightMargin:5
	        }

	        PC3.Label{
	            id:syncStatus
	            text:{
	                if (lliurexOneDriveWidget.syncStatus){
	                    if(lliurexOneDriveWidget.clickedSyncBtn){
	                        i18n("Stopping...")
	                    }else{
	                        i18n("Running")
	                    }
	                }else{
	                    if(lliurexOneDriveWidget.clickedSyncBtn){
	                        i18n("Starting...")
	                    }else{
	                        i18n("Stopped")
	                    }

	                }
	            }

	            Layout.fillWidth:true
	        }

	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:lliurexOneDriveWidget.syncStatus?"kt-stop":"kt-start"
	            enabled:!lliurexOneDriveWidget.lliurexOneDriveOpen && !lliurexOneDriveWidget.clickedSyncBtn
	            PC3.ToolTip{
	            	id:syncTP
	                text:lliurexOneDriveWidget.syncStatus?i18n("Click to stop synchronization"):i18n("Click to start synchronization")
	            }
	            onClicked:{
	            	syncTP.hide()
	            	lliurexOneDriveWidget.manageSync()
	            } 
	        } 
	    } 
	    RowLayout{
	        id:latestFiles
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        visible:lliurexOneDriveWidget.spaceType=="onedriveBackup"?false:true

	        PC3.Label{
	            id:latestText
	            text:i18n("Recently modified files")
	            Layout.fillWidth:true
	        }
	       
	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:"arrow-right"
	            PC3.ToolTip{
	            	id:filesTP
	                text:i18n("Click to see the list")
	            }
	           	onClicked:{
	           		filesTP.hide()
	                lliurexOneDriveWidget.getLatestFiles()
	                lliurexOneDriveWidget.manageNavigation(2)
	            }

	        } 
	    }

	     RowLayout{
	        id:latestUploadedFiles
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        visible:lliurexOneDriveWidget.spaceType=="onedriveBackup"?true:false

	        PC3.Label{
	            id:latestUploadedText
	            text:i18n("Recently uploaded files")
	            Layout.fillWidth:true
	        }
	       
	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:"arrow-right"
	            PC3.ToolTip{
	            	id:uploadedFilesTP
	                text:i18n("Click to see the list")
	            }
	           	onClicked:{
	           		uploadedFilesTP.hide()
	                lliurexOneDriveWidget.getLatestUploadedFiles()
	                lliurexOneDriveWidget.manageNavigation(3)
	            }

	        } 
	    }

	    RowLayout{
	        id: logFile
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        visible:{
	        	if (lliurexOneDriveWidget.logFileSize!=""){
	        		true
	        	}else{
	        		false
	        	}
	        }

	        PC3.Label{
	            id:logSizeT
	            text:i18n("Log file size:")
	            Layout.rightMargin:5
	            
	        }

	        PC3.Label{
	            id:logSizeValue
	            text:lliurexOneDriveWidget.logFileSize
	            Layout.fillWidth:true
	        }
	       
	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:"document-open-file"
	            PC3.ToolTip{
	            	id:logTP
	                text:i18n("Click to see log file")
	            }
	            onClicked:{
	            	logTP.hide()
	                lliurexOneDriveWidget.openLogFile()
	            }

	        } 
	    }
	}
      
} 

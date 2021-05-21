import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import org.freedesktop.gstreamer.GLVideoItem 1.0
import Player 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            id: fileMenu
            title: qsTr("&File")

            MenuItem {
                action: fileOpenAction
            }
        }
    }

    header: ToolBar {
        Flow {
            anchors.fill: parent
            ToolButton {
                text: qsTr("Open")
                icon.name: "document-open"
                onClicked: fileOpenDialog.open()
            }
        }
    }

    FileDialog {
        id: fileOpenDialog
        title: qsTr("Select an video file")
        folder: shortcuts.movies
        nameFilters: {
            "Video files (*.mp4 *.avi *.wmv)"
        }

        onAccepted: player.uri = fileUrl;
    }

    Action {
        id: fileOpenAction
        text: "&Open"
        onTriggered: fileOpenDialog.open()
    }


    Item {
        anchors.fill: parent

        Player {
            id: player
            output: videoOuput
        }

        GstGLVideoItem {
            id: videoOuput
            objectName: "videoItem"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
        }
    }
}

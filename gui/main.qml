import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 800
    height: 800
    color: "#9862BD"
    title: "Cube timer"

    property string currentCube: "3x3"

    Text {
        id: scramble

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 90

        text: "R U R' U' L D' F' B U"
        font.pixelSize: 24
    }

    // Temporal timer, sync on C++
    Item {
        id: timerItem
        width: 300
        height: 200
        focus: true

        anchors.centerIn: parent

        property real elapsedSeconds: 0
        property bool is_running: true

        function formatTime(seconds){
            var mins = Math.floor(seconds / 60)
            var secs = Math.floor(seconds % 60)
            var millis = Math.floor((seconds - Math.floor(seconds)) * 100)
            var pad = function(n){ return (n < 10 ? "0" : "") + n; }

            return pad(mins) + ":" + pad(secs) + "." + pad(millis)
        }

        Timer {
            interval: 10
            running: parent.is_running
            repeat: true
            onTriggered: {
                parent.elapsedSeconds += 0.01
            }
        }

        Keys.onPressed: function(event){
            if(event.key === Qt.Key_Space){
                is_running = !is_running
                event.accepted = true
            }
        }

        Text {
            anchors.centerIn: parent
            text: parent.formatTime(parent.elapsedSeconds)
            font.pixelSize: 48
            font.bold: true
            color: parent.is_running ? "white" : "gray"

            style: Text.Outline
            styleColor: "black"
        }
    }

    // --- Menu to select the cube (starts here)---
    Rectangle {
        id: cubes
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        width: 160
        height: 30
        radius: 17
        anchors.topMargin: 20
        color: "#4F69FF"

        Text {
            anchors.centerIn: parent
            text: currentCube //sync on C++
            color: "black"
            font.pixelSize: 16
        }

        MouseArea {
            anchors.fill: parent
            onClicked: menu_cubes.open()
        }
    }

    Popup {
        id: menu_cubes
        anchors.centerIn: Overlay.overlay
        width: 200
        padding: 8
        modal: true // atenua el fondo
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        onClosed: timerItem.forceActiveFocus()

        background: Rectangle {
            radius: 17
            color: "#4F69FF"
            border.color: "#121212"
        }

        contentItem: Column {
            spacing: 4

            Repeater {
                model: ["2x2", "3x3", "4x4"]

                delegate: Rectangle {
                    width: parent.width
                    height: 32
                    radius: 8
                    color: mouse_area.containsMouse ? "#A3B5FF" : "transparent"

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        x: 12
                        text: modelData
                        color: "black"
                    }

                    MouseArea {
                        id: mouse_area
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: { 
                            currentCube = modelData
                            menu_cubes.close()
                        }
                    }
                }
            }
        }

    }
    // --- Menu to select the cube (ends here)---
}
